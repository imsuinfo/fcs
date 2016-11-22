<?php

/**
 * @file
 * Defines McNeese facilities use request move page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request move page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for a request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_manage_0_move_form($form, &$form_state, $request_id) {
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

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    cf_error::invalid_array('request');
    drupal_not_found();
    drupal_exit();
  }

  $user = cf_current_user();

  mfcs_add_canonical_headers('requests/manage-0/move/' . $request_id);

  $page_title = 'Move Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Move Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  mfcs_include(MFCS_INCLUDE_STRUCTURE);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $form = array();

  // cache the page for 70-hours.
  $form['#cache_expires'] = 252000;

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['form']['existing'] = array(
    '#id' => 'field-form-existing',
    '#type' => 'value',
    '#value' => mfcs_get_request_existing_values($request),
  );

  $form['form']['step'] = array(
    '#type' => 'value',
    '#value' => 'location',
  );

  $form['form']['operation'] = array(
    '#type' => 'value',
    '#value' => 'move',
  );

  $form['form']['sections'] = array(
    '#type' => 'value',
    '#value' => array(
      'information' => array(
        'steps' => array(
          'standard',
          'location',
          'rooms',
        ),
      ),
    ),
  );


  $form['form']['target_section'] = array(
    '#id' => 'field-form-target_section',
    '#type' => 'value',
    '#value' => 'move',
  );

  if (!empty($form_state['values']['form']['step'])) {
    $form['form']['step']['#value'] = $form_state['values']['form']['step'];
  }
  $step = $form['form']['step']['#value'];

  $form['form']['locations'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['buildings'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['rooms'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['parents'] = array(
    '#id' => 'field-form-parents',
    '#type' => 'value',
    '#value' => array(
      'structure' => array('request'),
      'path' => 'request[',
    ),
  );

  if (empty($form_state['values']['form']['locations'])) {
    $form['form']['locations']['#value'] = mfcs_get_request_location_list_options(NULL, FALSE, TRUE);
  }
  else {
    $form['form']['locations']['#value'] = $form_state['values']['form']['locations'];
  }
  $location_options = $form['form']['locations']['#value'];

  if (empty($form_state['values']['form']['buildings'])) {
    $form['form']['buildings']['#value'] = mfcs_get_request_building_list_options(NULL, NULL, FALSE, TRUE);
  }
  else {
    $form['form']['buildings']['#value'] = $form_state['values']['form']['buildings'];
  }
  $building_options = $form['form']['buildings']['#value'];

  if (empty($form_state['values']['form']['rooms'])) {
    $form['form']['rooms']['#value'] = mfcs_get_request_room_list_options(NULL, NULL, FALSE, TRUE);
  }
  else {
    $form['form']['rooms']['#value'] = $form_state['values']['form']['rooms'];
  }
  $room_options = $form['form']['rooms']['#value'];

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );

  $form['request'] = array(
    '#tree' => TRUE,
  );

  $form['log'] = array(
    '#tree' => TRUE,
  );

  global $base_path;

  $ps = NULL;

  if (!isset($form_state['values']['request'])) {
    if (isset($form['form']['existing']['#value']['request'])) {
      $form_state['values']['request'] = $form['form']['existing']['#value']['request'];
    }
    else {
      $form_state['values']['request'] = array();
    }
  }
  $request_state = &$form_state['values']['request'];

  // initialize conditionally_required 3, which defines range limits for each given field.
  $form_state['conditionally_required'][3] = array();


  // Fieldset: Location
  mfcs_get_request_field_location($form, $form_state, $form['request'], $request_state, $ps);


  // Fieldset: Room
  $room = mfcs_get_request_field_room($form, $form_state, $form['request'], $request_state, $ps);

  $form['form']['room_information'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  if (isset($room[0])) {
    $form['form']['room_information']['#value'] = mfcs_load_rooms(NULL, NULL, $room[0]);
  }


  // Fieldset: Building
  mfcs_get_request_field_building($form, $form_state, $form['request'], $request_state, $ps);


  if ($step == 'final') {
    $form['log']['message'] = array(
      '#id' => 'move-log-message',
      '#type' => 'textarea',
      '#title' => 'Message',
      '#default_value' => NULL,
      '#rows' => 4,
      '#attributes' => array(
        'class' => array(
          'no-print',
        ),
      ),
    );

    if (isset($form_state['values']['log']['message'])) {
      $form['log']['message']['#default_value'] = $form_state['values']['log']['message'];
    }
  }
  else {
    $form['log']['message'] = array(
      '#type' => 'value',
      '#value' => NULL,
    );

    if (isset($form_state['values']['log']['message'])) {
      $form['log']['message']['#value'] = $form_state['values']['log']['message'];
    }
  }


  $form['display']['submit'] = array(
    '#id' => 'request-move-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  if ($step == 'location') {
    $form['display']['submit']['cancel'] = array(
      '#id' => 'submit-request-cancel',
      '#name' => 'submit-request-cancel',
      '#type' => 'submit',
      '#default_value' => 'Cancel',
    );

    $form['display']['submit']['next'] = array(
      '#id' => 'submit-request-next',
      '#name' => 'submit-request-next',
      '#type' => 'submit',
      '#default_value' => 'Continue',
    );
  }
  elseif ($step == 'rooms') {
    $form['display']['submit']['prev'] = array(
      '#id' => 'submit-request-prev',
      '#name' => 'submit-request-prev',
      '#type' => 'submit',
      '#default_value' => 'Back',
    );

    $form['display']['submit']['next'] = array(
      '#id' => 'submit-request-next',
      '#name' => 'submit-request-next',
      '#type' => 'submit',
      '#default_value' => 'Continue',
    );
  }
  elseif ($step == 'final') {
    $form['display']['submit']['prev'] = array(
      '#id' => 'submit-request-prev',
      '#name' => 'submit-request-prev',
      '#type' => 'submit',
      '#default_value' => 'Back',
    );

    $form['display']['submit']['submit'] = array(
      '#id' => 'submit-request-submit',
      '#name' => 'submit-request-submit',
      '#type' => 'submit',
      '#default_value' => 'Submit',
    );
  }


  // generate the top of page markup.
  $current_location = 'Unknown';
  if (array_key_exists($request['top']['location'][0]->value, $location_options)) {
    $current_location = $location_options[$request['top']['location'][0]->value];
  }

  $current_building = 'Unknown';
  if (array_key_exists($request['top']['building'][0]->value, $building_options)) {
    $current_building = $building_options[$request['top']['building'][0]->value];
  }

  $current_room = 'Unknown';
  if (array_key_exists($request['top']['room'][0]->value, $room_options)) {
    $current_room = $room_options[$request['top']['room'][0]->value];
  }

  $new_location = 'Unknown';
  if (isset($form_state['values']['request']['location'][0]) && array_key_exists($form_state['values']['request']['location'][0], $location_options)) {
    $new_location = $location_options[$form_state['values']['request']['location'][0]];
  }

  $new_building = 'Unknown';
  if (isset($form_state['values']['request']['building'][0]) && array_key_exists($form_state['values']['request']['building'][0], $building_options)) {
    $new_building = $building_options[$form_state['values']['request']['building'][0]];
  }

  $new_room = 'Unknown';
  if (isset($form_state['values']['request']['room'][0]) && array_key_exists($form_state['values']['request']['room'][0], $room_options)) {
    $new_room = $room_options[$form_state['values']['request']['room'][0]];
  }

  $form['markup']['#markup'] .= '<div class="move_request-markup">';

  $form['markup']['#markup'] .= '<div class="move_request-markup-column column-current">';
  $form['markup']['#markup'] .=   '<div class="move_request-markup-column-label">Current Location:</div>';
  $form['markup']['#markup'] .=   '<div class="move_request-markup-column-rows">';
  $form['markup']['#markup'] .=     '<div class="move_request-markup-column-row row-location">';
  $form['markup']['#markup'] .=       '<div class="move_request-markup-column-row-label row-label-location">Location:</div><div class="move_request-markup-column-row-value row-value-location">' . check_plain($current_location) . '</div>';
  $form['markup']['#markup'] .=     '</div>';
  $form['markup']['#markup'] .=     '<div class="move_request-markup-column-row row-building">';
  $form['markup']['#markup'] .=       '<div class="move_request-markup-column-row-label row-label-building">Building:</div><div class="move_request-markup-column-row-value row-value-building">' . check_plain($current_building) . '</div>';
  $form['markup']['#markup'] .=     '</div>';
  $form['markup']['#markup'] .=     '<div class="move_request-markup-column-row row-room">';
  $form['markup']['#markup'] .=       '<div class="move_request-markup-column-row-label row-label-room">Room:</div><div class="move_request-markup-column-row-value row-value-room">' . check_plain($current_room) . '</div>';
  $form['markup']['#markup'] .=     '</div>';
  $form['markup']['#markup'] .=   '</div>';
  $form['markup']['#markup'] .= '</div>';

  $form['markup']['#markup'] .= '<div class="move_request-markup-column column-new">';
  $form['markup']['#markup'] .=   '<div class="move_request-markup-column-label">New Location:</div>';
  $form['markup']['#markup'] .=   '<div class="move_request-markup-column-rows">';
  $form['markup']['#markup'] .=     '<div class="move_request-markup-column-row row-location">';
  $form['markup']['#markup'] .=       '<div class="move_request-markup-column-row-label row-label-location">Location:</div><div class="move_request-markup-column-row-value row-value-location">' . check_plain($new_location)  . '</div>';
  $form['markup']['#markup'] .=     '</div>';
  $form['markup']['#markup'] .=     '<div class="move_request-markup-column-row row-building">';
  $form['markup']['#markup'] .=       '<div class="move_request-markup-column-row-label row-label-building">Building:</div><div class="move_request-markup-column-row-value row-value-building">' . check_plain($new_building)  . '</div>';
  $form['markup']['#markup'] .=     '</div>';
  $form['markup']['#markup'] .=     '<div class="move_request-markup-column-row row-room">';
  $form['markup']['#markup'] .=       '<div class="move_request-markup-column-row-label row-label-room">Room:</div><div class="move_request-markup-column-row-value row-value-room">' . check_plain($new_room)  . '</div>';
  $form['markup']['#markup'] .=     '</div>';
  $form['markup']['#markup'] .=   '</div>';
  $form['markup']['#markup'] .= '</div>';

  $form['markup']['#markup'] .= '</div>';

  return $form;
}

/**
 * Pre-Validation for mfcs_request_manage_0_move_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_move_form()
 */
