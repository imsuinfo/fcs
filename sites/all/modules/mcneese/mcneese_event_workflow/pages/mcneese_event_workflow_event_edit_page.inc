<?php

/**
 * @file
 * Defines McNeese event workflow event edit page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Wrapper to mcneese_event_workflow_event_edit_0_form() for ctools.
 *
 * @param int $id
 *   The unique identifier for an event request.
 * @param string|null $target_section
 *   Load only the specified section.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see mcneese_event_workflow_event_edit_0_form()
 * @see ctools_include()
 * @see ctools_add_js()
 * @see ctools_build_form()
 */
function mcneese_event_workflow_event_edit_0_form_ctools($id, $target_section) {
  if (!function_exists('ctools_api_version')) {
    drupal_not_found();
    drupal_exit();
    return '';
  }

  $js = !empty($_REQUEST['js']);

  $commands = array();

  if ($js) {
    ctools_include('modal');
    ctools_add_js('ajax-responder');
  }

  ctools_include('wizard');

  if (!cf_is_integer($id)) {
    $commands[] = ajax_command_html('#page_not_found', "Page Not Found");
    $commands[] = ctools_modal_command_dismiss();
    print(ajax_render($commands));
    exit();
  }

  if (!is_string($target_section)) {
    $commands[] = ajax_command_html('#page_not_found', "Page Not Found");
    $commands[] = ctools_modal_command_dismiss();
    print(ajax_render($commands));
    exit();
  }

  $form_state = array();
  $form_state['ajax'] = $js;
  $form_state['mcneese_event_workflow_event_edit_0_form_ctools'] = TRUE;
  $form_state['no_redirect'] = TRUE;
  $form_state['re_render'] = TRUE;
  $form_state['build_info'] = array(
    'args' => array($id, $target_section),
  );

  $form = drupal_build_form('mcneese_event_workflow_event_edit_0_form', $form_state);
  $output = drupal_render($form);

  if ($output === FALSE) {
    $commands[] = ctools_modal_command_dismiss();
  }
  elseif (array_key_exists('ajax_cancel', $form_state) && $form_state['ajax_cancel']) {
    $commands[] = ctools_modal_command_dismiss();
  }
  else {
    $commands = ctools_modal_form_render($form_state, $output);
  }

  if ($js) {
    print(ajax_render($commands));
    exit();
  }

  return $output;
}

/**
 * Provides the event workflow event edit page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $id
 *   The unique identifier for an event request.
 * @param string|null $target_section
 *   (optional) Load only the specified section.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see ctools_include()
 */
