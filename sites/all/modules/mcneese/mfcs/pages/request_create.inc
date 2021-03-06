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
 * @param int $copy_request
 *   (optional) When specified, will copy an existing request in which the
 *   current user has appropriate access to view and edit.
 *
 * @return array $form
 *   An array containing the generated form.
 */
function mfcs_request_create_0_form($form, &$form_state, $copy_request = NULL) {
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
  if (!is_null($copy_request)) {
    if (!cf_is_integer($copy_request)) {
      cf_error::invalid_integer('copy_request');
      drupal_not_found();
      drupal_exit();
    }

    $request = mfcs_load_request_by_id($copy_request);

    $can_copy = mfcs_page_request_access('create', $copy_request);

    // if the request does not exist or the user cannot access the request to copy, fallback to a normal create.
    if (empty($request) || !$can_copy) {
      $request = NULL;
      $copy_request = NULL;
    }
  }

  if (is_null($copy_request)) {
    mfcs_add_canonical_headers('requests/create-0');
  }
  else {
    mfcs_add_canonical_headers('requests/create-0/' . $copy_request);
  }

  $page_title = 'Create Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Copy Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  mfcs_include(MFCS_INCLUDE_STRUCTURE);

  if (empty($form_state['values']['form']['step'])) {
    $step = 'agreement';
  }
  else {
    $step = $form_state['values']['form']['step'];
  }

  // cache the page for 70-hours.
  $form['#cache_expires'] = 252000;

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['agreement'] = array(
    '#tree' => TRUE,
  );

  $form['warnings'] = array(
    '#type' => 'markup',
    '#markup' => '',
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

  $form['form']['operation'] = array(
    '#type' => 'value',
    '#value' => 'create',
  );

  $form['form']['new'] = array(
    '#id' => 'field-form-new',
    '#type' => 'value',
    '#value' => TRUE,
  );

  $form['form']['copy'] = array(
    '#id' => 'field-form-copy',
    '#type' => 'value',
    '#value' => $copy_request,
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

  $form['form']['ignore_past'] = array(
    '#id' => 'field-form-ignore_past',
    '#type' => 'value',
    '#value' => FALSE,
  );

  if (isset($form_state['warnings']) && !empty($form_state['warnings'])) {
    $warnings_markup = NULL;
    foreach ($form_state['warnings'] as $warning_group => $warning) {
      if (empty($warning)) {
        continue;
      }

      $warnings_markup .= $warning;
    }
    unset($warning);

    if (!is_null($warnings_markup)) {
      $form['warnings'] .= $warnings_markup;
    }
    unset($warnings_markup);
  }

  if (is_array($request)) {
    $form['form']['existing']['#value'] = mfcs_get_request_existing_values($request);
    $request_fields = &$form['form']['existing']['#value']['request'];

    // Reset fields that should not be copied by the copy process.
    unset($request_fields['status']);
    unset($request_fields['step']);
    unset($request_fields['review']);
    unset($request_fields['requirements']);
    unset($request_fields['insurance']);
    unset($request_fields['request_coordinator']);
  }

  $statement_markup = 'McNeese State University is happy to provide facilities and resources to our Students, Employees, and the Community to further educational, cultural, and recreational initiatives through events held on campus.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the Facilities Use and Special Event Request form. The following questionnaire assists campus officials in not only scheduling campus facilities or maintaining a master calendar; it allows better service through inter-departmental communication and collaboration enhancing the excellence of McNeese.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'The questionnaire is designed according to the University\'s <a href="//www.mcneese.edu/node/3217">Facility Use Policy</a>. Any activity scheduled on behalf of the University, regardless of venue, must be submitted.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= '<strong>All requests should be made <a href="//www.mcneese.edu/node/3217#3_week_requirement" target="_blank">a minimum of three weeks in advance</a> of the event or of expected start date of publicity for the event, whichever comes first.</strong>' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'If you are only making a request for a <em>Table Reservation</em> in the <em>New Ranch</em> or in the <em>Quad</em>, then you should instead use the <a href="//www.mcneese.edu/studentunion/student_union_reservations">Table Request Form</a>.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'La. R.S. 14:133 (A) provides that "<em>Filing false public records is the filing or depositing for record in any public office or with any public official, or the maintaining as required by law, regulation, or rule, with knowledge of its falsity, any of the following: (1) Any forged document. (2) Any wrongfully altered document. (3) Any document containing a false statement or false representation of a material fact.</em>"' . '<br>';
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
      '#title' => 'I have read the above information and agree to the terms of the <a href="//www.mcneese.edu/node/3217">Facility Use Policy</a>.',
    );

    $form['agreement']['submit'] = array(
      '#id' => 'submit-agreement',
      '#name' => 'submit-agreement',
      '#type' => 'submit',
      '#default_value' => 'Continue',
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
        '#default_value' => 'Back',
      );
    }

    if ($step == 'standard') {
      $form['request']['next'] = array(
        '#id' => 'submit-request-next',
        '#name' => 'submit-request-next',
        '#type' => 'submit',
        '#default_value' => 'Continue',

        // disabled as per fcs-100.
        #'#states' => array(
        #  'invisible' => array(
        #    ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type][0]' . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
        #  ),
        #),
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

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $request_type_options = mfcs_get_request_type_list_options();

  if (empty($form_state['values']['form']['step'])) {
    form_set_error('form][step', 'An internal error occured. Form Step is not defined. Please contact the System Administrator.');
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
        // when copying a request, skip the location/building/room because their use should be expected.
        if (isset($form['form']['copy']['#value']) && is_numeric($form['form']['copy']['#value'])) {
          $form_state['values']['form']['step'] = 'standard';
        }
        else {
          $form_state['values']['form']['step'] = 'location';
        }
      }
      else {
        form_set_error('agreement][agree', 'You must agree to the terms to continue.');
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
        $next_date = mfcs_get_next_delta(array_keys($form_state['values']['request']['dates']['date']));
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
      elseif ($clicked_id == 'submit-request-submit' || $clicked_id == 'submit-request-next') {
        // @todo: fixme unavailability must be processed.
        if (isset($form_state['values']['request']['information']['type'][0]) && $form_state['values']['request']['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING && $clicked_id == 'submit-request-submit') {
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
                form_set_error('request][information][type][0', 'You are not authorized to create a quick meeting.');
              }
              else {
                $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
                $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
                $form_state['rebuild'] = FALSE;
              }
            }
          }
          else {
            form_set_error('request][information][type][0', 'An internal error occured while trying to validating the submit process for a quick meeting. Please contact the support staff.');
          }
        }

        // this can happen when the coordinators classifications change while the user is using the form.
        if (empty($form_state['values']['request']['request_coordinator']['classification'][0]) || !is_numeric($form_state['values']['request']['request_coordinator']['classification'][0])) {
          form_set_error('request][request_coordinator][classification][0', 'No request coordinator classification has been specified.');
        }

        if ($clicked_id == 'submit-request-next') {
          $room = explode('_', $form_state['values']['request']['room'][0], 3);
          $room = (int) $room[0];
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

              $v = mfcs_convert_value_to_database_format($time_start[$key], 'timestamp', array('view' => MFCS_DATE_FORMAT_TIME, 'relative_to' => 'request][dates][date'), $form_state, $key);
              if ($v === FALSE) {
                $time_start[$key] = NULL;
                $time_stop[$key] = NULL;
              }
              else {
                $time_start[$key] = $v;

                $v = mfcs_convert_value_to_database_format($time_stop[$key], 'timestamp', array('view' => MFCS_DATE_FORMAT_TIME, 'relative_to' => 'request][dates][date'), $form_state, $key);
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
          unset($room);
          unset($date);
          unset($time_start);
          unset($time_stop);


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
      else {
        form_set_error('', 'Illegal operation detected while processing request. Please contact the support staff.');
        watchdog(MFCS_WATCHDOG_ID, 'During the create process, an unhandled click state was presented: @clicked_id.', array('@clicked_id' => $clicked_id), WATCHDOG_ERROR);
      }
    }
    elseif ($form_state['values']['form']['step'] == 'details') {
      if ($clicked_id == 'submit-request-prev') {
        $form_state['values']['form']['step'] = 'standard';
      }
      elseif ($clicked_id == 'submit-request-submit') {
        // @todo: fixme unavailability must be processed.
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
                form_set_error('request][information][type][0', 'You are not authorized to create a quick meeting.');
              }
              else {
                $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
                $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
                $form_state['rebuild'] = FALSE;
              }
            }
          }
          else {
            form_set_error('request][information][type][0', 'An internal error occured while trying to validating the submit process for a quick meeting. Please contact the support staff.');
          }
        }
        else {
          $form_state['rebuild'] = FALSE;
        }
      }
    }
    elseif ($form_state['values']['form']['step'] == 'copy') {
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
  mfcs_include(MFCS_INCLUDE_STRUCTURE);
  mfcs_include(MFCS_INCLUDE_TABLE);

  $instance = mfcs_instance();
  $user = cf_current_user();

  // The 'Venue Coordinator' location is not a real location, so find the first available location for the room and use it on save.
  if ($form_state['values']['request']['location'][0] == MFCS_BANNER_LOCATION_VENUE_COORDINATOR) {
    $locations = mfcs_load_locations_by_room($form_state['values']['request']['room'][0]);
    $location = reset($locations);
    $form_state['values']['request']['location'][0] = (int) $location->location_id;
  }

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

  $prepared = mfcs_prepare_request_values(NULL, $changes);
  if ($prepared === FALSE) {
    form_set_Error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }

  $request_id = NULL;
  $structure = mfcs_table_structure();
  $success = mfcs_set_request_values(NULL, $prepared, $structure, array(), $form_state);
  if ($success === FALSE) {
    form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to save the request.', array(), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }
  elseif ($success === TRUE) {
    $request_id = $form_state['created_request_id'];

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
