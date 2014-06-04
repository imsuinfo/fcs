<?php

/**
 * @file
 * Defines McNeese event workflow event cancel page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event cancel page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $event_id
 *   The unique identifier for an event request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_cancel_0_form($form, &$form_state, $event_id) {
  if (!cf_is_integer($event_id)) {
    cf_error::invalid_integer('event_id');
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    cf_error::invalid_array('event');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['event_id'] = array(
    '#type' => 'value',
    '#value' => $event_id,
  );

  $form['form']['event'] = array(
    '#type' => 'value',
    '#value' => &$event,
  );

  $form['form']['mode'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  $form['form']['pdf'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['message'] = array(
    '#markup' => '',
  );
  $form['message']['#markup'] .= '<h3>Are you sure wou want to cancel the event request?</h3> ';
  $form['message']['#markup'] .= '<div>';
  $form['message']['#markup'] .= 'Cancelling the form will remove your request from the queue. ';
  $form['message']['#markup'] .= '</div>';

  $form['display']['submit'] = array(
    '#id' => 'event-cancel-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'event-cancel-submit-no',
    '#type' => 'submit',
    '#value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'event-cancel-submit-yes',
    '#type' => 'submit',
    '#value' => t("Yes"),
  );

  return $form;
}

/**
 * Validation for mcneese_event_workflow_event_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_cancel_0_form()
 */
function mcneese_event_workflow_event_cancel_0_form_validate($form, &$form_state) {
}

/**
 * Submit process for mcneese_event_workflow_event_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_cancel_0_form()
 */
function mcneese_event_workflow_event_cancel_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  $event_id = (int) $form['form']['event_id']['#value'];

  if ($clicked_id == 'event-cancel-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($event_id)) {
        $form_state['redirect'] = 'events';
      }
      else {
        $form_state['redirect'] = 'events/view-0/' . $event_id;
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mcneese_event_workflow_include(6);

  $cancelled = mcneese_event_workflow_workflow_set_status($event_id, MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CANCELLED);

  if ($cancelled) {
    mcneese_event_workflow_workflow_status_change($event_id, NULL, MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_CANCELLED);
  }
  else {
    form_set_error('form', "An error occurred while trying to cancel the event request. Please contact the support staff.");
    watchdog('mew', "An error occured while trying to cancel the event request: @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  $parameters = array();
  $parameters['changed_by'] = $user->uid;
  $parameters['changed_type'] = 'user';
  $parameters['cancelled'] = TRUE;
  mcneese_event_workflow_send_workflow_emails($event_id, $parameters);

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($event_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      $form_state['redirect'] = 'events/list-0';
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the event workflow event uncancel page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $event_id
 *   The unique identifier for an event request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_uncancel_0_form($form, &$form_state, $event_id) {
  if (!cf_is_integer($event_id)) {
    cf_error::invalid_integer('event_id');
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    cf_error::invalid_array('event');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['event_id'] = array(
    '#type' => 'value',
    '#value' => $event_id,
  );

  $form['form']['event'] = array(
    '#type' => 'value',
    '#value' => &$event,
  );

  $form['form']['mode'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  $form['form']['pdf'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['message'] = array(
    '#markup' => '',
  );
  $form['message']['#markup'] .= '<h3>Are you sure wou want to uncancel the event request?</h3> ';
  $form['message']['#markup'] .= '<div>';
  $form['message']['#markup'] .= 'Uncancelling the form will add the request back to the queue in its prior status. ';
  $form['message']['#markup'] .= '</div>';

  $form['display']['submit'] = array(
    '#id' => 'event-uncancel-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'event-uncancel-submit-no',
    '#type' => 'submit',
    '#value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'event-uncancel-submit-yes',
    '#type' => 'submit',
    '#value' => t("Yes"),
  );

  return $form;
}

/**
 * Validation for mcneese_event_workflow_event_uncancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_uncancel_0_form()
 */
function mcneese_event_workflow_event_uncancel_0_form_validate($form, &$form_state) {
}

/**
 * Submit process for mcneese_event_workflow_event_uncancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_uncancel_0_form()
 */
function mcneese_event_workflow_event_uncancel_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  $event_id = (int) $form['form']['event_id']['#value'];

  if ($clicked_id == 'event-uncancel-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($event_id)) {
        $form_state['redirect'] = 'events';
      }
      else {
        $form_state['redirect'] = 'events/view-0/' . $event_id;
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mcneese_event_workflow_include(6);

  $previous = NULL;
  $uncancelled = TRUE;
  try {
    $query = db_select('mew_field_top_status', 'mfts');

    $query->fields('mfts');

    $query->condition('mfts.revision', $form['form']['event']['#value']['current']['top']['status'], '<');

    $query->range(0, 1);
    $query->orderby('mfts.revision', 'desc');

    $previous = $query->execute()->fetchAll();
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);

    $uncancelled = FALSE;
  }

  if (!isset($previous[0]) || !is_object($previous[0])) {
    $uncancelled = FALSE;
  }

  if ($uncancelled) {
    $uncancelled = mcneese_event_workflow_workflow_set_status($event_id, $previous[0]->value);
  }

  if ($uncancelled) {
    mcneese_event_workflow_workflow_status_change($event_id, NULL, MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNCANCELLED);
  }
  else {
    form_set_error('form', "An error occurred while trying to uncancel the event request. Please contact the support staff.");
    watchdog('mew', "An error occured while trying to uncancel the event request: @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  $parameters = array();
  $parameters['changed_by'] = $user->uid;
  $parameters['changed_type'] = 'user';
  $parameters['uncancelled'] = TRUE;
  mcneese_event_workflow_send_workflow_emails($event_id, $parameters);

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($event_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      $form_state['redirect'] = 'events/list-0';
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
