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
    watchdog(MEW_WATCHDOG_ID, "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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

  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $venue_coordinator = ($event['venue_coordinator']['user_id'][0]->value == $user->uid);

  $can_create = mcneese_event_workflow_management_page_event_access('create');

  $can_create = mcneese_event_workflow_management_page_event_access('create');
  $can_edit = mcneese_event_workflow_management_page_event_access('edit');
  $can_view = mcneese_event_workflow_management_page_event_access('view');
  $can_list = mcneese_event_workflow_management_page_event_access('list');
  $can_search = mcneese_event_workflow_management_page_event_access('search');
  $can_delete = mcneese_event_workflow_management_page_event_access('delete');
  $can_log = mcneese_event_workflow_management_page_event_access('log');
  $can_review = mcneese_event_workflow_management_page_event_access('review');

  $can_view_event = mcneese_event_workflow_management_page_event_access('view', $event_id);
  $can_edit_event = mcneese_event_workflow_management_page_event_access('edit', $event_id);
  $can_log_event = mcneese_event_workflow_management_page_event_access('log', $event_id);
  $can_review_event = mcneese_event_workflow_management_page_event_access('review', $event_id);
  $can_comment_event = mcneese_event_workflow_management_page_event_access('comment', $event_id);
  $can_history_event = mcneese_event_workflow_management_page_event_access('history', $event_id);

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

  if ($can_review_event || $can_comment_event) {
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

  $can = array(
    'edit' => FALSE,
    'history' => FALSE,
  );

  if ($pdf) {
    $can['edit'] = FALSE;
    $can['history'] = FALSE;
    $can['clone'] = FALSE;
  }
  else {
    $can['edit'] = (bool) $can_edit_event;
    $can['history'] = (bool) $can_history_event;
    $can['clone'] = (bool) ($can_create && $can_view_event);
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

  $event_classification = $event['event_coordinator']['classification'][0]->value;
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
      mcneese_event_workflow_event_view_0_page_setup_group_path($form, $event_id, $field_group, $presentation_tables, $can, $event);
    }

    // pre-process special case of review decisions.
    $decision_presentation = $presentation['reviews']['decision'];
    unset($presentation['reviews']['decision']);

    $reviews = mcneese_event_workflow_get_latest_reviews($event_id, array(MEW_REVIEW_STEP_REVIEW, MEW_REVIEW_STEP_MAKE_DECISIONS), array(MEW_REVIEW_DECISION_APPROVE, MEW_REVIEW_DECISION_DENY));

    if (!empty($reviews)) {
      mcneese_event_workflow_include(4);

      $classification_options = event_workflow_get_reviewer_classification_list_options(NULL, TRUE);
      $preprocessed_decisions = array();

      $reviews_step = array();
      if (isset($reviews[MEW_REVIEW_STEP_REVIEW])) {
        $reviews_step = $reviews[MEW_REVIEW_STEP_REVIEW];
      }

      if (!empty($reviews_step)) {
        foreach ($reviews_step as $reviewer_classification => $reviewer_decisions) {
          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          // the last decision made is the first item and should be the only value displayed.
          $decision = array_shift($reviewer_decisions);
          $preprocessed_decisions[$reviewer_classification] = array(0 => $decision);
        }
      }

      // apply alphabetical sorting to decisions.
      $weight = 0;
      foreach ($classification_options as $key => $name) {
        $key_string = 'classification-' . $key;
        if (isset($preprocessed_decisions[$key])) {
          $presentation['reviews'][$key_string] = $decision_presentation;
          $presentation['reviews'][$key_string]['label'] = $name;
          $presentation['reviews'][$key_string]['weight'] = $weight;

          $event['reviews'][$key_string] = $preprocessed_decisions[$key];
          $weight++;
        }
      }

      // display the final decision last.
      $decision_step = array();
      if (isset($reviews[MEW_REVIEW_STEP_MAKE_DECISIONS])) {
        $decision_step = $reviews[MEW_REVIEW_STEP_MAKE_DECISIONS];
      }

      if (!empty($decision_step)) {
        foreach ($decision_step as $reviewer_classification => $reviewer_decisions) {
          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          // the last decision made is the first item and should be the only value displayed.
          $decision = array_shift($reviewer_decisions);

          $key_string = 'classification-final_decision';
          $presentation['reviews'][$key_string] = $decision_presentation;
          $presentation['reviews'][$key_string]['label'] = "Final Decision";
          $presentation['reviews'][$key_string]['weight'] = $weight;

          $event['reviews'][$key_string] = array(0 => $decision);
          $weight++;
          break;
        }
      }

      unset($classification_options);
    }
    unset($reviews);

    // build the presentation tables.
    foreach ($event as $field_group => $event_array) {
      foreach (array('event_type' => $event_type, 'event_classification' => $event_classification) as $show_hide_type => $show_hide_value) {
        if (isset($presentation_tables[$field_group][$show_hide_type]) && is_array($presentation_tables[$field_group][$show_hide_type]) && array_key_exists('hide', $presentation_tables[$field_group][$show_hide_type])) {
          if ($presentation_tables[$field_group][$show_hide_type]['hide'] === TRUE) {
            if (array_key_exists('show', $presentation_tables[$field_group][$show_hide_type])) {
              if ($presentation_tables[$field_group][$show_hide_type]['show'] === TRUE) {
                // do nothing
              }
              elseif ($presentation_tables[$field_group][$show_hide_type]['show'] === FALSE) {
                continue 2;
              }
              elseif (is_array($presentation_tables[$field_group][$show_hide_type]['show']) && !in_array($show_hide_value, $presentation_tables[$field_group][$show_hide_type]['show'])) {
                continue 2;
              }
            }
          }
          elseif ($presentation_tables[$field_group][$show_hide_type]['hide'] === FALSE) {
            // do nothing
          }
          elseif (is_array($presentation_tables[$field_group][$show_hide_type]['hide']) && in_array($show_hide_value, $presentation_tables[$field_group][$show_hide_type]['hide'])) {
            continue 2;
          }
        }
      }

      $row = NULL;
      if (isset($presentation_tables[$field_group])) {
        $row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $event_id, $field_group, $presentation_tables, $can, $event);
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

          foreach (array('event_type' => $event_type, 'event_classification' => $event_classification) as $show_hide_type => $show_hide_value) {
            if (isset($field_presentation[$show_hide_type]) && is_array($field_presentation[$show_hide_type]) && array_key_exists('hide', $field_presentation[$show_hide_type])) {
              if ($field_presentation[$show_hide_type]['hide'] === TRUE) {
                if (array_key_exists('show', $field_presentation[$show_hide_type])) {
                  if ($field_presentation[$show_hide_type]['show'] === TRUE) {
                    // do nothing
                  }
                  elseif ($field_presentation[$show_hide_type]['show'] === FALSE) {
                    continue 2;
                  }
                  elseif (is_array($field_presentation[$show_hide_type]['show']) && !in_array($show_hide_value, $field_presentation[$show_hide_type]['show'])) {
                    continue 2;
                  }
                }
              }
              elseif ($field_presentation[$show_hide_type]['hide'] === FALSE) {
                // do nothing
              }
              elseif (is_array($field_presentation[$show_hide_type]['hide']) && in_array($show_hide_value, $field_presentation[$show_hide_type]['hide'])) {
                continue 2;
              }
            }
          }

          if (isset($field_presentation['group'])) {
            $group = $field_presentation['group'];

            $custom_row = NULL;
            if (isset($presentation_tables[$group])) {
              $custom_row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $event_id, $group, $presentation_tables, $can, $event);
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

              $collate_into = FALSE;
              if (!empty($field_presentation['collate_into'])) {
                $collate_into = $field_presentation['collate_into'];
              }

              if (!isset($collate[$collate_name])) {
                $collate[$collate_name] = array();
              }

              if (!isset($collate[$collate_name]['collate_into'])) {
                $collate[$collate_name]['collate_into'] = $collate_order;
              }

              $collate[$collate_name][$collate_order] = array(
                'field_name' => $field_presentation_name,
                'field_path' => &$group_path[$group][$weight],
                'collate_into' => $collate_into,
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
      $collate_into = $collate_fields['collate_into'];
      unset($collate_fields['collate_into']);
      unset($collate_fields[$collate_into]);

      ksort($collate_fields);

      if (!empty($collate_fields)) {
        foreach ($collate_fields as $collate_delta => &$collate_value) {
          foreach ($collate_value['field_path'][$collate_value['field_name']] as $delta => &$value) {
            foreach ($value['value'] as $value_name => &$value_value) {
              if (!isset($collate_value['field_path'][$collate_name][$delta]['value'][$value_name]['markup']['#markup'])) {
                $collate_value['field_path'][$collate_name][$delta]['value'][$value_name]['markup']['#markup'] = '';
              }

              $collate_markup = &$collate_value['field_path'][$collate_name][$delta]['value'][$value_name]['markup']['#markup'];

              if ($collate_delta < $collate_into) {
                $collate_markup = $value_value['markup']['#markup'] . $collate_markup;
              }
              else {
                $collate_markup .= $value_value['markup']['#markup'];
              }
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
          '#name' => 'event-display-submit-cancel',
          '#type' => 'submit',
          '#default_value' => t("Cancel"),
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
          '#name' => 'event-display-submit-uncancel',
          '#type' => 'submit',
          '#default_value' => t("Uncancel"),
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
        '#name' => 'event-display-submit-print',
        '#type' => 'submit',
        '#default_value' => t("Print"),
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
        '#name' => 'event-display-submit-download',
        '#type' => 'submit',
        '#default_value' => t("Download"),
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

      $reviewers_yet_to_review = mcneese_event_workflow_get_reviewers_yet_to_review($event_id);

      $yet_to_review_markup = '';
      if (!empty($reviewers_yet_to_review)) {
        $yet_to_review_markup .= '<div id="event-' . $event_id . '-section-review-log-yet_to_review" class="event-section-review-log-yet_to_review">';
        $yet_to_review_markup .= '<div class="event-section-review-log-yet_to_review-label">Reviewer classifications that have not yet reviewed:</div>';
        $yet_to_review_markup .= '<ul class="event-section-review-log-yet_to_review-list">';

        foreach ($reviewers_yet_to_review as $yet_to_review_classification) {
          $yet_to_review_markup .= '<li class="event-section-review-log-yet_to_review-list_item">' . $yet_to_review_classification . '</li>';
        }

        $yet_to_review_markup .= '</ul>';
      }

      $form['review']['log']['yet_to_review'] = array(
        '#markup' => $yet_to_review_markup,
        '#weight' => 2,
      );

      $form['review']['log']['history'] = array(
        '#prefix' => '<div id="event-' . $event_id . '-section-review-log-history" class="event-section-review-log-history mode-' . $mode . '">',
        '#suffix' => '</div>',
        '#weight' => 3,
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
        '#name' => 'event-review-log-submit-print',
        '#type' => 'submit',
        '#default_value' => t("Print"),
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
        '#name' => 'event-review-log-submit-download',
        '#type' => 'submit',
        '#default_value' => t("Download"),
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
      $decision_options = event_workflow_get_reviewer_decision_list_options('review_step');

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

      $form['review']['operations']['instructions']['#markup'] .= "The current step is: <strong>" . $step_options[$step] . "</strong>. ";

      $form['review']['operations']['as_reviewer'] = array(
        '#markup' => '',
      );

      $form['review']['operations']['usage_fees'] = array(
        '#markup' => '',
      );

      mcneese_event_workflow_include(4);
      $reviewer_classification_options = event_workflow_get_reviewer_classification_list_options();

      $reviewer_conditions = array(
        'mr.user_id' => $user->uid,
        'mr.event_classification' => $event_classification,
        'mr.review_step' => $step,
        'mr.disabled' => 0,
      );

      $reviewers = mcneese_event_workflow_get_reviewers($reviewer_conditions, TRUE);
      if (!is_array($reviewers)) {
        $reviewers = array();
      }

      $form['review']['operations']['as_reviewer']['#markup'] = '<div class="event-section-review-operations-as_reviewer">';

      $reviewer_classification = array();
      if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_MAKE_DECISIONS) {
        if ($venue_coordinator) {
          $reviewer_classification[MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
        }
      }
      elseif ($step == MEW_REVIEW_STEP_REVIEW || $step == MEW_REVIEW_STEP_USAGE_FEES) {
        foreach ($reviewers as $reviewer) {
          if (isset($reviewer->review_step) && $reviewer->review_step == $step) {
            if (isset($reviewer->reviewer_classification)) {
              $reviewer_classification[$reviewer->reviewer_classification] = $reviewer_classification_options[$reviewer->reviewer_classification];
            }
          }
        }
      }

      if ($can_comment_event) {
        if (empty($reviewer_classification)) {
          if ($manager) {
            $reviewer_classification[MEW_REVIEW_CLASSIFICATION_MANAGER] = 'Manager';
          }
        }
      }

      if (!empty($reviewer_classification)) {
        $form['review']['operations']['as_reviewer']['#markup'] .= '<div class="event-section-review-operations-as_reviewer-message">You are reviewing with the following classifications:</div>';
        $form['review']['operations']['as_reviewer']['#markup'] .= theme('item_list', array('items' => $reviewer_classification, 'type' => 'ul', 'attributes' => array('class' => array('event-section-review-operations-as_reviewer-list'))));
      }

      $form['review']['operations']['as_reviewer']['#markup'] .= '</div>';

      // make the 'usage fee' step available only under very specific situations.
      if ($step != MEW_REVIEW_STEP_VENUE_AVAILABLE && $step != MEW_REVIEW_STEP_REVIEW) {
        unset($decision_options[MEW_REVIEW_DECISION_USAGE_FEE]);
      }

      // restrict options to commenting only, if the user is only allowed to comment.
      if (!$can_review_event && $can_comment_event) {
        $decision_options = array(
          MEW_REVIEW_DECISION_COMMENT => $decision_options[MEW_REVIEW_DECISION_COMMENT],
        );
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

      if ($can_review_event && ($step == MEW_REVIEW_STEP_REVIEW || $step == MEW_REVIEW_STEP_VENUE_AVAILABLE)) {
        $form['review']['operations']['use_fees'] = array(
          '#tree' => TRUE,
        );

        $form['form']['use_fees-custodial'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $form['form']['use_fees-security'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $form['form']['use_fees-insurance'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $uses = array();
        $insurance_fee = FALSE;
        if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
          if ($venue_coordinator) {
            $uses['facilities'] = "Facilities";
            $uses['equipment'] = "Additional Equipment";
          }
        }
        else {
          foreach ($reviewers as $reviewer) {
            if ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_FACILITIES) {
              $uses['custodial'] = "Custodial";
              $form['form']['use_fees-custodial']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_SECURITY) {
              $uses['security'] = "Security";
              $form['form']['use_fees-security']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_PURCHASING) {
              $insurance_fee = TRUE;
              $form['form']['use_fees-insurance']['#value'] = TRUE;
            }
          }
        }

        // if the reviewer has no usage fees or insurance fees to specify, then do not display the Usage Fees select option.
        if (!$insurance_fee && empty($uses)) {
          unset($decision_options[MEW_REVIEW_DECISION_USAGE_FEE]);
          $form['review']['operations']['decision']['#options'] = $decision_options;
        }

        // if the usage fees are waved, then do not display the Usage Fees select option.
        $usage_fees_waved = FALSE;
        if ($step != MEW_REVIEW_STEP_VENUE_AVAILABLE) {
          if (isset($event['use_fees']['waved'][0]->value) && $event['use_fees']['waved'][0]->value == 1) {
            $usage_fees_waved = TRUE;
          }
          elseif (isset($event['use_fees']['university'][0]->value) && $event['use_fees']['university'][0]->value == 1) {
            $usage_fees_waved = TRUE;
          }
        }

        if ($usage_fees_waved) {
          unset($decision_options[MEW_REVIEW_DECISION_USAGE_FEE]);
          $form['review']['operations']['decision']['#options'] = $decision_options;

          $form['review']['operations']['usage_fees']['#markup'] = '<div class="event-section-review-operations-usage_fees">';
          $form['review']['operations']['usage_fees']['#markup'] .= "Usage Fees are waved and therefore no usage fee option is provided. You may immediately approve/deny this request.";
          $form['review']['operations']['usage_fees']['#markup'] .= '</div>';
        }

        $use_fees_amount_defaults = array('custodial' => '18.50', 'security' => '30.00');
        $uses_personnel = array('custodial', 'security');
        $uses_hours = array('custodial', 'security');
        $uses_requirements = array('facilities' => TRUE, 'equipment' => FALSE, 'custodial' => TRUE, 'security' => TRUE);
        $uses_review = array('facilities' => FALSE, 'equipment' => FALSE, 'custodial' => TRUE, 'security' => TRUE);
        foreach ($uses as $use_name => $use_title) {
          if ($uses_review[$use_name] && $step != MEW_REVIEW_STEP_REVIEW) {
            continue;
          }

          $states = array(
            'visible' => array(
              ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
            ),
          );

          if (!$uses_review[$use_name] && $step != MEW_REVIEW_STEP_REVIEW) {
            $states = array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
                ':input[name="review[operations][presidents_approval]"]' => array('checked' => FALSE),
                ':input[name="review[operations][university_business]"]' => array('checked' => FALSE),
              ),
              'invisible' => array(
                'or' => array(
                  ':input[name="review[operations][presidents_approval]"]' => array('checked' => TRUE),
                  ':input[name="review[operations][university_business]"]' => array('checked' => TRUE),
                ),
              ),
            );
          }

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
            '#states' => $states,
          );

          if (in_array($use_name, $uses_personnel)) {
            $form['review']['operations']['use_fees'][$use_name]['quantity'] = array(
              '#id' => 'event-review-operations-use_fees-' . $use_name . '-quantity',
              '#type' => 'numberfield',
              '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Personnel',
              '#default_value' => NULL,
              '#size' => 16,
              '#min' => 0,
              '#required' => $uses_requirements[$use_name],
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'use_fee-quantity',
                ),
              ),
              '#states' => $states,
            );

            if (!empty($event['use_fees'][$use_name][0])) {
              $form['review']['operations']['use_fees'][$use_name]['quantity']['#default_value'] = $event['use_fees'][$use_name][0]->quantity;
            }
          }

          if (in_array($use_name, $uses_hours)) {
            $form['review']['operations']['use_fees'][$use_name]['hours'] = array(
              '#id' => 'event-review-operations-use_fees-' . $use_name . '-hours',
              '#type' => 'textfield',
              '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Hours',
              '#default_value' => NULL,
              '#size' => 16,
              '#min' => 0,
              '#required' => $uses_requirements[$use_name],
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'use_fee-hours',
                ),
              ),
              '#states' => $states,
            );

            if (!empty($event['use_fees'][$use_name][0])) {
              $form['review']['operations']['use_fees'][$use_name]['hours']['#default_value'] = event_workflow_convert_value_from_database_format($event['use_fees'][$use_name][0]->hours, 'currency');
            }
          }

          $form['review']['operations']['use_fees'][$use_name]['days'] = array(
            '#id' => 'event-review-operations-use_fees-' . $use_name . '-days',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Days',
            '#default_value' => NULL,
            '#min' => 0,
            '#size' => 16,
            '#required' => $uses_requirements[$use_name],
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'use_fee-days',
              ),
            ),
            '#states' => $states,
          );

          if (!empty($event['use_fees'][$use_name][0])) {
            $form['review']['operations']['use_fees'][$use_name]['days']['#default_value'] = event_workflow_convert_value_from_database_format($event['use_fees'][$use_name][0]->days, 'currency');
          }

          $form['review']['operations']['use_fees'][$use_name]['amount'] = array(
            '#id' => 'event-review-operations-use_fees-' . $use_name . '-amount',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Use Fee </span>Amount',
            '#default_value' => NULL,
            '#size' => 16,
            '#min' => 0,
            '#required' => $uses_requirements[$use_name],
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'use_fee-amount',
              ),
            ),
            '#states' => $states,
          );

          if (!empty($event['use_fees'][$use_name][0])) {
            $form['review']['operations']['use_fees'][$use_name]['amount']['#default_value'] = event_workflow_convert_value_from_database_format($event['use_fees'][$use_name][0]->amount, 'currency');
          }
          elseif (isset($use_fees_amount_defaults[$use_name])) {
            $form['review']['operations']['use_fees'][$use_name]['amount']['#default_value'] = $use_fees_amount_defaults[$use_name];
          }
        }

        if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
          if ($venue_coordinator) {
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
                  ':input[name="review[operations][presidents_approval]"]' => array('checked' => FALSE),
                  ':input[name="review[operations][university_business]"]' => array('checked' => FALSE),
                ),
                'invisible' => array(
                  'or' => array(
                    ':input[name="review[operations][presidents_approval]"]' => array('checked' => TRUE),
                    ':input[name="review[operations][university_business]"]' => array('checked' => TRUE),
                  ),
                ),
              ),
            );

            if (!empty($event['use_fees']['other'][0])) {
              $form['review']['operations']['use_fees']['other']['#default_value'] = $event['use_fees']['other'][0]->value;
            }
          }
        }

        $form['review']['operations']['insurance'] = array(
          '#tree' => TRUE,
        );

        if ($insurance_fee && $step == MEW_REVIEW_STEP_REVIEW) {
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

      if ($can_review_event && $step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
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
              ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
            ),
          ),
        );

        $form['review']['operations']['university_business'] = array(
          '#id' => 'event-review-operations-university_business',
          '#type' => 'checkbox',
          '#title' => "Strictly University Business",
          '#description' => "By selecting this checkbox you claim that this is strictly for university business and usage fees do not apply.",
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
              ':input[name="review[operations][decision]"]' => array('value' => MEW_REVIEW_DECISION_USAGE_FEE),
            ),
          ),
        );
      }

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
        '#name' => 'event-review-operations-submit-submit',
        '#type' => 'submit',
        '#default_value' => t("Submit"),
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
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param int $event_id
 *   The event request id.
 * @param string $group
 *   The group name.
 * @param array $presentation_tables
 *   An array of tables defining how the presentation is to be built.
 * @param array $show
 *   An array containing the following keys:
 *   'edit': When TRUE, the edit link will be shown in the group header.
 *   'history': When TRUE, the history link is made visible.
 * @param array $event
 *   The event settings array.
 *
 * @return int|null
 *   The row number or NULL when there is no valid row.
 *   FALSE is returned for in
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_setup_group_path(&$form, $event_id, $group, $presentation_tables, $show, $event) {
  $row = NULL;
  $group_path = &$form['display']['groups'];
  $event_type = $event['information']['type'][0]->value;

  $show_link = TRUE;
  if (isset($presentation_tables[$group]['event_type']['hide_link']) && is_array($presentation_tables[$group]['event_type']['hide_link'])) {
    if (in_array($event_type, $presentation_tables[$group]['event_type']['hide_link'])) {
      $show_link = FALSE;
    }
  }

  if ($show['edit']) {
    if (array_key_exists('editable', $presentation_tables[$group])) {
      $show['edit'] = $presentation_tables[$group]['editable'];
    }
  }

  if ($show['history']) {
    if ($group != 'request') {
      $show['history'] = FALSE;
    }
  }

  if ($show['clone']) {
    if ($group != 'request') {
      $show['clone'] = FALSE;
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
        if ($show['clone']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'events/create-0/' . $event_id . '" class="section-event-' . $event_id . '-' . $group . '-history section-event-clone no-print" title="'. t("Copy " . $presentation_tables[$group]['label']) . '">Copy</a>';
        }

        if ($show['edit']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'events/edit-0/' . $event_id . '/' . $group . '" class="section-event-' . $event_id . '-' . $group . '-edit section-event-edit no-print" title="'. t("Edit " . $presentation_tables[$group]['label']) . '">Edit</a>';
        }

        if ($show['history']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'events/history-0/' . $event_id . '" class="section-event-' . $event_id . '-' . $group . '-history section-event-history no-print" title="'. t("Edit " . $presentation_tables[$group]['label']) . '">History</a>';
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

  $decision = NULL;
  if (isset($form_state['values']['review']['operations']['decision'])) {
    $decision = $form_state['values']['review']['operations']['decision'];
  }

  $unrequire_usage_fields = TRUE;
  if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_REVIEW) {
    if ($decision == MEW_REVIEW_DECISION_USAGE_FEE) {
      $usage_fees_waved = FALSE;
      if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
        $usage_fees_waved = TRUE;
      }

      if (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
        $usage_fees_waved = TRUE;
      }

      $unrequire_usage_fields = $usage_fees_waved;
    }
  }

  if ($unrequire_usage_fields) {
    if (isset($form['review']['operations']['use_fees']) && is_array($form['review']['operations']['use_fees'])) {
      foreach ($form['review']['operations']['use_fees'] as $key => &$value) {
        if (is_array($value)) {
          if (isset($value['#required'])) {
            $value['#required'] = FALSE;
          }

          foreach ($value as $subkey => &$subvalue) {
            if (is_array($subvalue)) {
              if (isset($subvalue['#required'])) {
                $subvalue['#required'] = FALSE;
              }
            }
          }
        }
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
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $form_state['rebuild'] = TRUE;

  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $event = $form['form']['event']['#value'];
  $event_id = (int) $form['form']['event_id']['#value'];
  $mode = (int) $form['form']['mode']['#value'];
  $user = cf_current_user();

  $can_review_event = mcneese_event_workflow_management_page_event_access('review', $event_id);
  $can_comment_event = mcneese_event_workflow_management_page_event_access('comment', $event_id);

  $event_classification = $event['event_coordinator']['classification'][0]->value;
  $step = $event['top']['step'][0]->value;
  $status = $event['top']['status'][0]->value;

  $decision = NULL;
  if (isset($form_state['values']['review']['operations']['decision'])) {
    $decision = $form_state['values']['review']['operations']['decision'];
  }

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

    $reviewers = array();
    if ($mode < 0 && $mode > 3) {
      form_set_error('', "Illegal operation detected. Invalid mode selected.");
      $failure = TRUE;
    }
    elseif (!$can_review_event) {
      if ($decision == MEW_REVIEW_DECISION_COMMENT) {
        if ($can_comment_event) {
          unset($form_state['values']['review']['operations']['use_fees']);
          unset($form_state['values']['review']['operations']['insurance']);
        }
        else {
          form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
          $failure = TRUE;
        }
      }
      else {
        form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
        $failure = TRUE;
      }
    }

    // A request was made to have the fees waved options be displayed with the usage fees to avoid confusion.
    // To do this without changing the underlining design, when the usage fee is specified with a wave checked, then convert the decision to a wave.
    if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
      if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
        $form_state['values']['review']['operations']['decision'] = MEW_REVIEW_DECISION_WAVE;
        $decision = MEW_REVIEW_DECISION_WAVE;
      }
      elseif (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
        $form_state['values']['review']['operations']['decision'] = MEW_REVIEW_DECISION_WAVE;
        $decision = MEW_REVIEW_DECISION_WAVE;
      }
    }

    $fees_required = TRUE;
    if (isset($event['use_fees']['waved'][0]->value)) {
      if ($event['use_fees']['waved'][0]->value == 1) {
        $fees_required = FALSE;
      }
    }

    if (isset($event['use_fees']['university'][0]->value)) {
      if ($event['use_fees']['university'][0]->value == 1) {
        $fees_required = FALSE;
      }
    }

    if (!$fees_required) {
      if (isset($form_state['values']['review']['operations']['use_fees'])) {
        unset($form_state['values']['review']['operations']['use_fees']);
      }
    }

    if ($step == MEW_REVIEW_STEP_REVIEW && $decision == MEW_REVIEW_DECISION_USAGE_FEE && $fees_required) {
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

        if (isset($use_fees['facilities']['quantity']) && intval($use_fees['facilities']['quantity']) != $use_fees['facilities']['quantity']) {
          form_set_error('review][operations][use_fees][facilities][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $use_fees['facilities']['quantity'])));
          $failure = TRUE;
        }

        if (isset($use_fees['facilities']['hours']) && !cf_is_integer($use_fees['facilities']['hours'])) {
          form_set_error('review][operations][use_fees][facilities][hours', t("@hours is not an valid number. Use only numbers places.", array('@hours' => $use_fees['facilities']['hours'])));
          $failure = TRUE;
        }

        if (isset($use_fees['facilities']['days']) && intval($use_fees['facilities']['days']) != $use_fees['facilities']['days']) {
          form_set_error('review][operations][use_fees][facilities][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $use_fees['facilities']['days'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('equipment', $use_fees) && !empty($use_fees['equipment']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $use_fees['equipment']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][use_fees][equipment][amount', t("@equipment is not a valid additional equipment usage fee. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@equipment' => $use_fees['equipment']['amount'])));
          $failure = TRUE;
        }

        if (isset($use_fees['equipment']['quantity']) && intval($use_fees['equipment']['quantity']) != $use_fees['equipment']['quantity']) {
          form_set_error('review][operations][use_fees][equipment][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $use_fees['equipment']['quantity'])));
          $failure = TRUE;
        }

        if (isset($use_fees['equipment']['hours']) && !cf_is_integer($use_fees['equipment']['hours'])) {
          form_set_error('review][operations][use_fees][equipment][hours', t("@hours is not an valid number. Use only numbers places.", array('@hours' => $use_fees['equipment']['hours'])));
          $failure = TRUE;
        }

        if (isset($use_fees['equipment']['days']) && intval($use_fees['equipment']['days']) != $use_fees['equipment']['days']) {
          form_set_error('review][operations][use_fees][equipment][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $use_fees['equipment']['days'])));
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

        if (isset($use_fees['custodial']['quantity']) && intval($use_fees['custodial']['quantity']) != $use_fees['custodial']['quantity']) {
          form_set_error('review][operations][use_fees][custodial][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $use_fees['custodial']['quantity'])));
          $failure = TRUE;
        }

        if (isset($use_fees['custodial']['hours']) && !cf_is_integer($use_fees['custodial']['hours'])) {
          form_set_error('review][operations][use_fees][custodial][hours', t("@hours is not an valid number. Use only numbers places.", array('@hours' => $use_fees['custodial']['hours'])));
          $failure = TRUE;
        }

        if (isset($use_fees['custodial']['days']) && intval($use_fees['custodial']['days']) != $use_fees['custodial']['days']) {
          form_set_error('review][operations][use_fees][custodial][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $use_fees['custodial']['days'])));
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

        if (isset($use_fees['security']['quantity']) && intval($use_fees['security']['quantity']) != $use_fees['security']['quantity']) {
          form_set_error('review][operations][use_fees][security][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $use_fees['security']['quantity'])));
          $failure = TRUE;
        }

        if (isset($use_fees['security']['hours']) && !cf_is_integer($use_fees['security']['hours'])) {
          form_set_error('review][operations][use_fees][security][hours', t("@hours is not an valid number. Use only numbers places.", array('@hours' => $use_fees['security']['hours'])));
          $failure = TRUE;
        }

        if (isset($use_fees['security']['days']) && intval($use_fees['security']['days']) != $use_fees['security']['days']) {
          form_set_error('review][operations][use_fees][security][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $use_fees['security']['days'])));
          $failure = TRUE;
        }
      }

      unset($use_fees);
    }

    if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_APPROVE) {
      $fees_not_supplied = TRUE;
      if (isset($event['use_fees']['waved'][0]->value)) {
        if ($event['use_fees']['waved'][0]->value) {
          $fees_not_supplied = FALSE;
        }
      }

      if (isset($event['use_fees']['university'][0]->value)) {
        if ($event['use_fees']['university'][0]->value) {
          $fees_not_supplied = FALSE;
        }
      }

      if (isset($event['use_fees']['facilities'][0]->days)) {
        $fees_not_supplied = FALSE;
      }

      if ($fees_not_supplied) {
        form_set_error('review][operations][decision', t("The event cannot be approved until the usage fees are either specified or waved."));
      }
    }

    if ($step == MEW_REVIEW_STEP_REVIEW && $decision == MEW_REVIEW_DECISION_APPROVE) {
      $fees_not_supplied = TRUE;
      if (isset($event['use_fees']['waved'][0]->value)) {
        if ($event['use_fees']['waved'][0]->value) {
          $fees_not_supplied = FALSE;
        }
      }

      if (isset($event['use_fees']['university'][0]->value)) {
        if ($event['use_fees']['university'][0]->value) {
          $fees_not_supplied = FALSE;
        }
      }

      $custodial_fees_required = FALSE;
      if (isset($form['form']['use_fees-custodial']['#value'])) {
        $custodial_fees_required = $form['form']['use_fees-custodial']['#value'];
      }

      $security_fees_required = FALSE;
      if (isset($form['form']['use_fees-custodial']['#value'])) {
        $security_fees_required = $form['form']['use_fees-security']['#value'];
      }

      $insurance_fees_required = FALSE;
      if (isset($form['form']['use_fees-custodial']['#value'])) {
        $insurance_fees_required = $form['form']['use_fees-insurance']['#value'];
      }

      if (!$custodial_fees_required && !$security_fees_required && !$insurance_fees_required) {
        $fees_not_supplied = FALSE;
      }

      if (isset($form['review']['operations']['use_fees']['custodial'])) {
        if (isset($event['use_fees']['custodial'][0]->amount)) {
          $fees_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['use_fees']['security'])) {
        if (isset($event['use_fees']['security'][0]->amount)) {
          $fees_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['insurance']['contractor'])) {
        if (isset($event['insurance']['contractor'][0]->value)) {
          $fees_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['insurance']['unaffiliated'])) {
        if (isset($event['insurance']['unaffiliated'][0]->value)) {
          $fees_not_supplied = FALSE;
        }
      }

      if ($fees_not_supplied) {
        form_set_error('review][operations][decision', t("The event cannot be approved until the usage fees are specified."));
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
    $success = TRUE;

    mcneese_event_workflow_include(6);
    $decision = $form_state['values']['review']['operations']['decision'];
    $message = $form_state['values']['review']['operations']['message'];

    $event = $form['form']['event']['#value'];

    $event_classification = $event['event_coordinator']['classification'][0]->value;
    $step = $event['top']['step'][0]->value;

    $extra = array();
    if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_REVIEW) {
      if (isset($form_state['values']['review']['operations']['use_fees'])) {
        $extra['use_fees'] = $form_state['values']['review']['operations']['use_fees'];
      }

      if (isset($form_state['values']['review']['operations']['insurance'])) {
        $extra['insurance'] = $form_state['values']['review']['operations']['insurance'];
      }
    }

    if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
      if (isset($form_state['values']['review']['operations']['presidents_approval'])) {
        $extra['use_fees']['waved'] = $form_state['values']['review']['operations']['presidents_approval'];
      }

      if (isset($form_state['values']['review']['operations']['university_business'])) {
        $extra['use_fees']['university'] = $form_state['values']['review']['operations']['university_business'];
      }
    }

    if ($success) {
      $success = mcneese_event_workflow_workflow_step_review($event_id, $form['form']['user']['#value'], $step, $decision, $message, $extra);
    }

    if ($success) {
      drupal_set_message("Your review or comment has been recieved.");
    }
    else {
      form_set_error('form', "An error occurred while trying to submit the event review. Please contact the support staff.");
      watchdog(MEW_WATCHDOG_ID, "An error occured while trying to submit the event review.", array(), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }

    // change to the event view page on submit for these decisions to prevent potential access denied messages following the submit process.
    if ($decision == MEW_REVIEW_DECISION_APPROVE || $decision == MEW_REVIEW_DECISION_DENY) {
      $form_state['values']['redirect_to'] = 'events/view-0/' .  $event_id;
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
    watchdog(MEW_WATCHDOG_ID, "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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
    watchdog(MEW_WATCHDOG_ID, "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }

  mcneese_event_workflow_include(0);

  $presentation = mcneese_event_workflow_table_presentation();

  mcneese_event_workflow_include(4);

  $decision_options = event_workflow_get_reviewer_decision_list_options();

  // review revisions are neither used nor incremented in the same manner as other fields.
  // the revision number will not increment when a new review is made.
  $review_revision = mcneese_event_workflow_load_event_revision_number($event_id, 'mew_field_review_review');

  if ($review_revision === FALSE || is_null($review_revision)) {
    $review_revision = 0;
  }

  try {
    $query = db_select('mew_field_review_review', 'mfrr');

    $query->fields('mfrr');

    $query->condition('mfrr.event_id', $event_id);
    #$query->condition('mfrr.revision', $review_revision);

    $query->orderBy('mfrr.date');

    $reviews = $query->execute()->fetchAll();

    if (is_array($reviews)) {
      foreach ($reviews as $review) {
        $query = db_select('mew_review_classifications', 'mrc');

        $query->fields('mrc');
        $query->condition('mrc.review_id', $review->id);

        $review_classifications = $query->execute()->fetchAll();

        $review->reviewer_classifications = array();
        if (is_array($review_classifications)) {
          foreach ($review_classifications as $review_classification) {
            // some classifications are not allowed in certain steps.
            if ($review->step == MEW_REVIEW_STEP_REVIEW ||$review->step == MEW_REVIEW_STEP_USAGE_FEES) {
              if ($review_classification->classification == MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR) {
                continue;
              }
              elseif ($review_classification->classification == MEW_REVIEW_CLASSIFICATION_SYSTEM_ADMINISTRATOR) {
                continue;
              }
              elseif ($review_classification->classification == MEW_REVIEW_CLASSIFICATION_MANAGER) {
                continue;
              }
              elseif ($review_classification->classification == MEW_REVIEW_CLASSIFICATION_FAILSAFE_COORDINATOR) {
                continue;
              }
            }

            $review->reviewer_classifications[$review_classification->classification] = $review_classification->classification;
          }
        }
      }
    }
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
    'university',
  );
  $changelog_fields['insurance'] = array(
    'insurance',
    'unaffiliated',
  );

  $users = array();
  $reviewers = array();
  $has_log = FALSE;
  if (is_array($reviews)) {
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

      $markup = '<div class="review-item review-item-' . $row. '">';

      $user_name = '';
      if (is_null($users[$user_id])) {
        // @todo: this is an error, throw a watchdog message.
        continue;
      }

      $reviewer_classification = NULL;
      foreach ($review->reviewer_classifications as $classification) {
        if (is_null($reviewer_classification)) {
          $reviewer_classification = '';
        }
        else {
          $reviewer_classification .= ', ';
        }

        if ($classification == MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR) {
          $reviewer_classification .= "Venue Coordinator";
        }
        elseif ($classification == MEW_REVIEW_CLASSIFICATION_MANAGER) {
          $reviewer_classification .= "Manager";
        }
        elseif ($classification == MEW_REVIEW_CLASSIFICATION_SYSTEM_ADMINISTRATOR) {
          $reviewer_classification .= "System Administrator";
        }
        else {
          $reviewer_classification .= $reviewer_classification_options[$classification];
        }
      }

      if (!is_null($reviewer_classification)) {
        $reviewer_classification = ' as <strong class="review-item-label-text-row-reviewer_classification">'. $reviewer_classification . '</strong>';
      }

      $user_name .= $users[$user_id]->field_user_first_name['und'][0]['value'];
      $user_name .= ' ' . $users[$user_id]->field_user_last_name['und'][0]['value'];

      $user_name_markup = '<strong class="review-item-label-text-row-user">';
      $user_name_markup .= $user_name;
      $user_name_markup .= '</strong>';

      $user_id_markup = '<span class="updater_id">[id: ' . $user_id . ']</span>';

      $row_id_block = '<div class="review-item-label-text-row">[' . $row . ']</div> ';

      $markup .= '<div class="review-item-label">';
      $markup .= '<h3 class="review-item-label-text">' . $row_id_block . '<div class="review-item-label-text-title">';

      if ($decision == MEW_REVIEW_DECISION_AMENDMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">' . "Amended" . '</strong> the event request' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Unavailable</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_MAKE_DECISIONS && $decision == MEW_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Confirmed and Reserved</strong> the request' . $reviewer_classification . "";
      }
      elseif ($review_step == MEW_REVIEW_STEP_MAKE_DECISIONS && $decision == MEW_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Denied</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MEW_REVIEW_DECISION_COMMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has made a <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_REVIEW && $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_REVIEW && $decision == MEW_REVIEW_DECISION_WAVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has waved the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> step' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_USAGE_FEES && $decision == MEW_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has accepted the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MEW_REVIEW_STEP_USAGE_FEES && $decision == MEW_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has denied the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      else {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' chose to <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> the event request' . $reviewer_classification . '.';
      }
      $markup .= '</div></h3>';
      $markup .= '<div class="review-item-label-date">' . date("Y/m/d h:i:s a", $date) . "</div>";
      $markup .= '</div>';


      // provide additional changelog information.
      if ($decision == MEW_REVIEW_DECISION_AMENDMENT) {
        $message .= '<div class="review-item-message-decision-amendment review-item-message-decision">';
        $message .= 'The review step has been reset and all prior reviews no longer apply.';
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['facilities'])) {
        $message .= '<div class="review-item-message-use_fees-facilities review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Facilities Use Fees:</div> ';

        $value = reset($changelog['use_fees']['facilities']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'facilities', 0, $value, $presentation['use_fees']['facilities'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['equipment'])) {
        $message .= '<div class="review-item-message-use_fees-equipment review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Additional Equipment Use Fees:</div> ';

        $value = reset($changelog['use_fees']['equipment']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'equipment', 0, $value, $presentation['use_fees']['equipment'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['security'])) {
        $message .= '<div class="review-item-message-use_fees-security review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Security Use Fees:</div> ';

        $value = reset($changelog['use_fees']['security']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'security', 0, $value, $presentation['use_fees']['security'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['custodial'])) {
        $message .= '<div class="review-item-message-use_fees-custodial review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Custodial Use Fees:</div> ';

        $value = reset($changelog['use_fees']['custodial']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'custodial', 0, $value, $presentation['use_fees']['custodial'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['use_fees']['other'])) {
        $message .= '<div class="review-item-message-use_fees-other review-item-message-use_fees review-item-message-fees">';
        $message .= '<div class="item-label">Other Use Fees:</div> ';

        // always display the boolean when displayed in the review log.
        foreach ($presentation['use_fees']['other']['display'] as &$value) {
          if ($value == 'when_true' || $value == 'when_false') {
            $value = 'raw';
          }
        }

        $value = reset($changelog['use_fees']['other']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'other', 0, $value, $presentation['use_fees']['other'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if ($review_step == MEW_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MEW_REVIEW_DECISION_USAGE_FEE || $decision == MEW_REVIEW_DECISION_WAVE)) {
        if (!empty($changelog['use_fees']['waved'])) {
          $message .= '<div class="review-item-message-use_fees-waved review-item-message-use_fees review-item-message-fees">';
          $message .= '<div class="item-label">Waved with Presidents Approval:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['use_fees']['waved']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['use_fees']['waved']);
          $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'waved', 0, $value, $presentation['use_fees']['waved'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }

        if (!empty($changelog['use_fees']['university'])) {
          $message .= '<div class="review-item-message-use_fees-university review-item-message-university review-item-message-fees">';
          $message .= '<div class="item-label">Strictly University Business:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['use_fees']['university']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['use_fees']['university']);
          $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'use_fees', 'university', 0, $value, $presentation['use_fees']['university'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }
      }

      if (!empty($changelog['insurance']['contractor'])) {
        $message .= '<div class="review-item-message-insurance-contractor review-item-message-insurance review-item-message-fees">';
        $message .= '<div class="item-label">Contractor Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        foreach ($presentation['insurance']['contractor']['display'] as &$value) {
          if ($value == 'when_true' || $value == 'when_false') {
            $value = 'raw';
          }
        }

        $value = reset($changelog['insurance']['contractor']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'insurance', 'contractor', 0, $value, $presentation['insurance']['contractor'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['insurance']['unaffiliated'])) {
        $message .= '<div class="review-item-message-insurance-unaffiliated review-item-message-insurance review-item-message-fees">';
        $message .= '<div class="item-label">Unaffiliated Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        foreach ($presentation['insurance']['unaffiliated']['display'] as &$value) {
          if ($value == 'when_true' || $value == 'when_false') {
            $value = 'raw';
          }
        }

        $value = reset($changelog['insurance']['unaffiliated']);
        $value_markup = mcneese_event_workflow_build_event_request_value_structure($event_id, 'insurance', 'unaffiliated', 0, $value, $presentation['insurance']['unaffiliated'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
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
