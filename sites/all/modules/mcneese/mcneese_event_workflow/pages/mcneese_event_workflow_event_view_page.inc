<?php

/**
 * @file
 * Defines McNeese event workflow event view page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event view page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $event_id
 *   The unique identifier for an event request.
 * @param int $mode
 *   Specifies the mode that alters how the form is rendered.
 *   0: No restrictions.
 *   1: Render only the display part.
 *   2: Render only the log part.
 *   3: Render only the operations part.
 * @param bool $pdf
 *   (optional) Enable special handling for PDF generation.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_view_0_page($form, &$form_state, $event_id, $mode = 0, $pdf = FALSE) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_numeric($mode) || $mode < 0 || $mode > 3) {
    $mode = 0;
  }
  else {
    $mode = (int) $mode;
  }

  if (!is_bool($pdf)) {
    $pdf = FALSE;
  }

  // if cancelling the event, return only the cancellation confirmation form.
  if (isset($form_state['cancel_was_clicked'])) {
    mcneese_event_workflow_include(5);

    $cancel_form = array();
    $cancel_form_state = array();
    return mcneese_event_workflow_event_cancel_0_form($cancel_form, $cancel_form_state, $event_id);
  }
  // if uncancelling the event, return only the uncancellation confirmation form.
  elseif (isset($form_state['uncancel_was_clicked'])) {
    mcneese_event_workflow_include(5);

    $cancel_form = array();
    $cancel_form_state = array();
    return mcneese_event_workflow_event_uncancel_0_form($cancel_form, $cancel_form_state, $event_id);
  }

  $user = cf_current_user();

  $administer = user_access('mcneese event workflow administer');
  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $can_log = mcneese_event_workflow_management_page_event_access('log');
  $can_review = mcneese_event_workflow_management_page_event_access('review', $event_id);

  $show_display = TRUE;
  $show_log = TRUE;
  $show_operations = TRUE;
  if ($mode == 1) {
    $show_log = FALSE;
    $show_operations = FALSE;
  }
  elseif ($mode == 2) {
    $show_display = FALSE;
    $show_operations = FALSE;
  }
  elseif ($mode == 3) {
    $show_display = FALSE;
    $show_log = FALSE;
  }

  if (!$can_log) {
    $show_log = FALSE;

    if ($mode == 2) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }
  }

  if (!$can_review) {

  }

  if ($can_review) {
    // show review operations only if the content is reviewable by the current user.
    $event_classification = $event['coordinator']['classification'][0]->value;
    $step = $event['top']['step'][0]->value;
    $status = $event['top']['status'][0]->value;

    $reviewers = mcneese_event_workflow_get_reviewers(array('mr.user_id' => $user->uid, 'mr.event_classification' => $event_classification, 'mr.review_step' => $step));

    if (empty($reviewers)) {
      $show_operations = FALSE;

      if ($mode == 3 && !$administer) {
        drupal_access_denied();
        drupal_exit();
        return array();
      }
    }
    else {
      $reviewer = array_pop($reviewers);
    }

    // only show operations for states and steps that make sense to.
    $allowed_statuses = array(
      MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED,
      MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED,
    );

    if (!in_array($status, $allowed_statuses)) {
      $show_operations = FALSE;

      if ($mode == 3) {
        drupal_access_denied();
        drupal_exit();
        return array();
      }
    }

    $allowed_steps = array(
      116,
      132,
      133,
      117,
    );

    if (!in_array($step, $allowed_steps)) {
      $show_operations = FALSE;

      if ($mode == 3) {
        drupal_access_denied();
        drupal_exit();
        return array();
      }
    }
  }
  else {
    $show_operations = FALSE;

    if ($mode == 3) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }
  }

  if ($pdf) {
    $can_edit = FALSE;
  }
  else {
    $can_edit = (bool) mcneese_event_workflow_management_page_event_access('edit', $event_id);
  }

  if ($can_edit) {
    // Include the CTools tools that we need.
    #ctools_include('ajax');
    #ctools_include('modal');

    // Add CTools' javascript to the page.
    #ctools_modal_add_js();
  }

  $page_title = "Event Request";
  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Event Request: " . $event['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  // provide margin information in the printer-friendly version of the page.
  $print_css = '@page { ' . "\n";
  $print_css .= '  size: A4 portrait;' . "\n";
  $print_css .= '  margin: 30px 30px 30px 30px;' . "\n";

  // note: @top-left, and @top-right are currently not supported by most major browsers.
  $print_css .= '  @top-left { content: "' . $page_title . '"; }' . "\n";
  $print_css .= '  @top-right { content: "Page " counter(page); }' . "\n";
  $print_css .= '}' . "\n";
  drupal_add_css($print_css, array('type' => 'inline', 'group' => CSS_THEME, 'weight' => 10, 'media' => 'print', 'preprocess' => FALSE));


  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['event_id'] = array(
    '#type' => 'value',
    '#value' => $event_id,
  );

  $form['form']['event'] = array(
    '#type' => 'value',
    '#value' => &$event,
  );

  $form['form']['mode'] = array(
    '#type' => 'value',
    '#value' => $mode,
  );

  $form['form']['pdf'] = array(
    '#type' => 'value',
    '#value' => $pdf,
  );

  $form['form']['reviewer'] = array(
    '#type' => 'value',
    '#value' => $reviewer,
  );

  if (!isset($event['information']['type'][0]->value) || !cf_is_integer($event['information']['type'][0]->value)) {
    cf_error::invalid_integer('event[information][type][0]->value');
    $form['message'] = array(
      '#markup' => "Error: Unable to load event (ID = " . check_plain($event_id) . ") due to missing event type value.",
    );

    return $form;
  }

  $event_type = $event['information']['type'][0]->value;

  if ($show_display) {
    $form['display'] = array(
      '#id' => 'event-' . $event_id . '-section-display',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Request Details",
      '#attributes' => array(
        'class' => array(
          'event-section-display',
          'mode-' . $mode,
        ),
      ),
      '#tree' => TRUE,
    );

    if ($pdf) {
      unset($form['display']['#type']);
      unset($form['display']['#collapsible']);
      unset($form['display']['#collapsed']);

      $form['display']['log']['#type'] = 'container';
      $form['display']['log']['#attributes']['id'] = 'event-' . $event_id . '-section-display';
    }

    $form['display']['groups'] = array(
      '#prefix' => '<div id="event-' . $event_id . '-field_groups" class="event-field_groups mode-' . $mode . '">',
      '#suffix' => '</div>',
    );

    $form['groups'] = array();
    $form['groups'] = array(
      '#tree' => TRUE,
    );

    $fields_count = array('top' => 0, 'group' => array());

    mcneese_event_workflow_include(0);

    $presentation = mcneese_event_workflow_secondary_table_presentation();
    $presentation_tables = mcneese_event_workflow_secondary_table_presentation(1);

    $collate = array();

    foreach ($presentation_tables as $field_group => $presentation_table) {
      mcneese_event_workflow_event_view_0_page_setup_group_path($form, $event_id, $field_group, $presentation_tables, $can_edit, $event);
    }

    foreach ($event as $field_group => $event_array) {
      if (isset($presentation_tables[$field_group]['event_type']) && is_array($presentation_tables[$field_group]['event_type']) && array_key_exists('hide', $presentation_tables[$field_group]['event_type'])) {
        if ($presentation_tables[$field_group]['event_type']['hide'] === TRUE) {
          if (array_key_exists('show', $presentation_tables[$field_group]['event_type'])) {
            if ($presentation_tables[$field_group]['event_type']['show'] === TRUE) {
              // do nothing
            }
            elseif ($presentation_tables[$field_group]['event_type']['show'] === FALSE) {
              continue;
            }
            elseif (is_array($presentation_tables[$field_group]['event_type']['show']) && !in_array($event_type, $presentation_tables[$field_group]['event_type']['show'])) {
              continue;
            }
          }
        }
        elseif ($presentation_tables[$field_group]['event_type']['hide'] === FALSE) {
          // do nothing
        }
        elseif (is_array($presentation_tables[$field_group]['event_type']['hide']) && in_array($event_type, $presentation_tables[$field_group]['event_type']['hide'])) {
          continue;
        }
      }

      $row = NULL;
      if (isset($presentation_tables[$field_group])) {
        $row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $event_id, $field_group, $presentation_tables, $can_edit, $event);
      }

      if (!array_key_exists($field_group, $form['groups'])) {
        $form['groups'][$field_group] = array(
          '#tree' => TRUE,
        );
      }

      foreach ($event_array as $field_name => $values) {
        if (!isset($presentation[$field_group][$field_name])) {
          continue;
        }

        $field_presentation = $presentation[$field_group][$field_name];

        if (!isset($field_presentation['label'])) {
          continue;
        }

        if (empty($values)) {
          continue;
        }

        if (isset($field_presentation['event_type']) && is_array($field_presentation['event_type']) && array_key_exists('hide', $field_presentation['event_type'])) {
          if ($field_presentation['event_type']['hide'] === TRUE) {
            if (array_key_exists('show', $field_presentation['event_type'])) {
              if ($field_presentation['event_type']['show'] === TRUE) {
                // do nothing
              }
              elseif ($field_presentation['event_type']['show'] === FALSE) {
                continue;
              }
              elseif (is_array($field_presentation['event_type']['show']) && !in_array($event_type, $field_presentation['event_type']['show'])) {
                continue;
              }
            }
          }
          elseif ($field_presentation['event_type']['hide'] === FALSE) {
            // do nothing
          }
          elseif (is_array($field_presentation['event_type']['hide']) && in_array($event_type, $field_presentation['event_type']['hide'])) {
            continue;
          }
        }

        if (isset($field_presentation['group'])) {
          $group = $field_presentation['group'];

          $custom_row = NULL;
          if (isset($presentation_tables[$group])) {
            $custom_row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $event_id, $group, $presentation_tables, $can_edit, $event);
          }

          if (is_null($custom_row)) {
            $group_path = &$form['display']['groups'];
          }
          else {
            $group_path = &$form['display']['groups'][$custom_row];

            if (!isset($fields_count['group'][$custom_row] )) {
              $fields_count['group'][$custom_row] = 0;
            }
          }

          unset($custom_row);
        }
        else {
          $group = $field_group;

          if (is_null($row)) {
            $group_path = &$form['display']['groups'];
          }
          else {
            $group_path = &$form['display']['groups'][$row];

            if (!isset($fields_count['group'][$row])) {
              $fields_count['group'][$row] = 0;
            }
          }
        }

        if (!array_key_exists('#group_fields_count', $group_path)) {
          $group_path['#group_fields_count'] = 0;
        }

        $weight = 0;
        if (isset($field_presentation['weight'])) {
          $weight = $field_presentation['weight'];
        }

        if (!array_key_exists($weight, $group_path[$group])) {
          $group_path[$group][$weight] = array();
        }

        $group_path[$group][$weight][$field_name] = mcneese_event_workflow_build_event_request_item_structure($event_id, $group, $field_name, $values, $field_presentation, $event);

        // This can be FALSE on error or NULL when the ROW should not be displayed, so remove the field entirely.
        if ($group_path[$group][$weight][$field_name] === FALSE || is_null($group_path[$group][$weight][$field_name])) {
          unset($group_path[$group][$weight][$field_name]);
        }
        else {
          // process collate
          if (!empty($field_presentation['collate'])) {
            $collate_name = $field_presentation['collate'];
            $collate_order = 0;
            if (!empty($field_presentation['collate_order'])) {
              $collate_order = $field_presentation['collate_order'];
            }

            if (!isset($collate[$collate_name])) {
              $collate[$collate_name] = array();
            }

            $collate[$collate_name][$collate_order] = array(
              'field_name' => $field_name,
              'field_path' => &$group_path[$group][$weight],
            );
          }
        }
      }

      // make sure the weights are in the correct order.
      ksort($group_path[$group]);
    }

    // perform collation
    foreach ($collate as $collate_name => &$collate_fields) {
      ksort($collate_fields);

      $collate_parent = array_shift($collate_fields);

      if (!empty($collate_fields)) {
        foreach ($collate_fields as &$collate_value) {
          foreach ($collate_value['field_path'][$collate_value['field_name']] as $delta => &$value) {
            foreach ($value['value'] as $value_name => &$value_value) {
              if (!isset($collate_value['field_path'][$collate_name][$delta]['value'][$value_name]['markup']['#markup'])) {
                $collate_value['field_path'][$collate_name][$delta]['value'][$value_name]['markup']['#markup'] = '';
              }
              $collate_value['field_path'][$collate_name][$delta]['value'][$value_name]['markup']['#markup'] .= $value_value['markup']['#markup'];
            }
          }

          unset($collate_value['field_path'][$collate_value['field_name']]);
        }
      }
    }

    // post-process the structure for operations that may only be performed once the structure is completed.
    foreach ($form['display']['groups'] as $display_key => &$display_value) {
      if (!empty($display_value['#attributes'])) {
        $display_value['#prefix'] = '<div';
        $display_value['#suffix'] = '</div>';

        foreach ($display_value['#attributes'] as $attribute => $attribute_values) {
          if (is_array($attribute_values)) {
            $display_value['#prefix'] .= ' ' . $attribute . '="' . implode(' ', $attribute_values) . '"';
          }
          else {
            $display_value['#prefix'] .= ' ' . $attribute . '="' . $attribute_values . '"';
          }
        }

        $display_value['#prefix'] .= '>';

        if (isset($display_value['#extra_prefix'])) {
          $display_value['#prefix'] .= $display_value['#extra_prefix'];
        }
      }

      // create the group prefix and suffix and build the row stripes.
      if (is_numeric($display_key)) {
        foreach (array_keys($display_value) as $group_name) {
          if (preg_match('/^#/', $group_name)) continue;

          if (!empty($display_value[$group_name]['#attributes'])) {
            $display_value[$group_name]['#prefix'] = '<div';
            $display_value[$group_name]['#suffix'] = '</div>';

            foreach ($display_value[$group_name]['#attributes'] as $attribute => $attribute_values) {
              if (is_array($attribute_values)) {
                $display_value[$group_name]['#prefix'] .= ' ' . $attribute . '="' . implode(' ', $attribute_values) . '"';
              }
              else {
                $display_value[$group_name]['#prefix'] .= ' ' . $attribute . '="' . $attribute_values . '"';
              }
            }

            $display_value[$group_name]['#prefix'] .= '>';

            if (isset($display_value[$group_name]['#extra_prefix'])) {
              $display_value[$group_name]['#prefix'] .= $display_value[$group_name]['#extra_prefix'];
            }
          }

          $count = 0;

          $by_weight = array();
          foreach ($display_value[$group_name] as $weight => &$group_values) {
            if (!is_numeric($weight)) continue;

            foreach ($group_values as $field_name => &$field_values) {
              if (!array_key_exists($weight, $by_weight)) {
                $by_weight[$weight] = array();
                $field_weight = 0;
              }
              else {
                $field_weight = count($by_weight[$weight]);
              }

              $by_weight[$weight][$field_weight] = &$field_values;
            }
          }

          // make sure the weights are in the correct order.
          ksort($by_weight);

          $count = 0;
          foreach ($by_weight as $weight => &$item_row) {
            foreach ($item_row as $item_weight => &$item) {
              foreach ($item as $delta => &$value) {
                if ($count % 2 == 0) {
                  $value['#attributes']['class'][] = 'even';
                }
                else {
                  $value['#attributes']['class'][] = 'odd';
                }

                $count++;
              }
            }
          }
        }
      }
    }

    if (!$pdf) {
      $form['display']['submit'] = array(
        '#id' => 'event-display-submit',
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-display-submit',
            'mode-' . $mode,
          ),
        ),
        '#weight' => 1000,
      );

      if (mcneese_event_workflow_management_page_event_access('cancel', $event_id)) {
        $form['display']['submit']['cancel'] = array(
          '#id' => 'event-display-submit-cancel',
          '#type' => 'submit',
          '#value' => t("Cancel"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'event-display-submit-cancel',
              'mode-' . $mode,
            ),
          ),
          '#submit' => array(
            'mcneese_event_workflow_event_cancel_0_form_submit',
          ),
        );
      }
      elseif (mcneese_event_workflow_management_page_event_access('uncancel', $event_id)) {
        $form['display']['submit']['uncancel'] = array(
          '#id' => 'event-display-submit-uncancel',
          '#type' => 'submit',
          '#value' => t("Uncancel"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'event-display-submit-uncancel',
              'mode-' . $mode,
            ),
          ),
          '#submit' => array(
            'mcneese_event_workflow_event_uncancel_0_form_submit',
          ),
        );
      }

      $form['display']['submit']['print'] = array(
        '#id' => 'event-display-submit-print',
        '#type' => 'submit',
        '#value' => t("Print"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-display-submit-print',
            'mode-' . $mode,
          ),
          'onclick' => 'window.print(); return false;',
        ),
      );

      $form['display']['submit']['download'] = array(
        '#id' => 'event-display-submit-download',
        '#type' => 'submit',
        '#value' => t("Download"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-display-submit-download',
            'mode-' . $mode,
          ),
        ),
        '#submit' => array(
          'mcneese_event_workflow_event_view_0_page_submit_pdf',
        ),
      );
    }
  }

  if ($show_log || $show_operations) {
    $form['review'] = array(
      '#tree' => TRUE,
    );

    mcneese_event_workflow_include(4);

    $step_options = event_workflow_get_event_steps_list_options();

    if ($show_log) {
      $form['review']['log'] = array(
        '#id' => 'event-' . $event_id . '-section-review-log',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => "Review Log",
        '#attributes' => array(
          'class' => array(
            'event-section-review-log',
            'mode-' . $mode,
          ),
        ),
        '#tree' => TRUE,
      );

      $log_page_title_css = '';
      if (!$pdf) {
        $log_page_title_css = 'no-screen no-braille';
      }

      $form['review']['log']['page_title'] = array(
        '#markup' => '<div id="' . 'event-' . $event_id . '-section-review-log-page_title' . '" class="event-section-review-log-page_title ' . $log_page_title_css . '">' . $page_title . '</div>',
        '#weight' => 0,
      );

      if ($pdf) {
        unset($form['review']['log']['#type']);
        unset($form['review']['log']['#collapsible']);
        unset($form['review']['log']['#collapsed']);

        $form['review']['log']['#type'] = 'container';
        $form['review']['log']['#attributes']['id'] = 'event-' . $event_id . '-section-review-log';
      }

      $form['review']['log']['header'] = array(
        '#markup' => '<h2 id="event-' . $event_id . '-section-review-log-header" class="event-section-review-log-header">Review Log</h2>',
        '#weight' => 1,
      );

      $form['review']['log']['history'] = array(
        '#prefix' => '<div id="event-' . $event_id . '-section-review-log-history" class="event-section-review-log-history mode-' . $mode . '">',
        '#suffix' => '</div>',
        '#weight' => 2,
      );

      mcneese_event_workflow_build_reviews_log($form, $form_state, $event_id);

      $form['review']['log']['submit'] = array(
        '#id' => 'event-review-log-submit',
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-review-log-submit',
            'mode-' . $mode,
          ),
        ),
        '#weight' => 1000,
      );


      $form['review']['log']['submit']['print'] = array(
        '#id' => 'event-review-log-submit-print',
        '#type' => 'submit',
        '#value' => t("Print"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-review-log-submit-print',
            'mode-' . $mode,
          ),
          'onclick' => 'window.print(); return false;',
        ),
      );

      $form['review']['log']['submit']['download'] = array(
        '#id' => 'event-review-log-submit-download',
        '#type' => 'submit',
        '#value' => t("Download"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-review-log-submit-download',
            'mode-' . $mode,
          ),
        ),
        '#submit' => array(
          'mcneese_event_workflow_event_view_0_page_submit_pdf',
        ),
      );
    }

    if ($show_operations && !$pdf) {
      $decision_options = event_workflow_get_reviewer_decision_list_options('select');

      $form['review']['operations'] = array(
        '#id' => 'event-' . $event_id . '-section-review-operations',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => "Review Operations",
        '#attributes' => array(
          'class' => array(
            'event-section-review-operations',
            'no-print',
            'mode-' . $mode,
          ),
        ),
        '#tree' => TRUE,
      );

      if ($pdf) {
        unset($form['review']['operations']['#type']);
        unset($form['review']['operations']['#collapsible']);
        unset($form['review']['operations']['#collapsed']);
      }

      $form['review']['operations']['header'] = array(
        '#markup' => '<h2 id="event- ' . $event_id . '-section-review-operations-header" class="event-section-review-operations-header">Review Operations</h2>',
      );

      $form['review']['operations']['instructions'] = array(
        '#markup' => '',
      );

      if ($step == 116) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the venue coordinator to determine whether or not a given room is available at the specified time slots. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[115] . "</em>, then the event request is considered to be available and will continue to the next step. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[114] . "</em>, then the event request is considered to be unavailable and will be denied on grounds of no availability. ";
      }
      elseif ($step == 132 || $step == 133) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the reviewers to approve or deny the request. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[115] . "</em>, then the event request is considered to be accapted by you. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[114] . "</em>, then the event request is considered to be rejected by you. ";
        $form['review']['operations']['instructions']['#markup'] .= "Once all reviews have had their say, the event will proceed to the next step. ";
      }
      elseif ($step == 117) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the venue coordinator to perform the final approvel or denial. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[115] . "</em>, then the event request is considered to be accapted. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[114] . "</em>, then the event request is considered to be rejected. ";

        // @todo: add support for resetting the request back to the review step and then add a comment about that option here.
      }

      if ($step == 133) {
        $form['review']['operations']['equipment_use_fee'] = array(
          '#id' => 'event-review-operations-equipment_use_fee',
          '#type' => 'textfield',
          '#title' => 'Equipment Use Fee',
          '#default_value' => NULL,
          '#required' => TRUE,
          '#attributes' => array(
            'class' => array(
              'no-print',
              'mode-' . $mode,
            ),
          ),
        );

        $form['review']['operations']['facilities_use_fee'] = array(
          '#id' => 'event-review-operations-facilities_use_fee',
          '#type' => 'textfield',
          '#title' => 'Facilities Use Fee',
          '#default_value' => NULL,
          '#required' => TRUE,
          '#attributes' => array(
            'class' => array(
              'no-print',
              'mode-' . $mode,
            ),
          ),
        );
      }

      // make the 'skip' step available only under very specific situations (currently only the usage fees step).
      if ($step != 133) {
        unset($decision_options[135]);
      }

      $form['review']['operations']['decision'] = array(
        '#id' => 'event-review-operations-decision',
        '#type' => 'select',
        '#title' => "Decision",
        '#default_value' => NULL,
        '#options' => $decision_options,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'mode-' . $mode,
          ),
        ),
      );

      $form['review']['operations']['presidents_approval'] = array(
        '#id' => 'event-review-operations-presidents_approval',
        '#type' => 'checkbox',
        '#title' => "Approved by the President",
        '#description' => "By selecting this checkbox you claim that the president has approved this action. This action will be logged.",
        '#default_value' => NULL,
        '#required' => FALSE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'mode-' . $mode,
          ),
        ),
        '#states' => array(
          'visible' => array(
            ':input[name="review[operations][decision]"]' => array('value' => 135),
          ),
        ),
      );

      $form['review']['operations']['message'] = array(
        '#id' => 'event-review-operations-message',
        '#type' => 'textarea',
        '#title' => "Message",
        '#default_value' => NULL,
        '#rows' => 4,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'mode-' . $mode,
          ),
        ),
      );

      $form['review']['operations']['submit'] = array(
        '#id' => 'event-review-operations-submit',
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-review-operations-submit',
            'mode-' . $mode,
          ),
        ),
        '#weight' => 1000,
      );

      $form['review']['operations']['submit']['submit'] = array(
        '#id' => 'event-review-operations-submit-submit',
        '#type' => 'submit',
        '#value' => t("Submit"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-review-operations-submit-submit',
            'mode-' . $mode,
          ),
        ),
      );
    }
  }

  return $form;
}

/**
 * Create the group path presentation structure.
 *
 * The appropraite ctools_include() and ctools_modal_add_js() must be called
 * before using this function when $show_edit is TRUE.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param int $event_id
 *   The event request id.
 * @param string $group
 *   The group name.
 * @param array $presentation_tables
 *   An array of tables defining how the presentation is to be built.
 * @param bool $show_edit
 *   When TRUE, the edit link will be shown in the group header.
 * @param array $event
 *   The event settings array.
 *
 * @return int|null
 *   The row number or NULL when there is no valid row.
 *   FALSE is returned for in
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 * @see: ctools_include()
 * @see: ctools_modal_add_js()
 */
