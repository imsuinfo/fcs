<?php

/**
 * @file
 * Defines McNeese facilities use request override page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request override page.
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
function mfcs_request_manage_0_override_form($form, &$form_state, $request_id) {
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

  $user = cf_current_user();

  $can_override_request = mfcs_request_access('override', $request, $user);
  if (!$can_override_request) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/manage-0/override/' . $request_id);

  $page_title = "Override Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Override Request: " . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $current_step = (int) $request['top']['step'][0]->value;
  $current_status = (int) $request['top']['status'][0]->value;

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

  $form['form']['request_step'] = array(
    '#type' => 'value',
    '#value' => $current_step,
  );

  $form['form']['request_status'] = array(
    '#type' => 'value',
    '#value' => $current_status,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['message'] = array(
    '#markup' => '',
  );

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $all_steps = mfcs_get_review_steps_list_options();
  $all_status = mfcs_get_request_status_list_options();

  $options_steps = mfcs_get_review_steps_list_options('select');
  $options_status = mfcs_get_request_status_list_options('select');

  // do not allow assigning the following steps.
  unset($options_steps[MFCS_REVIEW_STEP_CHANGES_REQUIRED]);
  unset($options_steps[MFCS_REVIEW_STEP_FINAL_APPROVAL]);
  unset($options_steps[MFCS_REVIEW_STEP_RELEASE_HOLD]);

  // do not allow assigning the following statuses.
  unset($options_status[MFCS_REQUEST_STATUS_DELETED]);
  unset($options_status[MFCS_REQUEST_STATUS_CANCELLED]);
  unset($options_status[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED]);
  unset($options_status[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK]);

  $form['message']['#markup'] .= '<div class="override-description-changes">';
  $form['message']['#markup'] .= 'This will force the review process into a new step.<br>';
  $form['message']['#markup'] .= 'Under some circumstances, the status should should also be changed (otherwise status should be left alone).<br>';
  $form['message']['#markup'] .= '<strong class="override-description-caution">Use this with caution.</strong><br>';
  $form['message']['#markup'] .= '</div>';

  $form['message']['#markup'] .= '<br>';

  $form['message']['#markup'] .= '<div class="override-description-existing">';
  $form['message']['#markup'] .= 'The current step is: <strong class="override-current override-current-step">' . $all_steps[$current_step] . '</strong>.<br>';
  $form['message']['#markup'] .= 'The current status is: <strong class="override-current override-current-status">' . $all_status[$current_status] . '</strong>.<br>';
  $form['message']['#markup'] .= '</div>';

  $form['decision'] = array(
    '#tree' => TRUE,
  );

  $form['decision']['step'] = array(
    '#id' => 'request-override-step',
    '#type' => 'select',
    '#tree' => TRUE,
    '#title' => 'New Step',
    '#options' => $options_steps,
    '#default_value' => NULL,
    '#required' => TRUE,
    '#description' => "This is a listing of all steps available on the system."
  );

  $form['decision']['status'] = array(
    '#id' => 'request-override-status',
    '#type' => 'select',
    '#tree' => TRUE,
    '#title' => 'New Status',
    '#options' => $options_status,
    '#default_value' => NULL,
    '#required' => FALSE,
    '#description' => "This is a listing of all (common) statuses available on the system.<br>When assigning a step of <em>Completed</em> the status of <em>Accepted</em> or <em>Denied</em> should also be specified."
  );

  if ($current_status === MFCS_REQUEST_STATUS_CANCELLED || $current_status === MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED) {
    $form['decision']['status']['#disabled'] = TRUE;
    $form['decision']['status']['#options'] = array(NULL => '- Unavailable -');
    $form['decision']['status']['#description'] = "The request is currently cancelled. To assign a new status, please first uncancel the request.";
  }

  $form['decision']['message'] = array(
    '#id' => 'request-override-message',
    '#type' => 'textarea',
    '#tree' => TRUE,
    '#title' => 'Log Message',
    '#default_value' => NULL,
    '#required' => FALSE,
    '#description' => "Optionally provide a message to display in the logs."
  );

  $form['display']['submit'] = array(
    '#id' => 'request-override-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['cancel'] = array(
    '#id' => 'request-override-submit-cancel',
    '#name' => 'request-override-submit-cancel',
    '#type' => 'submit',
    '#default_value' => t("Cancel"),
  );

  $form['display']['submit']['submit'] = array(
    '#id' => 'request-override-submit-submit',
    '#name' => 'request-override-submit-submit',
    '#type' => 'submit',
    '#default_value' => t("Submit"),
  );

  return $form;
}

/**
 * Pre-Validation for mfcs_request_manage_0_override_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_override_form()
 */
