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

  if (!in_array($step, $step_options)) {
    watchdog('mew', "%status is not a valid step.", array('%step' => $step), WATCHDOG_ERROR);
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
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_step_review($event_id, $reviewer_id, $decision, $message, $user = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    drupal_not_found();
    drupal_exit();
    return array();
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

  if (is_null($user)) {
    $user = cf_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  $reviewers = mcneese_event_workflow_get_reviewers(array('mr.id' => $reviewer_id));

  if (empty($reviewers)) {
    cf_error::invalid_integer('reviewer_id');
    return FALSE;
  }

  $reviewer = array_pop($reviewers);

  $user_data = array();
  $user_data['user_id'] = $user->uid;

  $mew_event_requests = array(
    'updated' => REQUEST_TIME,
  );

  $mew_event_revisions = array(
    'event_id' => $event_id,
    'date' => REQUEST_TIME,
  );

  $mew_field_top_review = array(
    'event_id' => $event_id,
    'revision' => 0,
    'delta' => 0,
    'user_id' => $reviewer->user_id,
    'date' => REQUEST_TIME,
    'reviewer_id' => $reviewer_id,
    'decision' => $decision,
    'message' => $message,
  );

  if (!empty($event['top']['review'])) {
    $mew_field_top_review['delta'] = count($event['top']['review']);

    // do not count the initial stub field
    if ($mew_field_top_review['delta'] == 1 && $event['top']['review'][0]->decision == 0) {
      $mew_field_top_review['delta'] = 0;
    }
  }

  $mew_current_top = array(
    'event_id' => $event_id,
  );

  $failure = FALSE;
  $transaction = db_transaction();

  try {
    // remove any stub reviews
    $query = db_delete('mew_field_top_review');
    $query->condition('decision', 0);
    $query->execute();

    // now perform the operations.
    $query = db_update('mew_event_requests');
    $query->condition('id', $event_id);
    $query->fields($mew_event_requests);
    $query->execute();

    $query = db_insert('mew_event_revisions');
    $query->fields($mew_event_revisions);
    $query->execute();

    $query = db_insert('mew_field_top_review');
    $query->fields($mew_field_top_review);
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
    $query->innerjoin('mew_field_top_review', 'mftr', 'mr.id = mftr.reviewer_id');

    $query->addField('mr', 'reviewer_classification', 'reviewer_classification');

    $query->condition('mftr.event_id', $event_id);
    $query->condition('mr.event_classification', $event_classification);
    $query->condition('mr.review_step', $event_step);
    $query->condition('mftr.decision', array(114, 115), 'IN');

    $total_reviews = $query->execute()->fetchField();

    drupal_set_message("DEBUG: total reviews = " . print_r($total_reviews, TRUE));

    $query->countQuery();

    $total_reviews = (int) $query->execute()->fetchField();
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  // forcefully clear the event cache so that it can reflect the changes.
  event_workflow_load_event_by_id($event_id, TRUE);

  if ($total_reviews < $required_reviews) {
    return;
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
  // Review Step 1
  elseif ($event_step == 132) {
    if ($decision == 114 || $decision == 115) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = 133;
    }
  }
  // Review Step 2
  elseif ($event_step == 133) {
    if ($decision == 114 || $decision == 115) {
      $changed_user = $system_user;
      $changed_by = $system_user->uid;
      $changed_type = 'system';
      $new_step = 117;
    }
  }
  // Review Step Final
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
 * @param null|int $from
 *   A previous workflow step the event is coming from.
 *   If NULL, will guess the from step using the event array.
 * @param int $to
 *   The new workflow state the event is going to.
 * @param object|null $user
 *   (optional) If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_status_change($event_id, $to, $user = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (!is_array($event)) {
    cf_error::invalid_array('event');
    return FALSE;
  }

  if (!cf_is_integer($to)) {
    cf_error::invalid_integer('to');
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
  $parameters['status'] = $to;

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
    cf_error::invalid_array('event');
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
    $reviewers = mcneese_event_workflow_get_reviewers(array('mr.id' => $parameters['changed_by']), TRUE);

    if (empty($reviewers)) {
      cf_error::invalid_integer('parameters[changed_by]');
      return FALSE;
    }

    $reviewer = array_pop($reviewers);
    $changed_by = user_load($reviewer->user_id);
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
    if (!is_integer($status)) {
      cf_error::invalid_integer('parameters[status]');
      return FALSE;
    }
  }

  $is_step = FALSE;
  if (array_key_exists('step',  $parameters)) {
    $is_step = $parameters['step'];
    if (!is_bool($is_step)) {
      cf_error::invalid_bool('parameters[step]');
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

  $email_from = 'no-reply@mcneese.edu';
  $email_to = array();

  $standard_content = "Event Request ID: " . $event_id . "\n";
  $standard_content .= "Event Request Title: " . $event['information']['title'][0]->value . "\n";
  $standard_content .= "Event Location: " . $event['top']['location'][0]->value;
  $standard_content .= ", " . $event['top']['building'][0]->value;

  if (!empty($event['top']['room'][0]->value)) {
    $standard_content .= " Room: " . $event['top']['room'][0]->value;
  }

  $standard_content .= "Event Dates:\n";
  foreach ($event['dates']['date'] as $key => $date) {
    $standard_content .= " - " . date("Y/m/d", $date->value);

    if (!is_null($event['dates']['time_start'][$key]->value)) {
      $standard_content .= " " . date("h:ia", $event['dates']['time_start'][$key]->value);
      $standard_content .= " to " . date("h:ia", $event['dates']['time_stop'][$key]->value);
    }

    $standard_content .= "\n";
  }

  $standard_content .= "\n";

  $email_params['event_title'] = $event['information']['title'][0]->value;

  if ($is_new) {
    $email_params['title_suffix'] = "Requested";
    $email_params['content'] = $standard_content;
    $email_params['content'] .= "\n";
    $email_params['content'] .= $changed_by_string . " has created this event request.\n";

    foreach ($venue_coordinators as $r) {
      $email_to[$r->user_id] = $r->email;
    }
  }
  elseif ($is_update) {
    $email_params['title_suffix'] = "Modified";
    $email_params['content'] = $standard_content;
    $email_params['content'] .= "\n";
    $email_params['content'] .= $changed_by_string . " has modified this event request.\n";

    // the update message only needs to be sent during the review process.
    if ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED) {
      foreach ($venue_coordinators as $r) {
        $email_to[$r->user_id] = $r->email;
      }

      if ($event['top']['step'][0]->value == 132) {
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->user_id] = $r->email;
        }
      }

      if ($event['top']['step'][0]->value == 133) {
        foreach ($step_2_reviewers as $r) {
          $email_to[$r->user_id] = $r->email;
        }
      }
    }
  }
  elseif ($is_cancelled || $is_uncancelled || !is_null($status)) {
    if ($is_cancelled || $is_uncancelled || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CANCELLED || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_DELETED) {
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";

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

      $email_to[$event_requester->uid] = $event_requester->mail;

      foreach ($venue_coordinators as $r) {
        $email_to[$r->user_id] = $r->email;
      }

      // send cancellation messages to all reviewers when an event is in a review step.
      if ($event['top']['step'][0]->value == 132 || $event['top']['step'][0]->value == 133) {
        foreach ($step_1_reviewers as $r) {
          $email_to[$r->user_id] = $r->email;
        }

        foreach ($step_2_reviewers as $r) {
          $email_to[$r->user_id] = $r->email;
        }
      }
    }
    elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_ACCEPTED) {
      $email_params['title_suffix'] = "Approved";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= "The event request has been approved.\n";

      $email_to[$event_requester->uid] = $event_requester->mail;
    }
    elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_REJECTED) {
      $email_params['title_suffix'] = "Denied";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= "The event request has been denied.\n";

      $email_to[$event_requester->uid] = $event_requester->mail;
    }
    elseif ($status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_UNAVAILABLE || $status == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CLOSED_DUE_TO_LOC) {
      $email_params['title_suffix'] = "Rejected";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= "The event request has been rejected due to the requested location not being available for the requested time slots.\n";

      $email_to[$event_requester->uid] = $event_requester->mail;
    }
  }
  elseif ($is_step) {
    if ($event['top']['step'][0]->value == 132) {
      $email_params['title_suffix'] = "Needs Review";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= $changed_by_string . " has made the event request available for review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";

      foreach ($step_1_reviewers as $r) {
        $email_to[$r->user_id] = $r->email;
      }
    }
    elseif ($event['top']['step'][0]->value == 132) {
      $email_params['title_suffix'] = "Needs Review";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= "The event request is available for review.\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->user_id] = $r->email;
      }
    }
    elseif ($event['top']['step'][0]->value == 117) {
      $email_params['title_suffix'] = "Reviewed";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= "The event request has been reviewed and is ready for conflict resolution / decision making / final review.\n";

      foreach ($venue_coordinators as $r) {
        $email_to[$r->user_id] = $r->email;
      }
    }
  }
  elseif ($is_comment) {
    if ($event['top']['status'][0]->value == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED || $event['top']['status'][0]->value == MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED) {
      $email_params['title_suffix'] = "Comment";
      $email_params['content'] = $standard_content;
      $email_params['content'] .= "\n";
      $email_params['content'] .= $changed_by_string . " has made the following comment:\n";
      $email_params['content'] .= "\n";
      $email_params['content'] .= $message . "\n";

      foreach ($venue_coordinators as $r) {
        $email_to[$r->user_id] = $r->email;
      }

      foreach ($step_1_reviewers as $r) {
        $email_to[$r->user_id] = $r->email;
      }

      foreach ($step_2_reviewers as $r) {
        $email_to[$r->user_id] = $r->email;
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
