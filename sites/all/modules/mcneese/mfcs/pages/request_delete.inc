<?php

/**
 * @file
 * Defines McNeese facilities use request delete page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request delete page.
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
function mfcs_request_delete_0_form($form, &$form_state, $request_id) {
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

  mfcs_add_canonical_headers('requests/delete-0/' . $request_id);

  $page_title = "Delete Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Delete Request: " . $request['information']['title'][0]->value;
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

  $form['confirmation'] = array(
    '#markup' => '',
  );

  $form['confirmation']['#markup'] .= '<h3>Are you sure wou want to delete the request?</h3> ';
  $form['confirmation']['#markup'] .= '<div>';
  $form['confirmation']['#markup'] .= 'Deleting the form will remove your request from the system. ';
  $form['confirmation']['#markup'] .= '</div>';

  $form['display']['submit'] = array(
    '#id' => 'request-delete-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'request-delete-submit-no',
    '#name' => 'request-delete-submit-no',
    '#type' => 'submit',
    '#default_value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'request-delete-submit-yes',
    '#name' => 'request-delete-submit-yes',
    '#type' => 'submit',
    '#default_value' => t("Yes"),
  );

  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => "Justification",
    '#default_value' => '',
    '#required' => FALSE,
    '#description' => "Provide a reason explaining why this request is to be deleted. Add your name and e-mail address to this.",
  );

  $form['signature'] = array(
    '#type' => 'textarea',
    '#title' => "Signature",
    '#default_value' => '',
    '#required' => FALSE,
    '#description' => "The above message must be signed by a PGP key. This is a digital signature that ensures the person who deleted this is who they claim they are according to the PGP key.",
  );

  return $form;
}

/**
 * Validation for mfcs_request_delete_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_delete_0_form()
 */
function mfcs_request_delete_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-delete-submit-yes') {
    if (empty($form_state['values']['message'])) {
      form_set_error('message', $form['message']['#title'] . " field is required.");
    }

    if (empty($form_state['values']['signature'])) {
      form_set_error('signature', $form['signature']['#title'] . " field is required.");
    }
  }
}

/**
 * Submit process for mfcs_request_delete_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_delete_0_form()
 */
function mfcs_request_delete_0_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-delete-submit-no') {
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

  $extra = array(
    'revisions' => array(
      'message' => $form_state['values']['message'],
      'signature' => $form_state['values']['signature'],
    ),
  );

  $deleted = mfcs_workflow_set_status($request_id, MFCS_REQUEST_STATUS_DELETED, NULL, $extra);

  if ($deleted) {
    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['deleted'] = TRUE;
    $parameters['deleted-message'] = $form_state['values']['message'];
    $parameters['deleted-signature'] = $form_state['values']['signature'];
    mfcs_send_workflow_emails($request_id, $parameters);
  }
  else {
    form_set_error('form', "An error occurred while trying to delete the request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to delete the request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);

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
 * @} End of '@addtogroup mfcs'.
 */