function mcneese_event_workflow_event_view_0_page_setup_group_path(&$form, $event_id, $group, $presentation_tables, $show_edit, $event) {
  $row = NULL;
  $group_path = &$form['display']['groups'];
  $event_type = $event['information']['type'][0]->value;

  $show_link = TRUE;
  if (isset($presentation_tables[$group]['event_type']['hide_link']) && is_array($presentation_tables[$group]['event_type']['hide_link'])) {
    if (in_array($event_type, $presentation_tables[$group]['event_type']['hide_link'])) {
      $show_link = FALSE;
    }
  }

  if ($show_edit) {
    if (array_key_exists('editable', $presentation_tables[$group])) {
      $show_edit = $presentation_tables[$group]['editable'];
    }
  }

  if (array_key_exists('group', $presentation_tables[$group])) {
    $row = (int) $presentation_tables[$group]['group'];

    if (!array_key_exists($row, $group_path)) {
      $form['display']['groups'][$row] = array(
        '#prefix' => '',
        '#suffix' => '',
        '#weight' => $row,
        '#attributes' => array(
          'id' => 'section-event-' . $event_id . '-group-' . $row,
          'class' => array(
            'group',
            'group-' . $row,
          ),
        ),
        '#extra_prefix' => '',
      );
    }

    $group_path = &$form['display']['groups'][$row];
  }

  if (!array_key_exists($group, $group_path)) {
    $group_path[$group] = array(
      '#prefix' => '',
      '#suffix' => '',
      '#weight' => 0,
      '#attributes' => array(
        'id' => 'section-event-' . $event_id . '-' . $group,
        'class' => array(
          'section-event',
          'section-event-' . $group,
        ),
      ),
      '#extra_prefix' => '',
    );

    if (isset($presentation_tables[$group]['label'])) {
      $group_path[$group]['#extra_prefix'] .= '<div class="section-event-' . $event_id . '-' . $group . '-label-wrapper section-event-label-wrapper">';

      if ($show_link) {
        #if ($show_edit && function_exists('ctools_modal_text_button')) {
        #  $group_path[$group]['#extra_prefix'] .= ctools_modal_text_button(t("Edit"), 'events/edit-0/ctools/' . $event_id . '/' . $group, t("Edit " . $presentation_tables[$group]['label']), 'section-event-' . $event_id . '-' . $group . '-edit section-event-edit no-print');
        #}

        if ($show_edit) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'events/edit-0/' . $event_id . '/' . $group . '" class="section-event-' . $event_id . '-' . $group . '-edit section-event-edit no-print" title="'. t("Edit " . $presentation_tables[$group]['label']) . '">Edit</a>';
        }
      }

      $group_path[$group]['#extra_prefix'] .= '<h2 id="section-event-' . $event_id . '-' . $group . '-label" class="section-event-label section-event-' . $group . '-label">';
      $group_path[$group]['#extra_prefix'] .= $presentation_tables[$group]['label'];
      $group_path[$group]['#extra_prefix'] .= '</h2>';

      $group_path[$group]['#extra_prefix'] .= '</div>';
    }

    if (isset($presentation_tables[$group]['weight'])) {
      $group_path[$group]['#weight'] = $presentation_tables[$group]['weight'];
    }
  }

  return $row;
}

