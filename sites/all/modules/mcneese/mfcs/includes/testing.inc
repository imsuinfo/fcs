<?php

/**
 * @file
 * Defines McNeese facilities use testing functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Programmatically clone the given request.
 *
 * @param int $request_id
 *   The request id.
 * @param string $custom_title
 *   (optional) When specified, will replace the cloned requests title with
 *   this string.
 * @param array $custom_dates
 *   (optional) When specified, will replace the cloned requests dates with
 *   the values in this array.
 * @param array $custom_time_start
 *   (optional) When specified, will replace the cloned requests time_start
 *   with the values in this array.
 * @param array $custom_time_stop
 *   (optional) When specified, will replace the cloned requests time_stop with
 *   the values in this array.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_testing_clone_request($request_id, $custom_title = NULL, $custom_dates = NULL, $custom_time_start = NULL, $custom_time_stop = NULL) {
  // restrict access to executing this function unless the proper variables are defined.
  global $conf;

  if (!isset($conf['fcs_testing']) || $conf['fcs_testing'] !== TRUE) {
    return FALSE;
  }

  // restrict access to localhost only.
  if (!isset($_SERVER['REMOTE_ADDR']) || $_SERVER['REMOTE_ADDR'] != '127.0.0.1') {
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    return FALSE;
  }

  if (!is_null($custom_title) && !is_string($custom_title)) {
    cf_error::invalid_string('custom_title');
    return FALSE;
  }

  if (!is_null($custom_dates) && !is_array($custom_dates)) {
    cf_error::invalid_array('custom_dates');
    return FALSE;
  }

  if (!is_null($custom_time_start) && !is_array($custom_time_start)) {
    cf_error::invalid_array('custom_time_start');
    return FALSE;
  }

  if (!is_null($custom_time_stop) && !is_array($custom_time_stop)) {
    cf_error::invalid_array('custom_time_stop');
    return FALSE;
  }

  global $user;

  $original_user = $user;

  try {
    $user = user_load(1);

    mfcs_include(MFCS_INCLUDE_REQUEST_CREATE);

    $id = 'mfcs_request_create_0_form';

    $state = array();
    $state['build_info']['args'] = array($request_id);
    $state['no_cache'] = TRUE;
    $state['values']['form']['step'] = 'clone';

    $form = drupal_build_form($id, $state);

    if (!empty($custom_title)) {
      $state['values']['request']['information']['title'][0] = $custom_title;
    }

    if (!is_null($custom_dates)) {
      $state['values']['request']['dates']['date'] = $custom_dates;
    }

    if (!is_null($custom_time_start)) {
      $state['values']['request']['dates']['time_start'] = $custom_time_start;
    }

    if (!is_null($custom_time_stop)) {
      $state['values']['request']['dates']['time_stop'] = $custom_time_stop;
    }

    drupal_form_submit($id, $state);
  }
  catch (Exception $ex) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return TRUE;
}

/**
 * Programmatically generates dates and times.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 * @see: mfcs_testing_clone_request()
 */
function mfcs_testing_generate_dates_and_times() {
  $dates_and_times = array();

  $date = rand(1, 62);

  $time_start_24 = rand(0, 23);
  $time_stop = 1;

  $am_pm = 'pm';
  $time_start = $time_start_24;
  if ($time_start_24 < 11) {
    $am_pm = 'am';
  }
  elseif ($time_start_24 > 12) {
    $time_start = $time_start_24 - 12;
  }

  if ($time_start_24 < 23) {
    $time_stop = rand(1, 23 - $time_start_24);
  }

  $dates_and_times['date'] = strtotime('+' . $date . ' days');
  $dates_and_times['time_start'] = strtotime($time_start . ':00' . $am_pm, $dates_and_times['date']);
  $dates_and_times['time_stop'] = strtotime('+' . $time_stop . ' hours', $dates_and_times['time_start']);

  $dates_and_times['format_date'] = date('Y/m/d', $dates_and_times['date']);
  $dates_and_times['format_time_start'] = date('h:ia', $dates_and_times['time_start']);
  $dates_and_times['format_time_stop'] = date('h:ia', $dates_and_times['time_stop']);

  return $dates_and_times;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
