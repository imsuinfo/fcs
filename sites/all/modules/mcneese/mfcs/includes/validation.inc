<?php

/**
 * @file
 * Defines McNeese facilities use request commonly used form and validation functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

 /**
 * Perform pre-validation on a list of request form fields.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param array $dont_validate
 *   (optional) an array of submit ids to disable/ignore validation.
 *
 * @return bool|NULL
 *   TRUE on no failures, FALSE otherwise.
 *   NULL is returned when unable to traverse the form or form state.
 */
function mfcs_pre_validate_request_fields(&$form, &$form_state, $dont_validate = array()) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return NULL;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return NULL;
  }

  if (!is_array($dont_validate)) {
    cf_error::invalid_array('dont_validate');
    return NULL;
  }

  $location_form = &$form;
  $location_state = &$form_state['values'];

  $ps_c = $form['form']['parents']['#value']['structure'];
  $ps_p = NULL;
  while (!empty($ps_c)) {
    $ps = array_pop($ps_c);

    if (!array_key_exists($ps, $location_form)) {
      watchdog(MFCS_WATCHDOG_ID, "Failed to perform pre-validation, the key '@key' is missing from the form. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      return NULL;
    }

    if (!array_key_exists($ps, $location_state)) {
      // for creating new requests, the form state may not yet exist under certain circumstances and in such cases do not present and error.
      if (isset($form['form']['request'])) {
        watchdog(MFCS_WATCHDOG_ID, "Failed to perform pre-validation, the key '@key' is missing from the form_state. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      }
      return NULL;
    }

    if (is_null($ps_p)) {
      $ps_p = $ps;
    }
    else {
      $ps_p .= '][' . $ps;
    }

    $location_form = &$location_form[$ps];
    $location_state = &$location_state[$ps];
  }
  unset($ps_c);
  unset($ps);

  $passed = TRUE;
  if (empty($form_state['values']['form']['step'])) {
    return $passed;
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $is_new_request = NULL;
  if (isset($form['form']['new']['#value'])) {
    $is_new_request = $form['form']['new']['#value'];
  }

  $is_quick_meeting = FALSE;
  // quick meetings can only be used by venue coordinators.
  if (isset($location_state['information']['type'][0]) && $location_state['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING) {
    $is_quick_meeting = TRUE;
  }


  if ($form_state['values']['form']['step'] == 'standard') {
    mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

    // quick meetings can only be used by venue coordinators, their proxies, and managers.
    if ($clicked_id == 'submit-request-submit') {
      if ($is_quick_meeting) {
        $can_perform_quick_meetings = FALSE;
        if (user_access('mfcs manage', $form['form']['user']['#value'])) {
          $can_perform_quick_meetings = TRUE;
        }
        else {
          $venue_coordinator_ids = array();
          if (!empty($form['form']['user']['#value']->uid)) {
            $proxies = mfcs_get_proxy_venue_coordinator($form['form']['user']['#value']->uid);
            if (!empty($proxies)) {
              $venue_coordinator_ids = $proxies;
            }

            $venue_coordinator_ids[$form['form']['user']['#value']->uid] = $form['form']['user']['#value']->uid;
            unset($proxies);
          }

          if (!empty($location_state['venue_coordinator']['user_id'][0]) && in_array($location_state['venue_coordinator']['user_id'][0], $venue_coordinator_ids)) {
            $can_perform_quick_meetings = TRUE;
          }
        }

        if ($can_perform_quick_meetings) {
          mfcs_assign_validation_requirement('information][adviser_approval', $location_form, FALSE);

          // quick meetings are always for invited guests only, but only if they are new requests.
          // existing request may be converted into quick meetings, in which case leave the eisting 'audience' value alone.
          if ($is_new_request) {
            $location_form['plans']['audience'][0]['#value'] = MFCS_REQUEST_AUDIENCE_GUESTS_ONLY;
            $location_state['plans']['audience'][0] = MFCS_REQUEST_AUDIENCE_GUESTS_ONLY;
          }
        }
      }
      else {
        // the submit button is not allowed when not in a custom target section.
        if (isset($form['form']['target_section']) && array_key_exists('$value', $form['form']['target_section']) && !is_null($form['form']['target_section']['#value'])) {
          form_set_error('submit-request-submit', 'This request cannot be submitted during this step at this time. Try using the Continue button.');
          $passed = FALSE;
        }
      }
    }

    if ($clicked_id == 'submit-request-submit' || $clicked_id == 'submit-request-next') {
      // check to see if the request coordinator's name has changed.
      // if so, then attempt to determine the user id for that name or set to NULL on fail.
      if (isset($location_form['request_coordinator']['name']) && is_array($location_form['request_coordinator']['name'])) {
        foreach ($location_form['request_coordinator']['name'] as $key => $value) {
          if (!is_numeric($key) || !is_array($value)) {
            continue;
          }

          if (!isset($value['#default_value']) || $value['#default_value'] != $location_state['request_coordinator']['name'][$key]) {
            $found_ids = NULL;
            if (!empty($location_state['request_coordinator']['name'][$key]) && is_string($location_state['request_coordinator']['name'][$key])) {
              $found_ids = mfcs_find_from_form_user_name($location_state['request_coordinator']['name'][$key]);
            }

            if (!is_array($found_ids) || count($found_ids) != 1) {
              // unable to identify user name, so set the user id to NULL to mark it as unknown.
              $location_state['request_coordinator']['user_id'][$key] = NULL;
            }
            else {
              $location_state['request_coordinator']['user_id'][$key] = $found_ids[0];
            }
            unset($found_ids);
          }
        }
        unset($key);
        unset($value);
      }
    }
  }
  elseif ($form_state['values']['form']['step'] == 'location') {
  }
  elseif ($form_state['values']['form']['step'] == 'rooms') {
    // always enforce the venue/room coordinator.
    if (isset($location_state['venue_coordinator']['user_id'][0])) {
      $location_state['venue_coordinator']['user_id'][0] = NULL;
    }

    if (!empty($form_state['values']['request']['room'][0]) && isset($form_state['values']['request']['room'][0])) {
      $form['form']['room_information']['#value'] = mfcs_load_rooms(NULL, NULL, $form_state['values']['request']['room'][0]);
    }

    $coordinator = NULL;
    if (isset($form['form']['room_information']['#value']->room_coordinator_id)) {
      $coordinator = user_load($form['form']['room_information']['#value']->room_coordinator_id);
    }

    if (!is_object($coordinator)) {
      global $conf;

      if (empty($conf['fcs_failsafe_coordinator']) || !is_integer($conf['fcs_failsafe_coordinator'])) {
        watchdog(MFCS_WATCHDOG_ID, 'Failed to load \'fcs_failsafe_coordinator\' settings. Your settings.php does not contain the failsafe coordinator user id.', array(), WATCHDOG_CRITICAL);
        $coordinator = user_load(1); // as a last resort fall back to the default drupal admin account.
      }
      else {
        $coordinator = user_load($conf['fcs_failsafe_coordinator']);
      }
    }

    // override field settings with the proper attributes.
    if (is_object($coordinator)) {
      $location_state['venue_coordinator']['user_id'][0] = $coordinator->uid;
    }

    unset($coordinator);
  }
  elseif ($form_state['values']['form']['step'] == 'clone') {
    if ($form_state['programmed'] !== TRUE) {
      form_set_error('', 'Illegal operation detected. The clone step cannot be performed non-programmatically');
      $passed = FALSE;
    }
    else {
      // always enforce the venue/room coordinator.
      if (isset($location_state['venue_coordinator']['user_id'][0])) {
        $location_state['venue_coordinator']['user_id'][0] = NULL;
      }

      if (!empty($form_state['values']['request']['room'][0]) && isset($form_state['values']['request']['room'][0])) {
        $form['form']['room_information']['#value'] = mfcs_load_rooms(NULL, NULL, $form_state['values']['request']['room'][0]);
      }

      $coordinator = NULL;
      if (isset($form['form']['room_information']['#value']->room_coordinator_id)) {
        $coordinator = user_load($form['form']['room_information']['#value']->room_coordinator_id);
      }

      if (!is_object($coordinator)) {
        global $conf;

        if (empty($conf['fcs_failsafe_coordinator']) || !is_integer($conf['fcs_failsafe_coordinator'])) {
          watchdog(MFCS_WATCHDOG_ID, "Failed to load 'fcs_failsafe_coordinator' settings. Your settings.php does not contain the failsafe coordinator user id.", array(), WATCHDOG_CRITICAL);
          $coordinator = user_load(1); // as a last resort fall back to the default drupal admin account.
        }
        else {
          $coordinator = user_load($conf['fcs_failsafe_coordinator']);
        }
      }

      // override field settings with the proper attributes.
      if (is_object($coordinator)) {
        $location_state['venue_coordinator']['user_id'][0] = $coordinator->uid;
      }

      unset($coordinator);
    }
  }

  // condition 2 is for student requests.
  if (!empty($form_state['conditionally_required'][2])) {
    $is_required = FALSE;

    if (isset($location_state['request_coordinator']['classification']) && array_key_exists(0, $location_state['request_coordinator']['classification'])) {
      if ($location_state['request_coordinator']['classification'][0] == MFCS_REQUEST_CLASSIFICATION_STUDENT) {
        $is_required = TRUE;
      }
    }

    foreach ($form_state['conditionally_required'][2] as $field) {
      if (!is_string($field)) {
        continue;
      }

      if ($is_quick_meeting) {
        if ($field == 'information][adviser_approval') {
          continue;
        }
        elseif ($field == 'information][organization') {
          continue;
        }
      }

      mfcs_assign_validation_requirement($field, $location_form, $is_required);
    }
  }

  // condition 0 is for when fields are displayed, only then are they required.
  // this must be performed after all other conditions so that going backwards is not accidentally restricted by previous conditional requirements.
  if (array_key_exists($clicked_id, $dont_validate)) {
    if (!empty($form_state['conditionally_required'][0])) {
      foreach ($form_state['conditionally_required'][0] as &$field) {
        if (!is_string($field)) {
          continue;
        }

        mfcs_assign_validation_requirement($field, $location_form, FALSE);
      }
    }
  }

  return $passed;
}

/**
 * Perform validation on a list of request form fields.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @return bool|NULL
 *   TRUE on no failures, FALSE otherwise.
 *   NULL is returned when unable to traverse the form or form state.
 */
function mfcs_validate_request_fields($form, &$form_state) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return NULL;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return NULL;
  }

  global $base_path;

  $location_form = &$form;
  $location_state = &$form_state['values'];

  $instance = mfcs_instance();

  $ps_c = $form['form']['parents']['#value']['structure'];
  $ps_p = NULL;
  while (!empty($ps_c)) {
    $ps = array_pop($ps_c);

    if (!array_key_exists($ps, $location_form)) {
      watchdog(MFCS_WATCHDOG_ID, 'Failed to perform validation, the key \'@key\' is missing from the form. Currently processed path is @processed', array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      return NULL;
    }

    if (!array_key_exists($ps, $location_state)) {
      // for creating new requests, the form state may not yet exist under certain circumstances and in such cases do not present and error.
      if (isset($form['form']['request'])) {
        watchdog(MFCS_WATCHDOG_ID, 'Failed to perform validation, the key \'@key\' is missing from the form_state. Currently processed path is @processed', array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      }
      return NULL;
    }

    if (is_null($ps_p)) {
      $ps_p = $ps;
    }
    else {
      $ps_p .= '][' . $ps;
    }

    $location_form = &$location_form[$ps];
    $location_state = &$location_state[$ps];
  }
  unset($ps_c);
  unset($ps);

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $is_quick_meeting = FALSE;
  if (isset($location_state['information']['type'][0]) && $location_state['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING) {
    $is_quick_meeting = TRUE;
  }


  $passed = TRUE;
  if ($form_state['values']['form']['step'] == 'standard' || $clicked_id == 'submit-request-submit') {
    $quick_meeting_name = 'Quick Meeting';

    // quick meetings can only be used by venue coordinators.
    if ($is_quick_meeting) {
      if ($clicked_id == 'submit-request-next') {
        form_set_error($clicked_id, t('The form has no additional steps when \'@term_name\' is used. Use the submit button instead.', array('@term_name' => $quick_meeting_name)));
      }
      elseif ($clicked_id == 'submit-request-submit') {
        $cannot_perform_quick_meetings = TRUE;

        if (user_access('mfcs manage', $form['form']['user']['#value'])) {
          $cannot_perform_quick_meetings = FALSE;
        }
        else {
          $venue_coordinator_ids = array();
          if (!empty($form['form']['user']['#value']->uid)) {
            $proxies = mfcs_get_proxy_venue_coordinator($form['form']['user']['#value']->uid);
            if (!empty($proxies)) {
              $venue_coordinator_ids = $proxies;
            }

            $venue_coordinator_ids[$form['form']['user']['#value']->uid] = $form['form']['user']['#value']->uid;
            unset($proxies);
          }

          if (!empty($location_state['venue_coordinator']['user_id'][0]) && in_array($location_state['venue_coordinator']['user_id'][0], $venue_coordinator_ids)) {
            $cannot_perform_quick_meetings = FALSE;
          }
        }

        if ($cannot_perform_quick_meetings) {
          form_set_error($ps_p . '][information][type][0', t('You are not authorized to utilize a \'@term_name\'.', array('@term_name' => $quick_meeting_name)));
        }
      }
    }
    else {
      $not_custom_section = FALSE;
      if (isset($form['form']['target_section']) && array_key_exists('$value', $form['form']['target_section']) && !is_null($form['form']['target_section']['#value'])) {
        $not_custom_section = TRUE;
      }

      if ($clicked_id == 'submit-request-submit' && $not_custom_section) {
        form_set_error($clicked_id, t('The form may only be submitted during this step if a <strong>@term_name</strong> is used. Use the continue button instead.', array('@term_name' => $quick_meeting_name)));
      }
    }

    // confirm the availability of the room, building, and location.
    if (empty($location_state['location'][0]) || empty($location_state['building'][0]) || empty($location_state['room'][0])) {
      form_set_error('', 'The request room, building, or location has not been specified.');
    }
    else {
      $room = mfcs_load_rooms($location_state['location'][0], $location_state['building'][0], $location_state['room'][0]);
      if ($room === FALSE) {
        form_set_error('', 'The request room/building/location cannot be selected at this time. Please try another room, building, or location.');
      }
    }

    $coordinator_user = NULL;
    if (isset($location_form['venue_coordinator']['user_id'][0]['#value'])) {
      $coordinator_user = user_load($location_form['venue_coordinator']['user_id'][0]['#value']);
    }

    if (!is_object($coordinator_user)) {
      form_set_error($ps_p . '][coordinator][email][0', 'Unable to map the coordinator to a valid e-mail address. Either an invalid McNeese e-mail address was provided or an internal system error occurred.');
    }


    // make sure the request coordinator classification is never NULL.
    if (!isset($location_state['request_coordinator']['classification'][0]) || is_null($location_state['request_coordinator']['classification'][0])) {
      form_set_error($ps_p . '][request_coordinator][classification][0', 'An invalid request coordinator classification has been specified.');
    }
    else {
      // club meeting type is only allowed for student organizations.
      if (isset($location_state['information']['type'][0]) && is_numeric($location_state['information']['type'][0])) {
        if ($location_state['information']['type'][0] == MFCS_REQUEST_TYPE_CLUB_MEETING && $location_state['request_coordinator']['classification'][0] != MFCS_REQUEST_CLASSIFICATION_STUDENT) {
          mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

          $request_classifications = mfcs_get_request_classification_list_options(NULL, NULL, TRUE, TRUE);
          $request_classification_text = 'Unknown';
          if (is_array($request_classifications) && array_key_exists($location_state['request_coordinator']['classification'][0], $request_classifications)) {
            $request_classification_text = $request_classifications[MFCS_REQUEST_CLASSIFICATION_STUDENT];
          }
          unset($request_classifications);

          $request_types = mfcs_get_request_type_list_options(NULL, NULL, TRUE, TRUE);
          $request_type_text = 'Unknown';
          if (is_array($request_types) && array_key_exists($location_state['information']['type'][0], $request_types)) {
            $request_type_text = $request_types[$location_state['information']['type'][0]];
          }
          unset($request_types);

          form_set_error($ps_p . '][information][type][0', 'A <strong>' . check_plain($request_type_text) . '</strong> request type may only be specified with <strong>' . check_plain($request_classification_text) . '</strong> request classifications.');
        }
      }
    }


    // only validate dates when editing a form that has the date fields.
    $validate_dates = TRUE;
    if (isset($form_state['values']['form']['step'])) {
      if ($form_state['values']['form']['step'] == 'standard') {
        if (isset($form['form']['target_section']['#value']) && $form['form']['target_section']['#value'] != 'request' && $form['form']['target_section']['#value'] != 'dates') {
          $validate_dates = FALSE;
        }
      }
      else {
        $validate_dates = FALSE;
      }
    }

    // validate the dates.
    if ($validate_dates) {
      $dates_passed = mfcs_validate_request_fields_dates($form, $form_state, $ps_p, $location_form, $location_state, $room);

      $passed = $passed && $dates_passed;
      unset($dates_passed);
    }
    unset($validate_dates);
  }


  // condition 3 is for range limiting to provide more graceful errors than what the database would otherwise present.
  if (!empty($form_state['conditionally_required'][3])) {
    mfcs_include(MFCS_INCLUDE_TABLE);
    $structure = mfcs_table_structure();

    foreach ($form_state['conditionally_required'][3] as $field) {
      if (is_array($field)) {
        $result = mfcs_check_range_limits($field, $form, $form_state, $structure);
      }
    }
  }

  return $passed;
}

/**
 * Perform validation on a list of request form date fields.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param string $path_string_prefix
 *   This is used form reporting errors with form_set_error().
 * @param array $location_form
 *   A location within the form where the specific fields are to be processed.
 * @param array $location_state
 *  A location within the form state where the specific fields are to be
 *   processed.
 * @param array|object|bool $room
 *   An array or boolean as returned by mfcs_load_rooms().
 *
 * @return bool|NULL
 *   TRUE on no failures, FALSE otherwise.
 *   NULL is returned when unable to traverse the form or form state.
 *
 * @see: mfcs_validate_request_fields()
 * @see: mfcs_load_rooms()
 */
function mfcs_validate_request_fields_dates($form, &$form_state, $path_string_prefix, &$location_form, &$location_state, $room) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return NULL;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return NULL;
  }

  if (!is_string($path_string_prefix)) {
    cf_error::invalid_string('path_string_prefix');
    return NULL;
  }

  if (!is_array($location_form)) {
    cf_error::invalid_array('location_form');
    return NULL;
  }

  if (!is_array($location_state)) {
    cf_error::invalid_array('location_state');
    return NULL;
  }

  if (!is_array($room) && !is_object($room) && !is_bool($room)) {
    cf_error::invalid_variable('room', 'Must be a valid array, object, or boolean.');
    return NULL;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $is_new_request = FALSE;
  if (isset($form['form']['new']['#value'])) {
    $is_new_request = $form['form']['new']['#value'];
  }

  // allow bypassing if within 3-weeks requirements or not.
  $within_3_weeks_bypass = FALSE;
  if (isset($location_state['information']['type'][0])) {
    if ($location_state['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING) {
      $within_3_weeks_bypass = TRUE;
    }
  }

  $is_venue_coordinator = FALSE;
  $is_power_user = FALSE;
  if (isset($form['form']['user']['#value']->uid)) {
    $is_manager = user_access('mfcs manage', $form['form']['user']['#value']);
    $is_administer = user_access('mfcs administer', $form['form']['user']['#value']);
    // Determine if user is a venue coordinator.
    if (isset($location_state['venue_coordinator']['user_id'][0])) {
      $proxies = mfcs_get_proxy_venue_coordinator($form['form']['user']['#value']->uid);
      $is_venue_coordinator_ids = array();
      if (!empty($proxies)) {
        $is_venue_coordinator_ids = $proxies;
      }
      $is_venue_coordinator_ids[$form['form']['user']['#value']->uid] = $form['form']['user']['#value']->uid;
      $is_venue_coordinator = (in_array($location_state['venue_coordinator']['user_id'][0], $is_venue_coordinator_ids));

      unset($is_venue_coordinator_ids);
      unset($proxies);
    }

    // allow managers and administers to bypass the 21-day requirement.
    if ($is_manager || $is_administer) {
      $is_power_user = TRUE;
      $within_3_weeks_bypass = TRUE;
    }
  }

  // bypass can only be used by venue coordinators or power users.
  if ($within_3_weeks_bypass && !($is_venue_coordinator || $is_power_user)) {
    $within_3_weeks_bypass = FALSE;
  }

  $request_id = NULL;
  if (isset($form['form']['request']['#value']['mer']['id'][0]->value)) {
    $request_id = (int) $form['form']['request']['#value']['mer']['id'][0]->value;
  }

  $instance = mfcs_instance();

  $passed = TRUE;

  $date_timestamp = array(0 => NULL);
  $start_timestamp = array(0 => NULL);
  $stop_timestamp = array(0 => NULL);
  $date_in_past = array(0 => FALSE);

  $holidays = array();
  $unavailabilities = array();

  if (!empty($location_state['dates']['date'])) {
    $all_day = array();
    $partial_day = array();

    foreach ($location_state['dates']['date'] as $key => &$value) {
      $date_in_past[$key] = FALSE;

      if (isset($form['form']['existing']['#value']['request']['dates']['date'])) {
        if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['date'])) {
          // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
          // but only do this when the request is not new.
          if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['date'][$key] == $value) {
            $date_timestamp[$key] = strtotime($value);
            if ($date_timestamp[$key] !== FALSE) {
              $date_timestamp[$key] = strtotime('midnight today', $date_timestamp[$key]);
            }

            // the check for the past still needs to be performed in case individual times do change.
            $current_date = strtotime('midnight today', $instance);
            if ($date_timestamp[$key] < $current_date) {
              $date_in_past[$key] = TRUE;
            }
            unset($current_date);

            continue;
          }
        }
      }

      if (!array_key_exists($key, $start_timestamp)) {
        $start_timestamp[$key] = NULL;
      }

      if (!array_key_exists($key, $stop_timestamp)) {
        $stop_timestamp[$key] = NULL;
      }

      $date_timestamp[$key] = NULL;
      if (!empty($value) && is_string($value)) {
        $date_timestamp[$key] = strtotime($value);

        // request time and date time may have same day, but different times, so only compare day.
        $current_date = strtotime('midnight today', $instance);
        $min_days_by_policy = strtotime('+21 days', $instance);
        $min_days_by_policy = strtotime('midnight today', $min_days_by_policy);

        if ($date_timestamp[$key] < $current_date) {
          $date_in_past[$key] = TRUE;

          if ($is_power_user) {
            drupal_set_message(t('<span class="strong uppercase">Warning</span>: The date <strong>@date_nice</strong> is in the past and should not be used.', array('@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_timestamp[$key]))), 'warning');
          }
          else {
            form_set_error($path_string_prefix . '][dates][date][' . $key . '][date', t('The date <strong>@date_nice</strong> is in the past and cannot be used.', array('@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_timestamp[$key]))));
            $passed = FALSE;
          }
        }
        elseif ($date_timestamp[$key] <= $min_days_by_policy) {
          if ($within_3_weeks_bypass) {
            drupal_set_message(t('<span class="strong uppercase">Warning</span>: The date <strong>@date_nice</strong> is too soon and should be <a href="//www.mcneese.edu/node/3217#3_week_requirement" class="strong" target="_blank">a minimum of three weeks in advance</a> prior to the event or prior to the expected start date for publicity for the event, whichever comes first.', array('@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_timestamp[$key]))), 'warning');
          }
          else {
            form_set_error($path_string_prefix . '][dates][date][' . $key . '][date', t('The date <strong>@date_nice</strong> is too soon and must be <a href="//www.mcneese.edu/node/3217#3_week_requirement" class="strong" target="_blank">a minimum of three weeks in advance</a> prior to the event or prior to the expected start date for publicity for the event, whichever comes first.', array('@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_timestamp[$key]))));
            $passed = FALSE;
          }
        }

        unset($min_days_by_policy);
        unset($current_date);
      }

      // check for holidays.
      if (!is_null($date_timestamp[$key]) && $date_timestamp[$key] !== FALSE) {
        $day_start = strtotime('midnight today', $date_timestamp[$key]);

        if (!array_key_exists($day_start, $holidays)) {
          $holidays[$day_start] = mfcs_get_holidays((int) date('Y', $day_start), $day_start);

          if ($holidays[$day_start] === FALSE) {
            watchdog(MFCS_WATCHDOG_ID, 'Failed to load holidays for year @year and timestamp @timestamp (based on @date).', array('@year' => (int) date('Y', $day_start), '@timestamp' => $day_start, '@date' => $start_timestamp[$key]));
            unset($holidays[$day_start]);
          }
        }

        if (!empty($holidays[$day_start])) {
          $holiday = reset($holidays[$day_start]);
          drupal_set_message(t('<span class="strong uppercase">Warning</span>: <strong>@date</strong> may not available because of: <strong><a href="' . $base_path . 'holiday-0/view/' . $holiday->year . '/' . $holiday->id . $url_arguments . '" title="View details about holiday." target="_blank">@holiday</a></strong>.', array('@year' => date('Y', $holiday->date), '@date' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $holiday->date), '@holiday' => $holiday->title)), 'warning');
          unset($holiday);
        }
      }

      // prevent invalid all-day/partial-day date combinations from being used.
      if (!is_null($date_timestamp[$key])) {
        if (empty($location_state['dates']['time_start'][$key])) {
          if (array_key_exists($date_timestamp[$key], $all_day)) {
            form_set_error($path_string_prefix . '][dates][date][' . $key . '][date', t('<strong>@date</strong> has already been specified.', array('@date' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_timestamp[$key]))));
          }
          else {
            $all_day[$date_timestamp[$key]] = $key;
          }
        }
        else {
          $partial_day[$date_timestamp[$key]] = $key;
        }
      }
    }

    // detect and report errors for invalid date combinations.
    foreach ($all_day as $datestamp => $key) {
      if (array_key_exists($datestamp, $partial_day)) {
        form_set_error($path_string_prefix . '][dates][date][' . $key . '][date', t('<strong>@date</strong> is set to all day, but there are also partial day times in use for that same day. Please use either all day or partial day times but not both.', array('@date' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $datestamp))));
        $passed = FALSE;
      }
    }
  }

  if (!empty($location_state['dates']['time_start'])) {
    foreach ($location_state['dates']['time_start'] as $key => &$value) {
      if (!isset($date_timestamp[$key])) {
        continue;
      }

      // prevent errors due to undefined start times.
      if (!array_key_exists($key, $start_timestamp)) {
        $start_timestamp[$key] = NULL;
      }

      if (!empty($value) && !empty($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'])) {
        if (is_string($value)) {
          $start_timestamp[$key] = strtotime($value, $date_timestamp[$key]);

          if (isset($form['form']['existing']['#value']['request']['dates']['time_start'])) {
            if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['time_start'])) {
              // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
              // but only do this when the request is not new.
              if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['time_start'][$key] == $value) {
                continue;
              }
            }
          }

          $matched = preg_match('@^\s*(\d|\d\d):(\d|\d\d)\s*(pm|am)*\s*$@i', $value);

          if ($start_timestamp[$key] === FALSE || !$matched) {
            form_set_error($path_string_prefix . '][dates][time_start][' . $key, t('\'@value\' is not a valid start time.', array('@value' => $value)));
            $passed = FALSE;
            $start_timestamp[$key] = NULL;
          }
          else {
            $matched = preg_match('/(pm|am)\s*$/i', $value);

            if (!$matched) {
              form_set_error($path_string_prefix . '][dates][time_start][' . $key, t('\'@value\' is missing a valid \'am\' or \'pm\'. Please specify either am or pm at the end of the start time.', array('@value' => $value)));
              $passed = FALSE;
            }
            else {
              if (!empty($location_state['dates']['time_stop'][$key])) {
                $stop_value = strtotime($location_state['dates']['time_stop'][$key], $date_timestamp[$key]);

                if ($start_timestamp[$key] == $stop_value) {
                  form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'The start and stop times must not be the same.');
                }
              }

              $value = date($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'], $start_timestamp[$key]);
            }

            // if a time is related to a day in the past, then restrict changing the field.
            if (array_key_exists($key, $date_in_past) && $date_in_past[$key]) {
              if ($form['form']['existing']['#value']['request']['dates']['time_start'][$key] != $value) {
                if ($is_power_user) {
                  drupal_set_message(t('<span class="strong uppercase">Warning</span>: The time <strong>@time</strong> on <strong>@date_nice</strong> is in the past and should not be used.', array('@time' => $value, '@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $start_timestamp[$key]))), 'warning');
                }
                else {
                  form_set_error($path_string_prefix . '][dates][time_start][' . $key, t('<span class="strong uppercase">Warning</span>: The time <strong>@time</strong> on <strong>@date_nice</strong> is in the past and cannot be used.', array('@time' => $value, '@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $start_timestamp[$key]))));
                  $passed = FALSE;
                }
              }
            }
          }
        }
      }
    }
  }

  if (!empty($location_state['dates']['time_stop'])) {
    foreach ($location_state['dates']['time_stop'] as $key => &$value) {
      if (!isset($date_timestamp[$key])) {
        continue;
      }

      // prevent errors due to undefined stop times.
      if (!array_key_exists($key, $stop_timestamp)) {
        $stop_timestamp[$key] = NULL;
      }

      if (!empty($value) && !empty($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'])) {
        if (is_string($value)) {
          $stop_timestamp[$key] = strtotime($value, $date_timestamp[$key]);

          if (isset($form['form']['existing']['#value']['request']['dates']['time_stop'])) {
            if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['time_stop'])) {
              // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
              // but only do this when the request is not new.
              if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['time_stop'][$key] == $value) {
                continue;
              }
            }
          }

          // midnight is allowed, but triggers a false failure due to the logic in this function.
          // when midnight is specified, it ends up being midnight of the current day, which is considered the start of the day.
          // when a user specifies midnight, they esentially mean the start of the next day.
          $midnight = strtotime('midnight today', $stop_timestamp[$key]);

          if ($stop_timestamp[$key] == $midnight) {
            $stop_timestamp[$key] = strtotime('tomorrow', $midnight);
          }

          $matched = preg_match('@^\s*(\d|\d\d):(\d|\d\d)\s*(pm|am)*\s*$@i', $value);

          if ($stop_timestamp[$key] === FALSE || !$matched) {
            form_set_error($path_string_prefix . '][dates][time_stop][' . $key, t('\'@value\' is not a valid stop time.', array('@value' => $value)));
            $passed = FALSE;
          }
          else {
            $matched = preg_match('/(pm|am)\s*$/i', $value);

            if (!$matched) {
              form_set_error($path_string_prefix . '][dates][time_stop][' . $key, t('\'@value\' is missing a valid \'am\' or \'pm\'. Please specify either am or pm at the end of the stop time.', array('@value' => $value)));
              $passed = FALSE;
            }
            else {
              $value = date($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'], $stop_timestamp[$key]);
            }
          }

          // if a time is related to a day in the past, then restrict changing the field.
          if (array_key_exists($key, $date_in_past) && $date_in_past[$key]) {
            if ($form['form']['existing']['#value']['request']['dates']['time_stop'][$key] != $value) {
              if ($is_power_user) {
                drupal_set_message(t('<span class="strong uppercase">Warning</span>: The time <strong>@time</strong> on <strong>@date_nice</strong> is in the past and should not be used.', array('@time' => $value, '@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $stop_timestamp[$key]))), 'warning');
              }
              else {
                form_set_error($path_string_prefix . '][dates][time_stop][' . $key, t('<span class="strong uppercase">Warning</span>: The time <strong>@time</strong> on <strong>@date_nice</strong> is in the past and cannot be used.', array('@time' => $value, '@date_nice' => date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $stop_timestamp[$key]))));
                $passed = FALSE;
              }
            }
          }
        }
      }
    }
  }


  // make sure the timestamps do not conflict with existing reservations.
  if (!empty($start_timestamp)) {
    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

    $specified_times = array();
    $nearby = array(
      'before' => array(),
      'after' => array(),
    );

    $conflict_checked = array();
    foreach ($start_timestamp as $key => &$value) {
      if (isset($form['form']['existing']['#value']['request']['dates']['time_start'])) {
        if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['time_start'])) {
          // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
          // but only do this when the request is not new.
          if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['time_start'][$key] == $value) {
            continue;
          }
        }
      }

      if (!is_null($start_timestamp[$key]) && !is_null($stop_timestamp[$key])) {
        $conflict_checked[$key] = $key;

        if ($stop_timestamp[$key] < $start_timestamp[$key]) {
          form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'The stop time cannot be before the start time.');
          $passed = FALSE;
        }
        elseif ($stop_timestamp[$key] == $start_timestamp[$key]) {
          form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'The stop time cannot be the same as the start time.');
          $passed = FALSE;
        }

        // make sure start and stop timestamps do not overlap.
        if ($passed) {
          foreach ($specified_times as $specified_time_start => $specified_time_stop) {
            if (($start_timestamp[$key] >= $specified_time_start && $start_timestamp[$key] < $specified_time_stop) || ($specified_time_start >= $start_timestamp[$key] && $specified_time_start < $stop_timestamp[$key])) {
              $start_time_string = date($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'], $start_timestamp[$key]);
              $stop_time_string = date($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'], $stop_timestamp[$key]);

              $start_existing_string = date($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'], $specified_time_start);
              $stop_existing_string = date($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'], $specified_time_stop);
              form_set_error($path_string_prefix . '][dates][time_start][' . $key, t('The time range \'@start_time\' to \'@stop_time\' overlaps with the specified time range of \'@start_existing\' to \'@stop_existing\'.', array('@start_time' => $start_time_string, '@stop_time' => $stop_time_string, '@start_existing' => $start_existing_string, '@stop_existing' => $stop_existing_string)));
              $passed = FALSE;
              break;
            }
          }
        }

        if ($passed) {
          $specified_times[$start_timestamp[$key]] = $stop_timestamp[$key];
        }

        // check to see if the times have any requests appearing immediately before or immediately after and present a warning.
        if ($room !== FALSE) {
          $nearby_requests = mfcs_miscellaneous_nearby_requests($start_timestamp[$key], $stop_timestamp[$key], $location_state['room'][0]);

          if (is_array($nearby_requests)) {
            foreach ($nearby_requests['before'] as $before_id => $before_title) {
              $nearby['before'][$start_timestamp[$key]][$before_id] = $before_title;
            }

            foreach ($nearby_requests['after'] as $after_id => $after_title) {
              $nearby['after'][$stop_timestamp[$key]][$after_id] = $after_title;
            }
          }
        }
      }
    }

    foreach ($stop_timestamp as $key => &$value) {
      if (isset($form['form']['existing']['#value']['request']['dates']['time_stop'])) {
        if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['time_stop'])) {
          // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
          // but only do this when the request is not new.
          if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['time_stop'][$key] == $value) {
            continue;
          }
        }
      }

      if (array_key_exists($key, $conflict_checked)) {
        continue;
      }

      if (!is_null($start_timestamp[$key]) && !is_null($stop_timestamp[$key])) {
        $conflict_checked[$key] = $key;

        if ($stop_timestamp[$key] < $start_timestamp[$key]) {
          form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'The stop time cannot be before the start time.');
          $passed = FALSE;
        }
        elseif ($stop_timestamp[$key] == $start_timestamp[$key]) {
          form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'The stop time cannot be the same as the start time.');
          $passed = FALSE;
        }

        // make sure start and stop timestamps do not overlap.
        if ($passed) {
          foreach ($specified_times as $specified_time_start => $specified_time_stop) {
            if (($start_timestamp[$key] >= $specified_time_start && $start_timestamp[$key] < $specified_time_stop) || ($specified_time_start >= $start_timestamp[$key] && $specified_time_start < $stop_timestamp[$key])) {
              $start_time_string = date($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'], $start_timestamp[$key]);
              $stop_time_string = date($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'], $stop_timestamp[$key]);

              $start_existing_string = date($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'], $specified_time_start);
              $stop_existing_string = date($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'], $specified_time_stop);
              form_set_error($path_string_prefix . '][dates][time_start][' . $key, t('The time range \'@start_time\' to \'@stop_time\' overlaps with the specified time range of \'@start_existing\' to \'@stop_existing\'.', array('@start_time' => $start_time_string, '@stop_time' => $stop_time_string, '@start_existing' => $start_existing_string, '@stop_existing' => $stop_existing_string)));
              $passed = FALSE;
              break;
            }
          }
        }

        if ($passed) {
          $specified_times[$start_timestamp[$key]] = $stop_timestamp[$key];
        }

        // check to see if the times have any requests appearing immediately before or immediately after and present a warning.
        if ($room !== FALSE) {
          $nearby_requests = mfcs_miscellaneous_nearby_requests($start_timestamp[$key], $stop_timestamp[$key], $location_state['room'][0]);

          if (is_array($nearby_requests)) {
            foreach ($nearby_requests['before'] as $before_id => $before_title) {
              $nearby['before'][$start_timestamp[$key]][$before_id] = $before_title;
            }

            foreach ($nearby_requests['after'] as $after_id => $after_title) {
              $nearby['after'][$stop_timestamp[$key]][$after_id] = $after_title;
            }
          }
        }
      }
    }
    unset($conflict_checked);

    // scan through each nearby request and report a warnings when the times between multiple requests is very short.
    if (!empty($room)) {
      $minutes_range = floor(MFCS_NEARBY_REQUESTS_RANGE_SECONDS / 60);

      foreach ($nearby['before'] as $nearby_time => $nearby_value) {
        foreach ($nearby_value as $before_id => $before_title) {
          drupal_set_message('Warning: <em>' . $room->building_name . " " . $room->room_name . '</em> has the following meeting within ' . $minutes_range . ' minutes before ' . date(MFCS_DATE_FORMAT_DATE_TIME, $nearby_time) . ': <ul><li><a href="' . $base_path . 'requests/view-0/' . $before_id . '">' . $before_title . '</a></li></ul>', 'warning');
        }
      }

      foreach ($nearby['after'] as $nearby_time => $nearby_value) {
        foreach ($nearby_value as $after_id => $after_title) {
          drupal_set_message('Warning: <em>' . $room->building_name . " " . $room->room_name . '</em> has the following meeting within ' . $minutes_range . ' minutes after ' . date(MFCS_DATE_FORMAT_DATE_TIME, $nearby_time) . ': <ul><li><a href="' . $base_path . 'requests/view-0/' . $after_id . '">' . $after_title . '</a></li></ul>', 'warning');
        }
      }

      if (!empty($nearby['before']) || !empty($nearby['after'])) {
        drupal_set_message('<strong>Please be aware that there may be complications and the room may not be available or clean without adequate time between events or meetings.</strong>', 'warning');
      }
    }

    // check for room availability, but only if the room has been defined.
    $room_id = (int) $room->room_id;
    $room_unavailability = mfcs_check_room_availability($room_id, $date_timestamp, $start_timestamp, $stop_timestamp, $request_id);
    if ($room_unavailability === FALSE) {
      form_set_error('', 'An internal error occurred while checking the room for availability. Please contact the support staff.');
    }
    elseif (!empty($room_unavailability['conflicts']) || !empty($room_unavailability['unavailable'])) {
      $unavailable_message = '<span class="strong uppercase">Conflict</span>: The specified room is currently unavailable for the following dates: ';
      $unavailable_message .= mfcs_build_room_unavailable_message($room_unavailability, TRUE);
      form_set_error('review][operations][decision', $unavailable_message);
      unset($unavailable_message);
    }
    unset($room_unavailability);
    unset($room_id);
  }


  // validate the start/stop time relationships.
  $relationship_processed = array();
  if (!empty($location_state['dates']['time_start'])) {
    foreach ($location_state['dates']['time_start'] as $key => &$value) {
      if (isset($form['form']['existing']['#value']['request']['dates']['time_start'])) {
        if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['time_start'])) {
          // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
          // but only do this when the request is not new.
          if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['time_start'][$key] == $value) {
            continue;
          }
        }
      }

      $relationship_processed[$key] = $key;

      if (empty($location_state['dates']['time_start'][$key]) && !empty($location_state['dates']['time_stop'][$key])) {
        form_set_error($path_string_prefix . '][dates][time_start][' . $key, 'If you specify a stop time, then you must specify a start time.');
        $passed = FALSE;
      }
      elseif (!empty($location_state['dates']['time_start'][$key]) && empty($location_state['dates']['time_stop'][$key])) {
        form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'If you specify a start time, then you must specify a stop time.');
        $passed = FALSE;
      }
    }
  }

  if (!empty($location_state['dates']['time_stop'])) {
    foreach ($location_state['dates']['time_stop'] as $key => &$value) {
      if (isset($form['form']['existing']['#value']['request']['dates']['time_stop'])) {
        if (array_key_exists($key, $form['form']['existing']['#value']['request']['dates']['time_stop'])) {
          // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
          // but only do this when the request is not new.
          if (!$is_new_request && $form['form']['existing']['#value']['request']['dates']['time_stop'][$key] == $value) {
            continue;
          }
        }
      }

      // skip tests already performed to avoid duplicate error messages..
      if (array_key_exists($key, $relationship_processed)) {
        continue;
      }

      $relationship_processed[$key] = $key;

      if (empty($location_state['dates']['time_start'][$key]) && !empty($location_state['dates']['time_stop'][$key])) {
        form_set_error($path_string_prefix . '][dates][time_start][' . $key, 'If you specify a stop time, then you must specify a start time.');
        $passed = FALSE;
      }
      elseif (!empty($location_state['dates']['time_start'][$key]) && empty($location_state['dates']['time_stop'][$key])) {
        form_set_error($path_string_prefix . '][dates][time_stop][' . $key, 'If you specify a start time, then you must specify a stop time.');
        $passed = FALSE;
      }
    }
  }
  unset($relationship_processed);


  // validate the categories for each date.
  $no_actual_request = TRUE;
  if (!empty($location_state['dates']['category'])) {
    foreach ($location_state['dates']['category'] as $key => &$value) {
      if ($value == MFCS_REQUEST_DATES_CATEGORY_ACTUAL) {
        $no_actual_request = FALSE;
        break;
      }
    }
  }

  if ($no_actual_request) {
    form_set_error($path_string_prefix . '][dates][category', 'At least one request must have a category of \'Actual Event\'.');
    $passed = FALSE;
  }

  return $passed;
}

/**
 * Changes a form field's required state.
 *
 * @param string $field
 *   The array path name of the field.
 * @param array $form
 *   The form array where the field is stored.
 *   This will be updated.
 * @param bool $is_required
 *   Whether or not to change the field to required or not required.
 *
 * @return bool
 *   TRUE on successfull processing, FALSE otherwise.
 */
function mfcs_assign_validation_requirement($field, &$form, $is_required) {
  if (!is_string($field)) {
    cf_error::invalid_string('field');
    return FALSE;
  }

  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_bool($is_required)) {
    cf_error::invalid_bool('is_required');
    return FALSE;
  }

  $parts = explode('][', $field);

  $found = TRUE;
  $location = &$form;
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
        if (isset($location['#options']) && is_array($location_field['#options'])) {
          if ($is_required) {
            if (array_key_exists('', $location_field['#options'])) {
              unset($location_field['#options']['']);
            }
          }
          else {
            if (!array_key_exists('', $location_field['#options'])) {
              $location_field['#options'][''] = '';
            }
          }
        }

        $location_field['#required'] = $is_required;
      }
    }
  }

  return TRUE;
}

/**
 * Confirms that a form field's values are within their allowed range.
 *
 * @param array $field
 *   The array path name of the field.
 * @param array $form
 *   The form array where the field is stored.
 * @param array $form_state
 *   The form state array where the field is stored.
 * @param array $structure
 *   The table data structure for all fields.
 * @param bool $always
 *   (optional) When TRUE, always performs validation on field regardless of
 *   its visibility state. This is useful for the final submit process on
 *   multi-page forms where all values should be checked before submitting.
 *
 * @return bool
 *   TRUE on no validation errors, FALSE on validation errors, NULL in all
 *   other cases.
 */
function mfcs_check_range_limits($field, $form, &$form_state, $structure, $always = FALSE) {
  if (!is_array($field)) {
    cf_error::invalid_array('field');
    return NULL;
  }

  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return NULL;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return NULL;
  }

  if (!is_array($structure)) {
    cf_error::invalid_array('structure');
    return NULL;
  }

  if (!isset($field['group']) || !isset($field['field']) || !isset($structure[$field['group']][$field['field']])) {
    return NULL;
  }

  if (empty($field['path']) || !isset($field['ps']) || empty($field['title'])) {
    return NULL;
  }

  if (!$always) {
    if (!isset($field['visible']) || $field['visible'] === FALSE) {
      return NULL;
    }
  }

  if (empty($structure[$field['group']][$field['field']]['size'])) {
    return NULL;
  }
  $size = $structure[$field['group']][$field['field']]['size'];

  $parts = array_merge($form['form']['parents']['#value']['structure'], explode('][', $field['path']));

  $found = TRUE;
  $location_state = &$form_state['values'];
  while (!empty($parts)) {
    $part = array_shift($parts);
    if (!is_array($location_state) || !array_key_exists($part, $location_state)) {
      $found = FALSE;
      break;
    }

    $location_state = &$location_state[$part];
  }

  if (!$found || empty($location_state)) {
    return NULL;
  }

  $success = NULL;
  foreach ($location_state as $delta => &$location_value) {
    if (!is_numeric($delta)) continue;

    if (is_array($size)) {
      foreach ($size as $key => $value) {
        if (!array_key_exists($key, $location_value)) continue;

        $result = mfcs_check_range_limit($field, $form, $structure, $size[$key], $delta, $location_value[$key]);

        if ($result === FALSE) {
          $success = FALSE;
        }
        elseif (is_null($success) && $result === TRUE) {
          $success = TRUE;
        }
      }
    }
    else {
      $result = mfcs_check_range_limit($field, $form, $structure, $size, $delta, $location_value);

      if ($result === FALSE) {
        $success = FALSE;
      }
      elseif (is_null($success) && $result === TRUE) {
        $success = TRUE;
      }
    }
  }

  return $success;
}

/**
 * Confirms that a form field's value is within its allowed range.
 *
 * @param array $field
 *   The array path name of the field.
 * @param array $form
 *   The form array where the field is stored.
 * @param array $structure
 *   The array defining the structure settings for individual fields.
 * @param int|string $size
 *   The form state array where the field is stored.
 * @param int $delta
 *   The numeric index that the value is associated with.
 * @param $value
 *   The value to check the range of.
 *
 * @return bool
 *   TRUE on no validation errors, FALSE on validation errors, NULL in all
 *   other cases.
 */
function mfcs_check_range_limit($field, $form, $structure, $size, $delta, $value) {
  if (!is_array($field)) {
    cf_error::invalid_array('field');
    return NULL;
  }

  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return NULL;
  }

  if (!is_array($structure)) {
    cf_error::invalid_array('structure');
    return NULL;
  }

  if (!cf_is_integer($delta)) {
    cf_error::invalid_integer('delta');
    return NULL;
  }

  $ps = '';
  if (!empty($form['form']['parents']['#value']['structure'])) {
    $ps .= implode('][', $form['form']['parents']['#value']['structure']);
    $ps .= '][';
  }

  if ($field['ps'] == 'top]' || $field['ps'] == 'top') {
    // top ps means the field is at the top level and is not nested under any array.
    $ps .= $field['field'] . ']';
  }
  else {
    $ps .= $field['ps'] . '[' . $field['field'] . ']';
  }

  $unsigned = TRUE;
  if (isset($structure[$field['group']][$field['field']]['signed']) && $structure[$field['group']][$field['field']]['signed']) {
    $unsigned = FALSE;
  }

  $storage = NULL;
  if (isset($structure[$field['group']][$field['field']]['storage'])) {
    $storage = $structure[$field['group']][$field['field']]['storage'];
  }

  $type = NULL;
  if (isset($structure[$field['group']][$field['field']]['type'])) {
    $type = $structure[$field['group']][$field['field']]['type'];
  }

  $specific_min = NULL;
  if (isset($structure[$field['group']][$field['field']]['min'])) {
    $specific_min = $structure[$field['group']][$field['field']]['min'];
  }

  $specific_max = NULL;
  if (isset($structure[$field['group']][$field['field']]['max'])) {
    $specific_max = $structure[$field['group']][$field['field']]['max'];
  }

  $title = strip_tags($field['title']);


  if ($type == 'bool') {
    // bools are either true or false, aka: 0 or 1.
    if ($value != 0 && $value != 1) {
      form_set_error($ps . '[' . $delta, t("The field '@title' has an invalid value.", array('@title' => $title)));
      return FALSE;
    }
  }
  elseif (is_numeric($size)) {
    // when size is numeric, the value needs to be a string of a certain length.
    if (!is_string($value)) {
      form_set_error($ps . '[' . $delta, t("The field '@title' had an internal error. Please contact the support staff.", array('@title' => $title)));
      watchdog(MFCS_WATCHDOG_ID, "Field value is supposed to be a string but is not. Field: @title. Path: @path. Value: @value.", array('@title' => $title, '@path' => $field['path'], '@value' => $value), WATCHDOG_ERROR);
      return FALSE;
    }

    $min = 0;
    $max = $size;

    // now process a specific min/max as defined by the fields storage settings.
    // this cannot go lower/higher than the current $min/$max that represent the database limitations (or PHP limitations).
    if (!is_null($specific_min)) {
      if ($specific_min > $min) {
        $min = $specific_min;
      }
    }

    if (!is_null($specific_max)) {
      if ($specific_max < $max) {
        $max = $specific_max;
      }
    }

    $length = strlen($value);

    if ($length < $min) {
      form_set_error($ps . '[' . $delta, t("The field '@title' is too short. The field has a minimum length of @length characters.", array('@title' => $title, '@length' => number_format($min))));
      return FALSE;
    }

    if ($length > $max) {
      form_set_error($ps . '[' . $delta, t("The field '@title' is too long. The field has a maximum length of @length characters.", array('@title' => $title, '@length' => number_format($max))));
      return FALSE;
    }
  }
  elseif (is_string($size)) {
    $max = NULL;

    // when size is a string, then it is an integer of some size.
    if ($size == 'normal') {
      // normal appears to bo the same as big.
      if ($unsigned) {
        // PHP does not support unsigned 8-byte integers and converts them to floats.
        #$max = 18446744073709551615;
        $max = 9223372036854775807;
      }
      else {
        $max = 9223372036854775807;
      }
    }
    elseif ($size == 'tiny') {
      if ($unsigned) {
        $max = 255;
      }
      else {
        $max = 127;
      }
    }
    elseif ($size == 'small') {
      if ($unsigned) {
        $max = 65535;
      }
      else {
        $max = 32767;
      }
    }
    elseif ($size == 'medium') {
      if ($unsigned) {
        $max = 4294967295;
      }
      else {
        $max = 2147483647;
      }
    }
    elseif ($size == 'big') {
      if ($unsigned) {
        // PHP does not support unsigned 8-byte integers and converts them to floats.
        #$max = 18446744073709551615;
        $max = 9223372036854775807;
      }
      else {
        $max = 9223372036854775807;
      }
    }
    else {
      // unknown size.
      return NULL;
    }

    // note: I would be using number_form() on the outputs, but PHP's number_format() does not function properly wioth large integers.
    // for example: 9223372036854775807 gets reported as 9223372036854775808.

    // numeric uses an integer that is shifted by 10^2 places.
    if ($storage == 'numeric') {
      $max = floor($max / 100);
    }

    if ($unsigned) {
      $min = 0;
    }
    else {
      $min = -$max;
    }

    // now process a specific min/max as defined by the fields storage settings.
    // this cannot go lower/higher than the current $min/$max that represent the database limitations (or PHP limitations).
    if (!is_null($specific_min)) {
      if ($specific_min > $min) {
        $min = $specific_min;
      }
    }

    if (!is_null($specific_max)) {
      if ($specific_max < $max) {
        $max = $specific_max;
      }
    }

    if ($value < $min) {
      form_set_error($ps . '[' . $delta, t("The field '@title' is too small. The field can be no smaller than @size.", array('@title' => $title, '@size' => $min)));
      return FALSE;
    }

    if ($value > $max) {
      form_set_error($ps . '[' . $delta, t("The field '@title' is too large. The field can be no larger than @size.", array('@title' => $title, '@size' => $max)));
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * Confirms that the given time is in a valid (12-hour) format.
 *
 * @param string $time
 *   The time string to validate
 *
 * @return bool
 *   TRUE on no validation errors.
 *   FALSE on validation errors.
 *   NULL in all other cases.
 */
function mfcs_validate_time_12hour($time) {
  if (!is_string($time)) {
    cf_error::invalid_string('time');
    return NULL;
  }

  $matches = array();
  $matched = preg_match('/^\s*(\d+):(\d+)\s*(am|pm)\s*$/i', $time, $matches);

  if (!$matched) {
    return FALSE;
  }

  $hour = (int) $matches[1];
  $minutes = (int) $matches[2];

  if ($hour < 1 || $hour > 12) {
    return FALSE;
  }
  elseif ($hour < 10) {
    $hour = '0' . $hour;
  }

  if ($minutes < 0 || $minutes > 59) {
    return FALSE;
  }
  elseif ($minutes < 10) {
    $minutes = '0' . $minutes;
  }

  if (empty($matches[3])) {
    return FALSE;
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