/**
 * Pre-Validation for mcneese_event_workflow_event_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  if ($clicked_id == 'event-review-operations-submit-submit') {
    if (isset($form_state['values']['review']['operations']['decision']) && $form_state['values']['review']['operations']['decision'] == 113) {
      $form['review']['operations']['message']['#required'] = TRUE;
    }
  }
  else {
    $form['review']['operations']['decision']['#required'] = FALSE;
  }

  $event = $form['form']['event']['#value'];
  $event_id = (int) $form['form']['event_id']['#value'];
  $mode = (int) $form['form']['mode']['#value'];
  $user = cf_current_user();

  $event_classification = $event['coordinator']['classification'][0]->value;
  $step = $event['top']['step'][0]->value;
  $status = $event['top']['status'][0]->value;

  if ($clicked_id == 'event-review-operations-submit-submit') {
    if ($step == 133) {
      if ($form_state['values']['review']['operations']['decision'] == 135) {
        $form['review']['operations']['presidents_approval']['#required'] = TRUE;
        $form['review']['operations']['equipment_use_fee']['#required'] = FALSE;
        $form['review']['operations']['facilities_use_fee']['#required'] = FALSE;
      }
    }
  }
}

/**
 * Validation for mcneese_event_workflow_event_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  $form_state['rebuild'] = TRUE;

  $administer = user_access('mcneese event workflow administer');
  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $event = $form['form']['event']['#value'];
  $event_id = (int) $form['form']['event_id']['#value'];
  $mode = (int) $form['form']['mode']['#value'];
  $user = cf_current_user();

  $event_classification = $event['coordinator']['classification'][0]->value;
  $step = $event['top']['step'][0]->value;
  $status = $event['top']['status'][0]->value;

  if ($clicked_id == 'event-display-submit-print' || $clicked_id == 'event-review-log-submit-print') {
    form_set_error('', "The print button uses javascript to help force a print. You can always use your browsers print button (or equivalent) directly.");
  }
  elseif ($clicked_id == 'event-display-submit-download' || $clicked_id == 'event-review-log-submit-download') {
    $form_state['rebuild'] = FALSE;
  }
  elseif ($clicked_id == 'event-display-submit-cancel') {
    if ($mode != 0 && $mode != 1) {
      form_set_error('', "Illegal operation detected.");
    }

    $form_state['cancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'event-display-submit-uncancel') {
    if ($mode != 0 && $mode != 1) {
      form_set_error('', "Illegal operation detected.");
    }

    $form_state['uncancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'event-review-operations-submit-submit') {
    $failure = FALSE;

    // only allow operations for states and steps that make sense to.
    $allowed_statuses = array(
      MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED,
      MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED,
    );

    if (!in_array($status, $allowed_statuses)) {
      form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current status.");
      $failure = TRUE;
    }

    $allowed_steps = array(
      116,
      132,
      133,
      117,
    );

    if (!in_array($step, $allowed_steps)) {
      form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current step.");
      $failure = TRUE;
    }

    $reviewers = array();
    if ($mode < 0 && $mode > 3) {
      form_set_error('', "Illegal operation detected. Invalid mode selected.");
      $failure = TRUE;
    }
    elseif (!($administer || $reviewer)) {
      form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
      $failure = TRUE;
    }
    else {
      // show review only if the content is reviewable by the current user.
      $event_classification = $event['coordinator']['classification'][0]->value;
      $step = $event['top']['step'][0]->value;

      $reviewers = mcneese_event_workflow_get_reviewers(array('mr.user_id' => $user->uid, 'mr.event_classification' => $event_classification, 'mr.review_step' => $step));
    }

    if (empty($reviewers)) {
      form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
      $failure = TRUE;
    }

    if ($step == 133) {
      if (!empty($form_state['values']['review']['operations']['equipment_use_fee'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $form_state['values']['review']['operations']['equipment_use_fee'], $matches);

        if (!$matched) {
          form_set_error('review][operations][equipment_use_fee', t("@equipment_use_fee is not a valid usage fee. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@equipment_use_fee' => $form_state['values']['review']['operations']['equipment_use_fee'])));
          $failure = TRUE;
        }
      }

      if (!empty($form_state['values']['review']['operations']['facilities_use_fee'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d\d)$/i', $form_state['values']['review']['operations']['facilities_use_fee']);

        if (!$matched) {
          form_set_error('review][operations][facilities_use_fee', t("@facilities_use_fee is not a valid usage fee. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@facilities_use_fee' => $form_state['values']['review']['operations']['facilities_use_fee'])));
          $failure = TRUE;
        }
      }
    }

    if (!$failure) {
      $form_state['rebuild'] = FALSE;
    }
  }
  elseif ($clicked_id == 'event-cancel-submit-yes' || $clicked_id == 'event-cancel-submit-no') {
    $form_state['rebuild'] = FALSE;
  }
  elseif ($clicked_id == 'event-uncancel-submit-yes' || $clicked_id == 'unevent-cancel-submit-no') {
    $form_state['rebuild'] = FALSE;
  }
}

/**
 * Submit process for mcneese_event_workflow_event_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_submit($form, &$form_state) {
  $user = cf_current_user();

  $event_id = NULL;
  if (isset($form['form']['event_id']['#value'])) {
    $event_id = $form['form']['event_id']['#value'];
  }

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  if ($clicked_id == 'event-cancel-submit-yes') {
    $deleted = mcneese_event_workflow_include(5);

    mcneese_event_workflow_event_cancel_0_form_submit($form, $form_state);

    return;
  }
  elseif ($clicked_id == 'event-uncancel-submit-yes') {
    $deleted = mcneese_event_workflow_include(5);

    mcneese_event_workflow_event_uncancel_0_form_submit($form, $form_state);

    return;
  }
  // @todo: add submit processing for resetting the step back to the review stage.
  elseif ($clicked_id == 'event-review-operations-submit-submit') {
    // process the change
    mcneese_event_workflow_include(6);

    $reviewer_id = $form['form']['reviewer']['#value']->id;
    $decision = $form_state['values']['review']['operations']['decision'];
    $message = $form_state['values']['review']['operations']['message'];

    $event = $form['form']['event']['#value'];
    $step = $event['top']['step'][0]->value;

    $extra = array();

    if ($step == 133) {
      if ($decision == 135) {
        $extra['equipment_use_fee'] = 0;
        $extra['facilities_use_fee'] = 0;
      }
      else {
        $extra['equipment_use_fee'] = $form_state['values']['review']['operations']['equipment_use_fee'];
        $extra['facilities_use_fee'] = $form_state['values']['review']['operations']['facilities_use_fee'];
      }
    }

    $success = mcneese_event_workflow_workflow_step_review($event_id, $reviewer_id, $decision, $message, $extra, $user);

    if ($success) {
      drupal_set_message("Your review or comment has been recieved.");
    }
    else {
      form_set_error('form', "An error occurred while trying to submit the event review. Please contact the support staff.");
      watchdog('mew', "An error occured while trying to submit the event review.", array(), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($event_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      if ($form['form']['mode']['#value'] > 0) {
        $form_state['redirect'] = 'events/view-0/' . $event_id . '/' . $form['form']['mode']['#value'];
      }
      else {
        $form_state['redirect'] = 'events/view-0/' . $event_id;
      }
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Custom submit process for mcneese_event_workflow_event_view_0_page().
 *
 * Used for generating a print PDF.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_submit_pdf($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  $event_id = (int) $form['form']['event_id']['#value'];

  if (($clicked_id != 'event-display-submit-download' && $clicked_id != 'event-review-log-submit-download') || empty($event_id)) {
    if (empty($event_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      $form_state['redirect'] = 'events/view-0/' . $event_id;
    }

    return;
  }

  global $base_path;

  $mode = $form['form']['mode']['#value'];
  $form_state['redirect'] = '/events/pdf-0/' . $event_id . '/' . $mode;
}

/**
 * Renders a PDF of a given event request.
 *
 * @param int $event_id
 *   The unique identifier for an event request.
 * @param int|null $mode
 *   Specifies the mode that alters how the form is rendered.
 *   1: Render only the display part.
 */
