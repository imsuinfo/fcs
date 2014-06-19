<?php

/**
 * @file
 * Defines McNeese event workflow event history page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event view page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $event_id
 *   The unique identifier for an event request.
 * @param int $page_number
 *   (optional) Page number to view results.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_history_0_page($form, &$form_state, $event_id, $page_number = 0, $group = NULL, $field = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $user = cf_current_user();

  $administer = user_access('mcneese event workflow administer');
  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];


  try {
    $query = db_select('mew_event_revisions', 'merv');

    $query->condition('merv.event_id', $event_id);
  }
  catch (Exception $ex) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($ex);
    }

    return FALSE;
  }


  return $form;
}


/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
