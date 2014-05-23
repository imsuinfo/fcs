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
    $start_timestamp = FALSE;
    $stop_timestamp = FALSE;

    if (!empty($location_state['dates']['date'])) {
      $date_timestamp = strtotime($location_state['dates']['date']);

      if ($date_timestamp < REQUEST_TIME) {
        form_set_error($ps_p . '][dates][date][date', t("The date '@date' is in the past and cannot be used.", array('@date' => $location_state['dates']['date'])));
      }
      elseif ($date_timestamp == REQUEST_TIME) {
        form_set_error($ps_p . '][dates][date][date', t("The date '@date' is too soon, the event request process takes time.", array('@date' => $location_state['dates']['date'])));
      }
    }

    if (!empty($location_state['dates']['time_start']) && !empty($location_form['dates']['time_start']['#expected_format']['date'])) {
      if (is_string($location_state['dates']['time_start'])) {
        $start_timestamp = strtotime($location_state['dates']['time_start']);

        if ($start_timestamp === FALSE) {
          form_set_error($ps_p . '][dates][time_start', t("'@value' is not a valid start time.", array('@value' => $location_state['dates']['time_start'])));
          $passed = FALSE;
        }
        else {
          $location_state['dates']['time_start'] = date($location_form['dates']['time_start']['#expected_format']['date'], $start_timestamp);
        }
      }
    }

    if (!empty($location_state['dates']['time_stop']) && !empty($location_form['dates']['time_stop']['#expected_format']['date'])) {
      if (is_string($location_state['dates']['time_stop'])) {
        $stop_timestamp = strtotime($location_state['dates']['time_stop']);

        if ($stop_timestamp === FALSE) {
          form_set_error($ps_p . '][dates][time_stop', t("'@value' is not a valid start time.", array('@value' => $location_state['dates']['time_stop'])));
          $passed = FALSE;
        }
        else {
          $location_state['dates']['time_stop'] = date($location_form['dates']['time_stop']['#expected_format']['date'], $stop_timestamp);
        }
      }
    }

    if ($start_timestamp !== FALSE && $stop_timestamp !== FALSE) {
      if ($stop_timestamp < $start_timestamp) {
        form_set_error($ps_p . '][dates][time_stop', "The stop time cannot be before the start time.");
        $passed = FALSE;
      }
      elseif ($stop_timestamp == $start_timestamp) {
        form_set_error($ps_p . '][dates][time_stop', "The stop time cannot be the same as the start time.");
        $passed = FALSE;
      }
    }

    if (empty($location_state['dates']['time_start']) && !empty($location_state['dates']['time_stop'])) {
      if (!empty($location_state['dates']['time_stop'])) {
        form_set_error($ps_p . '][dates][time_start', "If you specify a stop time, then you must specify a start time.");
        $passed = FALSE;
      }
    }
    elseif (empty($location_state['dates']['time_stop']) && !empty($location_state['dates']['time_start'])) {
      form_set_error($ps_p . '][dates][time_stop', "If you specify a start time, then you must specify a stop time.");
      $passed = FALSE;
    }
  }

  return $passed;
}


/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
