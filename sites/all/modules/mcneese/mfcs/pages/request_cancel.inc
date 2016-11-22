<?php

/**
 * @file
 * Defines McNeese facilities use request cancel page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request cancel page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for a request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_cancel_0_form($form, &$form_state, $request_id) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    cf_error::invalid_array('request');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/cancel-0/' . $request_id);

  $page_title = 'Cancel Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Cancel Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['revision_id'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['form']['mode'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  $form['form']['contained'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['message'] = array(
    '#markup' => '',
  );
  $form['message']['#markup'] .= '<h3>Are you sure you want to cancel the request?</h3> ';
  $form['message']['#markup'] .= '<div>';
  $form['message']['#markup'] .= 'By cancelling this request you agree to forfeit the time slots that you have requested. You cannot undo this action. ' . '<br>';
  $form['message']['#markup'] .= '<br>';
  $form['message']['#markup'] .= 'Are you certain you want to cancel? ';
  $form['message']['#markup'] .= '</div>';

  $form['decision'] = array(
    '#tree' => TRUE,
  );

  $form['decision']['message'] = array(
    '#id' => 'request-cancel-message',
    '#type' => 'textarea',
    '#tree' => TRUE,
    '#title' => 'Log Message',
    '#default_value' => NULL,
    '#required' => FALSE,
    '#description' => 'Optionally provide a message in the log.'
  );

  $form['display']['submit'] = array(
    '#id' => 'request-cancel-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'request-cancel-submit-no',
    '#name' => 'request-cancel-submit-no',
    '#type' => 'submit',
    '#default_value' => 'No',
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'request-cancel-submit-yes',
    '#name' => 'request-cancel-submit-yes',
    '#type' => 'submit',
    '#default_value' => 'Yes',
  );

  return $form;
}

/**
 * Validation for mfcs_request_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_cancel_0_form()
 */
function mfcs_request_cancel_0_form_validate($form, &$form_state) {
}

/**
 * Submit process for mfcs_request_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_cancel_0_form()
 */
function mfcs_request_cancel_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-cancel-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  $extra = array();
  if (isset($form_state['values']['decision']['message'])) {
    $extra['revisions']['message'] = $form_state['values']['decision']['message'];
  }

  if (isset($form['form']['request']['top']['status'][0]->value) && $form['form']['request']['top']['status'][0]->value == MFCS_REQUEST_STATUS_CLOSED_ACCEPTED) {
    $cancelled = mfcs_workflow_set_status($request_id, MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED, NULL, $extra);
  }
  else {
    $cancelled = mfcs_workflow_set_status($request_id, MFCS_REQUEST_STATUS_CANCELLED, NULL, $extra);
  }

  if ($cancelled) {
    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_CANCEL, MFCS_REVIEW_RESTRICTIONS_NONE, $extra['revisions']['message']);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['cancelled'] = TRUE;
    mfcs_send_workflow_emails($request_id, $parameters);

    // perform check against this request to see if new problems are detected or existing problems are resolved.
    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
    mfcs_miscellaneous_check_requests_for_problems($request_id);
  }
  else {
    form_set_error('form', 'An error occurred while trying to cancel the request. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to cancel the request: @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/list-0');
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request uncancel page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for a request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_uncancel_0_form($form, &$form_state, $request_id) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    cf_error::invalid_array('request');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/uncancel-0/' . $request_id);

  $page_title = 'Uncancel Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Uncancel Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['revision_id'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['form']['mode'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  $form['form']['contained'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['message'] = array(
    '#markup' => '',
  );
  $form['message']['#markup'] .= '<h3>Are you sure you want to uncancel the request?</h3> ';
  $form['message']['#markup'] .= '<div>';
  $form['message']['#markup'] .= 'Uncancelling the form will add the request back to the queue in its prior status. ';
  $form['message']['#markup'] .= '</div>';

  $form['decision'] = array(
    '#tree' => TRUE,
  );

  $form['decision']['message'] = array(
    '#id' => 'request-cancel-message',
    '#type' => 'textarea',
    '#tree' => TRUE,
    '#title' => 'Log Message',
    '#default_value' => NULL,
    '#required' => FALSE,
    '#description' => 'Optionally provide a message in the log.'
  );

  $form['display']['submit'] = array(
    '#id' => 'request-uncancel-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'request-uncancel-submit-no',
    '#name' => 'request-uncancel-submit-no',
    '#type' => 'submit',
    '#default_value' => 'No',
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'request-uncancel-submit-yes',
    '#name' => 'request-uncancel-submit-yes',
    '#type' => 'submit',
    '#default_value' => 'Yes',
  );

  return $form;
}

