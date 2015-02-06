<?php

/**
 * @file
 * Defines McNeese facilities use request edit page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request edit page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for a request.
 * @param string|null $target_section
 *   (optional) Load only the specified section.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_edit_0_form($form, &$form_state, $request_id, $target_section = NULL) {
  // force the page to load the view page when only the id is specified.
  if (is_null($target_section)) {
    drupal_goto('requests/view-0/' . $request_id, array('query' => drupal_get_query_parameters()));
    return '';
  }

  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    drupal_not_found();
    drupal_exit();
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @id.", array('@id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  mfcs_include(MFCS_INCLUDE_STRUCTURE);

  $section_information = mfcs_get_request_section_information();

  if (empty($section_information[$target_section]['steps'])) {
    watchdog(MFCS_WATCHDOG_ID, "The section @target_section does not exist or has no steps.", array('@target_section' => $target_section), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  if (is_null($target_section)) {
    mfcs_add_canonical_headers('requests/edit-0/' . $request_id);
  }
  else {
    mfcs_add_canonical_headers('requests/edit-0/' . $request_id . '/' . $target_section);
  }

  $form = array();

  if (empty($form_state['values']['form']['step'])) {
    $step = reset($section_information[$target_section]['steps']);
  }
  else {
    $step = $form_state['values']['form']['step'];
  }

  if ($step == 'cancel') {
    return array();
  }


  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['sections'] = array(
    '#type' => 'value',
    '#value' => mfcs_get_request_section_information(),
  );

  $form['form']['step'] = array(
    '#id' => 'field-form-step',
    '#type' => 'value',
    '#value' => $step,
  );

  $form['form']['new'] = array(
    '#id' => 'field-form-new',
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['form']['existing'] = array(
    '#id' => 'field-form-existing',
    '#type' => 'value',
    '#value' => mfcs_get_request_existing_values($request),
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  $form['form']['parents'] = array(
    '#id' => 'field-form-parents',
    '#type' => 'value',
    '#value' => array(
      'structure' => array('request'),
      'path' => 'request[',
    ),
  );

  $form['form']['request'] = array(
    '#id' => 'field-form-request',
    '#type' => 'value',
    '#value' => $request,
  );

  $form['form']['target_section'] = array(
    '#id' => 'field-form-target_section',
    '#type' => 'value',
    '#value' => $target_section,
  );


  // $target_section determines which sections are to be made visible and which aren't.
  if ($target_section == 'request') {
    // in particular, the 'request' is used to load the entire form, such as during an edit process.
  }
  elseif (array_key_exists($target_section, $form['form']['sections']['#value'])) {
    $form['form']['sections']['#value'] = array($target_section => $form['form']['sections']['#value'][$target_section]);
  }

  $form['request'] = array(
    '#tree' => TRUE,
  );

  $form['request'] = mfcs_get_request_fields($form, $form_state);
  $form['request']['#id'] = 'section-request';
  $form['request']['#tree'] = TRUE;

  if ($target_section == 'request') {
    if ($step == 'location') {
      $form['request']['cancel'] = array(
        '#id' => 'submit-request-cancel',
        '#name' => 'submit-request-cancel',
        '#type' => 'submit',
        '#default_value' => t("Cancel"),
      );
    }
    else {
      $form['request']['prev'] = array(
        '#id' => 'submit-request-prev',
        '#name' => 'submit-request-prev',
        '#type' => 'submit',
        '#default_value' => t("Back"),
      );
    }

    if ($step == 'standard') {
      $form['request']['next'] = array(
        '#id' => 'submit-request-next',
        '#name' => 'submit-request-next',
        '#type' => 'submit',
        '#default_value' => t("Continue"),
        '#states' => array(
          'invisible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type][0]' . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
          ),
        ),
      );

      $form['request']['submit'] = array(
        '#id' => 'submit-request-submit',
        '#name' => 'submit-request-submit',
        '#type' => 'submit',
        '#default_value' => t("Submit"),
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type][0]' . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
          ),
        ),
      );
    }
    elseif ($step == 'details') {
      $form['request']['submit'] = array(
        '#id' => 'submit-request-submit',
        '#name' => 'submit-request-submit',
        '#type' => 'submit',
        '#default_value' => t("Submit"),
      );
    }
    else {
      $form['request']['next'] = array(
        '#id' => 'submit-request-next',
        '#name' => 'submit-request-next',
        '#type' => 'submit',
        '#default_value' => t("Continue"),
      );
    }
  }
  else {
    $form['request']['cancel'] = array(
      '#id' => 'submit-request-cancel',
      '#name' => 'submit-request-cancel',
      '#type' => 'submit',
      '#default_value' => t("Cancel"),
    );

    $form['request']['submit'] = array(
      '#id' => 'submit-request-submit',
      '#name' => 'submit-request-submit',
      '#type' => 'submit',
      '#default_value' => t("Submit"),
    );
  }

  return $form;
}

/**
 * Pre-Validation for mfcs_request_edit_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_edit_0_form()
 */
