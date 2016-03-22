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
  elseif (isset($form_state['reassign_was_clicked'])) {
    mfcs_include(MFCS_INCLUDE_REQUEST_REASSIGN);

    $cancel_form = array();
    $cancel_form_state = array();
    return mfcs_request_reassign_0_form($cancel_form, $cancel_form_state, $request_id);
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
    $can_reassign_request = FALSE;
  }
  elseif ($mode == MFCS_VIEW_MODE_OPERATIONS) {
    $show_display = FALSE;
    $show_log = FALSE;
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
      if (mfcs_disabled_users_with_active_content($request['venue_coordinator']['user_id'][0]->value)) {
        $user_venue_coordinator = mfcs_load_basic_user_settings_by_user_id($request['venue_coordinator']['user_id'][0]->value);

        if (!empty($user_venue_coordinator['name'])) {
          global $base_path;

          if (!empty($user_venue_coordinator['first_name']) && !empty($user_venue_coordinator['last_name'])) {
            if ($manager || $administer) {
              $user_venue_coordinator_name = '<a href="' . $base_path . 'user/' . $request['venue_coordinator']['user_id'][0]->value . '" title="User ID: ' . $request['venue_coordinator']['user_id'][0]->value .'">' . $user_venue_coordinator['first_name'] . ' ' . $user_venue_coordinator['last_name'] . '</a>';
            }
            else {
              $user_venue_coordinator_name = $user_venue_coordinator['first_name'] . ' ' . $user_venue_coordinator['last_name'];
            }
          }
          else {
            if ($manager || $administer) {
              $user_venue_coordinator_name = '<a href="' . $base_path . 'user/' . $request['venue_coordinator']['user_id'][0]->value . '" title="User ID: ' . $request['venue_coordinator']['user_id'][0]->value .'">' . $user_venue_coordinator['name'] . '</a>';
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
          if (is_string($reviewer_classification)) {
            $reviewer_classification = (int) $reviewer_classification;
          }

          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          // the last decision made is the first item and should be the only value displayed.
          $decision = array_shift($reviewer_decisions);
          if (is_string($decision)) {
            $decision = (int) $decision;
          }
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
          if (is_string($reviewer_classification)) {
            $reviewer_classification = (int) $reviewer_classification;
          }

          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          // the last decision made is the first item and should be the only value displayed.
          $decision = array_shift($reviewer_decisions);
          if (is_string($decision)) {
            $decision = (int) $decision;
          }

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

      if ($can_reassign_request && is_null($use_revision)) {
        $form['display']['submit']['reassign'] = array(
          '#id' => 'request-display-submit-reassign',
          '#name' => 'request-display-submit-reassign',
          '#type' => 'submit',
          '#default_value' => t("Re-Assign"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'request-display-submit-reassign',
              'mode-' . $mode,
            ),
          ),
          '#submit' => array(
            'mfcs_request_reassign_0_form_submit',
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

    $step_options = mfcs_get_request_steps_list_options();

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
      if ((!empty($decisions[$request_id]) && is_array($decisions[$request_id])) || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
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
        elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
          $yet_to_review[$request['venue_coordinator']['user_id'][0]->value] = "Venue Coordinator";
        }

        if (!empty($yet_to_review)) {
          $module_path = drupal_get_path('module', 'mfcs');
          drupal_add_js($module_path . '/js/request_view-reviewers.js', array('type' => 'file', 'group' => JS_DEFAULT, 'preprocess' => TRUE));

          $yet_to_review_markup .= '<div id="request-' . $request_id . '-section-review-log-yet_to_review" class="request-section-review-log-yet_to_review">' . "\n";

          if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
            if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Reviewer classifications that have not yet reviewed:</div>' . "\n";
            }
            elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Requirements that have not yet reviewed:</div>' . "\n";
            }

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
                  $yet_to_review_markup .= ' <span class="details_list-user_id">[id: ' . $user_id . ']</span>' . "\n";
                  $yet_to_review_markup .= '</li>' . "\n";
                }
              }

              $yet_to_review_markup .= '</ul>' . "\n";
              $yet_to_review_markup .= '</div>' . "\n";
              $yet_to_review_markup .= '</li>' . "\n";
            }

            $yet_to_review_markup .= '</ul>' . "\n";
          }
          elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $request['top']['step'][0]->value == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
            if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Waiting for the Venue Coordinator or a Venue Coordinator Proxy:</div>' . "\n";
            }
            elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
              $yet_to_review_markup .= '<div class="yet_to_review-label">Final Decision is waiting for the Venue Coordinator or a Venue Coordinator Proxy:</div>' . "\n";
            }

            $yet_to_review_markup .= '<ul class="yet_to_review-list">' . "\n";
            $yet_to_review_markup .= '<li class="yet_to_review-list_item">' . "\n";
            $yet_to_review_markup .= "Venue Coordinator" . "\n";

            $basic_user_settings = mfcs_load_basic_user_settings_by_user_id($request['venue_coordinator']['user_id'][0]->value);
            if ($basic_user_settings !== FALSE) {
              // create a javascript based users list to easily display the users associated with the given classification.
              $yet_to_review_markup .= '<div class="details_list inline" title="View list of users associated with this reviewer classification.">' . "\n";
              $yet_to_review_markup .= '<span class="no-script-hidden no-print details_list-label label-venue_coordinator">(<span class="details_list-link link-venue_coordinator inline fake_link">Details</span>)</span>';
              $yet_to_review_markup .= '<ul class="script-hidden details_list-item item-venue_coordinator">' . "\n";

              $yet_to_review_markup .= '<li class="details_list-user">' . "\n";
              if (empty($basic_user_settings['first_name']) || empty($basic_user_settings['last_name'])) {
                $yet_to_review_markup .= '<span class="details_list-user_name">' . $basic_user_settings['name'] . '</span>' . "\n";
              }
              else {
                $yet_to_review_markup .= '<span class="details_list-user_name">' . "\n";
                $yet_to_review_markup .= $basic_user_settings['first_name'] . ' ' . $basic_user_settings['last_name'] . "\n";
                $yet_to_review_markup .= '</span>' . "\n";
              }
              $yet_to_review_markup .= ' <span class="details_list-user_id">[id: ' . $request['venue_coordinator']['user_id'][0]->value . ']</span>' . "\n";
              $yet_to_review_markup .= '</li>' . "\n";
              $yet_to_review_markup .= '</ul>' . "\n";
              $yet_to_review_markup .= '</div>' . "\n";
            }

            $yet_to_review_markup .= '</li>' . "\n";

            // do not display the venue coordinator in the proxy list.
            $request_proxies = array();
            if (isset($request['venue_coordinator']['user_id'][0]->value)) {
              $request_proxies = mfcs_get_venue_coordinator_proxy($request['venue_coordinator']['user_id'][0]->value);
            }

            if (isset($request_proxies[$request['venue_coordinator']['user_id'][0]->value])) {
              unset($request_proxies[$request['venue_coordinator']['user_id'][0]->value]);
            }

            if (!empty($request_proxies)) {
              $yet_to_review_markup .= '<li class="yet_to_review-list_item">' . "\n";
              $yet_to_review_markup .= "Venue Coordinator Proxy" . "\n";

              // create a javascript based users list to easily display the users associated with the given classification.
              $yet_to_review_markup .= '<div class="details_list inline" title="View list of users allowed to proxy as the venue coordinator.">' . "\n";
              $yet_to_review_markup .= '<span class="no-script-hidden no-print details_list-label label-venue_coordinator_proxy">(<span class="details_list-link link-venue_coordinator_proxy inline fake_link">Details</span>)</span>';
              $yet_to_review_markup .= '<ul class="script-hidden details_list-item item-venue_coordinator_proxy">' . "\n";

              foreach ($request_proxies as $proxy_id) {
                $basic_user_settings = mfcs_load_basic_user_settings_by_user_id($proxy_id);
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

                $yet_to_review_markup .= ' <span class="details_list-user_id">[id: ' . $proxy_id . ']</span>' . "\n";
                $yet_to_review_markup .= '</li>' . "\n";
              }

              $yet_to_review_markup .= '</ul>' . "\n";
              $yet_to_review_markup .= '</div>' . "\n";

              $yet_to_review_markup .= '</li>' . "\n";
            }
            unset($request_proxies);

            $yet_to_review_markup .= '</ul>' . "\n";
          }

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

      mfcs_view_build_reviews_log($form, $form_state, $request_id, $use_revision);

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

    if ($show_operations && !$contained && is_null($use_revision)) {
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

      $form['review']['operations']['instructions']['#markup'] .= "<div>The current step is: <strong>" . $step_options[$step] . "</strong>.</div>\n";

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

      if ($can_comment_request && !$can_review_request) {
        if (empty($reviewer_classifications) && empty($venue_coordinator)) {
          if ($administer) {
            $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR] = 'Administrator';
          }
          elseif ($manager) {
            $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = 'Manager';
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
      if ($can_review_request && ($step === MFCS_REVIEW_STEP_REVIEW || $step === MFCS_REVIEW_STEP_REQUIREMENTS || $step === MFCS_REVIEW_STEP_MAKE_DECISIONS || $step === MFCS_REVIEW_STEP_COMPLETED)) {
        if ($status === MFCS_REQUEST_STATUS_LOCKED || $status === MFCS_REQUEST_STATUS_CLOSED_ACCEPTED) {
          if (isset($reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING])) {
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
        if (empty($reviewer_classifications) || ($step !== MFCS_REVIEW_STEP_REVIEW && $step !== MFCS_REVIEW_STEP_REQUIREMENTS)) {

          if ($request['venue_coordinator']['user_id'][0]->value == $user->uid) {
            $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
            $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
          }
          else {
            $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = "Venue Coordinator (Proxy)";
            $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = "Venue Coordinator (Proxy)";
          }
        }
      }

      // if the current user has no review classifications for the current step, then do not provide the "Approve/Deny/Comment" operations appropriately.
      if (empty($reviewer_classifications)) {
        unset($decision_options[MFCS_REVIEW_DECISION_APPROVE]);
        unset($decision_options[MFCS_REVIEW_DECISION_DENY]);

        if (!$can_comment_request) {
          unset($decision_options[MFCS_REVIEW_DECISION_COMMENT]);
        }
      }

      // make the 'requirement' step available only under very specific situations.
      if ($insurance_provided_available) {
        $decision_options[MFCS_REVIEW_DECISION_REQUIREMENT] = $decision_options_all[MFCS_REVIEW_DECISION_REQUIREMENT];

        if ($step !== MFCS_REVIEW_STEP_REVIEW) {
          $decision_options = array(
            MFCS_REVIEW_DECISION_REQUIREMENT => $decision_options_all[MFCS_REVIEW_DECISION_REQUIREMENT]
          );

          if ($can_comment_request) {
            $decision_options[MFCS_REVIEW_DECISION_COMMENT] = $decision_options_all[MFCS_REVIEW_DECISION_COMMENT];
          }

          $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_PURCHASING] = $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING];
          $insurance_provided_available_only = TRUE;
        }
      }
      elseif ($step !== MFCS_REVIEW_STEP_VENUE_AVAILABLE && $step !== MFCS_REVIEW_STEP_REVIEW) {
        unset($decision_options[MFCS_REVIEW_DECISION_REQUIREMENT]);
      }
      elseif (empty($reviewer_classifications)) {
        unset($decision_options[MFCS_REVIEW_DECISION_REQUIREMENT]);
      }

      if (!empty($reviewer_classifications)) {
        $form['review']['operations']['as_reviewer']['#markup'] .= '<div class="request-section-review-operations-as_reviewer-message">You are reviewing with the following classifications:</div>';
        $form['review']['operations']['as_reviewer']['#markup'] .= theme('item_list', array('items' => $reviewer_classifications, 'type' => 'ul', 'attributes' => array('class' => array('request-section-review-operations-as_reviewer-list'))));
      }

      $form['review']['operations']['as_reviewer']['#markup'] .= '</div>';

      // restrict options to commenting only, if the user is only allowed to comment.
      if (!$can_review_request && $can_comment_request) {
        $decision_options = array(
          MFCS_REVIEW_DECISION_COMMENT => $decision_options_all[MFCS_REVIEW_DECISION_COMMENT],
        );
      }

      $form['review']['operations']['warnings'] = array(
        '#type' => 'markup',
        '#markup' => "",
      );

      // Display warning messages if the user can make a decision.
      if ($can_review_request && !empty($reviewer_classifications) && ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step === MFCS_REVIEW_STEP_REVIEW || $step === MFCS_REVIEW_STEP_REQUIREMENTS || $step === MFCS_REVIEW_STEP_MAKE_DECISIONS)) {
        $holidays = array();
        foreach ($request['dates']['date'] as $date_delta => $date_value) {
          $holidays = mfcs_get_holidays((int) date('Y', $request['dates']['date'][$date_delta]->value), $request['dates']['date'][$date_delta]->value);
        }

        if (!empty($holidays)) {
          $form['review']['operations']['warnings']['#markup'] .= '<div class="warning-holidays">' . "\n";
          $form['review']['operations']['warnings']['#markup'] .= '<div class="warning-holidays-title">This request is scheduled on the same dates as the following:</div>' . "\n";
          $form['review']['operations']['warnings']['#markup'] .= '<ul class="warning-holidays-list">' . "\n";
          foreach ($holidays as $holiday) {
            $form['review']['operations']['warnings']['#markup'] .= '<li class="warning-holidays-list-item">' . t("<strong>@holiday</strong> on: <em>@date</em>.", array('@date' => date('l, F j, Y', $holiday->date), '@holiday' => $holiday->title)) . '</li>' . "\n";
          }
          $form['review']['operations']['warnings']['#markup'] .= '</ul>' . "\n";
          $form['review']['operations']['warnings']['#markup'] .= '</div>' . "\n";
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

      if ($can_review_request && ($step === MFCS_REVIEW_STEP_REVIEW || $step === MFCS_REVIEW_STEP_VENUE_AVAILABLE || $insurance_provided_available)) {
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
        $uses_as_manager = array();
        if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
          if ($venue_coordinator) {
            $uses['facilities'] = "Facilities";
            $uses['equipment'] = "Additional Equipment";
          }
          elseif ($manager) {
            $uses['facilities'] = "Facilities";
            $uses['equipment'] = "Additional Equipment";

            $uses_as_manager['facilities'] = "Facitilies Fees";
            $uses_as_manager['equipment'] = "Equipment Fees";
          }
        }
        else {
          if ($manager) {
            $uses['custodial'] = "Custodial";
            $uses['maintenance'] = "Maintenance";
            $uses['grounds'] = "Grounds";
            $uses['security'] = "Security";
            $insurance_requirement = TRUE;

            $form['form']['requirements-custodial']['#value'] = FALSE;
            $form['form']['requirements-maintenance']['#value'] = FALSE;
            $form['form']['requirements-grounds']['#value'] = FALSE;
            $form['form']['requirements-security']['#value'] = FALSE;
            $form['form']['requirements-insurance']['#value'] = FALSE;

            $uses_as_manager['custodial'] = "Custodial Fees";
            $uses_as_manager['maintenance'] = "Maintenance Fees";
            $uses_as_manager['grounds'] = "Grounds Fees";
            $uses_as_manager['security'] = "Security Fees";
            $uses_as_manager['insurance'] = "Insurance Fees";
          }

          foreach ($reviewers as $reviewers_reviewer) {
            if (is_string($reviewers_reviewer->reviewer_classification)) {
              $reviewer_classification = (int) $reviewers_reviewer->reviewer_classification;
            }
            else {
              $reviewer_classification = $reviewers_reviewer->reviewer_classification;
            }

            if ($reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_FACILITIES_CUSTODIAL || $reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL) {
              $uses['custodial'] = "Custodial";
              $form['form']['requirements-custodial']['#value'] = TRUE;
              unset($uses_as_manager['custodial']);
            }
            elseif ($reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE) {
              $uses['maintenance'] = "Maintenance";
              $form['form']['requirements-maintenance']['#value'] = TRUE;
              unset($uses_as_manager['maintenance']);
            }
            elseif ($reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_FACILITIES) {
              $uses['grounds'] = "Grounds";
              $form['form']['requirements-grounds']['#value'] = TRUE;
              unset($uses_as_manager['grounds']);
            }
            elseif ($reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_SECURITY) {
              $uses['security'] = "Security";
              $form['form']['requirements-security']['#value'] = TRUE;
              unset($uses_as_manager['security']);
            }
            elseif ($reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_PURCHASING) {
              $insurance_requirement = TRUE;
              $form['form']['requirements-insurance']['#value'] = TRUE;
              unset($uses_as_manager['insurance']);
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

        $states = array(
          'visible' => array(
            ':input[name="review[operations][decision]"]' => array('value' => MFCS_REVIEW_DECISION_REQUIREMENT),
          ),
        );

        foreach ($uses as $use_name => $use_title) {
          if ($uses_review[$use_name] && $step !== MFCS_REVIEW_STEP_REVIEW) {
            continue;
          }

          if (!$uses_review[$use_name] && $step !== MFCS_REVIEW_STEP_REVIEW) {
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
              '#min' => 1,
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

            // do not require this field when used as a manager.
            if (array_key_exists($use_name, $uses_as_manager)) {
              $form['review']['operations']['requirements'][$use_name]['quantity']['#required'] = FALSE;
            }

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

            // do not require this field when used as a manager.
            if (array_key_exists($use_name, $uses_as_manager)) {
              $form['review']['operations']['requirements'][$use_name]['hours']['#required'] = FALSE;
            }

            if (!empty($request['requirements'][$use_name][0])) {
              $form['review']['operations']['requirements'][$use_name]['hours']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->hours, 'currency');
            }
          }

          $form['review']['operations']['requirements'][$use_name]['days'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-days',
            '#type' => 'numberfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee </span>Days',
            '#default_value' => NULL,
            '#min' => 1,
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

          // do not require this field when used as a manager.
          if (array_key_exists($use_name, $uses_as_manager)) {
            $form['review']['operations']['requirements'][$use_name]['days']['#required'] = FALSE;
          }

          if (!empty($request['requirements'][$use_name][0])) {
            $form['review']['operations']['requirements'][$use_name]['days']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->days, 'currency');
          }

          $form['review']['operations']['requirements'][$use_name]['amount'] = array(
            '#id' => 'request-review-operations-requirements-' . $use_name . '-amount',
            '#type' => 'textfield',
            '#title' => '<span class="element-invisible">' . $use_title . ' Fee</span> Amount',
            '#default_value' => NULL,
            '#size' => 16,
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

          // do not require this field when used as a manager.
          if (array_key_exists($use_name, $uses_as_manager)) {
            $form['review']['operations']['requirements'][$use_name]['amount']['#required'] = FALSE;
          }

          if (!empty($request['requirements'][$use_name][0]) && strlen($request['requirements'][$use_name][0]->amount) > 0) {
            $form['review']['operations']['requirements'][$use_name]['amount']['#default_value'] = mfcs_convert_value_from_database_format($request['requirements'][$use_name][0]->amount, 'currency');
          }
          elseif (isset($requirements_amount_defaults[$use_name])) {
            $form['review']['operations']['requirements'][$use_name]['amount']['#default_value'] = $requirements_amount_defaults[$use_name];
          }
        }

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

        // add as_manager information to communicate the requirements that are added solely because they are provided because the user is a manager.
        // these requirement would otherwise not be available.
        if (!empty($uses_as_manager)) {
          // adding a container works arounds drupals lack of support for '#states' with '#markup' fields.
          $form['review']['operations']['requirements']['as_manager'] = array(
            '#type' => 'container',
            '#states' => $states,
            '#attributes' => array(
              'class' => array(
                'requirements-as_manager-wrapper',
              ),
            ),
          );

          $as_manager_markup = '<div class="requirements-as_manager">' . "\n";
          $as_manager_markup .= '<strong class="requirements-as_manager-explanation">The following requirements are made available solely because you have manager access:</strong>' . "<br>\n";
          $as_manager_markup .= '<ul class="requirements-as_manager-items">' . "\n";

          foreach ($uses_as_manager as $use_as_manager_key => $use_as_manager_value) {
            $as_manager_markup .= '<li class="requirements-as_manager-item item-' . $use_as_manager_key . '">' . $use_as_manager_value . '</li>' . "\n";
          }
          unset($use_as_manager_key);
          unset($use_as_manager_value);

          $as_manager_markup .= '</ul>' . "\n";
          $as_manager_markup .= '</div>' . "\n";

          $form['review']['operations']['requirements']['as_manager']['markup'] = array(
            '#markup' => $as_manager_markup,
          );
        }

        if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
          if ($venue_coordinator || $manager) {
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

        if ($insurance_requirement && ($step === MFCS_REVIEW_STEP_REVIEW || $insurance_provided_available)) {

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

      if ($can_review_request && $step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
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

  $unrequire_usage_fields = TRUE;
  if ($decision === MFCS_REVIEW_DECISION_REQUIREMENT) {
    if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      $requirements_waived = FALSE;
      if (isset($form_state['values']['review']['operations']['presidents_approval']) || isset($form_state['values']['review']['operations']['presidents_approval'])) {
        if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
          $requirements_waived = TRUE;
        }
        elseif (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
          $requirements_waived = TRUE;
        }
      }
      else {
        if (isset($form['form']['request']['#value']['requirements']['waived'][0]->value) && $form['form']['request']['#value']['requirements']['waived'][0]->value == 1) {
          $requirements_waived = TRUE;
        }
        elseif (isset($form['form']['request']['#value']['requirements']['university'][0]->value) && $form['form']['request']['#value']['requirements']['university'][0]->value == 1) {
          $requirements_waived = TRUE;
        }
      }

      $unrequire_usage_fields = $requirements_waived;
    }
    else {
      $requirements_waived = FALSE;

      if (isset($form['form']['request']['#value']['requirements']['waived'][0]->value) && $form['form']['request']['#value']['requirements']['waived'][0]->value == 1) {
        $requirements_waived = TRUE;
      }
      elseif (isset($form['form']['request']['#value']['requirements']['university'][0]->value) && $form['form']['request']['#value']['requirements']['university'][0]->value == 1) {
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
  elseif ($clicked_id == 'request-display-submit-reassign') {
    if ($mode != MFCS_VIEW_MODE_NORMAL && $mode != MFCS_VIEW_MODE_DISPLAY) {
      form_set_error('', "This view mode does not allow re-assignment.");
    }

    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
      form_set_error('', "Revisions can not be re-assigned.");
    }

    $form_state['reassign_was_clicked'] = TRUE;
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

    if ($can_comment_request && $decision === MFCS_REVIEW_DECISION_COMMENT) {
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

    // A request was made to have the requirements waived options be displayed with the requirements to avoid confusion.
    // To do this without changing the underlining design, when the requirement is specified with a wave checked, then convert the decision to a wave.
    // @todo: remove the Wave Decision entirely.
    $requirements_waived = FALSE;
    if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if ($decision === MFCS_REVIEW_DECISION_REQUIREMENT) {
        if (isset($form_state['values']['review']['operations']['presidents_approval']) && $form_state['values']['review']['operations']['presidents_approval'] == 1) {
          $form_state['values']['review']['operations']['decision'] = MFCS_REVIEW_DECISION_WAVE;
          $decision = MFCS_REVIEW_DECISION_WAVE;
          $requirements_waived = TRUE;
        }

        if (isset($form_state['values']['review']['operations']['university_business']) && $form_state['values']['review']['operations']['university_business'] == 1) {
          if ($requirements_waived) {
            form_set_error('review][operations][university_business', t("The system does not allow both @presidents_approval and @university_business to be selected at the same time. Choose one or the other (They both wave fees).", array('@presidents_approval' => '"Approved by the President"', '@university_business' => '"Strictly University Business"')));
            $failure = TRUE;
          }
          else {
            $form_state['values']['review']['operations']['decision'] = MFCS_REVIEW_DECISION_WAVE;
            $decision = MFCS_REVIEW_DECISION_WAVE;
            $requirements_waived = TRUE;
          }
        }
      }
      elseif ($decision !== MFCS_REVIEW_DECISION_COMMENT) {
        $check_requirements = TRUE;
      }
    }

    // ticket: fcs-77.
    // make sure the request cannot be acted on when there already is a locked request or have final approval.
    if (($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision === MFCS_REVIEW_DECISION_APPROVE || $decision === MFCS_REVIEW_DECISION_WAVE || $decision === MFCS_REVIEW_DECISION_REQUIREMENT)) || ($step == MFCS_REVIEW_STEP_MAKE_DECISIONS && $decision === MFCS_REVIEW_DECISION_APPROVE)) {
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

      $requests_using_room = mfcs_check_room_availability($room, $all_dates, $all_time_start, $all_time_stop, $request_id);
      unset($all_dates);
      unset($all_time_start);
      unset($all_time_stop);

      if ($requests_using_room === FALSE) {
        form_set_error('', "An internal error occurred while checking the room for availability. Please contact the support staff.");
      }
      elseif (!empty($requests_using_room)) {
        $problematic_dates = array();
        foreach ($requests_using_room as $request_using_room) {
          if (isset($request_using_room->date)) {
            if (isset($request_using_room->time_start)) {
              $problematic_date = date("Y/m/d", $request_using_room->date) . " from " . date("h:ia", $request_using_room->time_start) . " to " . date("h:ia", $request_using_room->time_stop) . '.';
              $problematic_dates[$problematic_date] = $problematic_date;
            }
            else {
              $problematic_date = date("Y/m/d", $request_using_room->date) . " (all day).";
              $problematic_dates[$problematic_date] = $problematic_date;
            }
          }
          elseif (isset($request_using_room->reserved_day)) {
            $problematic_date = date("Y/m/d", $request_using_room->reserved_day) . " from " . date("h:ia", $request_using_room->reserved_time_start) . " to " . date("h:ia", $request_using_room->reserved_time_stop) . '.';
            $problematic_dates[$problematic_date] = $problematic_date;
          }
        }
        ksort($problematic_dates);

        $unavailable_message = "Cannot continue. The room is unavailable for the following dates:<br>\n";
        $unavailable_message .= '<ul>' . "\n";
        foreach ($problematic_dates as $date) {
          $unavailable_message .= '<li>' . $date . '</li>' . "\n";
        }
        $unavailable_message .= '</ul>' . "\n";
        form_set_error('review][operations][decision', $unavailable_message);
      }

      // check for holidays.
      if (!empty($all_holidays)) {
        $holiday = reset($all_holidays);
        if (is_object($holiday)) {
          #form_set_error('review][operations][decision', t("<strong>@date</strong> is not available because of: <strong>@holiday</strong>.", array('@date' => date('l, F j, Y', $holiday->date), '@holiday' => $holiday->title)));
          drupal_set_message(t("Warning: <strong>@date</strong> may not available because of: <strong>@holiday</strong>.", array('@date' => date('l, F j, Y', $holiday->date), '@holiday' => $holiday->title)), 'warning');
        }
        unset($holiday);
      }
      unset($all_holidays);
    }

    // The requirements values should be removed when they are not needed.
    if ($requirements_waived || ($decision !== MFCS_REVIEW_DECISION_REQUIREMENT && $decision !== MFCS_REVIEW_DECISION_WAVE)) {
      unset($form_state['values']['review']['operations']['requirements']);
    }

    if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if ($decision === MFCS_REVIEW_DECISION_APPROVE && $type !== MFCS_REQUEST_TYPE_QUICK_MEETING) {
        $requirements_not_supplied = TRUE;

        if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
          // @todo: convert 'waived' to 'presidents_approval' and 'university' to 'university_business'.
          if (isset($request['requirements']['waived'][0]) && property_exists($request['requirements']['waived'][0], 'value')) {
            if ($request['requirements']['waived'][0]->value) {
              $requirements_not_supplied = FALSE;
            }
          }

          if (isset($request['requirements']['university'][0]) && property_exists($request['requirements']['university'][0], 'value')) {
            if ($request['requirements']['university'][0]->value) {
              $requirements_not_supplied = FALSE;
            }
          }
        }

        if (isset($request['requirements']['facilities'][0]) && property_exists($request['requirements']['facilities'][0], 'days') && !empty($request['requirements']['facilities'][0]->days)) {
          $requirements_not_supplied = FALSE;
        }

        if ($requirements_not_supplied) {
          form_set_error('review][operations][decision', t("The request cannot be approved until the requirements are either specified or waived."));
          $failure = TRUE;
        }
      }
    }

    if ($decision === MFCS_REVIEW_DECISION_REQUIREMENT && !$requirements_waived) {
      $check_requirements = TRUE;
    }

    // @fixme: is autowave valid anymore?
    $form_state['autowave'] = FALSE;

    if ($check_requirements) {
      $requirements = array();
      if (isset($form_state['values']['review']['operations']['requirements'])) {
        $requirements = $form_state['values']['review']['operations']['requirements'];
      }

      $requirements_names = array(
        'facilities' => array('human_name' => "Facilities Fee", 'amount' => TRUE, 'hours' => FALSE, 'days' => TRUE, 'quantity' => FALSE),
        'equipment' => array('human_name' => "Equipment Fee", 'amount' => TRUE, 'hours' => FALSE, 'days' => TRUE, 'quantity' => FALSE),
        'custodial' => array('human_name' => "Custodial Fee", 'amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
        'maintenance' => array('human_name' => "Maintenance Fee", 'amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
        'grounds' => array('human_name' => "Grounds Fee", 'amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
        'security' => array('human_name' => "Security Fee", 'amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
      );

      $requirements_real = array(
        'amount' => array('error_message' => "The :human_name '@number' is not a valid @number_name. It must be a number greater than 0 (with at most 2 decimal places). Do not provide a dollar sign."),
        'hours' => array('error_message' => "The :human_name '@number' is not a valid @number_name. It must be a number greater than 0 (with at most 2 decimal places)."),
      );

      $requirements_integer = array(
        'days' => array(),
        'quantity' => array(),
      );

      foreach ($requirements_names as $requirements_machine_name => $requirements_settings) {
        if (!array_key_exists($requirements_machine_name, $requirements)) {
          continue;
        }

        foreach ($requirements_real as $requirements_real_name => $requirements_real_value) {
          if ($requirements_settings[$requirements_real_name] && isset($requirements[$requirements_machine_name][$requirements_real_name]) && (!empty($requirements[$requirements_machine_name][$requirements_real_name]) || cf_is_integer($requirements[$requirements_machine_name][$requirements_real_name]))) {
            $matches = array();
            $matched = preg_match('/^(\d+|\d+\.\d|\d+\.\d\d)$/i', $requirements[$requirements_machine_name][$requirements_real_name], $matches);

            if (!$matched) {
              form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_real_name, t($requirements_real_value['error_message'], array(':human_name' => $requirements_settings['human_name'], '@number' => $requirements[$requirements_machine_name][$requirements_real_name], '@number_name' => $requirements_real_name)));
              $failure = TRUE;
            }
            else {
              $number = (real) $requirements[$requirements_machine_name][$requirements_real_name];
              if ($number <= 0) {
                form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_real_name, t($requirements_real_value['error_message'], array(':human_name' => $requirements_settings['human_name'], '@number' => $number, '@number_name' => $requirements_real_name)));
                $failure = TRUE;
              }
              else {
                // save the value as a real instead of an integer or string.
                $requirements[$requirements_machine_name][$requirements_real_name] = $number;
              }

              unset($number);
            }
          }
        }

        foreach ($requirements_integer as $requirements_integer_name => $requirements_integer_value) {
          if ($requirements_settings[$requirements_integer_name] && isset($requirements[$requirements_machine_name][$requirements_integer_name]) && (!empty($requirements[$requirements_machine_name][$requirements_integer_name]) || cf_is_integer($requirements[$requirements_machine_name][$requirements_integer_name]))) {
            $number = intval($requirements[$requirements_machine_name][$requirements_integer_name]);

            if (!preg_match('/^\d+$/i', $requirements[$requirements_machine_name][$requirements_integer_name])) {
              form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_integer_name, t("The :human_name '@number' is not a valid @number_name. It is not an allowed number. Use only numbers without decimal places.", array(':human_name' => $requirements_settings['human_name'], '@number' => $requirements[$requirements_machine_name][$requirements_integer_name], '@number_name' => $requirements_integer_name)));
              $failure = TRUE;
            }
            elseif ($number <= 0) {
              form_set_error('review][operations][requirements][' . $requirements_machine_name . '][' . $requirements_integer_name, t("The :human_name '@number' is not a valid @number_name. It must be greater than 0.", array(':human_name' => $requirements_settings['human_name'], '@number' => $number, '@number_name' => $requirements_integer_name)));
              $failure = TRUE;
            }

            unset($number);
          }
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
  elseif ($clicked_id == 'request-reassign-submit-yes' || $clicked_id == 'request-reassign-submit-no') {
    if (isset($form['form']['use_revision']['#value']) && !is_null($form['form']['use_revision']['#value'])) {
      form_set_error('', "Revisions can not be Re-assigned.");
      return;
    }

    if ($clicked_id == 'request-reassign-submit-yes') {
      if (!isset($form['form']['old_coordinator_id']['#value']) || !isset($form['form']['new_coordinator_id']['#value'])) {
        form_set_error('', 'Cannot submit, key information is missing.');
        return;
      }

      if ($form['form']['old_coordinator_id']['#value'] == $form['form']['new_coordinator_id']['#value']) {
        form_set_error('', 'The new coordinator would be the same as the old one. Either the Banner Database Cache is out of sync or the Venue Coordinator for this Room has not been updated in the Banner Database.');
        return;
      }
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
  elseif ($clicked_id == 'request-reassign-submit-yes') {
    $deleted = mfcs_include(MFCS_INCLUDE_REQUEST_REASSIGN);

    mfcs_request_reassign_0_form_submit($form, $form_state);

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
    if ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step === MFCS_REVIEW_STEP_REVIEW) {
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

    if ($decision === MFCS_REVIEW_DECISION_COMMENT) {
      // comments do not need to be processed on a per step basis, so skip the $step conditions below.
    }
    elseif ($step === MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
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

        $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], MFCS_REVIEW_DECISION_REQUIREMENT, "The system has auto-waved the fees because the request is a Quick Meeting.", $requirements_extra, $already_reviewed_as);
      }
    }
    elseif ($step === MFCS_REVIEW_STEP_REVIEW) {
      if (isset($form_state['autowave']) && $form_state['autowave']) {
        // @todo: this might be a valid use of MFCS_REVIEW_DECISION_WAVE. If 'autowave' is removed, then this is no longer necessary.
        $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], MFCS_REVIEW_DECISION_WAVE, "", array('send_email' => FALSE), $already_reviewed_as);
      }
    }

    if ($success) {
      // walk through each requirement and only apply it if values change for a given requirement group.
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

      $requirements_fields = array(
        'facilities' => array('amount' => TRUE, 'hours' => FALSE, 'days' => TRUE, 'quantity' => FALSE),
        'equipment' => array('amount' => TRUE, 'hours' => FALSE, 'days' => TRUE, 'quantity' => FALSE),
        'custodial' => array('amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
        'maintenance' => array('amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
        'grounds' => array('amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
        'security' => array('amount' => TRUE, 'hours' => TRUE, 'days' => TRUE, 'quantity' => TRUE),
      );

      $changes_requirements = array(
        'facilities' => array('detected' => FALSE),
        'equipment' => array('detected' => FALSE),
        'custodial' => array('detected' => FALSE),
        'maintenance' => array('detected' => FALSE),
        'grounds' => array('detected' => FALSE),
        'security' => array('detected' => FALSE),
      );

      $changes_waived = array(
        'waived' => array('detected' => FALSE),
        'university' => array('detected' => FALSE),
        'other' => array('detected' => FALSE),
      );

      $changes_insurance = array(
        'provided' => array('detected' => FALSE),
        'contractor' => array('detected' => FALSE),
        'unaffiliated' => array('detected' => FALSE),
      );

      $any_requirements_changes_made = FALSE;

      // process requirements changes.
      foreach ($changes_requirements as $requirements_field => &$changes) {
        if (!isset($requirements_form[$requirements_field]) || !isset($requirements_state[$requirements_field])) {
          continue;
        }

        foreach ($requirements_fields[$requirements_field] as $requirements_part_key => $requirements_part_value) {
          if (!$requirements_part_value || !isset($requirements_state[$requirements_field][$requirements_part_key]) || !isset($requirements_form[$requirements_field][$requirements_part_key])) {
            continue;
          }

          $value_length = strlen(trim($requirements_state[$requirements_field][$requirements_part_key]));
          if ($value_length == 0) {
            if (array_key_exists('#default_value', $requirements_form[$requirements_field][$requirements_part_key]) && !is_null($requirements_form[$requirements_field][$requirements_part_key]['#default_value'])) {
              if (strlen(trim(($requirements_form[$requirements_field][$requirements_part_key]['#default_value']))) != $value_length) {
                $changes['detected'] = NULL;
                $any_requirements_changes_made = TRUE;
                break;
              }
            }
            else {
              $changes['detected'] = NULL;
              $any_requirements_changes_made = TRUE;
              break;
            }
          }
          elseif ($requirements_form[$requirements_field][$requirements_part_key]['#default_value'] != $requirements_state[$requirements_field][$requirements_part_key]) {
            $changes['detected'] = TRUE;
            $any_requirements_changes_made = TRUE;
          }
          unset($value_length);
        }
      }

      foreach ($changes_requirements as $requirement_name => $requirements_change) {
        if (isset($extra['requirements']) && array_key_exists($requirement_name, $extra['requirements'])) {
          // remove requirements fields that have no changes detected from the "save" variable.
          if ($requirements_change['detected'] === FALSE) {
            unset($extra['requirements'][$requirement_name]);
          }
          // if any requirements fields has a null detected, set the entire group to null.
          elseif (is_null($requirements_change['detected'])) {
            $extra['requirements'][$requirement_name] = NULL;
          }
        }
      }

      // process waived changes.
      // @todo: perform the necessary work to convert this to their proper names, such as 'presidents_approval', or 'university_business' to be more consistent.
      if (isset($requirements_form['other']) && isset($requirements_state['other'])) {
        if ($requirements_form['other']['#default_value'] != $requirements_state['other']) {
          $changes_waived['other']['detected'] = TRUE;

          $any_requirements_changes_made = TRUE;
        }
      }

      if (isset($operations_form['presidents_approval']) && isset($operations_state['presidents_approval'])) {
        if ($operations_form['presidents_approval']['#default_value'] != $operations_state['presidents_approval']) {
          $changes_waived['waived']['detected'] = TRUE;

          $any_requirements_changes_made = TRUE;
        }
      }
      if (isset($operations_form['university_business']) && isset($operations_state['university_business'])) {
        if ($operations_form['university_business']['#default_value'] != $operations_state['university_business']) {
          $changes_waived['university']['detected'] = TRUE;

          $any_requirements_changes_made = TRUE;
        }
      }

      // remove waived fields that have no changes detected from the "save" variable.
      foreach ($changes_waived as $waived_name => $waived_change) {
        if (isset($extra['requirements']) && array_key_exists($waived_name, $extra['requirements'])) {
          if ($waived_change['detected'] === FALSE) {
            unset($extra['requirements'][$waived_name]);
          }
          elseif (is_null($waived_change['detected'])) {
            $extra['requirements'][$waived_name] = NULL;
          }
        }
      }

      // process insurance changes.
      foreach ($changes_insurance as $insurance_field => &$changes) {
        if (!isset($insurance_form[$insurance_field]) || !isset($insurance_state[$insurance_field])) {
          continue;
        }

        if ($insurance_form[$insurance_field]['#default_value'] != $insurance_state[$insurance_field]) {
          $changes['detected'] = TRUE;

          $any_requirements_changes_made = TRUE;
        }
      }

      // remove insurance fields that have no changes detected from the "save" variable.
      foreach ($changes_insurance as $insurance_name => $insurance_change) {
        if (isset($extra['insurance']) && array_key_exists($insurance_name, $extra['insurance'])) {
          if ($insurance_change['detected'] === FALSE) {
            unset($extra[$insurance_name]);
          }
          elseif (is_null($insurance_change['detected'])) {
            $extra[$insurance_name] = NULL;
          }
        }
      }

      if (($decision === MFCS_REVIEW_DECISION_REQUIREMENT || $decision === MFCS_REVIEW_DECISION_WAVE) && !$any_requirements_changes_made) {
        form_set_error('review][operations][decision', "The Requirements decision has been selected, but no changes have been made. If you only wish to leave a comment, then select the Comment instead of Requirements.");
        watchdog(MFCS_WATCHDOG_ID, "The Requirements decision has been selected, but no changes have been made.", array(), WATCHDOG_ERROR);

        $form_state['rebuild'] = TRUE;
        $form_state['redirect'] = FALSE;
        $form_state['submitted'] = FALSE;

        return;
      }

      $success = mfcs_workflow_step_review($request_id, $form['form']['user']['#value'], $decision, $message, $extra, $already_reviewed_as);
    }

    if ($success) {
      if ($decision === MFCS_REVIEW_DECISION_COMMENT) {
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
    if ($decision === MFCS_REVIEW_DECISION_APPROVE || $decision === MFCS_REVIEW_DECISION_DENY) {
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
