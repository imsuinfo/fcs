<?php

/**
 * @file
 * Defines McNeese event workflow event commonly used form and validation functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

 /**
 * Perform pre-validation on a list of event form fields.
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
function mcneese_event_workflow_pre_validate_event_fields(&$form, &$form_state, $dont_validate = array()) {
  $location_form = &$form;
  $location_state = &$form_state['values'];

  $ps_c = $form['form']['parents']['#value']['structure'];
  $ps_p = NULL;
  while (!empty($ps_c)) {
    $ps = array_pop($ps_c);

    if (!array_key_exists($ps, $location_form)) {
      watchdog('mer', "Failed to perform pre-validation, the key '@key' is missing from the form. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      return NULL;
    }

    if (!array_key_exists($ps, $location_state)) {
      watchdog('mer', "Failed to perform pre-validation, the key '@key' is missing from the form_state. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
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

  if ($form_state['values']['form']['step'] == 'location') {
    // when location[0] is empty, then "Unsure of which venue to choose is selected.
    // this field is considered not-selected by drupal, so it triggers a failure for the required settings.
    if ($location_state['location'][0] == "") {
      $location_form['location'][0]['#required'] = FALSE;
    }
  }
  elseif ($form_state['values']['form']['step'] == 'rooms') {
    // always enforce the venue/room coordinator.
    if (isset($location_state['venue_coordinator']['user_id'][0])) {
      $location_state['venue_coordinator']['user_id'][0] = NULL;
    }

    if (!empty($form_state['values']['event']['room_list'][0]) && empty($form['form']['room_information'])) {
      $room_information_parts = explode('_', $form_state['values']['event']['room_list'][0], 3);

      if (count($room_information_parts) == 3) {
        $form['form']['room_information']['#value'] = mcneese_event_workflow_load_buildings($room_information_parts[0], $room_information_parts[1], $room_information_parts[2]);

        if (empty($form['form']['room_information']['#value'])) {
          $form['form']['room_information']['#value'] = NULL;
        }
        else {
          $form['form']['room_information']['#value'] = array_pop($form['form']['room_information']['#value']);
        }
      }
      unset($room_information_parts);
    }

    $coordinator_email = MEW_REVIEW_CLASSIFICATION_FAILSAFE_COORDINATOR;
    if (!empty($form['form']['room_information']['#value']['room_coordinator_email'])) {
      $coordinator_email = $form['form']['room_information']['#value']['room_coordinator_email'];
    }
    elseif (!empty($form['form']['room_information']['#value']['venue_coordinator_email'])) {
      $coordinator_email = $form['form']['room_information']['#value']['venue_coordinator_email'];
    }

    $coordinator = mcneese_event_workflow_ensure_user_by_email($coordinator_email, array('reviewer'));

    // override field settings with the proper attributes.
    if (is_object($coordinator)) {
      $location_state['venue_coordinator']['user_id'][0] = $coordinator->uid;
    }

    unset($coordinator);
  }

  if (array_key_exists($clicked_id, $dont_validate)) {
    if (!empty($form_state['conditionally_required'][0])) {
      foreach ($form_state['conditionally_required'][0] as $field) {
        if (is_string($field)) {
          $parts = explode('][', $field);

          $found = TRUE;
          $location = &$location_form;
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
                if (isset($location['#options'])) {
                  $location_field['#options'][''] = '';
                }
                $location_field['#required'] = FALSE;
              }
            }
          }
        }
      }
    }

    // @todo: handle conditionally_required[1] fields, which are required only when their dependant fields make them visible based on the '#state' field settings.
  }

  return $passed;
}

/**
 * Perform validation on a list of event form fields.
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
function mcneese_event_workflow_validate_event_fields($form, &$form_state) {
  $location_form = &$form;
  $location_state = &$form_state['values'];

  $ps_c = $form['form']['parents']['#value']['structure'];
  $ps_p = NULL;
  while (!empty($ps_c)) {
    $ps = array_pop($ps_c);

    if (!array_key_exists($ps, $location_form)) {
      watchdog('mer', "Failed to perform validation, the key '@key' is missing from the form. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
      return NULL;
    }

    if (!array_key_exists($ps, $location_state)) {
      watchdog('mer', "Failed to perform validation, the key '@key' is missing from the form_state. Currently processed path is @processed", array('@key' => $ps, '@processed' => isset($ps_p) ? $ps_p : NULL), WATCHDOG_ERROR);
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
  if ($form_state['values']['form']['step'] == 'standard') {
    $date_timestamp = array(0 => NULL);
    $start_timestamp = array(0 => FALSE);
    $stop_timestamp = array(0 => FALSE);

    if (!empty($location_state['dates']['date'])) {
      foreach ($location_state['dates']['date'] as $key => &$value) {
        if (isset($form['form']['existing']['#value']['event']['dates']['date'])) {
          if (in_array($value, $form['form']['existing']['#value']['event']['dates']['date'])) {
            // skip values already saved to allow updating events if they are sufficiently near/past the event date.
            continue;
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
            form_set_error($ps_p . '][dates][date][' . $key . '][date', t("The date '@date' is too soon, the event request process takes time.", array('@date' => $value)));
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

        if (isset($form['form']['existing']['#value']['event']['dates']['time_start'])) {
          if (in_array($value, $form['form']['existing']['#value']['event']['dates']['time_start'])) {
            // skip values already saved to allow updating events if they are sufficiently near/past the event date.
            continue;
          }
        }

        if (!empty($value) && !empty($location_form['dates']['time_start'][$key]['#expected_format']['date'])) {
          if (is_string($value)) {
            $start_timestamp[$key] = strtotime($value, $date_timestamp[$key]);

            if ($start_timestamp[$key] === FALSE) {
              form_set_error($ps_p . '][dates][time_start][' . $key, t("'@value' is not a valid start time.", array('@value' => $value)));
              $passed = FALSE;
            }
            else {
              $value = date($location_form['dates']['time_start'][$key]['#expected_format']['date'], $start_timestamp[$key]);
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

        if (isset($form['form']['existing']['#value']['event']['dates']['time_stop'])) {
          if (in_array($value, $form['form']['existing']['#value']['event']['dates']['time_stop'])) {
            // skip values already saved to allow updating events if they are sufficiently near/past the event date.
            continue;
          }
        }

        if (!empty($value) && !empty($location_form['dates']['time_stop'][$key]['#expected_format']['date'])) {
          if (is_string($value)) {
            $stop_timestamp[$key] = strtotime($value, $date_timestamp[$key]);

            if ($stop_timestamp[$key] === FALSE) {
              form_set_error($ps_p . '][dates][time_stop][' . $key, t("'@value' is not a valid start time.", array('@value' => $value)));
              $passed = FALSE;
            }
            else {
              $value = date($location_form['dates']['time_stop'][$key]['#expected_format']['date'], $stop_timestamp[$key]);
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
        if (isset($form['form']['existing']['#value']['event']['dates']['time_start'])) {
          if (in_array($value, $form['form']['existing']['#value']['event']['dates']['time_start'])) {
            // skip values already saved to allow updating events if they are sufficiently near/past the event date.
            continue;
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

    $coordinator_user = NULL;
    if (isset($location_form['venue_coordinator']['user_id'][0]['#value'])) {
      $coordinator_user = user_load($location_form['venue_coordinator']['user_id'][0]['#value']);
    }

    if (!is_object($coordinator_user)) {
      form_set_error($ps_p . '][coordinator][email][0', "Unable to map the coordinator to a valid e-mail address. Either an invalid McNeese e-mail address was provided or an internal system error occurred.");
    }
  }

  return $passed;
}


/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
