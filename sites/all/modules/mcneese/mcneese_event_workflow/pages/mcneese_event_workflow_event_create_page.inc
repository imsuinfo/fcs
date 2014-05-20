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
 * @param
 *
 * @return array $form
 *   An array containing the generated form.
 */
function mcneese_event_workflow_event_create_0_form($form, &$form_state, $classification = NULL, $room = NULL) {
  $form = array();

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

  $form_state['conditionally_required'] = array('#tree' => FALSE, '#type' => 'value');
  $form_state['conditionally_required'][0] = array();

  $form['form']['step'] = array(
    '#id' => 'field-form-step',
    '#type' => 'value',
    '#value' => &$step,
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
    $module_path = drupal_get_path('module', 'mcneese_event_workflow');
    require_once($module_path . '/includes/mcneese_event_workflow_event_structure.inc');

    $form['event'] = mcneese_event_workflow_get_event_fields($form, $form_state, $step);
    $form['event']['#id'] = 'section-event';
    $form['event']['#tree'] = TRUE;

    if ($step != 'category') {
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
 * Pre-Validation for mcneese_event_workflow_event_create_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_create_form()
 */
function mcneese_event_workflow_event_create_0_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }

  if (empty($form_state['values']['form']['step'])) {
    return;
  }

  if ($clicked_id == 'submit-event-prev') {
    if (!empty($form_state['conditionally_required'][0])) {
      foreach ($form_state['conditionally_required'][0] as $field) {
        if (is_string($field)) {
          $parts = explode('][', $field);

          $found = TRUE;
          $location = &$form['event'];
          while (!empty($parts)) {
            $part = array_shift($parts);
            if (!isset($location[$part])) {
              $found = FALSE;
              break;
            }

            $location = &$location[$part];
          }

          if ($found && $location['#required']) {
            if (isset($location['#options'])) {
              $location['#options'][''] = '';
            }
            $location['#required'] = FALSE;
          }
        }
      }
    }
  }
}

/**
 * Validation for mcneese_event_workflow_event_create_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_create_form()
 */
function mcneese_event_workflow_event_create_0_form_validate($form, &$form_state) {
  $no_problems = TRUE;
  $form_state['rebuild'] = TRUE;

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }

  if (empty($form_state['values']['form']['step'])) {
    form_set_error('form][step', "An internal error occured. Form Step is not defined. Please contact the System Administer.");
  }

  if ($no_problems) {
    if ($form_state['values']['form']['step'] == 'agreement') {
      if ($form_state['values']['agreement']['agree']) {
        $form_state['values']['form']['step'] = 'category';
      }
      else {
        form_set_error('agreement][agree', "You must agree to the terms to continue.");
      }
    }
    elseif ($form_state['values']['form']['step'] == 'category') {
      $form_state['values']['form']['step'] = 'rooms';
    }
    elseif ($form_state['values']['form']['step'] == 'rooms') {
      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'category';
      }
      else {
        $form_state['values']['form']['step'] = 'standard';
      }
    }
    elseif ($form_state['values']['form']['step'] == 'standard') {
      // when classification is readonly, ensure that no change happens.
      if ($form_state['values']['event']['coordinator']['classification']) {
        if (isset($form['event']['coordinator']['classification']['#value'])) {
          $form_state['values']['event']['coordinator']['classification'] = $form['event']['coordinator']['classification']['#value'];
        }
      }

      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'rooms';
      }
      else {
        if (empty($form_state['values']['event']['information']['type'])) {
          form_set_error('event][information][type', "The <strong>" . $form['event']['information']['type']['#title'] . "</strong> is required.");
        }
        else {
          $type_term = taxonomy_term_load($form_state['values']['event']['information']['type']);

          if (is_object($type_term)) {
            $form_state['values']['form']['step'] = 'details';
          }
          else {
            form_set_error('event][information][type', "An invalid <strong>" . $form['event']['information']['type']['#title'] . "</strong> is has been selected.");
          }
        }
      }
    }
    elseif ($form_state['values']['form']['step'] == 'details') {
      if ($clicked_id == 'submit-event-prev') {
        $form_state['values']['form']['step'] = 'standard';
      }
      elseif ($clicked_id == 'submit-event-submit') {
        // if there is any validation that must be manually performed prior to submit, do it now.
        #form_set_error('event][submit', "TODO: the submit functionality is not yet implemented.");
        $form_state['rebuild'] = FALSE;

      }
    }
  }
}

/**
 * Submit process for mcneese_event_workflow_event_create_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_create_form()
 */
