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
    unset($request_fields['request_coordinator']);
  }

  $statement_markup = 'McNeese State University is happy to provide facilities and resources to our Students, Employees, and the Community to further educational, cultural, and recreational initiatives through requests held on campus.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the Facilities Use and Special Event Request form. The following questionnaire assists campus officials in not only scheduling campus facilities or maintaining a master calendar; it allows better service through inter-departmental communication and collaboration enhancing the excellence of McNeese.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'The questionnaire is designed according to the University\'s <a href="//www.mcneese.edu/node/3217">Facility Use Policy</a>. Any activity scheduled on behalf of the University, regardless of venue, must be submitted.' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= '<strong>All requests should be made <a href="//www.mcneese.edu/node/3217#3_week_requirement" target="_blank">a minimum of three weeks in advance</a> of the event or of expected start date of publicity for the event, whichever comes first.</strong>' . '<br>';
  $statement_markup .= '<br>';
  $statement_markup .= 'If you are only making a request for a <em>Table Reservation</em> in the <em>New Ranch</em> or in the <em>Quad</em>, then you should instead use the <a href="//www.mcneese.edu/studentunion/student_union_reservations">Table Request Form</a>.' . '<br>';
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
      elseif ($clicked_id == 'submit-request-submit' || $clicked_id == 'submit-request-next') {
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

          // as of fcs-79, this check is now performed elsewhere during the validation process and is triggered via a form_set_error() instead of a warning.
          // the code is left commented out in case there is still something needed.
          /*
          if (!empty($date)) {
            $room_unavailability = mfcs_check_room_availability($room, $date, $time_start, $time_stop);

            if ($room_unavailability === FALSE) {
              form_set_error('', "An internal error occurred while checking the room for availability. Please contact the support staff.");
            }
            elseif (!empty($room_unavailability['conflicts']) || !empty($room_unavailability['unavailable'])) {
              $unavailable_message = '<span class="uppercase">Warning</span>: The specified room is currently unavailable for the following dates: ';
              $unavailable_message .= mfcs_build_room_unavailable_message($room_unavailability);
              $unavailable_message .= 'You may still submit this request and your request attempt will still be logged (but your request will be auto-closed). ';
              drupal_set_message($unavailable_message, 'warning');
              unset($unavailable_message);

              // force the status to be closed due to room being unavailable on save.
              if (!isset($form_state['room_unavailable_prior_settings'])) {
                $form_state['room_unavailable_prior_settings'] = array(
                  'status' => NULL,
                  'step' => NULL,
                );

                if (isset($form_state['values']['request']['status'])) {
                  $form_state['room_unavailable_prior_settings']['status'] = $form_state['values']['request']['status'][0];

                  if (is_string($form_state['room_unavailable_prior_settings']['status'])) {
                    $form_state['room_unavailable_prior_settings']['status'] = (int) $form_state['room_unavailable_prior_settings']['status'];
                  }
                }

                if (isset($form_state['values']['request']['step'])) {
                  $form_state['room_unavailable_prior_settings']['step'] = $form_state['values']['request']['step'][0];

                  if (is_string($form_state['room_unavailable_prior_settings']['step'])) {
                    $form_state['room_unavailable_prior_settings']['step'] = (int) $form_state['room_unavailable_prior_settings']['step'];
                  }
                }

                // if any of the rooms that is causing the scheduling conflict is locked (but not approved) close the request due to lock instead of as unavailable.
                $conflict_is_locked = FALSE;
                if (!empty($room_unavailability['conflicts'])) {
                  foreach ($room_unavailability['conflicts'] as $conflict) {
                    $request_using_room_status = NULL;

                    if (isset($conflict->status)) {
                      if (is_string($conflict->status)) {
                        $request_using_room_status = (int) $conflict->status;
                      }
                      else {
                        $request_using_room_status = $conflict->status;
                      }

                      if ($request_using_room_status === MFCS_REQUEST_STATUS_LOCKED) {
                        $conflict_is_locked = TRUE;
                        break;
                      }
                    }
                  }
                  unset($request_using_room_status);
                }

                if ($conflict_is_locked) {
                  $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK;
                }
                else {
                  $form_state['values']['request']['status'][0] = MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE;
                }
                unset($conflict_is_locked);

                $form_state['values']['request']['step'][0] = MFCS_REVIEW_STEP_COMPLETED;
              }
            }
            else {
              // restore status settings in case an available room was selected.
              if (isset($form_state['room_unavailable_prior_settings'])) {
                $form_state['values']['request']['status'][0] = $form_state['room_unavailable_prior_settings']['status'];
                $form_state['values']['request']['step'][0] = $form_state['room_unavailable_prior_settings']['step'];

                if (is_string($form_state['values']['request']['status'][0])) {
                  $form_state['values']['request']['status'][0] = (int) $form_state['values']['request']['status'][0];
                }

                if (is_string($form_state['values']['request']['step'][0])) {
                  $form_state['values']['request']['step'][0] = (int) $form_state['values']['request']['step'][0];
                }

                unset($form_state['room_unavailable_prior_settings']);
              }
            }
          }
          */

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
      else {
        form_set_error('', 'Illegal operation detected while processing request. Please contact the support staff.');
        watchdog(MFCS_WATCHDOG_ID, "During the create process, an unhandled click state was presented: @clicked_id.", array('@clicked_id' => $clicked_id), WATCHDOG_ERROR);
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
  mfcs_include(MFCS_INCLUDE_STRUCTURE);
  mfcs_include(MFCS_INCLUDE_TABLE);

  $instance = mfcs_instance();
  $user = cf_current_user();

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
