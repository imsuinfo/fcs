<?php

/**
 * @file
 * Defines McNeese facilities use workflow functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Assign a new step to a given request.
 *
 * @param int $request_id
 *   The id of the given request.
 * @param int $step
 *   The new step to assign to the specified request.
 * @param object|null $user
 *   If specified, the user to log this change as.
 *
 * @return bool
 *   (optional) TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_set_step($request_id, $step, $user = NULL) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    return FALSE;
  }

  if (!cf_is_integer($step)) {
    cf_error::invalid_integer('step');
    return FALSE;
  }

  if (is_null($user)) {
    $user = cf_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $step_options = mfcs_get_review_step_list_options();

  if (!array_key_exists($step, $step_options)) {
    watchdog(MFCS_WATCHDOG_ID, "%step is not a valid step.", array('%step' => $step), WATCHDOG_ERROR);
    return FALSE;
  }

  $instance = mfcs_instance();

  $mfcs_requests = array(
    'updated' => $instance,
    'step' => $step,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

  if ($request_revision === FALSE) {
    watchdog(MFCS_WATCHDOG_ID, "Failed to load revision number for request %request_id.", array('%request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($request_revision)) {
    $request_revision_next = 0;
  }
  else {
    $request_revision_next = $request_revision + 1;
  }

  $mfcs_request_revisions = array(
    'request_id' => $request_id,
    'date' => $instance,
    'revision' => $request_revision_next,
    'user_id' => $user->uid,
  );

  $step_revision = $request['current']['top']['step'] + 1;

  $mfcs_current = array(
    'top_step' => $step_revision,
  );

  $mfcs_field_top_step = array(
    'request_id' => $request_id,
    'revision' => $step_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => $instance,
    'value' => $step,
  );


  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_requests');
    $query->condition('id', $request_id);
    $query->fields($mfcs_requests);
    $query->execute();

    if (!$processed_mfcs_request_revisions) {
      $query = db_insert('mfcs_request_revisions');
      $query->fields($mfcs_request_revisions);
      $query->execute();

      $processed_mfcs_request_revisions = TRUE;
    }

    $query = db_insert('mfcs_field_top_step');
    $query->fields($mfcs_field_top_step);
    $query->execute();

    $query = db_update('mfcs_current');
    $query->condition('request_id', $request_id);
    $query->fields($mfcs_current);
    $query->execute();

    $query = db_delete('mfcs_reviewed');
    $query->condition('request_id', $request_id);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // reset the cached request object
  mfcs_load_request_by_id($request_id, FALSE);

  return TRUE;
}

/**
 * Assign a new status to a given request.
 *
 * @param int $request_id
 *   The id of the given request.
 * @param int $status
 *   The new status to assign to the specified request.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 * @param array $extra
 *   (optional) Additional information to save. This is used for the revision
 *   log message and signature.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_set_status($request_id, $status, $user = NULL, $extra = array()) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    return FALSE;
  }

  if (!cf_is_integer($status)) {
    cf_error::invalid_integer('status');
    return FALSE;
  }

  if (is_null($user)) {
    $user = cf_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if (!is_array($extra)) {
    cf_error::invalid_array('extra');
    return FALSE;
  }

  $status_options = array(
    MFCS_REQUEST_STATUS_DELETED,
    MFCS_REQUEST_STATUS_LOCKED,
    MFCS_REQUEST_STATUS_UNLOCKED,
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    MFCS_REQUEST_STATUS_CLOSED_DENIED,
    MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
    MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
    MFCS_REQUEST_STATUS_CANCELLED,
  );

  if (!in_array($status, $status_options)) {
    watchdog(MFCS_WATCHDOG_ID, "%status is not a valid status.", array('%status' => $status), WATCHDOG_ERROR);
    return FALSE;
  }

  $instance = mfcs_instance();

  $mfcs_requests = array(
    'updated' => $instance,
    'status' => $status,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

  if ($request_revision === FALSE) {
    watchdog(MFCS_WATCHDOG_ID, "Failed to load revision number for request %request_id.", array('%request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($request_revision)) {
    $request_revision_next = 0;
  }
  else {
    $request_revision_next = $request_revision + 1;
  }

  $mfcs_request_revisions = array(
    'request_id' => $request_id,
    'date' => $instance,
    'revision' => $request_revision_next,
    'user_id' => $user->uid,
  );

  $revisions_has_extras = FALSE;
  if (!empty($extra['revisions']['message'])) {
    $mfcs_request_revisions['message'] = $extra['revisions']['message'];
    $revisions_has_extras = TRUE;
  }

  if (!empty($extra['revisions']['signature'])) {
    $mfcs_request_revisions['signature'] = $extra['revisions']['signature'];
    $revisions_has_extras = TRUE;
  }

  $status_revision = $request['current']['top']['status'] + 1;

  $mfcs_current = array(
    'top_status' => $status_revision,
  );

  $mfcs_field_top_status = array(
    'request_id' => $request_id,
    'revision' => $status_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => $instance,
    'value' => $status,
  );

  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_requests');
    $query->condition('id', $request_id);
    $query->fields($mfcs_requests);
    $query->execute();

    if ($processed_mfcs_request_revisions) {
      if ($revisions_has_extras) {
        $query = db_update('mfcs_request_revisions');
        $query->condition('id', $request_id);
        $query->condition('date', $instance);
        $query->condition('user_id', $user->uid);
        $query->fields($mfcs_request_revisions);
        $query->execute();
      }
    }
    else {
      $query = db_insert('mfcs_request_revisions');
      $query->fields($mfcs_request_revisions);
      $query->execute();

      $processed_mfcs_request_revisions = TRUE;
    }

    $query = db_insert('mfcs_field_top_status');
    $query->fields($mfcs_field_top_status);
    $query->execute();

    $query = db_update('mfcs_current');
    $query->condition('request_id', $request_id);
    $query->fields($mfcs_current);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // reset the cached request object
  mfcs_load_request_by_id($request_id, FALSE);

  return TRUE;
}

/**
 * Assign a new venue coordinator to a given request.
 *
 * @param int $request_id
 *   The id of the given request.
 * @param int $venue_coordinator_id
 *   The new venue coordinator id to assign to the request.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 * @param array $extra
 *   (optional) Additional information to save. This is used for the revision
 *   log message and signature.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_set_venue_coordinator($request_id, $venue_coordinator_id, $user = NULL, $extra = array()) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    return FALSE;
  }

  if (!cf_is_integer($venue_coordinator_id)) {
    cf_error::invalid_integer('venue_coordinator_id');
    return FALSE;
  }

  if (is_null($user)) {
    $user = cf_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if (!is_array($extra)) {
    cf_error::invalid_array('extra');
    return FALSE;
  }

  $venue_coordinator = mfcs_load_basic_user_settings_by_user_id($venue_coordinator_id);
  if (!is_array($venue_coordinator)) {
    watchdog(MFCS_WATCHDOG_ID, "%venue_coordinator is not a valid user id.", array('%venue_coordinator' => $venue_coordinator_id), WATCHDOG_ERROR);
    return FALSE;
  }

  $instance = mfcs_instance();

  $mfcs_requests = array(
    'updated' => $instance,
    'venue_coordinator' => $venue_coordinator_id,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

  if ($request_revision === FALSE) {
    watchdog(MFCS_WATCHDOG_ID, "Failed to load revision number for request %request_id.", array('%request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($request_revision)) {
    $request_revision_next = 0;
  }
  else {
    $request_revision_next = $request_revision + 1;
  }

  $mfcs_request_revisions = array(
    'request_id' => $request_id,
    'date' => $instance,
    'revision' => $request_revision_next,
    'user_id' => $user->uid,
  );

  $revisions_has_extras = FALSE;
  if (!empty($extra['revisions']['message'])) {
    $mfcs_request_revisions['message'] = $extra['revisions']['message'];
    $revisions_has_extras = TRUE;
  }

  if (!empty($extra['revisions']['signature'])) {
    $mfcs_request_revisions['signature'] = $extra['revisions']['signature'];
    $revisions_has_extras = TRUE;
  }

  $venue_coordinator_id_revision = $request['current']['venue_coordinator']['user_id'] + 1;

  $mfcs_current = array(
    'venue_coordinator_user_id' => $venue_coordinator_id_revision,
  );

  $mfcs_field_venue_coordinator_user_id = array(
    'request_id' => $request_id,
    'revision' => $venue_coordinator_id_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => $instance,
    'value' => $venue_coordinator_id,
  );

  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_requests');
    $query->condition('id', $request_id);
    $query->fields($mfcs_requests);
    $query->execute();

    if ($processed_mfcs_request_revisions) {
      if ($revisions_has_extras) {
        $query = db_update('mfcs_request_revisions');
        $query->condition('id', $request_id);
        $query->condition('date', $instance);
        $query->condition('user_id', $user->uid);
        $query->fields($mfcs_request_revisions);
        $query->execute();
      }
    }
    else {
      $query = db_insert('mfcs_request_revisions');
      $query->fields($mfcs_request_revisions);
      $query->execute();

      $processed_mfcs_request_revisions = TRUE;
    }

    $query = db_insert('mfcs_field_venue_coordinator_user_id');
    $query->fields($mfcs_field_venue_coordinator_user_id);
    $query->execute();

    $query = db_update('mfcs_current');
    $query->condition('request_id', $request_id);
    $query->fields($mfcs_current);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // reset the cached request object
  mfcs_load_request_by_id($request_id, FALSE);

  return TRUE;
}

/**
 * Trigger the next step.
 *
 * @param int $request_id
 *   The request id.
 * @param object $reviewer
 *   The reviewer.
 * @param int $decision
 *   The facilities use decision.
 * @param string $message
 *   (optional) The facilities use decision message.
 * @param array $extra
 *   (optional) Additional fields that are specific to the individual step.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_step_review($request_id, $user, $decision, $message = "", $extra = array()) {
  if (!is_object($user)) {
    cf_error::invalid_integer('user');
    return FALSE;
  }

  if (!cf_is_integer($decision)) {
    cf_error::invalid_integer('decision');
    return FALSE;
  }

  if (!is_string($message)) {
    cf_error::invalid_string('message');
    return FALSE;
  }

  if (!is_array($extra)) {
    cf_error::invalid_array('extra');
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is not a valid request id, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  $manager = user_access('mfcs manage', $user);

  $instance = mfcs_instance();

  $send_email = TRUE;
  if (isset($extra['send_email']) && $extra['send_email'] === FALSE) {
    $send_email = FALSE;
  }

  $classification = $request['request_coordinator']['classification'][0]->value;
  $status = $request['top']['status'][0]->value;
  $step = $request['top']['step'][0]->value;
  $type = $request['information']['type'][0]->value;
  $venue_coordinator_id = $request['venue_coordinator']['user_id'][0]->value;

  $reviewer_classifications = array();
  $reviewer_classifications_all = array();
  $reviewer_conditions = array(
    'mr.user_id' => $user->uid,
    'mr.request_classification' => $classification,
    'mr.disabled' => 0,
  );

  $reviewers_returned = mfcs_get_reviewers($reviewer_conditions, array('join' => TRUE));
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  foreach ($reviewers as $reviewer) {
    if (isset($reviewer->review_step) && $reviewer->review_step == $step) {
      $reviewer_classifications[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
    }

    $reviewer_classifications_all[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
  }

  $proxies = mfcs_get_proxy_venue_coordinator($user->uid);
  $venue_coordinator_ids = array();
  if (!empty($proxies)) {
    $venue_coordinator_ids = $proxies;
  }
  $venue_coordinator_ids[$user->uid] = $user->uid;

  $is_effective_venue_coordinator = FALSE;
  if ($venue_coordinator_id == $user->uid) {
    $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR;
    $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR;
    $is_effective_venue_coordinator = TRUE;
  }
  elseif (in_array($venue_coordinator_id, $venue_coordinator_ids)) {
    $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY;
    $reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY;
    $is_effective_venue_coordinator = TRUE;
  }

  $is_quick_meeting = FALSE;
  if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && ($is_effective_venue_coordinator || $manager)) {
    $is_quick_meeting = TRUE;
  }

  // review revisions are neither used nor incremented in the same manner as other fields.
  // the revision number will not increment when a new review is made.
  $review_revision = mfcs_load_request_revision_number($request_id, 'mfcs_field_review_review');

  if ($review_revision === FALSE || is_null($review_revision)) {
    $review_revision = 0;
  }

  $user_data = array(
    'user_id' => $user->uid,
  );

  $mfcs_requests = array(
    'updated' => $instance,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

  if ($request_revision === FALSE) {
    watchdog(MFCS_WATCHDOG_ID, "Failed to load revision number for request %request_id.", array('%request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($request_revision)) {
    $request_revision_next = 0;
  }
  else {
    $request_revision_next = $request_revision + 1;
  }

  $mfcs_request_revisions = array(
    'request_id' => $request_id,
    'date' => $instance,
    'revision' => $request_revision_next,
    'user_id' => $user->uid,
  );

  $mfcs_current_requirements = array(
    'request_id' => $request_id,
    'requirements_facilities' => NULL,
    'requirements_equipment' => NULL,
    'requirements_custodial' => NULL,
    'requirements_maintenance' => NULL,
    'requirements_grounds' => NULL,
    'requirements_security' => NULL,
    'requirements_other' => NULL,
    'requirements_waived' => NULL,
    'requirements_university' => NULL,
  );

  $requirements_latest = array(
    'facilities' => NULL,
    'equipment' => NULL,
    'custodial' => NULL,
    'maintenance' => NULL,
    'grounds' => NULL,
    'security' => NULL,
    'other' => NULL,
    'waived' => NULL,
    'university' => NULL,
  );

  try {
    foreach ($requirements_latest as $requirement_name => &$requirement_latest_value) {
      if (array_key_exists($requirement_name, $request['current']['requirements']) && !is_null($request['current']['requirements'][$requirement_name])) {
        $mfcs_current_requirements['requirements_' . $requirement_name] = $request['current']['requirements'][$requirement_name];
        $requirement_latest_value = $mfcs_current_requirements['requirements_' . $requirement_name];
      }
      else {
        $query = db_select('mfcs_field_requirements_' . $requirement_name, 'mfufx');
        $query->addField('mfufx', 'revision', 'revision');
        $query->condition('mfufx.request_id', $request_id);
        $query->orderBy('mfufx.revision', 'DESC');
        $query->range(0, 1);

        $result = $query->execute()->fetchField();

        if ($result !== FALSE) {
          $requirement_latest_value = $result;
        }
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  $insurance_requirements = FALSE;
  $insurance_provided_exception = FALSE;

  $mfcs_current_insurance = array(
    'request_id' => $request_id,
    'insurance_contractor' => NULL,
    'insurance_unaffiliated' => NULL,
    'insurance_provided' => NULL,
  );

  $insurance_latest = array(
    'contractor' => NULL,
    'unaffiliated' => NULL,
    'provided' => NULL,
  );

  if (!empty($request['current']['insurance'])) {
    if (array_key_exists('contractor', $request['current']['insurance']) && !is_null($request['current']['insurance']['contractor'])) {
      $mfcs_current_insurance['insurance_contractor'] = $request['current']['insurance']['contractor'];
    }

    if (array_key_exists('unaffiliated', $request['current']['insurance']) && !is_null($request['current']['insurance']['unaffiliated'])) {
      $mfcs_current_insurance['insurance_unaffiliated'] = $request['current']['insurance']['unaffiliated'];
    }

    if (array_key_exists('provided', $request['current']['insurance']) && !is_null($request['current']['insurance']['provided'])) {
      $mfcs_current_insurance['insurance_provided'] = $request['current']['insurance']['provided'];
    }
  }

  try {
    foreach ($insurance_latest as $insurance_name => &$insurance_latest_value) {
      if (array_key_exists($insurance_name, $request['current']['insurance']) && !is_null($request['current']['insurance'][$insurance_name])) {
        $mfcs_current_insurance['insurance_' . $insurance_name] = $request['current']['insurance'][$insurance_name];
        $insurance_latest_value = $mfcs_current_insurance['insurance_' . $insurance_name];
      }
      else {
        $query = db_select('mfcs_field_insurance_' . $insurance_name, 'mfix');
        $query->addField('mfix', 'revision', 'revision');
        $query->condition('mfix.request_id', $request_id);
        $query->orderBy('mfix.revision', 'DESC');
        $query->range(0, 1);

        $result = $query->execute()->fetchField();
        if ($result !== FALSE) {
          $insurance_latest_value = $result;
        }
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  $mfcs_field_review_review = array(
    'request_id' => $request_id,
    'revision' => $review_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => $instance,
    'step' => $step,
    'decision' => $decision,
    'message' => $message,
  );

  if ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    // remove classifications not allowed in these steps, unless they are for a comment or a message for the review log.
    if ($decision != MFCS_REVIEW_DECISION_COMMENT && $decision != MFCS_REVIEW_DECISION_CANCELLED && $decision != MFCS_REVIEW_DECISION_UNCANCELLED && $decision != MFCS_REVIEW_DECISION_CLOSED && $decision != MFCS_REVIEW_DECISION_REASSIGNED) {
      unset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR]);
      unset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY]);
    }
  }

  $mfcs_field_requirements = array();
  $requirements_waived_save = FALSE;
  $requirements_university_save = FALSE;
  $requirements_other_save = FALSE;
  if (($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $is_quick_meeting) && ($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
    $requirements_waived = NULL;
    if (isset($extra['requirements']['waived'])) {
      $requirements_waived = $extra['requirements']['waived'];
      $requirements_waived_save = TRUE;
    }

    if (is_null($mfcs_current_requirements['requirements_waived'])) {
      if (is_null($requirements_latest['waived'])) {
        $mfcs_current_requirements['requirements_waived'] = 0;
        $requirements_waived_save = TRUE;
      }
      elseif ($requirements_waived_save) {
        $mfcs_current_requirements['requirements_waived'] = $requirements_latest['waived'] + 1;
      }
    }
    elseif ($requirements_waived_save) {
      $mfcs_current_requirements['requirements_waived']++;
    }

    if ($requirements_waived_save) {
      $mfcs_field_requirements['waived'] = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_requirements['requirements_waived'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => $instance,
        'value' => $requirements_waived,
      );
    }

    $requirements_university = NULL;
    if (isset($extra['requirements']['university'])) {
      $requirements_university = $extra['requirements']['university'];
      $requirements_university_save = TRUE;
    }

    if (is_null($mfcs_current_requirements['requirements_university'])) {
      if (is_null($requirements_latest['university'])) {
        $mfcs_current_requirements['requirements_university'] = 0;
        $requirements_university_save = TRUE;
      }
      elseif ($requirements_university_save) {
        $mfcs_current_requirements['requirements_university'] = $requirements_latest['university'] + 1;
      }
    }
    elseif ($requirements_university_save) {
      $mfcs_current_requirements['requirements_university']++;
    }

    if ($requirements_university_save) {
      $mfcs_field_requirements['university'] = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_requirements['requirements_university'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => $instance,
        'value' => $requirements_university,
      );
    }

    $requirements_other = NULL;
    if (isset($extra['requirements']['other'])) {
      $requirements_other = $extra['requirements']['other'];
      $requirements_other_save = TRUE;

      // do not save the 'other' value when requirements are waved or reset if 'other' is already saved.
      if (($requirements_university_save && $requirements_waived) || ($requirements_university_save && $requirements_university)) {
        $requirements_other = NULL;
      }
    }

    if (is_null($mfcs_current_requirements['requirements_other'])) {
      if (is_null($requirements_latest['other'])) {
        if (is_null($requirements_other)) {
          $requirements_other_save = FALSE;
        }
        else {
          $mfcs_current_requirements['requirements_other'] = 0;
          $requirements_other_save = TRUE;
        }
      }
      elseif ($requirements_other_save) {
        $mfcs_current_requirements['requirements_other'] = $requirements_latest['other'] + 1;
      }
    }
    elseif ($requirements_other_save) {
      $mfcs_current_requirements['requirements_other']++;
    }

    if ($requirements_other_save) {
      $mfcs_field_requirements['other'] = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_requirements['requirements_other'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => $instance,
      );

      if (!is_null($requirements_other)) {
        $mfcs_field_requirements['other']['value'] = $requirements_other;
      }
    }
  }

  if (($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_REQUIREMENT) && ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
    $uses = array();
    $uses_as_manager = array();

    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if ($type != MFCS_REQUEST_TYPE_QUICK_MEETING) {
        if ($is_effective_venue_coordinator) {
          $uses['facilities'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES;
          $uses['equipment'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES; // @fixme: create an equipment classification
        }
        elseif ($manager) {
          $uses['facilities'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES;
          $uses['equipment'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES; // @fixme: create an equipment classification

          $uses_as_manager['facilities'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES;
          $uses_as_manager['equipment'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES; // @fixme: create an equipment classification
        }
      }
    }
    elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
      if ($manager) {
        $uses['custodial'] = MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL;
        $uses['maintenance'] = MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE;
        $uses['grounds'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES; // @fixme: create a grounds classification
        $uses['security'] = MFCS_REVIEWER_CLASSIFICATION_SECURITY;

        $uses_as_manager['custodial'] = MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL;
        $uses_as_manager['maintenance'] = MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE;
        $uses_as_manager['grounds'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES; // @fixme: create a grounds classification
        $uses_as_manager['security'] = MFCS_REVIEWER_CLASSIFICATION_SECURITY;
      }

      if (isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_FACILITIES_CUSTODIAL]) || isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL])) {
        $uses['custodial'] = MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL;
        unset($uses_as_manager['custodial']);
      }

      if (isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE])) {
        $uses['maintenance'] = MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE;
        unset($uses_as_manager['maintenance']);
      }

      if (isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_FACILITIES])) {
        $uses['grounds'] = MFCS_REVIEWER_CLASSIFICATION_FACILITIES; // @fixme: create an grounds classification
        unset($uses_as_manager['grounds']);
      }

      if (isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_SECURITY])) {
        $uses['security'] = MFCS_REVIEWER_CLASSIFICATION_SECURITY;
        unset($uses_as_manager['security']);
      }
    }

    if (isset($reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING])) {
      if ($step == MFCS_REVIEW_STEP_REVIEW) {
        $insurance_requirements = TRUE;
      }
      else {
        $insurance_provided_exception = TRUE;
        $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_PURCHASING] = MFCS_REVIEWER_CLASSIFICATION_PURCHASING;
      }
    }
    elseif ($manager) {
      if ($step == MFCS_REVIEW_STEP_REVIEW) {
        $insurance_requirements = TRUE;
      }
      else {
        $insurance_provided_exception = TRUE;
      }
    }

    // when waiving fees, automatically reset all use fees, by assigning ID as NULL to prevent them from being utilized as a reviewer_classification.
    if (($requirements_university_save && $requirements_waived) || ($requirements_university_save && $requirements_university)) {
      foreach (array('facilities', 'equipment', 'custodial', 'maintenance', 'grounds', 'security', 'other') as $use) {
        if (!array_key_exists($use, $uses)) {
          $uses[$use] = NULL;
        }
      }
    }

    foreach ($uses as $use => $use_id) {
      if (!is_null($use_id) && (!isset($extra['requirements']) || !array_key_exists($use, $extra['requirements']))) {
        unset($uses_as_manager[$use]);

        // @fixme: at the moment, handle the duplicate cases, which will be removed once the duplicates are made distinct.
        if ($use == 'facilities' || $use == 'equipment' || $use == 'grounds') {
          if (!isset($uses_as_manager['facilities']) && !isset($uses_as_manager['equipment']) && !isset($uses_as_manager['grounds'])) {
            unset($reviewer_classifications[$use_id]);
          }
        }
        else {
          unset($reviewer_classifications[$use_id]);
        }

        if (!($requirements_university_save && $requirements_waived) && !($requirements_university_save && $requirements_university)) {
          continue;
        }
      }

      if (is_null($use_id)) {
        if (($requirements_university_save && $requirements_waived) || ($requirements_university_save && $requirements_university)) {
          $requirements = array('quantity' => NULL, 'hours' => NULL, 'days' => NULL, 'amount' => NULL);
        }
        else {
          continue;
        }
      }
      elseif (($requirements_university_save && $requirements_waived) || ($requirements_university_save && $requirements_university) || is_null($extra['requirements'][$use])) {
        $requirements = array('quantity' => NULL, 'hours' => NULL, 'days' => NULL, 'amount' => NULL);
      }
      else {
        $requirements = array('quantity' => 0, 'hours' => 0, 'days' => 0, 'amount' => 0);

        if (!empty($extra['requirements'][$use]['quantity'])) {
          $requirements['quantity'] = $extra['requirements'][$use]['quantity'];
        }

        if (!empty($extra['requirements'][$use]['hours'])) {
          $hours = mfcs_convert_value_to_database_format($extra['requirements'][$use]['hours'], 'currency');
          if ($hours !== FALSE) {
            $requirements['hours'] = $hours;
          }
        }

        if (!empty($extra['requirements'][$use]['days'])) {
          $days = mfcs_convert_value_to_database_format($extra['requirements'][$use]['days'], 'currency');
          if ($days !== FALSE) {
            $requirements['days'] = $days;
          }
        }

        if (!empty($extra['requirements'][$use]['amount'])) {
          $amount = mfcs_convert_value_to_database_format($extra['requirements'][$use]['amount'], 'currency');

          if ($amount !== FALSE) {
            $requirements['amount'] = $amount;
          }
        }
      }

      $use_name = 'requirements_' . $use;

      // if the use fees are not set, then do not reset them when waiving fees.
      if (is_null($use_id) && is_null($mfcs_current_requirements[$use_name]) && is_null($requirements_latest[$use])) {
        continue;
      }

      if (is_null($mfcs_current_requirements[$use_name])) {
        if (is_null($requirements_latest[$use])) {
          $mfcs_current_requirements[$use_name] = 0;
        }
        else {
          $mfcs_current_requirements[$use_name] = $requirements_latest[$use] + 1;
        }
      }
      else {
        $mfcs_current_requirements[$use_name]++;
      }

      $mfcs_field_requirements[$use] = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_requirements[$use_name],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => $instance,
      );

      foreach (array('quantity', 'hours', 'days', 'amount') as $use_column) {
        if (!is_null($requirements[$use_column])) {
          $mfcs_field_requirements[$use][$use_column] = $requirements[$use_column];
        }
      }
      unset($use_column);
      unset($requirements);
    }

    if ($manager && !empty($uses_as_manager)) {
      if (count($uses_as_manager) == count($uses)) {
        $reviewer_classifications = array();
        $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = MFCS_REVIEWER_CLASSIFICATION_MANAGER;
      }
      else {
        $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = MFCS_REVIEWER_CLASSIFICATION_MANAGER;
      }
    }

    if ($insurance_requirements) {
      $contractor_insurance = 0;
      $unaffiliated_insurance = 0;

      if (isset($extra['insurance']['contractor'])) {
        $contractor_insurance = $extra['insurance']['contractor'];
      }

      if (isset($extra['insurance']['unaffiliated'])) {
        $unaffiliated_insurance = $extra['insurance']['unaffiliated'];
      }

      $new_contractor_insurance = FALSE;
      if (isset($request['insurance']['contractor'][0]->value)) {
        if ($contractor_insurance != $request['insurance']['contractor'][0]->value) {
          $new_contractor_insurance = TRUE;
        }
      }
      else {
        if (isset($extra['insurance']['contractor']) && !is_null($extra['insurance']['contractor'])) {
          $new_contractor_insurance = TRUE;
        }
      }

      $new_unaffiliated_insurance = FALSE;
      if (isset($request['insurance']['unaffiliated'][0]->value)) {
        if ($unaffiliated_insurance != $request['insurance']['unaffiliated'][0]->value) {
          $new_unaffiliated_insurance = TRUE;
        }
      }
      else {
        if (isset($extra['insurance']['unaffiliated']) && !is_null($extra['insurance']['unaffiliated'])) {
          $new_unaffiliated_insurance = TRUE;
        }
      }

      // contractor insurance
      if ($new_contractor_insurance) {
        if (is_null($mfcs_current_insurance['insurance_contractor'])) {
          if (is_null($insurance_latest['contractor'])) {
            $mfcs_current_insurance['insurance_contractor'] = 0;
          }
          else {
            $mfcs_current_insurance['insurance_contractor'] = $insurance_latest['contractor'] + 1;
          }
        }
        else {
          $mfcs_current_insurance['insurance_contractor']++;
        }

        $mfcs_field_insurance_contractor = array(
          'request_id' => $request_id,
          'revision' => $mfcs_current_insurance['insurance_contractor'],
          'delta' => 0,
          'user_id' => $user->uid,
          'date' => $instance,
          'value' => $contractor_insurance,
        );
      }

      // unaffiliated insurance
      if ($new_unaffiliated_insurance) {
        if (is_null($mfcs_current_insurance['insurance_unaffiliated'])) {
          if (is_null($insurance_latest['unaffiliated'])) {
            $mfcs_current_insurance['insurance_unaffiliated'] = 0;
          }
          else {
            $mfcs_current_insurance['insurance_unaffiliated'] = $insurance_latest['unaffiliated'] + 1;
          }
        }
        else {
          $mfcs_current_insurance['insurance_unaffiliated']++;
        }

        $mfcs_field_insurance_unaffiliated = array(
          'request_id' => $request_id,
          'revision' => $mfcs_current_insurance['insurance_unaffiliated'],
          'delta' => 0,
          'user_id' => $user->uid,
          'date' => $instance,
          'value' => $unaffiliated_insurance,
        );
      }
    }

    // provided insurance
    if ($insurance_requirements || $insurance_provided_exception) {
      $provided_insurance = 0;

      if (isset($extra['insurance']['provided'])) {
        $provided_insurance = $extra['insurance']['provided'];
      }

      $new_provided_insurance = FALSE;
      if (isset($request['insurance']['provided'][0]->value)) {
        if ($provided_insurance != $request['insurance']['provided'][0]->value) {
          $new_provided_insurance = TRUE;
        }
      }
      else {
        if (isset($extra['insurance']['provided']) && !is_null($extra['insurance']['provided'])) {
          $new_provided_insurance = TRUE;
        }
      }

      if ($new_provided_insurance) {
        if (is_null($mfcs_current_insurance['insurance_provided'])) {
          if (is_null($insurance_latest['provided'])) {
            $mfcs_current_insurance['insurance_provided'] = 0;
          }
          else {
            $mfcs_current_insurance['insurance_provided'] = $insurance_latest['provided'] + 1;
          }
        }
        else {
          $mfcs_current_insurance['insurance_provided']++;
        }
        $mfcs_field_insurance_provided = array(
          'request_id' => $request_id,
          'revision' => $mfcs_current_insurance['insurance_provided'],
          'delta' => 0,
          'user_id' => $user->uid,
          'date' => $instance,
          'value' => $provided_insurance,
        );
      }
    }
  }

  $failure = FALSE;
  $transaction = db_transaction();

  try {
    $query = db_update('mfcs_requests');
    $query->condition('id', $request_id);
    $query->fields($mfcs_requests);
    $query->execute();

    if (!$processed_mfcs_request_revisions) {
      $query = db_insert('mfcs_request_revisions');
      $query->fields($mfcs_request_revisions);
      $query->execute();
      $processed_mfcs_request_revisions = TRUE;
    }

    $query = db_insert('mfcs_field_review_review');
    $query->fields($mfcs_field_review_review);
    $review_id = $query->execute();

    foreach ($reviewer_classifications as $reviewer_classification) {
      $mfcs_review_classifications = array(
        'review_id' => $review_id,
        'classification' => $reviewer_classification,
      );

      $query = db_insert('mfcs_review_classifications');
      $query->fields($mfcs_review_classifications);
      $query->execute();

      $mfcs_reviewed = array(
        'request_id' => $request_id,
        'classification' => $reviewer_classification,
      );

      if ($decision == MFCS_REVIEW_DECISION_APPROVE || $decision == MFCS_REVIEW_DECISION_DENY) {
        $query = db_insert('mfcs_reviewed');
        $query->fields($mfcs_reviewed);
        $query->execute();
      }

      unset($mfcs_review_classifications);
    }

    if (($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_REQUIREMENT) && ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW  || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
      foreach ($mfcs_field_requirements as $use => $fields) {
        $query = db_insert('mfcs_field_requirements_' . $use);
        $query->fields($fields);
        $query->execute();
      }

      $query = db_update('mfcs_current');
      $query->condition('request_id', $request_id);
      $query->fields($mfcs_current_requirements);
      $query->execute();

      if ($insurance_requirements) {
        if ($new_contractor_insurance) {
          $query = db_insert('mfcs_field_insurance_contractor');
          $query->fields($mfcs_field_insurance_contractor);
          $query->execute();
        }

        if ($new_unaffiliated_insurance) {
          $query = db_insert('mfcs_field_insurance_unaffiliated');
          $query->fields($mfcs_field_insurance_unaffiliated);
          $query->execute();
        }
      }

      if ($insurance_requirements || $insurance_provided_exception) {
        if ($new_provided_insurance) {
          $query = db_insert('mfcs_field_insurance_provided');
          $query->fields($mfcs_field_insurance_provided);
          $query->execute();
        }
      }

      if ($insurance_requirements && ($new_contractor_insurance || $new_unaffiliated_insurance)) {
        $query = db_update('mfcs_current');
        $query->condition('request_id', $request_id);
        $query->fields($mfcs_current_insurance);
        $query->execute();
      }
      elseif (($insurance_requirements || $insurance_provided_exception) && $new_provided_insurance) {
        $query = db_update('mfcs_current');
        $query->condition('request_id', $request_id);
        $query->fields($mfcs_current_insurance);
        $query->execute();
      }
    }
    elseif ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
      if ($requirements_other_save) {
        $query = db_insert('mfcs_field_requirements_other');
        $query->fields($mfcs_field_requirements['other']);
        $query->execute();
      }

      if ($requirements_waived_save) {
        $query = db_insert('mfcs_field_requirements_waived');
        $query->fields($mfcs_field_requirements['waived']);
        $query->execute();
      }

      if ($requirements_university_save) {
        $query = db_insert('mfcs_field_requirements_university');
        $query->fields($mfcs_field_requirements['university']);
        $query->execute();
      }

      if ($requirements_other_save || $requirements_waived_save || $requirements_university_save) {
        $query = db_update('mfcs_current');
        $query->condition('request_id', $request_id);
        $query->fields($mfcs_current_requirements);
        $query->execute();
      }
    }

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // forcefully clear the request cache so that it can reflect the changes.
  $request = mfcs_load_request_by_id($request_id, TRUE);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is not a valid request id, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  if (isset($request['venue_coordinator']['user_id'][0]->value)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is missing the venue coordinator, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  if (isset($request['request_coordinator']['classification'][0]->value)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is missing the coordinator classification, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  if (isset($request['top']['status'][0]->value)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is missing the status, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  if (isset($request['top']['step'][0]->value)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is missing the step, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  if (isset($request['information']['type'][0]->value)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_id is missing the type, cannot continue processing workflow changes.", array('%request_id' => $request_id), WATCHDOG_CRITICAL);
    return FALSE;
  }

  $classification = $request['request_coordinator']['classification'][0]->value;
  $status = $request['top']['status'][0]->value;
  $step = $request['top']['step'][0]->value;
  $type = $request['information']['type'][0]->value;
  $venue_coordinator_id = $request['venue_coordinator']['user_id'][0]->value;

  // handle steps that do not alter the workflow.
  if ($decision == MFCS_REVIEW_DECISION_COMMENT) {
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'reviewer';
    $parameters['message'] = $message;
    $parameters['comment'] = TRUE;

    if ($send_email) {
      mfcs_send_workflow_emails($request_id, $parameters);
    }

    return TRUE;
  }
  elseif ($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_CANCELLED || $decision == MFCS_REVIEW_DECISION_UNCANCELLED || $decision == MFCS_REVIEW_DECISION_CLOSED || ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && !$insurance_provided_exception) || $decision == MFCS_REVIEW_DECISION_REASSIGNED) {
    // do not send an e-mail and do not progress to the next step.
    return TRUE;
  }

  // continue to next step only if at least 1 of each reviewer classification has submitted a reply.
  if ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    $decisions = mfcs_get_requests_current_review_decisions(array($request_id => $review_revision), $classification, $step);

    if (is_bool($decisions)) {
      return $decisions;
    }

    if (!array_key_exists($request_id, $decisions)) {
      return FALSE;
    }

    // When FALSE, then there are not enough reviews to continue.
    if ($decisions[$request_id]['current'] < $decisions[$request_id]['total']) {
      return TRUE;
    }
  }
  elseif ($insurance_provided_exception) {
    // for all other steps, when insurance is provided, send an e-mail to alert reviewers that the insurance has been provided.
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'reviewer';
    $parameters['message'] = $message;
    $parameters['insurance_provided'] = TRUE;

    if ($send_email) {
      mfcs_send_workflow_emails($request_id, $parameters);
    }

    return TRUE;
  }

  mfcs_workflow_next_step($request_id, $type, $step, $decision, $message, $send_email);

  return TRUE;
}

/**
 * Continue onto the next step in the workflow.
 *
 * @param int $request_id
 *   The request id.
 * @param int $type
 *   The current request classification type.
 * @param int $step
 *   The current step.
 * @param int $decision
 *   The facilities use decision.
 * @param string $message
 *   The facilities use decision message.
 * @param bool $send_email
 *   TRUE to send e-mail, FALSE to not send.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 */