function mcneese_event_workflow_event_edit_0_form($form, &$form_state, $id, $target_section = NULL) {
  // force the page to load the view page when only the id is specified.
  if (is_null($target_section)) {
    drupal_goto('events/view-0/' . $id, array('query' => drupal_get_query_parameters()));
    return '';
  }

  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');

    if (array_key_exists('mcneese_event_workflow_event_edit_0_form_ctools', $form_state)) {
      $commands[] = ctools_modal_command_dismiss();
      print(ajax_render($commands));
      exit();
    }
    else {
      drupal_not_found();
      drupal_exit();
    }

    return array();
  }

  $event = event_workflow_load_event_by_id($id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @id.", array('@id' => $id), WATCHDOG_ERROR);

    if (array_key_exists('mcneese_event_workflow_event_edit_0_form_ctools', $form_state)) {
      $commands[] = ctools_modal_command_dismiss();
      print(ajax_render($commands));
      exit();
    }
    else {
      drupal_not_found();
      drupal_exit();
    }
  }

  mcneese_event_workflow_include(1);

  $section_information = mcneese_event_workflow_get_event_section_information();

  if (empty($section_information[$target_section]['steps'])) {
    watchdog('mew', "The section @target_section does not exist or has no steps.", array('@target_section' => $target_section), WATCHDOG_ERROR);

    if (array_key_exists('mcneese_event_workflow_event_edit_0_form_ctools', $form_state)) {
      $commands[] = ctools_modal_command_dismiss();
      print(ajax_render($commands));
      exit();
    }
    else {
      drupal_not_found();
      drupal_exit();
    }
  }

  $form = array();

  if (empty($form_state['values']['form']['step'])) {
    $step = reset($section_information[$target_section]['steps']);
  }
  else {
    $step = $form_state['values']['form']['step'];
  }

  if ($step == 'cancel') {
    if (array_key_exists('mcneese_event_workflow_event_edit_0_form_ctools', $form_state)) {
      $commands = array();
      $commands[] = ctools_modal_command_dismiss();
      print(ajax_render($commands));
      exit();
    }

    // @todo: perform a normal redirect to some previous page up the path tree.
    return array();
  }


  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['sections'] = array(
    '#type' => 'value',
    '#value' => mcneese_event_workflow_get_event_section_information(),
  );

  $form['form']['step'] = array(
    '#id' => 'field-form-step',
    '#type' => 'value',
    '#value' => $step,
  );

  $form['form']['existing'] = array(
    '#id' => 'field-form-existing',
    '#type' => 'value',
    '#value' => event_workflow_get_event_existing_values($event),
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => NULL,
  );

  if (isset($event['mer']['user_id'][0]->value)) {
    $form['form']['user']['#value'] = user_load($event['mer']['user_id'][0]->value);
  }

  if (!is_object($form['form']['user']['#value'])) {
    $form['form']['user']['#value'] = cf_current_user();
  }

  $form['form']['parents'] = array(
    '#id' => 'field-form-parents',
    '#type' => 'value',
    '#value' => array(
      'structure' => array('event'),
      'path' => 'event[',
    ),
  );

  $form['form']['event'] = array(
    '#id' => 'field-form-event',
    '#type' => 'value',
    '#value' => $event,
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

  $form['event'] = array(
    '#tree' => TRUE,
  );

  $form['event'] = mcneese_event_workflow_get_event_fields($form, $form_state);
  $form['event']['#id'] = 'section-event';
  $form['event']['#tree'] = TRUE;

  if ($target_section == 'request') {
    if ($step == 'location') {
      $form['event']['cancel'] = array(
        '#id' => 'submit-event-cancel',
        '#name' => 'submit-event-cancel',
        '#type' => 'submit',
        '#default_value' => t("Cancel"),
      );
    }
    else {
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
  else {
    $form['event']['cancel'] = array(
      '#id' => 'submit-event-cancel',
      '#name' => 'submit-event-cancel',
      '#type' => 'submit',
      '#default_value' => t("Cancel"),
    );

    $form['event']['submit'] = array(
      '#id' => 'submit-event-submit',
      '#name' => 'submit-event-submit',
      '#type' => 'submit',
      '#default_value' => t("Submit"),
    );
  }

  return $form;
}

/**
 * Pre-Validation for mcneese_event_workflow_event_edit_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_edit_0_form()
 */
function mcneese_event_workflow_event_edit_0_form_pre_validate(&$form, &$form_state) {
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
    'submit-event-cancel' => TRUE,
    'submit-event-prev' => TRUE,
    'submit-event-add-date' => TRUE,
    'submit-event-remove-date' => TRUE,
  );

  if ($clicked_sub_id == 'submit-event-remove-date' && is_numeric($clicked_id_part)) {
    $dont_validate[$clicked_id] = TRUE;
  }

  mcneese_event_workflow_include(2);

  mcneese_event_workflow_pre_validate_event_fields($form, $form_state, $dont_validate);
}

/**
 * Validation for mcneese_event_workflow_event_edit_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_edit_0_form()
 */
function mcneese_event_workflow_event_edit_0_form_validate($form, &$form_state) {
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

  $event = $form['form']['event']['#value'];
  $event_id = (int) $event['mer']['id'][0]->value;

  $can_edit_event = mcneese_event_workflow_management_page_event_access('edit', $event_id);
  $can_cancel_event = mcneese_event_workflow_management_page_event_access('cancel', $event_id);

  if (!$can_edit_event) {
    form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
    $no_problems = FALSE;
  }

  if ($clicked_id == 'submit-event-cancel') {
    if (!$can_cancel_event) {
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

    return;
  }

  if (empty($form_state['values']['form']['step'])) {
    form_set_error('form][step', "An internal error occured. Form Step is not defined. Please contact the support staff.");
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
    mcneese_event_workflow_include(2);

    $no_problems = mcneese_event_workflow_validate_event_fields($form, $form_state);
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
          $form_state['values']['event']['group_dates']['dates']['date'][$next_date] = NULL;
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
          $category = $form_state['values']['event']['dates']['category'];

          foreach ($date as $key => $d) {
            $v = event_workflow_convert_value_to_database_format($d, 'date_popup', array('view' => 'Y/m/d'), $form_state, $key);

            if ($v === FALSE) {
              unset($date[$key]);
              unset($time_start[$key]);
              unset($time_stop[$key]);
              unset($category[$key]);
            }
            else {
              $date[$key] = $v;

              $v = event_workflow_convert_value_to_database_format($time_start[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'event][dates][date'), $form_state, $key);
              if ($v === FALSE) {
                $time_start[$key] = NULL;
                $time_stop[$key] = NULL;
                $category[$key] = NULL;
              }
              else {
                $time_start[$key] = $v;

                $v = event_workflow_convert_value_to_database_format($time_stop[$key], 'timestamp', array('view' => 'h:ia', 'relative_to' => 'event][dates][date'), $form_state, $key);
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
            $events_using_room = mcneese_event_workflow_check_room_availability($room[0], $date, $time_start, $time_stop, $event_id);

            if ($events_using_room === FALSE) {
              form_set_error('', "An internal error occurred while checking the room for availability. Please contact the support staff.");
            }
            elseif (!empty($events_using_room)) {
              mcneese_event_workflow_display_room_unavailable_message();
            }
          }

          unset($room);
          unset($date);
          unset($time_start);
          unset($time_stop);
          unset($category);

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
    else {
      if ($clicked_id == 'submit-event-submit') {
        $form_state['rebuild'] = FALSE;
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
    }
  }
}

/**
 * Submit process for mcneese_event_workflow_event_edit_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_edit_0_form()
 */
function mcneese_event_workflow_event_edit_0_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $failure = FALSE;
  $event = &$form['form']['event']['#value'];
  $event_id = (int) $event['mer']['id'][0]->value;

  // make sure the cancel button is never treated as a save submit.
  if ($clicked_id == 'submit-event-cancel') {
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

    return;
  }

  $user = cf_current_user();

  $user_data = array(
    'user_id' => $user->uid,
  );

  $mew_event_requests = array(
    'updated' => REQUEST_TIME,
  );

  $event_revision = mcneese_event_workflow_load_event_revision_number($event_id);
  $processed_mew_event_revisions = &drupal_static('processed_mew_event_revisions_' . REQUEST_TIME, FALSE);

  if ($event_revision === FALSE) {
    form_set_error('form', "An error occurred while trying to save the event request. Please contact the support staff.");
    watchdog('mew', "Failed to load revision number for event %event_id while trying to save the event request.", array('%event_id' => $event_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }
  elseif (is_null($event_revision)) {
    $event_revision_next = 0;
  }
  else {
    $event_revision_next = $event_revision + 1;
  }

  $mew_event_revisions = array(
    'event_id' => $event_id,
    'date' => REQUEST_TIME,
    'revision' => $event_revision_next,
  );

  mcneese_event_workflow_include(0);

  $structure = mcneese_event_workflow_table_structure();
  $structure_tables = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    $db_table = 'mew_current_' . $table_name;

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
          watchdog('mew', "Failed to process '@part' for '@field_name' in '@table_name' during form submit. Invalid array or missing array key (existing).", array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
          continue 2;
        }

        if (!is_array($current_new) || !array_key_exists($part, $current_new)) {
          watchdog('mew', "Failed to process '@part' for '@field_name' in '@table_name' during form submit. Invalid array or missing array key (new).", array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
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

      if (!array_key_exists($db_table, $structure_tables)) {
        $structure_tables[$db_table] = array();
      }

      // @todo: add support for using multiple values instead of using only delta = 0.
      $delta = 0;

      $db_table_field = 'mew_field_' . $table_name . '_' . $field_name;

      mcneese_event_workflow_build_table_structure($structure, $structure_tables, $structure_field_tables, $table_name, $field_name, $db_table, $db_table_field, $user_data, $current_new, $form_state, $event);
    }
  }

  if (!empty($structure_field_tables)) {
    $failure = FALSE;
    $transaction = db_transaction();

    try {
      // now perform the operations.
      $query = db_update('mew_event_requests');
      $query->condition('id', $event_id);
      $query->fields($mew_event_requests);
      $query->execute();

      if (!$processed_mew_event_revisions) {
        $query = db_insert('mew_event_revisions');
        $query->fields($mew_event_revisions);
        $query->execute();

        $processed_mew_event_revisions = TRUE;
      }

      foreach ($structure_field_tables as $table_name => $fields) {
        foreach ($fields as $delta => $values) {
          $query = db_insert($table_name);
          $query->fields($values);
          $query->execute();
        }
      }

      foreach ($structure_tables as $table_name => $fields) {
        $query = db_update($table_name);
        $query->condition('event_id', $event_id);
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
      watchdog('mew', "An error occured while trying to save the event request: @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
    else {
      // forcefully clear the event cache so that it can reflect the changes.
      event_workflow_load_event_by_id($event_id, TRUE);

      mcneese_event_workflow_include(6);

      $parameters = array();
      $parameters['changed_by'] = $user->uid;
      $parameters['changed_type'] = 'user';
      $parameters['update'] = TRUE;
      mcneese_event_workflow_send_workflow_emails($event_id, $parameters);
    }
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
