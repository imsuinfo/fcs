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
 * @param array|null $custom_dates
 *   (optional) When specified, will replace the cloned requests dates with
 *   the values in this array.
 * @param array|null $custom_time_start
 *   (optional) When specified, will replace the cloned requests time_start
 *   with the values in this array.
 * @param array $custom_time_stop
 *   (optional) When specified, will replace the cloned requests time_stop with
 *   the values in this array.
 * @param array $custom_category
 *   (optional) When specified, will replace the cloned requests category with
 *   the values in this array.
 *
 * @return int|bool
 *   Created request id on success, FALSE otherwise.
 */
function mfcs_testing_clone_request($request_id, $custom_title = NULL, $custom_dates = NULL, $custom_time_start = NULL, $custom_time_stop = NULL, $custom_category = NULL) {
  if (mfcs_testing_is_enabled() === FALSE) {
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

  if (!is_null($custom_category) && !is_array($custom_category)) {
    cf_error::invalid_array('custom_category');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_CREATE);

  global $user;

  $original_user = $user;

  try {
    $user = user_load(1);

    $id = 'mfcs_request_create_0_form';

    $state = array();
    $state['build_info']['args'] = array($request_id);
    $state['no_cache'] = TRUE;
    $state['values']['form']['step'] = 'clone';
    $state['triggering_element']['#id'] = 'submit-request-submit';

    $form = drupal_build_form($id, $state);

    if (!empty($custom_title)) {
      $state['values']['request']['information']['title'][0] = $custom_title;
    }

    if (!is_null($custom_dates)) {
      $state['values']['request']['dates']['date'] = $custom_dates;
    }

    if (!is_null($custom_time_start)) {
      $state['values']['request']['dates']['time_start'] = $custom_time_start;
      $state['complete form']['form']['existing']['#value']['request']['dates']['time_start'] = $custom_time_start;
    }

    if (!is_null($custom_time_stop)) {
      $state['values']['request']['dates']['time_stop'] = $custom_time_stop;
      $state['complete form']['form']['existing']['#value']['request']['dates']['time_start'] = $custom_time_stop;
    }

    if (!is_null($custom_category)) {
      $state['values']['request']['dates']['category'] = $custom_category;
    }

    drupal_form_submit($id, $state);

    if (array_key_exists('created_request_id', $state)) {
      $user = $original_user;
      return $state['created_request_id'];
    }
  }
  catch (Error $e) {
    $user = $original_user;
    return FALSE;
  }
  catch (Exception $e) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return FALSE;
}

/**
 * Programmatically cancel the given request.
 *
 * @param int $request_id
 *   The request id.
 *
 * @return int|bool
 *   Created request id on success, FALSE otherwise.
 */
function mfcs_testing_cancel_request($request_id) {
  if (mfcs_testing_is_enabled() === FALSE) {
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request['venue_coordinator']['user_id'][0])) {
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_CANCEL);

  global $user;

  $original_user = $user;

  try {
    $user = user_load($request['venue_coordinator']['user_id'][0]->value);

    $id = 'mfcs_request_cancel_0_form';

    $state = array();
    $state['build_info']['args'] = array($request_id);
    $state['no_cache'] = TRUE;

    $form = drupal_build_form($id, $state);

    // force it to think the triggering element is the 'yes' submit button.
    $state['triggering_element']['#id'] = 'request-cancel-submit-yes';

    drupal_form_submit($id, $state);
  }
  catch (Error $e) {
    $user = $original_user;
    return FALSE;
  }
  catch (Exception $e) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return TRUE;
}

/**
 * Programmatically review the given request.
 *
 * This applies to requests in the Venue Available step.
 *
 * @param int $request_id
 *   The request id.
 * @param int $decision
 *   The review decision to assign.
 * @param array $requirement_options
 *   An array containing variables related to the operation.
 *
 * @return int|bool
 *   Created request id on success, FALSE otherwise.
 */
