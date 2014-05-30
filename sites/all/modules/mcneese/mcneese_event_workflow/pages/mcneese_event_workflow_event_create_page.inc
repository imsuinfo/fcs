<?php

/**
 * @file
 * Defines McNeese event workflow event create page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event create page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $classification
 *   (optional) Number representing the event classification.
 * @param int|null $room
 *   (optional) Number representing the event room.
 *
 * @return array $form
 *   An array containing the generated form.
 */
function mcneese_event_workflow_event_create_0_form($form, &$form_state, $classification = NULL, $room = NULL) {
  $form = array();

  mcneese_event_workflow_include(1);

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
    '#value' => mcneese_event_workflow_get_event_section_information(),
  );

  $form_state['conditionally_required'] = array('#tree' => FALSE, '#type' => 'value');
  $form_state['conditionally_required'][0] = array();

  $form['form']['step'] = array(
    '#id' => 'field-form-step',
    '#type' => 'value',
    '#value' => $step,
  );

  $form['form']['paramater_1'] = array(
    '#id' => 'field-form-parameter_1',
    '#type' => 'value',
    '#value' => $classification,
  );

  $form['form']['paramater_2'] = array(
    '#id' => 'field-form-paramater_2',
    '#type' => 'value',
    '#value' => $room,
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
      '#type' => 'submit',
      '#value' => t("Continue"),
    );
  }
  else {
    $form['event'] = mcneese_event_workflow_get_event_fields($form, $form_state);
    $form['event']['#id'] = 'section-event';
    $form['event']['#tree'] = TRUE;

    if ($step != 'location') {
      $form['event']['prev'] = array(
        '#id' => 'submit-event-prev',
        '#type' => 'submit',
        '#value' => t("Back"),
      );
    }

    if ($step == 'details') {
      $form['event']['submit'] = array(
        '#id' => 'submit-event-submit',
        '#type' => 'submit',
        '#value' => t("Submit"),
      );
    }
    else {
      $form['event']['next'] = array(
        '#id' => 'submit-event-next',
        '#type' => 'submit',
        '#value' => t("Continue"),
      );
    }
  }

  return $form;
}

/**
 * Pre-Validation for mcneese_event_workflow_event_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_create_0_form()
 */
function mcneese_event_workflow_event_create_0_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  if (empty($form_state['values']['form']['step'])) {
    return;
  }

  $dont_validate = array(
    'submit-event-prev' => TRUE,
    'submit-event-add-date' => TRUE,
  );

  if (array_key_exists($clicked_id, $dont_validate)) {
    if (!empty($form_state['conditionally_required'][0])) {
      foreach ($form_state['conditionally_required'][0] as $field) {
        if (is_string($field)) {
          $parts = explode('][', $field);

          $found = TRUE;
          $location = &$form['event'];
          while (!empty($parts)) {
            $part = array_shift($parts);
            if (!is_array($location) || !array_key_exists($part, $location)) {
              $found = FALSE;
              break;
            }

            $location = &$location[$part];
          }

          if ($found && !empty($location)) {
            foreach ($location as $delta => &$location_field) {
              if (!is_numeric($delta)) continue;

              if (array_key_exists('#required', $location_field)) {
                if (isset($location['#options'])) {
                  $location_field['#options'][''] = '';
                }
                $location_field['#required'] = FALSE;
              }
            }
          }
        }
      }
    }

    // @todo: handle conditionally_required[1] fields, which are required only when their dependant fields make them visible based on the '#state' field settings.
  }
}

/**
 * Validation for mcneese_event_workflow_event_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_create_0_form()
 */
