<?php

/**
 * @file
 * Defines McNeese facilities use event cancel page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use event cancel page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for an event request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_event_cancel_0_form($form, &$form_state, $request_id) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($request_id);

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

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
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
    '#name' => 'event-cancel-submit-no',
    '#type' => 'submit',
    '#default_value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'event-cancel-submit-yes',
    '#name' => 'event-cancel-submit-yes',
    '#type' => 'submit',
    '#default_value' => t("Yes"),
  );

  return $form;
}

/**
 * Validation for mfcs_event_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_cancel_0_form()
 */
function mfcs_event_cancel_0_form_validate($form, &$form_state) {
}

/**
 * Submit process for mfcs_event_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_cancel_0_form()
 */
function mfcs_event_cancel_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'event-cancel-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = 'events';
      }
      else {
        $form_state['redirect'] = 'events/view-0/' . $request_id;
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mfcs_include(6);

  $cancelled = mfcs_workflow_set_status($request_id, MFCS_EVENT_STATUS_CANCELLED);

  if ($cancelled) {
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['cancelled'] = TRUE;
    mfcs_send_workflow_emails($request_id, $parameters);
  }
  else {
    form_set_error('form', "An error occurred while trying to cancel the event request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to cancel the event request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
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
 * Provides the facilities use event uncancel page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for an event request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_event_uncancel_0_form($form, &$form_state, $request_id) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($request_id);

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

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
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
    '#name' => 'event-uncancel-submit-no',
    '#type' => 'submit',
    '#default_value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'event-uncancel-submit-yes',
    '#name' => 'event-uncancel-submit-yes',
    '#type' => 'submit',
    '#default_value' => t("Yes"),
  );

  return $form;
}

/**
 * Validation for mfcs_event_uncancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_uncancel_0_form()
 */
function mfcs_event_uncancel_0_form_validate($form, &$form_state) {
}

/**
 * Submit process for mfcs_event_uncancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_uncancel_0_form()
 */
function mfcs_event_uncancel_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'event-uncancel-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = 'events';
      }
      else {
        $form_state['redirect'] = 'events/view-0/' . $request_id;
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mfcs_include(6);

  $previous = NULL;
  $uncancelled = TRUE;
  try {
    $query = db_select('mfcs_field_top_status', 'mfts');

    $query->fields('mfts');

    $query->condition('mfts.request_id', $request_id);
    $query->condition('mfts.revision', $form['form']['event']['#value']['current']['top']['status'], '<');

    $query->range(0, 1);
    $query->orderBy('mfts.revision', 'desc');

    $previous = $query->execute()->fetchAll();
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    $uncancelled = FALSE;
  }

  if (!isset($previous[0]) || !is_object($previous[0])) {
    $uncancelled = FALSE;
  }

  if ($uncancelled) {
    $uncancelled = mfcs_workflow_set_status($request_id, $previous[0]->value);
  }

  if ($uncancelled) {
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['uncancelled'] = TRUE;
    mfcs_send_workflow_emails($request_id, $parameters);
  }
  else {
    form_set_error('form', "An error occurred while trying to uncancel the event request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to uncancel the event request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

