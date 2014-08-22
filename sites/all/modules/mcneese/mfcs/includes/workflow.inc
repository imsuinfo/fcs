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
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
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

  mfcs_include(4);

  $step_options = mfcs_get_review_step_list_options();

  if (!array_key_exists($step, $step_options)) {
    watchdog(MFCS_WATCHDOG_ID, "%step is not a valid step.", array('%step' => $step), WATCHDOG_ERROR);
    return FALSE;
  }

  $mfcs_requests = array(
    'updated' => REQUEST_TIME,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . REQUEST_TIME, FALSE);

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
    'date' => REQUEST_TIME,
    'revision' => $request_revision_next,
  );

  $step_revision = $request['current']['top']['step'] + 1;

  $mfcs_current_top = array(
    'step' => $step_revision,
  );

  $mfcs_field_top_step = array(
    'request_id' => $request_id,
    'revision' => $step_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
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

    $query = db_update('mfcs_current_top');
    $query->condition('request_id', $request_id);
    $query->fields($mfcs_current_top);
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
  mfcs_load_request_by_id($request_id, TRUE);

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
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_set_status($request_id, $status, $user = NULL) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
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

  $status_options = array(
    MFCS_EVENT_STATUS_DELETED,
    MFCS_EVENT_STATUS_LOCKED,
    MFCS_EVENT_STATUS_UNLOCKED,
    MFCS_EVENT_STATUS_CLOSED_ACCEPTED,
    MFCS_EVENT_STATUS_CLOSED_DENIED,
    MFCS_EVENT_STATUS_CLOSED_UNAVAILABLE,
    MFCS_EVENT_STATUS_CLOSED_DUE_TO_LOCK,
    MFCS_EVENT_STATUS_CANCELLED,
  );

  if (!in_array($status, $status_options)) {
    watchdog(MFCS_WATCHDOG_ID, "%status is not a valid status.", array('%status' => $status), WATCHDOG_ERROR);
    return FALSE;
  }

  $mfcs_requests = array(
    'updated' => REQUEST_TIME,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . REQUEST_TIME, FALSE);

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
    'date' => REQUEST_TIME,
    'revision' => $request_revision_next,
  );

  $status_revision = $request['current']['top']['status'] + 1;

  $mfcs_current_top = array(
    'status' => $status_revision,
  );

  $mfcs_field_top_status = array(
    'request_id' => $request_id,
    'revision' => $status_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
    'value' => $status,
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

    $query = db_insert('mfcs_field_top_status');
    $query->fields($mfcs_field_top_status);
    $query->execute();

    $query = db_update('mfcs_current_top');
    $query->condition('request_id', $request_id);
    $query->fields($mfcs_current_top);
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
  mfcs_load_request_by_id($request_id, TRUE);

  // @todo: if it is the final step, accepted, etc..make sure to generate the PDF file and save it to the file system for remote storage.

  return TRUE;
}