function mcneese_event_workflow_event_create_0_form_validate($form, &$form_state) {
  $no_problems = TRUE;
  $form_state['rebuild'] = TRUE;

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  if (empty($form_state['values']['form']['step'])) {
    form_set_error('form][step', "An internal error occured. Form Step is not defined. Please contact the System Administer.");
  }

  // perform validation when not going backwards or adding fields.
  $dont_validate = array(
    'submit-event-prev' => TRUE,
    'submit-event-add-date' => TRUE,
  );

  if (!array_key_exists($clicked_id, $dont_validate)) {
    mcneese_event_workflow_include(2);

    $no_problems = mcneese_event_workflow_validate_event_fields($form, $form_state);
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
      // when classification is readonly, ensure that no changes happen.
      if ($form_state['values']['event']['coordinator']['classification'][0]) {
        if (isset($form['event']['coordinator']['classification']['#value'])) {
          $form_state['values']['event']['coordinator']['classification'][0] = $form['event']['coordinator']['classification'][0]['#value'];
        }
      }

      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'rooms';
      }
      elseif ($clicked_id == 'submit-event-add-date') {
        $next_date = count($form_state['values']['event']['dates']['date']);
        $form_state['values']['event']['dates']['date'][$next_date] = NULL;
        $form_state['values']['event']['dates']['time_start'][$next_date] = NULL;
        $form_state['values']['event']['dates']['time_stop'][$next_date] = NULL;
      }
      elseif ($clicked_id == 'submit-event-remove-date') {
        $prev_date = count($form_state['values']['event']['dates']['date']) - 1;
        unset($form_state['values']['event']['dates']['date'][$prev_date]);
        unset($form_state['values']['event']['dates']['time_start'][$prev_date]);
        unset($form_state['values']['event']['dates']['time_stop'][$prev_date]);
      }
      else {
        $room_list = explode('_', $form_state['values']['event']['room_list'][0], 3);
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

        $events_using_room = mcneese_event_workflow_check_room_availability($room_list[0], $room_list[1], $room_list[2], $date, $time_start, $time_stop);

        unset($room_list);
        unset($date);
        unset($time_start);
        unset($time_stop);

        if (!empty($events_using_room)) {
          mcneese_event_workflow_display_room_unavailable_message();
        }

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
 * Submit process for mcneese_event_workflow_event_create_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_create_0_form()
 */
function mcneese_event_workflow_event_create_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $user_data = array();
  $user_data['user_id'] = $user->uid;

  $mew_event_requests = array();
  $mew_event_requests['user_id'] = (int) $user_data['user_id'];
  $mew_event_requests['created'] = REQUEST_TIME;
  $mew_event_requests['updated'] = REQUEST_TIME;

  $mew_event_revisions = array();
  $mew_event_revisions['date'] = REQUEST_TIME;

  mcneese_event_workflow_include(0);

  $structure = mcneese_event_workflow_secondary_table_structure();
  $structure_tables = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    $db_table = 'mew_current_' . $table_name;
    $structure_tables[$db_table] = array();

    foreach ($fields as $field_name => &$field_values) {
      // @todo: add support for using multiple values instead of using only delta = 0.
      $delta = 0;

      $structure_tables[$db_table][$field_name] = 0;

      // store the values for each individual mew_field_* table.
      $db_table_field = 'mew_field_' . $table_name . '_' . $field_name;

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
            watchdog('mew', "Failed to process '@part' for '@field_name' in '@table_name' during form submit. Invalid array or missing array key (new).", array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
            unset($current);
            $current = NULL;
            continue 2;
          }
        }
      }

      mcneese_event_workflow_build_table_structure($structure, $structure_tables, $structure_field_tables, $table_name, $field_name, $db_table, $db_table_field, $user_data, $current, $form_state);
    }
  }

  $failure = FALSE;
  $transaction = db_transaction();

  try {
    // now perform the operations.
    $query = db_insert('mew_event_requests');
    $query->fields($mew_event_requests);
    $event_id = $query->execute();

    if (!is_numeric($event_id) || $event_id <= 0) {
      // the event was not properly saved, present an error here.
      $transaction->rollback();
      return FALSE;
    }

    $mew_event_revisions['event_id'] = $event_id;

    $query = db_insert('mew_event_revisions');
    $query->fields($mew_event_revisions);
    $query->execute();

    foreach ($structure_field_tables as $table_name => $fields) {
      foreach ($fields as $delta => $values) {
        $values['event_id'] = $event_id;

        $query = db_insert($table_name);
        $query->fields($values);
        $query->execute();
      }
    }

    foreach ($structure_tables as $table_name => $fields) {
      $fields['event_id'] = $event_id;

      $query = db_insert($table_name);
      $query->fields($fields);
      $query->execute();
    }

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    $failure = TRUE;
  }

  if ($failure) {
    form_set_error('form', "An error occurred while trying to save the event request. Please contact the support staff.");
    watchdog('mew', "An error occured while trying to save the event request.", array(), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($event_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      $form_state['redirect'] = 'events/view-0/' . $event_id;
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