function mfcs_testing_review_request_venue_available($request_id, $decision = MFCS_REVIEW_DECISION_AVAILABLE, $requirement_options = array()) {
  if (mfcs_testing_is_enabled() === FALSE) {
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request['top']['step'][0]) || $request['top']['step'][0]->value != MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
    return FALSE;
  }

  if ($decision !== MFCS_REVIEW_DECISION_AVAILABLE && $decision !== MFCS_REVIEW_DECISION_UNAVAILABLE) {
    cf_error::invalid_integer('decision');
    return FALSE;
  }

  if (!is_array($requirement_options)) {
    cf_error::invalid_array('requirement_options');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_VIEW);

  global $user;

  $original_user = $user;

  try {
    $user = user_load($request['venue_coordinator']['user_id'][0]->value);

    $id = 'mfcs_request_view_0_form';


    // assign the requirements.
    $state = array();
    $state['build_info']['args'] = array($request_id, MFCS_VIEW_MODE_OPERATIONS);
    $state['no_cache'] = TRUE;

    $form = drupal_build_form($id, $state);

    // force it to think the triggering element is the 'operations' submit button.
    $state['triggering_element']['#id'] = 'request-review-operations-submit-submit';

    $state['values']['review']['operations']['decision'] = MFCS_REVIEW_DECISION_REQUIREMENT;
    $state['values']['review']['operations']['university_business'] = TRUE;
    $state['values']['review']['operations']['message'] = "This has been auto-populated by the system for testing purposes.";

    foreach ($requirement_options as $key => $value) {
      if ($key == 'decision') continue;

      $state['values']['review']['operations'][$key] = $value;
    }

    drupal_form_submit($id, $state);

    unset($form);
    unset($state);


    // approve the decision
    $state = array();
    $state['build_info']['args'] = array($request_id, MFCS_VIEW_MODE_OPERATIONS);
    $state['no_cache'] = TRUE;

    $form = drupal_build_form($id, $state);

    // force it to think the triggering element is the 'operations' submit button.
    $state['triggering_element']['#id'] = 'request-review-operations-submit-submit';

    $state['values']['review']['operations']['decision'] = $decision;
    $state['values']['review']['operations']['message'] = "This has been auto-approved or auto-denied by the system for testing purposes.";

    drupal_form_submit($id, $state);
  }
  catch (Error $e) {
    $user = $original_user;
    return FALSE;
  }
  catch (Exception $e) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return TRUE;
}

/**
 * Programmatically review the given request.
 *
 * This applies to requests in the Venue Available step.
 *
 * @param int $request_id
 *   The request id.
 * @param int $decision
 *   The review decision to assign.
 *
 * @return int|bool
 *   Created request id on success, FALSE otherwise.
 */