function mfcs_workflow_next_step($request_id, $type, $step, $decision, $message, $send_email) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  if (!cf_is_integer($type)) {
    cf_error::invalid_integer('type');
    return FALSE;
  }

  if (!cf_is_integer($step)) {
    cf_error::invalid_integer('step');
    return FALSE;
  }

  if (!cf_is_integer($decision)) {
    cf_error::invalid_integer('decision');
    return FALSE;
  }

  if (!is_string($message)) {
    cf_error::invalid_string('message');
    return FALSE;
  }

  if (!is_bool($send_email)) {
    cf_error::invalid_bool('send_email');
    return FALSE;
  }

  global $user;

  $new_status = NULL;
  $new_step = NULL;

  $system_user = user_load(1);
  $changed_user = $user;
  $changed_by = $user->uid;
  $changed_type = 'reviewer';

  // Venue Available
  if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
    if ($decision == MFCS_REVIEW_DECISION_APPROVE) {
      if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING) {
        // quick meetings bypass the review process.
        $new_status = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
        $new_step = MFCS_REVIEW_STEP_COMPLETED;
      }
      else {
        $new_status = MFCS_REQUEST_STATUS_LOCKED;
        $new_step = MFCS_REVIEW_STEP_REVIEW;
      }
    }
    elseif ($decision == MFCS_REVIEW_DECISION_DENY) {
      $new_status = MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE;
      $new_step = MFCS_REVIEW_STEP_COMPLETED;
    }
  }
  // Review Step
  elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
    if ($decision == MFCS_REVIEW_DECISION_DENY || $decision == MFCS_REVIEW_DECISION_APPROVE || $decision == MFCS_REVIEW_DECISION_MANAGER_RECHECK) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = MFCS_REVIEW_STEP_REQUIREMENTS;
    }
  }
  // Requirements Step
  elseif ($step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    if ($decision == MFCS_REVIEW_DECISION_DENY || $decision == MFCS_REVIEW_DECISION_APPROVE || $decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_MANAGER_RECHECK) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = MFCS_REVIEW_STEP_MAKE_DECISIONS;
    }
  }
  // Venue Coordinator (Final) Review Step
  elseif ($step == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
    $message = '';

    if ($decision == MFCS_REVIEW_DECISION_APPROVE) {
      $new_status = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED;
      $new_step = MFCS_REVIEW_STEP_COMPLETED;
    }
    elseif ($decision == MFCS_REVIEW_DECISION_DENY) {
      $new_status = MFCS_REQUEST_STATUS_CLOSED_DENIED;
      $new_step = MFCS_REVIEW_STEP_COMPLETED;
    }

    $reviews = mfcs_get_latest_reviews($request_id, array(MFCS_REVIEW_STEP_REVIEW), array(MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY));

    if (!empty($reviews)) {
      mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

      $classification_options = mfcs_get_reviewer_classification_list_options(NULL, TRUE);
      $preprocessed_decisions = array();

      $reviews_step = array();
      if (isset($reviews[MFCS_REVIEW_STEP_REVIEW])) {
        $reviews_step = $reviews[MFCS_REVIEW_STEP_REVIEW];
      }

      if (!empty($reviews_step)) {
        foreach ($reviews_step as $reviewer_classification => $reviewer_decisions) {
          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          $most_recent_decision = array_shift($reviewer_decisions);
          if ($most_recent_decision->decision == MFCS_REVIEW_DECISION_APPROVE) {
            $message .= "Approved";
          }
          else {
            $message .= "Denied";
          }

          $message .= " - " . $classification_options[$reviewer_classification] . "\n";
        }
      }
    }
  }

  if (!is_null($new_status)) {
    mfcs_workflow_set_status($request_id, $new_status, $changed_user);
  }

  if (!is_null($new_step)) {
    mfcs_workflow_set_step($request_id, $new_step, $changed_user);

    #if ($new_step == MFCS_REVIEW_STEP_COMPLETED && $type != MFCS_REQUEST_TYPE_QUICK_MEETING) {
    #  mfcs_workflow_step_review($request_id, $changed_user, MFCS_REVIEW_DECISION_CLOSED);
    #}
  }


  // send out e-mails.
  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array();
  $parameters['changed_by'] = $changed_by;
  $parameters['changed_type'] = $changed_type;
  $parameters['message'] = $message;

  if (!is_null($new_status) && $new_status != MFCS_REQUEST_STATUS_LOCKED) {
    $parameters['status'] = $new_status;
  }
  elseif ($new_step == MFCS_REVIEW_STEP_REVIEW || $new_step == MFCS_REVIEW_STEP_REQUIREMENTS || $new_step == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
    $parameters['step'] = $new_step;
  }
  elseif ($new_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $type == MFCS_REQUEST_TYPE_QUICK_MEETING && ($decision == MFCS_REVIEW_DECISION_APPROVE || $decision == MFCS_REVIEW_DECISION_DENY)) {
    $parameters['step'] = $new_step;
  }
  else {
    return TRUE;
  }

  if ($send_email) {
    mfcs_send_workflow_emails($request_id, $parameters);
  }

  return TRUE;
}

