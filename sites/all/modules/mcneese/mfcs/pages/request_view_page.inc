<?php

/**
 * @file
 * Defines McNeese facilities use request view page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request view page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for an request.
 * @param int $mode
 *   Specifies the mode that alters how the form is rendered.
 * @param bool $pdf
 *   (optional) Enable special handling for PDF generation.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_view_0_page($form, &$form_state, $request_id, $mode = MFCS_VIEW_MODE_NORMAL, $pdf = FALSE) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_numeric($mode) || $mode < MFCS_VIEW_MODE_NORMAL || $mode > MFCS_VIEW_MODE_OPERATIONS) {
    $mode = MFCS_VIEW_MODE_NORMAL;
  }
  else {
    $mode = (int) $mode;
  }

  if (!is_bool($pdf)) {
    $pdf = FALSE;
  }

  // if cancelling the request, return only the cancellation confirmation form.
  if (isset($form_state['cancel_was_clicked'])) {
    mfcs_include(5);

    $cancel_form = array();
    $cancel_form_state = array();
    return mfcs_request_cancel_0_form($cancel_form, $cancel_form_state, $request_id);
  }
  // if uncancelling the request, return only the uncancellation confirmation form.
  elseif (isset($form_state['uncancel_was_clicked'])) {
    mfcs_include(5);

    $cancel_form = array();
    $cancel_form_state = array();
    return mfcs_request_uncancel_0_form($cancel_form, $cancel_form_state, $request_id);
  }

  $user = cf_current_user();

  $manager = user_access('mfcs manage');
  $reviewer = user_access('mfcs review');
  $requester = user_access('mfcs request');

  $venue_coordinator = ($request['venue_coordinator']['user_id'][0]->value == $user->uid);

  $can_create = mfcs_management_page_request_access('create');

  $can_create = mfcs_management_page_request_access('create');
  $can_edit = mfcs_management_page_request_access('edit');
  $can_view = mfcs_management_page_request_access('view');
  $can_list = mfcs_management_page_request_access('list');
  $can_search = mfcs_management_page_request_access('search');
  $can_delete = mfcs_management_page_request_access('delete');
  $can_log = mfcs_management_page_request_access('log');
  $can_review = mfcs_management_page_request_access('review');

  $can_edit_request = mfcs_management_page_request_access('edit', $request_id);
  $can_log_request = mfcs_management_page_request_access('log', $request_id);
  $can_review_request = mfcs_management_page_request_access('review', $request_id);
  $can_comment_request = mfcs_management_page_request_access('comment', $request_id);
  $can_history_request = mfcs_management_page_request_access('history', $request_id);
  $can_agreement_request = mfcs_management_page_request_access('agreement', $request_id);

  $show_display = TRUE;
  $show_log = TRUE;
  $show_operations = TRUE;
  if ($mode == MFCS_VIEW_MODE_DISPLAY) {
    $show_log = FALSE;
    $show_operations = FALSE;
  }
  elseif ($mode == MFCS_VIEW_MODE_LOG) {
    $show_display = FALSE;
    $show_operations = FALSE;
  }
  elseif ($mode == MFCS_VIEW_MODE_OPERATIONS) {
    $show_display = FALSE;
    $show_log = FALSE;
  }

  if (!$can_log_request) {
    $show_log = FALSE;

    if ($mode == MFCS_VIEW_MODE_LOG) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }
  }

  if ($can_review_request || $can_comment_request) {
    // show review operations only if the content is reviewable by the current user.
    $request_classification = $request['request_coordinator']['classification'][0]->value;
    $step = $request['top']['step'][0]->value;
    $status = $request['top']['status'][0]->value;

    // only show operations for states and steps that make sense to.
    $allowed_statuses = array(
      MFCS_EVENT_STATUS_LOCKED,
      MFCS_EVENT_STATUS_UNLOCKED,
    );

    if (!in_array($status, $allowed_statuses)) {
      $show_operations = FALSE;

      if ($mode == MFCS_VIEW_MODE_OPERATIONS) {
        drupal_access_denied();
        drupal_exit();
        return array();
      }
    }

    $allowed_steps = array(
      MFCS_REVIEW_STEP_VENUE_AVAILABLE,
      MFCS_REVIEW_STEP_REVIEW,
      MFCS_REVIEW_STEP_REQUIREMENTS,
      MFCS_REVIEW_STEP_MAKE_DECISIONS,
    );

    if (!in_array($step, $allowed_steps)) {
      $show_operations = FALSE;

      if ($mode == MFCS_VIEW_MODE_OPERATIONS) {
        drupal_access_denied();
        drupal_exit();
        return array();
      }
    }
  }
  else {
    $show_operations = FALSE;

    if ($mode == MFCS_VIEW_MODE_OPERATIONS) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }
  }

  $can = array(
    'edit' => FALSE,
    'history' => FALSE,
    'agreement' => FALSE,
  );

  if ($pdf) {
    $can['edit'] = FALSE;
    $can['history'] = FALSE;
    $can['agreement'] = FALSE;
    $can['clone'] = FALSE;
  }
  else {
    $can['edit'] = (bool) $can_edit_request;
    $can['history'] = (bool) $can_history_request;
    $can['agreement'] = (bool) $can_agreement_request;
    $can['clone'] = (bool) $can_create;
  }

  $page_title = "Facilities Use Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Facilities Use Request: " . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);


  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
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

  if (!isset($request['information']['type'][0]->value) || !cf_is_integer($request['information']['type'][0]->value)) {
    cf_error::invalid_integer('request[information][type][0]->value');
    $form['message'] = array(
      '#markup' => "Error: Unable to load request (ID = " . check_plain($request_id) . ") due to missing request type value.",
    );

    return $form;
  }

  $request_classification = $request['request_coordinator']['classification'][0]->value;
  $request_type = $request['information']['type'][0]->value;

  if ($show_display) {
    $form['display'] = array(
      '#id' => 'request-' . $request_id . '-section-display',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Request Details",
      '#attributes' => array(
        'class' => array(
          'request-section-display',
          'mode-' . $mode,
          'fieldset_print_as_div',
        ),
      ),
      '#tree' => TRUE,
    );

    if ($pdf) {
      unset($form['display']['#type']);
      unset($form['display']['#collapsible']);
      unset($form['display']['#collapsed']);

      $form['display']['log']['#type'] = 'container';
      $form['display']['log']['#attributes']['id'] = 'request-' . $request_id . '-section-display';
    }

    $form['display']['groups'] = array(
      '#prefix' => '<div id="request-' . $request_id . '-field_groups" class="request-field_groups mode-' . $mode . '">',
      '#suffix' => '</div>',
    );

    $form['groups'] = array();
    $form['groups'] = array(
      '#tree' => TRUE,
    );

    $fields_count = array('top' => 0, 'group' => array());

    mfcs_include(0);

    $presentation = mfcs_table_presentation();
    $presentation_tables = mfcs_table_presentation(1);

    $collate = array();

    foreach ($presentation_tables as $field_group => $presentation_table) {
      mfcs_request_view_0_page_setup_group_path($form, $request_id, $field_group, $presentation_tables, $can, $request);
    }

    // pre-process special case of review decisions.
    $decision_presentation = $presentation['reviews']['decision'];
    unset($presentation['reviews']['decision']);

    $reviews = mfcs_get_latest_reviews($request_id, array(MFCS_REVIEW_STEP_REVIEW, MFCS_REVIEW_STEP_MAKE_DECISIONS), array(MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY));

    if (!empty($reviews)) {
      mfcs_include(4);

      $classification_options = mfcs_get_reviewer_classification_list_options(NULL, TRUE);
      $preprocessed_decisions = array();

      $reviews_step = array();
      if (isset($reviews[MFCS_REVIEW_STEP_REVIEW])) {
        $reviews_step = $reviews[MFCS_REVIEW_STEP_REVIEW];
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

          $request['reviews'][$key_string] = $preprocessed_decisions[$key];
          $weight++;
        }
      }

      // display the final decision last.
      $decision_step = array();
      if (isset($reviews[MFCS_REVIEW_STEP_MAKE_DECISIONS])) {
        $decision_step = $reviews[MFCS_REVIEW_STEP_MAKE_DECISIONS];
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

          $request['reviews'][$key_string] = array(0 => $decision);
          $weight++;
          break;
        }
      }

      unset($classification_options);
    }
    unset($reviews);

    // build the presentation tables.
    foreach ($request as $field_group => $request_array) {
      foreach (array('request_type' => $request_type, 'request_classification' => $request_classification) as $show_hide_type => $show_hide_value) {
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
        $row = mfcs_request_view_0_page_setup_group_path($form, $request_id, $field_group, $presentation_tables, $can, $request);
      }

      if (!array_key_exists($field_group, $form['groups'])) {
        $form['groups'][$field_group] = array(
          '#tree' => TRUE,
        );
      }

      foreach ($request_array as $field_name => $values) {
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

          foreach (array('request_type' => $request_type, 'request_classification' => $request_classification) as $show_hide_type => $show_hide_value) {
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
              $custom_row = mfcs_request_view_0_page_setup_group_path($form, $request_id, $group, $presentation_tables, $can, $request);
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

          $group_path[$group][$weight][$field_presentation_name] = mfcs_build_request_item_structure($request_id, $group, $field_presentation_name, $values, $field_presentation, $request);

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
        '#id' => 'request-display-submit',
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-display-submit',
            'mode-' . $mode,
          ),
        ),
        '#weight' => 1000,
      );

      if (mfcs_management_page_request_access('cancel', $request_id)) {
        $form['display']['submit']['cancel'] = array(
          '#id' => 'request-display-submit-cancel',
          '#name' => 'request-display-submit-cancel',
          '#type' => 'submit',
          '#default_value' => t("Cancel"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'request-display-submit-cancel',
              'mode-' . $mode,
            ),
          ),
          '#submit' => array(
            'mfcs_request_cancel_0_form_submit',
          ),
        );
      }
      elseif (mfcs_management_page_request_access('uncancel', $request_id)) {
        $form['display']['submit']['uncancel'] = array(
          '#id' => 'request-display-submit-uncancel',
          '#name' => 'request-display-submit-uncancel',
          '#type' => 'submit',
          '#default_value' => t("Uncancel"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'request-display-submit-uncancel',
              'mode-' . $mode,
            ),
          ),
          '#submit' => array(
            'mfcs_request_uncancel_0_form_submit',
          ),
        );
      }

      $form['display']['submit']['print'] = array(
        '#id' => 'request-display-submit-print',
        '#name' => 'request-display-submit-print',
        '#type' => 'submit',
        '#default_value' => t("Print"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-display-submit-print',
            'mode-' . $mode,
          ),
          'onclick' => 'window.print(); return false;',
        ),
      );

      $form['display']['submit']['download'] = array(
        '#id' => 'request-display-submit-download',
        '#name' => 'request-display-submit-download',
        '#type' => 'submit',
        '#default_value' => t("Download"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-display-submit-download',
            'mode-' . $mode,
          ),
        ),
        '#submit' => array(
          'mfcs_request_view_0_page_submit_pdf',
        ),
      );
    }
  }

  if ($show_log || $show_operations) {
    $form['review'] = array(
      '#tree' => TRUE,
    );

    mfcs_include(4);

    $step_options = mfcs_get_request_steps_list_options();

    if ($show_log) {
      $form['review']['log'] = array(
        '#id' => 'request-' . $request_id . '-section-review-log',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => "Review Log",
        '#attributes' => array(
          'class' => array(
            'request-section-review-log',
            'mode-' . $mode,
            'fieldset_print_as_div',
          ),
        ),
        '#tree' => TRUE,
      );

      $log_page_title_css = '';
      if (!$pdf) {
        $log_page_title_css = 'no-screen no-braille';
      }

      $form['review']['log']['page_title'] = array(
        '#markup' => '<div id="' . 'request-' . $request_id . '-section-review-log-page_title' . '" class="request-section-review-log-page_title ' . $log_page_title_css . '">' . $page_title . '</div>',
        '#weight' => 0,
      );

      if ($pdf) {
        unset($form['review']['log']['#type']);
        unset($form['review']['log']['#collapsible']);
        unset($form['review']['log']['#collapsed']);

        $form['review']['log']['#type'] = 'container';
        $form['review']['log']['#attributes']['id'] = 'request-' . $request_id . '-section-review-log';
      }

      $form['review']['log']['header'] = array(
        '#markup' => '<h2 id="request-' . $request_id . '-section-review-log-header" class="request-section-review-log-header">Review Log</h2>',
        '#weight' => 1,
      );

      $reviewers_yet_to_review = mfcs_get_reviewers_yet_to_review($request_id);

      $yet_to_review_markup = '';
      if (!empty($reviewers_yet_to_review)) {
        $yet_to_review_markup .= '<div id="request-' . $request_id . '-section-review-log-yet_to_review" class="request-section-review-log-yet_to_review">';
        $yet_to_review_markup .= '<div class="request-section-review-log-yet_to_review-label">Reviewer classifications that have not yet reviewed:</div>';
        $yet_to_review_markup .= '<ul class="request-section-review-log-yet_to_review-list">';

        foreach ($reviewers_yet_to_review as $yet_to_review_classification) {
          $yet_to_review_markup .= '<li class="request-section-review-log-yet_to_review-list_item">' . $yet_to_review_classification . '</li>';
        }

        $yet_to_review_markup .= '</ul>';
      }

      $form['review']['log']['yet_to_review'] = array(
        '#markup' => $yet_to_review_markup,
        '#weight' => 2,
      );

      $form['review']['log']['history'] = array(
        '#prefix' => '<div id="request-' . $request_id . '-section-review-log-history" class="request-section-review-log-history mode-' . $mode . '">',
        '#suffix' => '</div>',
        '#weight' => 3,
      );

      mfcs_build_reviews_log($form, $form_state, $request_id);

      $form['review']['log']['submit'] = array(
        '#id' => 'request-review-log-submit',
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-review-log-submit',
            'mode-' . $mode,
          ),
        ),
        '#weight' => 1000,
      );

      $form['review']['log']['submit']['print'] = array(
        '#id' => 'request-review-log-submit-print',
        '#name' => 'request-review-log-submit-print',
        '#type' => 'submit',
        '#default_value' => t("Print"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-review-log-submit-print',
            'mode-' . $mode,
          ),
          'onclick' => 'window.print(); return false;',
        ),
      );

      $form['review']['log']['submit']['download'] = array(
        '#id' => 'request-review-log-submit-download',
        '#name' => 'request-review-log-submit-download',
        '#type' => 'submit',
        '#default_value' => t("Download"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-review-log-submit-download',
            'mode-' . $mode,
          ),
        ),
        '#submit' => array(
          'mfcs_request_view_0_page_submit_pdf',
        ),
      );
    }

    if ($show_operations && !$pdf) {
      $decision_options = mfcs_get_reviewer_decision_list_options('review_step');

      $form['review']['operations'] = array(
        '#id' => 'request-' . $request_id . '-section-review-operations',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => "Review Operations",
        '#attributes' => array(
          'class' => array(
            'request-section-review-operations',
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
        '#markup' => '<h2 id="request- ' . $request_id . '-section-review-operations-header" class="request-section-review-operations-header">Review Operations</h2>',
        '#weight' => 0,
      );

      $form['review']['operations']['instructions'] = array(
        '#markup' => '',
        '#weight' => 0,
      );

      $form['review']['operations']['instructions']['#markup'] .= "The current step is: <strong>" . $step_options[$step] . "</strong>. ";

      $form['review']['operations']['as_reviewer'] = array(
        '#markup' => '',
        '#weight' => 0,
      );

      $form['review']['operations']['requirements'] = array(
        '#markup' => '',
        '#weight' => 0,
      );

      mfcs_include(4);
      $reviewer_classification_options = mfcs_get_reviewer_classification_list_options();

      $reviewer_conditions = array(
        'mr.user_id' => $user->uid,
        'mr.request_classification' => $request_classification,
        'mr.review_step' => $step,
        'mr.disabled' => 0,
      );

      $reviewers = mfcs_get_reviewers($reviewer_conditions, TRUE);
      if (!is_array($reviewers)) {
        $reviewers = array();
      }

      $form['review']['operations']['as_reviewer']['#markup'] = '<div class="request-section-review-operations-as_reviewer">';

      $reviewer_classification = array();
      if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
        if ($venue_coordinator) {
          $reviewer_classification[MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
        }
      }
      elseif ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS) {
        foreach ($reviewers as $reviewer) {
          if (isset($reviewer->review_step) && $reviewer->review_step == $step) {
            if (isset($reviewer->reviewer_classification)) {
              $reviewer_classification[$reviewer->reviewer_classification] = $reviewer_classification_options[$reviewer->reviewer_classification];
            }
          }
        }
      }

      if ($can_comment_request) {
        if (empty($reviewer_classification)) {
          if ($manager) {
            $reviewer_classification[MFCS_REVIEW_CLASSIFICATION_MANAGER] = 'Manager';
          }
        }
      }

      if (!empty($reviewer_classification)) {
        $form['review']['operations']['as_reviewer']['#markup'] .= '<div class="request-section-review-operations-as_reviewer-message">You are reviewing with the following classifications:</div>';
        $form['review']['operations']['as_reviewer']['#markup'] .= theme('item_list', array('items' => $reviewer_classification, 'type' => 'ul', 'attributes' => array('class' => array('request-section-review-operations-as_reviewer-list'))));
      }

      $form['review']['operations']['as_reviewer']['#markup'] .= '</div>';

      // make the 'requirement' step available only under very specific situations.
      if ($step != MFCS_REVIEW_STEP_VENUE_AVAILABLE && $step != MFCS_REVIEW_STEP_REVIEW) {
        unset($decision_options[MFCS_REVIEW_DECISION_REQUIREMENT]);
      }

      // restrict options to commenting only, if the user is only allowed to comment.
      if (!$can_review_request && $can_comment_request) {
        $decision_options = array(
          MFCS_REVIEW_DECISION_COMMENT => $decision_options[MFCS_REVIEW_DECISION_COMMENT],
        );
      }

      $form['review']['operations']['decision'] = array(
        '#id' => 'request-review-operations-decision',
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
        '#weight' => 1,
      );

      if ($can_review_request && ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_VENUE_AVAILABLE)) {
        $form['review']['operations']['requirements'] = array(
          '#tree' => TRUE,
          '#weight' => 2,
        );

        $form['form']['requirements-custodial'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $form['form']['requirements-security'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $form['form']['requirements-insurance'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $uses = array();
        $insurance_requirement = FALSE;
        if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
          if ($venue_coordinator) {
            $uses['facilities'] = "Facilities";
            $uses['equipment'] = "Additional Equipment";
          }
        }
        else {
          foreach ($reviewers as $reviewer) {
            if ($reviewer->reviewer_classification == MFCS_REVIEW_CLASSIFICATION_FACILITIES) {
              $uses['custodial'] = "Custodial";
              $form['form']['requirements-custodial']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MFCS_REVIEW_CLASSIFICATION_SECURITY) {
              $uses['security'] = "Security";
              $form['form']['requirements-security']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MFCS_REVIEW_CLASSIFICATION_PURCHASING) {
              $insurance_requirement = TRUE;
              $form['form']['requirements-insurance']['#value'] = TRUE;
            }
          }
        }

        // if the reviewer has no requirements or insurance requirements to specify, then do not display the Requirements select option.
        if (!$insurance_requirement && empty($uses)) {
          unset($decision_options[MFCS_REVIEW_DECISION_REQUIREMENT]);
          $form['review']['operations']['decision']['#options'] = $decision_options;
        }

        $requirements_amount_defaults = array('custodial' => '18.50', 'security' => '30.00');
        $uses_personnel = array('custodial', 'security');
        $uses_hours = array('custodial', 'security');
        $uses_requirements = array('facilities' => TRUE, 'equipment' => FALSE, 'custodial' => TRUE, 'security' => TRUE);
        $uses_review = array('facilities' => FALSE, 'equipment' => FALSE, 'custodial' => TRUE, 'security' => TRUE);
        foreach ($uses as $use_name => $use_title) {
          if ($uses_review[$use_name] && $step != MFCS_REVIEW_STEP_REVIEW) {
            continue;
          }

          $states = array(
            'visible' => array(
              ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
            ),
          );

          if (!$uses_review[$use_name] && $step != MFCS_REVIEW_STEP_REVIEW) {
            $states = array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
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

          $form['review']['operations']['requirements'][$use_name] = array(
            '#type' => 'fieldset',
            '#title' => $use_title . ' Fee',
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#tree' => TRUE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirements',
              ),
            ),
            '#states' => $states,
          );

          if (in_array($use_name, $uses_personnel)) {
            $form['review']['operations']['requirements'][$use_name]['quantity'] = array(
              '#id' => 'request-review-operations-requirements-' . $use_name . '-quantity',
              '#type' => 'numberfield',
              '#title' => '<span class="element-invisible">' . $use_title . ' Fee </span>Personnel',
              '#default_value' => NULL,
              '#size' => 16,
              '#min' => 0,
              '#required' => $uses_requirements[$use_name],
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'requirement-quantity',
                ),
              ),
              '#states' => $states,
            );

            if (!empty($request['requirements'][$use_name][0])) {
              $form['review']['operations']['requirements'][$use_name]['quantity']['#default_value'] = $request['requirements'][$use_name][0]->quantity;
            }
          }

          if (in_array($use_name, $uses_hours)) {
            $form['review']['operations']['requirements'][$use_name]['hours'] = array(
              '#id' => 'request-review-operations-requirements-' . $use_name . '-hours',
              '#type' => 'textfield',
              '#title' => '<span class="element-invisible">' . $use_title . ' Fee </span>Hours',
              '#default_value' => NULL,
              '#size' => 16,
              '#min' => 0,
              '#required' => $uses_requirements[$use_name],
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'requirement-hours',
                ),
              ),
              '#states' => $states,
            );

            if (!empty($request['requirements'][$use_name][0])) {
              $form['review']['operations']['requirements'][$use_name]['hours']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->hours, 'currency');
            }
          }

          $form['review']['operations']['requirements'][$use_name]['days'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-days',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee </span>Days',
            '#default_value' => NULL,
            '#min' => 0,
            '#size' => 16,
            '#required' => $uses_requirements[$use_name],
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-days',
              ),
            ),
            '#states' => $states,
          );

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['days']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->days, 'currency');
          }

          $form['review']['operations']['requirements'][$use_name]['amount'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-amount',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee </span>Amount',
            '#default_value' => NULL,
            '#size' => 16,
            '#min' => 0,
            '#required' => $uses_requirements[$use_name],
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-amount',
              ),
            ),
            '#states' => $states,
          );

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['amount']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->amount, 'currency');
          }
          elseif (isset($requirements_amount_defaults[$use_name])) {
            $form['review']['operations']['requirements'][$use_name]['amount']['#default_value'] = $requirements_amount_defaults[$use_name];
          }
        }

        if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
          if ($venue_coordinator) {
            $form['review']['operations']['requirements']['other'] = array(
              '#id' => 'request-review-operations-requirements-other',
              '#type' => 'checkbox',
              '#title' => 'Other Fees',
              '#description' => "Other charges, unknown presently, will be agreemented to the presenter at a later time.",
              '#default_value' => NULL,
              '#attributes' => array(
                'class' => array(
                  'no-print',
                  'mode-' . $mode,
                  'requirement-other',
                ),
              ),
              '#states' => array(
                'visible' => array(
                  ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
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

            if (!empty($request['requirements']['other'][0])) {
              $form['review']['operations']['requirements']['other']['#default_value'] = $request['requirements']['other'][0]->value;
            }
          }
        }

        $form['review']['operations']['insurance'] = array(
          '#tree' => TRUE,
          '#weight' => 3,
        );

        if ($insurance_requirement && $step == MFCS_REVIEW_STEP_REVIEW) {
          // contractor insurance
          $form['review']['operations']['insurance']['contractor'] = array(
            '#id' => 'request-review-operations-insurance-contractor',
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
                ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
              ),
            ),
          );

          if (!empty($request['insurance']['contractor'][0])) {
            $form['review']['operations']['insurance']['contractor']['#default_value'] = $request['insurance']['contractor'][0]->value;
          }

          // unaffiliated insurance
          $form['review']['operations']['insurance']['unaffiliated'] = array(
            '#id' => 'request-review-operations-insurance-unaffiliated',
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
                ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
              ),
            ),
          );

          if (!empty($request['insurance']['unaffiliated'][0])) {
            $form['review']['operations']['insurance']['unaffiliated']['#default_value'] = $request['insurance']['unaffiliated'][0]->value;
          }

          // provided insurance
          $form['review']['operations']['insurance']['provided'] = array(
            '#id' => 'request-review-operations-insurance-provided',
            '#type' => 'checkbox',
            '#title' => 'Provided Insurance',
            '#description' => "Select this once insurance has been provided. If insurance is required, then this must be selected before you can approve this request.",
            '#default_value' => NULL,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'insurance-provided',
              ),
            ),
            '#states' => array(
              'visible' => array(
                ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
              ),
            ),
          );

          if (!empty($request['insurance']['provided'][0])) {
            $form['review']['operations']['insurance']['provided']['#default_value'] = $request['insurance']['provided'][0]->value;
          }
        }
      }

      if ($can_review_request && $step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
        $form['review']['operations']['presidents_approval'] = array(
          '#id' => 'request-review-operations-presidents_approval',
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
              ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
            ),
          ),
          '#weight' => 4,
        );

        $form['review']['operations']['university_business'] = array(
          '#id' => 'request-review-operations-university_business',
          '#type' => 'checkbox',
          '#title' => "Strictly University Business",
          '#description' => "By selecting this checkbox you claim that this is strictly for university business and requirements do not apply.",
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
              ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
            ),
          ),
          '#weight' => 5,
        );
      }

      $form['review']['operations']['message'] = array(
        '#id' => 'request-review-operations-message',
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
        '#weight' => 6,
      );

      $form['review']['operations']['submit'] = array(
        '#id' => 'request-review-operations-submit',
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-review-operations-submit',
            'mode-' . $mode,
          ),
        ),
        '#weight' => 1000,
      );

      $form['review']['operations']['submit']['submit'] = array(
        '#id' => 'request-review-operations-submit-submit',
        '#name' => 'request-review-operations-submit-submit',
        '#type' => 'submit',
        '#default_value' => t("Submit"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'request-review-operations-submit-submit',
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
 * @param int $request_id
 *   The request id.
 * @param string $group
 *   The group name.
 * @param array $presentation_tables
 *   An array of tables defining how the presentation is to be built.
 * @param array $show
 *   An array containing the following keys:
 *   'edit': When TRUE, the edit link will be shown in the group header.
 *   'history': When TRUE, the history link is made visible.
 * @param array $request
 *   The request settings array.
 *
 * @return int|null
 *   The row number or NULL when there is no valid row.
 *   FALSE is returned for in
 *
 * @see: mfcs_request_view_0_page()
 */
