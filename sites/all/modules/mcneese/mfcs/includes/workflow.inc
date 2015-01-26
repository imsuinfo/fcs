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
        $query->fields($mfcs_requests);
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
    return FALSE;
  }

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
  if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && $is_effective_venue_coordinator) {
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
    'requirements_security' => NULL,
    'requirements_other' => NULL,
    'requirements_waived' => NULL,
    'requirements_university' => NULL,
  );

  $requirements_latest = array(
    'facilities' => NULL,
    'equipment' => NULL,
    'custodial' => NULL,
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
    if ($decision != MFCS_REVIEW_DECISION_COMMENT && $decision != MFCS_REVIEW_DECISION_CANCELLED && $decision != MFCS_REVIEW_DECISION_UNCANCELLED && $decision != MFCS_REVIEW_DECISION_CLOSED) {
      unset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR]);
      unset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY]);
    }
  }

  $mfcs_field_requirements = array();
  if (($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_REQUIREMENT || $decision == MFCS_REVIEW_DECISION_WAVE)) || ($is_quick_meeting && $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
    $other_requirement = 0;
    if (isset($extra['requirements']['other'])) {
      $other_requirement = $extra['requirements']['other'];
    }

    if (is_null($mfcs_current_requirements['requirements_other'])) {
      if (is_null($requirements_latest['other'])) {
        $mfcs_current_requirements['requirements_other'] = 0;
      }
      else {
        $mfcs_current_requirements['requirements_other'] = $requirements_latest['other'] + 1;
      }
    }
    else {
      $mfcs_current_requirements['requirements_other']++;
    }
    $mfcs_field_requirements['other'] = array(
      'request_id' => $request_id,
      'revision' => $mfcs_current_requirements['requirements_other'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => $instance,
      'value' => $other_requirement,
    );
  }

  if ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
    $uses = array();
    if ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      if ($is_effective_venue_coordinator && $type != MFCS_REQUEST_TYPE_QUICK_MEETING) {
        $uses['facilities'] = 'facilities';
        $uses['equipment'] = 'equipment';
      }
    }
    elseif ($step == MFCS_REVIEW_STEP_REVIEW) {
      if (isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_FACILITIES])) {
        $uses['custodial'] = 'custodial';
      }

      if (isset($reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_SECURITY])) {
        $uses['security'] = 'security';
      }
    }

    if (isset($reviewer_classifications_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING])) {
      if ($step == MFCS_REVIEW_STEP_REVIEW) {
        $insurance_requirements = TRUE;
      }

      $insurance_provided_exception = TRUE;
      $reviewer_classifications[MFCS_REVIEWER_CLASSIFICATION_PURCHASING] = MFCS_REVIEWER_CLASSIFICATION_PURCHASING;
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

      $use_name = 'requirements_' . $use;
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

  if (($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $is_quick_meeting) && ($decision == MFCS_REVIEW_DECISION_WAVE || $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
    $requirements_waived = 0;
    if (isset($extra['requirements']['waived'])) {
      $requirements_waived = $extra['requirements']['waived'];
    }

    if (is_null($mfcs_current_requirements['requirements_waived'])) {
      if (is_null($requirements_latest['waived'])) {
        $mfcs_current_requirements['requirements_waived'] = 0;
      }
      else {
        $mfcs_current_requirements['requirements_waived'] = $requirements_latest['waived'] + 1;
      }
    }
    else {
      $mfcs_current_requirements['requirements_waived']++;
    }

    $mfcs_field_requirements['waived'] = array(
      'request_id' => $request_id,
      'revision' => $mfcs_current_requirements['requirements_waived'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => $instance,
      'value' => $requirements_waived,
    );

    $requirements_university = 0;
    if (isset($extra['requirements']['university'])) {
      $requirements_university = $extra['requirements']['university'];
    }

    if (is_null($mfcs_current_requirements['requirements_university'])) {
      if (is_null($requirements_latest['university'])) {
        $mfcs_current_requirements['requirements_university'] = 0;
      }
      else {
        $mfcs_current_requirements['requirements_university'] = $requirements_latest['university'] + 1;
      }
    }
    else {
      $mfcs_current_requirements['requirements_university']++;
    }

    $mfcs_field_requirements['university'] = array(
      'request_id' => $request_id,
      'revision' => $mfcs_current_requirements['requirements_university'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => $instance,
      'value' => $requirements_university,
    );
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

    if ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $step == MFCS_REVIEW_STEP_REVIEW  || $step == MFCS_REVIEW_STEP_REQUIREMENTS || $step == MFCS_REVIEW_STEP_MAKE_DECISIONS || $step == MFCS_REVIEW_STEP_COMPLETED)) {
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
    elseif ($step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_WAVE) {
      $query = db_insert('mfcs_field_requirements_other');
      $query->fields($mfcs_field_requirements['other']);
      $query->execute();

      $query = db_insert('mfcs_field_requirements_waived');
      $query->fields($mfcs_field_requirements['waived']);
      $query->execute();

      $query = db_insert('mfcs_field_requirements_university');
      $query->fields($mfcs_field_requirements['university']);
      $query->execute();

      $query = db_update('mfcs_current');
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
  $request = mfcs_load_request_by_id($request_id, TRUE);

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
  elseif ($decision == MFCS_REVIEW_DECISION_WAVE || $decision = MFCS_REVIEW_DECISION_CANCELLED || $decision == MFCS_REVIEW_DECISION_UNCANCELLED || $decision = MFCS_REVIEW_DECISION_CLOSED || ($decision == MFCS_REVIEW_DECISION_REQUIREMENT && !$insurance_provided_exception)) {
    // do not send an e-mail.
    return TRUE;
  }

  // continue to next step only if at least 1 of each reviewer classification has submitted a reply.
  if ($step == MFCS_REVIEW_STEP_REVIEW || $step == MFCS_REVIEW_STEP_REQUIREMENTS) {
    $checked = mfcs_check_if_requests_are_fully_reviewed(array($request_id => $review_revision), $classification, $step);

    if (is_bool($checked)) {
      return $checked;
    }

    if (!array_key_exists($request_id, $checked)) {
      return FALSE;
    }

    // When FALSE, then there are not enough reviews to continue.
    if ($checked[$request_id] === FALSE) {
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
 *   The keys: 'new', 'update', 'step', 'status', 'cancelled', and
 *   'uncancelled', 'amendment', 'reminder' are mutually exclusive.
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

  $request_requester = user_load($request['mer']['user_id'][0]->value);

  // load appropriate reviewers in their own categories.
  $venue_coordinator = user_load($request['venue_coordinator']['user_id'][0]->value);

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
    $type = $request['information']['type'][0]->value;

    if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && $venue_coordinator->uid == $request_requester->uid) {
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

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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
      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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
      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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

    if (is_object($venue_coordinator)) {
      $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
    }

    // also send e-mails to the venue coordinator proxies.
    $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
    if (!empty($proxies_returned['results'])) {
      foreach ($proxies_returned['results'] as $proxy_returned) {
        $proxy = user_load($proxy_returned->proxy_id);

        if (is_object($proxy)) {
          $email_to[$proxy->mail] = $proxy->mail;
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
  elseif ($is_cancelled || $is_auto_cancelled || $is_uncancelled || $is_deleted || !is_null($status)) {
    if ($is_cancelled || $is_uncancelled || $status == MFCS_REQUEST_STATUS_CANCELLED || $status == MFCS_REQUEST_STATUS_DELETED) {
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

      $email_to[$request_requester->mail] = $request_requester->mail;

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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

      $email_to[$request_requester->mail] = $request_requester->mail;

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

      $email_to[$request_requester->mail] = $request_requester->mail;

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

      // also send e-mails to the venue coordinator proxies.
      $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
      if (!empty($proxies_returned['results'])) {
        foreach ($proxies_returned['results'] as $proxy_returned) {
          $proxy = user_load($proxy_returned->proxy_id);

          if (is_object($proxy)) {
            $email_to[$proxy->mail] = $proxy->mail;
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

    if (is_object($venue_coordinator)) {
      $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
    }

    // also send e-mails to the venue coordinator proxies.
    $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $venue_coordinator->uid, 'mpvc.disabled' => 0));
    if (!empty($proxies_returned['results'])) {
      foreach ($proxies_returned['results'] as $proxy_returned) {
        $proxy = user_load($proxy_returned->proxy_id);

        if (is_object($proxy)) {
          $email_to[$proxy->mail] = $proxy->mail;
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

  // when testing, but not debugging, then prepend the testing prefix to e-mails to distinguish between production and non-production e-mails.
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
 * Check whether to at least 1 of each reviewer classification has reviewed.
 *
 * @param array $request_ids
 *   An array of request ids to check.
 *   The array should be keyed by request id with a value of the request the
 *   revision.
 * @param int $classification
 *   The requests current classification.
 * @param int $step
 *   The requests current step.
 *
 * @param array|bool.
 *   The array is be keyed by request id and the value is a boolean designating
 *   whether the request is fully reviewed or not.
 *   TRUE is returned when there are no requests to validate (when requests_ids
 *   is NULL).
 *   FALSE is returned on error.
 */
function mfcs_check_if_requests_are_fully_reviewed($request_ids, $classification, $step) {
  if (!is_array($request_ids) || empty($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!cf_is_integer($classification)) {
    cf_error::invalid_integer('classification');
    return FALSE;
  }

  if (!cf_is_integer($step)) {
    cf_error::invalid_integer('step');
    return FALSE;
  }

  // do not operate on steps that cannot be normally reviewed.
  if ($step != MFCS_REVIEW_STEP_REVIEW && $step != MFCS_REVIEW_STEP_REQUIREMENTS) {
    return FALSE;
  }

  $reviewers_returned = mfcs_get_reviewers(array('mr.request_classification' => $classification, 'mr.review_step' => $step, 'mr.disabled' => 0));
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  $required_reviews = 0;
  $reviewers_steps_by_classification = array();
  foreach ($reviewers as $reviewer) {
    if (!array_key_exists($reviewer->reviewer_classification, $reviewers_steps_by_classification)) {
      if ($reviewer->reviewer_classification == MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR) {
        continue;
      }

      $reviewers_steps_by_classification[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
    }
  }
  $required_reviews = count($reviewers_steps_by_classification);

  $review_decisions = array(
    MFCS_REVIEW_DECISION_DENY,
    MFCS_REVIEW_DECISION_APPROVE,
  );

  $checked = array();
  foreach ($request_ids as $request_id => $request_revision) {
    if (array_key_exists($request_id, $checked)) {
      continue;
    }

    $checked[$request_id] = FALSE;

    $total_reviews = 0;
    try {
      $query = db_select('mfcs_field_review_review', 'mfrr');
      $query->innerJoin('mfcs_review_classifications', 'mrc', 'mfrr.id = mrc.review_id');

      $query->addField('mrc', 'classification', 'classification');

      $query->condition('mfrr.request_id', $request_id);
      $query->condition('mfrr.revision', $request_revision);
      $query->condition('mfrr.decision', $review_decisions, 'IN');

      $query->condition('mrc.classification', $reviewers_steps_by_classification, 'IN');
      $query->distinct();

      $reviews = $query->execute()->fetchAll();
      if ($reviews === FALSE) {
        watchdog(MFCS_WATCHDOG_ID, "Failed to load reviewer classification count for request %request_id (revision: %request_revision) with classification %classification on step %step. Assuming total is 0.", array('%request_id' => $request_id, '%request_revision' => $request_revision, '%classification' => $classification, '%step' => $step), WATCHDOG_ERROR);
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

    if ($total_reviews >= $required_reviews) {
      $checked[$request_id] = TRUE;
    }
  }

  return $checked;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