/**
 * Validation for mfcs_request_uncancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_uncancel_0_form()
 */
function mfcs_request_uncancel_0_form_validate($form, &$form_state) {
}

/**
 * Submit process for mfcs_request_uncancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_uncancel_0_form()
 */
function mfcs_request_uncancel_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-uncancel-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  $previous = NULL;
  $uncancelled = TRUE;
  try {
    $query = db_select('mfcs_field_top_status', 'mfts');

    $query->fields('mfts');

    $query->condition('mfts.request_id', $request_id);
    $query->condition('mfts.revision', $form['form']['request']['#value']['current']['top']['status'], '<');
    $query->condition('mfts.value', MFCS_REQUEST_STATUS_CANCELLED, '<>');
    $query->condition('mfts.value', MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED, '<>');

    $query->range(0, 1);
    $query->orderBy('mfts.revision', 'desc');

    $previous = $query->execute()->fetchAll();
  }
  catch (Error $e) {
    cf_error::on_query_execution($e);

    $uncancelled = FALSE;
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);

    $uncancelled = FALSE;
  }

  if (!isset($previous[0]) || !is_object($previous[0])) {
    $uncancelled = FALSE;
  }

  $extra = array();
  if (isset($form_state['values']['decision']['message'])) {
    $extra['revisions']['message'] = $form_state['values']['decision']['message'];
  }

  if ($uncancelled) {
    $uncancelled = mfcs_workflow_set_status($request_id, $previous[0]->value, NULL, $extra);
  }

  if ($uncancelled) {
    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_UNCANCEL, MFCS_REVIEW_RESTRICTIONS_NONE, $extra['revisions']['message']);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['uncancelled'] = TRUE;
    mfcs_send_workflow_emails($request_id, $parameters);

    // perform check against this request to see if new problems are detected or existing problems are resolved.
    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
    mfcs_miscellaneous_check_requests_for_problems($request_id);
  }
  else {
    form_set_error('form', 'An error occurred while trying to uncancel the request. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to uncancel the request: @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // assign auto_cancel locked code.
  $locked_object = mfcs_load_request_locks($request_id, array('auto_cancel'));
  if ($locked_object !== FALSE) {
    if ($locked_object === TRUE || is_null($locked_object->auto_cancel)) {
      // when auto_cancel is set to NULL (or is not defined), set it to TRUE.
      // once auto_cancel lock is TRUE, the auto_cancel process will no longer attempt to auto-cancel a given request.
      mfcs_set_request_locks($request_id, array('auto_cancel' => 't'));
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/list-0');
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request cancel or uncancel page for individual dates.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for a request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_cancelling_0_form($form, &$form_state, $request_id) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request) || empty($request['dates']['date'])) {
    cf_error::invalid_array('request');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (count($request['dates']['date']) == 1) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/cancelling-0/' . $request_id);

  $page_title = 'Cancel or Uncancel Individual Dates for Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Cancel or Uncancel Individual Dates for Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $instance = mfcs_instance();

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['revision_id'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['message'] = array(
    '#markup' => '',
  );
  $form['message']['#markup'] .= '<h3>Which dates would you like to cancel or uncancel?</h3> ';
  $form['message']['#markup'] .= '<div>';
  $form['message']['#markup'] .= 'Once a date is cancelled, that request may no longer be available for uncancelling if another request takes up the time slot.' . '<br>';
  $form['message']['#markup'] .= '</div>';

  $form['dates'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#value' => array(),
  );

  foreach ($request['dates']['date'] as $key => $date) {
    $form['dates'][$key] = array(
      '#id' => 'request-cancelling-' . $key,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'cancelling-items',
        ),
      ),
    );

    $form['dates'][$key]['select'] = array(
      '#id' => 'request-cancelling-' . $key . '-select',
      '#type' => 'select',
      '#tree' => TRUE,
      '#options' => array(
        MFCS_CANCELLING_NOTHING => 'Do Nothing',
        MFCS_CANCELLING_CANCEL => 'Cancel Date',
        MFCS_CANCELLING_UNCANCEL => 'Uncancel Date',
        MFCS_CANCELLING_UNAVAILABLE => 'Unavailable',
      ),
      '#default' => MFCS_CANCELLING_NOTHING,
      '#attributes' => array(
        'class' => array(
          'form-item',
          'form-type-select',
          'form-item-' . $key . '-select',
          'cancelling-item-select',
        ),
      ),
    );

    $extra_css = '';
    if (isset($request['dates']['time_start'][$key]) && !is_null($request['dates']['time_start'][$key]->value) && $request['dates']['time_start'][$key]->value <= $instance) {
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_NOTHING]);
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_CANCEL]);
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_UNCANCEL]);
      $form['dates'][$key]['select']['#default'] = MFCS_CANCELLING_UNAVAILABLE;
      $form['dates'][$key]['select']['#readonly'] = TRUE;
      #$form['dates'][$key]['select']['#disabled'] = TRUE;

      $extra_css .= ' cancellable-unavailable';
    }
    elseif ($date->value <= $instance) {
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_NOTHING]);
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_CANCEL]);
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_UNCANCEL]);
      $form['dates'][$key]['select']['#default'] = MFCS_CANCELLING_UNAVAILABLE;
      $form['dates'][$key]['select']['#readonly'] = TRUE;
      #$form['dates'][$key]['select']['#disabled'] = TRUE;

      $extra_css .= ' cancellable-unavailable';
    }
    elseif ($date->cancelled == MFCS_CANCELLING_CANCEL) {
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_CANCEL]);
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_UNAVAILABLE]);

      $extra_css .= ' strike';
    }
    else {
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_UNCANCEL]);
      unset($form['dates'][$key]['select']['#options'][MFCS_CANCELLING_UNAVAILABLE]);
    }

    if (isset($request['dates']['time_start'][$key]) && !is_null($request['dates']['time_start'][$key]->value)) {
      $form['dates'][$key]['markup'] = array(
        '#type' => 'markup',
        '#markup' => '<span id="request-cancelling-' . $key . '-markup" class="cancelling-item-message' . $extra_css . '">' . date(MFCS_DATE_FORMAT_DATE, $date->value) . ' from ' . date(MFCS_DATE_FORMAT_TIME_SECONDS, $request['dates']['time_start'][$key]->value) . ' to ' . date(MFCS_DATE_FORMAT_TIME_SECONDS, $request['dates']['time_stop'][$key]->value) . '</span>',
      );
    }
    else {
      $form['dates'][$key]['markup'] = array(
        '#type' => 'markup',
        '#markup' => '<span id="request-cancelling-' . $key . '-markup" class="cancelling-item-message' . $extra_css . '">' . date(MFCS_DATE_FORMAT_DATE, $date->value) . ' (all day)</span>',
      );
    }
  }

  $form['submit'] = array(
    '#id' => 'request-cancelling-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['submit']['cancel'] = array(
    '#id' => 'cancelling-cancel',
    '#name' => 'cancelling-cancel',
    '#type' => 'submit',
    '#default_value' => 'Cancel',
  );

  $form['submit']['submit'] = array(
    '#id' => 'cancelling-submit',
    '#name' => 'cancelling-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
  );


  return $form;
}