function mfcs_testing_review_request_review($request_id, $decision = MFCS_REVIEW_DECISION_ISSUES_NONE) {
  if (mfcs_testing_is_enabled() === FALSE) {
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request['top']['step'][0]) || ($request['top']['step'][0]->value != MFCS_REVIEW_STEP_REVIEW && $request['top']['step'][0]->value != MFCS_REVIEW_STEP_FINAL_DECISION)) {
    return FALSE;
  }

  if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW) {
    if ($decision !== MFCS_REVIEW_DECISION_ISSUES_NONE && $decision !== MFCS_REVIEW_DECISION_ISSUES) {
      cf_error::invalid_integer('decision');
      return FALSE;
    }
  }
  else {
    if ($decision !== MFCS_REVIEW_DECISION_APPROVE && $decision !== MFCS_REVIEW_DECISION_DENY && $decision !== MFCS_REVIEW_DECISION_WAIVE && $decision !== MFCS_REVIEW_DECISION_REQUIREMENT) {
      cf_error::invalid_integer('decision');
      return FALSE;
    }
  }

  $conditions = array(
    'mr.request_classification' => $request['request_coordinator']['classification'][0]->value,
    'mr.review_step' => $request['top']['step'][0]->value,
    'mr.disabled' => 0,
  );

  $reviewers_returned = mfcs_get_reviewers($conditions);
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  if (empty($reviewers)) {
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_VIEW);

  global $user;

  $original_user = $user;

  try {
    $id = 'mfcs_request_view_0_form';

    $processed = array();

    foreach ($reviewers as $reviewer) {
      if (array_key_exists($reviewer->user_id, $processed)) {
        continue;
      }

      $user = user_load($reviewer->user_id);

      $state = array();
      $state['build_info']['args'] = array($request_id, MFCS_VIEW_MODE_OPERATIONS);
      $state['no_cache'] = TRUE;

      $form = drupal_build_form($id, $state);

      // force it to think the triggering element is the 'operations' submit button.
      $state['triggering_element']['#id'] = 'request-review-operations-submit-submit';

      $state['values']['review']['operations']['decision'] = $decision;
      $state['values']['review']['operations']['message'] = "This has been auto-approved or auto-denied by the system for testing purposes.";

      drupal_form_submit($id, $state);

      unset($form);
      unset($state);

      $processed[$reviewer->user_id] = TRUE;
    }
  }
  catch (Error $e) {
    $user = $original_user;
    return FALSE;
  }
  catch (Exception $e) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return TRUE;
}

/**
 * Programmatically review the given request.
 *
 * This applies to requests in the Venue Available step.
 *
 * @param int $request_id
 *   The request id.
 * @param int $decision
 *   The review decision to assign.
 *
 * @return int|bool
 *   Created request id on success, FALSE otherwise.
 */
function mfcs_testing_review_request_r($request_id, $decision = MFCS_REVIEW_DECISION_APPROVE) {
  if (mfcs_testing_is_enabled() === FALSE) {
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request['venue_coordinator']['user_id'][0]) || !is_object($request['venue_coordinator']['user_id'][0])) {
    return FALSE;
  }

  if (empty($request['top']['step'][0]) || !is_object($request['top']['step'][0]) || $request['top']['step'][0]->value != MFCS_REVIEW_STEP_FINAL_DECISION) {
    return FALSE;
  }

  if ($decision !== MFCS_REVIEW_DECISION_APPROVE && $decision !== MFCS_REVIEW_DECISION_DENY) {
    cf_error::invalid_integer('decision');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_VIEW);

  global $user;

  $original_user = $user;

  try {
    $id = 'mfcs_request_view_0_form';

    $user = user_load($request['venue_coordinator']['user_id'][0]->value);

    $state = array();
    $state['build_info']['args'] = array($request_id, MFCS_VIEW_MODE_OPERATIONS);
    $state['no_cache'] = TRUE;

    $form = drupal_build_form($id, $state);

    // force it to think the triggering element is the 'operations' submit button.
    $state['triggering_element']['#id'] = 'request-review-operations-submit-submit';

    $state['values']['review']['operations']['decision'] = $decision;
    $state['values']['review']['operations']['message'] = "This has been auto-approved or auto-denied by the system for testing purposes.";

    drupal_form_submit($id, $state);
  }
  catch (Error $e) {
    $user = $original_user;
    return FALSE;
  }
  catch (Exception $e) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return TRUE;
}

/**
 * Programmatically review the given request.
 *
 * This applies to requests in the Venue Available step.
 *
 * @param int $request_id
 *   The request id.
 * @param int $decision
 *   The review decision to assign.
 *
 * @return int|bool
 *   Created request id on success, FALSE otherwise.
 */