function mfcs_request_manage_0_move_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-request-next') {
    // when the location changes, unset the existing rooms/buildings.
    if ($form_state['values']['form']['step'] == 'location') {
      if ($form['request']['location'][0]['#default_value'] != $form_state['values']['request']['location'][0]) {
        $form['request']['building'][0]['#default_value'] = NULL;
        $form['request']['room'][0]['#default_value'] = NULL;
        $form_state['values']['request']['building'][0] = NULL;
        $form_state['values']['request']['room'][0] = NULL;
      }
    }
    // when the room changes, re-load the building.
    elseif ($form_state['values']['form']['step'] == 'rooms') {
      if ($form['request']['room'][0]['#default_value'] != $form_state['values']['request']['room'][0]) {
        $form['form']['room_information']['#value'] = mfcs_load_rooms(NULL, NULL, $form_state['values']['request']['room'][0]);
        if (is_object($form['form']['room_information']['#value'])) {
          $form_state['values']['request']['building'][0] = $form['form']['room_information']['#value']->building_id;
        }
      }
    }
  }

  // do not perform validation when not going backwards or adding fields.
  $dont_validate = array(
    'submit-request-cancel' => TRUE,
    'submit-request-prev' => TRUE,
    'submit-request-add-date' => TRUE,
    'submit-request-remove-date' => TRUE,
  );

  mfcs_include(MFCS_INCLUDE_VALIDATION);

  mfcs_pre_validate_request_fields($form, $form_state, $dont_validate);
}