function mfcs_request_edit_0_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $clicked_sub_id = NULL;
  $clicked_id_part = NULL;
  $clicked_id_parts = explode('-', $clicked_id);
  if (!empty($clicked_id_parts)) {
    $clicked_id_part = array_pop($clicked_id_parts);
    $clicked_sub_id = implode('-', $clicked_id_parts);
  }
  unset($clicked_id_parts);

  // do not perform validation when not going backwards or adding fields.
  $dont_validate = array(
    'submit-request-cancel' => TRUE,
    'submit-request-prev' => TRUE,
    'submit-request-add-date' => TRUE,
    'submit-request-remove-date' => TRUE,
  );

  if ($clicked_sub_id == 'submit-request-remove-date' && is_numeric($clicked_id_part)) {
    $dont_validate[$clicked_id] = TRUE;
  }

  mfcs_include(MFCS_INCLUDE_VALIDATION);

  mfcs_pre_validate_request_fields($form, $form_state, $dont_validate);
}

/**
 * Validation for mfcs_request_edit_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_edit_0_form()
 */
function mfcs_request_edit_0_form_validate($form, &$form_state) {
  $no_problems = TRUE;
  $form_state['rebuild'] = TRUE;

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $clicked_sub_id = NULL;
  $clicked_id_part = NULL;
  $clicked_id_parts = explode('-', $clicked_id);
  if (!empty($clicked_id_parts)) {
    $clicked_id_part = array_pop($clicked_id_parts);
    $clicked_sub_id = implode('-', $clicked_id_parts);
  }
  unset($clicked_id_parts);

  if (empty($form_state['values']['form']['step'])) {
    return;
  }

  $request = $form['form']['request']['#value'];
  $request_id = (int) $request['mer']['id'][0]->value;

  $user = NULL;
  if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
    $user = $form['form']['user']['#value'];
  }

  $can_edit_request = mfcs_request_access('edit', $request, $user);
  $can_cancel_request = mfcs_request_access('cancel', $request, $user);

  if (!$can_edit_request) {
    form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
    $no_problems = FALSE;
  }

  if ($clicked_id == 'submit-request-cancel') {
    if (!$can_cancel_request) {
      form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
      $no_problems = FALSE;
    }

    if (isset($form_state['ajax'])) {
      // ajax method of cancelling.
      $form_state['values']['form']['step'] = 'cancel';
    }
    else {
      // non-ajax method of cancelling.
      $form_state['rebuild'] = FALSE;

      if (empty($form_state['values']['redirect_to'])) {
        if (empty($request_id)) {
          $form_state['redirect'] = mfcs_build_redirect_array('requests');
        }
        else {
          $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/');
        }
      }
      else {
        $form_state['redirect'] = $form_state['values']['redirect_to'];
      }
    }

    return;
  }

  if (empty($form_state['values']['form']['step'])) {
    form_set_error('form][step', "An internal error occured. Form Step is not defined. Please contact the support staff.");
  }

  // do not perform validation when not going backwards or adding fields.
  $dont_validate = array(
    'submit-request-prev' => TRUE,
    'submit-request-add-date' => TRUE,
  );

  if ($clicked_sub_id == 'submit-request-remove-date' && is_numeric($clicked_id_part)) {
    $dont_validate[$clicked_id] = TRUE;
  }

  if (!array_key_exists($clicked_id, $dont_validate)) {
    mfcs_include(MFCS_INCLUDE_VALIDATION);

    $no_problems = mfcs_validate_request_fields($form, $form_state);
    if (is_null($no_problems)) {
      $no_problems = TRUE;
    }
  }

  if ($no_problems) {
    if ($form['form']['target_section']['#value'] == 'request') {
      if ($form_state['values']['form']['step'] == 'location') {
        $form_state['values']['form']['step'] = 'rooms';
      }
      elseif ($form_state['values']['form']['step'] == 'rooms') {
        if ($clicked_id == 'submit-request-prev') {
          $form_state['values']['form']['step'] = 'location';
        }
        else {
          $form_state['values']['form']['step'] = 'standard';
        }
      }
      elseif ($form_state['values']['form']['step'] == 'standard') {
        if ($clicked_id == 'submit-request-prev') {
          $form_state['values']['form']['step'] = 'rooms';
        }
        elseif ($clicked_id == 'submit-request-add-date') {
          $next_date = count($form_state['values']['request']['dates']['date']);
          $form_state['values']['request']['dates']['date'][$next_date] = NULL;
          $form_state['values']['request']['dates']['time_start'][$next_date] = NULL;
          $form_state['values']['request']['dates']['time_stop'][$next_date] = NULL;
          $form_state['values']['request']['dates']['category'][$next_date] = NULL;
        }
        elseif ($clicked_sub_id == 'submit-request-remove-date' && is_numeric($clicked_id_part)) {
          unset($form_state['values']['request']['dates']['date'][$clicked_id_part]);
          unset($form_state['values']['request']['dates']['time_start'][$clicked_id_part]);
          unset($form_state['values']['request']['dates']['time_stop'][$clicked_id_part]);
          unset($form_state['values']['request']['dates']['category'][$clicked_id_part]);
        }
        else {
          $room = explode('_', $form_state['values']['request']['room'][0], 3);
          $date = $form_state['values']['request']['dates']['date'];
          $time_start = $form_state['values']['request']['dates']['time_start'];
          $time_stop = $form_state['values']['request']['dates']['time_stop'];
          $category = $form_state['values']['request']['dates']['category'];

          foreach ($date as $key => $d) {
            $v = mfcs_convert_value_to_database_format($d, 'date_popup', array('view' => 'Y/m/d'), $form_state, $key);

            if ($v === FALSE) {
              unset($date[$key]);
              unset($time_start[$key]);
              unset($time_stop[$key]);
              unset($category[$key]);
            }
            else {
              $date[$key] = $v;

              $v = mfcs_convert_value_to_database_format($time_start[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'request][dates][date'), $form_state, $key);
              if ($v === FALSE) {
                $time_start[$key] = NULL;
                $time_stop[$key] = NULL;
                $category[$key] = NULL;
              }
              else {
                $time_start[$key] = $v;

                $v = mfcs_convert_value_to_database_format($time_stop[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'request][dates][date'), $form_state, $key);
                if ($v === FALSE) {
                  $time_start[$key] = NULL;
                  $time_stop[$key] = NULL;
                  $category[$key] = NULL;
                }
                else {
                  $time_stop[$key] = $v;
                }
              }
            }

            unset($v);
          }

          if (!empty($date)) {
            $requests_using_room = mfcs_check_room_availability($room[0], $date, $time_start, $time_stop, $request_id);

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
              mfcs_display_room_unavailable_message($problematic_dates);
            }
          }

          unset($room);
          unset($date);
          unset($time_start);
          unset($time_stop);
          unset($category);

          if (empty($form_state['values']['request']['information']['type'][0])) {
            form_set_error('request][information][type][0', "The <strong>" . $form['request']['information']['type'][0]['#title'] . "</strong> is required.");
          }
          else {
            $type_term = taxonomy_term_load($form_state['values']['request']['information']['type'][0]);

            if (is_object($type_term)) {
              $form_state['values']['form']['step'] = 'details';
            }
            else {
              form_set_error('request][information][type][0', "An invalid <strong>" . $form['request']['information']['type'][0]['#title'] . "</strong> is has been selected.");
            }
          }
        }
      }
      elseif ($form_state['values']['form']['step'] == 'details') {
        if ($clicked_id == 'submit-request-prev') {
          $form_state['values']['form']['step'] = 'standard';
        }
        elseif ($clicked_id == 'submit-request-submit') {
          $form_state['rebuild'] = FALSE;
        }
      }
    }
    else {
      if ($clicked_id == 'submit-request-submit') {
        $form_state['rebuild'] = FALSE;
      }
      elseif ($clicked_id == 'submit-request-add-date') {
        $next_date = count($form_state['values']['request']['dates']['date']);
        $form_state['values']['request']['dates']['date'][$next_date] = NULL;
        $form_state['values']['request']['dates']['time_start'][$next_date] = NULL;
        $form_state['values']['request']['dates']['time_stop'][$next_date] = NULL;
        $form_state['values']['request']['dates']['category'][$next_date] = NULL;
      }
      elseif ($clicked_sub_id == 'submit-request-remove-date' && is_numeric($clicked_id_part)) {
        unset($form_state['values']['request']['dates']['date'][$clicked_id_part]);
        unset($form_state['values']['request']['dates']['time_start'][$clicked_id_part]);
        unset($form_state['values']['request']['dates']['time_stop'][$clicked_id_part]);
        unset($form_state['values']['request']['dates']['category'][$clicked_id_part]);
      }
    }
  }
}

