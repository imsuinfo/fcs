<?php

/**
 * @file
 * Defines McNeese facilities use event create page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use event create page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $clone_event
 *   (optional) When specified, will clone an existing event in which the
 *   current user has appropriate access to view and edit.
 *
 * @return array $form
 *   An array containing the generated form.
 */
function mfcs_event_create_0_form($form, &$form_state, $clone_event = NULL) {
  $form = array();

  $event = NULL;
  if (!is_null($clone_event)) {
    if (!cf_is_integer($clone_event)) {
      cf_error::invalid_integer('clone_event');
      drupal_not_found();
      drupal_exit();
    }

    $event = event_workflow_load_event_by_id($clone_event);

    $can_view = mfcs_management_page_event_access('view', $clone_event);

    // if the event does not exist or the user cannot access the event to copy, fallback to a normal create.
    if (empty($event) || !$can_view) {
      $event = NULL;
      $clone_event = NULL;
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

  $form['event'] = array(
    '#tree' => TRUE,
  );

  $form['form']['sections'] = array(
    '#type' => 'value',
    '#value' => mfcs_get_event_section_information(),
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
      'structure' => array('event'),
      'path' => 'event[',
    ),
  );

  if (is_array($event)) {
    $form['form']['existing']['#value'] = event_workflow_get_event_existing_values($event);
    $event_fields = &$form['form']['existing']['#value']['event'];

    // Reset fields that should not be copied by the clone process.
    unset($event_fields['status']);
    unset($event_fields['step']);
    unset($event_fields['review']);
    unset($event_fields['requirements']);
    unset($event_fields['insurance']);
  }

  $statement_markup = 'McNeese State University is happy to provide facilities and resources to our Students, Employees, and the Community to further educational, cultural, and recreational initiatives through events held on campus.<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'All activities, meetings, or events, excluding scheduled academic classes, must be scheduled through the Facilities Use and Special Event Request form. The following questionnaire assists campus officials in not only scheduling campus facilities or maintaining a master calendar; it allows better service through inter-departmental communication and collaboration enhancing the excellence of McNeese.<br>';
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

    $form['event'] = array(
      '#id' => 'section-event',
      '#tree' => TRUE,
    );
  }
  else {
    $form['event'] = mfcs_get_event_fields($form, $form_state);
    $form['event']['#id'] = 'section-event';
    $form['event']['#tree'] = TRUE;

    if ($step != 'location') {
      $form['event']['prev'] = array(
        '#id' => 'submit-event-prev',
        '#name' => 'submit-event-prev',
        '#type' => 'submit',
        '#default_value' => t("Back"),
      );
    }

    if ($step == 'details') {
      $form['event']['submit'] = array(
        '#id' => 'submit-event-submit',
        '#name' => 'submit-event-submit',
        '#type' => 'submit',
        '#default_value' => t("Submit"),
      );
    }
    else {
      $form['event']['next'] = array(
        '#id' => 'submit-event-next',
        '#name' => 'submit-event-next',
        '#type' => 'submit',
        '#default_value' => t("Continue"),
      );
    }
  }

  return $form;
}

/**
 * Pre-Validation for mfcs_event_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_create_0_form()
 */
function mfcs_event_create_0_form_pre_validate(&$form, &$form_state) {
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
    'submit-event-prev' => TRUE,
    'submit-event-add-date' => TRUE,
    'submit-event-remove-date' => TRUE,
  );

  if ($clicked_sub_id == 'submit-event-remove-date' && is_numeric($clicked_id_part)) {
    $dont_validate[$clicked_id] = TRUE;
  }

  mfcs_include(2);

  mfcs_pre_validate_event_fields($form, $form_state, $dont_validate);
}

/**
 * Validation for mfcs_event_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_create_0_form()
 */
