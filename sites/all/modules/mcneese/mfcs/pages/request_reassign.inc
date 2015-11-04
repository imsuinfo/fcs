<?php

/**
 * @file
 * Defines McNeese facilities use request reassign page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request reassign page.
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
function mfcs_request_reassign_0_form($form, &$form_state, $request_id) {
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

  mfcs_add_canonical_headers('requests/reassign-0/' . $request_id);

  $page_title = "Re-Assign Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Re-Assign Request: " . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);


  if (!mfcs_disabled_users_with_active_content($request['venue_coordinator']['user_id'][0]->value)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $conf;

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  if (empty($conf['fcs_failsafe_coordinator']) || !is_integer($conf['fcs_failsafe_coordinator'])) {
    watchdog(MFCS_WATCHDOG_ID, "Failed to load 'fcs_failsafe_coordinator' settings. Your settings.php does not contain the failsafe coordinator user id.", array(), WATCHDOG_CRITICAL);
    $new_coordinator_id = 1; // as a last resort fall back to the default drupal admin account.
  }
  else {
    $new_coordinator_id = $conf['fcs_failsafe_coordinator'];
  }

  if (isset($request['top']['room'][0]->value)) {
    $room_information = mfcs_load_rooms(NULL, NULL, $request['top']['room'][0]->value);
    if (isset($room_information->coordinator_id)) {
      $new_coordinator_id = $room_information->coordinator_id;
    }
  }

  $old_coordinator_id = NULL;
  if (isset($request['venue_coordinator']['user_id'][0]->value)) {
    $old_coordinator_id = $request['venue_coordinator']['user_id'][0]->value;
  }

  $old_coordinator_user = mfcs_load_basic_user_settings_by_user_id($old_coordinator_id);
  $old_coordinator_name = NULL;
  if (!empty($old_coordinator_user['name'])) {
    if (!empty($old_coordinator_user['first_name']) && !empty($old_coordinator_user['last_name'])) {
      $old_coordinator_name = $old_coordinator_user['first_name'] . ' ' . $old_coordinator_user['last_name'];
    }
    else {
      $old_coordinator_name = $old_coordinator_user['name'];
    }
  }

  $new_coordinator_user = mfcs_load_basic_user_settings_by_user_id($new_coordinator_id);
  $new_coordinator_name = NULL;
  if (!empty($new_coordinator_user['name'])) {
    if (!empty($new_coordinator_user['first_name']) && !empty($new_coordinator_user['last_name'])) {
      $new_coordinator_name = $new_coordinator_user['first_name'] . ' ' . $new_coordinator_user['last_name'];
    }
    else {
      $new_coordinator_name = $new_coordinator_user['name'];
    }
  }

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

  $form['form']['old_coordinator_id'] = array(
    '#type' => 'value',
    '#value' => $old_coordinator_id,
  );

  $form['form']['old_coordinator_name'] = array(
    '#type' => 'value',
    '#value' => $old_coordinator_name,
  );

  $form['form']['new_coordinator_id'] = array(
    '#type' => 'value',
    '#value' => $new_coordinator_id,
  );

  $form['form']['new_coordinator_name'] = array(
    '#type' => 'value',
    '#value' => $new_coordinator_name,
  );

  $form['message'] = array(
    '#markup' => '',
  );

  if ($old_coordinator_id == $new_coordinator_id) {
    $form['message']['#markup'] .= '<h3>Unable to Re-Assign Venue Coordinator!</h3> ';
    $form['message']['#markup'] .= '<div>';
    $form['message']['#markup'] .= 'The new coordinator id would be the same as the old one. Either the Banner Database Cache is out of sync or the Venue Coordinator for this Room has not been updated in the Banner Database.' . "<br>\n";
    $form['message']['#markup'] .= "<br>\n";
    $form['message']['#markup'] .= "This request is currently assigned to <span title=\"User ID: " . $old_coordinator_id . "\"><strong>" . $old_coordinator_name . "</strong> <sup>[id: " . $old_coordinator_id . "]</sup></span>." . "<br>\n";
    $form['message']['#markup'] .= "<br>\n";
  }
  else {
    $form['message']['#markup'] .= '<h3>Are you sure you want to re-assign the request?</h3> ';
    $form['message']['#markup'] .= '<div>';
    $form['message']['#markup'] .= 'If this request has an out dated Venue Coordinator, then it will be re-assigned to the correct, updated, Venue Coordinator. ' . "<br>\n";
    $form['message']['#markup'] .= "<br>\n";
    $form['message']['#markup'] .= "This request would be re-assigned from <span title=\"User ID: " . $old_coordinator_id . "\"><strong>" . $old_coordinator_name . "</strong> <sup>[id: " . $old_coordinator_id . "]</sup></span> to <span title=\"User ID: " . $new_coordinator_id . "\"><strong>" . $new_coordinator_name . "</strong> <sup>[id: " . $new_coordinator_id . "]</sup></span>.<br>\n";
    $form['message']['#markup'] .= "<br>\n";
  }

  $form['message']['#markup'] .= 'Are you certain you want to re-assign? ';
  $form['message']['#markup'] .= '</div>';

  $form['display']['submit'] = array(
    '#id' => 'request-reassign-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'request-reassign-submit-no',
    '#name' => 'request-reassign-submit-no',
    '#type' => 'submit',
    '#default_value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'request-reassign-submit-yes',
    '#name' => 'request-reassign-submit-yes',
    '#type' => 'submit',
    '#default_value' => t("Yes"),
  );

  if ($old_coordinator_id == $new_coordinator_id) {
    $form['display']['submit']['yes']['#disabled'] = TRUE;
    $form['display']['submit']['yes']['#attributes']['title'] = "Cannot re-assign the Venue Coordinator!";
  }

  return $form;
}

/**
 * Validation for mfcs_request_reassign_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reassign_0_form()
 */
function mfcs_request_reassign_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-reassign-submit-yes' && $form['form']['old_coordinator_id']['#value'] == $form['form']['new_coordinator_id']['#value']) {
    form_set_error('', 'The new coordinator would be the same as the old one. Either the Banner Database Cache is out of sync or the Venue Coordinator for this Room has not been updated in the Banner Database.');
  }
}

/**
 * Submit process for mfcs_request_reassign_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reassign_0_form()
 */
function mfcs_request_reassign_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-reassign-submit-no' || $form['form']['old_coordinator_id']['#value'] == $form['form']['new_coordinator_id']['#value']) {
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
  if (!empty($form['form']['old_coordinator_name']['#value']) && !empty($form['form']['new_coordinator_name']['#value'])) {
    $extra['revisions']['message'] = "The Venue Coordinator has been changed from " . $form['form']['old_coordinator_name']['#value'] . " [id: " . $form['form']['old_coordinator_id']['#value'] . "] to " . $form['form']['new_coordinator_name']['#value'] . " [id: " . $form['form']['new_coordinator_id']['#value'] . "].";
  }

  $reassigned = mfcs_workflow_set_venue_coordinator($request_id, $form['form']['new_coordinator_id']['#value'], NULL, $extra);

  if ($reassigned) {
    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_REASSIGNED, $extra['revisions']['message']);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['reassigned'] = TRUE;
    $parameters['reassigned_from'] = $form['form']['old_coordinator_id']['#value'];
    $parameters['reassigned_to'] = $form['form']['new_coordinator_id']['#value'];
    mfcs_send_workflow_emails($request_id, $parameters);
  }
  else {
    form_set_error('form', "An error occurred while trying to re-assign the request. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to re-assign the request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);

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