/**
 * Submit process for mfcs_request_edit_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_edit_0_form()
 */
function mfcs_request_edit_0_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $failure = FALSE;
  $request = &$form['form']['request']['#value'];
  $request_id = (int) $request['mer']['id'][0]->value;
  $status = (int) $request['top']['status'][0]->value;
  $step = (int) $request['top']['step'][0]->value;
  $requirements = $request['current']['requirements'];

  // make sure the cancel button is never treated as a save submit.
  if ($clicked_id == 'submit-request-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $user = cf_current_user();

  $user_data = array(
    'user_id' => $user->uid,
  );

  $instance = mfcs_instance();

  $mfcs_requests = array(
    'updated' => $instance,
    'status' =>  $form_state['values']['request']['status'][0],
    'step' => $form_state['values']['request']['step'][0],
    'type' =>  $form_state['values']['request']['information']['type'][0],
    'classification' => $form_state['values']['request']['request_coordinator']['classification'][0],
    'location' =>  $form_state['values']['request']['location'][0],
    'building' => $form_state['values']['request']['building'][0],
    'room' => $form_state['values']['request']['room'][0],
    'venue_coordinator' => $form_state['values']['request']['venue_coordinator']['user_id'][0],
    'title' => $form_state['values']['request']['information']['title'][0],
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

  if ($request_revision === FALSE) {
    form_set_error('form', "An error occurred while trying to save the request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "Failed to load revision number for request %request_id while trying to save the request.", array('%request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }
  elseif (is_null($request_revision)) {
    $request_revision_next = 0;
  }
  else {
    $request_revision_next = $request_revision + 1;
  }

  $mfcs_request_revisions = array(
    'request_id' => $request_id,
    'date' => $instance,
    'revision' => $request_revision_next,
    'user_id' => $user->uid,
  );

  mfcs_include(MFCS_INCLUDE_TABLE);

  $structure = mfcs_table_structure();
  $structure_current = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    foreach ($fields as $field_name => &$field_values) {
      // manual processing fields must not be auto-populated through the normal form submit process.
      if (array_key_exists('manual_processing', $field_values) && $field_values['manual_processing']) {
        continue;
      }

      // load the path so that it can be checked to see if anything needs to be changed.
      $parts = explode('][', $field_values['path']);
      if (empty($parts)) {
        // this should not happen.
        continue;
      }

      unset($current_existing);
      unset($current_new);

      $current_existing = &$form['form']['existing']['#value'];
      $current_new = &$form_state['values'];

      while (!empty($parts)){
        $part = array_shift($parts);

        if (!is_array($current_existing) || !array_key_exists($part, $current_existing)) {
          watchdog(MFCS_WATCHDOG_ID, "Failed to process '@part' for '@field_name' in '@table_name' during form submit. Invalid array or missing array key (existing).", array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
          continue 2;
        }

        if (!is_array($current_new) || !array_key_exists($part, $current_new)) {
          watchdog(MFCS_WATCHDOG_ID, "Failed to process '@part' for '@field_name' in '@table_name' during form submit. Invalid array or missing array key (new).", array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
          continue 2;
        }

        $current_existing = &$current_existing[$part];
        $current_new = &$current_new[$part];
      }
      unset($part);

      // when the values are the same, do not update them.
      if ($current_existing == $current_new) {
        continue;
      }

      $db_table_field = 'mfcs_field_' . $table_name . '_' . $field_name;

      mfcs_build_table_structure($structure, $structure_current, $structure_field_tables, $table_name, $field_name, $db_table_field, $user_data, $current_new, $form_state, $request);
    }
  }

  if (!empty($structure_field_tables)) {
    $failure = FALSE;
    $transaction = db_transaction();

    try {
      // now perform the operations.
      $query = db_update('mfcs_requests');
      $query->condition('id', $request_id);
      $query->fields($mfcs_requests);
      $query->execute();

      if (!$processed_mfcs_request_revisions) {
        $query = db_insert('mfcs_request_revisions');
        $query->fields($mfcs_request_revisions);
        $query->execute();

        $processed_mfcs_request_revisions = TRUE;
      }

      foreach ($structure_field_tables as $table_name => $fields) {
        foreach ($fields as $delta => $values) {
          $query = db_insert($table_name);
          $query->fields($values);
          $query->execute();
        }
      }

      // if the request has been updated while it is in a locked status, the review log must be reset/restarted.
      if ($status == MFCS_REQUEST_STATUS_LOCKED) {
        $step_revision = $request['current']['top']['step'] + 1;
        $structure_current['top_step'] = $step_revision;

        $mfcs_field_top_step = array(
          'request_id' => $request_id,
          'revision' => $step_revision,
          'delta' => 0,
          'user_id' => $user->uid,
          'date' => $instance,
          'value' => MFCS_REVIEW_STEP_VENUE_AVAILABLE,
        );

        $mfcs_requests = array(
          'step' => MFCS_REVIEW_STEP_VENUE_AVAILABLE,
          'updated' => $instance,
        );

        $query = db_update('mfcs_requests');
        $query->fields($mfcs_requests);
        $query->condition('id', $request_id);
        $query->execute();

        $query = db_insert('mfcs_field_top_step');
        $query->fields($mfcs_field_top_step);
        $query->execute();

        $query = db_delete('mfcs_reviewed');
        $query->condition('request_id', $request_id);
        $query->execute();

        $review_revision = mfcs_load_request_revision_number($request_id, 'mfcs_field_review_review');

        if ($review_revision === FALSE || is_null($review_revision)) {
          $review_revision = 0;
        }
        else {
          $review_revision++;
        }

        $structure_current['review_review'] = $review_revision;

        $mfcs_field_review_review = array(
          'request_id' => $request_id,
          'revision' => $review_revision,
          'delta' => 0,
          'user_id' => $user->uid,
          'date' => $instance,
          'step' => $step,
          'decision' => MFCS_REVIEW_DECISION_AMENDMENT,
          'message' => '',
        );

        $query = db_insert('mfcs_field_review_review');
        $query->fields($mfcs_field_review_review);
        $query->execute();

        $mfcs_current['requirements_facilities'] = NULL;
        $mfcs_current['requirements_equipment'] = NULL;
        $mfcs_current['requirements_custodial'] = NULL;
        $mfcs_current['requirements_security'] = NULL;
        $mfcs_current['requirements_other'] = NULL;
        $mfcs_current['requirements_waived'] = NULL;
        $mfcs_current['requirements_university'] = NULL;

        $mfcs_current['insurance_contractor'] = NULL;
        $mfcs_current['insurance_unaffiliated'] = NULL;
        $mfcs_current['insurance_provided'] = NULL;
      }

      // update the current revisions table.
      $query = db_update('mfcs_current');
      $query->condition('request_id', $request_id);
      $query->fields($structure_current);
      $query->execute();

      // enforce transaction execution
      unset($transaction);
    }
    catch (Exception $ex) {
      $transaction->rollback();
      cf_error::on_query_execution($ex);

      $failure = TRUE;
    }

    if ($failure) {
      form_set_error('form', "An error occurred while trying to save the request. Please contact the support staff.");
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to save the request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
    else {
      // forcefully clear the request cache so that it can reflect the changes.
      mfcs_load_request_by_id($request_id, FALSE);

      mfcs_include(MFCS_INCLUDE_WORKFLOW);

      // send an e-mail to all reviewers that the request step has been reset.
      if ($status == MFCS_REQUEST_STATUS_LOCKED) {
        $parameters['amendment'] = TRUE;
      }
      else {
        $parameters['update'] = TRUE;
      }

      $parameters = array();
      $parameters['changed_by'] = $user->uid;
      $parameters['changed_type'] = 'user';
      mfcs_send_workflow_emails($request_id, $parameters);
    }
  }


  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