/**
 * Sends e-mails about certain requests.
 *
 * @param int $request_id
 *   The Request ID.
 * @param arary $parameters
 *   Array with the following keys:
 *   - changed_by: The Reviewer ID or User ID who changed the request.
 *   - changed_type: Either 'reviewer', 'user', or 'system'.
 *   - message: A message to display in regards to the decision.
 *   - status: A taxonomy number representing the previous request status.
 *     If NULL, then there is no old status or it is not to be processed.
 *   - step: A boolean representing whether or not to process as a new step.
 *   - new: A boolean representing whether or not this is new.
 *   - update: A boolean representing whether or not this is updated.
 *   - comment: A boolean representing whether or not this is a comment.
 *   - cancelled: A boolean representing whether or not the request is
 *     cancelled.
 *   - uncancelled: A boolean representing whether or not the request is
 *     uncancelled.
 *   - reassigned: A boolean representing whether or not the request is
 *     re-assigned to a new venue coordinator.
 *   - reassigned_from: user id of the old venue coordinator.
 *   - reassigned_to: user id of the new venue coordinator.
 *   - amended: A boolean representing whether or not the request is an
 *     amendment.
 *   - insurance_provided: A boolean representing whether or not to send a
 *     message about insurance being provided for the request.
 *   - reminder: a boolean representing whether or not to send a message
 *     to remind the request coordinator about something.
 *   - reminder_reason: A string representing what the reminder is about.
 *     Current strings are: 'insurance_provided'.
 *   - reminder_data: An array of values needed by a particular reminder
 *     reason.
 *
 *   The keys: 'new', 'update', 'step', 'status', 'cancelled',
 *   'uncancelled', 'amendment', 'reminder', and 'reassigned' are mutually
 *   exclusive.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_send_workflow_emails($request_id, $parameters) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    return FALSE;
  }

  if (!is_array($parameters)) {
    cf_error::invalid_array('parameters');
    return FALSE;
  }

  if (!array_key_exists('changed_by',  $parameters) || !cf_is_integer($parameters['changed_by'])) {
    cf_error::invalid_integer('parameters[changed_by]');
    return FALSE;
  }

  if (!array_key_exists('changed_type',  $parameters) || !is_string($parameters['changed_type'])) {
    cf_error::invalid_string('parameters[changed_type]');
    return FALSE;
  }

  $instance = mfcs_instance();

  $reviewer = NULL;
  $changed_by = NULL;
  $changed_by_string = NULL;
  if ($parameters['changed_type'] == 'reviewer') {
    $changed_by = user_load($parameters['changed_by']);
  }
  elseif ($parameters['changed_type'] == 'user' && $parameters['changed_by'] != 1) {
    $changed_by = user_load($parameters['changed_by']);
  }
  elseif ($parameters['changed_type'] == 'system' || ($parameters['changed_type'] == 'user' && $parameters['changed_by'] == 1)) {
    $changed_by_string = 'The System';
  }
  else {
    cf_error::invalid_string('parameters[changed_type]');
    return FALSE;
  }

  if (!is_null($changed_by)) {
    $changed_by_string = '';

    if (!empty($changed_by->field_user_first_name['und'][0]['value'])) {
      $changed_by_string .= $changed_by->field_user_first_name['und'][0]['value'];

      if (!empty($changed_by->field_user_last_name['und'][0]['value'])) {
        $changed_by_string .= ' ' . $changed_by->field_user_last_name['und'][0]['value'];
      }

      $changed_by_string .= " (user id: " . $changed_by->uid . ")";
    }
    else {
      $changed_by_string .= $changed_by->name . " (user id: " . $changed_by->uid . ")";
    }
  }

  unset($changed_by);

  $message = NULL;
  if (array_key_exists('message',  $parameters)) {
    $message = $parameters['message'];
    if (!is_string($message)) {
      cf_error::invalid_string('parameters[message]');
      return FALSE;
    }
  }

  $status = NULL;
  if (array_key_exists('status',  $parameters)) {
    $status = $parameters['status'];
    if (!cf_is_integer($status)) {
      cf_error::invalid_integer('parameters[status]');
      return FALSE;
    }
  }

  $step = NULL;
  if (array_key_exists('step',  $parameters)) {
    $step = $parameters['step'];
    if (!cf_is_integer($step)) {
      cf_error::invalid_integer('parameters[step]');
      return FALSE;
    }
  }

  $is_comment = FALSE;
  if (array_key_exists('comment',  $parameters)) {
    $is_comment = $parameters['comment'];
    if (!is_bool($is_comment)) {
      cf_error::invalid_bool('parameters[comment]');
      return FALSE;
    }
  }

  $is_new = FALSE;
  if (array_key_exists('new',  $parameters)) {
    $is_new = $parameters['new'];
    if (!is_bool($is_new)) {
      cf_error::invalid_bool('parameters[is_new]');
      return FALSE;
    }
  }

  $is_update = FALSE;
  if (array_key_exists('update',  $parameters)) {
    $is_update = $parameters['update'];
    if (!is_bool($is_update)) {
      cf_error::invalid_bool('parameters[update]');
      return FALSE;
    }
  }

  $is_amendment = FALSE;
  if (array_key_exists('amendment',  $parameters)) {
    $is_amendment = $parameters['amendment'];
    if (!is_bool($is_amendment)) {
      cf_error::invalid_bool('parameters[amendment]');
      return FALSE;
    }
  }

  $is_cancelled = FALSE;
  if (array_key_exists('cancelled',  $parameters)) {
    $is_cancelled = $parameters['cancelled'];
    if (!is_bool($is_cancelled)) {
      cf_error::invalid_bool('parameters[cancelled]');
      return FALSE;
    }
  }

  $is_auto_cancelled = FALSE;
  if (array_key_exists('auto-cancelled',  $parameters)) {
    $is_auto_cancelled = $parameters['auto-cancelled'];
    if (!is_bool($is_cancelled)) {
      cf_error::invalid_bool('parameters[auto-cancelled]');
      return FALSE;
    }
  }

  $is_uncancelled = FALSE;
  if (array_key_exists('uncancelled',  $parameters)) {
    $is_uncancelled = $parameters['uncancelled'];
    if (!is_bool($is_uncancelled)) {
      cf_error::invalid_bool('parameters[uncancelled]');
      return FALSE;
    }
  }

  $is_reassigned = FALSE;
  $reassigned_from = NULL;
  $reassigned_to = NULL;
  if (array_key_exists('reassigned',  $parameters)) {
    $is_reassigned = $parameters['reassigned'];
    if (!is_bool($is_reassigned)) {
      cf_error::invalid_bool('parameters[reassigned]');
      return FALSE;
    }
  }

  if ($is_reassigned) {
    if (!array_key_exists('reassigned_from',  $parameters) || !cf_is_integer($parameters['reassigned_from'])) {
      cf_error::invalid_bool('parameters[reassigned_from]');
      return FALSE;
    }

    if (!array_key_exists('reassigned_to',  $parameters) || !cf_is_integer($parameters['reassigned_to'])) {
      cf_error::invalid_bool('parameters[reassigned_to]');
      return FALSE;
    }

    $reassigned_from = $parameters['reassigned_from'];
    $reassigned_to = $parameters['reassigned_to'];
  }

  $is_insurance_provided = FALSE;
  if (array_key_exists('insurance_provided',  $parameters)) {
    $is_insurance_provided = $parameters['insurance_provided'];
    if (!is_bool($is_insurance_provided)) {
      cf_error::invalid_bool('parameters[insurance_provided]');
      return FALSE;
    }
  }

  $is_reminder = FALSE;
  if (array_key_exists('reminder',  $parameters)) {
    $is_reminder = $parameters['reminder'];
    if (!is_bool($is_reminder)) {
      cf_error::invalid_bool('parameters[reminder]');
      return FALSE;
    }

    if (empty($parameters['reminder_reason']) || !is_string($parameters['reminder_reason'])) {
      cf_error::invalid_string('parameters[reminder_reason]');
    }

    if (!array_key_exists('reminder_data', $parameters) || !is_object($parameters['reminder_data'])) {
      cf_error::invalid_object('parameters[reminder_data]');
    }
  }

  $is_deleted = FALSE;
  if (array_key_exists('deleted',  $parameters)) {
    $is_deleted = $parameters['deleted'];
    if (!is_bool($is_deleted)) {
      cf_error::invalid_bool('parameters[deleted]');
      return FALSE;
    }
  }

  $filename = NULL;
  if (array_key_exists('filename',  $parameters)) {
    $filename = $parameters['filename'];
    if (!is_string($filename)) {
      cf_error::invalid_string('parameters[filename]');
      return FALSE;
    }
  }

  global $base_url;

  $request_view_url = $base_url . '/requests/view-0/' . $request_id;

  $request_requester = mfcs_load_basic_user_settings_by_user_id($request['mer']['user_id'][0]->value);
  if (!is_array($request_requester)) {
    watchdog(MFCS_WATCHDOG_ID, "%request_requester is not a valid user id.", array('%request_requester' => $request['mer']['user_id'][0]->value), WATCHDOG_ERROR);

    // temporary fallback (@todo: implement proper fallback).
    $request_requester = array(
      'user_id' => (int) $request['mer']['user_id'][0]->value,
      'email' => NULL,
    );
  }

  // load appropriate reviewers in their own categories.
  $venue_coordinator = mfcs_load_basic_user_settings_by_user_id($request['venue_coordinator']['user_id'][0]->value);
  if (!is_array($venue_coordinator)) {
    watchdog(MFCS_WATCHDOG_ID, "The supplied venue coordinator id for request %request_id is not a valid user id.", array('%request_id' => $request_id), WATCHDOG_ERROR);

    // temporary fallback (@todo: implement proper fallback).
    $venue_coordinator = array(
      'user_id' => $request['venue_coordinator']['user_id'][0]->value,
      'email' => NULL,
    );
  }

  $reviewers_returned = mfcs_get_reviewers(array('mr.request_classification' => $request['request_coordinator']['classification'][0]->value, 'mr.review_step' => MFCS_REVIEW_STEP_REVIEW, 'mr.disabled' => 0), array('join' => TRUE));
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  $step_1_reviewers = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $r) {
      $step_1_reviewers[$r->user_id] = $r;
    }
  }

  $reviewers_returned = mfcs_get_reviewers(array('mr.request_classification' => $request['request_coordinator']['classification'][0]->value, 'mr.review_step' => MFCS_REVIEW_STEP_REQUIREMENTS, 'mr.disabled' => 0), array('join' => TRUE));
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  $step_2_reviewers = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $r) {
      $step_2_reviewers[$r->user_id] = $r;
    }
  }

  // prepare e-mail(s) for sending
  $fcs_email = variable_get('fcs_email', 'facilities_use@fcs.mcneese.edu');
  $fcs_name = variable_get('fcs_name', 'McNeese Facilities Use System');

  $email_params = array();
  $email_params['request_id'] = $request_id;
  $email_params['from'] = $fcs_email;

  $email_to = array();

  if (isset($parameters['message_priority'])) {
    $email_params['message_priority'] = $parameters['message_priority'];
  }

  $email_from = '"' . $fcs_name . '" <' . $fcs_email . '>';

  $location_text = '';
  $room_text = '';
  $building_text = '';

  $location = mfcs_load_locations($request['top']['location'][0]->value);

  if (is_object($location)) {
    $location_text = $location->location_name;
  }

  $room = mfcs_load_rooms(NULL, NULL, $request['top']['room'][0]->value);

  if (is_object($room)) {
    $room_text = $room->room_name;
    $building = mfcs_load_buildings(NULL, $room->building_id);

    if (is_object($building)) {
      $building_text = $building->building_name;
    }
  }

  $standard_content = "\n\n";
  $standard_content .= "Request ID:\n - " . $request_id . "\n\n";
  $standard_content .= "Request Name:\n - " . $request['information']['title'][0]->value . "\n\n"; // renamed from title to "Request Name" as per fcs-23.
  $standard_content .= "Location:\n - " . $location_text . "\n\n";
  $standard_content .= "Building:\n - " . $building_text . "\n\n";
  $standard_content .= "Room:\n - " . $room_text . "\n\n";
  $standard_content .= "Dates:\n";

  $standard_markup = "<br>\n<br>\n";
  $standard_markup .= "<strong>Request ID</strong>:\n<ul><li>" . $request_id . "</li></ul>\n<br>\n";
  $standard_markup .= "<strong>Request Name</strong>:\n<ul><li>" . $request['information']['title'][0]->value . "</li></ul>\n<br>\n"; // renamed from title to "Request Name" as per fcs-23.
  $standard_markup .= "<strong>Location</strong>:\n<ul><li>" . $location_text . "</li></ul>\n<br>\n";
  $standard_markup .= "<strong>Building</strong>:\n<ul><li>" . $building_text . "</li></ul>\n<br>\n";
  $standard_markup .= "<strong>Room</strong>:\n<ul><li>" . $room_text . "</li></ul>\n<br>\n";
  $standard_markup .= "<strong>Dates</strong>:\n";

  foreach ($request['dates']['date'] as $key => $date) {
    $standard_content .= " - " . date("Y/m/d", $date->value);
    $standard_markup .= "<ul><li>" . date("Y/m/d", $date->value);

    if (!is_null($request['dates']['time_start'][$key]->value)) {
      $standard_content .= " " . date("h:ia", $request['dates']['time_start'][$key]->value);
      $standard_content .= " to " . date("h:ia", $request['dates']['time_stop'][$key]->value);

      $standard_markup .= " " . date("h:ia", $request['dates']['time_start'][$key]->value);
      $standard_markup .= " to " . date("h:ia", $request['dates']['time_stop'][$key]->value);
    }

    $standard_content .= "\n";
    $standard_markup .= "</li></ul>\n";
  }

  $email_params['request_title'] = $request['information']['title'][0]->value;

  if ($is_new) {
    $type = $request['information']['type'][0]->value;

    if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && ($venue_coordinator['user_id'] == $request_requester['user_id'] || user_access('mfcs manage', $request_requester))) {
      $extra = array(
        'send_email' => FALSE,
        'requirements' => array('other' => 0, 'waived' => 0, 'university' => 1),
      );
      mfcs_workflow_step_review($request_id, $request_requester, MFCS_REVIEW_DECISION_REQUIREMENT, "", $extra);

      $extra = array(
        'send_email' => FALSE,
      );
      mfcs_workflow_step_review($request_id, $request_requester, MFCS_REVIEW_DECISION_APPROVE, "", $extra);

      #mfcs_workflow_step_review($request_id, $request_requester, MFCS_REVIEW_DECISION_CLOSED);

      $email_params['title_suffix'] = "Approved";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been approved.\n";
      $email_params['content'] .= "For more information, view the review log.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been approved.<br>\n";
      $email_params['markup'] .= "For more information, view the <a href=" . '"' . $request_view_url . '/' . MFCS_VIEW_MODE_LOG . '"' . ">review log</a>.<br>\n";
      $email_params['markup'] .= $standard_markup;

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }
    }
    elseif ($status == MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK) {
      $email_params['title_suffix'] = "Unavailable (due to lock)";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has created this request, but the request was auto-closed due to an existing lock for the specified room, date, and time.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= $changed_by_string . " has created this request, but the request was auto-closed due to an existing lock for the specified room, date, and time.<br>\n";
      $email_params['markup'] .= $standard_markup;

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }
    }
    elseif ($status == MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE) {
      $email_params['title_suffix'] = "Unavailable";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has created this request, but the request was auto-closed due the requested room, date, and time being already reserved.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= $changed_by_string . " has created this request, but the request was auto-closed due the requested room, date, and time being already reserved.<br>\n";
      $email_params['markup'] .= $standard_markup;

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }
    }
    else {
      $email_params['title_suffix'] = "Requested";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has created this request.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= $changed_by_string . " has created this request.<br>\n";
      $email_params['markup'] .= $standard_markup;

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }
    }
  }
  elseif ($is_update) {
    $email_params['title_suffix'] = "Modified";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has modified this request.\n";
    $email_params['content'] .= $standard_content;
    $email_params['markup'] = "";
    $email_params['markup'] .= $changed_by_string . " has modified this request.<br>\n";
    $email_params['markup'] .= $standard_markup;

    // the update message only needs to be sent during the review process.
    if ($status == MFCS_REQUEST_STATUS_LOCKED || $status == MFCS_REQUEST_STATUS_UNLOCKED) {
      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }

      if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW) {
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }

      if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
  }
  elseif ($is_amendment) {
    $email_params['title_suffix'] = "Amended";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has amended this request.\n";
    $email_params['content'] .= "The review process has been restarted.\n";
    $email_params['content'] .= $standard_content;
    $email_params['markup'] = "";
    $email_params['markup'] .= $changed_by_string . " has amended this request.<br>\n";
    $email_params['markup'] .= "The review process has been restarted.<br>\n";
    $email_params['markup'] .= $standard_markup;

    // amendments can only happen while the request is locked.
    if ($status == MFCS_REQUEST_STATUS_LOCKED) {
      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }

      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
  }
  elseif ($is_insurance_provided) {
    $email_params['title_suffix'] = "Insurance Provided";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has designated that the insurance has been provided for this request.\n";
    $email_params['content'] .= "\n";
    $email_params['content'] .= $message . "\n";
    $email_params['content'] .= $standard_content;
    $email_params['markup'] = "";
    $email_params['markup'] .= $changed_by_string . " has designated that the insurance has been provided for this request.<br>\n";
    $email_params['markup'] .= "<br>\n";
    $email_params['markup'] .= $message . "<br>\n";
    $email_params['markup'] .= $standard_markup;

    if (is_array($venue_coordinator)) {
      $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
    }

    // also send e-mails to the venue coordinator proxies.
    $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
    if (!empty($proxies_returned['results'])) {
      foreach ($proxies_returned['results'] as $proxy_returned) {
        $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

        if (is_array($proxy)) {
          $email_to[$proxy['email']] = $proxy['email'];
        }
      }
    }

    foreach ($step_1_reviewers as $r) {
      $email_to[$r->email] = $r->email;
    }

    foreach ($step_2_reviewers as $r) {
      $email_to[$r->email] = $r->email;
    }
  }
  elseif ($is_reminder) {
    if ($parameters['reminder_reason'] == 'insurance_provided') {
      $request_title = 'Request';
      if (!empty($parameters['reminder_data']->title)) {
        $request_title = $parameters['reminder_data']->title;
      }

      $remaining_content = '';
      $remaining_markup = '';

      $current_date = date('l, F jS, Y', $instance);
      if (isset($parameters['reminder_data']->interval)) {
        if ($parameters['reminder_data']->interval == MFCS_REMINDER_INTERVAL_DAY_30) {
          $remaining_content = "\nThis is the 30-day reminder e-mail submitted on: " . $current_date . ".\n";
          $remaining_markup = "<br>\nThis is the 30-day reminder e-mail submitted on: " . $current_date . ").<br>\n";
        }
      }

      $email_params['title_suffix'] = "Insurance Required";
      $email_params['content'] = "";
      $email_params['content'] .= "This is a reminder that you have not yet provided the insurance for the request: " . check_plain($request_title) . ".\n";
      $email_params['content'] .= $remaining_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "This is a reminder that you have not yet provided the insurance for the request: <em>" . check_plain($request_title) . "</em>.<br>\n";
      $email_params['markup'] .= $remaining_markup;
      $email_params['markup'] .= "<br>\n";
      $email_params['markup'] .= $standard_markup;

      if (!empty($parameters['reminder_data']->coordinator_email)) {
        $email_to[$parameters['reminder_data']->coordinator_email] = $parameters['reminder_data']->coordinator_email;
      }
    }
    else {
      // no known reminder_reason was provided.
      return FALSE;
    }
  }
  elseif ($is_cancelled || $is_auto_cancelled || $is_uncancelled || $is_reassigned || $is_deleted || !is_null($status)) {
    if ($is_cancelled || $is_uncancelled || $is_reassigned || $status == MFCS_REQUEST_STATUS_CANCELLED || $status == MFCS_REQUEST_STATUS_DELETED) {
      $email_params['content'] = "";
      $email_params['markup'] = "";

      if ($is_cancelled || $is_auto_cancelled || $status == MFCS_REQUEST_STATUS_CANCELLED) {
        $email_params['title_suffix'] = "Cancelled";
        $email_params['content'] .= $changed_by_string . " has cancelled the request.\n";
        $email_params['markup'] .= $changed_by_string . " has cancelled the request.<br>\n";

        if ($is_auto_cancelled) {
          $email_params['title_suffix'] = "Cancelled (Auto)";
          $email_params['content'] .= "The request was auto-cancelled";

          if (empty($parameters['auto-cancelled-reason'])) {
            $email_params['content'] .= ".\n";
            $email_params['markup'] .= ".<br>\n";
          }
          else {
            $email_params['content'] .= " because: " . $parameters['auto-cancelled-reason'] . "\n";
            $email_params['markup'] .= " because: " . $parameters['auto-cancelled-reason'] . "<br>\n";
          }
        }
      }
      elseif ($is_uncancelled) {
        $email_params['title_suffix'] = "Uncancelled";
        $email_params['content'] .= $changed_by_string . " has uncancelled the request.\n";
        $email_params['markup'] .= $changed_by_string . " has uncancelled the request.<br>\n";
      }
      elseif ($is_reassigned) {
        $reassigned_from_user = mfcs_load_basic_user_settings_by_user_id($reassigned_from);
        $reassigned_to_user = mfcs_load_basic_user_settings_by_user_id($reassigned_to);

        $reassigned_from_user_name = $reassigned_from_user['name'];
        if (!empty($reassigned_from_user['first_name']) && !empty($reassigned_from_user['last_name'])) {
          $reassigned_from_user_name = $reassigned_from_user['first_name'] . ' ' . $reassigned_from_user['last_name'];
        }

        $reassigned_to_user_name = $reassigned_to_user['name'];
        if (!empty($reassigned_to_user['first_name']) && !empty($reassigned_to_user['last_name'])) {
          $reassigned_to_user_name = $reassigned_to_user['first_name'] . ' ' . $reassigned_to_user['last_name'];
        }

        $email_params['title_suffix'] = "Re-Assigned";
        $email_params['content'] .= $changed_by_string . " has re-assigned the venue coordinator of the request from " . $reassigned_from_user_name . " [id: " . $reassigned_from . "] to " . $reassigned_to_user_name . " [id: " . $reassigned_to . "].\n";
        $email_params['markup'] .= $changed_by_string . " has re-assigned the venue coordinator of the request from <strong>" . $reassigned_from_user_name . "</strong> <sup>[id: " . $reassigned_from . "]</sup> to <strong>" . $reassigned_to_user_name . "</strong> <sup>[id: " . $reassigned_to . "]</sup>.<br>\n";
      }
      elseif ($is_deleted || $status == MFCS_REQUEST_STATUS_DELETED) {
        $email_params['title_suffix'] = "Deleted";
        $email_params['content'] .= $changed_by_string . " has deleted the request.\n";
        $email_params['markup'] .= $changed_by_string . " has deleted the request.<br>\n";

        if (!empty($parameters['deleted-message'])) {
          $email_params['content'] .= "\n" . $parameters['deleted-message'] . "\n";
          $email_params['markup'] .= "<br>\n" . $parameters['deleted-message'] . "<br>\n";
        }
      }

      $email_params['content'] .= $standard_content;
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester['email']] = $request_requester['email'];

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }

      if ($status != MFCS_REQUEST_STATUS_DELETED) {
        // send messages to all reviewers.
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }

        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
    elseif ($status == MFCS_REQUEST_STATUS_CLOSED_ACCEPTED) {
      $email_params['title_suffix'] = "Approved";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been approved.\n";
      $email_params['content'] .= "For more information, view the review log.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been approved.<br>\n";
      $email_params['markup'] .= "For more information, view the <a href=" . '"' . $request_view_url . '/' . MFCS_VIEW_MODE_LOG . '"' . ">review log</a>.<br>\n";
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester['email']] = $request_requester['email'];

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }

      // send messages to all reviewers.
      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      mfcs_include(MFCS_INCLUDE_ICAL);

      $requests = mfcs_ical_generate_array_from_id($request_id);

      if (is_array($requests)) {
        $ical = mfcs_ical_generate($requests);

        if ($ical !== FALSE) {
          if (!isset($email_params['files'])) {
            $email_params['files'] = array();
          }

          $file = array(
            'type' => 'ical',
            'mime' => 'text/calendar',
            'name' => 'mcneese-facilities_use-request-' . $request_id . '.ics',
            'data' => $ical,
            'disposition' => 'attachment',
          );

          $email_params['files'][] = $file;
        }
      }
    }
    elseif ($status == MFCS_REQUEST_STATUS_CLOSED_DENIED) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been denied.\n";
      $email_params['content'] .= "For more information, view the review log.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been denied.<br>\n";
      $email_params['markup'] .= "For more information, view the <a href=" . '"' . $request_view_url . '/' . MFCS_VIEW_MODE_LOG . '"' . ">review log</a>.<br>\n";
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester['email']] = $request_requester['email'];

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }

      // send messages to all reviewers.
      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($status == MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE || $status == MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been denied due to the requested location not being available for the requested time slots.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been <em>denied</em> due to the requested location <em>not being available</em> for the requested time slots.<br>\n";
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester['email']] = $request_requester['email'];

      if ($status == MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK) {
        if (is_array($venue_coordinator)) {
          $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
        }

        // also send e-mails to the venue coordinator proxies.
        $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
        if (!empty($proxies_returned['results'])) {
          foreach ($proxies_returned['results'] as $proxy_returned) {
            $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

            if (is_array($proxy)) {
              $email_to[$proxy['email']] = $proxy['email'];
            }
          }
        }
      }
    }
  }
  elseif (!is_null($step)) {
    if ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW) {
      $email_params['title_suffix'] = "Needs Review";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has made the request available for review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= $changed_by_string . " has made the request available for <em>review</em>.<br>\n";
      $email_params['markup'] .= "<br>\n";
      $email_params['markup'] .= $message . "<br>\n";
      $email_params['markup'] .= $standard_markup;

      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }
    }
    elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REQUIREMENTS) {
      $email_params['title_suffix'] = "Requirements";
      $email_params['content'] = "";
      $email_params['content'] .= "The request is ready to have its requirements approved or denied.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request is ready to have its requirements <em>approved or denied</em>.<br>\n";
      $email_params['markup'] .= "<br>\n";
      $email_params['markup'] .= $message . "<br>\n";
      $email_params['markup'] .= $standard_markup;

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['email'];
          }
        }
      }
    }
    elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
      $email_params['title_suffix'] = "Reviewed";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been reviewed and is ready for conflict resolution / decision making / final review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been reviewed and is ready for <em>conflict resolution / decision making / final review</em>.<br>\n";
      $email_params['markup'] .= "<br>\n";
      $email_params['markup'] .= $message . "<br>\n";
      $email_params['markup'] .= $standard_markup;

      if (is_array($venue_coordinator)) {
        $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

          if (is_array($proxy)) {
            $email_to[$proxy['email']] = $proxy['smail'];
          }
        }
      }
    }
  }
  elseif ($is_comment) {
    $email_params['title_suffix'] = "Comment";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has made the following comment:\n";
    $email_params['content'] .= "\n";
    $email_params['content'] .= $message . "\n";
    $email_params['content'] .= $standard_content;
    $email_params['markup'] = "";
    $email_params['markup'] .= '<em>' . $changed_by_string . "</em> has made the following <em>comment</em>:<br>\n";
    $email_params['markup'] .= "<br>\n";
    $email_params['markup'] .= $message . "<br>\n";
    $email_params['markup'] .= $standard_markup;

    if (is_array($venue_coordinator)) {
      $email_to[$venue_coordinator['email']] = $venue_coordinator['email'];
    }

    // also send e-mails to the venue coordinator proxies.
    $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator['user_id'], 'mpvc.disabled' => 0));
    if (!empty($proxies_returned['results'])) {
      foreach ($proxies_returned['results'] as $proxy_returned) {
        $proxy = mfcs_load_basic_user_settings_by_user_id($proxy_returned->proxy_id);

        if (is_array($proxy)) {
          $email_to[$proxy['email']] = $proxy['email'];
        }
      }
    }

    foreach ($step_1_reviewers as $r) {
      $email_to[$r->email] = $r->email;
    }

    foreach ($step_2_reviewers as $r) {
      $email_to[$r->email] = $r->email;
    }
  }

  // for testing, add/remove email addresses
  global $conf;

  $testing = isset($conf['fcs_testing']) && $conf['fcs_testing'];
  $debugging_email = isset($conf['fcs_testing-email_to-debug']) && $conf['fcs_testing-email_to-debug'];

  // when testing, prepend the testing prefix to e-mails to distinguish between production and non-production e-mails.
  if ($testing) {
    if (isset($conf['fcs_testing-email_to-prefix']) && is_string($conf['fcs_testing-email_to-prefix'])) {
      $email_params['title_prefix'] = $conf['fcs_testing-email_to-prefix'];
    }
  }

  if ($testing && $debugging_email) {
    $debug_accounts = array();

    if (isset($conf['fcs_testing-email_to-add']) && is_array($conf['fcs_testing-email_to-add'])) {
      foreach ($conf['fcs_testing-email_to-add'] as $add_email) {
        $email_to[$add_email] = $add_email;
        $debug_accounts[$add_email] = $add_email;
      }
    }

    if (isset($conf['fcs_testing-email_to-remove']) && is_array($conf['fcs_testing-email_to-remove'])) {
      foreach ($conf['fcs_testing-email_to-remove'] as $remove_email) {
        unset($email_to[$remove_email]);
        unset($debug_accounts[$remove_email]);
      }
    }

    foreach ($email_to as $to) {
      if (!in_array($to, $debug_accounts)) {
        drupal_set_message(t("Warning: not sending mail to @to while testing the system.", array('@to' => $to)), 'warning');
        #watchdog('debug', "Warning: not sending mail to @to while testing the system.", array('@to' => $to), WATCHDOG_WARNING);
        continue;
      }

      drupal_mail('mfcs', 'request_alert', $to, language_default(), $email_params, $email_from, TRUE);
    }
  }
  else {
    foreach ($email_to as $to) {
      drupal_mail('mfcs', 'request_alert', $to, language_default(), $email_params, $email_from, TRUE);
    }
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
