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

  $mew_event_requests = array();
  $mew_event_requests['updated'] = REQUEST_TIME;

  $mew_event_revisions = array();
  $mew_event_revisions['event_id'] = $event_id;
  $mew_event_revisions['date'] = REQUEST_TIME;

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

    $query = db_insert('mew_event_revisions');
    $query->fields($mew_event_revisions);
    $query->execute();

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
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

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
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_DELETED,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_ACCEPTED,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_REJECTED,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_UNAVAILABLE,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_DUE_TO_LOCK,
    MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CANCELLED,
  );

  if (!in_array($status, $status_options)) {
    watchdog('mew', "%status is not a valid status.", array('%status' => $status), WATCHDOG_ERROR);
    return FALSE;
  }

  $mew_event_requests = array();
  $mew_event_requests['updated'] = REQUEST_TIME;

  $mew_event_revisions = array();
  $mew_event_revisions['event_id'] = $event_id;
  $mew_event_revisions['date'] = REQUEST_TIME;

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

    $query = db_insert('mew_event_revisions');
    $query->fields($mew_event_revisions);
    $query->execute();

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
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

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
 * @param int $reviewer_id
 *   The reviewer id.
 * @param int $decision
 *   The event workflow decision.
 * @param int $message
 *   The event workflow decision message.
 * @param array $extra
 *   (optional) Additional fields that are specific to the individual step.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_step_review($event_id, $reviewer_id, $decision, $message, $extra = array(), $user = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    return FALSE;
  }

  if (!cf_is_integer($reviewer_id)) {
    cf_error::invalid_integer('reviewer_id');
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

  $step = $event['top']['step'][0]->value;

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.id' => $reviewer_id));

  if (empty($reviewers)) {
    cf_error::invalid_integer('reviewer_id');
    return FALSE;
  }

  $reviewer = array_pop($reviewers);

  $revision = mcneese_event_workflow_get_latest_revision($event_id, 'mew_field_review_review');

  if ($revision === FALSE) {
    $revision = 0;
  }

  $user_data = array();
  $user_data['user_id'] = $user->uid;

  $mew_event_requests = array(
    'updated' => REQUEST_TIME,
  );

  $mew_event_revisions = array(
    'event_id' => $event_id,
    'date' => REQUEST_TIME,
  );

  $mew_current_review = array(
    'event_id' => $event_id,
    'review' => $revision,
    'facilities_use_fee' => $event['current']['review']['facilities_use_fee'],
    'equipment_use_fee' => $event['current']['review']['equipment_use_fee'],
  );

  $mew_field_review_review = array(
    'event_id' => $event_id,
    'revision' => $revision,
    'delta' => 0,
    'user_id' => $reviewer->user_id,
    'date' => REQUEST_TIME,
    'reviewer' => $reviewer_id,
    'decision' => $decision,
    'message' => $message,
  );

  if (!empty($event['review']['review'])) {
    $mew_field_review_review['delta'] = count($event['review']['review']);
  }

  if ($step == 133) {
    $next_revision = $event['current']['review']['equipment_use_fee'] + 1;
    $mew_current_review['equipment_use_fee'] = $next_revision;
    $mew_field_review_equipment_use_fee = array(
      'event_id' => $event_id,
      'revision' => $next_revision,
      'delta' => 0,
      'user_id' => $reviewer->user_id,
      'date' => REQUEST_TIME,
      'value' => $extra['equipment_use_fee'],
    );

    $next_revision = $event['current']['review']['facilities_use_fee'] + 1;
    $mew_current_review['facilities_use_fee'] = $next_revision;
    $mew_field_review_facilities_use_fee = array(
      'event_id' => $event_id,
      'revision' => $next_revision,
      'delta' => 0,
      'user_id' => $reviewer->user_id,
      'date' => REQUEST_TIME,
      'value' => $extra['facilities_use_fee'],
    );
  }

  $failure = FALSE;
  $transaction = db_transaction();

  try {
    $query = db_update('mew_event_requests');
    $query->condition('id', $event_id);
    $query->fields($mew_event_requests);
    $query->execute();

    $query = db_insert('mew_event_revisions');
    $query->fields($mew_event_revisions);
    $query->execute();

    $query = db_insert('mew_field_review_review');
    $query->fields($mew_field_review_review);
    $query->execute();

    if ($step == 133) {
      $query = db_insert('mew_field_review_equipment_use_fee');
      $query->fields($mew_field_review_equipment_use_fee);
      $query->execute();

      $query = db_insert('mew_field_review_facilities_use_fee');
      $query->fields($mew_field_review_facilities_use_fee);
      $query->execute();
    }

    $query = db_update('mew_current_review');
    $query->condition('event_id', $event_id);
    $query->fields($mew_current_review);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($ex);
    }

    return FALSE;
  }

  // forcefully clear the event cache so that it can reflect the changes.
  event_workflow_load_event_by_id($event_id, TRUE);

  // handle comments and return without altering the workflow
  if ($decision == 113) {
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'reviewer';
    $parameters['message'] = $message;
    $parameters['comment'] = TRUE;
    mcneese_event_workflow_send_workflow_emails($event_id, $parameters);
    return TRUE;
  }

  // continue to next step only if at least 1 of each reviewer classification has submitted a reply.
  $event_status = $event['top']['status'][0]->value;
  $event_step = $event['top']['step'][0]->value;
  $event_classification = $event['coordinator']['classification'][0]->value;

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event_classification, 'mr.review_step' => $event_step));

  $required_reviews = count($reviewers);

  try {
    $query = db_select('mew_reviewers', 'mr');
    $query->innerjoin('mew_field_review_review', 'mfrr', 'mr.id = mfrr.reviewer');

    $query->addField('mr', 'reviewer_classification', 'reviewer_classification');

    $query->condition('mfrr.event_id', $event_id);
    $query->condition('mfrr.revision', $event['current']['review']['review']);
    $query->condition('mr.event_classification', $event_classification);
    $query->condition('mr.review_step', $event_step);
    $query->condition('mfrr.decision', array(114, 115), 'IN');

    $reviews = $query->execute()->fetchAll();
    $total_reviews = count($reviews);
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  if ($total_reviews < $required_reviews) {
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
  if ($event_step == 116) {
    if ($decision == 115) {
      $new_status = MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED;
      $new_step = 132;
    }
    elseif ($decision == 114) {
      $new_status = MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_UNAVAILABLE;
      $new_step = 134;
      $generate_pdf = TRUE;
    }
  }
  // Review Step
  elseif ($event_step == 132) {
    if ($decision == 114 || $decision == 115) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = 133;
    }
  }
  // Usage Fees Step
  elseif ($event_step == 133) {
    if ($decision == 114 || $decision == 115) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = 117;
    }
  }
  // Coordinator Review Step
  elseif ($event_step == 117) {
    if ($decision == 115) {
      $new_status = MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_ACCEPTED;
      $new_step = 134;
      $generate_pdf = TRUE;
    }
    elseif ($decision == 114) {
      $new_status = MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_REJECTED;
      $new_step = 134;
      $generate_pdf = TRUE;
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

  if (!is_null($new_status) && $new_status != MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED) {
    $parameters['status'] = $new_status;
  }
  elseif ($new_step == 132 || $new_step == 133 || $new_step == 117) {
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
    $reviewers = mcneese_event_workflow_get_reviewers(array('mr.user_id' => $parameters['changed_by']), TRUE);

    if (empty($reviewers)) {
      $changed_by = NULL;
    }
    else {
      $reviewer = array_pop($reviewers);
      $changed_by = user_load($reviewer->user_id);
    }
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
  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event['coordinator']['classification'][0]->value, 'mr.reviewer_classification' => 112), TRUE);
  $venue_coordinators = array();
  foreach ($reviewers as $r) {
    $venue_coordinators[$r->user_id] = $r;
  }

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event['coordinator']['classification'][0]->value, 'mr.review_step' => 132), TRUE);
  $step_1_reviewers = array();
  foreach ($reviewers as $r) {
    $step_1_reviewers[$r->user_id] = $r;
  }

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.event_classification' => $event['coordinator']['classification'][0]->value, 'mr.review_step' => 133), TRUE);
  $step_2_reviewers = array();
  foreach ($reviewers as $r) {
    $step_2_reviewers[$r->user_id] = $r;
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

    foreach ($venue_coordinators as $r) {
      $email_to[$r->email] = $r->email;
    }
  }
  elseif ($is_update) {
    $email_params['title_suffix'] = "Modified";
    $email_params['content'] = "";
    $email_params['content'] .= $changed_by_string . " has modified this event request.\n";
    $email_params['content'] .= $standard_content;

    // the update message only needs to be sent during the review process.
    if ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED) {
      foreach ($venue_coordinators as $r) {
        $email_to[$r->email] = $r->email;
      }

      if ($event['top']['step'][0]->value == 132) {
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }

      if ($event['top']['step'][0]->value == 133) {
        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
  }
  elseif ($is_cancelled || $is_uncancelled || !is_null($status)) {
    if ($is_cancelled || $is_uncancelled || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CANCELLED || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_DELETED) {
      $email_params['content'] = "";

      if ($is_cancelled || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CANCELLED) {
        $email_params['title_suffix'] = "Cancelled";
        $email_params['content'] .= $changed_by_string . " has cancelled the event request.\n";
      }
      elseif ($is_uncancelled) {
        $email_params['title_suffix'] = "Uncancelled";
        $email_params['content'] .= $changed_by_string . " has uncancelled the event request.\n";
      }
      elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_DELETED) {
        $email_params['title_suffix'] = "Deleted";
        $email_params['content'] .= $changed_by_string . " has deleted the event request.\n";
      }

      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;

      foreach ($venue_coordinators as $r) {
        $email_to[$r->email] = $r->email;
      }

      // send cancellation messages to all reviewers when an event is in a review step.
      if ($event['top']['step'][0]->value == 132 || $event['top']['step'][0]->value == 133) {
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }

        foreach ($step_2_reviewers as $r) {
          $email_to[$r->email] = $r->email;
        }
      }
    }
    elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_ACCEPTED) {
      $email_params['title_suffix'] = "Approved";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been approved.\n";
      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;
    }
    elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_REJECTED) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been denied.\n";
      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;
    }
    elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_UNAVAILABLE || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_DUE_TO_LOCK) {
      $email_params['title_suffix'] = "Rejected";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been rejected due to the requested location not being available for the requested time slots.\n";
      $email_params['content'] .= $standard_content;

      $email_to[$event_requester->mail] = $event_requester->mail;
    }
  }
  elseif (!is_null($step)) {
    if ($event['top']['step'][0]->value == 132) {
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
    elseif ($event['top']['step'][0]->value == 132) {
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
    elseif ($event['top']['step'][0]->value == 133) {
      $email_params['title_suffix'] = "Usage Fees";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request is ready to have its usage fees processed.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
    elseif ($event['top']['step'][0]->value == 117) {
      $email_params['title_suffix'] = "Reviewed";
      $email_params['content'] = "";
      $email_params['content'] .= "The event request has been reviewed and is ready for conflict resolution / decision making / final review.\n";
      $email_params['content'] .= $standard_content;

      foreach ($venue_coordinators as $r) {
        $email_to[$r->email] = $r->email;
      }
    }
  }
  elseif ($is_comment) {
    if ($event['top']['status'][0]->value == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED || $event['top']['status'][0]->value == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED) {
      $email_params['title_suffix'] = "Comment";
      $email_params['content'] = "";
      $email_params['content'] .= $changed_by_string . " has made the following comment:\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";
      $email_params['content'] .= $standard_content;

      foreach ($venue_coordinators as $r) {
        $email_to[$r->email] = $r->email;
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
    foreach ($email_to as $to) {
      drupal_mail('mcneese_event_workflow', 'event_request_alert', $to, language_default(), $email_params, $email_from, TRUE);
    }
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
