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
 * @param null|int $use_revision
 *   (optional) When not NULL, load the request based on the revision number
 *   instead of the latest revision.
 * @param bool $contained
 *   (optional) Enable special handling for PDF and print generation.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_view_0_form($form, &$form_state, $request_id, $mode = MFCS_VIEW_MODE_NORMAL, $use_revision = NULL, $contained = FALSE) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_string($mode) || !($mode == MFCS_VIEW_MODE_DISPLAY || $mode == MFCS_VIEW_MODE_LOG || $mode == MFCS_VIEW_MODE_OPERATIONS)) {
    $mode = MFCS_VIEW_MODE_NORMAL;
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $use_revision);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $user = cf_current_user();

  if (is_null($use_revision)) {
    $can_create_request = mfcs_request_access('create', $request, $user);
    $can_edit_request = mfcs_request_access('edit', $request, $user);
    $can_log_request = mfcs_request_access('log', $request, $user);
    $can_review_request = mfcs_request_access('review', $request, $user);
    $can_comment_request = mfcs_request_access('comment', $request, $user);
    $can_history_request = mfcs_request_access('history', $request, $user);
    $can_agreement_request = mfcs_request_access('agreement', $request, $user);
    $can_cancel_request = mfcs_request_access('cancel', $request, $user);
    $can_uncancel_request = mfcs_request_access('uncancel', $request, $user);
    $can_manage_request = mfcs_request_access('manage', $request, $user);
    $can_override_request = mfcs_request_access('override', $request, $user);
    $can_reassign_request = mfcs_request_access('reassign', $request, $user);
    $can_cancelling_request = mfcs_request_access('cancelling', $request, $user);
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
    $can_manage_request = mfcs_request_access('manage', $request_latest, $user);
    $can_override_request = mfcs_request_access('override', $request_latest, $user);
    $can_reassign_request = mfcs_request_access('reassign', $request_latest, $user);
    $can_cancelling_request = mfcs_request_access('cancelling', $request_latest, $user);

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

  if (is_null($use_revision)) {
    if ($mode == MFCS_VIEW_MODE_NORMAL) {
      mfcs_add_canonical_headers('requests/view-0/' . $request_id . '/' . $mode, 'requests/view-0/' . $request_id);
    }
    else {
      mfcs_add_canonical_headers('requests/view-0/' . $request_id . '/' . $mode);
    }
  }
  else {
    mfcs_add_canonical_headers('requests/view-0/' . $request_id . '/' . $mode . '/' . $use_revision);
  }

    if (!is_string($mode) || !($mode == MFCS_VIEW_MODE_DISPLAY || $mode == MFCS_VIEW_MODE_LOG || $mode == MFCS_VIEW_MODE_OPERATIONS)) {
    $mode = MFCS_VIEW_MODE_NORMAL;
  }

  $page_title = "Facilities Use Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Facilities Use Request: " . $request['information']['title'][0]->value;
  }

  if ($contained === FALSE) {
    if ($mode == MFCS_VIEW_MODE_LOG) {
      $page_title = "Request Review Log";
      if (!empty($request['information']['title'][0]->value)) {
        $page_title = "Request Review Log: " . $request['information']['title'][0]->value;
      }
    }
    elseif ($mode == MFCS_VIEW_MODE_OPERATIONS) {
      $page_title = "Request Review Operations";
      if (!empty($request['information']['title'][0]->value)) {
        $page_title = "Request Review Operations: " . $request['information']['title'][0]->value;
      }
    }
  }

  drupal_set_title($page_title);

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

  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
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
    $can_manage_request = FALSE;
    $can_reassign_request = FALSE;
  }
  elseif ($mode == MFCS_VIEW_MODE_OPERATIONS) {
    $show_display = FALSE;
    $show_log = FALSE;
    $can_manage_request = FALSE;
    $can_reassign_request = FALSE;
  }

  if (!$can_log_request) {
    $show_log = FALSE;

    if ($mode == MFCS_VIEW_MODE_LOG) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }
  }

  if ($can_reassign_request && is_null($use_revision)) {
    if ($show_display) {
      $disabled_with_active = mfcs_disabled_venue_coordinators_with_active_content($request['venue_coordinator']['user_id'][0]->value);
      if (!empty($disabled_with_active)) {
        $user_venue_coordinator = mfcs_load_basic_user_settings_by_user_id($request['venue_coordinator']['user_id'][0]->value);

        if (!empty($user_venue_coordinator['name'])) {
          global $base_path;

          if (!empty($user_venue_coordinator['first_name']) && !empty($user_venue_coordinator['last_name'])) {
            if ($manager || $administer) {
              $user_venue_coordinator_name = '<a href="' . $base_path . 'requests/users-0/view/' . $request['venue_coordinator']['user_id'][0]->value . '" title="User ID: ' . $request['venue_coordinator']['user_id'][0]->value .'">' . $user_venue_coordinator['first_name'] . ' ' . $user_venue_coordinator['last_name'] . '</a>';
            }
            else {
              $user_venue_coordinator_name = $user_venue_coordinator['first_name'] . ' ' . $user_venue_coordinator['last_name'];
            }
          }
          else {
            if ($manager || $administer) {
              $user_venue_coordinator_name = '<a href="' . $base_path . 'requests/users-0/view/' . $request['venue_coordinator']['user_id'][0]->value . '" title="User ID: ' . $request['venue_coordinator']['user_id'][0]->value .'">' . $user_venue_coordinator['name'] . '</a>';
            }
            else {
              $user_venue_coordinator_name = $user_venue_coordinator['name'];
            }
          }

          drupal_set_message("Warning: The Venue Coordinator (" . $user_venue_coordinator_name . ") assigned to this request no longer has access to this system.", 'warning', FALSE);
        }
        else {
          drupal_set_message("Warning: The Venue Coordinator assigned to this request no longer has access to this system.", 'warning', FALSE);
        }
      }
      else {
        $can_reassign_request = FALSE;
      }
    }
    else {
      $can_reassign_request = FALSE;
    }
  }

  $step = $request['top']['step'][0]->value;
  $status = $request['top']['status'][0]->value;

  if (is_string($step)) {
    $step = (int) $step;
  }

  if (is_string($status)) {
    $status = (int) $status;
  }


  if ($can_review_request || $can_comment_request) {
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
    'cancel' => FALSE,
    'uncancel' => FALSE,
  );

  if ($contained) {
    $can['edit'] = FALSE;
    $can['history'] = FALSE;
    $can['agreement'] = FALSE;
    $can['clone'] = FALSE;
    $can['ical'] = FALSE;
    $can['cancelling'] = FALSE;
  }
  elseif (!is_null($use_revision)) {
    $can['edit'] = FALSE;
    $can['history'] = (bool) $can_history_request;
    $can['agreement'] = FALSE;
    $can['clone'] = FALSE;
    $can['ical'] = FALSE;
    $can['cancelling'] = FALSE;
  }
  else {
    $can['edit'] = (bool) $can_edit_request;
    $can['history'] = (bool) $can_history_request;
    $can['agreement'] = (bool) $can_agreement_request;
    $can['clone'] = (bool) $can_create_request;
    $can['ical'] = FALSE;
    $can['cancelling'] = (bool) $can_cancelling_request;

    if (($status === MFCS_REQUEST_STATUS_CLOSED_ACCEPTED || $status === MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED) && $step === MFCS_REVIEW_STEP_COMPLETED) {
      $can['ical'] = TRUE;
    }

    if ($can['cancelling'] && count($request['dates']['date']) == 1) {
      // hide/prevent cancelling when there is only one date.
      $can['cancelling'] = FALSE;
    }
  }


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
    '#value' => $use_revision,
  );

  if (is_null($use_revision)) {
    $form['form']['revision_id']['#value'] = $request['mer']['revision'][0]->value;
  }

  $form['form']['use_revision'] = array(
    '#type' => 'value',
    '#value' => $use_revision,
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

  $form['form']['already_reviewed_as'] = array(
    '#type' => 'value',
    '#value' => array(),
  );

  if (!isset($request['information']['type'][0]->value) || !cf_is_integer($request['information']['type'][0]->value)) {
    cf_error::invalid_integer('request[information][type][0]->value');
    $form['message'] = array(
      '#markup' => "Error: Unable to load request (ID = " . check_plain($request_id) . ") due to missing request type value.",
    );

    return $form;
  }

  // show review operations only if the content is reviewable by the current user.
  $request_classification = $request['request_coordinator']['classification'][0]->value;
  if (is_string($request_classification)) {
    $request_classification = (int) $request_classification;
  }

  $request_type = $request['information']['type'][0]->value;
  if (is_string($request_type)) {
    $request_type = (int) $request_type;
  }

  if ($show_display) {
    mfcs_include(MFCS_INCLUDE_VIEW);

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

      mfcs_view_setup_group_path($form, $request_id, $field_group, $presentation_tables, $can_for_section, $request);

      unset($can_for_section);
    }

    // pre-process special case of review decisions.
    $decision_presentation = $presentation['reviews']['decision'];
    if (is_string($decision_presentation)) {
      $decision_presentation = (int) $decision_presentation;
    }
    unset($presentation['reviews']['decision']);

    $reviews = mfcs_get_latest_reviews($request_id, array(MFCS_REVIEW_STEP_VENUE_AVAILABLE, MFCS_REVIEW_STEP_REVIEW, MFCS_REVIEW_STEP_REQUIREMENTS, MFCS_REVIEW_STEP_COMPLETED), array(MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY));

    if (!empty($reviews['order']) && isset($reviews['group'])) {
      mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

      $classification_options = mfcs_get_reviewer_classification_list_options(NULL, TRUE, TRUE);
      $preprocessed_decisions = array();

      // do not use the none value
      unset($classification_options[MFCS_REVIEWER_CLASSIFICATION_NONE]);

      foreach ($reviews['order'] as $review) {
        if (is_string($review->classification)) {
          $reviewer_classification = (int) $review->classification;
        }
        else {
          $reviewer_classification = $review->classification;
        }

        if (!isset($classification_options[$reviewer_classification])) {
          continue;
        }

        // the last decision made is the first item and should be the only value displayed.
        if (is_string($review->decision)) {
          $decision = (int) $review->decision;
        }
        else {
          $decision = $review->decision;
        }

        $preprocessed_decisions[$reviewer_classification] = array(0 => $review);
      }
      unset($decision);
      unset($reviewer_classification);
      unset($review);

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
      // @fixme: this needs to be rewritten to ensure consistency with "Approved/Denied" and not display "Issues/No Issues".
      $decision_step = array();
      if (isset($reviews['group'][MFCS_REVIEW_STEP_REQUIREMENTS])) {
        $decision_step = $reviews['group'][MFCS_REVIEW_STEP_REQUIREMENTS];
      }

      if (!empty($decision_step)) {
        foreach ($decision_step as $reviewer_classification => $reviewer_decisions) {
          if (is_string($reviewer_classification)) {
            $reviewer_classification = (int) $reviewer_classification;
          }

          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          // the last decision made is the first item and should be the only value displayed.
          $review = array_shift($reviewer_decisions);
          $decision = (int) $review->decision;

          $key_string = 'classification-final_decision';
          $presentation['reviews'][$key_string] = $decision_presentation;
          $presentation['reviews'][$key_string]['label'] = "Final Decision";
          $presentation['reviews'][$key_string]['weight'] = $weight;

          $request['reviews'][$key_string] = array(0 => $review);
          unset($decision);
          unset($review);

          $weight++;
          break;
        }

        unset($reviewer_classification);
      }
      unset($decision_step);
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
        $row = mfcs_view_setup_group_path($form, $request_id, $field_group, $presentation_tables, $can, $request);
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
              $custom_row = mfcs_view_setup_group_path($form, $request_id, $group, $presentation_tables, $can, $request);
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

          $group_path[$group][$weight][$field_presentation_name] = mfcs_view_build_request_item_structure($request_id, $group, $field_presentation_name, $values, $field_presentation, $request);

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

    unset($key);
    unset($value);

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

      if ($can_cancel_request && is_null($use_revision)) {
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
            'title' => "Cancel this request.",
          ),
          '#submit' => array(
            'mfcs_request_cancel_0_form_submit',
          ),
        );
      }
      elseif ($can_uncancel_request && is_null($use_revision)) {
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
            'title' => "Uncancel this request.",
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
          'title' => "Send this request to your printer.",
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
          'title' => "View or Download this request as a PDF.",
        ),
        '#submit' => array(
          'mfcs_request_view_0_form_submit_pdf',
        ),
      );

      if ($can_manage_request && is_null($use_revision)) {
        $form['display']['submit']['manage'] = array(
          '#id' => 'request-display-submit-manage',
          '#name' => 'request-display-submit-manage',
          '#type' => 'link',
          '#title' => t("Manage"),
          '#href' => 'requests/manage-0/' . $request_id . $url_arguments,
          '#attributes' => array(
            'class' => array(
              'no-print',
              'button',
              'request-display-submit-manage',
              'mode-' . $mode,
            ),
            'title' => "Perform management tasks on this request.",
          ),
        );
      }

      if ($can_reassign_request && is_null($use_revision)) {
        $form['display']['submit']['reassign'] = array(
          '#id' => 'request-display-submit-reassign',
          '#name' => 'request-display-submit-reassign',
          '#type' => 'link',
          '#title' => t("Re-Assign"),
          '#href' => 'requests/manage-0/reassign/' . $request_id . $url_arguments,
          '#attributes' => array(
            'class' => array(
              'no-print',
              'button',
              'request-display-submit-reassign',
              'mode-' . $mode,
            ),
            'title' => "Re-assign the Venue Coordinator.",
          ),
        );
      }
    }
  }

  if ($show_log || $show_operations) {
    mfcs_include(MFCS_INCLUDE_VIEW);

    $form['review'] = array(
      '#tree' => TRUE,
    );

    $form['review']['revision'] = array(
      '#type' => 'hidden',
      '#value' => (int) $request['mer']['revision'][0]->value,
    );

    // When a form is submitted, the current data at generation time is stored in the form_state array.
    // Read the value and if it exists assign it instead of the value from the form.
    // This allows for detecting changes since the last time the page was loaded.
    if (isset($form_state['input']['review']['revision'])) {
      $form['review']['revision']['#value'] = (int) $form_state['input']['review']['revision'];
    }

    mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

    $step_options = mfcs_get_review_steps_list_options();

    $decisions = array();
    if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
      $decisions = mfcs_get_requests_current_review_decisions(array($request_id => NULL), $request['request_coordinator']['classification'][0]->value, $request['top']['step'][0]->value);
    }

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

      $yet_to_review_markup = '';
      if ((!empty($decisions[$request_id]) && is_array($decisions[$request_id])) || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
        // preprocess and generate a list of reviewer classifications that have not yet reviewed.
        $yet_to_review = array();

        if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
          foreach ($decisions[$request_id]['needed'] as $classification_id => $classification_name) {
            if (array_key_exists($classification_id, $decisions[$request_id]['completed'])) {
              continue;
            }

            $yet_to_review[$classification_id] = $classification_name;
          }
        }
        elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
          $yet_to_review[$request['venue_coordinator']['user_id'][0]->value] = "Venue Coordinator";
        }

        if (!empty($yet_to_review) && ($reviewer || $manager || $administer)) {
          $module_path = drupal_get_path('module', 'mfcs');
          drupal_add_js($module_path . '/js/request_view-reviewers.js', array('type' => 'file', 'group' => JS_DEFAULT, 'preprocess' => TRUE));

          $yet_to_review_markup .= '<div id="request-' . $request_id . '-section-review-log-yet_to_review" class="request-section-review-log-yet_to_review">';

          if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
            if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Reviewer classifications that have not yet reviewed:</div>';
            }
            elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Requirements that have not yet reviewed:</div>';
            }

            $yet_to_review_markup .= '<ul class="yet_to_review-list">';
            foreach ($yet_to_review as $classification_id => $classification_name) {
              $yet_to_review_markup .= '<li class="yet_to_review-list_item">';
              $yet_to_review_markup .= $classification_name;

              // create a javascript based users list to easily display the users associated with the given classification.
              $yet_to_review_markup .= '<div class="details_list inline" title="View list of users associated with this reviewer classification.">';
              $yet_to_review_markup .= ' <span class="no-script-hidden no-print details_list-label label-' . $classification_id . '">(<span class="details_list-link link-' . $classification_id . ' inline fake_link">Details</span>)</span>';
              $yet_to_review_markup .= '<ul class="script-hidden details_list-item item-' . $classification_id . '">';
              if (!empty($decisions[$request_id]['reviewers'][$classification_id])) {
                foreach ($decisions[$request_id]['reviewers'][$classification_id] as $user_id) {
                  $basic_user_settings = mfcs_load_basic_user_settings_by_user_id($user_id);
                  if ($basic_user_settings === FALSE) {
                    continue;
                  }

                  $yet_to_review_markup .= '<li class="details_list-user">';
                  if (empty($basic_user_settings['first_name']) || empty($basic_user_settings['last_name'])) {
                    $yet_to_review_markup .= '<span class="details_list-user_name">' . $basic_user_settings['name'] . '</span>';
                  }
                  else {
                    $yet_to_review_markup .= '<span class="details_list-user_name">';
                    $yet_to_review_markup .= $basic_user_settings['first_name'] . ' ' . $basic_user_settings['last_name'];
                    $yet_to_review_markup .= '</span>';
                  }
                  $yet_to_review_markup .= ' <span class="details_list-user_id">[id: ' . $user_id . ']</span>';
                  $yet_to_review_markup .= '</li>';
                }
              }

              $yet_to_review_markup .= '</ul>';
              $yet_to_review_markup .= '</div>';
              $yet_to_review_markup .= '</li>';
            }

            $yet_to_review_markup .= '</ul>';
          }
          elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
            if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Waiting for the Venue Coordinator or a Venue Coordinator Proxy:</div>';
            }

            $yet_to_review_markup .= '<ul class="yet_to_review-list">';
            $yet_to_review_markup .= '<li class="yet_to_review-list_item">';
            $yet_to_review_markup .= "Venue Coordinator";

            $basic_user_settings = mfcs_load_basic_user_settings_by_user_id($request['venue_coordinator']['user_id'][0]->value);
            if ($basic_user_settings !== FALSE) {
              // create a javascript based users list to easily display the users associated with the given classification.
              $yet_to_review_markup .= '<div class="details_list inline" title="View list of users associated with this reviewer classification.">';
              $yet_to_review_markup .= '<span class="no-script-hidden no-print details_list-label label-venue_coordinator">(<span class="details_list-link link-venue_coordinator inline fake_link">Details</span>)</span>';
              $yet_to_review_markup .= '<ul class="script-hidden details_list-item item-venue_coordinator">';

              $yet_to_review_markup .= '<li class="details_list-user">';
              if (empty($basic_user_settings['first_name']) || empty($basic_user_settings['last_name'])) {
                $yet_to_review_markup .= '<span class="details_list-user_name">' . $basic_user_settings['name'] . '</span>';
              }
              else {
                $yet_to_review_markup .= '<span class="details_list-user_name">';
                $yet_to_review_markup .= $basic_user_settings['first_name'] . ' ' . $basic_user_settings['last_name'];
                $yet_to_review_markup .= '</span>';
              }
              $yet_to_review_markup .= ' <span class="details_list-user_id">[id: ' . $request['venue_coordinator']['user_id'][0]->value . ']</span>';
              $yet_to_review_markup .= '</li>';
              $yet_to_review_markup .= '</ul>';
              $yet_to_review_markup .= '</div>';
            }

            $yet_to_review_markup .= '</li>';

            // do not display the venue coordinator in the proxy list.
            $request_proxies = array();
            if (isset($request['venue_coordinator']['user_id'][0]->value)) {
              $request_proxies = mfcs_get_venue_coordinator_proxy($request['venue_coordinator']['user_id'][0]->value);
            }

            if (isset($request_proxies[$request['venue_coordinator']['user_id'][0]->value])) {
              unset($request_proxies[$request['venue_coordinator']['user_id'][0]->value]);
            }

            if (!empty($request_proxies)) {
              $yet_to_review_markup .= '<li class="yet_to_review-list_item">';
              $yet_to_review_markup .= "Venue Coordinator Proxy";

              // create a javascript based users list to easily display the users associated with the given classification.
              $yet_to_review_markup .= '<div class="details_list inline" title="View list of users allowed to proxy as the venue coordinator.">';
              $yet_to_review_markup .= '<span class="no-script-hidden no-print details_list-label label-venue_coordinator_proxy">(<span class="details_list-link link-venue_coordinator_proxy inline fake_link">Details</span>)</span>';
              $yet_to_review_markup .= '<ul class="script-hidden details_list-item item-venue_coordinator_proxy">';

              foreach ($request_proxies as $proxy_id) {
                $basic_user_settings = mfcs_load_basic_user_settings_by_user_id($proxy_id);
                if ($basic_user_settings === FALSE) {
                  continue;
                }

                $yet_to_review_markup .= '<li class="details_list-user">';
                if (empty($basic_user_settings['first_name']) || empty($basic_user_settings['last_name'])) {
                  $yet_to_review_markup .= '<span class="details_list-user_name">' . $basic_user_settings['name'] . '</span>';
                }
                else {
                  $yet_to_review_markup .= '<span class="details_list-user_name">';
                  $yet_to_review_markup .= $basic_user_settings['first_name'] . ' ' . $basic_user_settings['last_name'];
                  $yet_to_review_markup .= '</span>';
                }

                $yet_to_review_markup .= ' <span class="details_list-user_id">[id: ' . $proxy_id . ']</span>';
                $yet_to_review_markup .= '</li>';
              }

              $yet_to_review_markup .= '</ul>';
              $yet_to_review_markup .= '</div>';

              $yet_to_review_markup .= '</li>';
            }
            unset($request_proxies);

            $yet_to_review_markup .= '</ul>';
          }

          $yet_to_review_markup .= '</div>';
        }
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

      mfcs_view_build_reviews_log($form, $form_state, $request_id, $use_revision);

      if (isset($form['review'])) {
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
          'title' => "Send this request to your printer.",
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
           'title' => "View or Download this request as a PDF.",
          ),
          '#submit' => array(
            'mfcs_request_view_0_form_submit_pdf',
          ),
        );
      }
    }

    if ($show_operations && !$contained && is_null($use_revision)) {
      $decision_options = mfcs_request_view_0_detect_allowed_decisions($request, $user, $administer, $manager, $venue_coordinator, $reviewer);
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
        '#markup' => '<h2 id="request-' . $request_id . '-section-review-operations-header" class="request-section-review-operations-header">Review Operations</h2>',
        '#weight' => 0,
      );

      $form['review']['operations']['instructions'] = array(
        '#markup' => '',
        '#weight' => 0,
      );

      $form['review']['operations']['instructions']['#markup'] .= "<div class=\"instructions-current_step\">The current step is: <strong>" . $step_options[$step] . "</strong>.</div>";

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
      $reviewer_classifications = array();
      $reviewer_classifications_all = array();
      foreach ($reviewers as $reviewers_reviewer) {
        // ensure that all classifications and steps are cast to integers.
        $reviewer_classification = NULL;
        if (property_exists($reviewers_reviewer, 'reviewer_classification')) {
          if (is_string($reviewers_reviewer->reviewer_classification)) {
            $reviewer_classification = (int) $reviewers_reviewer->reviewer_classification;
          }
          else {
            $reviewer_classification = $reviewers_reviewer->reviewer_classification;
          }
        }

        $reviewers_review_step = NULL;
        if (property_exists($reviewers_reviewer, 'review_step')) {
          if (is_string($reviewers_reviewer->review_step)) {
            $reviewers_review_step = (int) $reviewers_reviewer->review_step;
          }
          else {
            $reviewers_review_step = $reviewers_reviewer->review_step;
          }
        }

        if (is_null($reviewer_classification)) {
          continue;
        }

        if ($reviewers_review_step === $step) {
          $reviewer_classifications[$reviewer_classification] = $reviewer_classification_options[$reviewer_classification];
        }

        $reviewer_classifications_all[$reviewer_classification] = $reviewer_classification_options[$reviewer_classification];
      }
      unset($reviewers_reviewer);

      // remove all classifications that may have been reviewed already.
      if (!empty($reviewer_classifications)) {
        $form['form']['already_reviewed_as']['#value'] = mfcs_get_request_already_reviewed_as_classifications($request_id, array_keys($reviewer_classifications));
        if (isset($form['form']['already_reviewed_as']['#value']['reviewed'])) {
          foreach ($form['form']['already_reviewed_as']['#value']['reviewed'] as $reviewer_classification) {
            unset($reviewer_classifications[$reviewer_classification]);
          }
          unset($reviewer_classification);
        }
        else {
          $form['form']['already_reviewed_as']['#value'] = array();
        }
      }

      // report administers and managers if not defined.
      if (empty($reviewer_classifications) && empty($venue_coordinator) && ($administer || $manager)) {
        if ($administer) {
          $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR] = 'Administrator';
        }
        elseif ($manager) {
          $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = 'Manager';
        }
      }

      // the venue coordinator must be reported as venue coordinator when not reviewing.
      if ($venue_coordinator && (empty($reviewer_classifications) || $step === MFCS_REVIEW_STEP_VENUE_AVAILABLE)) {

        if ($request['venue_coordinator']['user_id'][0]->value == $user->uid) {
          $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
          $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
        }
        else {
          $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = "Venue Coordinator (Proxy)";
          $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = "Venue Coordinator (Proxy)";
        }
      }

      if (!empty($reviewer_classifications)) {
        $form['review']['operations']['as_reviewer']['#markup'] .= '<div class="request-section-review-operations-as_reviewer-message">You are reviewing with the following classifications:</div>';
        $form['review']['operations']['as_reviewer']['#markup'] .= theme('item_list', array('items' => $reviewer_classifications, 'type' => 'ul', 'attributes' => array('class' => array('request-section-review-operations-as_reviewer-list'))));
      }

      $form['review']['operations']['as_reviewer']['#markup'] .= '</div>';



      // restrict options to commenting only, if the user is only allowed to comment.
      if (!$can_review_request && $can_comment_request) {
        $decision_options = array();
        if ($can_comment_request) {
          $decision_options[MFCS_REVIEW_DECISION_COMMENT] = $decision_options_all[MFCS_REVIEW_DECISION_COMMENT];
        }
      }


      // provide warnings.
      $form['review']['operations']['warnings'] = array(
        '#type' => 'markup',
        '#markup' => "",
      );

      // Display warning messages if the user can make a decision.
      if ($can_review_request && !empty($reviewer_classifications) && ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step === MFCS_REVIEW_STEP_REVIEW || $step === MFCS_REVIEW_STEP_REQUIREMENTS)) {
        $holidays = array();
        foreach ($request['dates']['date'] as $date_delta => $date_value) {
          $holidays = mfcs_get_holidays((int) date('Y', $request['dates']['date'][$date_delta]->value), $request['dates']['date'][$date_delta]->value);
        }

        if (!empty($holidays)) {
          $form['review']['operations']['warnings']['#markup'] .= '<div class="warning-holidays">';
          $form['review']['operations']['warnings']['#markup'] .= '<div class="warning-holidays-title">This request is scheduled on the same dates as the following:</div>';
          $form['review']['operations']['warnings']['#markup'] .= '<ul class="warning-holidays-list">';
          foreach ($holidays as $holiday) {
            $form['review']['operations']['warnings']['#markup'] .= '<li class="warning-holidays-list-item">' . t("<strong>@holiday</strong> on: <em>@date</em>.", array('@date' => date('l, F j, Y', $holiday->date), '@holiday' => $holiday->title)) . '</li>';
          }
          $form['review']['operations']['warnings']['#markup'] .= '</ul>';
          $form['review']['operations']['warnings']['#markup'] .= '</div>';
        }
      }

      if (array_key_exists(MFCS_REVIEW_DECISION_ISSUES, $decision_options) && array_key_exists(MFCS_REVIEW_DECISION_ISSUES_NONE, $decision_options)) {
        if (!$can_review_request || !empty($reviewer_classifications)) {
          $form['review']['operations']['instructions']['#markup'] .= "<div class=\"instructions-issues\">To mark this request as <strong>reviewed</strong>, be sure to select either <strong>" . $decision_options[MFCS_REVIEW_DECISION_ISSUES_NONE] . "</strong> or <strong>" . $decision_options[MFCS_REVIEW_DECISION_ISSUES] . "</strong>.</div>";
        }
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

      if (count($decision_options) == 1 && array_key_exists(MFCS_REVIEW_DECISION_COMMENT, $decision_options)) {
        $form['review']['operations']['decision']['#title'] = "Action / Decision";
        $form['review']['operations']['decision']['#default_value'] = MFCS_REVIEW_DECISION_COMMENT;
      }

      if (array_key_exists(MFCS_REVIEW_DECISION_REQUIREMENT, $decision_options)) {
        $states = array(
          'visible' => array(
            ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
          ),
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
          '#weight' => 3,
        );

        if (!empty($request['requirements']['university'][0])) {
          $form['review']['operations']['university_business']['#default_value'] = $request['requirements']['university'][0]->value;
        }

        $form['review']['operations']['requirements'] = array(
          '#tree' => TRUE,
          '#weight' => 4,
        );

        // adding a container works arounds drupals lack of support for '#states' with '#markup' fields.
        $form['review']['operations']['requirements']['description'] = array(
          '#type' => 'container',
          '#states' => $states,
          '#attributes' => array(
            'class' => array(
              'requirements-description-wrapper',
            ),
          ),
        );

        $form['review']['operations']['requirements']['description']['markup'] = array(
          '#markup' => '<div class="requirements-description"><strong class="requirements-description-statement-1">Use Fees totals are auto-calculated by the system.</strong> Do not provide the total value in the amount field. Instead, provide the amount using the appropriate per-hour/per-day/per-personnel cost.</div>',
        );

        $uses = array();
        $uses['facilities_fees'] = "Facilities";
        $uses['equipment_fees'] = "Additional Equipment";
        $uses['custodial_fees'] = "Custodial";
        $uses['maintenance_fees'] = "Maintenance";
        $uses['grounds_fees'] = "Grounds";
        $uses['security_fees'] = "Security";

        $insurance_requirement = TRUE;

        $requirements_amount_defaults = array('custodial_fees' => array('rate' => '18.50'), 'maintenance_fees' => array('rate' => '18.50'), 'grounds_fees' => array('rate' => '18.50'), 'security_fees' => array('rate' => '30.00'));

        foreach ($uses as $use_name => $use_title) {
          $form['review']['operations']['requirements'][$use_name] = array(
            '#type' => 'fieldset',
            '#title' => $use_title . ' Fee',
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
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


          // rates group
          $form['review']['operations']['requirements'][$use_name]['group_rates'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-group_rates',
            '#type' => 'container',
            '#attributes' => array(
              'class' => array(
                'group',
                'group_rates',
                'requirements-' . $use_name . '-group_rates',
              ),
            ),
          );

          $form['review']['operations']['requirements'][$use_name]['group_rates']['personnel'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-personnel',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Personnel',
            '#default_value' => NULL,
            '#size' => 16,
            '#min' => 0,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-personnel',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'personnel'),
          );

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['group_rates']['personnel']['#default_value'] = (int) $request['requirements'][$use_name][0]->personnel;
          }

          $form['review']['operations']['requirements'][$use_name]['group_rates']['hours'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-hours',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Hours',
            '#default_value' => NULL,
            '#min' => 0,
            '#size' => 16,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-hours',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'hours'),
          );

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['group_rates']['hours']['#default_value'] = (int) $request['requirements'][$use_name][0]->hours;
          }

          $form['review']['operations']['requirements'][$use_name]['group_rates']['days'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-days',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Days',
            '#default_value' => NULL,
            '#min' => 0,
            '#size' => 16,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-days',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'days'),
          );

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['group_rates']['days']['#default_value'] = (int) $request['requirements'][$use_name][0]->days;
          }

          $form['review']['operations']['requirements'][$use_name]['group_rates']['rate'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-rate',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Rate',
            '#default_value' => NULL,
            '#size' => 16,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-rate',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'rate'),
          );

          if (!empty($request['requirements'][$use_name][0]) && strlen($request['requirements'][$use_name][0]->rate) > 0) {
            $form['review']['operations']['requirements'][$use_name]['group_rates']['rate']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->rate, 'currency');
          }
          elseif (isset($requirements_amount_defaults[$use_name]['rate'])) {
            $form['review']['operations']['requirements'][$use_name]['group_rates']['rate']['#default_value'] = $requirements_amount_defaults[$use_name]['rate'];
          }


          // supplies group
          $form['review']['operations']['requirements'][$use_name]['group_supplies'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-group_supplies',
            '#type' => 'container',
            '#attributes' => array(
              'class' => array(
                'group',
                'group_supplies',
                'requirements-' . $use_name . '-group_supplies',
              ),
            ),
          );

          $form['review']['operations']['requirements'][$use_name]['group_supplies']['supplies'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-supplies',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Supplies',
            '#default_value' => NULL,
            '#size' => 16,
            '#min' => 0,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-supplies',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'supplies'),
          );

          if (!empty($request['requirements'][$use_name][0]) && strlen($request['requirements'][$use_name][0]->supplies) > 0) {
            $form['review']['operations']['requirements'][$use_name]['group_supplies']['supplies']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->supplies, 'currency');
          }


          // post group
          $form['review']['operations']['requirements'][$use_name]['group_post'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-group_post',
            '#type' => 'container',
            '#attributes' => array(
              'class' => array(
                'group',
                'group_post',
                'requirements-' . $use_name . '-group_post',
              ),
            ),
          );

          $form['review']['operations']['requirements'][$use_name]['group_post']['post'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-post',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Post',
            '#default_value' => NULL,
            '#size' => 16,
            '#min' => 0,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-post',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'post'),
          );

          if (!empty($request['requirements'][$use_name][0]) && strlen($request['requirements'][$use_name][0]->post) > 0) {
            $form['review']['operations']['requirements'][$use_name]['group_post']['post']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->post, 'currency');
          }

          $form['review']['operations']['requirements'][$use_name]['group_post']['description'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-description',
            '#type' => 'textarea',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Post Description',
            '#default_value' => NULL,
            '#rows' => 4,
            '#required' => FALSE,
            '#attributes' => array(
              'class' => array(
                'no-print',
                'mode-' . $mode,
                'requirement-description',
              ),
            ),
            '#states' => $states,
            '#parents' => array('review', 'operations', 'requirements', $use_name, 'description'),
          );

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['group_post']['description']['#default_value'] = $request['requirements'][$use_name][0]->description;
          }
        }

        $form['review']['operations']['insurance'] = array(
          '#tree' => TRUE,
          '#weight' => 5,
        );

        if ($can_review_request && ($step === MFCS_REVIEW_STEP_REQUIREMENTS || $step === MFCS_REVIEW_STEP_COMPLETED)) {
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
            '#states' => $states,
          );

          if (!empty($request['insurance']['provided'][0])) {
            $form['review']['operations']['insurance']['provided']['#default_value'] = $request['insurance']['provided'][0]->value;
          }
        }
      }

      if ($reviewer || $manager || $administer) {
        $form['review']['operations']['message_is_reviewers_only'] = array(
          '#id' => 'request-review-operations-message_is_reviewers_only',
          '#type' => 'checkbox',
          '#title' => "Restrict message to reviewers only.",
          '#default_value' => FALSE,
          '#required' => FALSE,
          '#attributes' => array(
            'title' => "When enabled, messages will be restricted to reviewers, managers, and administrators.",
          ),
          '#label_attributes' => array(
            'title' => "When enabled, messages will be restricted to reviewers, managers, and administrators.",
          ),
          '#weight' => 6,
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
        '#weight' => 7,
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

  if (is_string($request_classification)) {
    $request_classification = (int) $request_classification;
  }

  if (is_string($step)) {
    $step = (int) $step;
  }

  if (is_string($status)) {
    $status = (int) $status;
  }

  $decision = NULL;
  if (isset($form_state['values']['review']['operations']['decision'])) {
    $decision = $form_state['values']['review']['operations']['decision'];

    if (is_string($decision)) {
      $decision = (int) $decision;
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
  global $base_path;

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

  if (is_string($request_classification)) {
    $request_classification = (int) $request_classification;
  }

  if (is_string($step)) {
    $step = (int) $step;
  }

  if (is_string($status)) {
    $status = (int) $status;
  }

  if (is_string($type)) {
    $type = (int) $type;
  }

  $latest_revision = (int) mfcs_load_request_revision_number($request_id);
  if (isset($form['review']['revision']['#value']) && $form['review']['revision']['#value'] != $latest_revision) {
    // check to see if the old revision and the new revision are on different states.
    $request_previous = mfcs_load_request_by_id($request_id, NULL, $form['review']['revision']['#value']);
    $request_latest = mfcs_load_request_by_id($request_id, NULL, $latest_revision);

    $step_previous = $request_previous['top']['step'][0]->value;
    $step_latest = $request_latest['top']['step'][0]->value;

    if (is_string($step_previous)) {
      $step_previous = (int) $step_previous;
    }
    if (is_string($step_latest)) {
      $step_latest = (int) $step_latest;
    }

    if ($step_latest !== $step_previous) {
      drupal_set_message("Warning: the request has changed since you last loaded this page. This page is out of date and has been reloaded. No changes have been made.", 'warning');
      watchdog(MFCS_WATCHDOG_ID, "The user '%user_name' (%user_id) attempted to review a request using a stale page", array('%user_name' => $user->name, '%user_id' => $user->uid), WATCHDOG_WARNING);
      return;
    }
  }
  unset($previous_revision_id);
  unset($current_revision_id);

  $decision = NULL;
  if (isset($form_state['values']['review']['operations']['decision'])) {
    $decision = $form_state['values']['review']['operations']['decision'];

    if (is_string($decision)) {
      $decision = (int) $decision;
    }
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

    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
      form_set_error('', "Revisions can not be cancelled.");
    }

    $form_state['cancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'request-display-submit-uncancel') {
    if ($mode != MFCS_VIEW_MODE_NORMAL && $mode != MFCS_VIEW_MODE_DISPLAY) {
      form_set_error('', "This view mode does not allow uncancelling.");
    }

    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
      form_set_error('', "Revisions can not be uncancelled.");
    }

    $form_state['uncancel_was_clicked'] = TRUE;
  }
  elseif ($clicked_id == 'request-review-operations-submit-submit') {
    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
      form_set_error('', "Revisions can not be changed or reviewed.");
    }

    $failure = FALSE;
    $check_requirements = FALSE;

    // only allow operations for states and steps that make sense to.
    $allowed_statuses = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
    );

    $allowed_steps = array(
      MFCS_REVIEW_STEP_VENUE_AVAILABLE,
      MFCS_REVIEW_STEP_REVIEW,
      MFCS_REVIEW_STEP_REQUIREMENTS,
    );

    if ($decision === MFCS_REVIEW_DECISION_REQUIREMENT) {
      $allowed_statuses = array(
        MFCS_REQUEST_STATUS_LOCKED,
        MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
        MFCS_REQUEST_STATUS_CLOSED_DENIED,
        MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
        MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
        MFCS_REQUEST_STATUS_CANCELLED,
        MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
      );

      $allowed_steps = array(
        MFCS_REVIEW_STEP_REVIEW,
        MFCS_REVIEW_STEP_REQUIREMENTS,
        MFCS_REVIEW_STEP_COMPLETED,
      );
    }

    if ($manager) {
    }

    if ($decision === MFCS_REVIEW_DECISION_ISSUES || $decision === MFCS_REVIEW_DECISION_ISSUES_NONE) {
      $allowed_statuses = array(
        MFCS_REQUEST_STATUS_LOCKED,
        MFCS_REQUEST_STATUS_UNLOCKED,
        MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
        MFCS_REQUEST_STATUS_CLOSED_DENIED,
        MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
        MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
        MFCS_REQUEST_STATUS_CANCELLED,
        MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
      );

      $allowed_steps = array(
        MFCS_REVIEW_STEP_REVIEW,
        MFCS_REVIEW_STEP_REQUIREMENTS,
        MFCS_REVIEW_STEP_COMPLETED,
      );
    }

    if ($can_comment_request && $decision === MFCS_REVIEW_DECISION_COMMENT) {
      // if there are any cases where a comment cannot be posted, then this is the place to add the restriction.
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
      if ($decision === MFCS_REVIEW_DECISION_COMMENT) {
        unset($form_state['values']['review']['operations']['requirements']);
        unset($form_state['values']['review']['operations']['insurance']);

        if (!$can_comment_request) {
          form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
          $failure = TRUE;
        }
      }
      else {
        form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
        $failure = TRUE;
      }
    }

    // Check requirements during the requirements step unless a comment is made.
    if ($step === MFCS_REVIEW_STEP_REQUIREMENTS && $decision !== MFCS_REVIEW_DECISION_COMMENT) {
      $check_requirements = TRUE;
    }

    // ticket: fcs-77.
    // make sure the request cannot be acted on when there already is a locked request or have final approval.
    if (($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision === MFCS_REVIEW_DECISION_AVAILABLE) || ($step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision === MFCS_REVIEW_DECISION_APPROVE)) {
      $room = (int) $request['top']['room'][0]->value;

      $all_dates = array();
      $all_time_start = array();
      $all_time_stop = array();
      $all_holidays = array();
      foreach ($request['dates']['date'] as $date_delta => $date_value) {
        $all_dates[$date_delta] = $request['dates']['date'][$date_delta]->value;
        $all_time_start[$date_delta] = $request['dates']['time_start'][$date_delta]->value;
        $all_time_stop[$date_delta] = $request['dates']['time_stop'][$date_delta]->value;

        $all_holidays = mfcs_get_holidays((int) date('Y', $request['dates']['date'][$date_delta]->value), $request['dates']['date'][$date_delta]->value);
      }

      $room_unavailability = mfcs_check_room_availability($room, $all_dates, $all_time_start, $all_time_stop, $request_id);
      if ($room_unavailability === FALSE) {
        form_set_error('', "An internal error occurred while checking the room for availability. Please contact the support staff.");
      }
      elseif (!empty($room_unavailability['conflict']) || !empty($room_unavailability['unavailable'])) {
        $unavailable_message = '<span class="uppercase">Conflict</span>: The specified room is currently unavailable for the following dates: ';
        $unavailable_message .= mfcs_build_room_unavailable_message($room_unavailability);
        form_set_error('review][operations][decision', $unavailable_message);
        unset($unavailable_message);
      }
      unset($all_dates);
      unset($all_time_start);
      unset($all_time_stop);
      unset($room_unavailability);

      // check for holidays.
      if (!empty($all_holidays)) {
        // there currently can only be a single holiday per day, so only process a single holiday.
        $holiday = reset($all_holidays);
        if (is_object($holiday)) {
          $unavailable_message .= '<strong>';
          $unavailable_message .= '@title</a></strong>.';

          drupal_set_message(t('Warning: <strong>@date</strong> may not available because of: <strong><a href="' . $base_path . 'requests/holiday-0/view/@year/' . $holiday->year. '/' . $holiday->id . $url_arguments . '" title="View details about holiday.">@holiday</a></strong>.', array('@year' => $holiday->year, '@date' => date('l, F j, Y', $holiday->date), '@holiday' => $holiday->title)), 'warning');
        }
        unset($holiday);
      }
      unset($all_holidays);
    }

    // The requirements values should only be used during the requirements decision.
    if ($decision !== MFCS_REVIEW_DECISION_REQUIREMENT) {
      unset($form_state['values']['review']['operations']['requirements']);
    }

    if ($step === MFCS_REVIEW_STEP_REQUIREMENTS) {
      $university_business = FALSE;

      if ($decision === MFCS_REVIEW_DECISION_APPROVE && $type !== MFCS_REQUEST_TYPE_QUICK_MEETING) {
        $requirements_not_supplied = TRUE;

        if ($step === MFCS_REVIEW_STEP_REQUIREMENTS) {
          if (isset($request['requirements']['university'][0]) && property_exists($request['requirements']['university'][0], 'value')) {
            if ($request['requirements']['university'][0]->value) {
              $requirements_not_supplied = FALSE;

              $university_business = TRUE;
            }
          }
        }

        if ($university_business || (isset($request['requirements']['facilities'][0]) && property_exists($request['requirements']['facilities'][0], 'days') && !empty($request['requirements']['facilities'][0]->days))) {
          $requirements_not_supplied = FALSE;
        }

        if ($requirements_not_supplied) {
          form_set_error('review][operations][decision', t("The request cannot be approved until the requirements are either specified or waived."));
          $failure = TRUE;
        }
      }
    }

    // @fixme: is autowaive valid anymore?
    $form_state['autowaive'] = FALSE;

    if ($check_requirements) {
      $requirements = array();
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        $requirements = $form_state['values']['review']['operations']['requirements'];
      }

      $requirements_names = array(
        'facilities_fees' => "Facilities Fee",
        'equipment_fees' => "Equipment Fee",
        'custodial_fees' => "Custodial Fee",
        'maintenance_fees' => "Maintenance Fee",
        'grounds_fees' => "Grounds Fee",
        'security_fees' => "Security Fee",
      );

      $requirements_real = array(
        'rate' => array('group' => 'group_rates', 'error_message' => "The :human_name '@number' is not a valid @number_name. It must be a number greater than or equal to 0 (with at most 2 decimal places). Do not provide a dollar sign."),
        'hours' => array('group' => 'group_rates', 'error_message' => "The :human_name '@number' is not a valid @number_name. It must be a number greater than or equal to 0 (with at most 2 decimal places)."),
        'supplies' => array('group' => 'group_supplies', 'error_message' => "The :human_name '@number' is not a valid @number_name. It must be a number greater than or equal to 0 (with at most 2 decimal places). Do not provide a dollar sign."),
        'post' => array('group' => 'group_post', 'error_message' => "The :human_name '@number' is not a valid @number_name. It must be a number greater than or equal to 0 (with at most 2 decimal places). Do not provide a dollar sign."),
      );

      $requirements_integer = array(
        'days' => array('group' => 'group_rates', 'error_message' => "The :human_name '@number' is not a valid @number_name. It is not an allowed number. Use only numbers without decimal places that are greater than or equal to 0."),
        'personnel' => array('group' => 'group_rates', 'error_message' => "The :human_name '@number' is not a valid @number_name. It is not an allowed number. Use only numbers without decimal places that are greater than or equal to 0."),
      );

      foreach ($requirements_names as $requirements_machine_name => $requirements_human_name) {
        if (!array_key_exists($requirements_machine_name, $requirements)) {
          continue;
        }

        foreach ($requirements_real as $requirements_field_name => $requirements_field_value) {
          if (!array_key_exists($requirements_field_name, $requirements[$requirements_machine_name])) {
            continue;
          }

          $r = $requirements[$requirements_machine_name][$requirements_field_name];

          if (is_numeric($r)) {
            if (!preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $r)) {
              form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_field_name, t($requirements_field_value['error_message'], array(':human_name' => $requirements_human_name, '@number' => $r, '@number_name' => $requirements_field_name)));
              $failure = TRUE;
            }
            else {
              $number = floatval($r);
              if ($number < 0) {
                form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_field_name, t($requirements_field_value['error_message'], array(':human_name' => $requirements_human_name, '@number' => $number, '@number_name' => $requirements_field_name)));
                $failure = TRUE;
              }
              else {
                // save the value as a float instead of an integer or string.
                $r = $number;
              }

              unset($number);
            }
          }
          elseif (is_string($r) && strlen($r) > 0) {
            form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_field_name, t($requirements_field_value['error_message'], array(':human_name' => $requirements_human_name, '@number' => $r, '@number_name' => $requirements_field_name)));
            $failure = TRUE;
          }

          unset($r);
        }

        foreach ($requirements_integer as $requirements_field_name => $requirements_field_value) {
          if (!isset($requirements[$requirements_machine_name][$requirements_field_name])) {
            continue;
          }

          $r = $requirements[$requirements_machine_name][$requirements_field_name];

          if (cf_is_integer($r)) {
            if (!preg_match('/^\d+$/i', $r)) {
              form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_field_name, t($requirements_field_value['error_message'], array(':human_name' => $requirements_human_name, '@number' => $r, '@number_name' => $requirements_field_name)));
              $failure = TRUE;
            }
            else {
              $number = intval($r);

              if ($number < 0) {
                form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_field_name, t($requirements_field_value['error_message'], array(':human_name' => $requirements_human_name, '@number' => $number, '@number_name' => $requirements_field_name)));
                $failure = TRUE;
              }

              unset($number);
            }
          }
          elseif (is_string($r) && strlen($r) > 0) {
            form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_field_name, t($requirements_field_value['error_message'], array(':human_name' => $requirements_human_name, '@number' => $r, '@number_name' => $requirements_field_name)));
            $failure = TRUE;
          }

          unset($r);
        }
      }

      unset($requirements);
    }

    if (!$failure) {
      $form_state['rebuild'] = FALSE;
    }
  }
  elseif ($clicked_id == 'request-cancel-submit-yes' || $clicked_id == 'request-cancel-submit-no') {
    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
      form_set_error('', "Revisions can not be cancelled.");
      return;
    }

    $form_state['rebuild'] = FALSE;
  }
  elseif ($clicked_id == 'request-uncancel-submit-yes' || $clicked_id == 'request-uncancel-submit-no') {
    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
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
    $restriction = MFCS_REVIEW_RESTRICTIONS_NONE;

    if (isset($form_state['values']['review']['operations']['message_is_reviewers_only']) && $form_state['values']['review']['operations']['message_is_reviewers_only']) {
      $restriction = MFCS_REVIEW_RESTRICTIONS_REVIEWER;
    }

    $request = $form['form']['request']['#value'];

    $request_classification = $request['request_coordinator']['classification'][0]->value;
    $step = $request['top']['step'][0]->value;
    $type = $request['information']['type'][0]->value;

    if (is_string($decision)) {
      $decision = (int) $decision;
    }

    if (is_string($request_classification)) {
      $request_classification = (int) $request_classification;
    }

    if (is_string($step)) {
      $step = (int) $step;
    }

    if (is_string($type)) {
      $type = (int) $type;
    }

    $already_reviewed_as = NULL;
    if (!empty($form['form']['already_reviewed_as']['#value']['available']) && is_array($form['form']['already_reviewed_as']['#value']['available'])) {
      $already_reviewed_as = $form['form']['already_reviewed_as']['#value']['available'];
    }

    $insurance_provided_available = FALSE;
    if (isset($form['form']['insurance_provided_available']['#value'])) {
      $insurance_provided_available = (bool) $form['form']['insurance_provided_available']['#value'];
    }

    $extra = array();

    if ($decision === MFCS_REVIEW_DECISION_COMMENT) {
      // comments do not need to be processed on a per step basis, so skip the $step conditions below.
    }
    elseif ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      $is_new_request = NULL;
      if (isset($form['form']['new']['#value'])) {
        $is_new_request = $form['form']['new']['#value'];
      }

      // when the request type is a quick meeting, then mark this as waived due to university business.
      if ($type === MFCS_REQUEST_TYPE_QUICK_MEETING && $is_new_request) {
        $form_state['values']['review']['operations']['university_business'] = 1;
        unset($extra['requirements']);
        $requirements_extra = array(
          'send_email' => FALSE,
          'requirements' => array('other' => 0, 'waived' => 0, 'university' => 1),
        );

        $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], MFCS_REVIEW_DECISION_REQUIREMENT, MFCS_REVIEW_RESTRICTIONS_NONE, "The system has auto-waived the fees because the request is a Quick Meeting.", $requirements_extra, $already_reviewed_as);
      }
    }
    else {
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        $extra['requirements'] = $form_state['values']['review']['operations']['requirements'];
      }

      if (isset($form_state['values']['review']['operations']['insurance'])) {
        $extra['insurance'] = $form_state['values']['review']['operations']['insurance'];
      }

      if (isset($form_state['values']['review']['operations']['university_business'])) {
        $extra['requirements']['university'] = $form_state['values']['review']['operations']['university_business'];
      }

      if ($insurance_provided_available) {
        if (isset($form_state['values']['review']['operations']['insurance']['provided'])) {
          if (!isset($extra['insurance'])) {
            $extra['insurance'] = array();
          }

          $extra['insurance']['provided'] = $form_state['values']['review']['operations']['insurance']['provided'];
        }
      }
    }

    if ($success && $decision === MFCS_REVIEW_DECISION_REQUIREMENT) {
      if (!mfcs_request_view_0_detect_requirements_changes($form, $form_state, $extra)) {
        form_set_error('review][operations][decision', "The Requirements decision has been selected, but no changes have been made. If you only wish to leave a comment, then select the Comment instead of Requirements.");
        watchdog(MFCS_WATCHDOG_ID, "The Requirements decision has been selected, but no changes have been made.", array(), WATCHDOG_ERROR);

        $form_state['rebuild'] = TRUE;
        $form_state['redirect'] = FALSE;
        $form_state['submitted'] = FALSE;

        return;
      }
    }

    $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], $decision, $restriction, $message, $extra, $already_reviewed_as);

    if ($success) {
      if ($decision === MFCS_REVIEW_DECISION_COMMENT) {
        drupal_set_message("Your comment has been received.");
      }
      else {
        drupal_set_message("Your review has been received.");

        if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && ($decision === MFCS_REVIEW_DECISION_AVAILABLE || $decision === MFCS_REVIEW_DECISION_UNAVAILABLE)) {
          // perform check against this request to see if new problems are detected or existing problems are resolved.
          mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
          mfcs_mischellaneous_check_requests_for_problems($request_id);
        }
        elseif ($step == MFCS_REVIEW_STEP_REQUIREMENTS && ($decision === MFCS_REVIEW_DECISION_APPROVE || $decision === MFCS_REVIEW_DECISION_DENY)) {
          // perform check against this request to see if new problems are detected or existing problems are resolved.
          mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
          mfcs_mischellaneous_check_requests_for_problems($request_id);
        }
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

    // change to the request view page on submit for these decisions to protect against potential access denied messages following the submit process.
    if ($decision === MFCS_REVIEW_DECISION_APPROVE || $decision === MFCS_REVIEW_DECISION_DENY) {
      $form_state['values']['redirect_to'] = 'requests/view-0/' .  $request_id;
    }
    elseif ($decision === MFCS_REVIEW_DECISION_AVAILABLE || $decision === MFCS_REVIEW_DECISION_UNAVAILABLE) {
      $form_state['values']['redirect_to'] = 'requests/view-0/' .  $request_id;
    }
    //elseif ($decision === MFCS_REVIEW_DECISION_ISSUES || $decision === MFCS_REVIEW_DECISION_ISSUES_NONE) {
    //  $form_state['values']['redirect_to'] = 'requests/view-0/' .  $request_id;
    //}
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
 * Scan through the form and form state and determine if any changes in the requirements fields were peformed.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param array $extra
 *   The array containing extra fields to be updated, namely the 'requirements' columns.
 *
 * @return bool
 *   TRUE is returned if changes are detected, otherwise FALSE is returned.
 *   FALSE is returned on error.
 */
function mfcs_request_view_0_detect_requirements_changes($form, $form_state, &$extra) {
  if (!is_array($form)) {
    return FALSE;
  }

  if (!is_array($form_state)) {
    return FALSE;
  }

  $requirements_form = array();
  if (isset($form['review']['operations']['requirements'])) {
    $requirements_form = $form['review']['operations']['requirements'];
  }

  $requirements_state = array();
  if (isset($form_state['values']['review']['operations']['requirements'])) {
    $requirements_state = $form_state['values']['review']['operations']['requirements'];
  }

  $insurance_form = array();
  if (isset($form['review']['operations']['insurance'])) {
    $insurance_form = $form['review']['operations']['insurance'];
  }

  $insurance_state = array();
  if (isset($form_state['values']['review']['operations']['insurance'])) {
    $insurance_state = $form_state['values']['review']['operations']['insurance'];
  }

  $operations_form = array();
  if (isset($form['review']['operations'])) {
    $operations_form = $form['review']['operations'];
  }

  $operations_state = array();
  if (isset($form_state['values']['review']['operations'])) {
    $operations_state = $form_state['values']['review']['operations'];
  }

  $changes_requirements = array(
    'facilities_fees' => array('personnel' => FALSE, 'hours' => FALSE, 'days' => FALSE, 'rate' => FALSE, 'supplies' => FALSE, 'post' => FALSE, 'description' => FALSE),
    'equipment_fees' => array('personnel' => FALSE, 'hours' => FALSE, 'days' => FALSE, 'rate' => FALSE, 'supplies' => FALSE, 'post' => FALSE, 'description' => FALSE),
    'custodial_fees' => array('personnel' => FALSE, 'hours' => FALSE, 'days' => FALSE, 'rate' => FALSE, 'supplies' => FALSE, 'post' => FALSE, 'description' => FALSE),
    'maintenance_fees' => array('personnel' => FALSE, 'hours' => FALSE, 'days' => FALSE, 'rate' => FALSE, 'supplies' => FALSE, 'post' => FALSE, 'description' => FALSE),
    'grounds_fees' => array('personnel' => FALSE, 'hours' => FALSE, 'days' => FALSE, 'rate' => FALSE, 'supplies' => FALSE, 'post' => FALSE, 'description' => FALSE),
    'security_fees' => array('personnel' => FALSE, 'hours' => FALSE, 'days' => FALSE, 'rate' => FALSE, 'supplies' => FALSE, 'post' => FALSE, 'description' => FALSE),
  );

  $requirements_groups = array(
    'personnel' => 'group_rates',
    'hours' => 'group_rates',
    'days' => 'group_rates',
    'rate' => 'group_rates',
    'supplies' => 'group_supplies',
    'post' => 'group_post',
    'description' => 'group_post',
  );

  $changes_waived = array(
    'university' => FALSE,
  );

  $changes_insurance = array(
    'provided' => FALSE,
    'contractor' => FALSE,
    'unaffiliated' => FALSE,
  );

  $changes_made = FALSE;
  foreach ($changes_requirements as $requirements_field => &$columns) {
    if (!isset($requirements_form[$requirements_field])) {
      continue;
    }

    foreach ($columns as $column_name => &$change) {
      if (!isset($requirements_form[$requirements_field][$requirements_groups[$column_name]][$column_name])) {
        continue;
      }

      $form_column = $requirements_form[$requirements_field][$requirements_groups[$column_name]][$column_name];

      // if the field is not set but was previously set, then this is a change.
      if (!array_key_exists($column_name, $requirements_state[$requirements_field])) {
        continue;
      }

      // some browsers like to put '\r\n' or '\r', in the string value fields.
      // this causes the string length test to fail, so strip out and remove all '\r\n' and '\r', making them all a consistent '\n' (identical to the way the string is stored in the database).
      if ($column_name == 'description') {
        $requirements_state[$requirements_field][$column_name] = str_replace("\r\n", "\n", $requirements_state[$requirements_field][$column_name]);
        $requirements_state[$requirements_field][$column_name] = str_replace("\r", "\n", $requirements_state[$requirements_field][$column_name]);

        $extra['requirements'][$requirements_field][$column_name] = $requirements_state[$requirements_field][$column_name];
      }

      $value_length = strlen(trim($requirements_state[$requirements_field][$column_name]));
      if ($value_length == 0) {
        $original_length = strlen(trim($form_column['#default_value']));
        if ($original_length != $value_length) {
          $change = NULL;
          $changes_made = TRUE;
          break;
        }
      }
      elseif ($form_column['#default_value'] != $requirements_state[$requirements_field][$column_name]) {
        $change = TRUE;
        $changes_made = TRUE;
      }
    }
    unset($change);

    if (!isset($extra['requirements']) || !array_key_exists($requirements_field, $extra['requirements']) || !array_key_exists($column_name, $extra['requirements'][$requirements_field])) {
      continue;
    }

    foreach ($columns as $column_name => $change) {
      // remove requirements fields that have no changes detected.
      if ($change === FALSE) {
        unset($extra['requirements'][$requirements_field][$column_name]);
      }
    }

    // if not fields are provided, then remove the entire requirement.
    if (empty($extra['requirements'][$requirements_field])) {
      unset($extra['requirements'][$requirements_field]);
    }
    unset($change);
  }

  // process university business changes.
  if (isset($operations_form['university_business']) && isset($operations_state['university_business'])) {
    if ($operations_form['university_business']['#default_value'] != $operations_state['university_business']) {
      $changes_waived['university'] = TRUE;

      $changes_made = TRUE;
    }
    else {
      unset($extra['requirements']['university']);
    }
  }

  // process insurance changes.
  foreach ($changes_insurance as $insurance_field => &$change) {
    if (!isset($insurance_form[$insurance_field]) || !isset($insurance_state[$insurance_field])) {
      continue;
    }

    if ($insurance_form[$insurance_field]['#default_value'] != $insurance_state[$insurance_field]) {
      $change = TRUE;

      $changes_made = TRUE;
    }
  }
  unset($change);

  // remove insurance fields that have no changes detected.
  foreach ($changes_insurance as $insurance_name => $change) {
    if (isset($extra['insurance']) && array_key_exists($insurance_name, $extra['insurance'])) {
      if ($change === FALSE) {
        unset($extra['insurance'][$insurance_name]);
      }
      elseif (is_null($change)) {
        $extra[$insurance_name] = NULL;
      }
    }
  }
  unset($change);

  if (empty($extra['insurance'])) {
    unset($extra['insurance']);
  }

  return $changes_made;
}

/**
 * Determine which steps are allowed for the given request and given user.
 *
 * This helps make sure the decision options are populated based on the current users roles and the current review step.
 *
 * @param array $request
 *   An already loaded request array.
 * @param object $user
 *   An already loaded user object.
 * @param bool $administer
 *    Defines whether or not the current user is an administer.
 * @param bool $manager
 *    Defines whether or not the current user is a manager.
 * @param bool $venue_coordinator
 *    Defines whether or not the current user is a venue coordinator.
 * @param bool $reviewer
 *    Defines whether or not the current user is a reviewer.
 *
 * @return array
 *   An array of valid decision on success.
 */
function mfcs_request_view_0_detect_allowed_decisions($request, $user, $administer, $manager, $venue_coordinator, $reviewer) {
  if (!is_array($request)) {
    cf_error::invalid_array('request');
    return array();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return array();
  }

  if (!is_bool($venue_coordinator)) {
    cf_error::invalid_bool('venue_coordinator');
    return array();
  }

  if (!is_bool($manager)) {
    cf_error::invalid_bool('manager');
    return array();
  }

  if (!is_bool($reviewer)) {
    cf_error::invalid_bool('reviewer');
    return array();
  }

  if (!mfcs_request_access('review', $request, $user)) {
    return array();
  }

  if ($reviewer || $manager) {
    $allowed_statuses = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
      MFCS_REQUEST_STATUS_CLOSED_DENIED,
      MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
      MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
      MFCS_REQUEST_STATUS_CANCELLED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
    );
  }
  else {
    $allowed_statuses = array(
      MFCS_REQUEST_STATUS_UNLOCKED,
      MFCS_REQUEST_STATUS_LOCKED,
    );
  }

  $step = $request['top']['step'][0]->value;
  $status = $request['top']['status'][0]->value;

  $decision_options = mfcs_get_reviewer_decision_list_options(NULL, MFCS_RESTRICTION_DECISION_COMMENT_ONLY);

  if (!in_array($status, $allowed_statuses)) {
    return $decision_options;
  }

  if ($administer) {
    $decision_options = mfcs_get_reviewer_decision_list_options(NULL, MFCS_RESTRICTION_DECISION_ADMINISTER);
  }
  elseif ($manager) {
    $decision_options = mfcs_get_reviewer_decision_list_options(NULL, MFCS_RESTRICTION_DECISION_MANAGER);
  }
  elseif ($reviewer) {
    $decision_options = mfcs_get_reviewer_decision_list_options(NULL, MFCS_RESTRICTION_DECISION_REVIEWER);
  }

  if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
    // venue coordinators get special options.
    if ($venue_coordinator && !$manager && !$administer) {
      $decision_options = mfcs_get_reviewer_decision_list_options(NULL, MFCS_RESTRICTION_DECISION_VENUE_COORDINATOR);
    }

    // remove decisions that are not allowed during this step.
    unset($decision_options[MFCS_REVIEW_DECISION_ISSUES]);
    unset($decision_options[MFCS_REVIEW_DECISION_ISSUES_NONE]);
    unset($decision_options[MFCS_REVIEW_DECISION_APPROVE]);
    unset($decision_options[MFCS_REVIEW_DECISION_DENY]);
  }
  elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
    // remove decisions that are not allowed during this step.
    unset($decision_options[MFCS_REVIEW_DECISION_AVAILABLE]);
    unset($decision_options[MFCS_REVIEW_DECISION_UNAVAILABLE]);
    unset($decision_options[MFCS_REVIEW_DECISION_APPROVE]);
    unset($decision_options[MFCS_REVIEW_DECISION_DENY]);

  }
  elseif ($step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    // remove decisions that are not allowed during this step.
    unset($decision_options[MFCS_REVIEW_DECISION_AVAILABLE]);
    unset($decision_options[MFCS_REVIEW_DECISION_UNAVAILABLE]);
  }
  elseif ($step == MFCS_REVIEW_STEP_COMPLETED) {
    // remove decisions that are not allowed during this step.
    unset($decision_options[MFCS_REVIEW_DECISION_AVAILABLE]);
    unset($decision_options[MFCS_REVIEW_DECISION_UNAVAILABLE]);
    unset($decision_options[MFCS_REVIEW_DECISION_APPROVE]);
    unset($decision_options[MFCS_REVIEW_DECISION_DENY]);
  }

  return $decision_options;
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

  $mode = $form['form']['mode']['#value'];

  $use_revision = NULL;
  if (isset($form['form']['use_revision']['#value'])) {
    $use_revision = $form['form']['use_revision']['#value'];
  }

  if (is_null($use_revision)) {
    $form_state['redirect'] = mfcs_build_redirect_array('/requests/pdf-0/' . $request_id . '/' . $mode);
  }
  else {
    $form_state['redirect'] = mfcs_build_redirect_array('/requests/pdf-0/' . $request_id . '/' . $mode . '/' . $use_revision);
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
 * @} End of '@addtogroup mfcs'.
 */
