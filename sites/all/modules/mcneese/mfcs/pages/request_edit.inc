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

  // force the page to load the view page when only the id is specified.
  if (is_null($target_section)) {
    drupal_goto('requests/view-0/' . $request_id, array('query' => drupal_get_query_parameters()));
    return '';
  }

  if (!is_null($target_section) && !is_string($target_section)) {
    cf_error::invalid_string('target_section');
    drupal_not_found();
    drupal_exit();
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, 'Unable to find request with id of @id.', array('@id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  mfcs_include(MFCS_INCLUDE_STRUCTURE);

  $section_information = mfcs_get_request_section_information();

  if (empty($section_information[$target_section]['steps'])) {
    watchdog(MFCS_WATCHDOG_ID, 'The section @target_section does not exist or has no steps.', array('@target_section' => $target_section), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  if (is_null($target_section)) {
    mfcs_add_canonical_headers('requests/edit-0/' . $request_id);
  }
  else {
    mfcs_add_canonical_headers('requests/edit-0/' . $request_id . '/' . $target_section);
  }

  $page_title = 'Edit Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Edit Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

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

  $form['form']['operation'] = array(
    '#type' => 'value',
    '#value' => 'edit',
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
        '#default_value' => 'Cancel',
      );
    }
    else {
      $form['request']['prev'] = array(
        '#id' => 'submit-request-prev',
        '#name' => 'submit-request-prev',
        '#type' => 'submit',
        '#default_value' => 'Back',
      );
    }

    if ($step == 'standard') {
      $form['request']['next'] = array(
        '#id' => 'submit-request-next',
        '#name' => 'submit-request-next',
        '#type' => 'submit',
        '#default_value' => 'Continue',
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
        '#default_value' => 'Submit',
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
        '#default_value' => 'Submit',
      );
    }
    else {
      $form['request']['next'] = array(
        '#id' => 'submit-request-next',
        '#name' => 'submit-request-next',
        '#type' => 'submit',
        '#default_value' => 'Continue',
      );
    }
  }
  else {
    $form['request']['cancel'] = array(
      '#id' => 'submit-request-cancel',
      '#name' => 'submit-request-cancel',
      '#type' => 'submit',
      '#default_value' => 'Cancel',
    );

    $form['request']['submit'] = array(
      '#id' => 'submit-request-submit',
      '#name' => 'submit-request-submit',
      '#type' => 'submit',
      '#default_value' => 'Submit',
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

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $request_type_options = mfcs_get_request_type_list_options();

  $request = $form['form']['request']['#value'];
  $request_id = (int) $request['mer']['id'][0]->value;

  $user = NULL;
  if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
    $user = $form['form']['user']['#value'];
  }

  $can_edit_request = mfcs_request_access('edit', $request, $user);

  if (!$can_edit_request) {
    form_set_error('', 'Illegal operation detected. Unauthorized access attempt.');
    $no_problems = FALSE;
  }

  if ($clicked_id == 'submit-request-cancel') {
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

    return;
  }

  if (empty($form_state['values']['form']['step'])) {
    form_set_error('form][step', 'An internal error occured. Form Step is not defined. Please contact the support staff.');
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
          $room = (int) $room[0];
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

              $v = mfcs_convert_value_to_database_format($time_start[$key], 'timestamp', array('view' => MFCS_DATE_FORMAT_TIME, 'relative_to' => 'request][dates][date'), $form_state, $key);
              if ($v === FALSE) {
                $time_start[$key] = NULL;
                $time_stop[$key] = NULL;
                $category[$key] = NULL;
              }
              else {
                $time_start[$key] = $v;

                $v = mfcs_convert_value_to_database_format($time_stop[$key], 'timestamp', array('view' => MFCS_DATE_FORMAT_TIME, 'relative_to' => 'request][dates][date'), $form_state, $key);
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

          if (empty($form_state['values']['request']['information']['type'][0])) {
            form_set_error('request][information][type][0', 'The <strong>' . $form['request']['information']['type'][0]['#title'] . '</strong> is required.');
          }
          else {
            if (array_key_exists($form_state['values']['request']['information']['type'][0], $request_type_options)) {
              $form_state['values']['form']['step'] = 'details';
            }
            else {
              form_set_error('request][information][type][0', 'An invalid <strong>' . $form['request']['information']['type'][0]['#title'] . '</strong> is selected.');
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

  mfcs_include(MFCS_INCLUDE_STRUCTURE);
  mfcs_include(MFCS_INCLUDE_TABLE);

  $user = cf_current_user();
  $instance = mfcs_instance();

  $changes = array(
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

  $prepared = mfcs_prepare_request_values($request, $changes);
  if ($prepared === FALSE) {
    form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }

  $structure = mfcs_table_structure();
  $success = mfcs_set_request_values($request, $prepared, $structure, $form['form']['existing']['#value'], $form_state, TRUE);
  if ($success === FALSE) {
    form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }
  elseif ($success === TRUE) {
    // forcefully clear the request cache so that it can reflect the changes.
    mfcs_load_request_by_id($request_id, FALSE);

    mfcs_include(MFCS_INCLUDE_WORKFLOW);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['status'] = $status;
    $parameters['step'] = $step;


    // when switching the type to a quick meeting, do not send out update e-mails.
    $to_quick_meeting = NULL;
    if (isset($form_state['values']['request']['information']['type'][0])) {
      $to_quick_meeting = $form_state['values']['request']['information']['type'][0];
      if (is_string($to_quick_meeting)) {
        $to_quick_meeting = (int) $to_quick_meeting;
      }
    }

    if ($status === MFCS_REQUEST_STATUS_LOCKED || $status === MFCS_REQUEST_STATUS_CLOSED_ACCEPTED || $status === MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED) {
      $loaded_request = mfcs_load_request_by_id($request_id);

      $revision_id = NULL;
      if (isset($loaded_request['mer']['revision'][0]->value) && is_numeric($loaded_request['mer']['revision'][0]->value)) {
        $revision_id = (int) $loaded_request['mer']['revision'][0]->value;
      }

      // do not send e-mail when the previous step is already set to venue available.
      if ($step !== MFCS_REVIEW_STEP_VENUE_AVAILABLE && !is_null($revision_id)) {
        $parameters['amendment'] = TRUE;
        $parameters['message'] = mfcs_request_edit_0_form_build_revision_history_message($request_id, $revision_id);
        $parameters['restriction'] = MFCS_REVIEW_RESTRICTIONS_COORDINATOR;

        if (is_bool($parameters['message'])) {
          unset($parameters['message']);
        }

        mfcs_send_workflow_emails($request_id, $parameters);
      }
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
 * Builds the revision history message for use in amendment messages.
 *
 * This does not build HTML, instead it builds plain text.
 *
 * @param int $request_id
 *   The request ID.
 * @param int $revision_id
 *   The request's revision ID.
 *
 * @return bool|string
 *   A string containing a message about the changes on success.
 *   FALSE otherwise.
 */
function mfcs_request_edit_0_form_build_revision_history_message($request_id, $revision_id) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  if (!cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_TABLE);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_VIEW);

  $structure = mfcs_table_structure();
  $presentation = mfcs_table_presentation();
  $presentation_tables = mfcs_table_presentation(1);

  // The venue coordinator user id is a 'split' structure and does not normally get displayed.
  // Manually change the behavior such that the Venue Coordinator Name will be properly presented.
  $presentation['venue_coordinator']['user_id']['group'] = 'coordinator';
  $presentation_tables['coordinator']['label'] = 'Venue Coordinator';

  try {
    $query = db_select('mfcs_request_revisions', 'mrv');

    $query->addField('mrv', 'revision', 'revision');
    $query->addField('mrv', 'user_id', 'user_id');
    $query->addField('mrv', 'date', 'date');
    $query->condition('mrv.request_id', $request_id);
    $query->condition('mrv.revision', $revision_id);

    $revision = $query->execute()->fetchObject();

    if (is_object($revision)) {
      $revision_date = (int) $revision->date;
      $revision_user_id = (int) $revision->user_id;
    }
    unset($revision);

    $history = mfcs_load_request_by_timestamp($request_id, $revision_date, $revision_user_id);

    $processed_values = array();
    foreach ($history as $group_name => $group_values) {
      if ($group_name == 'current') {
        continue;
      }

      if ($group_name == 'mer') {
        continue;
      }

      if (empty($group_values)) {
        continue;
      }

      foreach ($group_values as $field_name => $field_values) {
        if (empty($field_values)) {
          continue;
        }

        $manual_processing = FALSE;
        if (isset($structure[$group_name][$field_name]) && array_key_exists('manual_processing', $structure[$group_name][$field_name])) {
          $manual_processing = $structure[$group_name][$field_name]['manual_processing'];
        }

        if ($manual_processing) {
          continue;
        }

        foreach ($field_values as $key => $value) {
          if (!is_object($value)) {
            continue;
          }

          if (!isset($processed_values[$value->user_id]['fields'][$group_name])) {
            $processed_values[$value->user_id]['fields'][$group_name] = array(
              $field_name => array(),
            );
          }

          if (!isset($processed_values[$value->user_id]['fields'][$group_name][$field_name])) {
            $processed_values[$value->user_id]['fields'][$group_name][$field_name] = array();
          }

          if (isset($presentation[$group_name][$field_name])) {
            // always display the boolean when displayed in the history log.
            if (!empty($presentation[$group_name][$field_name]['type'])) {
              foreach ($presentation[$group_name][$field_name]['type'] as $type_key => $type_value) {
                if ($type_value == 'boolean') {
                  if (!empty($presentation[$group_name][$field_name]['display'][$type_key])) {
                    $display_value = &$presentation[$group_name][$field_name]['display'][$type_key];

                    if ($display_value == 'when_true' || $display_value == 'when_false') {
                      $display_value = 'raw';
                    }
                  }
                }
                elseif ($type_value == 'insurance') {
                  if (isset($presentation[$group_name][$field_name])) {
                    $presentation[$group_name][$field_name]['display'][$type_key] = 'always';
                  }
                }
              }
            }

            $options = array(
              'no_prefix' => TRUE,
              'no_suffix' => TRUE,
              'print_delta_for_multiple' => TRUE,
              'history' => TRUE,
              'combine_partial' => TRUE,
              'combine_partial_separator' => FALSE,
            );

            $value_array = mfcs_view_build_request_value_structure($request_id, $group_name, $field_name, $key, $value, $presentation[$group_name][$field_name], $history, $options);
            if (is_array($value_array) && !isset($value_array['no_values'])) {
              $processed_values[$value->user_id]['fields'][$group_name][$field_name][$key] = $value_array;
            }
          }
        }
      }
    }

    if (empty($processed_values)) {
      return '';
    }

    $message = '';
    foreach ($processed_values as $user_id => $processed_value) {
      foreach ($processed_value['fields'] as $group_name => $group_values) {
        foreach ($group_values as $field_name => $field_values) {
          $presentation_group = $group_name;
          if (!empty($presentation[$group_name][$field_name]['group'])) {
            $presentation_group = $presentation[$group_name][$field_name]['group'];
          }

          $group_label = '';
          if (array_key_exists($presentation_group, $presentation_tables)) {
            $group_label = $presentation_tables[$presentation_group]['label'];
          }

          $field_label = $field_name;
          if (!empty($presentation[$group_name][$field_name]['label'])) {
            $field_label = $presentation[$group_name][$field_name]['label'];
          }

          if (!empty($field_values)) {
            if (count($field_values) > 1 || !array_key_exists(0, $field_values)) {
              $message .= '(' . $group_label . ') ' . $field_label . ' has been set to:' . "\n";
              foreach ($field_values as $field_key => $field_value) {
                if (isset($field_value['value']['markup']['#markup'])) {
                  $message .= ' ' . strip_tags($field_value['value']['markup']['#markup']) . "\n";
                }
                else {
                  $message .= ' [' . $field_key . '] has been altered.' . "\n";
                }
              }
            }
            else {
              $message .= '(' . $group_label . ') ' . $field_label . ' has been set to: ' . strip_tags($field_values[0]['value']['markup']['#markup']) . ".\n";
            }
          }
        }
      }
    }
  }
  catch (Error $e) {
    cf_error::on_query_execution($e);
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);
  }

  return $message;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