function mcneese_event_workflow_event_create_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $mew_event_requests = array();
  $mew_event_requests['user_id'] = $user->uid;
  $mew_event_requests['banner_id'] = NULL;
  $mew_event_requests['created'] = REQUEST_TIME;
  $mew_event_requests['updated'] = REQUEST_TIME;
  $mew_event_requests['status'] = MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_NORMAL;
  $mew_event_requests['state'] = 0;

  if (!empty($user->field_user_banner_id['und'][0]['value'])) {
    $mew_event_requests['banner_id'] = (int) $user->field_user_banner_id['und'][0]['value'];
  }

  $mew_event_states = array();
  $mew_event_states['user_id'] = $mew_event_requests['user_id'];
  $mew_event_states['banner_id'] = $mew_event_requests['banner_id'];
  $mew_event_states['date'] = $mew_event_requests['created'];
  $mew_event_states['revision'] = 0;
  $mew_event_states['delta'] = 0;
  $mew_event_states['state'] = 0;

  $workflow_states = event_workflow_get_workflow_states_tree();
  foreach ($workflow_states as $state_id => &$state_name) {
    if ($state_name == 'new') {
      $mew_event_states['state'] = $state_id;
      break;
    }
  }

  $structure = mcneese_event_workflow_secondary_table_structure();
  $structure_special = mcneese_event_workflow_secondary_table_structure(1);
  $structure_storage = mcneese_event_workflow_secondary_table_structure(2);
  $structure_tables = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    $name = 'mew_current_' . $table_name;
    $structure_tables[$name] = array();

    foreach ($fields as $field_name => &$field_values) {
      $structure_tables[$name][$field_name] = 0;

      // store the values for each individual mew_field_* table.
      $field_table_name = 'mew_field_' . $table_name . '_' . $field_name;

      $structure_field_tables[$field_table_name] = array();
      $structure_field_tables[$field_table_name]['user_id'] = $mew_event_requests['user_id'];
      $structure_field_tables[$field_table_name]['banner_id'] = $mew_event_requests['banner_id'];
      $structure_field_tables[$field_table_name]['date'] = $mew_event_requests['created'];
      $structure_field_tables[$field_table_name]['revision'] = 0;

      $current = &$form_state['values'];
      $parts = explode('][', $field_values);
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
            // @todo: handle error case when $part is not defined.
            unset($current);
            $current = NULL;
            break;
          }
        }
      }

      if (isset($structure_special[$table_name][$field_name])) {
        foreach ($structure_special[$table_name][$field_name] as $special_field_id => &$special_field_name) {
          if (isset($current[$special_field_id]) && $current[$special_field_id] > 0) {
            $structure_field_tables[$field_table_name][$special_field_name] = $special_field_id;
          }
        }
      }
      elseif (!is_null($current)) {
        if (empty($structure_storage[$table_name][$field_name])) {
          $structure_field_tables[$field_table_name]['value'] = $current;
        }
        else{
          // room list is stored in multiple separate fields in the database.
          if ($field_name == 'room_list') {
            unset($structure_tables[$name][$field_name]);
            unset($structure_field_tables[$field_table_name]);

            $structure_tables[$name]['building'] = 0;
            $structure_tables[$name]['room'] = 0;

            $room_list_parts = explode('_', $current, 3);

            $field_table_name = 'mew_field_' . $table_name . '_building';

            $structure_field_tables[$field_table_name] = array();
            $structure_field_tables[$field_table_name]['user_id'] = $mew_event_requests['user_id'];
            $structure_field_tables[$field_table_name]['banner_id'] = $mew_event_requests['banner_id'];
            $structure_field_tables[$field_table_name]['date'] = $mew_event_requests['created'];
            $structure_field_tables[$field_table_name]['revision'] = 0;
            $structure_field_tables[$field_table_name]['value'] = $room_list_parts[1];

            $field_table_name = 'mew_field_' . $table_name . '_room';

            $structure_field_tables[$field_table_name] = array();
            $structure_field_tables[$field_table_name]['user_id'] = $mew_event_requests['user_id'];
            $structure_field_tables[$field_table_name]['banner_id'] = $mew_event_requests['banner_id'];
            $structure_field_tables[$field_table_name]['date'] = $mew_event_requests['created'];
            $structure_field_tables[$field_table_name]['revision'] = 0;
            $structure_field_tables[$field_table_name]['value'] = $room_list_parts[2];
          }
          else {
            $storage_names = array_keys($structure_storage[$table_name][$field_name]);
            $storage_name = reset($storage_names);

            $storage_value = event_workflow_convert_value_to_database_format($current, $storage_name, $structure_storage[$table_name][$field_name][$storage_name]);

            if ($storage_value !== FALSE) {
              $structure_field_tables[$field_table_name]['value'] = $storage_value;
            }

            unset($storage_value);
            unset($storage_name);
            unset($storage_names);
          }
        }
      }
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

    $mew_event_states['event_id'] = $event_id;

    $query = db_insert('mew_event_states');
    $query->fields($mew_event_states);
    $query->execute();

    foreach ($structure_field_tables as $table_name => &$fields) {
      $fields['event_id'] = $event_id;

      $query = db_insert($table_name);
      $query->fields($fields);
      $query->execute();
    }

    foreach ($structure_tables as $table_name => &$fields) {
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