function mfcs_event_create_0_form_validate($form, &$form_state) {
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
    'submit-event-prev' => TRUE,
    'submit-event-add-date' => TRUE,
  );

  if ($clicked_sub_id == 'submit-event-remove-date' && is_numeric($clicked_id_part)) {
    $dont_validate[$clicked_id] = TRUE;
  }

  if (!array_key_exists($clicked_id, $dont_validate)) {
    mfcs_include(2);

    $no_problems = mfcs_validate_event_fields($form, $form_state);
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
      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'location';
      }
      else {
        $form_state['values']['form']['step'] = 'standard';
      }
    }
    elseif ($form_state['values']['form']['step'] == 'standard') {
      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'rooms';
      }
      elseif ($clicked_id == 'submit-event-add-date') {
        $next_date = count($form_state['values']['event']['dates']['date']);
        $form_state['values']['event']['dates']['date'][$next_date] = NULL;
        $form_state['values']['event']['dates']['time_start'][$next_date] = NULL;
        $form_state['values']['event']['dates']['time_stop'][$next_date] = NULL;
        $form_state['values']['event']['dates']['category'][$next_date] = NULL;
      }
      elseif ($clicked_sub_id == 'submit-event-remove-date' && is_numeric($clicked_id_part)) {
        unset($form_state['values']['event']['dates']['date'][$clicked_id_part]);
        unset($form_state['values']['event']['dates']['time_start'][$clicked_id_part]);
        unset($form_state['values']['event']['dates']['time_stop'][$clicked_id_part]);
        unset($form_state['values']['event']['dates']['category'][$clicked_id_part]);
      }
      else {
        $room = explode('_', $form_state['values']['event']['room'][0], 3);
        $date = $form_state['values']['event']['dates']['date'];
        $time_start = $form_state['values']['event']['dates']['time_start'];
        $time_stop = $form_state['values']['event']['dates']['time_stop'];

        foreach ($date as $key => $d) {
          $v = event_workflow_convert_value_to_database_format($d, 'date_popup', array('view' => 'Y/m/d'), $form_state, $key);

          if ($v === FALSE) {
            unset($date[$key]);
            unset($time_start[$key]);
            unset($time_stop[$key]);
          }
          else {
            $date[$key] = $v;

            $v = event_workflow_convert_value_to_database_format($time_start[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'event][dates][date'), $form_state, $key);
            if ($v === FALSE) {
              $time_start[$key] = NULL;
              $time_stop[$key] = NULL;
            }
            else {
              $time_start[$key] = $v;

              $v = event_workflow_convert_value_to_database_format($time_stop[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'event][dates][date'), $form_state, $key);
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
          $events_using_room = mfcs_check_room_availability($room[0], $date, $time_start, $time_stop);

          if ($events_using_room === FALSE) {
            form_set_error('', "An internal error occurred while checking the room for availability. Please contact the support staff.");
          }
          elseif (!empty($events_using_room)) {
            mfcs_display_room_unavailable_message();

            // force the status to be closed due to room being unavailable on save.
            $form_state['values']['event']['status'][0] = MFCS_EVENT_STATUS_CLOSED_UNAVAILABLE;
            $form_state['values']['event']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
          }
        }

        unset($room);
        unset($date);
        unset($time_start);
        unset($time_stop);


        if (empty($form_state['values']['event']['information']['type'][0])) {
          form_set_error('event][information][type][0', "The <strong>" . $form['event']['information']['type'][0]['#title'] . "</strong> is required.");
        }
        else {
          $type_term = taxonomy_term_load($form_state['values']['event']['information']['type'][0]);

          if (is_object($type_term)) {
            $form_state['values']['form']['step'] = 'details';
          }
          else {
            form_set_error('event][information][type][0', "An invalid <strong>" . $form['event']['information']['type'][0]['#title'] . "</strong> is has been selected.");
          }
        }
      }
    }
    elseif ($form_state['values']['form']['step'] == 'details') {
      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'standard';
      }
      elseif ($clicked_id == 'submit-event-submit') {
        $form_state['rebuild'] = FALSE;
      }
    }
  }
}

/**
 * Submit process for mfcs_event_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_create_0_form()
 */
function mfcs_event_create_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $user_data = array(
    'user_id' => $user->uid,
  );

  $mfcs_event_requests = array(
    'user_id' => (int) $user_data['user_id'],
    'created' => REQUEST_TIME,
    'updated' => REQUEST_TIME,
  );

  $mfcs_event_revisions = array(
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
    $query = db_insert('mfcs_event_requests');
    $query->fields($mfcs_event_requests);
    $request_id = $query->execute();

    if (!is_numeric($request_id) || $request_id <= 0) {
      // the event was not properly saved, present an error here.
      $transaction->rollback();
      return FALSE;
    }

    $mfcs_event_revisions['request_id'] = $request_id;

    $query = db_insert('mfcs_event_revisions');
    $query->fields($mfcs_event_revisions);
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
    form_set_error('form', "An error occurred while trying to save the event request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to save the event request.", array(), WATCHDOG_ERROR);

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
    mcneese_