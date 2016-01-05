<?php

/**
 * @file
 * Defines McNeese facilities use request create page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request create page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $clone_request
 *   (optional) When specified, will clone an existing request in which the
 *   current user has appropriate access to view and edit.
 *
 * @return array $form
 *   An array containing the generated form.
 */
function mfcs_request_create_0_form($form, &$form_state, $clone_request = NULL) {
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

  $form = array();

  $request = NULL;
  if (!is_null($clone_request)) {
    if (!cf_is_integer($clone_request)) {
      cf_error::invalid_integer('clone_request');
      drupal_not_found();
      drupal_exit();
    }

    $request = mfcs_load_request_by_id($clone_request);

    $can_clone = mfcs_page_request_access('create', $clone_request);

    // if the request does not exist or the user cannot access the request to copy, fallback to a normal create.
    if (empty($request) || !$can_clone) {
      $request = NULL;
      $clone_request = NULL;
    }
  }

  if (is_null($clone_request)) {
    mfcs_add_canonical_headers('requests/create-0');
  }
  else {
    mfcs_add_canonical_headers('requests/create-0/' . $clone_request);
  }

  $page_title = "Create Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Copy Request: " . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  mfcs_include(MFCS_INCLUDE_STRUCTURE);

  if (empty($form_state['values']['form']['step'])) {
    $step = 'agreement';
  }
  else {
    $step = $form_state['values']['form']['step'];
  }

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['agreement'] = array(
    '#tree' => TRUE,
  );

  $form['request'] = array(
    '#tree' => TRUE,
  );

  $form['form']['sections'] = array(
    '#type' => 'value',
    '#value' => mfcs_get_request_section_information(),
  );

  $form_state['conditionally_required'] = array('#tree' => FALSE, '#type' => 'value');
  $form_state['conditionally_required'][0] = array();

  $form['form']['step'] = array(
    '#id' => 'field-form-step',
    '#type' => 'value',
    '#value' => $step,
  );

  $form['form']['new'] = array(
    '#id' => 'field-form-new',
    '#type' => 'value',
    '#value' => TRUE,
  );

  $form['form']['existing'] = array(
    '#id' => 'field-form-existing',
    '#type' => 'value',
    '#value' => array(),
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

  if (is_array($request)) {
    $form['form']['existing']['#value'] = mfcs_get_request_existing_values($request);
    $request_fields = &$form['form']['existing']['#value']['request'];

    // Reset fields that should not be copied by the clone process.
    unset($request_fields['status']);
    unset($request_fields['step']);
    unset($request_fields['review']);
    unset($request_fields['requirements']);
    unset($request_fields['insurance']);
  }

  $statement_markup = 'McNeese State University is happy to provide facilities and resources to our Students, Employees, and the Community to further educational, cultural, and recreational initiatives through requests held on campus.' . "<br>\n";
  $statement_markup .= "<br>\n";
  $statement_markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the Facilities Use and Special Event Request form. The following questionnaire assists campus officials in not only scheduling campus facilities or maintaining a master calendar; it allows better service through inter-departmental communication and collaboration enhancing the excellence of McNeese.' . "<br>\n";
  $statement_markup .= "<br>\n";
  $statement_markup .= 'The questionnaire is designed according to the University\'s <a href="http://www.mcneese.edu/node/3217">Facility Use Policy</a>. Any activity scheduled on behalf of the University, regardless of venue, must be submitted.' . "<br>\n";
  $statement_markup .= "<br>\n";
  $statement_markup .= 'If you are only making a request for a <em>Table Reservation</em> in the <em>New Ranch</em> or in the <em>Quad</em>, then you should instead use the <a href="http://www.mcneese.edu/studentunion/student_union_reservations">Table Request Form</a>.' . "<br>\n";
  $statement_markup .= "<br>\n";
  $statement_markup .= 'Confirmation and approval of the submitted request must be received before any publicity is scheduled.';
  $statement_markup .= "<br>\n";

  if ($step == 'agreement') {
    $form['agreement']['statement'] = array(
      '#id' => 'section-agreement',
      '#type' => 'markup',
      '#markup' => $statement_markup,
    );

    $form['agreement']['agree'] = array(
      '#id' => 'field-agreement-agree',
      '#type' => 'checkbox',
      '#title' => 'I have read the above information and agree to the terms of the <a href="http://www.mcneese.edu/node/3217">Facility Use Policy</a>.',
    );

    $form['agreement']['submit'] = array(
      '#id' => 'submit-agreement',
      '#name' => 'submit-agreement',
      '#type' => 'submit',
      '#default_value' => t("Continue"),
    );

    $form['request'] = array(
      '#id' => 'section-request',
      '#tree' => TRUE,
    );
  }
  else {
    $form['request'] = mfcs_get_request_fields($form, $form_state);
    $form['request']['#id'] = 'section-request';
    $form['request']['#tree'] = TRUE;

    if ($step != 'location') {
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

  return $form;
}

/**
 * Pre-Validation for mfcs_request_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_create_0_form()
 */
function mfcs_request_create_0_form_pre_validate(&$form, &$form_state) {
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
 * Validation for mfcs_request_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_create_0_form()
 */
function mfcs_request_create_0_form_validate($form, &$form_state) {
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
    form_set_error('form][step', "An internal error occured. Form Step is not defined. Please contact the System Administrator.");
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
    if ($form_state['values']['form']['step'] == 'agreement') {
      if ($form_state['values']['agreement']['agree']) {
        $form_state['values']['form']['step'] = 'location';
      }
      else {
        form_set_error('agreement][agree', "You must agree to the terms to continue.");
      }
    }
    elseif ($form_state['values']['form']['step'] == 'location') {
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
      elseif ($clicked_id == 'submit-request-submit') {
        if (isset($form_state['values']['request']['information']['type'][0]) && $form_state['values']['request']['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING) {
          if (!empty($form['form']['user']['#value']) && user_access('mfcs manage', $form['form']['user']['#value'])) {
            $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
            $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
            $form_state['rebuild'] = FALSE;
          }
          elseif (!empty($form_state['values']['request']['venue_coordinator']['user_id'][0]) && !empty($form['form']['user']['#value']->uid)) {
            if ($form_state['values']['request']['venue_coordinator']['user_id'][0] == $form['form']['user']['#value']->uid) {
              $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
              $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
              $form_state['rebuild'] = FALSE;
            }
            else {
              $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $form_state['values']['request']['venue_coordinator']['user_id'][0], 'mpvc.disabled' => 0));
              if (empty($proxies_returned['results'])) {
                form_set_error('request][information][type][0', "You are not authorized to create a quick meeting.");
              }
              else {
                $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
                $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
                $form_state['rebuild'] = FALSE;
              }
            }
          }
          else {
            form_set_error('request][information][type][0', "An internal error occured while trying to validating the submit process for a quick meeting. Please contact the support staff.");
          }
        }

        // this can happen when the coordinators classifications change while the user is using the form.
        if (empty($form_state['values']['request']['request_coordinator']['classification'][0]) || !is_numeric($form_state['values']['request']['request_coordinator']['classification'][0])) {
          form_set_error('request][request_coordinator][classification][0', "No request coordinator classification has been specified.");
        }
      }
      else {
        $room = explode('_', $form_state['values']['request']['room'][0], 3);
        $date = $form_state['values']['request']['dates']['date'];
        $time_start = $form_state['values']['request']['dates']['time_start'];
        $time_stop = $form_state['values']['request']['dates']['time_stop'];

        foreach ($date as $key => $d) {
          $v = mfcs_convert_value_to_database_format($d, 'date_popup', array('view' => 'Y/m/d'), $form_state, $key);

          if ($v === FALSE) {
            unset($date[$key]);
            unset($time_start[$key]);
            unset($time_stop[$key]);
          }
          else {
            $date[$key] = $v;

            $v = mfcs_convert_value_to_database_format($time_start[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'request][dates][date'), $form_state, $key);
            if ($v === FALSE) {
              $time_start[$key] = NULL;
              $time_stop[$key] = NULL;
            }
            else {
              $time_start[$key] = $v;

              $v = mfcs_convert_value_to_database_format($time_stop[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'request][dates][date'), $form_state, $key);
              if ($v === FALSE) {
                $time_start[$key] = NULL;
                $time_stop[$key] = NULL;
              }
              else {
                $time_stop[$key] = $v;
              }
            }
          }

          unset($v);
        }

        if (!empty($date)) {
          $requests_using_room = mfcs_check_room_availability($room[0], $date, $time_start, $time_stop);

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

            // force the status to be closed due to room being unavailable on save.
            if (!isset($form_state['room_unavailable_prior_settings'])) {
              $form_state['room_unavailable_prior_settings'] = array(
                'status' => NULL,
                'step' => NULL,
              );

              if (isset($form_state['values']['request']['status'])) {
                $form_state['room_unavailable_prior_settings']['status'] = $form_state['values']['request']['status'][0];
              }

              if (isset($form_state['values']['request']['step'])) {
                $form_state['room_unavailable_prior_settings']['step'] = $form_state['values']['request']['step'][0];
              }

              $request_using_room = reset($requests_using_room);
              if (isset($request_using_room->status) && $request_using_room->status === MFCS_REQUEST_STATUS_LOCKED) {
                $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK;
              }
              else {
                $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE;
              }
              unset($request_using_room);

              $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
            }
          }
          else {
            // restore status settings in case an available room was selected.
            if (isset($form_state['room_unavailable_prior_settings'])) {
              $form_state['values']['request']['status'][0] = $form_state['room_unavailable_prior_settings']['status'];
              $form_state['values']['request']['step'][0] = $form_state['room_unavailable_prior_settings']['step'];

              unset($form_state['room_unavailable_prior_settings']);
            }
          }
        }

        unset($room);
        unset($date);
        unset($time_start);
        unset($time_stop);


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
    elseif ($form_state['values']['form']['step'] == 'clone') {
      $form_state['rebuild'] = FALSE;
    }
  }
}

/**
 * Submit process for mfcs_request_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_create_0_form()
 */
function mfcs_request_create_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $user_data = array(
    'user_id' => $user->uid,
  );

  $instance = mfcs_instance();

  $mfcs_requests = array(
    'user_id' => (int) $user_data['user_id'],
    'created' => $instance,
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

  $mfcs_request_revisions = array(
    'date' => $instance,
    'revision' => 0,
    'user_id' => $user->uid,
  );

  mfcs_include(MFCS_INCLUDE_TABLE);

  $structure = mfcs_table_structure();
  $structure_current = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    foreach ($fields as $field_name => &$field_values) {
      $structure_current[$table_name . '_' . $field_name] = 0;

      // manual processing fields must not be auto-populated through the normal form submit process.
      if (array_key_exists('manual_processing', $field_values) && $field_values['manual_processing']) {
        continue;
      }

      // store the values for each individual mfcs_field_* table.
      $db_table_field = 'mfcs_field_' . $table_name . '_' . $field_name;

      $current = &$form_state['values'];
      $parts = explode('][', $field_values['path']);
      if (empty($parts)) {
        unset($current);
        $current = NULL;
      }
      else {
        foreach ($parts as $part) {
          if (is_array($current) && array_key_exists($part, $current)) {
            $current = &$current[$part];

            if (empty($current)) {
              $current = NULL;
            }
          }
          else {
            watchdog(MFCS_WATCHDOG_ID, "Failed to process '@part' for '@field_name' in '@table_name' during form submit. Invalid array or missing array key (new).", array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
            unset($current);
            $current = NULL;
            continue 2;
          }
        }
      }

      mfcs_build_table_structure($structure, $structure_current, $structure_field_tables, $table_name, $field_name, $db_table_field, $user_data, $current, $form_state);
    }
  }

  $failure = FALSE;
  $transaction = db_transaction();

  try {
    // now perform the operations.
    $query = db_insert('mfcs_requests');
    $query->fields($mfcs_requests);
    $request_id = $query->execute();

    if (!is_numeric($request_id) || $request_id <= 0) {
      // the request was not properly saved, present an error here.
      $transaction->rollback();
      return FALSE;
    }

    // save the request id so that a programmatic caller can determine the request_id.
    $form_state['created_request_id'] = $request_id;

    $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

    $mfcs_request_revisions['request_id'] = $request_id;

    if (!$processed_mfcs_request_revisions) {
      $query = db_insert('mfcs_request_revisions');
      $query->fields($mfcs_request_revisions);
      $query->execute();

      $processed_mfcs_request_revisions = TRUE;
    }

    foreach ($structure_field_tables as $table_name => $fields) {
      foreach ($fields as $delta => $values) {
        $values['request_id'] = $request_id;

        $query = db_insert($table_name);
        $query->fields($values);
        $query->execute();
      }
    }

    $structure_current['request_id'] = $request_id;
    $query = db_insert('mfcs_current');
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
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to save the request.", array(), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }
  else {
    mfcs_include(MFCS_INCLUDE_WORKFLOW);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['new'] = TRUE;
    $parameters['status'] = $form_state['values']['request']['status'][0];
    $parameters['step'] = $form_state['values']['request']['step'][0];
    mfcs_send_workflow_emails($request_id, $parameters);
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