function mfcs_request_view_0_page_setup_group_path(&$form, $request_id, $group, $presentation_tables, $show, $request) {
  $row = NULL;
  $group_path = &$form['display']['groups'];
  $request_type = $request['information']['type'][0]->value;

  $show_link = TRUE;
  if (isset($presentation_tables[$group]['request_type']['hide_link']) && is_array($presentation_tables[$group]['request_type']['hide_link'])) {
    if (in_array($request_type, $presentation_tables[$group]['request_type']['hide_link'])) {
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

  if ($show['agreement']) {
    if ($group != 'request') {
      $show['agreement'] = FALSE;
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
          'id' => 'section-request-' . $request_id . '-group-' . $row,
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
        'id' => 'section-request-' . $request_id . '-' . $group,
        'class' => array(
          'section-request',
          'section-request-' . $group,
        ),
      ),
      '#extra_prefix' => '',
    );

    if (isset($presentation_tables[$group]['label'])) {
      $group_path[$group]['#extra_prefix'] .= '<div class="section-request-' . $request_id . '-' . $group . '-label-wrapper section-request-label-wrapper">';

      if ($show_link) {
        if ($show['clone']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/create-0/' . $request_id . '" class="section-request-' . $request_id . '-' . $group . '-history section-request-clone no-print" title="'. t("Copy " . $presentation_tables[$group]['label']) . '">Copy</a>';
        }

        if ($show['edit']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/edit-0/' . $request_id . '/' . $group . '" class="section-request-' . $request_id . '-' . $group . '-edit section-request-edit no-print" title="'. t("Edit " . $presentation_tables[$group]['label']) . '">Edit</a>';
        }

        if ($show['history']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/history-0/' . $request_id . '" class="section-request-' . $request_id . '-' . $group . '-history section-request-history no-print" title="'. t("Edit " . $presentation_tables[$group]['label']) . '">History</a>';
        }

        if ($show['agreement']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/agreement-0/' . $request_id . '" class="section-request-' . $request_id . '-' . $group . '-agreement section-request-agreement no-print" title="'. t("View Facilities Use Agreement for " . $presentation_tables[$group]['label']) . '">Agreement</a>';
        }
      }

      $group_path[$group]['#extra_prefix'] .= '<h2 id="section-request-' . $request_id . '-' . $group . '-label" class="section-request-label section-request-' . $group . '-label">';
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
 * Pre-Validation for mfcs_request_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_page()
 */
function mfcs_request_view_0_page_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-review-operations-submit-submit') {
    if (isset($form_state['values']['review']['operations']['decision']) && $form_state['values']['review']['operations']['decision'] == MFCS_REVIEW_DECISION_COMMENT) {
      $form['review']['operations']['message']['#required'] = TRUE;
    }
  }
  else {
    $form['review']['operations']['decision']['#required'] = FALSE;
  }

  $request = $form['form']['request']['#value'];
  $request_id = (int) $form['form']['request_id']['#value'];
  $mode = (int) $form['form']['mode']['#value'];
  $user = cf_current_user();

  $request_classification = $request['request_coordinator']['classification'][0]->value;
  $step = $request['top']['step'][0]->value;
  $status = $request['top']['status'][0]->value;

  $decision = NULL;
  if (isset($form_state['values']['review']['operations']['decision'])) {
    $decision = $form_state['values']['review']['operations']['decision'];
  }

  $unrequire_usage_fields = TRUE;
  if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
    if ($decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
      $requirements_waved = FALSE;
      if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
        $requirements_waved = TRUE;
      }

      if (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
        $requirements_waved = TRUE;
      }

      $unrequire_usage_fields = $requirements_waved;
    }
  }

  if ($unrequire_usage_fields) {
    if (isset($form['review']['operations']['requirements']) && is_array($form['review']['operations']['requirements'])) {
      foreach ($form['review']['operations']['requirements'] as $key => &$value) {
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
 * Validation for mfcs_request_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_page()
 */
function mfcs_request_view_0_page_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $form_state['rebuild'] = TRUE;

  $manager = user_access('mfcs manage');
  $reviewer = user_access('mfcs review');
  $requester = user_access('mfcs request');

  $request = $form['form']['request']['#value'];
  $request_id = (int) $form['form']['request_id']['#value'];
  $mode = (int) $form['form']['mode']['#value'];
  $user = cf_current_user();

  $can_review_request = mfcs_management_page_request_access('review', $request_id);
  $can_comment_request = mfcs_management_page_request_access('comment', $request_id);

  $request_classification = $request['request_coordinator']['classification'][0]->value;
  $step = $request['top']['step'][0]->value;
  $status = $request['top']['status'][0]->value;

  $decision = NULL;
  if (isset($form_state['values']['review']['operations']['decision'])) {
    $decision = $form_state['values']['review']['operations']['decision'];
  }

  if ($clicked_id == 'request-display-submit-print' || $clicked_id == 'request-review-log-submit-print') {
    form_set_error('', "The print button uses javascript to help force a print. You can always use your browsers print button (or equivalent) directly.");
  }
  elseif ($clicked_id == 'request-display-submit-download' || $clicked_id == 'request-review-log-submit-download') {
    $form_state['rebuild'] = FALSE;
  }
  elseif ($clicked_id == 'request-display-submit-cancel') {
    if ($mode != MFCS_VIEW_MODE_NORMAL && $mode != MFCS_VIEW_MODE_DISPLAY) {
      form_set_error('', "Illegal operation detected.");
    }

    $form_state['cancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'request-display-submit-uncancel') {
    if ($mode != MFCS_VIEW_MODE_NORMAL && $mode != MFCS_VIEW_MODE_DISPLAY) {
      form_set_error('', "Illegal operation detected.");
    }

    $form_state['uncancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'request-review-operations-submit-submit') {
    $failure = FALSE;

    // only allow operations for states and steps that make sense to.
    $allowed_statuses = array(
      MFCS_EVENT_STATUS_LOCKED,
      MFCS_EVENT_STATUS_UNLOCKED,
    );

    if (!in_array($status, $allowed_statuses)) {
      form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current status.");
      $failure = TRUE;
    }

    $allowed_steps = array(
      MFCS_REVIEW_STEP_VENUE_AVAILABLE,
      MFCS_REVIEW_STEP_REVIEW,
      MFCS_REVIEW_STEP_REQUIREMENTS,
      MFCS_REVIEW_STEP_MAKE_DECISIONS,
    );

    if (!in_array($step, $allowed_steps)) {
      form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current step.");
      $failure = TRUE;
    }

    if ($mode < MFCS_VIEW_MODE_NORMAL && $mode > MFCS_VIEW_MODE_OPERATIONS) {
      form_set_error('', "Illegal operation detected. Invalid mode selected.");
      $failure = TRUE;
    }
    elseif (!$can_review_request) {
      if ($decision == MFCS_REVIEW_DECISION_COMMENT) {
        if ($can_comment_request) {
          unset($form_state['values']['review']['operations']['requirements']);
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

    // A request was made to have the requirements waved options be displayed with the requirements to avoid confusion.
    // To do this without changing the underlining design, when the requirement is specified with a wave checked, then convert the decision to a wave.
    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
      if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
        $form_state['values']['review']['operations']['decision'] = MFCS_REVIEW_DECISION_WAVE;
        $decision = MFCS_REVIEW_DECISION_WAVE;
      }
      elseif (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
        $form_state['values']['review']['operations']['decision'] = MFCS_REVIEW_DECISION_WAVE;
        $decision = MFCS_REVIEW_DECISION_WAVE;
      }
    }

    $requirements_required = TRUE;
    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if (isset($request['requirements']['waved'][0]->value)) {
        if ($request['requirements']['waved'][0]->value == 1) {
          $requirements_required = FALSE;
        }
      }

      if (isset($request['requirements']['university'][0]->value)) {
        if ($request['requirements']['university'][0]->value == 1) {
          $requirements_required = FALSE;
        }
      }
    }

    if (!$requirements_required) {
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        unset($form_state['values']['review']['operations']['requirements']);
      }
    }

    if ($step == MFCS_REVIEW_STEP_REVIEW && $decision == MFCS_REVIEW_DECISION_REQUIREMENT && $requirements_required) {
      $requirements = array();
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        $requirements = $form_state['values']['review']['operations']['requirements'];
      }

      if (array_key_exists('facilities', $requirements) && !empty($requirements['facilities']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements['facilities']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][requirements][facilities][amount', t("@facilities_requirement is not a valid facilities requirement amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@facilities' => $requirements['facilities']['amount'])));
          $failure = TRUE;
        }

        if (isset($requirements['facilities']['quantity']) && intval($requirements['facilities']['quantity']) != $requirements['facilities']['quantity']) {
          form_set_error('review][operations][requirements][facilities][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $requirements['facilities']['quantity'])));
          $failure = TRUE;
        }

        if (isset($requirements['facilities']['hours']) && !cf_is_integer($requirements['facilities']['hours'])) {
          form_set_error('review][operations][requirements][facilities][hours', t("@hours is not an valid number. Use only numbers.", array('@hours' => $requirements['facilities']['hours'])));
          $failure = TRUE;
        }

        if (isset($requirements['facilities']['days']) && intval($requirements['facilities']['days']) != $requirements['facilities']['days']) {
          form_set_error('review][operations][requirements][facilities][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $requirements['facilities']['days'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('equipment', $requirements) && !empty($requirements['equipment']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements['equipment']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][requirements][equipment][amount', t("@equipment is not a valid additional equipment requirement. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@equipment' => $requirements['equipment']['amount'])));
          $failure = TRUE;
        }

        if (isset($requirements['equipment']['quantity']) && intval($requirements['equipment']['quantity']) != $requirements['equipment']['quantity']) {
          form_set_error('review][operations][requirements][equipment][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $requirements['equipment']['quantity'])));
          $failure = TRUE;
        }

        if (isset($requirements['equipment']['hours']) && !cf_is_integer($requirements['equipment']['hours'])) {
          form_set_error('review][operations][requirements][equipment][hours', t("@hours is not an valid number. Use only numbers.", array('@hours' => $requirements['equipment']['hours'])));
          $failure = TRUE;
        }

        if (isset($requirements['equipment']['days']) && intval($requirements['equipment']['days']) != $requirements['equipment']['days']) {
          form_set_error('review][operations][requirements][equipment][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $requirements['equipment']['days'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('custodial', $requirements) && !empty($requirements['custodial']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements['custodial']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][requirements][custodial][amount', t("@custodial is not a valid custodial requirement amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@custodial' => $requirements['custodial']['amount'])));
          $failure = TRUE;
        }

        if (isset($requirements['custodial']['quantity']) && intval($requirements['custodial']['quantity']) != $requirements['custodial']['quantity']) {
          form_set_error('review][operations][requirements][custodial][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $requirements['custodial']['quantity'])));
          $failure = TRUE;
        }

        if (isset($requirements['custodial']['hours']) && !cf_is_integer($requirements['custodial']['hours'])) {
          form_set_error('review][operations][requirements][custodial][hours', t("@hours is not an valid number. Use only numbers.", array('@hours' => $requirements['custodial']['hours'])));
          $failure = TRUE;
        }

        if (isset($requirements['custodial']['days']) && intval($requirements['custodial']['days']) != $requirements['custodial']['days']) {
          form_set_error('review][operations][requirements][custodial][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $requirements['custodial']['days'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('security', $requirements) && !empty($requirements['security']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements['security']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][requirements][security][amount', t("@security is not a valid security requirement amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@security' => $requirements['security']['amount'])));
          $failure = TRUE;
        }

        if (isset($requirements['security']['quantity']) && intval($requirements['security']['quantity']) != $requirements['security']['quantity']) {
          form_set_error('review][operations][requirements][security][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $requirements['security']['quantity'])));
          $failure = TRUE;
        }

        if (isset($requirements['security']['hours']) && !cf_is_integer($requirements['security']['hours'])) {
          form_set_error('review][operations][requirements][security][hours', t("@hours is not an valid number. Use only numbers.", array('@hours' => $requirements['security']['hours'])));
          $failure = TRUE;
        }

        if (isset($requirements['security']['days']) && intval($requirements['security']['days']) != $requirements['security']['days']) {
          form_set_error('review][operations][requirements][security][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $requirements['security']['days'])));
          $failure = TRUE;
        }
      }

      unset($requirements);
    }

    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_APPROVE) {
      $requirements_not_supplied = TRUE;

      if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
        if (isset($request['requirements']['waved'][0]->value)) {
          if ($request['requirements']['waved'][0]->value) {
            $requirements_not_supplied = FALSE;
          }
        }

        if (isset($request['requirements']['university'][0]->value)) {
          if ($request['requirements']['university'][0]->value) {
            $requirements_not_supplied = FALSE;
          }
        }
      }

      if (isset($request['requirements']['facilities'][0]->days)) {
        $requirements_not_supplied = FALSE;
      }

      if ($requirements_not_supplied) {
        form_set_error('review][operations][decision', t("The request cannot be approved until the requirements are either specified or waved."));
      }
    }

    // autowave will be set to TRUE if and only if a reviewer has not specified a WAVE yet, but still APPROVED.
    $form_state['autowave'] = FALSE;

    if ($step == MFCS_REVIEW_STEP_REVIEW && $decision == MFCS_REVIEW_DECISION_APPROVE) {
      $requirements_not_supplied = TRUE;
      $requirements_exist = TRUE;

      $insurance_contractor_required = FALSE;
      $insurance_unaffiliated_required = FALSE;

      $custodial_requirements_required = FALSE;
      if (isset($form['form']['requirements-custodial']['#value'])) {
        $custodial_requirements_required = $form['form']['requirements-custodial']['#value'];
      }

      $security_requirements_required = FALSE;
      if (isset($form['form']['requirements-custodial']['#value'])) {
        $security_requirements_required = $form['form']['requirements-security']['#value'];
      }

      $insurance_requirements_required = FALSE;
      if (isset($form['form']['requirements-custodial']['#value'])) {
        $insurance_requirements_required = $form['form']['requirements-insurance']['#value'];
      }

      if (!$custodial_requirements_required && !$security_requirements_required && !$insurance_requirements_required) {
        $requirements_not_supplied = FALSE;
        $requirements_exist = FALSE;
      }

      if (isset($form['review']['operations']['requirements']['custodial'])) {
        if (isset($request['requirements']['custodial'][0]->amount)) {
          $requirements_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['requirements']['security'])) {
        if (isset($request['requirements']['security'][0]->amount)) {
          $requirements_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['insurance']['contractor'])) {
        if (isset($request['insurance']['contractor'][0]->value)) {
          $requirements_not_supplied = FALSE;

          if ($request['insurance']['unaffiliated'][0]->value) {
            $insurance_contractor_required = TRUE;
          }
        }
      }

      if (isset($form['review']['operations']['insurance']['unaffiliated'])) {
        if (isset($request['insurance']['unaffiliated'][0]->value)) {
          $requirements_not_supplied = FALSE;

          if ($request['insurance']['unaffiliated'][0]->value) {
            $insurance_unaffiliated_required = TRUE;
          }
        }
      }

      if ($step == MFCS_REVIEW_STEP_REVIEW && $requirements_exist) {
        if ($requirements_not_supplied) {
          $form_state['autowave'] = TRUE;
        }
      }

      // determine whether or not the purchasing/insurance reviewer can approve based on the status of the review checkboxes.
      if ($insurance_contractor_required || $insurance_unaffiliated_required) {
        $reviewers_conditions = array(
          'u.uid' => $user->uid,
          'mr.review_step' => $step,
          'mr.request_classification' => $request_classification,
          'mr.reviewer_classification' => MFCS_REVIEW_CLASSIFICATION_PURCHASING,
        );

        $reviewer_handles_purchasing = mfcs_get_reviewers($reviewers_conditions, TRUE);

        if (!empty($reviewer_handles_purchasing)) {
          if (!$request['insurance']['provided'][0]->value) {
            form_set_error('review][operations][decision', t("The request cannot be approved until the insurance has been provided."));
          }
        }
      }
    }

    if (!$failure) {
      $form_state['rebuild'] = FALSE;
    }
  }
  elseif ($clicked_id == 'request-cancel-submit-yes' || $clicked_id == 'request-cancel-submit-no') {
    $form_state['rebuild'] = FALSE;
  }
  elseif ($clicked_id == 'request-uncancel-submit-yes' || $clicked_id == 'unrequest-cancel-submit-no') {
    $form_state['rebuild'] = FALSE;
  }
}

/**
 * Submit process for mfcs_request_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_page()
 */
function mfcs_request_view_0_page_submit($form, &$form_state) {
  $user = cf_current_user();

  $request_id = NULL;
  if (isset($form['form']['request_id']['#value'])) {
    $request_id = $form['form']['request_id']['#value'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-cancel-submit-yes') {
    $deleted = mfcs_include(5);

    mfcs_request_cancel_0_form_submit($form, $form_state);

    return;
  }
  elseif ($clicked_id == 'request-uncancel-submit-yes') {
    $deleted = mfcs_include(5);

    mfcs_request_uncancel_0_form_submit($form, $form_state);

    return;
  }
  // @todo: add submit processing for resetting the step back to the review stage.
  elseif ($clicked_id == 'request-review-operations-submit-submit') {
    $success = TRUE;

    mfcs_include(6);
    $decision = $form_state['values']['review']['operations']['decision'];
    $message = $form_state['values']['review']['operations']['message'];

    $request = $form['form']['request']['#value'];

    $request_classification = $request['request_coordinator']['classification'][0]->value;
    $step = $request['top']['step'][0]->value;

    $extra = array();
    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW) {
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        $extra['requirements'] = $form_state['values']['review']['operations']['requirements'];
      }

      if (isset($form_state['values']['review']['operations']['insurance'])) {
        $extra['insurance'] = $form_state['values']['review']['operations']['insurance'];
      }
    }

    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if (isset($form_state['values']['review']['operations']['presidents_approval'])) {
        $extra['requirements']['waved'] = $form_state['values']['review']['operations']['presidents_approval'];
      }

      if (isset($form_state['values']['review']['operations']['university_business'])) {
        $extra['requirements']['university'] = $form_state['values']['review']['operations']['university_business'];
      }
    }
    elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
      if (isset($form_state['autowave']) && $form_state['autowave']) {
        mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], MFCS_REVIEW_STEP_REVIEW, MFCS_REVIEW_DECISION_WAVE, "", array('send_email' => FALSE));
      }
    }


    if ($success) {
      $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], $step, $decision, $message, $extra);
    }

    if ($success) {
      drupal_set_message("Your review or comment has been recieved.");
    }
    else {
      form_set_error('form', "An error occurred while trying to submit the request review. Please contact the support staff.");
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to submit the request review.", array(), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }

    // change to the request view page on submit for these decisions to prrequest potential access denied messages following the submit process.
    if ($decision == MFCS_REVIEW_DECISION_APPROVE || $decision == MFCS_REVIEW_DECISION_DENY) {
      $form_state['values']['redirect_to'] = 'requests/view-0/' .  $request_id;
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = 'requests';
    }
    else {
      if ($form['form']['mode']['#value'] > 0) {
        $form_state['redirect'] = 'requests/view-0/' . $request_id . '/' . $form['form']['mode']['#value'];
      }
      else {
        $form_state['redirect'] = 'requests/view-0/' . $request_id;
      }
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Custom submit process for mfcs_request_view_0_page().
 *
 * Used for generating a print PDF.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_page()
 */
function mfcs_request_view_0_page_submit_pdf($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if (($clicked_id != 'request-display-submit-download' && $clicked_id != 'request-review-log-submit-download') || empty($request_id)) {
    if (empty($request_id)) {
      $form_state['redirect'] = 'requests';
    }
    else {
      $form_state['redirect'] = 'requests/view-0/' . $request_id;
    }

    return;
  }

  global $base_path;

  $mode = $form['form']['mode']['#value'];
  $form_state['redirect'] = '/requests/pdf-0/' . $request_id . '/' . $mode;
}

/**
 * Renders a PDF of a given request.
 *
 * @param int $request_id
 *   The unique identifier for an request.
 * @param int|null $mode
 *   Specifies the mode that alters how the form is rendered.
 *   1: Render only the display part.
 */
function mfcs_request_pdf_0_page($request_id, $mode) {
  $page_title = "Facilities Use Request";
  drupal_set_title($page_title);

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Facilities Use Request: " . $request['information']['title'][0]->value;
    drupal_set_title($page_title);
  }

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mfcs-request-view-0-page',
      'class' => array(
        'mfcs-request-pdf-0',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $page['html']['content'] = array(mfcs_request_view_0_page($form, $form_state, $request_id, $mode, TRUE));
  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  $result = mfcs_request_pdf_0_page_using_wkhtmltopdf($request_id, $html);

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
 * @param int $request_id
 *   The numeric request id.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_build_reviews_log(&$form, &$form_state, $request_id) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }

  mfcs_include(0);

  $presentation = mfcs_table_presentation();

  mfcs_include(4);

  $decision_options = mfcs_get_reviewer_decision_list_options();

  // review revisions are neither used nor incremented in the same manner as other fields.
  // the revision number will not increment when a new review is made.
  $review_revision = mfcs_load_request_revision_number($request_id, 'mfcs_field_review_review');

  if ($review_revision === FALSE || is_null($review_revision)) {
    $review_revision = 0;
  }

  try {
    $query = db_select('mfcs_field_review_review', 'mfrr');

    $query->fields('mfrr');

    $query->condition('mfrr.request_id', $request_id);
    #$query->condition('mfrr.revision', $review_revision);

    $query->orderBy('mfrr.date');

    $reviews = $query->execute()->fetchAll();

    if (is_array($reviews)) {
      foreach ($reviews as $review) {
        $query = db_select('mfcs_review_classifications', 'mrc');

        $query->fields('mrc');
        $query->condition('mrc.review_id', $review->id);

        $review_classifications = $query->execute()->fetchAll();

        $review->reviewer_classifications = array();
        if (is_array($review_classifications)) {
          foreach ($review_classifications as $review_classification) {
            // some classifications are not allowed in certain steps.
            if ($review->step == MFCS_REVIEW_STEP_REVIEW ||$review->step == MFCS_REVIEW_STEP_REQUIREMENTS) {
              if ($review_classification->classification == MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR) {
                continue;
              }
              elseif ($review_classification->classification == MFCS_REVIEW_CLASSIFICATION_SYSTEM_ADMINISTRATOR) {
                continue;
              }
              elseif ($review_classification->classification == MFCS_REVIEW_CLASSIFICATION_MANAGER) {
                continue;
              }
              elseif ($review_classification->classification == MFCS_REVIEW_CLASSIFICATION_FAILSAFE_COORDINATOR) {
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
  $changelog_fields['requirements'] = array(
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
    'provided',
  );

  $users = array();
  $reviewers = array();
  $has_log = FALSE;
  if (is_array($reviews)) {
    $review_step_options = mfcs_get_review_step_list_options();
    $reviewer_classification_options = mfcs_get_reviewer_classification_list_options();

    foreach ($reviews as $row => $review) {
      $user_id = $review->user_id;
      $review_step = $review->step;
      $decision = $review->decision;
      $message = $review->message;
      $date = $review->date;

      $changelog = mfcs_load_request_by_timestamp($request_id, $date, $changelog_fields);

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

        if ($classification == MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR) {
          $reviewer_classification .= "Venue Coordinator";
        }
        elseif ($classification == MFCS_REVIEW_CLASSIFICATION_MANAGER) {
          $reviewer_classification .= "Manager";
        }
        elseif ($classification == MFCS_REVIEW_CLASSIFICATION_SYSTEM_ADMINISTRATOR) {
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

      if ($decision == MFCS_REVIEW_DECISION_AMENDMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">' . "Amended" . '</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Unavailable</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_MAKE_DECISIONS && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Confirmed and Reserved</strong> the request' . $reviewer_classification . "";
      }
      elseif ($review_step == MFCS_REVIEW_STEP_MAKE_DECISIONS && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Denied</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_COMMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has made a <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif (($review_step == MFCS_REVIEW_STEP_REVIEW || $review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) && $decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif (($review_step == MFCS_REVIEW_STEP_REVIEW || $review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) && $decision == MFCS_REVIEW_DECISION_WAVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has waved the <strong class="review-item-label-text-row-decision">' . $decision_options[MFCS_REVIEW_DECISION_REQUIREMENT] . '</strong> ' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has accepted the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has denied the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      else {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' chose to <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> the request' . $reviewer_classification . '.';
      }
      $markup .= '</div></h3>';
      $markup .= '<div class="review-item-label-date">' . date("Y/m/d h:i:s a", $date) . "</div>";
      $markup .= '</div>';


      // provide additional changelog information.
      if ($decision == MFCS_REVIEW_DECISION_AMENDMENT) {
        $message .= '<div class="review-item-message-decision-amendment review-item-message-decision">';
        $message .= 'The review step has been reset and all prior reviews no longer apply.';
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['facilities'])) {
        $message .= '<div class="review-item-message-requirements-facilities review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Facilities Fees:</div> ';

        $value = reset($changelog['requirements']['facilities']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'facilities', 0, $value, $presentation['requirements']['facilities'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['equipment'])) {
        $message .= '<div class="review-item-message-requirements-equipment review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Additional Equipment Fees:</div> ';

        $value = reset($changelog['requirements']['equipment']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'equipment', 0, $value, $presentation['requirements']['equipment'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['security'])) {
        $message .= '<div class="review-item-message-requirements-security review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Security Fees:</div> ';

        $value = reset($changelog['requirements']['security']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'security', 0, $value, $presentation['requirements']['security'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['custodial'])) {
        $message .= '<div class="review-item-message-requirements-custodial review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Custodial Fees:</div> ';

        $value = reset($changelog['requirements']['custodial']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'custodial', 0, $value, $presentation['requirements']['custodial'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['other'])) {
        $message .= '<div class="review-item-message-requirements-other review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Other Fees:</div> ';

        // always display the boolean when displayed in the review log.
        foreach ($presentation['requirements']['other']['display'] as &$value) {
          if ($value == 'when_true' || $value == 'when_false') {
            $value = 'raw';
          }
        }

        $value = reset($changelog['requirements']['other']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'other', 0, $value, $presentation['requirements']['other'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_REQUIREMENT || $decision == MFCS_REVIEW_DECISION_WAVE)) {
        if (!empty($changelog['requirements']['waved'])) {
          $message .= '<div class="review-item-message-requirements-waved review-item-message-requirements review-item-message-requirements">';
          $message .= '<div class="item-label">Waved with Presidents Approval:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['requirements']['waved']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['requirements']['waved']);
          $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'waved', 0, $value, $presentation['requirements']['waved'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }

        if (!empty($changelog['requirements']['university'])) {
          $message .= '<div class="review-item-message-requirements-university review-item-message-university review-item-message-requirements">';
          $message .= '<div class="item-label">Strictly University Business:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['requirements']['university']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['requirements']['university']);
          $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'university', 0, $value, $presentation['requirements']['university'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }
      }

      // contractor insurance
      if (!empty($changelog['insurance']['contractor'])) {
        $message .= '<div class="review-item-message-insurance-contractor review-item-message-insurance review-item-message-requirements">';
        $message .= '<div class="item-label">Contractor Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        if (!empty($presentation['insurance']['contractor']['display'])) {
          foreach ($presentation['insurance']['contractor']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }
        }

        $value = reset($changelog['insurance']['contractor']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'insurance', 'contractor', 0, $value, $presentation['insurance']['contractor'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      // unaffiliated insurance
      if (!empty($changelog['insurance']['unaffiliated'])) {
        $message .= '<div class="review-item-message-insurance-unaffiliated review-item-message-insurance review-item-message-requirements">';
        $message .= '<div class="item-label">Unaffiliated Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        if (!empty($presentation['insurance']['unaffiliated']['display'])) {
          foreach ($presentation['insurance']['unaffiliated']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }
        }

        $value = reset($changelog['insurance']['unaffiliated']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'insurance', 'unaffiliated', 0, $value, $presentation['insurance']['unaffiliated'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      // provided insurance
      if (!empty($changelog['insurance']['provided'])) {
        $message .= '<div class="review-item-message-insurance-provided review-item-message-insurance review-item-message-requirements">';
        $message .= '<div class="item-label">Provided Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        if (!empty($presentation['insurance']['provided']['display'])) {
          foreach ($presentation['insurance']['provided']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }
        }

        $value = reset($changelog['insurance']['provided']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'insurance', 'provided', 0, $value, $presentation['insurance']['provided'], $changelog);

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
 * @} End of '@addtogroup mfcs'.
 */