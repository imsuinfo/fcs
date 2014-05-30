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
 * Assign a new status to a given event request.
 *
 * @param int $id
 *   The id of the given event.
 * @param int $status
 *   The new status to assign to the specified event.
 * @param object|null $user
 *   If specified, the user to log this change as.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mcneese_event_workflow_workflow_set_status($id, $status, $user = NULL) {
  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');
    return FALSE;
  }

  if (!cf_is_integer($status)) {
    cf_error::invalid_integer('status');
    return FALSE;
  }

  $event = event_workflow_load_event_by_id($id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @id.", array('@id' => $id), WATCHDOG_ERROR);
    return FALSE;
  }

  $user = cf_current_user();

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

  $status_revision = $event['current']['top']['status'] + 1;

  $mew_current_top = array(
    'status' => $status_revision,
  );

  $mew_field_top_status = array(
    'event_id' => $id,
    'revision' => $status_revision,
    'delta' => 0,
    'user_id' => $user->uid,
    'date' => REQUEST_TIME,
    'value' => $status,
  );

  $transaction = db_transaction();

  try {
    $query = db_update('mew_event_requests');
    $query->condition('id', $id);
    $query->fields($mew_event_requests);
    $query->execute();

    $query = db_update('mew_current_top');
    $query->condition('event_id', $id);
    $query->fields($mew_current_top);
    $query->execute();

    $query = db_insert('mew_field_top_status');
    $query->fields($mew_field_top_status);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    return FALSE;
  }

  // @todo: at this point, trigger some function that handles workflow status changes.

  return TRUE;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