function mfcs_testing_review_request_make_decisions($request_id, $decision = MFCS_REVIEW_DECISION_APPROVE) {
  if (mfcs_testing_is_enabled() === FALSE) {
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request['venue_coordinator']['user_id'][0]) || !is_object($request['venue_coordinator']['user_id'][0])) {
    return FALSE;
  }

  if (empty($request['top']['step'][0]) || !is_object($request['top']['step'][0]) || $request['top']['step'][0]->value != MFCS_REVIEW_STEP_REVIEW) {
    return FALSE;
  }

  if ($decision !== MFCS_REVIEW_DECISION_APPROVE && $decision !== MFCS_REVIEW_DECISION_DENY) {
    cf_error::invalid_integer('decision');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_VIEW);

  global $user;

  $original_user = $user;

  try {
    $id = 'mfcs_request_view_0_form';

    $user = user_load($request['venue_coordinator']['user_id'][0]->value);

    $state = array();
    $state['build_info']['args'] = array($request_id, MFCS_VIEW_MODE_OPERATIONS);
    $state['no_cache'] = TRUE;

    $form = drupal_build_form($id, $state);

    // force it to think the triggering element is the 'operations' submit button.
    $state['triggering_element']['#id'] = 'request-review-operations-submit-submit';

    $state['values']['review']['operations']['decision'] = $decision;
    $state['values']['review']['operations']['message'] = "This has been auto-approved or auto-denied by the system for testing purposes.";

    drupal_form_submit($id, $state);
  }
  catch (Error $e) {
    $user = $original_user;
    return FALSE;
  }
  catch (Exception $e) {
    $user = $original_user;
    return FALSE;
  }

  $user = $original_user;
  return TRUE;
}

/**
 * Check to see if testing is enabled.
 *
 * @return bool
 *   TRUE when testing is enabled and allowed, FALSE otherwise.
 */
function mfcs_testing_is_enabled() {
  // restrict access to executing this function unless the proper variables are defined.
  global $conf;

  if (!isset($conf['fcs_testing']) || $conf['fcs_testing'] !== TRUE) {
    return FALSE;
  }

  // restrict access to localhost only.
  if (!isset($_SERVER['REMOTE_ADDR']) || $_SERVER['REMOTE_ADDR'] != '127.0.0.1') {
    return FALSE;
  }

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

  $days = rand(1, 62);
  $category = rand(1, 9);

  if ($category < 6) {
    $category = MFCS_REQUEST_DATES_CATEGORY_ACTUAL;
    $format_category = "Event / Meeting";
  }
  elseif ($category < 8) {
    $category = MFCS_REQUEST_DATES_CATEGORY_SETUP;
    $format_category = "Rehearsal / Setup";
  }
  else {
    $category = MFCS_REQUEST_DATES_CATEGORY_CLEANUP;
    $format_category = "Cleanup / Breakdown";
  }

  $time_start_24 = rand(0, 22);
  $time_stop = 1;
  $time_short = rand(0, 2);

  $am_pm = 'pm';
  $time_start = $time_start_24;
  if ($time_start_24 < 12) {
    $am_pm = 'am';
  }
  elseif ($time_start_24 > 12) {
    $time_start = $time_start_24 - 12;
  }

  if ($time_short < 2) {
    $time_short = rand(1, 4);
    if ($time_short + $time_start_24 < 24) {
      $time_stop = $time_short;
    }
  }
  else {
    $time_stop = rand(1, 23 - $time_start_24);
  }

  $dates_and_times['date'] = strtotime('+' . $days . ' days');
  $dates_and_times['time_start'] = strtotime($time_start . ':00' . $am_pm, $dates_and_times['date']);
  $dates_and_times['time_stop'] = strtotime('+' . $time_stop . ' hours', $dates_and_times['time_start']);
  $dates_and_times['category'] = $category;

  $dates_and_times['format_date'] = date(MFCS_DATE_FORMAT_DATE, $dates_and_times['date']);
  $dates_and_times['format_time_start'] = date(MFCS_DATE_FORMAT_TIME, $dates_and_times['time_start']);
  $dates_and_times['format_time_stop'] = date(MFCS_DATE_FORMAT_TIME, $dates_and_times['time_stop']);
  $dates_and_times['format_category'] = $format_category;

  return $dates_and_times;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
