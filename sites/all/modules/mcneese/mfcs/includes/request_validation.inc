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

  $is_quick_meeting = FALSE;
  // quick meetings can only be used by venue coordinators.
  if (isset($location_state['information']['type'][0]) && $location_state['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING) {
    $is_quick_meeting = TRUE;
  }

  if ($form_state['values']['form']['step'] == 'standard') {
    // quick meetings can only be used by venue coordinators.
    if ($clicked_id == 'submit-request-submit') {
      if ($is_quick_meeting) {
        $can_perform_quick_meetings = FALSE;
        if (!empty($location_state['venue_coordinator']['user_id'][0]) && !empty($form['form']['user']['#value']->uid)) {
          $venue_coordinator_id = $location_state['venue_coordinator']['user_id'][0];

          if ($venue_coordinator_id == $form['form']['user']['#value']->uid) {
            $can_perform_quick_meetings = TRUE;
          }
        }

        if ($can_perform_quick_meetings) {
          mfcs_assign_validation_requirement('information][adviser_approval', $location_form, FALSE);

          // quick meetings are always for invited guests only.
          $location_form['plans']['audience'][0]['#value'] = MFCS_REQUEST_AUDIENCE_GUESTS_ONLY;
          $location_state['plans']['audience'][0] = MFCS_REQUEST_AUDIENCE_GUESTS_ONLY;
        }
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
    if (isset($form['form']['room_information']['#value']->coordinator_id)) {
      $coordinator = user_load($form['form']['room_information']['#value']->coordinator_id);
    }

    if (!is_object($coordinator)) {
      $coordinator = user_load(MFCS_REVIEW_CLASSIFICATION_FAILSAFE_COORDINATOR);
    }

    // override field settings with the proper attributes.
    if (is_object($coordinator)) {
      $location_state['venue_coordinator']['user_id'][0] = $coordinator->uid;
    }

    unset($coordinator);
  }

  // condition 2 is for student requests.
  if (!empty($form_state['conditionally_required'][2])) {
    $is_required = FALSE;

    if (isset($location_state['request_coordinator']['classification']) && array_key_exists(0, $location_state['request_coordinator']['classification'])) {
      if ($location_state['request_coordinator']['classification'][0] == MFCS_EVENT_CLASSIFICATION_STUDENT) {
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
  $location_form = &$form;
  $location_state = &$form_state['values'];

  $ps_c = $form['form']['parents']['#value']['structure'];
  $ps_p = NULL;
  while (!empty($ps_c)) {
    $ps = array_pop($ps_c);

    if (!array_key_exists($ps, $location_form)) {
      watchdog(MFCS_WATCHDOG_ID, "Failed to perform validation, the key '@key' is missing from the form. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      return NULL;
    }

    if (!array_key_exists($ps, $location_state)) {
      // for creating new requests, the form state may not yet exist under certain circumstances and in such cases do not present and error.
      if (isset($form['form']['request'])) {
        watchdog(MFCS_WATCHDOG_ID, "Failed to perform validation, the key '@key' is missing from the form_state. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
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

  $is_new_request = NULL;
  if (isset($form['form']['new']['#value'])) {
    $is_new_request = $form['form']['new']['#value'];
  }

  $is_quick_meeting = FALSE;
  if (isset($location_state['information']['type'][0]) && $location_state['information']['type'][0] == MFCS_REQUEST_TYPE_QUICK_MEETING) {
    $is_quick_meeting = TRUE;
  }

  $passed = TRUE;
  if ($form_state['values']['form']['step'] == 'standard' || $clicked_id == 'submit-request-submit') {
    $quick_meeting_name = "Quick Meeting";
    $quick_meeting_term = taxonomy_term_load(MFCS_REQUEST_TYPE_QUICK_MEETING);
    if (!empty($quick_meeting_term->name)) {
      $quick_meeting_name = $quick_meeting_term->name;
    }

    // quick meetings can only be used by venue coordinators.
    if ($is_quick_meeting) {
      if ($clicked_id == 'submit-request-next') {
        form_set_error($clicked_id, t("The form has no additional steps when '@term_name' is used. Use the submit button instead.", array('@term_name' => $quick_meeting_name)));
      }
      elseif ($clicked_id == 'submit-request-submit') {
        $cannot_perform_quick_meetings = TRUE;
        if (!empty($location_state['venue_coordinator']['user_id'][0]) && !empty($form['form']['user']['#value']->uid)) {
          $venue_coordinator_id = $location_state['venue_coordinator']['user_id'][0];

          if ($venue_coordinator_id == $form['form']['user']['#value']->uid) {
            $cannot_perform_quick_meetings = FALSE;
          }
        }

        if ($cannot_perform_quick_meetings) {
          form_set_error($ps_p . '][information][type][0]', t("You are not authorized to utilize a '@term_name'.", array('@term_name' => $quick_meeting_name)));
        }
      }
    }
    else {
      $not_custom_section = FALSE;
      if (isset($form['form']['target_section']) && array_key_exists('$value', $form['form']['target_section']) && !is_null($form['form']['target_section']['#value'])) {
        $not_custom_section = TRUE;
      }

      if ($clicked_id == 'submit-request-submit' && $not_custom_section) {
        form_set_error($clicked_id, t("The form may only be submitted during this step if a '@term_name' is used. Use the continue button instead.", array('@term_name' => $quick_meeting_name)));
      }
    }

    $date_timestamp = array(0 => NULL);
    $start_timestamp = array(0 => FALSE);
    $stop_timestamp = array(0 => FALSE);

    if (!empty($location_state['dates']['date'])) {
      foreach ($location_state['dates']['date'] as $key => &$value) {
        if (isset($form['form']['existing']['#value']['request']['dates']['date'])) {
          if (in_array($value, $form['form']['existing']['#value']['request']['dates']['date'])) {
            // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
            // but only do this when the request is not new.
            if ($is_new_request !== TRUE) {
              continue;
            }
          }
        }

        if (!array_key_exists($key, $start_timestamp)) {
          $start_timestamp[$key] = FALSE;
        }

        if (!array_key_exists($key, $stop_timestamp)) {
          $stop_timestamp[$key] = FALSE;
        }

        $date_timestamp[$key] = NULL;
        if (!empty($value) && is_string($value)) {
          $date_timestamp[$key] = strtotime($value);

          // request time and date time may have same day, but different times, so only compare day.
          $date_string = date("Y/m/d", $date_timestamp[$key]);
          $request_string = date("Y/m/d", REQUEST_TIME);

          if ($date_string == $request_string) {
            form_set_error($ps_p . '][dates][date][' . $key . '][date', t("The date '@date' is too soon, the request process takes time.", array('@date' => $value)));
          }
          elseif ($date_timestamp[$key] < REQUEST_TIME) {
            form_set_error($ps_p . '][dates][date][' . $key . '][date', t("The date '@date' is in the past and cannot be used.", array('@date' => $value)));
          }
        }
      }
    }

    if (!empty($location_state['dates']['time_start'])) {
      foreach ($location_state['dates']['time_start'] as $key => &$value) {
        if (!isset($date_timestamp[$key])) {
          continue;
        }

        if (isset($form['form']['existing']['#value']['request']['dates']['time_start'])) {
          if (in_array($value, $form['form']['existing']['#value']['request']['dates']['time_start'])) {
            // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
            // but only do this when the request is not new.
            if ($is_new_request !== TRUE) {
              continue;
            }
          }
        }

        if (!empty($value) && !empty($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'])) {
          if (is_string($value)) {
            $start_timestamp[$key] = strtotime($value, $date_timestamp[$key]);

            if ($start_timestamp[$key] === FALSE) {
              form_set_error($ps_p . '][dates][time_start][' . $key, t("'@value' is not a valid start time.", array('@value' => $value)));
              $passed = FALSE;
            }
            else {
              $value = date($location_form['dates']['group_dates'][$key]['dates']['time_start'][$key]['#expected_format']['date'], $start_timestamp[$key]);
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

        if (isset($form['form']['existing']['#value']['request']['dates']['time_stop'])) {
          if (in_array($value, $form['form']['existing']['#value']['request']['dates']['time_stop'])) {
            // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
            // but only do this when the request is not new.
            if ($is_new_request !== TRUE) {
              continue;
            }
          }
        }

        if (!empty($value) && !empty($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'])) {
          if (is_string($value)) {
            $stop_timestamp[$key] = strtotime($value, $date_timestamp[$key]);

            if ($stop_timestamp[$key] === FALSE) {
              form_set_error($ps_p . '][dates][time_stop][' . $key, t("'@value' is not a valid start time.", array('@value' => $value)));
              $passed = FALSE;
            }
            else {
              $value = date($location_form['dates']['group_dates'][$key]['dates']['time_stop'][$key]['#expected_format']['date'], $stop_timestamp[$key]);
            }
          }
        }
      }
    }

    if (!empty($start_timestamp)) {
      foreach ($start_timestamp as $key => &$value) {
        if ($start_timestamp[$key] !== FALSE && $stop_timestamp[$key] !== FALSE) {
          if ($stop_timestamp[$key] < $start_timestamp[$key]) {
            form_set_error($ps_p . '][dates][time_stop][' . $key, "The stop time cannot be before the start time.");
            $passed = FALSE;
          }
          elseif ($stop_timestamp[$key] == $start_timestamp[$key]) {
            form_set_error($ps_p . '][dates][time_stop][' . $key, "The stop time cannot be the same as the start time.");
            $passed = FALSE;
          }
        }
      }
    }

    if (!empty($location_state['dates']['time_start'])) {
      foreach ($location_state['dates']['time_start'] as $key => &$value) {
        if (isset($form['form']['existing']['#value']['request']['dates']['time_start'])) {
          if (in_array($value, $form['form']['existing']['#value']['request']['dates']['time_start'])) {
            // skip values already saved to allow updating requests if they are sufficiently near/past the request date.
            // but only do this when the request is not new.
            if ($is_new_request !== TRUE) {
              continue;
            }
          }
        }

        if (empty($value) && !empty($location_state['dates']['time_stop'][$key])) {
          form_set_error($ps_p . '][dates][time_start][' . $key, "If you specify a stop time, then you must specify a start time.");
          $passed = FALSE;
        }
        elseif (empty($location_state['dates']['time_stop'][$key]) && !empty($value)) {
          form_set_error($ps_p . '][dates][time_stop][' . $key, "If you specify a start time, then you must specify a stop time.");
          $passed = FALSE;
        }
      }
    }

    $no_actual_request = TRUE;
    if (!empty($location_state['dates']['category'])) {
      foreach ($location_state['dates']['category'] as $key => &$value) {
        if ($value == MFCS_EVENT_DATES_CATEGORY_ACTUAL) {
          $no_actual_request = FALSE;
          break;
        }
      }
    }

    if ($no_actual_request) {
      form_set_error($ps_p . '][dates][category', "At least one request must have a category of 'Actual Event'.");
      $passed = FALSE;
    }

    $coordinator_user = NULL;
    if (isset($location_form['venue_coordinator']['user_id'][0]['#value'])) {
      $coordinator_user = user_load($location_form['venue_coordinator']['user_id'][0]['#value']);
    }

    if (!is_object($coordinator_user)) {
      form_set_error($ps_p . '][coordinator][email][0', "Unable to map the coordinator to a valid e-mail address. Either an invalid McNeese e-mail address was provided or an internal system error occurred.");
    }
  }

  // condition 3 is for range limiting to provide more graceful errors than what the database would otherwise present.
  if (!empty($form_state['conditionally_required'][3])) {
    mfcs_include(0);
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

    // currency uses an integer that is shifted by 10^2 places.
    if ($storage == 'currency') {
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
 * @} End of '@addtogroup mfcs'.
 */