function mfcs_request_manage_0_override_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-override-submit-cancel') {
    $form['decision']['step']['#required'] = FALSE;
  }
}

/**
 * Validation for mfcs_request_manage_0_override_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_override_form()
 */
function mfcs_request_manage_0_override_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-override-submit-cancel') {
    // cancelling does not need validation.
    return;
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $options_steps = mfcs_get_review_steps_list_options();
  $options_status = mfcs_get_request_status_list_options();

  if ($form_state['values']['decision']['step'] == MFCS_REVIEW_STEP_COMPLETED) {
    if ($form_state['values']['decision']['status'] == '') {
      if ($form['form']['request_status']['#value'] != $options_status[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] && $form['form']['request_status']['#value'] != $options_status[MFCS_REQUEST_STATUS_CLOSED_DENIED] && !$form['form']['request_status']['#value'] == $options_status[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE]) {
        form_set_error('decision][status', 'Only <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] . '</strong>, <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_DENIED] . '</strong>, or <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] . '</strong> may be used when step is set to <strong>' . $options_steps[MFCS_REVIEW_STEP_COMPLETED] . '</strong>.');
      }
    }
    elseif ($form_state['values']['decision']['status'] != MFCS_REQUEST_STATUS_CLOSED_ACCEPTED && $form_state['values']['decision']['status'] != MFCS_REQUEST_STATUS_CLOSED_DENIED && $form_state['values']['decision']['status'] != MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE) {
      form_set_error('decision][status', 'Only <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] . '</strong>, <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_DENIED] . '</strong>, or <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] . '</strong> may be used when step is set to <strong>' . $options_steps[MFCS_REVIEW_STEP_COMPLETED] . '</strong>.');
    }
  }
  else {
    if ($form_state['values']['decision']['status'] == '') {
      if ($form['form']['request_status']['#value'] == $options_status[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] || $form['form']['request_status']['#value'] == $options_status[MFCS_REQUEST_STATUS_CLOSED_DENIED] || $form['form']['request_status']['#value'] == $options_status[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE]) {
        form_set_error('decision][status', 'Status must not be <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] . '</strong>, <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_DENIED] . '</strong>, or <strong>' . $options_status[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] . '</strong> when step is set to <strong>' . $options_steps[MFCS_REVIEW_STEP_COMPLETED] . '</strong>.');
      }
    }
    elseif ($form_state['values']['decision']['status'] == MFCS_REQUEST_STATUS_CLOSED_ACCEPTED || $form_state['values']['decision']['status'] == MFCS_REQUEST_STATUS_CLOSED_DENIED || $form_state['values']['decision']['status'] == MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE) {
      form_set_error('decision][status', '<strong>' . $options_status[$form_state['values']['decision']['status']] . '</strong> may only be used when the step is set to <strong>' . $options_steps[MFCS_REVIEW_STEP_COMPLETED] . '</strong>.');
    }
  }
}

/**
 * Submit process for mfcs_request_manage_0_override_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_override_form()
 */
function mfcs_request_manage_0_override_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-override-submit-cancel') {
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

  $message = NULL;
  if (isset($form_state['values']['decision']['message'])) {
    $message = $form_state['values']['decision']['message'];
  }

  $step = (int) $form_state['values']['decision']['step'];
  $status = NULL;
  if (isset($form_state['values']['decision']['status']) && $form_state['values']['decision']['status'] != '') {
    $status = (int) $form_state['values']['decision']['status'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $overrided = mfcs_workflow_force_step($request_id, $step, $status, MFCS_REVIEW_RESTRICTIONS_MANAGERS, $message, FALSE);
  if ($overrided) {
    $options_steps = mfcs_get_review_steps_list_options();
    $options_status = mfcs_get_request_status_list_options();

    if (is_null($status)) {
      $message = "Forcibly altered the workflow step from " . $options_steps[$form['form']['request_step']['#value']] . " to " . $options_steps[$step] . ".";
    }
    else {
      $message = "Forcibly altered the workflow step from " . $options_steps[$form['form']['request_step']['#value']] . " to " . $options_steps[$step] . " and status from " . $options_status[$form['form']['request_status']['#value']] . " to " . $options_status[$status] . ".";
    }
    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_OVERRIDE, MFCS_REVIEW_RESTRICTIONS_MANAGERS, $message);

    // perform check against this request to see if new problems are detected or existing problems are resolved.
    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
    mfcs_mischellaneous_check_requests_for_problems($request_id);
  }
  else {
    form_set_error('form', "An error occurred while trying to override the request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to override the request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);

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