function mcneese_event_workflow_event_pdf_0_page($event_id, $mode) {
  $page_title = "Event Request";
  drupal_set_title($page_title);

  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Event Request: " . $event['information']['title'][0]->value;
    drupal_set_title($page_title);
  }

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mcneese-event-workflow-event-view-0-page',
      'class' => array(
        'mcneese-event-workflow-event-pdf-0',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $page['html']['content'] = array(mcneese_event_workflow_event_view_0_page($form, $form_state, $event_id, $mode, TRUE));
  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  $result = mcneese_event_workflow_event_pdf_0_page_using_wkhtmltopdf($event_id, $html);

  if ($result) {
    exit();
  }
  else {
    drupal_not_found();
    drupal_exit();
    return array();
  }
}

/**
 * Build the review log display.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param int $event_id
 *   The numeric event id.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_build_reviews_log(&$form, &$form_state, $event_id) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }

  mcneese_event_workflow_include(4);

  $decision_options = event_workflow_get_reviewer_decision_list_options();

  $reviewers = array();

  // revisions are interpreted differently for reviews.
  // each revision is a 'repeat' or 're-review' so all users must re-process.
  // this means that the log must contain all revisions and all deltas.
  try {
    $query = db_select('mew_event_requests', 'mer');
    $query->innerjoin('mew_field_review_review', 'mfrr', 'mer.id = mfrr.event_id');

    $query->addField('mfrr', 'revision', 'revision');
    $query->addField('mfrr', 'delta', 'delta');
    $query->addField('mfrr', 'date', 'date');
    $query->addField('mfrr', 'reviewer', 'reviewer');
    $query->addField('mfrr', 'decision', 'decision');
    $query->addField('mfrr', 'message', 'message');

    $query->condition('mer.id', $event_id);

    $query->orderby('mfrr.revision');
    $query->orderby('mfrr.delta');

    $reviews = $query->execute()->fetchAll();
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  $has_log = FALSE;
  if (!empty($reviews)) {
    $review_step_options = event_workflow_get_review_step_list_options();

    foreach ($reviews as $row => $review) {
      $reviewer_id = $review->reviewer;
      $decision = $review->decision;
      $message = $review->message;
      $date = $review->date;

      if (!array_key_exists($reviewer_id, $reviewers)) {
        $reviewers_results = mcneese_event_workflow_get_reviewers(array('mr.id' => $reviewer_id), TRUE);

        if (!empty($reviewers_results)) {
          $reviewers[$reviewer_id] = array_pop($reviewers_results);
        }
        else {
          $reviewers[$reviewer_id] = NULL;
        }
      }

      $markup = '<div class="review-item review-item-' . $row. '">';

      $reviewer_name = '';
      if (is_null($reviewers[$reviewer_id])) {
        // @todo: this is an error, throw a watchdog message.
        continue;
      }

      $reviewer_name .= $reviewers[$reviewer_id]->first_name;
      $reviewer_name .= ' ' . $reviewers[$reviewer_id]->last_name;
      $review_step = $reviewers[$reviewer_id]->review_step;

      $markup .= '<div class="review-item-label">';
      $markup .= '<h3 class="review-item-label-text">';
      if ($review_step == 116 && $decision == 115) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has designated that the <strong>Venue is Available</strong>.";
      }
      elseif ($review_step == 116 && $decision == 114) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has designated that the <strong>Venue is Unavailable</strong>.";
      }
      elseif ($review_step == 117 && $decision == 115) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has <strong>Confirmed and Reserved</strong> the request.";
      }
      elseif ($review_step == 117 && $decision == 114) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has <strong>Denied</strong> the request.";
      }
      elseif ($decision == 113) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has made a <strong>" . $decision_options[$decision] . "</strong>.";
      }
      elseif ($review_step == 133 && $decision == 115) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has designated the <strong>" . $review_step_options[$review_step] . "</strong>.";
      }
      elseif ($review_step == 133 && $decision == 114) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has rejected the <strong>" . $review_step_options[$review_step] . "</strong>.";
      }
      elseif ($review_step == 133 && $decision == 135) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> has skipped the <strong>" . $review_step_options[$review_step] . "</strong> step.";
      }
      else {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong>' . $reviewer_name . "</strong> chose to <strong>" . $decision_options[$decision] . "</strong> the event request.";
      }
      $markup .= '</h3>';
      $markup .= '<div class="review-item-label-date">' . date("Y/m/d h:i:s a", $date) . "</div>";
      $markup .= '</div>';

      $markup .= '<div class="review-item-message">' . $message . '</div>';

      $markup .= '</div>';

      $form['review']['log']['history'][$row] = array(
        '#markup' => $markup,
      );

      $has_log = TRUE;
    }
  }

  if (!$has_log) {
    $form['review']['log']['history']['nothing'] = array(
      '#markup' => 'The review log is empty.',
    );
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
