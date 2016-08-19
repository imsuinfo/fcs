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

  $page_title = "Edit Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Edit Request: " . $request['information']['title'][0]->value;
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

  if (!$can_edit_request) {
    form_set_error('', "Illegal operation detected. Unauthorized access attempt.");
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

          if (!empty($date)) {
            $room_unavailability = mfcs_check_room_availability($room, $date, $time_start, $time_stop, $request_id);
            if ($room_unavailability === FALSE) {
              form_set_error('', "An internal error occurred while checking the room for availability. Please contact the support staff.");
            }
            elseif (!empty($room_unavailability['conflict']) || !empty($room_unavailability['unavailable'])) {
              $unavailable_message = '<span class="uppercase">Warning</span>: The specified room is currently unavailable for the following dates: ';
              $unavailable_message .= mfcs_build_room_unavailable_message($room_unavailability);
              drupal_set_message($unavailable_message, 'warning');
              unset($unavailable_message);
            }
          }
          unset($room);
          unset($date);
          unset($time_start);
          unset($time_stop);
          unset($category);
          unset($room_unavailability);

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
    form_set_error('form', "An error occurred while trying to save the request. Please contact the support staff.");

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

    if ($status === MFCS_REQUEST_STATUS_LOCKED) {
      // do not send e-mail when the previous step is already set to venue available.
      if ($step !== MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
        $parameters['amendment'] = TRUE;
        $parameters['restriction'] = MFCS_REVIEW_RESTRICTIONS_NONE;
        mfcs_send_workflow_emails($request_id, $parameters);
      }
    }
    elseif ($status === MFCS_REQUEST_STATUS_UNLOCKED) {
      // send emails when the request has been updated.
      $parameters['update'] = TRUE;
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