/**
 * Validation for mfcs_request_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_cancelling_0_form()
 */
function mfcs_request_cancelling_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'cancelling-cancel') {
    // the cancel operation should not need to validate anything.
    return;
  }

  if (count($form['form']['request']['#value']['dates']['date']) == 1) {
    form_set_error('form', 'Individual dates may not be cancelled when a request only has a single date.');
    return;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $room = $form['form']['request']['#value']['top']['room'][0]->value;
  if (is_string($room)) {
    $room = (int) $room;
  }

  $total_dates = count($form_state['values']['dates']);
  $cancelling_dates = 0;
  $cancelled_dates = 0;

  $changes = FALSE;
  $holidays = array();
  foreach ($form_state['values']['dates'] as $key => $operation) {
    if ($operation['select'] == MFCS_CANCELLING_CANCEL) {
      $cancelling_dates++;
      $changes = TRUE;
      continue;
    }
    elseif ($operation['select'] == MFCS_CANCELLING_UNCANCEL) {
      // check for holidays.
      $day_start = strtotime('midnight today', $form['form']['request']['#value']['dates']['date'][$key]->value);

      if (!array_key_exists($day_start, $holidays)) {
        $holidays[$day_start] = mfcs_get_holidays((int) date('Y', $day_start), $day_start);

        if ($holidays[$day_start] === FALSE) {
          watchdog(MFCS_WATCHDOG_ID, 'Failed to load holidays for year @year and timestamp @timestamp (based on @date).', array('@year' => (int) date('Y', $day_start), '@timestamp' => $day_start, '@date' => $form['form']['request']['#value']['dates']['date'][$key]->value));
        }
      }

      if (!empty($holidays[$day_start])) {
        $holiday = reset($holidays[$day_start]);
        drupal_set_message(t('Warning: <strong>@date</strong> may not available because of: <strong><a href="' . $base_path . 'requests/holiday-0/view/' . $holiday->year . '/' . $holiday->id . $url_arguments . '" title="View details about holiday.">@holiday</a></strong>.', array('@year' => date('Y', $holiday->date), '@date' => date(MFCS_DATE_FORMAT_DATE, $holiday->date), '@holiday' => $holiday->title)), 'warning');
      }
      unset($holiday);
      unset($day_start);


      // check room unavailability.
      $room_unavailability = mfcs_check_room_availability($room, array($form['form']['request']['#value']['dates']['date'][$key]->value), array($form['form']['request']['#value']['dates']['time_start'][$key]->value), array($form['form']['request']['#value']['dates']['time_stop'][$key]->value));
      if ($room_unavailability === FALSE) {
        form_set_error('dates][' . $key . '][select', 'An internal error occurred while checking the room for availability. Please contact the support staff.');
      }
      elseif (!empty($room_unavailability['conflicts']) || !empty($room_unavailability['unavailable'])) {
        $time = date(MFCS_DATE_FORMAT_DATE, $form['form']['request']['#value']['dates']['date'][$key]->value);
        if (!is_null($form['form']['request']['#value']['dates']['time_start'][$key]->value)) {
          $time .= ' from ' . date(MFCS_DATE_FORMAT_TIME_SECONDS, $form['form']['request']['#value']['dates']['time_start'][$key]->value);
          $time .= ' to ' . date(MFCS_DATE_FORMAT_TIME_SECONDS, $form['form']['request']['#value']['dates']['time_stop'][$key]->value);
        }

        $unavailable_message = '<span class="uppercase">Conflict</span>: The room is no longer available for <strong>' .  $time . '</strong> and therefore may not be uncancelled because of the following:<br>';
        $unavailable_message .= mfcs_build_room_unavailable_message($room_unavailability);
        form_set_error('dates][' . $key . '][select', $unavailable_message);
      }

      $changes = TRUE;
      continue;
    }
    elseif (isset($form['form']['request']['#value']['dates']['date'][$key]->cancelled) && $form['form']['request']['#value']['dates']['date'][$key]->cancelled == 1) {
      // keep a count of already cancelled dates so that the total number of dates cancelled and to be cancelled may be properly checked.
      $cancelled_dates++;
    }
  }

  if ($changes) {
    if (($cancelling_dates + $cancelled_dates) == $total_dates) {
      form_set_error('form', 'Cannot cancel all dates. If you want to cancel every date, then you must cancel the entire request and not any individual dates/times.');
    }
  }
  else {
    form_set_error('form', 'Unable to submit, no changes have been designated.');
  }
}

