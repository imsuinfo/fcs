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
 *   The unique identifier for a request.
 * @param int $mode
 *   Specifies the mode that alters how the form is rendered.
 * @param null|int $revision_id
 *   (optional) When not NULL, load the request based on the revision number
 *   instead of the latest revision.
 * @param bool $contained
 *   (optional) Enable special handling for PDF and print generation.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_view_0_form($form, &$form_state, $request_id, $mode = MFCS_VIEW_MODE_NORMAL, $revision_id = NULL, $contained = FALSE) {
  if (!is_string($mode) || !($mode == MFCS_VIEW_MODE_DISPLAY || $mode == MFCS_VIEW_MODE_LOG || $mode == MFCS_VIEW_MODE_OPERATIONS)) {
    $mode = MFCS_VIEW_MODE_NORMAL;
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $revision_id);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $user = cf_current_user();

  if (is_null($revision_id)) {
    $can_create_request = mfcs_request_access('create', $request, $user);
    $can_edit_request = mfcs_request_access('edit', $request, $user);
    $can_log_request = mfcs_request_access('log', $request, $user);
    $can_review_request = mfcs_request_access('review', $request, $user);
    $can_comment_request = mfcs_request_access('comment', $request, $user);
    $can_history_request = mfcs_request_access('history', $request, $user);
    $can_agreement_request = mfcs_request_access('agreement', $request, $user);
    $can_cancel_request = mfcs_request_access('cancel', $request, $user);
    $can_uncancel_request = mfcs_request_access('uncancel', $request, $user);
  }
  else {
    // perform access checks against the latest version and not the viewed revision.
    $request_latest = mfcs_load_request_by_id($request_id);

    if (empty($request)) {
      drupal_not_found();
      drupal_exit();
      return array();
    }

    $can_create_request = mfcs_request_access('create', $request_latest, $user);
    $can_edit_request = mfcs_request_access('edit', $request_latest, $user);
    $can_log_request = mfcs_request_access('log', $request_latest, $user);
    $can_review_request = mfcs_request_access('review', $request_latest, $user);
    $can_comment_request = mfcs_request_access('comment', $request_latest, $user);
    $can_history_request = mfcs_request_access('history', $request_latest, $user);
    $can_agreement_request = mfcs_request_access('agreement', $request_latest, $user);
    $can_cancel_request = mfcs_request_access('cancel', $request_latest, $user);
    $can_uncancel_request = mfcs_request_access('uncancel', $request_latest, $user);

    if (!$can_history_request) {
      drupal_not_found();
      drupal_exit();
    }
  }

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (is_null($revision_id)) {
    if ($mode == MFCS_VIEW_MODE_NORMAL) {
      mfcs_add_canonical_headers('requests/view-0/' . $request_id . '/' . $mode, 'requests/view-0/' . $request_id);
    }
    else {
      mfcs_add_canonical_headers('requests/view-0/' . $request_id . '/' . $mode);
    }
  }
  else {
    mfcs_add_canonical_headers('requests/view-0/' . $request_id . '/' . $mode . '/' . $revision_id);
  }

  if (!is_bool($contained)) {
    $contained = FALSE;
  }

  // if cancelling the request, return only the cancellation confirmation form.
  if (isset($form_state['cancel_was_clicked'])) {
    mfcs_include(MFCS_INCLUDE_REQUEST_CANCEL);

    $cancel_form = array();
    $cancel_form_state = array();
    return mfcs_request_cancel_0_form($cancel_form, $cancel_form_state, $request_id);
  }
  // if uncancelling the request, return only the uncancellation confirmation form.
  elseif (isset($form_state['uncancel_was_clicked'])) {
    mfcs_include(MFCS_INCLUDE_REQUEST_CANCEL);

    $cancel_form = array();
    $cancel_form_state = array();
    return mfcs_request_uncancel_0_form($cancel_form, $cancel_form_state, $request_id);
  }

  $proxies = mfcs_get_proxy_venue_coordinator($user->uid);
  $venue_coordinator_ids = array();
  if (!empty($proxies)) {
    $venue_coordinator_ids = $proxies;
  }
  $venue_coordinator_ids[$user->uid] = $user->uid;

  $venue_coordinator = (in_array($request['venue_coordinator']['user_id'][0]->value, $venue_coordinator_ids));

  $administer = user_access('mfcs administer', $user);
  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

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

  $step = $request['top']['step'][0]->value;
  $status = $request['top']['status'][0]->value;

  if ($can_review_request || $can_comment_request) {
    // show review operations only if the content is reviewable by the current user.
    $request_classification = $request['request_coordinator']['classification'][0]->value;

    // only show operations for states and steps that make sense to.
    $allowed_statuses = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED, // added due to insurance provided exception.
    );


    if (!$can_comment_request && !in_array($status, $allowed_statuses)) {
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
      MFCS_REVIEW_STEP_COMPLETED, // added due to insurance provided exception.
    );

    if (!$can_comment_request && !in_array($step, $allowed_steps)) {
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

  if ($contained) {
    $can['edit'] = FALSE;
    $can['history'] = FALSE;
    $can['agreement'] = FALSE;
    $can['clone'] = FALSE;
    $can['ical'] = FALSE;
  }
  elseif (!is_null($revision_id)) {
    $can['edit'] = FALSE;
    $can['history'] = (bool) $can_history_request;
    $can['agreement'] = FALSE;
    $can['clone'] = FALSE;
    $can['ical'] = FALSE;
  }
  else {
    $can['edit'] = (bool) $can_edit_request;
    $can['history'] = (bool) $can_history_request;
    $can['agreement'] = (bool) $can_agreement_request;
    $can['clone'] = (bool) $can_create_request;
    $can['ical'] = $status == MFCS_REQUEST_STATUS_CLOSED_ACCEPTED && $step == MFCS_REVIEW_STEP_COMPLETED;
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

  $form['form']['revision_id'] = array(
    '#type' => 'value',
    '#value' => $revision_id,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['form']['mode'] = array(
    '#type' => 'value',
    '#value' => $mode,
  );

  $form['form']['contained'] = array(
    '#type' => 'value',
    '#value' => $contained,
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

    if ($contained) {
      unset($form['display']['#type']);
      unset($form['display']['#collapsible']);
      unset($form['display']['#collapsed']);

      $form['display']['log']['#type'] = 'container';
      $form['display']['log']['#attributes']['id'] = 'request-' . $request_id . '-section-display';
    }

    $form['display']['groups'] = array(
      '#prefix' => '<div id="request-' . $request_id . '-field_groups" class="request-field_groups mode-' . $mode . ' no_print_links_as_link">',
      '#suffix' => '</div>',
    );

    $form['groups'] = array();
    $form['groups'] = array(
      '#tree' => TRUE,
    );

    $fields_count = array('top' => 0, 'group' => array());

    mfcs_include(MFCS_INCLUDE_TABLE);

    $presentation = mfcs_table_presentation();
    $presentation_tables = mfcs_table_presentation(1);

    $collate = array();

    foreach ($presentation_tables as $field_group => $presentation_table) {
      $can_for_section = $can;

      if ($can['edit']) {
        $can_for_section['edit'] = mfcs_request_access('edit', $request, $user, $field_group);
      }

      mfcs_request_view_0_form_setup_group_path($form, $request_id, $field_group, $presentation_tables, $can_for_section, $request);

      unset($can_for_section);
    }

    // pre-process special case of review decisions.
    $decision_presentation = $presentation['reviews']['decision'];
    unset($presentation['reviews']['decision']);

    $reviews = mfcs_get_latest_reviews($request_id, array(MFCS_REVIEW_STEP_REVIEW, MFCS_REVIEW_STEP_MAKE_DECISIONS), array(MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY));

    if (!empty($reviews)) {
      mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

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

        unset($reviewer_classification);
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

        unset($reviewer_classification);
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
        $row = mfcs_request_view_0_form_setup_group_path($form, $request_id, $field_group, $presentation_tables, $can, $request);
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
              $custom_row = mfcs_request_view_0_form_setup_group_path($form, $request_id, $group, $presentation_tables, $can, $request);
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

    if (!$contained) {
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

      if ($can_cancel_request && is_null($revision_id)) {
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
      elseif ($can_uncancel_request && is_null($revision_id)) {
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
          'mfcs_request_view_0_form_submit_pdf',
        ),
      );
    }
  }

  if ($show_log || $show_operations) {
    $form['review'] = array(
      '#tree' => TRUE,
    );

    mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

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
      if (!$contained) {
        $log_page_title_css = 'no-screen no-braille';
      }

      if ($mode == MFCS_VIEW_MODE_NORMAL) {
        $form['review']['log']['page_title'] = array(
          '#markup' => '<div id="' . 'request-' . $request_id . '-section-review-log-page_title' . '" class="request-section-review-log-page_title ' . $log_page_title_css . '">' . $page_title . '</div>',
          '#weight' => 0,
        );
      }

      if ($contained) {
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

      $decisions = mfcs_get_requests_current_review_decisions(array($request_id => NULL), $request['request_coordinator']['classification'][0]->value, $request['top']['step'][0]->value);

      $yet_to_review_markup = '';
      if (!empty($decisions[$request_id]) && is_array($decisions[$request_id])) {
        // preprocess and generate a list of reviewer classifications that have not yet reviewed.
        $yet_to_review = array();
        foreach ($decisions[$request_id]['needed'] as $classification_id => $classification_name) {
          if (array_key_exists($classification_id, $decisions[$request_id]['completed'])) {
            continue;
          }

          $yet_to_review[$classification_id] = $classification_name;
        }

        if (!empty($yet_to_review)) {
          $module_path = drupal_get_path('module', 'mfcs');
          drupal_add_js($module_path . '/js/request_view-reviewers.js', array('type' => 'file', 'group' => JS_DEFAULT, 'preprocess' => TRUE));

          $yet_to_review_markup .= '<div id="request-' . $request_id . '-section-review-log-yet_to_review" class="request-section-review-log-yet_to_review">' . "\n";
          $yet_to_review_markup .= '<div class="yet_to_review-label">Reviewer classifications that have not yet reviewed:</div>' . "\n";
          $yet_to_review_markup .= '<ul class="yet_to_review-list">' . "\n";

          foreach ($yet_to_review as $classification_id => $classification_name) {
            $yet_to_review_markup .= '<li class="yet_to_review-list_item">' . "\n";
            $yet_to_review_markup .= $classification_name . "\n";

            // create a javascript based users list to easily display the users associated with the given classification.
            $yet_to_review_markup .= '<div class="details_list inline" title="View list of users associated with this reviewer classification.">' . "\n";
            $yet_to_review_markup .= '<span class="no-script-hidden no-print details_list-label label-' . $classification_id . '">(<span class="details_list-link link-' . $classification_id . ' inline fake_link">Details</span>)</span>';
            $yet_to_review_markup .= '<ul class="script-hidden details_list-item item-' . $classification_id . '">' . "\n";
            if (!empty($decisions[$request_id]['reviewers'][$classification_id])) {
              foreach ($decisions[$request_id]['reviewers'][$classification_id] as $user_id) {
                $basic_user_settings = mfcs_load_basic_user_settings_by_user_id($user_id);
                if ($basic_user_settings === FALSE) {
                  continue;
                }

                $yet_to_review_markup .= '<li class="details_list-user">' . "\n";
                if (empty($basic_user_settings['first_name']) || empty($basic_user_settings['last_name'])) {
                  $yet_to_review_markup .= '<span class="details_list-user_name">' . $basic_user_settings['name'] . '</span>' . "\n";
                }
                else {
                  $yet_to_review_markup .= '<span class="details_list-user_name">' . "\n";
                  $yet_to_review_markup .= $basic_user_settings['first_name'] . ' ' . $basic_user_settings['last_name'] . "\n";
                  $yet_to_review_markup .= '</span>' . "\n";
                }
                $yet_to_review_markup .= '<span class="details_list-user_id">(id: ' . $user_id . ')</span>' . "\n";
                $yet_to_review_markup .= '</li>' . "\n";
              }
            }

            $yet_to_review_markup .= '</ul>' . "\n";
            $yet_to_review_markup .= '</div>' . "\n";
            $yet_to_review_markup .= '</li>' . "\n";
          }

          $yet_to_review_markup .= '</ul>' . "\n";
          $yet_to_review_markup .= '</div>' . "\n";
        }
      }

      $form['review']['log']['yet_to_review'] = array(
        '#markup' => $yet_to_review_markup,
        '#weight' => 2,
      );

      $form['review']['log']['history'] = array(
        '#prefix' => '<div id="request-' . $request_id . '-section-review-log-history" class="request-section-review-log-history mode-' . $mode . '">' . "\n",
        '#suffix' => '</div>' . "\n",
        '#weight' => 3,
      );

      mfcs_build_reviews_log($form, $form_state, $request_id, $revision_id);

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
          'mfcs_request_view_0_form_submit_pdf',
        ),
      );
    }

    if ($show_operations && !$contained && is_null($revision_id)) {
      $decision_options = mfcs_get_reviewer_decision_list_options('review_step');
      $decision_options_all = mfcs_get_reviewer_decision_list_options();

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

      if ($contained) {
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

      mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
      $reviewer_classification_options = mfcs_get_reviewer_classification_list_options();

      $reviewer_conditions = array(
        'mr.user_id' => $user->uid,
        'mr.request_classification' => $request_classification,
        'mr.disabled' => 0,
      );

      $reviewers_returned = mfcs_get_reviewers($reviewer_conditions);
      $reviewers = array();
      if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
        $reviewers = $reviewers_returned['results'];
      }
      unset($reviewers_returned);

      $form['review']['operations']['as_reviewer']['#markup'] = '<div class="request-section-review-operations-as_reviewer">';


      // determine the reviewers classifications.
      $reviewer_classification = array();
      $reviewer_classification_all = array();
      foreach ($reviewers as $reviewer) {
        if (!isset($reviewer->reviewer_classification)) {
          continue;
        }

        if (isset($reviewer->review_step) && $reviewer->review_step == $step) {
          $reviewer_classification[$reviewer->reviewer_classification] = $reviewer_classification_options[$reviewer->reviewer_classification];
        }

        $reviewer_classification_all[$reviewer->reviewer_classification] = $reviewer_classification_options[$reviewer->reviewer_classification];
      }

      if ($can_comment_request && !$can_review_request) {
        if (empty($reviewer_classification) && empty($venue_coordinator)) {
          if ($administer) {
            $reviewer_classification[MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR] = 'Administrator';
          }
          elseif ($manager) {
            $reviewer_classification[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = 'Manager';
          }
        }
      }


      // exception case: insurance provided must be able to be assigned at any point in time.
      $form['form']['insurance_provided_available'] = array(
        '#value' => FALSE,
      );

      $form['form']['insurance_provided_available_only'] = array(
        '#value' => FALSE,
      );

      $insurance_provided_available = &$form['form']['insurance_provided_available']['#value'];
      $insurance_provided_available_only = &$form['form']['insurance_provided_available_only']['#value'];
      if ($can_review_request && ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
        if ($status == MFCS_REQUEST_STATUS_LOCKED || $status == MFCS_REQUEST_STATUS_CLOSED_ACCEPTED) {
          if (isset($reviewer_classification_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING])) {
            if ((isset($request['insurance']['contractor'][0]->value) && $request['insurance']['contractor'][0]->value == 1) || (isset($request['insurance']['unaffiliated'][0]->value) && $request['insurance']['unaffiliated'][0]->value == 1)) {
              if (isset($request['insurance']['provided'][0]->value) && $request['insurance']['provided'][0]->value != 1) {
                $insurance_provided_available = TRUE;
              }
            }
          }
        }
      }

      // the venue coordinator must be reported as venue coordinator when not reviewing.
      if ($venue_coordinator) {
        if (empty($reviewer_classification) || ($step != MFCS_REVIEW_STEP_REVIEW && $step != MFCS_REVIEW_STEP_REQUIREMENTS)) {

          if ($request['venue_coordinator']['user_id'][0]->value == $user->uid) {
            $reviewer_classification[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
            $reviewer_classification_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
          }
          else {
            $reviewer_classification[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = "Venue Coordinator (Proxy)";
            $reviewer_classification_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = "Venue Coordinator (Proxy)";
          }
        }
      }

      // if the current user has no review classifications for the current step, then do not provide the "Approve/Deny/Comment" operations appropriately.
      if (empty($reviewer_classification)) {
        unset($decision_options[MFCS_REVIEW_DECISION_APPROVE]);
        unset($decision_options[MFCS_REVIEW_DECISION_DENY]);

        if (!$can_comment_request) {
          unset($decision_options[MFCS_REVIEW_DECISION_COMMENT]);
        }
      }

      // make the 'requirement' step available only under very specific situations.
      if ($insurance_provided_available) {
        $decision_options[MFCS_REVIEW_DECISION_REQUIREMENT] = $decision_options_all[MFCS_REVIEW_DECISION_REQUIREMENT];

        if ($step != MFCS_REVIEW_STEP_REVIEW) {
          $decision_options = array(
            MFCS_REVIEW_DECISION_REQUIREMENT => $decision_options_all[MFCS_REVIEW_DECISION_REQUIREMENT]
          );

          if ($can_comment_request) {
            $decision_options[MFCS_REVIEW_DECISION_COMMENT] = $decision_options_all[MFCS_REVIEW_DECISION_COMMENT];
          }

          $reviewer_classification[MFCS_REVIEWER_CLASSIFICATION_PURCHASING] = $reviewer_classification_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING];
          $insurance_provided_available_only = TRUE;
        }
      }
      elseif ($step != MFCS_REVIEW_STEP_VENUE_AVAILABLE && $step != MFCS_REVIEW_STEP_REVIEW) {
        unset($decision_options[MFCS_REVIEW_DECISION_REQUIREMENT]);
      }

      if (!empty($reviewer_classification)) {
        $form['review']['operations']['as_reviewer']['#markup'] .= '<div class="request-section-review-operations-as_reviewer-message">You are reviewing with the following classifications:</div>';
        $form['review']['operations']['as_reviewer']['#markup'] .= theme('item_list', array('items' => $reviewer_classification, 'type' => 'ul', 'attributes' => array('class' => array('request-section-review-operations-as_reviewer-list'))));
      }

      $form['review']['operations']['as_reviewer']['#markup'] .= '</div>';

      // restrict options to commenting only, if the user is only allowed to comment.
      if (!$can_review_request && $can_comment_request) {
        $decision_options = array(
          MFCS_REVIEW_DECISION_COMMENT => $decision_options_all[MFCS_REVIEW_DECISION_COMMENT],
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

      if ($can_review_request && ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $insurance_provided_available)) {
        $form['review']['operations']['requirements'] = array(
          '#tree' => TRUE,
          '#weight' => 2,
        );

        $form['form']['requirements-custodial'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $form['form']['requirements-maintenance'] = array(
          '#type' => 'value',
          '#value' => FALSE,
        );

        $form['form']['requirements-grounds'] = array(
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
            if ($reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_FACILITIES_CUSTODIAL || $reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL) {
              $uses['custodial'] = "Custodial";
              $form['form']['requirements-custodial']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE) {
              $uses['maintenance'] = "Maintenance";
              $form['form']['requirements-maintenance']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_FACILITIES) {
              $uses['grounds'] = "Grounds";
              $form['form']['requirements-grounds']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_SECURITY) {
              $uses['security'] = "Security";
              $form['form']['requirements-security']['#value'] = TRUE;
            }
            elseif ($reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_PURCHASING) {
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

        $requirements_amount_defaults = array('custodial' => '18.50', 'maintenance' => '18.50', 'grounds' => '18.50', 'security' => '30.00');
        $uses_personnel = array('custodial', 'maintenance', 'grounds', 'security');
        $uses_hours = array('custodial', 'maintenance', 'grounds', 'security');
        $uses_requirements = array('facilities' => TRUE, 'equipment' => FALSE, 'custodial' => TRUE, 'maintenance' => TRUE, 'grounds' => TRUE, 'security' => TRUE);
        $uses_review = array('facilities' => FALSE, 'equipment' => FALSE, 'custodial' => TRUE, 'maintenance' => TRUE, 'grounds' => TRUE, 'security' => TRUE);
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
              '#description' => "Other charges, unknown presently, will be provided to the presenter at a later time.",
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

        if ($insurance_requirement && ($step == MFCS_REVIEW_STEP_REVIEW || $insurance_provided_available)) {

          if (!$insurance_provided_available_only) {
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

          if ($insurance_provided_available_only) {
            $form['review']['operations']['insurance']['provided']['#required'] = TRUE;
          }

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

        if (!empty($request['requirements']['waived'][0])) {
          $form['review']['operations']['presidents_approval']['#default_value'] = $request['requirements']['waived'][0]->value;
        }

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

          if (!empty($request['requirements']['university'][0])) {
            $form['review']['operations']['university_business']['#default_value'] = $request['requirements']['university'][0]->value;
          }
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
 * @see: mfcs_request_view_0_form()
 */
function mfcs_request_view_0_form_setup_group_path(&$form, $request_id, $group, $presentation_tables, $show, $request) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

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
    elseif ($request['top']['status'][0]->value == MFCS_REQUEST_STATUS_DELETED) {
      $show['clone'] = FALSE;
    }
  }

  if ($show['ical']) {
    if ($group != 'dates') {
      $show['ical'] = FALSE;
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
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/create-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-history section-request-clone no-print" title="'. t("Copy Request") . '">Copy</a>';
        }

        if ($show['edit']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/edit-0/' . $request_id . '/' . $group . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-edit section-request-edit no-print" title="'. t("Edit Request") . '">Edit</a>';
        }

        if ($show['history']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/history-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-history section-request-history no-print" title="'. t("View History of Request") . '">History</a>';
        }

        if ($show['agreement']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/agreement-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-agreement section-request-agreement no-print" title="'. t("View Request") . '">Agreement</a>';
        }

        if ($show['ical']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/ical-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-ical section-request-ical no-print" title="'. t("Download iCal of Request") . '">iCal</a>';
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
 * Pre-Validation for mfcs_request_view_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_form()
 */
function mfcs_request_view_0_form_pre_validate(&$form, &$form_state) {
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
  $mode = $form['form']['mode']['#value'];
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
      $requirements_waived = FALSE;
      if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
        $requirements_waived = TRUE;
      }

      if (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
        $requirements_waived = TRUE;
      }

      $unrequire_usage_fields = $requirements_waived;
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
 * Validation for mfcs_request_view_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_form()
 */
function mfcs_request_view_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $form_state['rebuild'] = TRUE;

  $request = $form['form']['request']['#value'];
  $request_id = (int) $form['form']['request_id']['#value'];
  $mode = $form['form']['mode']['#value'];
  $user = cf_current_user();

  $can_review_request = mfcs_request_access('review', $request, $user);
  $can_comment_request = mfcs_request_access('comment', $request, $user);

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  $request_classification = $request['request_coordinator']['classification'][0]->value;
  $step = $request['top']['step'][0]->value;
  $status = $request['top']['status'][0]->value;
  $type = $request['information']['type'][0]->value;

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
      form_set_error('', "This view mode does not allow cancelling.");
    }

    if (!is_null($form['form']['revision_id']['#value'])) {
      form_set_error('', "Revisions can not be cancelled.");
    }

    $form_state['cancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'request-display-submit-uncancel') {
    if ($mode != MFCS_VIEW_MODE_NORMAL && $mode != MFCS_VIEW_MODE_DISPLAY) {
      form_set_error('', "This view mode does not allow uncancelling.");
    }

    if (!is_null($form['form']['revision_id']['#value'])) {
      form_set_error('', "Revisions can not be uncancelled.");
    }

    $form_state['uncancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'request-review-operations-submit-submit') {
    if (!is_null($form['form']['revision_id']['#value'])) {
      form_set_error('', "Revisions can not be changed or reviewed.");
    }

    $failure = FALSE;

    // only allow operations for states and steps that make sense to.
    $allowed_statuses = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
    );

    $allowed_steps = array(
      MFCS_REVIEW_STEP_VENUE_AVAILABLE,
      MFCS_REVIEW_STEP_REVIEW,
      MFCS_REVIEW_STEP_REQUIREMENTS,
      MFCS_REVIEW_STEP_MAKE_DECISIONS,
    );

    if (isset($form['form']['insurance_provided_available']['#value']) && $form['form']['insurance_provided_available']['#value']) {
      $allowed_statuses = array(
        MFCS_REQUEST_STATUS_LOCKED,
        MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
      );

      $allowed_steps = array(
        MFCS_REVIEW_STEP_REVIEW,
        MFCS_REVIEW_STEP_REQUIREMENTS,
        MFCS_REVIEW_STEP_MAKE_DECISIONS,
        MFCS_REVIEW_STEP_COMPLETED,
      );
    }

    if ($can_comment_request && $decision == MFCS_REVIEW_DECISION_COMMENT) {
      // if there are any cases where a comment cannot be posted, then is the place to add the restriction.
    }
    else {
      if (!in_array($status, $allowed_statuses)) {
        form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current status.");
        $failure = TRUE;
      }

      if (!in_array($step, $allowed_steps)) {
        form_set_error('', "Illegal operation detected. Operations cannot be performed for the request's current step.");
        $failure = TRUE;
      }
    }

    if (!($mode == MFCS_VIEW_MODE_NORMAL || $mode == MFCS_VIEW_MODE_DISPLAY || $mode == MFCS_VIEW_MODE_LOG || $mode == MFCS_VIEW_MODE_OPERATIONS)) {
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

    // A request was made to have the requirements waived options be displayed with the requirements to avoid confusion.
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
      if (isset($request['requirements']['waived'][0]->value)) {
        if ($request['requirements']['waived'][0]->value == 1) {
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

      if (array_key_exists('maintenance', $requirements) && !empty($requirements['maintenance']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements['maintenance']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][requirements][maintenance][amount', t("@maintenance is not a valid maintenance requirement amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@maintenance' => $requirements['maintenance']['amount'])));
          $failure = TRUE;
        }

        if (isset($requirements['maintenance']['quantity']) && intval($requirements['maintenance']['quantity']) != $requirements['maintenance']['quantity']) {
          form_set_error('review][operations][requirements][maintenance][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $requirements['maintenance']['quantity'])));
          $failure = TRUE;
        }

        if (isset($requirements['maintenance']['hours']) && !cf_is_integer($requirements['maintenance']['hours'])) {
          form_set_error('review][operations][requirements][maintenance][hours', t("@hours is not an valid number. Use only numbers.", array('@hours' => $requirements['maintenance']['hours'])));
          $failure = TRUE;
        }

        if (isset($requirements['maintenance']['days']) && intval($requirements['maintenance']['days']) != $requirements['maintenance']['days']) {
          form_set_error('review][operations][requirements][maintenance][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $requirements['maintenance']['days'])));
          $failure = TRUE;
        }
      }

      if (array_key_exists('grounds', $requirements) && !empty($requirements['grounds']['amount'])) {
        $matches = array();
        $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements['grounds']['amount'], $matches);

        if (!$matched) {
          form_set_error('review][operations][requirements][grounds][amount', t("@grounds is not a valid grounds requirement amount. Use only numbers and two decimal places. Do not provide a dollar sign.", array('@grounds' => $requirements['grounds']['amount'])));
          $failure = TRUE;
        }

        if (isset($requirements['grounds']['quantity']) && intval($requirements['grounds']['quantity']) != $requirements['grounds']['quantity']) {
          form_set_error('review][operations][requirements][grounds][quantity', t("@quantity is not an allowed number. Use only numbers without decimal places.", array('@quantity' => $requirements['grounds']['quantity'])));
          $failure = TRUE;
        }

        if (isset($requirements['grounds']['hours']) && !cf_is_integer($requirements['grounds']['hours'])) {
          form_set_error('review][operations][requirements][grounds][hours', t("@hours is not an valid number. Use only numbers.", array('@hours' => $requirements['grounds']['hours'])));
          $failure = TRUE;
        }

        if (isset($requirements['grounds']['days']) && intval($requirements['grounds']['days']) != $requirements['grounds']['days']) {
          form_set_error('review][operations][requirements][grounds][days', t("@days is not an allowed number. Use only numbers without decimal places.", array('@days' => $requirements['grounds']['days'])));
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

    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_APPROVE && $type != MFCS_REQUEST_TYPE_QUICK_MEETING) {
      $requirements_not_supplied = TRUE;

      if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
        if (isset($request['requirements']['waived'][0]->value)) {
          if ($request['requirements']['waived'][0]->value) {
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
        form_set_error('review][operations][decision', t("The request cannot be approved until the requirements are either specified or waived."));
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

      $maintenance_requirements_required = FALSE;
      if (isset($form['form']['requirements-maintenance']['#value'])) {
        $maintenance_requirements_required = $form['form']['requirements-maintenance']['#value'];
      }

      $grounds_requirements_required = FALSE;
      if (isset($form['form']['requirements-grounds']['#value'])) {
        $grounds_requirements_required = $form['form']['requirements-grounds']['#value'];
      }

      $security_requirements_required = FALSE;
      if (isset($form['form']['requirements-security']['#value'])) {
        $security_requirements_required = $form['form']['requirements-security']['#value'];
      }

      $insurance_requirements_required = FALSE;
      if (isset($form['form']['requirements-insurance']['#value'])) {
        $insurance_requirements_required = $form['form']['requirements-insurance']['#value'];
      }

      if (!$custodial_requirements_required && !$maintenance_requirements_required && !$grounds_requirements_required && !$security_requirements_required && !$insurance_requirements_required) {
        $requirements_not_supplied = FALSE;
        $requirements_exist = FALSE;
      }

      // @fixme: the logic of these functions does not seem correct. Should it be !isset(...->amount)?
      if (isset($form['review']['operations']['requirements']['custodial'])) {
        if (isset($request['requirements']['custodial'][0]->amount)) {
          $requirements_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['requirements']['maintenance'])) {
        if (isset($request['requirements']['maintenance'][0]->amount)) {
          $requirements_not_supplied = FALSE;
        }
      }

      if (isset($form['review']['operations']['requirements']['grounds'])) {
        if (isset($request['requirements']['grounds'][0]->amount)) {
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
    }

    if (!$failure) {
      $form_state['rebuild'] = FALSE;
    }
  }
  elseif ($clicked_id == 'request-cancel-submit-yes' || $clicked_id == 'request-cancel-submit-no') {
    if (!is_null($form['form']['revision_id']['#value'])) {
      form_set_error('', "Revisions can not be cancelled.");
      return;
    }

    $form_state['rebuild'] = FALSE;
  }
  elseif ($clicked_id == 'request-uncancel-submit-yes' || $clicked_id == 'request-uncancel-submit-no') {
    if (!is_null($form['form']['revision_id']['#value'])) {
      form_set_error('', "Revisions can not be uncancelled.");
    }

    $form_state['rebuild'] = FALSE;
  }
}

/**
 * Submit process for mfcs_request_view_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_form()
 */
function mfcs_request_view_0_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

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
    $deleted = mfcs_include(MFCS_INCLUDE_REQUEST_CANCEL);

    mfcs_request_cancel_0_form_submit($form, $form_state);

    return;
  }
  elseif ($clicked_id == 'request-uncancel-submit-yes') {
    $deleted = mfcs_include(MFCS_INCLUDE_REQUEST_CANCEL);

    mfcs_request_uncancel_0_form_submit($form, $form_state);

    return;
  }
  elseif ($clicked_id == 'request-review-operations-submit-submit') {
    $success = TRUE;

    mfcs_include(MFCS_INCLUDE_WORKFLOW);
    $decision = $form_state['values']['review']['operations']['decision'];
    $message = $form_state['values']['review']['operations']['message'];

    $request = $form['form']['request']['#value'];

    $request_classification = $request['request_coordinator']['classification'][0]->value;
    $step = $request['top']['step'][0]->value;
    $type = $request['information']['type'][0]->value;

    $insurance_provided_available = FALSE;
    if (isset($form['form']['insurance_provided_available']['#value'])) {
      $insurance_provided_available = (bool) $form['form']['insurance_provided_available']['#value'];
    }

    $extra = array();
    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW) {
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        $extra['requirements'] = $form_state['values']['review']['operations']['requirements'];
      }

      if (isset($form_state['values']['review']['operations']['insurance'])) {
        $extra['insurance'] = $form_state['values']['review']['operations']['insurance'];
      }
    }

    if ($insurance_provided_available) {
      if (isset($form_state['values']['review']['operations']['insurance']['provided'])) {
        if (!isset($extra['insurance'])) {
          $extra['insurance'] = array();
        }

        $extra['insurance']['provided'] = $form_state['values']['review']['operations']['insurance']['provided'];
      }
    }

    if ($decision == MFCS_REVIEW_DECISION_COMMENT) {
      // comments do not need to be processed on a per step basis, so skip the $step conditions below.
    }
    elseif ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if (isset($form_state['values']['review']['operations']['presidents_approval'])) {
        $extra['requirements']['waived'] = $form_state['values']['review']['operations']['presidents_approval'];

        if ($extra['requirements']['waived'] == 1) {
          $extra['requirements']['other'] = 0;
        }
      }

      if (isset($form_state['values']['review']['operations']['university_business'])) {
        $extra['requirements']['university'] = $form_state['values']['review']['operations']['university_business'];

        if ($extra['requirements']['university'] == 1) {
          $extra['requirements']['other'] = 0;
        }
      }

      // when the request type is a quick meeting, then mark this as waived due to university business.
      if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING) {
        $form_state['values']['review']['operations']['university_business'] = 1;
        unset($extra['requirements']);
        $requirements_extra = array(
          'send_email' => FALSE,
          'requirements' => array('other' => 0, 'waived' => 0, 'university' => 1),
        );

        $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], MFCS_REVIEW_DECISION_REQUIREMENT, "", $requirements_extra);
      }
    }
    elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
      if (isset($form_state['autowave']) && $form_state['autowave']) {
        $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], MFCS_REVIEW_DECISION_WAVE, "", array('send_email' => FALSE));
      }
    }

    if ($success) {
      $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], $decision, $message, $extra);
    }

    if ($success) {
      if ($decision == MFCS_REVIEW_DECISION_COMMENT) {
        drupal_set_message("Your comment has been received.");
      }
      else {
        drupal_set_message("Your review has been received.");
      }
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
    elseif (isset($form['form']['insurance_provided_available']['#value']) && $form['form']['insurance_provided_available']['#value']) {
      $form_state['values']['redirect_to'] = 'requests/view-0/' .  $request_id;
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      if (empty($form['form']['mode']['#value'])) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
      }
      else {
        if ($form['form']['mode']['#value'] == MFCS_VIEW_MODE_OPERATIONS) {
          if (mfcs_page_request_access('review', $request_id)) {
            $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id . '/' . $form['form']['mode']['#value']);
          }
          else {
            $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
          }
        }
        else {
          $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id . '/' . $form['form']['mode']['#value']);
        }
      }
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Custom submit process for mfcs_request_view_0_form().
 *
 * Used for generating a print PDF.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_form()
 */
function mfcs_request_view_0_form_submit_pdf($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if (($clicked_id != 'request-display-submit-download' && $clicked_id != 'request-review-log-submit-download') || empty($request_id)) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
    }

    return;
  }

  global $base_path;

  $mode = $form['form']['mode']['#value'];
  $revision_id = $form['form']['revision_id']['#value'];

  if (is_null($revision_id)) {
    $form_state['redirect'] = mfcs_build_redirect_array('/requests/pdf-0/' . $request_id . '/' . $mode);
  }
  else {
    $form_state['redirect'] = mfcs_build_redirect_array('/requests/pdf-0/' . $request_id . '/' . $mode . '/' . $revision_id);
  }
}

/**
 * Renders a PDF of a given request.
 *
 * @param int $request_id
 *   The unique identifier for a request.
 * @param int|null $mode
 *   Specifies the mode that alters how the form is rendered.
 *   1: Render only the display part.
 * @param null|int $revision_id
 *   (optional) When not NULL, load the request based on the revision number
 *   instead of the latest revision.
 *
 * @return exit
 *   This function should not return.
 */
function mfcs_request_pdf_0_page($request_id, $mode, $revision_id = NULL) {
  $page_title = "Facilities Use Request";
  drupal_set_title($page_title);

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    drupal_not_found();
    drupal_exit();
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $revision_id);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_null($revision_id)) {
    $user = cf_current_user();

    $can_history_request = mfcs_request_access('history', $request, $user);

    if (!$can_history_request) {
      drupal_not_found();
      drupal_exit();
    }
  }

  if (is_null($revision_id)) {
    if ($mode == MFCS_VIEW_MODE_NORMAL) {
      mfcs_add_canonical_headers('requests/pdf-0/' . $request_id . '/' . $mode, 'requests/pdf-0/' . $request_id);
    }
    else {
      mfcs_add_canonical_headers('requests/pdf-0/' . $request_id . '/' . $mode);
    }
  }
  else {
    mfcs_add_canonical_headers('requests/pdf-0/' . $request_id . '/' . $mode . '/' . $revision_id);
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
      'id' => 'mfcs-request-view-0-form',
      'class' => array(
        'mfcs-request-pdf-0',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $page['html']['content'] = array(mfcs_request_view_0_form($form, $form_state, $request_id, $mode, $revision_id, TRUE));
  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array(
    'page_title' => "Facilities Use Request: " . $request['information']['title'][0]->value,
    'title' => 'Request #' . $request_id . ': ' . $request['information']['title'][0]->value,
  );

  $contained = mfcs_output_print_page_view($html, $parameters);

  if (!is_string($contained)) {
    drupal_not_found();
    drupal_exit();
  }

  $result = mfcs_request_pdf_0_page_using_wkhtmltopdf($request_id, $revision_id, $contained);

  if ($result) {
    exit();
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * Renders a self-contained/printrable view of a given request.
 *
 * @param int $request_id
 *   The unique identifier for a request.
 * @param int|null $mode
 *   Specifies the mode that alters how the form is rendered.
 *   1: Render only the display part.
 * @param null|int $revision_id
 *   (optional) When not NULL, load the request based on the revision number
 *   instead of the latest revision.
 *
 * @return exit
 *   This function should not return.
 */
function mfcs_request_print_0_page($request_id, $mode, $revision_id = NULL) {
  $page_title = "Facilities Use Request";
  drupal_set_title($page_title);

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    drupal_not_found();
    drupal_exit();
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $revision_id);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($revision_id)) {
    $user = cf_current_user();

    $can_history_request = mfcs_request_access('history', $request, $user);

    if (!$can_history_request) {
      drupal_not_found();
      drupal_exit();
    }
  }

  if (is_null($revision_id)) {
    if ($mode == MFCS_VIEW_MODE_NORMAL) {
      mfcs_add_canonical_headers('requests/print-0/' . $request_id . '/' . $mode, 'requests/print-0/' . $request_id);
    }
    else {
      mfcs_add_canonical_headers('requests/print-0/' . $request_id . '/' . $mode);
    }
  }
  else {
    mfcs_add_canonical_headers('requests/print-0/' . $request_id . '/' . $mode . '/' . $revision_id);
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
      'id' => 'mfcs-request-view-0-form',
      'class' => array(
        'mfcs-request-print-0',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $page['html']['content'] = array(mfcs_request_view_0_form($form, $form_state, $request_id, $mode, $revision_id, TRUE));
  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array(
    'page_title' => "Facilities Use Request: " . $request['information']['title'][0]->value,
    'title' => 'Request #' . $request_id . ': ' . $request['information']['title'][0]->value,
    'all_media' => TRUE,
  );

  $contained = mfcs_output_print_page_view($html, $parameters);

  if (is_string($contained)) {
    drupal_send_headers();
    print($contained);
    exit();
  }

  drupal_not_found();
  drupal_exit();
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
 * @param null|int $revision_id
 *   (optional) When not NULL, load the request based on the revision number
 *   instead of the latest revision.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_build_reviews_log(&$form, &$form_state, $request_id, $revision_id = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    drupal_not_found();
    drupal_exit();
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $revision_id);

  if (empty($request)) {
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_TABLE);

  $presentation = mfcs_table_presentation();

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $decision_options = mfcs_get_reviewer_decision_list_options();

  try {
    $query = db_select('mfcs_field_review_review', 'mfrr');

    $query->fields('mfrr');

    $query->condition('mfrr.request_id', $request_id);

    if (!is_null($revision_id) && isset($request['mer']['updated'][0]->value)) {
      $query->condition('mfrr.date', $request['mer']['updated'][0]->value, '<=');
    }

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
    'maintenance',
    'grounds',
    'security',
    'other',
    'waived',
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
    $type = $request['information']['type'][0]->value;

    foreach ($reviews as $row => $review) {
      $user_id = $review->user_id;
      $review_step = $review->step;
      $decision = $review->decision;
      $message = $review->message;
      $date = $review->date;

      $user_can_administer = mfcs_user_can_administer($user_id);
      $user_can_manage = mfcs_user_can_manage($user_id);

      $changelog = mfcs_load_request_by_timestamp($request_id, $date, $user_id, $changelog_fields);

      if (!array_key_exists($user_id, $users)) {
        $users[$user_id] = user_load($user_id);

        if (!is_object($users[$user_id])) {
          $users[$user_id] = NULL;
        }
      }

      $markup = '<div class="review-item review-item-' . $row. '">';

      $user_name = '';
      if (is_null($users[$user_id])) {
        watchdog(MFCS_WATCHDOG_ID, "Failed to load the user id of %user_id for request %request_id review log dated %date.", array('%user_id' => $user_id, '%request_id' => $request_id, '%date' => date("Y/m/d h:i:s a", $date)), WATCHDOG_ERROR);
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

        if ($classification == MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR) {
          $reviewer_classification .= "Venue Coordinator";
        }
        elseif ($classification == MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY) {
          $reviewer_classification .= "Venue Coordinator (Proxy)";
        }
        elseif ($classification == MFCS_REVIEWER_CLASSIFICATION_MANAGER) {
          $reviewer_classification .= "Manager";
        }
        elseif ($classification == MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR) {
          $reviewer_classification .= "System Administrator";
        }
        else {
          $reviewer_classification .= $reviewer_classification_options[$classification];
        }
      }

      if (is_null($reviewer_classification)) {
        if ($user_can_administer) {
          $reviewer_classification = "Administrator";
        }
        elseif ($user_can_manage) {
          $reviewer_classification = "Manager";
        }
      }

      if (!is_null($reviewer_classification)) {
        $reviewer_classification = ' as <strong class="review-item-label-text-row-reviewer_classification">'. $reviewer_classification . '</strong>';
      }

      if ($user_id == 1) {
        $user_name .= "System";
        $reviewer_classification = "";
      }
      else {
        if (isset($users[$user_id]->field_user_first_name['und'][0]['value'])) {
          $user_name .= $users[$user_id]->field_user_first_name['und'][0]['value'];

          if (isset($users[$user_id]->field_user_last_name['und'][0]['value'])) {
            $user_name .= ' ' . $users[$user_id]->field_user_last_name['und'][0]['value'];
          }
        }
        elseif (isset($users[$user_id]->field_user_last_name['und'][0]['value'])) {
          $user_name .= ' ' . $users[$user_id]->field_user_last_name['und'][0]['value'];
        }
        else {
          $user_name .= $users[$user_id]->name;
        }
      }

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
        if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING) {
          $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>, <strong class="review-item-label-text-row-decision">Confirmed</strong>, and <strong class="review-item-label-text-row-decision">Reserved</strong>' . $reviewer_classification . '.';
        }
        else {
          $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>' . $reviewer_classification . '.';
        }
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
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has waived the <strong class="review-item-label-text-row-decision">' . $decision_options[MFCS_REVIEW_DECISION_REQUIREMENT] . '</strong> ' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has accepted the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has denied the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' assigned the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> for the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_APPROVE && $type == MFCS_REQUEST_TYPE_QUICK_MEETING) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>, <strong class="review-item-label-text-row-decision">Confirmed</strong>, and <strong class="review-item-label-text-row-decision">Reserved</strong>' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_CLOSED) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Closed</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_CANCELLED) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Cancelled</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_UNCANCELLED) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Uncancelled</strong> the request' . $reviewer_classification . '.';
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

      if ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_REQUIREMENT || $decision == MFCS_REVIEW_DECISION_WAVE) && $type != MFCS_REQUEST_TYPE_QUICK_MEETING ) {
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

      if (!empty($changelog['requirements']['maintenance'])) {
        $message .= '<div class="review-item-message-requirements-maintenance review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Maintenance Fees:</div> ';

        $value = reset($changelog['requirements']['maintenance']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'maintenance', 0, $value, $presentation['requirements']['maintenance'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['grounds'])) {
        $message .= '<div class="review-item-message-requirements-grounds review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Grounds Fees:</div> ';

        $value = reset($changelog['requirements']['grounds']);
        $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'grounds', 0, $value, $presentation['requirements']['grounds'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_REQUIREMENT || $decision == MFCS_REVIEW_DECISION_WAVE) || ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
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

        if (!empty($changelog['requirements']['waived'])) {
          $message .= '<div class="review-item-message-requirements-waived review-item-message-requirements review-item-message-requirements">';
          $message .= '<div class="item-label">Waived with Presidents Approval:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['requirements']['waived']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['requirements']['waived']);
          $value_markup = mfcs_build_request_value_structure($request_id, 'requirements', 'waived', 0, $value, $presentation['requirements']['waived'], $changelog);

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