/**
 * Validate process for mfcs_request_manage_0_move_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_move_form()
 */
function mfcs_request_manage_0_move_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  // cancelling does not need validation.
  if ($clicked_id == 'submit-request-cancel') {
    return;
  }

  $form_state['rebuild'] = TRUE;
  $no_problems = TRUE;

  // moving backwards does not need validation, but form does need to be rebuilt.
  if ($clicked_id == 'submit-request-prev') {
    if ($form_state['values']['form']['step'] == 'rooms') {
      $form_state['values']['form']['step'] = 'location';
    }
    elseif ($form_state['values']['form']['step'] == 'final') {
      $form_state['values']['form']['step'] = 'rooms';
    }

    return;
  }

  // do not perform validation when not going backwards or adding fields.
  $dont_validate = array(
    'submit-request-cancel' => TRUE,
    'submit-request-prev' => TRUE,
    'submit-request-add-date' => TRUE,
    'submit-request-remove-date' => TRUE,
  );

  if ($clicked_id == 'submit-request-next') {
    if ($form_state['values']['form']['step'] == 'location') {
      $form_state['values']['form']['step'] = 'rooms';
    }
    elseif ($form_state['values']['form']['step'] == 'rooms') {
      // check for room availability
      if (is_object($form['form']['room_information']['#value'])) {
        $room_id = (int) $form['form']['room_information']['#value']->room_id;

        $all_dates = array();
        $all_time_start = array();
        $all_time_stop = array();
        $all_holidays = array();
        foreach ($form['form']['request']['#value']['dates']['date'] as $date_delta => $date_value) {
          $all_dates[$date_delta] = $form['form']['request']['#value']['dates']['date'][$date_delta]->value;
          $all_time_start[$date_delta] = $form['form']['request']['#value']['dates']['time_start'][$date_delta]->value;
          $all_time_stop[$date_delta] = $form['form']['request']['#value']['dates']['time_stop'][$date_delta]->value;

          $holidays = mfcs_get_holidays((int) date('Y', $form['form']['request']['#value']['dates']['date'][$date_delta]->value), $form['form']['request']['#value']['dates']['date'][$date_delta]->value);
          if (is_array($holidays)) {
            $all_holidays = array_merge($all_holidays, $holidays);
          }
          unset($holidays);
        }

        $room_unavailability = mfcs_check_room_availability($room_id, $all_dates, $all_time_start, $all_time_stop, $form['form']['request_id']['#value']);
        if ($room_unavailability === FALSE) {
          form_set_error('', 'An internal error occurred while checking the room for availability. Please contact the support staff.');
        }
        elseif (!empty($room_unavailability['conflicts']) || !empty($room_unavailability['unavailable'])) {
          $unavailable_message = '<span class="uppercase">Warning</span>: The specified room is currently unavailable for the following dates: ';
          $unavailable_message .= mfcs_build_room_unavailable_message($room_unavailability, TRUE);
          drupal_set_message($unavailable_message, 'warning');
          unset($unavailable_message);
        }
        unset($room_unavailability);
        unset($room_id);

        // check for holidays.
        if (!empty($all_holidays)) {
          // there currently can only be a single holiday per day, so only process a single holiday.
          $holiday = reset($all_holidays);
          if (is_object($holiday)) {
            $unavailable_message .= '<strong>';
            $unavailable_message .= '@title</a></strong>.';
            drupal_set_message(t('Warning: <strong>@date</strong> may not available because of: <strong><a href="' . $base_path . 'requests/holiday-0/view/@year/' . $holiday->year. '/' . $holiday->id . $url_arguments . '" title="View details about holiday.">@holiday</a></strong>.', array('@year' => $holiday->year, '@date' => date(MFCS_DATE_FORMAT_DATE, $holiday->date), '@holiday' => $holiday->title)), 'warning');
          }
          unset($holiday);
        }
        unset($all_holidays);
      }
      else {
        $no_problems = FALSE;
      }

      if ($no_problems) {
        $form_state['values']['form']['step'] = 'final';
      }
    }
  }

  if ($clicked_id == 'submit-request-submit') {
    if ($no_problems) {
      $form_state['rebuild'] = FALSE;
    }
  }
}