/**
 * Submit process for mfcs_request_cancel_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_cancelling_0_form()
 */
function mfcs_request_cancelling_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'cancelling-cancel') {
    // do nothing
  }
  else {
    $user = cf_current_user();
    $instance = mfcs_instance();

    $request_revision = mfcs_load_request_revision_number($request_id);
    $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

    if ($request_revision === FALSE) {
      form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'Failed to load revision number for request %request_id while trying to save the request.', array('%request_id' => $request_id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;
      return;
    }
    elseif (is_null($request_revision)) {
      $request_revision_next = 0;
    }
    else {
      $request_revision_next = $request_revision + 1;
    }

    $dates_date_revision = mfcs_load_request_revision_number($request_id, 'mfcs_field_dates_date');
    if ($dates_date_revision === FALSE) {
      form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'Failed to load revision number for the dates date table of request %request_id while trying to save the request.', array('%request_id' => $request_id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;
      return;
    }
    elseif (is_null($dates_date_revision)) {
      $dates_date_revision_next = 0;
    }
    else {
      $dates_date_revision_next = $dates_date_revision + 1;
    }

    $do_cancel = array();
    $do_uncancel = array();
    $mfcs_field_dates_date_fields = array();

    $mfcs_requests = array(
      'updated' => $instance,
    );

    $mfcs_request_revisions = array(
      'request_id' => $request_id,
      'revision' => $request_revision_next,
      'user_id' => $user->uid,
      'date' => $instance,
      'message' => 'Individual dates have been cancelled or uncancelled.',
    );

    $mfcs_current = array(
      'dates_date' => $dates_date_revision_next,
    );

    $dates = &$form_state['values']['form']['request']['dates'];
    foreach ($form_state['values']['dates'] as $key => $operation) {
      $mfcs_field_dates_date_fields[$key] = array(
        'request_id' => $request_id,
        'revision' => $dates_date_revision_next,
        'delta' => $key,
        'user_id' => $user->uid,
        'date' => $instance,
        'value' => $dates['date'][$key]->value,
        'value_month' => $dates['date'][$key]->value_month,
        'value_day' => $dates['date'][$key]->value_day,
        'cancelled' => $dates['date'][$key]->cancelled,
      );

      if (!isset($dates['date'][$key])) {
        continue;
      }

      if ($operation['select'] == MFCS_CANCELLING_CANCEL) {
        $do_cancel[$key] = date(MFCS_DATE_FORMAT_DATE, $dates['date'][$key]->value);

        if (isset($dates['time_start'][$key]->value) && !is_null($dates['time_start'][$key]->value)) {
          $do_cancel[$key] .= ' from ' . date(MFCS_DATE_FORMAT_TIME, $dates['time_start'][$key]->value) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $dates['time_stop'][$key]->value);
        }
        else {
          $do_cancel[$key] .= ' (all day)';
        }

        $mfcs_field_dates_date_fields[$key]['cancelled'] = 1;
      }
      elseif ($operation['select'] == MFCS_CANCELLING_UNCANCEL) {
        $do_uncancel[$key] = date(MFCS_DATE_FORMAT_DATE, $dates['date'][$key]->value);

        if (isset($dates['time_start'][$key]->value) && !is_null($dates['time_start'][$key]->value)) {
          $do_uncancel[$key] .= ' from ' . date(MFCS_DATE_FORMAT_TIME, $dates['time_start'][$key]->value) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $dates['time_stop'][$key]->value);
        }
        else {
          $do_uncancel[$key] .= ' (all day)';
        }

        $mfcs_field_dates_date_fields[$key]['cancelled'] = 0;
      }
    }
    unset($dates);

    mfcs_include(MFCS_INCLUDE_WORKFLOW);

    if (!empty($do_cancel) || !empty($do_uncancel)) {
      $transaction = db_transaction();
      try {
        $query = db_insert('mfcs_request_revisions');
        $query->fields($mfcs_request_revisions);
        $query->execute();

        foreach ($mfcs_field_dates_date_fields as $mfcs_field_dates_date) {
          $query = db_insert('mfcs_field_dates_date');
          $query->fields($mfcs_field_dates_date);
          $query->execute();
        }

        $query = db_update('mfcs_requests');
        $query->fields($mfcs_requests);
        $query->condition('id', $request_id);
        $query->execute();

        $query = db_update('mfcs_current');
        $query->fields($mfcs_current);
        $query->condition('request_id', $request_id);
        $query->execute();

        // enforce transaction execution
        unset($transaction);
      }
      catch (Error $e) {
        $transaction->rollback();

        form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');

        cf_error::on_query_execution($e);

        $form_state['rebuild'] = TRUE;
        $form_state['redirect'] = FALSE;
        $form_state['submitted'] = FALSE;
        return;
      }
      catch (Exception $e) {
        $transaction->rollback();

        form_set_error('form', 'An error occurred while trying to save the request. Please contact the support staff.');

        cf_error::on_query_execution($e);

        $form_state['rebuild'] = TRUE;
        $form_state['redirect'] = FALSE;
        $form_state['submitted'] = FALSE;
        return;
      }

      // reset the cache so that the next use of the request information will reflect the changes.
      mfcs_load_request_by_id($request_id, FALSE);


      $parameters = array();
      $parameters['changed_by'] = $user->uid;
      $parameters['changed_type'] = 'user';
      $parameters['cancelling'] = array(
        'content' => '',
        'markup' => '',
      );

      $parameters['cancelling']['content'] .= "\n" . 'The following dates have just been cancelled:' . "\n";
      $parameters['cancelling']['markup'] .= '<br>The following dates have just been <em>cancelled</em>:';
      $parameters['cancelling']['markup'] .= '<ul>';

      if (empty($do_cancel)) {
        $parameters['cancelling']['content'] .= ' - No dates have been cancelled.' . "\n";
        $parameters['cancelling']['markup'] .= '<li>No dates have been cancelled.</li>';
      }
      else {
        foreach ($do_cancel as $key => $content) {
          $parameters['cancelling']['content'] .= ' - ' . $content . "\n";
          $parameters['cancelling']['markup'] .= '<li>' . $content . '</li>';
        }
      }

      $parameters['cancelling']['markup'] .= '</ul>';

      $parameters['cancelling']['content'] .= "\n" . 'The following dates have just been uncancelled:' . "\n";
      $parameters['cancelling']['markup'] .= '<br>The following dates have just been <em>uncancelled</em>:';
      $parameters['cancelling']['markup'] .= '<ul>';

      if (empty($do_uncancel)) {
        $parameters['cancelling']['content'] .= ' - No dates have been uncancelled.' . "\n";
        $parameters['cancelling']['markup'] .= '<li>No dates have been uncancelled.</li>';
      }
      else {
        foreach ($do_uncancel as $key => $content) {
          $parameters['cancelling']['content'] .= ' - ' . $content . "\n";
          $parameters['cancelling']['markup'] .= '<li>' . $content . '</li>';
        }
      }

      $parameters['cancelling']['markup'] .= '</ul>';

      mfcs_send_workflow_emails($request_id, $parameters);
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
