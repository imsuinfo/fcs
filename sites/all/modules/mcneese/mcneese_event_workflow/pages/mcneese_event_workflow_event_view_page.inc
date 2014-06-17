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

  $can_log_event = mcneese_event_workflow_management_page_event_access('log', $event_id);
  $can_review_event = mcneese_event_workflow_management_page_event_access('review', $event_id);

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

  if (!$can_log_event) {
    $show_log = FALSE;

    if ($mode == 2) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }
  }

  if (!$can_review_event) {

  }

  if ($can_review_event) {
    // show review operations only if the content is reviewable by the current user.
    $event_classification = $event['event_coordinator']['classification'][0]->value;
    $step = $event['top']['step'][0]->value;
    $status = $event['top']['status'][0]->value;

    // only show operations for states and steps that make sense to.
    $allowed_statuses = array(
      MEW_EVENT_STATUS_LOCKED,
      MEW_EVENT_STATUS_UNLOCKED,
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
      MEW_REVIEW_STEP_VENUE_AVAILABLE,
      MEW_REVIEW_STEP_REVIEW,
      MEW_REVIEW_STEP_USAGE_FEES,
      MEW_REVIEW_STEP_MAKE_DECISIONS,
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

  $form['form']['user'] = array(
    '#type' => 'value',
    '#value' => $user,
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

    $presentation = mcneese_event_workflow_table_presentation();
    $presentation_tables = mcneese_event_workflow_table_presentation(1);

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

        // 'split' allows for having a single value span across multiple fields/rows.
        if (empty($presentation[$field_group][$field_name]['split']) || !is_array($presentation[$field_group][$field_name]['split'])) {
          $field_presentations = array();
          $field_presentations[$field_name] = $field_group;
        }
        else {
          $field_presentations = $presentation[$field_group][$field_name]['split'];
        }

        foreach ($field_presentations as $field_presentation_name => $field_presentation_group) {
          $field_presentation = $presentation[$field_presentation_group][$field_presentation_name];

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
            $group = $field_presentation_group;

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

          $group_path[$group][$weight][$field_presentation_name] = mcneese_event_workflow_build_event_request_item_structure($event_id, $group, $field_presentation_name, $values, $field_presentation, $event);

          // This can be FALSE on error or NULL when the ROW should not be displayed, so remove the field entirely.
          if ($group_path[$group][$weight][$field_presentation_name] === FALSE || is_null($group_path[$group][$weight][$field_presentation_name])) {
            unset($group_path[$group][$weight][$field_presentation_name]);
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
                'field_name' => $field_presentation_name,
                'field_path' => &$group_path[$group][$weight],
              );
            }
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

      if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the venue coordinator to determine whether or not a given room is available at the specified time slots. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_APPROVE] . "</em>, then the event request is considered to be available and will continue to the next step. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_DENY] . "</em>, then the event request is considered to be unavailable and will be denied on grounds of no availability. ";
      }
      elseif ($step == MEW_REVIEW_STEP_REVIEW) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the reviewers to approve or deny the request or assign usage fees. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_APPROVE] . "</em>, then the event request is considered to be accapted by you. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_DENY] . "</em>, then the event request is considered to be rejected by you. ";
        $form['review']['operations']['instructions']['#markup'] .= "Once all reviewers have had their say, the event will proceed to the next step. ";
      }
      elseif ($step == MEW_REVIEW_STEP_USAGE_FEES) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the reviewers to approve or deny the usage fees. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_APPROVE] . "</em>, then the usage fees are considered to be accapted by you. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_DENY] . "</em>, then the usage fees are considered to be rejected by you. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_WAVE] . "</em>, then the usage fees are waved (with presidents approval). ";
      }
      elseif ($step == MEW_REVIEW_STEP_MAKE_DECISIONS) {
        $form['review']['operations']['instructions']['#markup'] .= "The <em>" . $step_options[$step] . "</em> step allows for the venue coordinator to perform the final approvel or denial. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_APPROVE] . "</em>, then the event request is considered to be accapted. ";
        $form['review']['operations']['instructions']['#markup'] .= "When you select <em>" . $decision_options[MEW_REVIEW_DECISION_DENY] . "</em>, then the event request is considered to be rejected. ";
      }

      $form['review']['operations']['as_reviewer'] = array(
        '#markup' => '',
      );

      mcneese_event_workflow_include(4);
      $reviewer_classification_options = event_workflow_get_reviewer_classification_list_options();

      $form['review']['operations']['as_reviewer']['#markup'] = '<div class="event-section-review-operations-as_reviewer">';

      $reviewer_classification = array();
      if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || MEW_REVIEW_STEP_MAKE_DECISIONS) {
        if ($event['venue_coordinator']['user_id'][0]->value == $user->uid) {
          $reviewer_classification[] = $reviewer_classification_options[MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR];
          $form['review']['operations']['as_reviewer']['#markup'] .= '';
        }
      }
      elseif ($step == MEW_REVIEW_STEP_REVIEW || $step == MEW_REVIEW_STEP_USAGE_FEES) {
        $reviewer_conditions = array(
          'mr.user_id' => $user->uid,
          'mr.event_classification' => $event_classification,
          'mr.review_step' => $step,
        );

        $reviewers = mcneese_event_workflow_get_reviewers($reviewer_conditions, TRUE);
        if (!is_array($reviewers)) {
          $reviewers = array();
        }

        if (!empty($reviewers[$user_id][$review_step])) {
          foreach ($reviewers[$user_id][$review_step] as $reviewer) {
            $reviewer_classification[] = $reviewer_classification_options[$reviewer->reviewer_classification];
          }
        }
      }

      if (empty($reviewer_classification)) {
        if ($administer) {
          $reviewer_classification[] = 'System Administer';
        }
      }

      if (empty($reviewer_classification)) {
        if ($manager) {
          $reviewer_classification[] = 'Manager';
        }
      }

      if (!empty($reviewer_classification)) {
        $form['review']['operations']['as_reviewer']['#markup'] .= '<div class="event-section-review-operations-as_reviewer-message">You are reviewing with the following classifications:</div>';
        $form['review']['operations']['as_reviewer']['#markup'] .= theme('item_list', array('items' => $reviewer_classification, 'type' => 'ul', 'attributes' => array('class' => array('event-section-review-operations-as_reviewer-list'))));
      }

      $form['review']['operations']['as_reviewer']['#markup'] .= '</div>';


      if ($step == MEW_REVIEW_STEP_REVIEW) {
        $form['review']['operations']['use_fees'] = array(
          '#tree' => TRUE,
        );

        $uses = array();
        $insurance_fee = FALSE;
        foreach ($reviewers as $reviewer) {
          if ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_FACILITIES) {
            $uses['facilities'] = "Facilities";
            $uses['custodial'] = "Custodial";
          }
          elseif ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_SECURITY) {
            $uses['security'] = "Security";
          }
          elseif ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_PURCHASING) {
            $uses['equipment'] = "Equipment";
            $insurance_fee = TRUE;
          }
        }

        $use_fees_amount_defaults = array('custodial' => '18.50', 'security' => '30.00');
        $uses_personnel = array('custodial', 'security');
        $uses_hours = array('custodial', 'security');
        foreach ($uses as $use_name => $use_title) {
          $form['review']['operations']['use_fees'][$use_name] = array(
            '#type' => 'fieldset',
            '#title' => $use_title . ' Use Fee',
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#tree' => TRUE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'use_fees',
              ),
            ),
            '#states' => array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
              ),
            ),
          );

          if (in_array($use_name, $uses_personnel)) {
            $form['review']['operations']['use_fees'][$use_name]['quantity'] = array(
              '#id' => 'event-review-operations-use_fees-' . $use_name . '-quantity',
              '#type' => 'numberfield',
              '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Personnel',
              '#default_value' => NULL,
              '#min' => 0,
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'use_fee-quantity',
                ),
              ),
              '#states' => array(
                'visible' => array(
                  ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
                ),
              ),
            );

            if (!empty($event['use_fees'][$use_name][0])) {
              $form['review']['operations']['use_fees'][$use_name]['quantity']['#default_value'] = $event['use_fees'][$use_name][0]->quantity;
            }
          }

          if (in_array($use_name, $uses_hours)) {
            $form['review']['operations']['use_fees'][$use_name]['hours'] = array(
              '#id' => 'event-review-operations-use_fees-' . $use_name . '-hours',
              '#type' => 'numberfield',
              '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Hours',
              '#default_value' => NULL,
              '#min' => 0,
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'use_fee-hours',
                ),
              ),
              '#states' => array(
                'visible' => array(
                  ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
                ),
              ),
            );

            if (!empty($event['use_fees'][$use_name][0])) {
              $form['review']['operations']['use_fees'][$use_name]['hours']['#default_value'] = $event['use_fees'][$use_name][0]->hours;
            }
          }

          $form['review']['operations']['use_fees'][$use_name]['days'] = array(
            '#id' => 'event-review-operations-use_fees-' . $use_name . '-days',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Days',
            '#default_value' => NULL,
            '#min' => 0,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'use_fee-days',
              ),
            ),
            '#states' => array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
              ),
            ),
          );

          if (!empty($event['use_fees'][$use_name][0])) {
            $form['review']['operations']['use_fees'][$use_name]['days']['#default_value'] = $event['use_fees'][$use_name][0]->days;
          }

          $form['review']['operations']['use_fees'][$use_name]['amount'] = array(
            '#id' => 'event-review-operations-use_fees-' . $use_name . '-amount',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Amount',
            '#default_value' => NULL,
            '#size' => 31,
            '#min' => 0,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'use_fee-amount',
              ),
            ),
            '#states' => array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
              ),
            ),
          );

          if (!empty($event['use_fees'][$use_name][0])) {
            $form['review']['operations']['use_fees'][$use_name]['amount']['#default_value'] = event_workflow_convert_value_from_database_format($event['use_fees'][$use_name][0]->amount, 'currency');
          }
          elseif (isset($use_fees_amount_defaults[$use_name])) {
            $form['review']['operations']['use_fees'][$use_name]['amount']['#default_value'] = $use_fees_amount_defaults[$use_name];
          }
        }

        $form['review']['operations']['use_fees']['other'] = array(
          '#id' => 'event-review-operations-use_fees-other',
          '#type' => 'checkbox',
          '#title' => 'Other Use Fees',
          '#description' => "Other charges, unknown presently, will be billed to the presenter at a later time.",
          '#default_value' => NULL,
          '#attributes' => array(
            'class' => array(
              'no-print',
              'mode-' . $mode,
              'use_fee-other',
            ),
          ),
          '#states' => array(
            'visible' => array(
              ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
            ),
          ),
        );

        if (!empty($event['use_fees']['other'][0])) {
          $form['review']['operations']['use_fees']['other']['#default_value'] = $event['use_fees']['other'][0]->value;
        }

        $form['review']['operations']['insurance'] = array(
          '#tree' => TRUE,
        );

        if ($insurance_fee) {
          $form['review']['operations']['insurance']['contractor'] = array(
            '#id' => 'event-review-operations-insurance-contractor',
            '#type' => 'checkbox',
            '#title' => 'Contractor Insurance',
            '#description' => "Each technical/service industry contractor must provide proof of liability insurance in amounts and coverages specified.",
            '#default_value' => NULL,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'insurance-contractor',
              ),
            ),
            '#states' => array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
              ),
            ),
          );

          if (!empty($event['insurance']['contractor'][0])) {
            $form['review']['operations']['insurance']['contractor']['#default_value'] = $event['insurance']['contractor'][0]->value;
          }

          $form['review']['operations']['insurance']['unaffiliated'] = array(
            '#id' => 'event-review-operations-insurance-unaffiliated',
            '#type' => 'checkbox',
            '#title' => 'Unaffiliated Insurance',
            '#description' => "Non-affiliated entity (renter) must provide proof of liability insurance in amounts and coverages specified.",
            '#default_value' => NULL,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'insurance-unaffiliated',
              ),
            ),
            '#states' => array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
              ),
            ),
          );

          if (!empty($event['insurance']['unaffiliated'][0])) {
            $form['review']['operations']['insurance']['unaffiliated']['#default_value'] = $event['insurance']['unaffiliated'][0]->value;
          }
        }
      }

      // make the 'usage fee' step available only under very specific situations.
      if ($step != MEW_REVIEW_STEP_REVIEW) {
        unset($decision_options[MEW_REVIEW_DECISION_USAGE_FEE]);
      }

      // make the 'wave' step available only under very specific situations.
      if ($step != MEW_REVIEW_STEP_USAGE_FEES) {
        unset($decision_options[MEW_REVIEW_DECISION_WAVE]);
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
            ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_WAVE),
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
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'event-review-operations-submit-submit') {
    if (isset($form_state['values']['review']['operations']['decision']) && $form_state['values']['review']['operations']['decision'] == MEW_REVIEW_DECISION_COMMENT) {
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

  $event_classification = $event['event_coordinator']['classification'][0]->value;
  $step = $event['top']['step'][0]->value;
  $status = $event['top']['status'][0]->value;
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
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
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

  $event_classification = $event['event_coordinator']['classification'][0]->value;
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
      MEW_EVENT_STATUS_LOCKED,
      MEW_EVENT_STATUS_UNLOCKED,
    );

    if (!in_array($status, $allowed_statuses)) {
      form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current status.");
      $failure = TRUE;
    }

    $allowed_steps = array(
      MEW_REVIEW_STEP_VENUE_AVAILABLE,
      MEW_REVIEW_STEP_REVIEW,
      MEW_REVIEW_STEP_USAGE_FEES,
      MEW_REVIEW_STEP_MAKE_DECISIONS,
    );

    if (!in_array($step, $allowed_steps)) {
      form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current step.");
      $failure = TRUE;
    }

    $can_review_event = mcneese_event_workflow_management_page_event_access('review', $event_id);

    $reviewers = array();
    if ($mode < 0 && $mode > 3) {
      form_set_error('', "Illegal operation detected. Invalid mode selected.");
      $failure = TRUE;
    }
    elseif (!$can_review_event) {
      form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
      $failure = TRUE;
    }

    if ($step == MEW_REVIEW_STEP_REVIEW) {
      $use_fees = array();
      if (isset($form_state['values']['review']['operations']['use_fees'])) {
        $use_fees = $form_state['values']['review']['operations']['use_fees'];
      }

      if (array_key_exists('facilities', $use_fees) && !empty($use_fees['facilities']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $use_fees['facilities']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][use_fees][facilities][amount', t("@facilities_use_fee is not a valid facilities usage fee amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@facilities' => $use_fees['facilities']['amount'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('facilities', $use_fees) && !empty($use_fees['facilities']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $use_fees['equipment']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][use_fees][equipment][amount', t("@equipment is not a valid equipment usage fee. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@equipment' => $use_fees['equipment']['amount'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('custodial', $use_fees) && !empty($use_fees['custodial']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $use_fees['custodial']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][use_fees][custodial][amount', t("@custodial is not a valid custodial usage fee amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@custodial' => $use_fees['custodial']['amount'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('security', $use_fees) && !empty($use_fees['security']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $use_fees['security']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][use_fees][security][amount', t("@security is not a valid security usage fee amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@security' => $use_fees['security']['amount'])));
          $failure = TRUE;
        }
      }

      unset($use_fees);
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
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
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
    mcneese_event_workflow_include(6);
    $decision = $form_state['values']['review']['operations']['decision'];
    $message = $form_state['values']['review']['operations']['message'];

    $event = $form['form']['event']['#value'];

    $event_classification = $event['event_coordinator']['classification'][0]->value;
    $step = $event['top']['step'][0]->value;

    $extra = array();
    if ($step == MEW_REVIEW_STEP_REVIEW) {
      if (isset($form_state['values']['review']['operations']['use_fees'])) {
        $extra['use_fees'] = $form_state['values']['review']['operations']['use_fees'];
      }

      if (isset($form_state['values']['review']['operations']['insurance'])) {
        $extra['insurance'] = $form_state['values']['review']['operations']['insurance'];
      }
    }
    elseif ($step == MEW_REVIEW_STEP_USAGE_FEES) {
      if (isset($form_state['values']['review']['operations']['presidents_approval'])) {
        $extra['use_fees'] = array('waved' => $form_state['values']['review']['operations']['presidents_approval']);
      }
    }

    $success = mcneese_event_workflow_workflow_step_review($event_id, $form['form']['user']['#value'], $step, $decision, $message, $extra);

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
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
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

  mcneese_event_workflow_include(0);

  $presentation = mcneese_event_workflow_table_presentation();

  mcneese_event_workflow_include(4);

  $decision_options = event_workflow_get_reviewer_decision_list_options();

  // revisions are interpreted differently for reviews.
  // each revision is a 'repeat' or 're-review' so all users must re-process.
  // this means that the log must contain all revisions and all deltas.
  try {
    $query = db_select('mew_event_requests', 'mer');
    $query->innerjoin('mew_field_review_review', 'mfrr', 'mer.id = mfrr.event_id');

    $query->addField('mfrr', 'revision', 'revision');
    $query->addField('mfrr', 'delta', 'delta');
    $query->addField('mfrr', 'date', 'date');
    $query->addField('mfrr', 'user_id', 'user_id');
    $query->addField('mfrr', 'step', 'step');
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


  $changelog_fields = array();
  $changelog_fields['use_fees'] = array(
    'facilities',
    'equipment',
    'custodial',
    'security',
    'other',
    'waved',
  );
  $changelog_fields['insurance'] = array(
    'insurance',
    'unaffiliated',
  );

  $users = array();
  $reviewers = array();
  $has_log = FALSE;
  if (!empty($reviews)) {
    $review_step_options = event_workflow_get_review_step_list_options();
    $reviewer_classification_options = event_workflow_get_reviewer_classification_list_options();

    foreach ($reviews as $row => $review) {
      $user_id = $review->user_id;
      $review_step = $review->step;
      $decision = $review->decision;
      $message = $review->message;
      $date = $review->date;

      $changelog = event_workflow_load_event_by_timestamp($event_id, $date, $changelog_fields);

      if (!array_key_exists($user_id, $users)) {
        $users[$user_id] = user_load($user_id);

        if (!is_object($users[$user_id])) {
          $users[$user_id] = NULL;
        }
      }

      if (!array_key_exists($user_id, $reviewers) || !array_key_exists($review_step, $reviewers[$user_id])) {
        $reviewer_conditions = array(
          'mr.user_id' => $user_id,
          'mr.event_classification' => $event['event_coordinator']['classification'][0]->value,
          'mr.review_step' => $review_step,
        );
        $reviewers[$user_id][$review_step] = mcneese_event_workflow_get_reviewers($reviewer_conditions);
      }

      $markup = '<div class="review-item review-item-' . $row. '">';

      $user_name = '';
      if (is_null($users[$user_id])) {
        // @todo: this is an error, throw a watchdog message.
        continue;
      }

      $reviewer_classification = NULL;
      if ($event['venue_coordinator']['user_id'][0]->value == $user_id) {
        $reviewer_classification = $reviewer_classification_options[MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR];
      }

      if (!empty($reviewers[$user_id][$review_step])) {
        foreach ($reviewers[$user_id][$review_step] as $reviewer) {
          if (is_null($reviewer_classification)) {
            $reviewer_classification = '';
          }
          else {
            $reviewer_classification .= ', ';
          }

          $reviewer_classification .= $reviewer_classification_options[$reviewer->reviewer_classification];
        }
      }

      if (is_null($reviewer_classification)) {
        if (user_access('mcneese event workflow administer', $users[$user_id])) {
          $reviewer_classification = 'System Administer';
        }
      }

      if (is_null($reviewer_classification)) {
        if (user_access('mcneese event workflow manage', $users[$user_id])) {
          $reviewer_classification = 'Manager';
        }
      }

      if (!is_null($reviewer_classification)) {
        $reviewer_classification = ' as <strong class="review-item-label-text-row-reviewer_classification">'. $reviewer_classification . '</strong>';
      }

      $user_name .= $users[$user_id]->field_user_first_name['und'][0]['value'];
      $user_name .= ' ' . $users[$user_id]->field_user_last_name['und'][0]['value'];

      $markup .= '<div class="review-item-label">';
      $markup .= '<h3 class="review-item-label-text">';
      if ($review_step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_APPROVE) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_DENY) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has designated that the <strong class="review-item-label-text-row-decision">Venue is Unavailable</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_MAKE_DECISIONS && $decision == MEW_REVIEW_DECISION_APPROVE) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has <strong class="review-item-label-text-row-decision">Confirmed and Reserved</strong> the request' . $reviewer_classification . "";
      }
      elseif ($review_step == MEW_REVIEW_STEP_MAKE_DECISIONS && $decision == MEW_REVIEW_DECISION_DENY) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has <strong class="review-item-label-text-row-decision">Denied</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MEW_REVIEW_DECISION_COMMENT) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has made a <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_REVIEW && $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has designated the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_REVIEW && $decision == MEW_REVIEW_DECISION_WAVE) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has waved the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> step' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_USAGE_FEES && $decision == MEW_REVIEW_DECISION_APPROVE) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has accepted the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_USAGE_FEES && $decision == MEW_REVIEW_DECISION_DENY) {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> has rejected the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      else {
        $markup .= '<span class="review-item-label-text-row">[' . $row . ']</span> <strong class="review-item-label-text-row-user">' . $user_name . '</strong> chose to <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> the event request' . $reviewer_classification . '.';
      }
      $markup .= '</h3>';
      $markup .= '<div class="review-item-label-date">' . date("Y/m/d h:i:s a", $date) . "</div>";
      $markup .= '</div>';


      // provide additional changelog information.
      if (!empty($changelog['use_fees']['facilities'])) {
        $message .= '<div class="review-item-message-use_fees-facilities review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Facilities Use Fees:</div> ';

        $value = reset($changelog['use_fees']['facilities']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'facilities', 0, $value, $presentation['use_fees']['facilities'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['equipment'])) {
        $message .= '<div class="review-item-message-use_fees-equipment review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Equipment Use Fees:</div> ';

        $value = reset($changelog['use_fees']['equipment']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'equipment', 0, $value, $presentation['use_fees']['equipment'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['security'])) {
        $message .= '<div class="review-item-message-use_fees-security review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Security Use Fees:</div> ';

        $value = reset($changelog['use_fees']['security']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'security', 0, $value, $presentation['use_fees']['security'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['custodial'])) {
        $message .= '<div class="review-item-message-use_fees-custodial review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Custodial Use Fees:</div> ';

        $value = reset($changelog['use_fees']['custodial']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'custodial', 0, $value, $presentation['use_fees']['custodial'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['other'])) {
        $message .= '<div class="review-item-message-use_fees-other review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Other Use Fees:</div> ';

        $value = reset($changelog['use_fees']['other']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'other', 0, $value, $presentation['use_fees']['other'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['waved'])) {
        $message .= '<div class="review-item-message-use_fees-waved review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Use Fees Waved:</div> ';

        $value = reset($changelog['use_fees']['waved']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'waved', 0, $value, $presentation['use_fees']['waved'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['insurance']['contractor'])) {
        $message .= '<div class="review-item-message-insurance-contractor review-item-message-insurance review-item-message-fees">';
        $message .= '<div class="item-label">Contractor Insurance:</div> ';

        $value = reset($changelog['insurance']['contractor']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'insurance', 'contractor', 0, $value, $presentation['insurance']['contractor'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      if (!empty($changelog['insurance']['unaffiliated'])) {
        $message .= '<div class="review-item-message-insurance-unaffiliated review-item-message-insurance review-item-message-fees">';
        $message .= '<div class="item-label">Unaffiliated Insurance:</div> ';

        $value = reset($changelog['insurance']['unaffiliated']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'insurance', 'unaffiliated', 0, $value, $presentation['insurance']['unaffiliated'], $changelog);

        $message .= drupal_render($value_markup);
        $message .= '</div>';
      }

      $markup .= '<div class="review-item-message">' . $message . '</div>';

      $markup .= '</div>';

      $form['review']['log']['history'][$row] = array(
        '#markup' => $markup,
      );

      $has_log = TRUE;
      unset($changelog);
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