/**
 * Submit process for mfcs_request_manage_0_move_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_move_form()
 */
function mfcs_request_manage_0_move_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-move-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/manage-0/' . $request_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mfcs_include(MFCS_INCLUDE_STRUCTURE);
  mfcs_include(MFCS_INCLUDE_TABLE);

  $request = &$form['form']['request']['#value'];
  $user = cf_current_user();
  $instance = mfcs_instance();

  $changes = array(
    'status' =>  $request['top']['status'][0]->value,
    'step' => $request['top']['step'][0]->value,
    'type' =>  $request['information']['type'][0]->value,
    'classification' => $request['request_coordinator']['classification'][0]->value,
    'location' =>  $form_state['values']['request']['location'][0],
    'building' => $form_state['values']['request']['building'][0],
    'room' => $form_state['values']['request']['room'][0],
    'venue_coordinator' => $request['venue_coordinator']['user_id'][0]->value,
    'title' => $request['information']['title'][0]->value,
  );

  $prepared = mfcs_prepare_request_values($request, $changes);
  if ($prepared === FALSE) {
    form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }

  // only a subset of the structure needs to be updated.
  $structure_original = mfcs_table_structure();
  $structure = array(
    'top' => array(
      'location' => $structure_original['top']['location'],
      'building' => $structure_original['top']['building'],
      'room' => $structure_original['top']['room'],
    ),
  );
  unset($structure_original);

  $success = mfcs_set_request_values($request, $prepared, $structure, $form['form']['existing']['#value'], $form_state);
  if ($success === FALSE) {
    form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to save the request: @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }
  elseif ($success === TRUE) {
    $log_message = '';
    if (isset($form_state['values']['log']['message'])) {
      $log_message = $form_state['values']['log']['message'];
    }

    // forcefully clear the request cache so that it can reflect the changes.
    mfcs_load_request_by_id($request_id, FALSE);

    $review_extra = array(
      'never_requester' => TRUE,
      'never_coordinator' => TRUE,
    );

    mfcs_include(MFCS_INCLUDE_WORKFLOW);
    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_MOVE, MFCS_REVIEW_RESTRICTIONS_NONE, $log_message, $review_extra);
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/manage-0/' . $request_id);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
