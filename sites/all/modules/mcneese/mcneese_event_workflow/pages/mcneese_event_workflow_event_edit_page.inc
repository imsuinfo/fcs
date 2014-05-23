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
  if (!cf_is_integer($id)) {
    watchdog('mew', "The ID @id is not a valid id.", array('@id' => $id), WATCHDOG_ERROR);

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

  $module_path = drupal_get_path('module', 'mcneese_event_workflow');
  require_once($module_path . '/includes/mcneese_event_workflow_event_structure.inc');

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

  // @todo: load and populate event information into the form_state
  $classification = '';
  $room = '';

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
    '#value' => (array) event_workflow_get_event_existing_values($event),
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
  if ($target_section == 'information') {
    // in particular, the 'information' is used to load the entire form, such as during an edit process.
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

  if ($target_section == 'information') {
    if ($step == 'location') {
      $form['event']['cancel'] = array(
        '#id' => 'submit-event-cancel',
        '#type' => 'submit',
        '#value' => t("Cancel"),
      );
    }
    else {
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
  else {
    $form['event']['cancel'] = array(
      '#id' => 'submit-event-cancel',
      '#type' => 'submit',
      '#value' => t("Cancel"),
    );

    $form['event']['submit'] = array(
      '#id' => 'submit-event-submit',
      '#type' => 'submit',
      '#value' => t("Submit"),
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
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }

  if (empty($form_state['values']['form']['step'])) {
    return;
  }

  if ($clicked_id == 'submit-event-cancel' || $clicked_id == 'submit-event-prev') {
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

          if ($found && array_key_exists('#required', $location)) {
            if (isset($location['#options'])) {
              $location['#options'][''] = '';
            }
            $location['#required'] = FALSE;
          }
        }
      }
    }

    // @todo: handle conditionally_required[1] fields, which are required only when their dependant fields make them visible based on the '#state' field settings.
  }
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
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }

  if (empty($form_state['values']['form']['step'])) {
    return;
  }

  if ($clicked_id == 'submit-event-cancel') {
    if (isset($form_state['ajax'])) {
      // ajax method of cancelling
      $form_state['values']['form']['step'] = 'cancel';
    }
    else {
      // non-ajax method of cancelling
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
    form_set_error('form][step', "An internal error occured. Form Step is not defined. Please contact the System Administer.");
  }

  // perform validation when not going backwards.
  if ($clicked_id != 'submit-event-prev') {
    $module_path = drupal_get_path('module', 'mcneese_event_workflow');
    require_once($module_path . '/includes/mcneese_event_workflow_event_validation.inc');

    $no_problems = mcneese_event_workflow_validate_event_fields($form, $form_state);
    if (is_null($no_problems)) {
      $no_problems = TRUE;
    }
  }

  if ($no_problems) {
    if ($form['form']['target_section']['#value'] == 'information') {
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
          $form_state['rebuild'] = FALSE;
        }
      }
    }
    else {
      if ($clicked_id == 'submit-event-submit') {
        $form_state['rebuild'] = FALSE;
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
  $user = cf_current_user();

  $user_data = array();
  $user_data['user_id'] = (int) $user->uid;
  $user_data['banner_id'] = NULL;

  if (!empty($user->field_user_banner_id['und'][0]['value'])) {
    $user_data['banner_id'] = (int) $user->field_user_banner_id['und'][0]['value'];
  }

  $failure = FALSE;
  $event = &$form['form']['event']['#value'];
  $event_id = (int) $event['mer']['id'][0]->value;

  $mew_event_requests = array();
  $mew_event_requests['updated'] = REQUEST_TIME;

  $structure = mcneese_event_workflow_secondary_table_structure();
  $structure_tables = array();
  $structure_field_tables = array();

  foreach ($structure as $table_name => &$fields) {
    $db_table = 'mew_current_' . $table_name;

    foreach ($fields as $field_name => &$field_values) {
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

      mcneese_event_workflow_build_table_structure($structure, $structure_tables, $structure_field_tables, $table_name, $field_name, $db_table, $db_table_field, $user_data, $current_new, $delta, $event);
    }
  }


  $failure = FALSE;
  $transaction = db_transaction();

  try {
    // now perform the operations.
    $query = db_update('mew_event_requests');
    $query->condition('id', (int) $event_id);
    $query->fields($mew_event_requests);
    $query->execute();

    if (!is_numeric($event_id) || $event_id <= 0) {
      // the event was not properly saved, present an error here.
      $transaction->rollback();
      return FALSE;
    }

    foreach ($structure_field_tables as $table_name => &$fields) {
      $query = db_insert($table_name);
      $query->fields($fields);
      $query->execute();
    }

    foreach ($structure_tables as $table_name => &$fields) {
      $query = db_update($table_name);
      $query->condition('event_id', $event_id);
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
