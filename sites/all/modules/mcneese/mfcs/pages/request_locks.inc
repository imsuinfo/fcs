<?php

/**
 * @file
 * Defines McNeese facilities use request locks page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request locks page.
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
function mfcs_request_manage_0_locks_form($form, &$form_state, $request_id) {
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
  }

  $user = cf_current_user();

  mfcs_add_canonical_headers('requests/manage-0/locks/' . $request_id);

  $page_title = 'Request Locks';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Locks for Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $locks = mfcs_load_request_locks($request_id);
  if ($locks === FALSE) {
    watchdog(MFCS_WATCHDOG_ID, 'Failed to load the request locks for request @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['locks'] = array(
    '#tree' => TRUE,
  );

  $options_locks = array(
    'default' => 'Default',
    'locked' => 'Locked',
    'unlocked' => 'Unlocked',
  );

  $default_auto_cancel = 'default';
  if (is_object($locks)) {
    if ($locks->auto_cancel === FALSE) {
      $default_auto_cancel = 'unlocked';
    }
    elseif ($locks->auto_cancel === TRUE) {
      $default_auto_cancel = 'locked';
    }
  }

  $form['locks']['auto_cancel'] = array(
    '#id' => 'request-lock-auto_cancel',
    '#type' => 'select',
    '#title' => 'Auto-Cancel',
    '#options' => $options_locks,
    '#default_value' => $default_auto_cancel,
    '#description' => '',
  );

  $form['locks']['auto_cancel']['#description'] .= 'When <strong>enabled (locked)</strong>, the auto-cancel process will be prohibited from auto-locking this request.<br>';
  $form['locks']['auto_cancel']['#description'] .= 'When set to <strong>default</strong>, the auto-cancel process will be allowed, but uncancelling a request will cause auto-cancel to become locked.<br>';
  $form['locks']['auto_cancel']['#description'] .= 'When <strong>disabled (unlocked)</strong>, the auto-cancel process will be allowed, but uncancelling a request will never cause auto-cancel to become locked.';

  $form['display']['submit'] = array(
    '#id' => 'request-lock-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['cancel'] = array(
    '#id' => 'request-lock-submit-cancel',
    '#name' => 'request-lock-submit-cancel',
    '#type' => 'submit',
    '#default_value' => 'Cancel',
  );

  $form['display']['submit']['submit'] = array(
    '#id' => 'request-lock-submit-submit',
    '#name' => 'request-lock-submit-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
  );

  return $form;
}

/**
 * Validate process for mfcs_request_manage_0_locks_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_locks_form()
 */
function mfcs_request_manage_0_locks_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-lock-submit-cancel') {
    // cancelling does not need validation.
    return;
  }

  $changes = FALSE;
  if ($form['locks']['auto_cancel']['#default_value'] != $form_state['values']['locks']['auto_cancel']) {
    $changes = TRUE;
  }

  if (!$changes) {
    form_set_error('locks][auto_cancel', 'No changes have been specified.');
  }
}

/**
 * Submit process for mfcs_request_manage_0_locks_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_locks_form()
 */
function mfcs_request_manage_0_locks_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-lock-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($request_id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/manage-0/' . $request_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  $auto_cancel = NULL;
  $auto_cancel_name = 'Default';
  if ($form_state['values']['locks']['auto_cancel'] == 'unlocked') {
    $auto_cancel = 'f';
    $auto_cancel_name = 'Unlocked';
  }
  elseif ($form_state['values']['locks']['auto_cancel'] == 'locked') {
    $auto_cancel = 't';
    $auto_cancel_name = 'Locked';
  }

  $result = mfcs_set_request_locks($request_id, array('auto_cancel' => $auto_cancel));
  if (!$result) {
    form_set_error('form', 'An error occurred while trying to update the request\'s locks. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to update the request\'s locks for the request: @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }


  // record the change in the request's history log.
  $extra['revisions']['message'] = 'Changed the auto-cancel lock from ';

  if ($form['locks']['auto_cancel']['#default_value'] == 'default') {
    $extra['revisions']['message'] .= 'Default';
  }
  elseif ($form['locks']['auto_cancel']['#default_value'] == 'locked') {
    $extra['revisions']['message'] .= 'Locked';
  }
  elseif ($form['locks']['auto_cancel']['#default_value'] == 'unlocked') {
    $extra['revisions']['message'] .= 'Unlocked';
  }

  $extra['revisions']['message'] .= ' to ' . $auto_cancel_name . '.';

  mfcs_workflow_set_history($request_id, $user, $extra);

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/manage-0/' . $request_id);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
