<?php

/**
 * @file
 * Defines McNeese event workflow workflow functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Assign a new step to a given event request.
 *
 * @param int $event_id
 *   The id of the given event.
 * @param int $step
 *   The new step to assign to the specified event.
 * @param object|null $user
 *   If specified, the user to log this change as.
 *
 * @return bool
 *   (optional) TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_set_step($event_id, $step, $user = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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

  mcneese_event_workflow_include(4);

  $step_options = event_workflow_get_review_step_list_options();

  if (!array_key_exists($step, $step_options)) {
    watchdog('mew', "%step is not a valid step.", array('%step' => $step), WATCHDOG_ERROR);
    return FALSE;
  }

  $mew_event_requests = array(
    'updated' => REQUEST_TIME,
  );

  $event_revision = mcneese_event_workflow_load_event_revision_number($event_id);
  $processed_mew_event_revisions = &drupal_static('processed_mew_event_revisions_' . REQUEST_TIME, FALSE);

  if ($event_revision === FALSE) {
    watchdog('mew', "Failed to load revision number for event %event_id.", array('%event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($event_revision)) {
    $event_revision_next = 0;
  }
  else {
    $event_revision_next = $event_revision + 1;
  }

  $mew_event_revisions = array(
    'event_id' => $event_id,
    'date' => REQUEST_TIME,
    'revision' => $event_revision_next,
  );

  $step_revision = $event['current']['top']['step'] + 1;

  $mew_current_top = array(
    'step' => $step_revision,
  );

  $mew_field_top_step = array(
    'event_id' => $event_id,
    'revision' => $step_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
    'value' => $step,
  );


  $transaction = db_transaction();

  try {
    $query = db_update('mew_event_requests');
    $query->condition('id', $event_id);
    $query->fields($mew_event_requests);
    $query->execute();

    if (!$processed_mew_event_revisions) {
      $query = db_insert('mew_event_revisions');
      $query->fields($mew_event_revisions);
      $query->execute();

      $processed_mew_event_revisions = TRUE;
    }

    $query = db_insert('mew_field_top_step');
    $query->fields($mew_field_top_step);
    $query->execute();

    $query = db_update('mew_current_top');
    $query->condition('event_id', $event_id);
    $query->fields($mew_current_top);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // reset the cached event object
  event_workflow_load_event_by_id($event_id, TRUE);

  return TRUE;
}

/**
 * Assign a new status to a given event request.
 *
 * @param int $event_id
 *   The id of the given event.
 * @param int $status
 *   The new status to assign to the specified event.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_set_status($event_id, $status, $user = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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
    MEW_EVENT_STATUS_DELETED,
    MEW_EVENT_STATUS_LOCKED,
    MEW_EVENT_STATUS_UNLOCKED,
    MEW_EVENT_STATUS_CLOSED_ACCEPTED,
    MEW_EVENT_STATUS_CLOSED_DENIED,
    MEW_EVENT_STATUS_CLOSED_UNAVAILABLE,
    MEW_EVENT_STATUS_CLOSED_DUE_TO_LOCK,
    MEW_EVENT_STATUS_CANCELLED,
  );

  if (!in_array($status, $status_options)) {
    watchdog('mew', "%status is not a valid status.", array('%status' => $status), WATCHDOG_ERROR);
    return FALSE;
  }

  $mew_event_requests = array(
    'updated' => REQUEST_TIME,
  );

  $event_revision = mcneese_event_workflow_load_event_revision_number($event_id);
  $processed_mew_event_revisions = &drupal_static('processed_mew_event_revisions_' . REQUEST_TIME, FALSE);

  if ($event_revision === FALSE) {
    watchdog('mew', "Failed to load revision number for event %event_id.", array('%event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($event_revision)) {
    $event_revision_next = 0;
  }
  else {
    $event_revision_next = $event_revision + 1;
  }

  $mew_event_revisions = array(
    'event_id' => $event_id,
    'date' => REQUEST_TIME,
    'revision' => $event_revision_next,
  );

  $status_revision = $event['current']['top']['status'] + 1;

  $mew_current_top = array(
    'status' => $status_revision,
  );

  $mew_field_top_status = array(
    'event_id' => $event_id,
    'revision' => $status_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
    'value' => $status,
  );

  $transaction = db_transaction();

  try {
    $query = db_update('mew_event_requests');
    $query->condition('id', $event_id);
    $query->fields($mew_event_requests);
    $query->execute();

    if (!$processed_mew_event_revisions) {
      $query = db_insert('mew_event_revisions');
      $query->fields($mew_event_revisions);
      $query->execute();

      $processed_mew_event_revisions = TRUE;
    }

    $query = db_insert('mew_field_top_status');
    $query->fields($mew_field_top_status);
    $query->execute();

    $query = db_update('mew_current_top');
    $query->condition('event_id', $event_id);
    $query->fields($mew_current_top);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // reset the cached event object
  event_workflow_load_event_by_id($event_id, TRUE);

  // @todo: if it is the final step, accepted, etc..make sure to generate the PDF file and save it to the file system for remote storage.

  return TRUE;
}

/**
 * Trigger the next step.
 *
 * @param int $event_id
 *   The event id.
 * @param object $reviewer
 *   The reviewer.
 * @param int $step
 *   The event review step at the time of the decision.
 * @param int $decision
 *   The event workflow decision.
 * @param int $message
 *   The event workflow decision message.
 * @param array $extra
 *   (optional) Additional fields that are specific to the individual step.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_step_review($event_id, $user, $step, $decision, $message, $extra = array()) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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

  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }

  $event_classification = $event['event_coordinator']['classification'][0]->value;
  $step = $event['top']['step'][0]->value;
  $venue_coordinator_id = $event['venue_coordinator']['user_id'][0]->value;

  // review revisions are neither used nor incremented in the same manner as other fields.
  // the revision number will not increment when a new review is made.
  $review_revision = mcneese_event_workflow_load_event_revision_number($event_id, 'mew_field_review_review');

  if ($review_revision === FALSE || is_null($review_revision)) {
    $review_revision = 0;
  }

  $user_data = array(
    'user_id' => $user->uid,
  );

  $mew_event_requests = array(
    'updated' => REQUEST_TIME,
  );

  $event_revision = mcneese_event_workflow_load_event_revision_number($event_id);
  $processed_mew_event_revisions = &drupal_static('processed_mew_event_revisions_' . REQUEST_TIME, FALSE);

  if ($event_revision === FALSE) {
    watchdog('mew', "Failed to load revision number for event %event_id.", array('%event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }
  elseif (is_null($event_revision)) {
    $event_revision_next = 0;
  }
  else {
    $event_revision_next = $event_revision + 1;
  }

  $mew_event_revisions = array(
    'event_id' => $event_id,
    'date' => REQUEST_TIME,
    'revision' => $event_revision_next,
  );

  $mew_current_use_fees = array(
    'event_id' => $event_id,
    'facilities' => NULL,
    'equipment' => NULL,
    'custodial' => NULL,
    'security' => NULL,
    'other' => NULL,
    'waved' => NULL,
    'university' => NULL,
  );

  if (!empty($event['current']['use_fees'])) {
    if (array_key_exists('facilities', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['facilities'])) {
      $mew_current_use_fees['facilities'] = $event['current']['use_fees']['facilities'];
    }

    if (array_key_exists('equipment', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['equipment'])) {
      $mew_current_use_fees['equipment'] = $event['current']['use_fees']['equipment'];
    }

    if (array_key_exists('custodial', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['custodial'])) {
      $mew_current_use_fees['custodial'] = $event['current']['use_fees']['custodial'];
    }

    if (array_key_exists('security', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['security'])) {
      $mew_current_use_fees['security'] = $event['current']['use_fees']['security'];
    }

    if (array_key_exists('other', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['other'])) {
      $mew_current_use_fees['other'] = $event['current']['use_fees']['other'];
    }

    if (array_key_exists('waved', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['waved'])) {
      $mew_current_use_fees['waved'] = $event['current']['use_fees']['waved'];
    }

    if (array_key_exists('university', $event['current']['use_fees']) && !is_null($event['current']['use_fees']['university'])) {
      $mew_current_use_fees['university'] = $event['current']['use_fees']['university'];
    }
  }

  $mew_current_insurance = array(
    'event_id' => $event_id,
    'contractor' => NULL,
    'unaffiliated' => NULL,
  );

  if (!empty($event['current']['insurance'])) {
    if (array_key_exists('contractor', $event['current']['insurance']) && !is_null($event['current']['insurance']['contractor'])) {
      $mew_current_insurance['contractor'] = $event['current']['insurance']['contractor'];
    }

    if (array_key_exists('unaffiliated', $event['current']['insurance']) && !is_null($event['current']['insurance']['unaffiliated'])) {
      $mew_current_insurance['unaffiliated'] = $event['current']['insurance']['unaffiliated'];
    }
  }

  $mew_field_review_review = array(
    'event_id' => $event_id,
    'revision' => $review_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
    'step' => $step,
    'decision' => $decision,
    'message' => $message,
  );

  $mew_field_use_fees = array();
  if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE && $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
    $other_use_fee = 0;
    if (isset($extra['use_fees']['other'])) {
      $other_use_fee = $extra['use_fees']['other'];
    }

    if (is_null($mew_current_use_fees['other'])) {
      $mew_current_use_fees['other'] = 0;
    }
    else {
      $mew_current_use_fees['other']++;
    }
    $mew_field_use_fees['other'] = array(
      'event_id' => $event_id,
      'revision' => $mew_current_use_fees['other'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => REQUEST_TIME,
      'value' => $other_use_fee,
    );
  }

  $reviewer_conditions = array(
    'mr.user_id' => $user->uid,
    'mr.event_classification' => $event_classification,
    'mr.review_step' => $step,
    'mr.disabled' => 0,
  );

  $reviewer_classifications = array();
  $reviewers = mcneese_event_workflow_get_reviewers($reviewer_conditions, TRUE);
  if (!is_array($reviewers)) {
    $reviewers = array();
  }

  foreach ($reviewers as $reviewer) {
    $reviewer_classifications[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
  }

  if ($venue_coordinator_id == $user->uid) {
    $reviewer_classifications[MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR] = MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR;
  }

  if (($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_REVIEW) && $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
    $uses = array();
    $insurance_fees = FALSE;
    if ($step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
      if ($event['venue_coordinator']['user_id'][0]->value == $user->uid) {
        $uses['facilities'] = 'facilities';
        $uses['equipment'] = 'equipment';
      }
    }
    else {
      foreach ($reviewers as $reviewer) {
        if ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_FACILITIES) {
          $uses['custodial'] = 'custodial';
        }
        elseif ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_SECURITY) {
          $uses['security'] = 'security';
        }
        elseif ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_PURCHASING) {
          $insurance_fees = TRUE;
        }
      }
    }

    foreach ($uses as $use) {
      $use_fees = array('quantity' => 0, 'hours' => 0, 'days' => 0, 'amount' => 0);

      if (!empty($extra['use_fees'][$use]['quantity'])) {
        $use_fees['quantity'] = $extra['use_fees'][$use]['quantity'];
      }

      if (!empty($extra['use_fees'][$use]['hours'])) {
        $use_fees['hours'] = $extra['use_fees'][$use]['hours'];
      }

      if (!empty($extra['use_fees'][$use]['days'])) {
        $use_fees['days'] = $extra['use_fees'][$use]['days'];
      }

      if (!empty($extra['use_fees'][$use]['amount'])) {
        $use_fees['amount'] = event_workflow_convert_value_to_database_format($extra['use_fees'][$use]['amount'], 'currency');
      }

      if ($use_fees['amount'] === FALSE) {
        $use_fees['amount'] = 0;
      }

      if (is_null($mew_current_use_fees[$use])) {
        $mew_current_use_fees[$use] = 0;
      }
      else {
        $mew_current_use_fees[$use]++;
      }

      $mew_field_use_fees[$use] = array(
        'event_id' => $event_id,
        'revision' => $mew_current_use_fees[$use],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'quantity' => $use_fees['quantity'],
        'hours' => $use_fees['hours'],
        'days' => $use_fees['days'],
        'amount' => $use_fees['amount'],
      );

      unset($use_fees);
    }

    if ($insurance_fees) {
      $contractor_insurance = 0;
      $unaffiliated_insurance = 0;

      if (isset($extra['insurance']['contractor'])) {
        $contractor_insurance = $extra['insurance']['contractor'];
      }

      if (isset($extra['insurance']['unaffiliated'])) {
        $unaffiliated_insurance = $extra['insurance']['unaffiliated'];
      }

      if (is_null($mew_current_insurance['contractor'])) {
        $mew_current_insurance['contractor'] = 0;
      }
      else {
        $mew_current_insurance['contractor']++;
      }
      $mew_field_insurance_contractor = array(
        'event_id' => $event_id,
        'revision' => $mew_current_insurance['contractor'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'value' => $contractor_insurance,
      );

      if (is_null($mew_current_insurance['unaffiliated'])) {
        $mew_current_insurance['unaffiliated'] = 0;
      }
      else {
        $mew_current_insurance['unaffiliated']++;
      }
      $mew_field_insurance_unaffiliated = array(
        'event_id' => $event_id,
        'revision' => $mew_current_insurance['unaffiliated'],
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => REQUEST_TIME,
        'value' => $unaffiliated_insurance,
      );
    }
  }
  elseif (($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_REVIEW) && $decision == MEW_REVIEW_DECISION_WAVE) {
    $fees_waved = 0;
    if (isset($extra['use_fees']['waved'])) {
      $fees_waved = $extra['use_fees']['waved'];
    }

    if (is_null($mew_current_use_fees['waved'])) {
      $mew_current_use_fees['waved'] = 0;
    }
    else {
      $mew_current_use_fees['waved']++;
    }

    $mew_field_use_fees['waved'] = array(
      'event_id' => $event_id,
      'revision' => $mew_current_use_fees['waved'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => REQUEST_TIME,
      'value' => $fees_waved,
    );

    $fees_university = 0;
    if (isset($extra['use_fees']['university'])) {
      $fees_university = $extra['use_fees']['university'];
    }

    if (is_null($mew_current_use_fees['university'])) {
      $mew_current_use_fees['university'] = 0;
    }
    else {
      $mew_current_use_fees['university']++;
    }

    $mew_field_use_fees['university'] = array(
      'event_id' => $event_id,
      'revision' => $mew_current_use_fees['university'],
      'delta' => 0,
      'user_id' => $user->uid,
      'date' => REQUEST_TIME,
      'value' => $fees_university,
    );
  }

  // replace all remaining NULL values with 0.
  foreach ($mew_current_use_fees as $key => &$value) {
    if (is_null($value)) {
      $value = 0;
    }
  }
  foreach ($mew_current_insurance as $key => &$value) {
    if (is_null($value)) {
      $value = 0;
    }
  }

  $failure = FALSE;
  $transaction = db_transaction();

  try {
    $query = db_update('mew_event_requests');
    $query->condition('id', $event_id);
    $query->fields($mew_event_requests);
    $query->execute();

    if (!$processed_mew_event_revisions) {
      $query = db_insert('mew_event_revisions');
      $query->fields($mew_event_revisions);
      $query->execute();
      $processed_mew_event_revisions = TRUE;
    }

    $query = db_insert('mew_field_review_review');
    $query->fields($mew_field_review_review);
    $review_id = $query->execute();

    foreach ($reviewer_classifications as $reviewer_classification) {
      $mew_review_classifications = array(
        'review_id' => $review_id,
        'classification' => $reviewer_classification,
      );

      $query = db_insert('mew_review_classifications');
      $query->fields($mew_review_classifications);
      $query->execute();
    }

    if (($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_REVIEW) && $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
      foreach ($mew_field_use_fees as $use => $fields) {
        $query = db_insert('mew_field_use_fees_' . $use);
        $query->fields($fields);
        $query->execute();
      }

      $query = db_update('mew_current_use_fees');
      $query->condition('event_id', $event_id);
      $query->fields($mew_current_use_fees);
      $query->execute();

      if ($insurance_fees) {
        $query = db_insert('mew_field_insurance_contractor');
        $query->fields($mew_field_insurance_contractor);
        $query->execute();

        $query = db_insert('mew_field_insurance_unaffiliated');
        $query->fields($mew_field_insurance_unaffiliated);
        $query->execute();
      }

      $query = db_update('mew_current_insurance');
      $query->condition('event_id', $event_id);
      $query->fields($mew_current_insurance);
      $query->execute();
    }
    elseif (($step == MEW_REVIEW_STEP_VENUE_AVAILABLE || $step == MEW_REVIEW_STEP_REVIEW) && $decision == MEW_REVIEW_DECISION_WAVE) {
      $query = db_insert('mew_field_use_fees_waved');
      $query->fields($mew_field_use_fees['waved']);
      $query->execute();

      $query = db_insert('mew_field_use_fees_university');
      $query->fields($mew_field_use_fees['university']);
      $query->execute();

      $query = db_update('mew_current_use_fees');
      $query->condition('event_id', $event_id);
      $query->fields($mew_current_use_fees);
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

  // forcefully clear the event cache so that it can reflect the changes.
  event_workflow_load_event_by_id($event_id, TRUE);

  // handle steps that do not alter the workflow.
  if ($decision == MEW_REVIEW_DECISION_COMMENT) {
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'reviewer';
    $parameters['message'] = $message;
    $parameters['comment'] = TRUE;
    mcneese_event_workflow_send_workflow_emails($event_id, $parameters);
    return TRUE;
  }
  elseif ($decision == MEW_REVIEW_DECISION_WAVE || $decision == MEW_REVIEW_DECISION_USAGE_FEE) {
    // do not send an e-mail.
    return TRUE;
  }

  // continue to next step only if at least 1 of each reviewer classification has submitted a reply.
  $event_status = $event['top']['status'][0]->value;
  $event_step = $event['top']['step'][0]->value;
  $event_classification = $event['event_coordinator']['classification'][0]->value;

  if ($event_step == MEW_REVIEW_STEP_REVIEW || $event_step == MEW_REVIEW_STEP_USAGE_FEES) {
    $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event_classification, 'mr.review_step' => $event_step, 'mr.disabled' => 0));

    $required_reviews = 0;
    $reviewers_steps_by_classification = array();
    foreach ($reviewers as $reviewer) {
      if (!array_key_exists($reviewer->reviewer_classification, $reviewers_steps_by_classification)) {
        if ($reviewer->reviewer_classification == MEW_REVIEW_CLASSIFICATION_VENUE_COORDINATOR) {
          continue;
        }

        $reviewers_steps_by_classification[$reviewer->reviewer_classification] = $reviewer->reviewer_classification;
      }
    }
    $required_reviews = count($reviewers_steps_by_classification);

    $desired_decisions = array(
      MEW_REVIEW_DECISION_DENY,
      MEW_REVIEW_DECISION_APPROVE,
    );

    $total_reviews = 0;
    try {
      $query = db_select('mew_field_review_review', 'mfrr');
      $query->innerjoin('mew_review_classifications', 'mrc', 'mfrr.id = mrc.review_id');

      $query->addField('mrc', 'classification', 'classification');

      $query->condition('mfrr.event_id', $event_id);
      $query->condition('mfrr.revision', $review_revision);
      $query->condition('mfrr.decision', $desired_decisions, 'IN');

      $query->condition('mrc.classification', $reviewers_steps_by_classification, 'IN');
      $query->distinct();

      $reviews = $query->execute()->fetchAll();
      if ($reviews === FALSE) {
        watchdog('mew', "Failed to load reviewer classification count for event %event_id with classification %classification on step %step. Assuming total is 0.", array('%event_id' => $event_id, '%classification' => $event_classification, '%step' => $event_step), WATCHDOG_ERROR);
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

  $new_status = NULL;
  $new_step = NULL;
  $generate_pdf = FALSE;

  $system_user = user_load(1);
  $changed_user = $user;
  $changed_by = $user->uid;
  $changed_type = 'reviewer';

  // Venue Available
  if ($event_step == MEW_REVIEW_STEP_VENUE_AVAILABLE) {
    if ($decision == MEW_REVIEW_DECISION_APPROVE) {
      $new_status = MEW_EVENT_STATUS_LOCKED;
      $new_step = MEW_REVIEW_STEP_REVIEW;
    }
    elseif ($decision == MEW_REVIEW_DECISION_DENY) {
      $new_status = MEW_EVENT_STATUS_CLOSED_UNAVAILABLE;
      $new_step = MEW_REVIEW_STEP_COMPLETED;
      $generate_pdf = TRUE;
    }
  }
  // Review Step
  elseif ($event_step == MEW_REVIEW_STEP_REVIEW) {
    if ($decision == MEW_REVIEW_DECISION_DENY || $decision == MEW_REVIEW_DECISION_APPROVE) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = MEW_REVIEW_STEP_USAGE_FEES;
    }
  }
  // Usage Fees Step
  elseif ($event_step == MEW_REVIEW_STEP_USAGE_FEES) {
    if ($decision == MEW_REVIEW_DECISION_DENY || $decision == MEW_REVIEW_DECISION_APPROVE || $decision == MEW_REVIEW_DECISION_WAVE) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = MEW_REVIEW_STEP_MAKE_DECISIONS;
    }
  }
  // Venue Coordinator (Final) Review Step
  elseif ($event_step == MEW_REVIEW_STEP_MAKE_DECISIONS) {
    $message = '';

    if ($decision == MEW_REVIEW_DECISION_APPROVE) {
      $new_status = MEW_EVENT_STATUS_CLOSED_ACCEPTED;
      $new_step = MEW_REVIEW_STEP_COMPLETED;
      $generate_pdf = TRUE;
    }
    elseif ($decision == MEW_REVIEW_DECISION_DENY) {
      $new_status = MEW_EVENT_STATUS_CLOSED_DENIED;
      $new_step = MEW_REVIEW_STEP_COMPLETED;
      $generate_pdf = TRUE;
    }

    $reviews = mcneese_event_workflow_get_latest_reviews($event_id, array(MEW_REVIEW_STEP_REVIEW), array(MEW_REVIEW_DECISION_APPROVE, MEW_REVIEW_DECISION_DENY));

    if (!empty($reviews)) {
      mcneese_event_workflow_include(4);

      $classification_options = event_workflow_get_reviewer_classification_list_options(NULL, TRUE);
      $preprocessed_decisions = array();

      $reviews_step = array();
      if (isset($reviews[MEW_REVIEW_STEP_REVIEW])) {
        $reviews_step = $reviews[MEW_REVIEW_STEP_REVIEW];
      }

      if (!empty($reviews_step)) {
        foreach ($reviews_step as $reviewer_classification => $reviewer_decisions) {
          if (!isset($classification_options[$reviewer_classification])) {
            continue;
          }

          $most_recent_decision = array_shift($reviewer_decisions);
          if ($most_recent_decision->decision == MEW_REVIEW_DECISION_APPROVE) {
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
    mcneese_event_workflow_workflow_set_status($event_id, $new_status, $changed_user);
  }

  if (!is_null($new_step)) {
    mcneese_event_workflow_workflow_set_step($event_id, $new_step, $changed_user);
  }

  $pdf_filename = NULL;
  if ($generate_pdf) {
    $success = mcneese_event_workflow_generate_pdf($event_id);

    if ($success) {
      $pdf_filename = mcneese_event_workflow_build_filename($event_id, 0);
    }
  }

  $parameters = array();
  $parameters['changed_by'] = $changed_by;
  $parameters['changed_type'] = $changed_type;
  $parameters['message'] = $message;

  if (!is_null($new_status) && $new_status != MEW_EVENT_STATUS_LOCKED) {
    $parameters['status'] = $new_status;
  }
  elseif ($new_step == MEW_REVIEW_STEP_REVIEW || $new_step == MEW_REVIEW_STEP_USAGE_FEES || $new_step == MEW_REVIEW_STEP_MAKE_DECISIONS) {
    $parameters['step'] = $new_step;
  }
  else {
    return TRUE;
  }

  mcneese_event_workflow_send_workflow_emails($event_id, $parameters);

  return TRUE;
}

/**
 * Process the change of an events status.
 *
 * @param int $event_id
 *   The event id.
 * @param int $status
 *   The new workflow status the event is going to.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_status_change_alert($event_id, $status, $user = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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

  mcneese_event_workflow_send_workflow_emails($event_id, $parameters);

  return TRUE;
}

/**
 * Sends e-mails about certain events.
 *
 * @param int $event_id
 *   The Event ID.
 * @param arary $parameters
 *   Array with the following keys:
 *   - changed_by: The Reviewer ID or User ID who changed the event request.
 *   - changed_type: Either 'reviewer', 'user', or 'system'.
 *   - message: A message to display in regards to the decision.
 *   - status: A taxonomy number representing the previous event request status.
 *     If NULL, then there is no old status or it is not to be processed.
 *   - step: A boolean representing whether or not to process as a new step.
 *   - new: A boolean representing whether or not this is new.
 *   - update: A boolean representing whether or not this was updated.
 *   - comment: A boolean representing whether or not this is a comment.
 *   - cancelled: A boolean representing whether or not the event was
 *     cancelled.
 *   - uncancelled: A boolean representing whether or not the event was
 *     uncancelled.
 *
 *   The keys: 'new', 'update', 'step', 'status', 'cancelled', and
 *   'uncancelled' are mutually exclusive.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_send_workflow_emails($event_id, $parameters) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
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

  $filename = NULL;
  if (array_key_exists('filename',  $parameters)) {
    $filename = $parameters['filename'];
    if (!is_string($filename)) {
      cf_error::invalid_string('parameters[filename]');
      return FALSE;
    }
  }

  $event_requester = user_load($event['mer']['user_id'][0]->value);

  // load appropriate reviewers in their own categories.
  $venue_coordinator = user_load($event['venue_coordinator']['user_id'][0]->value);

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event['event_coordinator']['classification'][0]->value, 'mr.review_step' => MEW_REVIEW_STEP_REVIEW, 'mr.disabled' => 0), TRUE);
  $step_1_reviewers = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $r) {
      $step_1_reviewers[$r->user_id] = $r;
    }
  }

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event['event_coordinator']['classification'][0]->value, 'mr.review_step' => MEW_REVIEW_STEP_USAGE_FEES, 'mr.disabled' => 0), TRUE);
  $step_2_reviewers = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $r) {
      $step_2_reviewers[$r->user_id] = $r;
    }
  }

  // prepare e-mail(s) for sending
  $email_params = array();
  $email_params['event_id'] = $event_id;
  #$email_params['filename'] = array($filename); // @todo: should files be attached?

  $email_from = '"McNeese Event Workflow System" <no-reply@mcneese.edu>';
  $email_to = array();

  $location_options = event_workflow_get_event_location_list_options();
  $building_options = event_workflow_get_event_building_list_options();

  $location = $event['top']['location'][0]->value;
  $building = $event['top']['building'][0]->value;
  $room = $event['top']['room'][0]->value;

  if (isset($location_options[$location])) {
    $location = $location_options[$location];
  }

  if (isset($building_options[$building])) {
    $building = $building_options[$building];
  }

  $standard_content = "\n\n";
  $standard_content .= "Request ID:\n - " . $event_id . "\n\n";
  $standard_content .= "Request Title:\n - " . $event['information']['title'][0]->value . "\n\n";
  $standard_content .= "Location:\n - " . $location . "\n\n";
  $standard_content .= "Building:\n - " . $building . "\n\n";
  $standard_content .= "Room:\n - " . $room . "\n\n";
  $standard_content .= "Dates:\n";
  foreach ($event['dates']['date'] as $key => $date) {
    $standard_content .= " - " . date("Y/m/d", $date->value);

    if (!is_null($event['dates']['time_start'][$key]->value)) {
      $standard_content .= " " . date("h:ia", $event['dates']['time_start'][$key]->value);
      $standard_content .= " to " . date("h:ia", $event['dates']['time_stop'][$key]->value);
    }

    $standard_content .= "\n";
  }

  $email_params['event_title'] = $event['information']['title'][0]->value;

  if ($is_new) {
    $email_params['title_suffix'] = "Requested";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has created this event request.\n";
    $email_params['content'] .= $standard_content;

    if (is_object($venue_coordinator)) {
      $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
    }
  }
  elseif ($is_update) {
    $email_params['title_suffix'] = "Modified";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has modified this event request.\n";
    $email_params['content'] .= $standard_content;

    // the update message only needs to be sent during the review process.
    if ($status == MEW_EVENT_STATUS_LOCKED || $status == MEW_EVENT_STATUS_UNLOCKED) {
      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      if ($event['top']['step'][0]->value == MEW_REVIEW_STEP_REVIEW) {
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }

      if ($event['top']['step'][0]->value == MEW_REVIEW_STEP_USAGE_FEES) {
        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
  }
  elseif ($is_cancelled || $is_uncancelled || !is_null($status)) {
    if ($is_cancelled || $is_uncancelled || $status == MEW_EVENT_STATUS_CANCELLED || $status == MEW_EVENT_STATUS_DELETED) {
      $email_params['content'] = "";

      if ($is_cancelled || $status == MEW_EVENT_STATUS_CANCELLED) {
        $email_params['title_suffix'] = "Cancelled";
        $email_params['content'] .= $changed_by_string . " has cancelled the event request.\n";
      }
      elseif ($is_uncancelled) {
        $email_params['title_suffix'] = "Uncancelled";
        $email_params['content'] .= $changed_by_string . " has uncancelled the event request.\n";
      }
      elseif ($status == MEW_EVENT_STATUS_DELETED) {
        $email_params['title_suffix'] = "Deleted";
        $email_params['content'] .= $changed_by_string . " has deleted the event request.\n";
      }

      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }

      if ($status != MEW_EVENT_STATUS_DELETED) {
        // send messages to all reviewers.
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }

        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
    elseif ($status == MEW_EVENT_STATUS_CLOSED_ACCEPTED) {
      $email_params['title_suffix'] = "Approved";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been approved.\n";
      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;

      // send messages to all reviewers.
      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($status == MEW_EVENT_STATUS_CLOSED_DENIED) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been denied.\n";
      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;

      // send messages to all reviewers.
      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($status == MEW_EVENT_STATUS_CLOSED_UNAVAILABLE || $status == MEW_EVENT_STATUS_CLOSED_DUE_TO_LOCK) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been denied due to the requested location not being available for the requested time slots.\n";
      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;
    }
  }
  elseif (!is_null($step)) {
    if ($event['top']['step'][0]->value == MEW_REVIEW_STEP_REVIEW) {
      $email_params['title_suffix'] = "Needs Review";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has made the event request available for review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

      foreach ($step_1_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($event['top']['step'][0]->value == MEW_REVIEW_STEP_REVIEW) {
      $email_params['title_suffix'] = "Needs Review";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request is available for review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($event['top']['step'][0]->value == MEW_REVIEW_STEP_USAGE_FEES) {
      $email_params['title_suffix'] = "Usage Fees";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request is ready to have its usage fees approved or denied.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($event['top']['step'][0]->value == MEW_REVIEW_STEP_MAKE_DECISIONS) {
      $email_params['title_suffix'] = "Reviewed";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been reviewed and is ready for conflict resolution / decision making / final review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

      if (is_object($venue_coordinator)) {
        $email_to[$venue_coordinator->mail] = $venue_coordinator->mail;
      }
    }
  }
  elseif ($is_comment) {
    if ($event['top']['status'][0]->value == MEW_EVENT_STATUS_LOCKED || $event['top']['status'][0]->value == MEW_EVENT_STATUS_UNLOCKED) {
      $email_params['title_suffix'] = "Comment";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has made the following comment:\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

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
    // DEBUG: temporarily disable e-mailing to prevent production accounts from receiving e-mails.
    //        only allow reserved accounts to receive e-mails
    $debug_accounts = array();
    $debug_accounts[] = 'kday@mcneese.edu';
    $debug_accounts[] = 'colleen@mcneese.edu';
    $debug_accounts[] = 'gfisher@mcneese.edu';
    $debug_accounts[] = 'stan@mcneese.edu';
    $debug_accounts[] = 'gbodin@mcneese.edu';
    $debug_accounts[] = 'shogan@mcneese.edu';

    foreach ($email_to as $to) {
      if (!in_array($to, $debug_accounts)) {
        drupal_set_message(t("Warning: not sending mail to @to while testing the system.", array('@to' => $to)), 'warning');
        watchdog('debug', "Warning: not sending mail to @to while testing the system.", array('@to' => $to), WATCHDOG_WARNING);
        continue;
      }

      drupal_mail('mcneese_event_workflow', 'event_request_alert', $to, language_default(), $email_params, $email_from, TRUE);
    }
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