/**
 * Trigger the next step.
 *
 * @param int $request_id
 *   The request id.
 * @param object $reviewer
 *   The reviewer.
 * @param int $step
 *   The request review step at the time of the decision.
 * @param int $decision
 *   The facilities use decision.
 * @param int $message
 *   The facilities use decision message.
 * @param array $extra
 *   (optional) Additional fields that are specific to the individual step.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_step_review($request_id, $user, $step, $decision, $message, $extra = array()) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_integer('user');
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

  if (!is_array($extra)) {
    cf_error::invalid_array('extra');
    return FALSE;
  }

  if (is_null($user)) {
    $user = cf_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    return FALSE;
  }


  $send_email = TRUE;
  if (isset($extra['send_email']) && $extra['send_email'] === FALSE) {
    $send_email = FALSE;
  }

  $request_classification = $request['request_coordinator']['classification'][0]->value;
  $step = $request['top']['step'][0]->value;
  $venue_coordinator_id = $request['venue_coordinator']['user_id'][0]->value;

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
    'updated' => REQUEST_TIME,
  );

  $request_revision = mfcs_load_request_revision_number($request_id);
  $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . REQUEST_TIME, FALSE);

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
    'date' => REQUEST_TIME,
    'revision' => $request_revision_next,
  );

  $mfcs_current_requirements = array(
    'request_id' => $request_id,
    'facilities' => NULL,
    'equipment' => NULL,
    'custodial' => NULL,
    'security' => NULL,
    'other' => NULL,
    'waved' => NULL,
    'university' => NULL,
  );

  $requirements_latest = $mfcs_current_requirements;
  unset($requirements_latest['request_id']);

  try {
    foreach ($requirements_latest as $requirement_name => &$requirement_latest_value) {
      if (array_key_exists($requirement_name, $request['current']['requirements']) && !is_null($request['current']['requirements'][$requirement_name])) {
        $mfcs_current_requirements[$requirement_name] = $request['current']['requirements'][$requirement_name];
        $requirement_latest_value = $mfcs_current_requirements[$requirement_name];
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
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  $insurance_requirements = FALSE;
  $insurance_provided_exception = FALSE;

  $mfcs_current_insurance = array(
    'request_id' => $request_id,
    'contractor' => NULL,
    'unaffiliated' => NULL,
    'provided' => NULL,
  );

  $insurance_latest = $mfcs_current_insurance;
  unset($insurance_latest['request_id']);

  if (!empty($request['current']['insurance'])) {
    if (array_key_exists('contractor', $request['current']['insurance']) && !is_null($request['current']['insurance']['contractor'])) {
      $mfcs_current_insurance['contractor'] = $request['current']['insurance']['contractor'];
    }

    if (array_key_exists('unaffiliated', $request['current']['insurance']) && !is_null($request['current']['insurance']['unaffiliated'])) {
      $mfcs_current_insurance['unaffiliated'] = $request['current']['insurance']['unaffiliated'];
    }

    if (array_key_exists('provided', $request['current']['insurance']) && !is_null($request['current']['insurance']['provided'])) {
      $mfcs_current_insurance['provided'] = $request['current']['insurance']['provided'];
    }
  }

  try {
    foreach ($insurance_latest as $insurance_name => &$insurance_latest_value) {
      if (array_key_exists($insurance_name, $request['current']['insurance']) && !is_null($request['current']['insurance'][$insurance_name])) {
        $mfcs_current_insurance[$insurance_name] = $request['current']['insurance'][$insurance_name];
        $insurance_latest_value = $mfcs_current_insurance[$insurance_name];
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
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  $mfcs_field_review_review = array(
    'request_id' => $request_id,
    'revision' => $review_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
    'step' => $step,
    'decision' => $decision,
    'message' => $message,
  );

  $mfcs_field_requirements = array();
  if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
    $other_requirement = 0;
    if (isset($extra['requirements']['other'])) {
      $other_requirement = $extra['requirements']['other'];
    }

    if (is_null($mfcs_current_requirements['other'])) {
      if (is_null($requirements_latest['other'])) {
        $mfcs_current_requirements['other'] = 0;
      }
      else {
        $mfcs_current_requirements['other'] = $requirements_latest['other'] + 1;
      }
    }
    else {
      $mfcs_current_requirements['other']++;
    }
    $mfcs_field_requirements['other'] = array(
      'request_id' => $request_id,
      'revision' => $mfcs_current_requirements['other'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => REQUEST_TIME,
      'value' => $other_requirement,
    );
  }

  $reviewer_conditions = array(
    'mr.user_id' => $user->uid,
    'mr.request_classification' => $request_classification,
    'mr.disabled' => 0,
  );

  $reviewer_classifications = array();
  $reviewer_classifications_all = array();
  $reviewers = mfcs_get_reviewers($reviewer_conditions, TRUE);
  if (!is_array($reviewers)) {
    $reviewers = array();
  }

  foreach ($reviewers as $reviewer) {
    if (isset($reviewer->review_step) && $reviewer->review_step == $step) {
      $reviewer_classifications[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
    }

    $reviewer_classifications_all[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
  }

  if ($venue_coordinator_id == $user->uid) {
    $reviewer_classifications[MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR] = MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR;
    $reviewer_classifications_all[MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR] = MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR;
  }

  if ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW  || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
    $uses = array();
    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if ($request['venue_coordinator']['user_id'][0]->value == $user->uid) {
        $uses['facilities'] = 'facilities';
        $uses['equipment'] = 'equipment';
      }
    }
    elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
      if (isset($reviewer_classifications[MFCS_REVIEW_CLASSIFICATION_FACILITIES])) {
        $uses['custodial'] = 'custodial';
      }

      if (isset($reviewer_classifications[MFCS_REVIEW_CLASSIFICATION_SECURITY])) {
        $uses['security'] = 'security';
      }
    }

    if (isset($reviewer_classifications_all[MFCS_REVIEW_CLASSIFICATION_PURCHASING])) {
      $insurance_provided_exception = TRUE;
      $reviewer_classifications[MFCS_REVIEW_CLASSIFICATION_PURCHASING] = MFCS_REVIEW_CLASSIFICATION_PURCHASING;
    }

    foreach ($uses as $use) {
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

      if (is_null($mfcs_current_requirements[$use])) {
        $mfcs_current_requirements[$use] = 0;
      }
      else {
        $mfcs_current_requirements[$use]++;
      }

      $mfcs_field_requirements[$use] = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_requirements[$use],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'quantity' => $requirements['quantity'],
        'hours' => $requirements['hours'],
        'days' => $requirements['days'],
        'amount' => $requirements['amount'],
      );

      unset($requirements);
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

      // contractor insurance
      if (is_null($mfcs_current_insurance['contractor'])) {
        if (is_null($insurance_latest['contractor'])) {
          $mfcs_current_insurance['contractor'] = 0;
        }
        else {
          $mfcs_current_insurance['contractor'] = $insurance_latest['contractor'] + 1;
        }
      }
      else {
        $mfcs_current_insurance['contractor']++;
      }
      $mfcs_field_insurance_contractor = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_insurance['contractor'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'value' => $contractor_insurance,
      );

      // unaffiliated insurance
      if (is_null($mfcs_current_insurance['unaffiliated'])) {
        if (is_null($insurance_latest['unaffiliated'])) {
          $mfcs_current_insurance['unaffiliated'] = 0;
        }
        else {
          $mfcs_current_insurance['unaffiliated'] = $insurance_latest['unaffiliated'] + 1;
        }
      }
      else {
        $mfcs_current_insurance['unaffiliated']++;
      }
      $mfcs_field_insurance_unaffiliated = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_insurance['unaffiliated'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'value' => $unaffiliated_insurance,
      );

      $mfcs_current_insurance = array(
        'request_id' => $request_id,
        'provided' => NULL,
      );
    }

    // provided insurance
    if ($insurance_requirements || $insurance_provided_exception) {
      $provided_insurance = 0;

      if (isset($extra['insurance']['provided'])) {
        $provided_insurance = $extra['insurance']['provided'];
      }

      if (is_null($mfcs_current_insurance['provided'])) {
        if (is_null($insurance_latest['provided'])) {
          $mfcs_current_insurance['provided'] = 0;
        }
        else {
          $mfcs_current_insurance['provided'] = $insurance_latest['provided'] + 1;
        }
      }
      else {
        $mfcs_current_insurance['provided']++;
      }
      $mfcs_field_insurance_provided = array(
        'request_id' => $request_id,
        'revision' => $mfcs_current_insurance['provided'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'value' => $provided_insurance,
      );
    }
  }

  if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
    $requirements_waved = 0;
    if (isset($extra['requirements']['waved'])) {
      $requirements_waved = $extra['requirements']['waved'];
    }

    if (is_null($mfcs_current_requirements['waved'])) {
      if (is_null($requirements_latest['waved'])) {
        $mfcs_current_requirements['waved'] = 0;
      }
      else {
        $mfcs_current_requirements['waved'] = $requirements_latest['waved'] + 1;
      }
    }
    else {
      $mfcs_current_requirements['waved']++;
    }

    $mfcs_field_requirements['waved'] = array(
      'request_id' => $request_id,
      'revision' => $mfcs_current_requirements['waved'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => REQUEST_TIME,
      'value' => $requirements_waved,
    );

    $requirements_university = 0;
    if (isset($extra['requirements']['university'])) {
      $requirements_university = $extra['requirements']['university'];
    }

    if (is_null($mfcs_current_requirements['university'])) {
      if (is_null($requirements_latest['university'])) {
        $mfcs_current_requirements['university'] = 0;
      }
      else {
        $mfcs_current_requirements['university'] = $requirements_latest['university'] + 1;
      }
    }
    else {
      $mfcs_current_requirements['university']++;
    }

    $mfcs_field_requirements['university'] = array(
      'request_id' => $request_id,
      'revision' => $mfcs_current_requirements['university'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => REQUEST_TIME,
      'value' => $requirements_university,
    );
  }

  // replace all remaining NULL values with 0.
  foreach ($mfcs_current_requirements as $key => &$value) {
    if (is_null($value)) {
      if (is_null($requirements_latest[$key])) {
        $value = 0;
      }
      else {
        $value = $requirements_latest[$key] + 1;
      }
    }
  }

  foreach ($mfcs_current_insurance as $key => &$value) {
    if (is_null($value)) {
      if (is_null($insurance_latest[$key])) {
        $value = 0;
      }
      else {
        $value = $insurance_latest[$key] + 1;
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
    }

    if ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW  || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
      foreach ($mfcs_field_requirements as $use => $fields) {
        $query = db_insert('mfcs_field_requirements_' . $use);
        $query->fields($fields);
        $query->execute();
      }

      $query = db_update('mfcs_current_requirements');
      $query->condition('request_id', $request_id);
      $query->fields($mfcs_current_requirements);
      $query->execute();

      if ($insurance_requirements) {
        $query = db_insert('mfcs_field_insurance_contractor');
        $query->fields($mfcs_field_insurance_contractor);
        $query->execute();

        $query = db_insert('mfcs_field_insurance_unaffiliated');
        $query->fields($mfcs_field_insurance_unaffiliated);
        $query->execute();
      }

      if ($insurance_requirements || $insurance_provided_exception) {
        $query = db_insert('mfcs_field_insurance_provided');
        $query->fields($mfcs_field_insurance_provided);
        $query->execute();
      }

      $query = db_update('mfcs_current_insurance');
      $query->condition('request_id', $request_id);
      $query->fields($mfcs_current_insurance);
      $query->execute();
    }
    elseif ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_WAVE) {
      $query = db_insert('mfcs_field_requirements_waved');
      $query->fields($mfcs_field_requirements['waved']);
      $query->execute();

      $query = db_insert('mfcs_field_requirements_university');
      $query->fields($mfcs_field_requirements['university']);
      $query->execute();

      $query = db_update('mfcs_current_requirements');
      $query->condition('request_id', $request_id);
      $query->fields($mfcs_current_requirements);
      $query->execute();
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
  mfcs_load_request_by_id($request_id, TRUE);

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
  elseif ($decision == MFCS_REVIEW_DECISION_WAVE || ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && !$insurance_provided_exception)) {
    // do not send an e-mail.
    return TRUE;
  }

  // continue to next step only if at least 1 of each reviewer classification has submitted a reply.
  $request_status = $request['top']['status'][0]->value;
  $request_step = $request['top']['step'][0]->value;
  $request_classification = $request['request_coordinator']['classification'][0]->value;

  if ($request_step == MFCS_REVIEW_STEP_REVIEW || $request_step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    $reviewers = mfcs_get_reviewers(array('mr.request_classification' => $request_classification, 'mr.review_step' => $request_step, 'mr.disabled' => 0));

    $required_reviews = 0;
    $reviewers_steps_by_classification = array();
    foreach ($reviewers as $reviewer) {
      if (!array_key_exists($reviewer->reviewer_classification, $reviewers_steps_by_classification)) {
        if ($reviewer->reviewer_classification == MFCS_REVIEW_CLASSIFICATION_VENUE_COORDINATOR) {
          continue;
        }

        $reviewers_steps_by_classification[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
      }
    }
    $required_reviews = count($reviewers_steps_by_classification);

    $desired_decisions = array(
      MFCS_REVIEW_DECISION_DENY,
      MFCS_REVIEW_DECISION_APPROVE,
    );

    $total_reviews = 0;
    try {
      $query = db_select('mfcs_field_review_review', 'mfrr');
      $query->innerjoin('mfcs_review_classifications', 'mrc', 'mfrr.id = mrc.review_id');

      $query->addField('mrc', 'classification', 'classification');

      $query->condition('mfrr.request_id', $request_id);
      $query->condition('mfrr.revision', $review_revision);
      $query->condition('mfrr.decision', $desired_decisions, 'IN');

      $query->condition('mrc.classification', $reviewers_steps_by_classification, 'IN');
      $query->distinct();

      $reviews = $query->execute()->fetchAll();
      if ($reviews === FALSE) {
        watchdog(MFCS_WATCHDOG_ID, "Failed to load reviewer classification count for request %request_id with classification %classification on step %step. Assuming total is 0.", array('%request_id' => $request_id, '%classification' => $request_classification, '%step' => $request_step), WATCHDOG_ERROR);
        $total_reviews = 0;
      }
      else {
        $total_reviews = count($reviews);
      }
    }
    catch (Exception $ex) {
      cf_error::on_query_execution($ex);

      return FALSE;
    }

    if ($total_reviews < $required_reviews) {
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

  $new_status = NULL;
  $new_step = NULL;
  $generate_pdf = FALSE;

  $system_user = user_load(1);
  $changed_user = $user;
  $changed_by = $user->uid;
  $changed_type = 'reviewer';

  // Venue Available
  if ($request_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
    if ($decision == MFCS_REVIEW_DECISION_APPROVE) {
      $new_status = MFCS_EVENT_STATUS_LOCKED;
      $new_step = MFCS_REVIEW_STEP_REVIEW;
    }
    elseif ($decision == MFCS_REVIEW_DECISION_DENY) {
      $new_status = MFCS_EVENT_STATUS_CLOSED_UNAVAILABLE;
      $new_step = MFCS_REVIEW_STEP_COMPLETED;
      $generate_pdf = TRUE;
    }
  }
  // Review Step
  elseif ($request_step == MFCS_REVIEW_STEP_REVIEW) {
    if ($decision == MFCS_REVIEW_DECISION_DENY || $decision == MFCS_REVIEW_DECISION_APPROVE) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = MFCS_REVIEW_STEP_REQUIREMENTS;
    }
  }
  // Requirements Step
  elseif ($request_step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    if ($decision == MFCS_REVIEW_DECISION_DENY || $decision == MFCS_REVIEW_DECISION_APPROVE || $decision == MFCS_REVIEW_DECISION_WAVE) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = MFCS_REVIEW_STEP_MAKE_DECISIONS;
    }
  }
  // Venue Coordinator (Final) Review Step
  elseif ($request_step == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
    $message = '';

    if ($decision == MFCS_REVIEW_DECISION_APPROVE) {
      $new_status = MFCS_EVENT_STATUS_CLOSED_ACCEPTED;
      $new_step = MFCS_REVIEW_STEP_COMPLETED;
      $generate_pdf = TRUE;
    }
    elseif ($decision == MFCS_REVIEW_DECISION_DENY) {
      $new_status = MFCS_EVENT_STATUS_CLOSED_DENIED;
      $new_step = MFCS_REVIEW_STEP_COMPLETED;
      $generate_pdf = TRUE;
    }

    $reviews = mfcs_get_latest_reviews($request_id, array(MFCS_REVIEW_STEP_REVIEW), array(MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY));

    if (!empty($reviews)) {
      mfcs_include(4);

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
  }

  $pdf_filename = NULL;
  if ($generate_pdf) {
    $success = mfcs_generate_pdf($request_id);

    if ($success) {
      $pdf_filename = mfcs_build_filename($request_id, 0);
    }
  }

  $parameters = array();
  $parameters['changed_by'] = $changed_by;
  $parameters['changed_type'] = $changed_type;
  $parameters['message'] = $message;

  if (!is_null($new_status) && $new_status != MFCS_EVENT_STATUS_LOCKED) {
    $parameters['status'] = $new_status;
  }
  elseif ($new_step == MFCS_REVIEW_STEP_REVIEW || $new_step == MFCS_REVIEW_STEP_REQUIREMENTS || $new_step == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
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
 * Process the change of an requests status.
 *
 * @param int $request_id
 *   The request id.
 * @param int $status
 *   The new workflow status the request is going to.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_workflow_status_change_alert($request_id, $status, $user = NULL) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
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

  $parameters = array();
  $parameters['changed_by'] = $user->uid;
  $parameters['changed_type'] = 'user';
  $parameters['status'] = $status;

  mfcs_send_workflow_emails($request_id, $parameters);

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
 *   - update: A boolean representing whether or not this was updated.
 *   - comment: A boolean representing whether or not this is a comment.
 *   - cancelled: A boolean representing whether or not the request was
 *     cancelled.
 *   - uncancelled: A boolean representing whether or not the request was
 *     uncancelled.
 *
 *   The keys: 'new', 'update', 'step', 'status', 'cancelled', and
 *   'uncancelled' are mutually exclusive.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_send_workflow_emails($request_id, $parameters) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find request with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
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

  $is_uncancelled = FALSE;
  if (array_key_exists('uncancelled',  $parameters)) {
    $is_uncancelled = $parameters['uncancelled'];
    if (!is_bool($is_uncancelled)) {
      cf_error::invalid_bool('parameters[uncancelled]');
      return FALSE;
    }
  }

  $is_insurance_provided = FALSE;
  if (array_key_exists('insurance_provided',  $parameters)) {
    $is_insurance_provided = $parameters['insurance_provided'];
    if (!is_bool($is_insurance_provided)) {
      cf_error::invalid_bool('parameters[insurance_provided]');
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

  $request_requester = user_load($request['mer']['user_id'][0]->value);

  // load appropriate reviewers in their own categories.
  $venue_coordinator = user_load($request['venue_coordinator']['user_id'][0]->value);

  $reviewers = mfcs_get_reviewers(array('mr.request_classification' => $request['request_coordinator']['classification'][0]->value, 'mr.review_step' => MFCS_REVIEW_STEP_REVIEW, 'mr.disabled' => 0), TRUE);
  $step_1_reviewers = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $r) {
      $step_1_reviewers[$r->user_id] = $r;
    }
  }

  $reviewers = mfcs_get_reviewers(array('mr.request_classification' => $request['request_coordinator']['classification'][0]->value, 'mr.review_step' => MFCS_REVIEW_STEP_REQUIREMENTS, 'mr.disabled' => 0), TRUE);
  $step_2_reviewers = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $r) {
      $step_2_reviewers[$r->user_id] = $r;
    }
  }

  // prepare e-mail(s) for sending
  $email_params = array();
  $email_params['request_id'] = $request_id;
  #$email_params['filename'] = array($filename); // @todo: should files be attached?
  $email_params['from'] = 'facilities_use@fcs.mcneese.edu';

  $email_from = '"McNeese Facilities Use System" <facilities_use@fcs.mcneese.edu>';

  $location = mfcs_load_locations($request['top']['location'][0]->value);
  $location_text = '';

  if (is_object($location)) {
    $location_text = $location->location_name;
  }

  $room = mfcs_load_rooms(NULL, NULL, $request['top']['room'][0]->value);
  $room_text = '';

  if (is_object($room)) {
    $room_text = $room->room_name;
    $building = mfcs_load_buildings(NULL, $room->building_id);
    $building_text = '';

    if (is_object($building)) {
      $building_text = $building->building_name;
    }
  }

  $standard_content = "\n\n";
  $standard_content .= "Request ID:\n - " . $request_id . "\n\n";
  $standard_content .= "Request Title:\n - " . $request['information']['title'][0]->value . "\n\n";
  $standard_content .= "Location:\n - " . $location_text . "\n\n";
  $standard_content .= "Building:\n - " . $building_text . "\n\n";
  $standard_content .= "Room:\n - " . $room_text . "\n\n";
  $standard_content .= "Dates:\n";

  $standard_markup = "<br>\n<br>\n";
  $standard_markup .= "<strong>Request ID</strong>:\n<ul><li>" . $request_id . "</li></ul>\n<br>\n";
  $standard_markup .= "<strong>Request Title</strong>:\n<ul><li>" . $request['information']['title'][0]->value . "</li></ul>\n<br>\n";
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
    $email_params['title_suffix'] = "Requested";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has created this request.\n";
    $email_params['content'] .= $standard_content;
    $email_params['markup'] = "";
    $email_params['markup'] .= $changed_by_string . " has created this request.<br>\n";
    $email_params['markup'] .= $standard_markup;

    if (is_object($venue_coordinator)) {
      $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
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
    if ($status == MFCS_EVENT_STATUS_LOCKED || $status == MFCS_EVENT_STATUS_UNLOCKED) {
      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
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
    if ($status == MFCS_EVENT_STATUS_LOCKED) {
      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
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

    if (is_object($venue_coordinator)) {
      $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
    }

    foreach ($step_1_reviewers as $r) {
      $email_to[$r->email] = $r->email;
    }

    foreach ($step_2_reviewers as $r) {
      $email_to[$r->email] = $r->email;
    }
  }
  elseif ($is_cancelled || $is_uncancelled || !is_null($status)) {
    if ($is_cancelled || $is_uncancelled || $status == MFCS_EVENT_STATUS_CANCELLED || $status == MFCS_EVENT_STATUS_DELETED) {
      $email_params['content'] = "";
      $email_params['markup'] = "";

      if ($is_cancelled || $status == MFCS_EVENT_STATUS_CANCELLED) {
        $email_params['title_suffix'] = "Cancelled";
        $email_params['content'] .= $changed_by_string . " has cancelled the request.\n";
        $email_params['markup'] .= $changed_by_string . " has cancelled the request.<br>\n";
      }
      elseif ($is_uncancelled) {
        $email_params['title_suffix'] = "Uncancelled";
        $email_params['content'] .= $changed_by_string . " has uncancelled the request.\n";
        $email_params['markup'] .= $changed_by_string . " has uncancelled the request.<br>\n";
      }
      elseif ($status == MFCS_EVENT_STATUS_DELETED) {
        $email_params['title_suffix'] = "Deleted";
        $email_params['content'] .= $changed_by_string . " has deleted the request.\n";
        $email_params['markup'] .= $changed_by_string . " has deleted the request.<br>\n";
      }

      $email_params['content'] .= $standard_content;
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester->mail] = $request_requester->mail;

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      if ($status != MFCS_EVENT_STATUS_DELETED) {
        // send messages to all reviewers.
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }

        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
    elseif ($status == MFCS_EVENT_STATUS_CLOSED_ACCEPTED) {
      $email_params['title_suffix'] = "Approved";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been approved.\n";
      $email_params['content'] .= "For more information, view the review log.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been approved.<br>\n";
      $email_params['markup'] .= "For more information, view the <a href=" . '"' . $request_view_url . '/2"' . ">review log</a>.<br>\n";
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester->mail] = $request_requester->mail;

      // send messages to all reviewers.
      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($status == MFCS_EVENT_STATUS_CLOSED_DENIED) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been denied.\n";
      $email_params['content'] .= "For more information, view the review log.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been denied.<br>\n";
      $email_params['markup'] .= "For more information, view the <a href=" . '"' . $request_view_url . '/2"' . ">review log</a>.<br>\n";
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester->mail] = $request_requester->mail;

      // send messages to all reviewers.
      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($status == MFCS_EVENT_STATUS_CLOSED_UNAVAILABLE || $status == MFCS_EVENT_STATUS_CLOSED_DUE_TO_LOCK) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The request has been denied due to the requested location not being available for the requested time slots.\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request has been <em>denied</em> due to the requested location <em>not being available</em> for the requested time slots.<br>\n";
      $email_params['markup'] .= $standard_markup;

      $email_to[$request_requester->mail] = $request_requester->mail;
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
    }
    elseif ($request['top']['step'][0]->value == MFCS_REVIEW_STEP_REVIEW) {
      $email_params['title_suffix'] = "Needs Review";
      $email_params['content'] = "";
      $email_params['content'] .= "The request is available for review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;
      $email_params['markup'] = "";
      $email_params['markup'] .= "The request is available for <em>review</em>.<br>\n";
      $email_params['markup'] .= "<br>\n";
      $email_params['markup'] .= $message . "<br>\n";
      $email_params['markup'] .= $standard_markup;

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
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

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }
    }
  }
  elseif ($is_comment) {
    if ($request['top']['status'][0]->value == MFCS_EVENT_STATUS_LOCKED || $request['top']['status'][0]->value == MFCS_EVENT_STATUS_UNLOCKED) {
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

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
  }

  if (!empty($email_to)) {
    // DEBUG: temporarily disable e-mailing to prrequest production accounts from receiving e-mails.
    //        only allow reserved accounts to receive e-mails
    $debug_accounts = array();
    $debug_accounts[] = 'kday@mcneese.edu';
    $debug_accounts[] = 'colleen@mcneese.edu';
    $debug_accounts[] = 'gfisher@mcneese.edu';
    $debug_accounts[] = 'stan@mcneese.edu';
    $debug_accounts[] = 'gbodin@mcneese.edu';
    $debug_accounts[] = 'shogan@mcneese.edu';
    $debug_accounts[] = 'rrhoden@mcneese.edu';
    $debug_accounts[] = 'thomas@mcneese.edu';
    $debug_accounts[] = 'rfontenot@mcneese.edu';
    $debug_accounts[] = 'mwhite@mcneese.edu';
    $debug_accounts[] = 'awoods@mcneese.edu';
    $debug_accounts[] = 'rspinks@mcneese.edu';
    $debug_accounts[] = 'bmungai@mcneese.edu';
    $debug_accounts[] = 'emeche@mcneese.edu';
    $debug_accounts[] = 'bprejean@mcneese.edu';

    foreach ($email_to as $to) {
      if (!in_array($to, $debug_accounts)) {
        drupal_set_message(t("Warning: not sending mail to @to while testing the system.", array('@to' => $to)), 'warning');
        watchdog('debug', "Warning: not sending mail to @to while testing the system.", array('@to' => $to), WATCHDOG_WARNING);
        continue;
      }

      drupal_mail('mfcs', 'request_alert', $to, language_default(), $email_params, $email_from, TRUE);
    }
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
