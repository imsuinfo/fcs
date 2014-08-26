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
  $form = array();

  $request = NULL;
  if (!is_null($clone_request)) {
    if (!cf_is_integer($clone_request)) {
      cf_error::invalid_integer('clone_request');
      drupal_not_found();
      drupal_exit();
    }

    $request = mfcs_load_request_by_id($clone_request);

    $can_view = mfcs_management_page_request_access('view', $clone_request);

    // if the request does not exist or the user cannot access the request to copy, fallback to a normal create.
    if (empty($request) || !$can_view) {
      $request = NULL;
      $clone_request = NULL;
    }
  }

  mfcs_include(1);

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

  $statement_markup = 'McNeese State University is happy to provide facilities and resources to our Students, Employees, and the Community to further educational, cultural, and recreational initiatives through requests held on campus.<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the Facilities Use and Special Event Request form. The following questionnaire assists campus officials in not only scheduling campus facilities or maintaining a master calendar; it allows better service through inter-departmental communication and collaboration enhancing the excellence of McNeese.<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'The questionnaire is designed according to the University\'s <a href="http://www.mcneese.edu/node/3217">Facility Use Policy</a>. Any activity scheduled on behalf of the University, regardless of venue, must be submitted.<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'Confirmation and approval of the submitted request must be received before any publicity is scheduled.';
  $statement_markup .= '<br>';

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

    if ($step == 'details') {
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

  mfcs_include(2);

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
    mfcs_include(2);

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
            mfcs_display_room_unavailable_message();

            // force the status to be closed due to room being unavailable on save.
            $form_state['values']['request']['status'][0] = MFCS_EVENT_STATUS_CLOSED_UNAVAILABLE;
            $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
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

  $mfcs_requests = array(
    'user_id' => (int) $user_data['user_id'],
    'created' => REQUEST_TIME,
    'updated' => REQUEST_TIME,
  );

  $mfcs_request_revisions = array(
    'date' => REQUEST_TIME,
    'revision' => 0,
  );

  mfcs_include(0);

  $structure = mfcs_table_structure();
  $structure_tables = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    $db_table = 'mfcs_current_' . $table_name;
    $structure_tables[$db_table] = array();

    foreach ($fields as $field_name => &$field_values) {
      $structure_tables[$db_table][$field_name] = 0;

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

      mfcs_build_table_structure($structure, $structure_tables, $structure_field_tables, $table_name, $field_name, $db_table, $db_table_field, $user_data, $current, $form_state);
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

    $mfcs_request_revisions['request_id'] = $request_id;

    $query = db_insert('mfcs_request_revisions');
    $query->fields($mfcs_request_revisions);
    $query->execute();

    foreach ($structure_field_tables as $table_name => $fields) {
      foreach ($fields as $delta => $values) {
        $values['request_id'] = $request_id;

        $query = db_insert($table_name);
        $query->fields($values);
        $query->execute();
      }
    }

    foreach ($structure_tables as $table_name => $fields) {
      $fields['request_id'] = $request_id;

      $query = db_insert($table_name);
      $query->fields($fields);
      $query->execute();
    }

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
    mfcs_include(6);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['new'] = TRUE;
    mfcs_send_workflow_emails($request_id, $parameters);
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = 'requests';
    }
    else {
      $form_state['redirect'] = 'requests/view-0/' . $request_id;
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */