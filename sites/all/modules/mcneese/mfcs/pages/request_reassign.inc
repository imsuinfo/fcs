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
 * Provides the facilities use request reassign coordinator page.
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
function mfcs_request_manage_0_reassign_coordinator_form($form, &$form_state, $request_id) {
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

  mfcs_add_canonical_headers('requests/manage-0/reassign/coordinator/' . $request_id);

  $page_title = 'Re-Assign Request Coordinator';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Re-Assign Request Coordinator: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  global $conf;
  global $base_path;

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  if (empty($conf['fcs_failsafe_coordinator']) || !is_integer($conf['fcs_failsafe_coordinator'])) {
    watchdog(MFCS_WATCHDOG_ID, 'Failed to load \'fcs_failsafe_coordinator\' settings. Your settings.php does not contain the failsafe coordinator user id.', array(), WATCHDOG_CRITICAL);
    $new_coordinator_id = 1; // as a last resort fall back to the default drupal admin account.
  }
  else {
    $new_coordinator_id = $conf['fcs_failsafe_coordinator'];
  }

  if (isset($request['top']['room'][0]->value)) {
    $room_information = mfcs_load_rooms(NULL, NULL, $request['top']['room'][0]->value);
    if (isset($room_information->room_coordinator_id)) {
      $new_coordinator_id = $room_information->room_coordinator_id;
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
    $form['message']['#markup'] .= 'The new coordinator id would be the same as the old one. If you believe this to be incorrect, then either the Banner Database Cache is out of sync or the Venue Coordinator for this Room has not been updated in the Banner Database.' . '<br>';
    $form['message']['#markup'] .= '<br>';
    $form['message']['#markup'] .= 'This request is currently assigned to <span><strong><a href="' . $base_path . 'requests/users-0/view/' . $old_coordinator_id . '">' . check_plain($old_coordinator_name) . '</a></strong> <sup>[id: ' . $old_coordinator_id . ']</sup></span>.' . '<br>';
    $form['message']['#markup'] .= '<br>';
  }
  else {
    $form['message']['#markup'] .= '<h3>Are you sure you want to re-assign the request?</h3> ';
    $form['message']['#markup'] .= '<div>';
    $form['message']['#markup'] .= 'The currently assigned coordinator is out dated based on the currently cached Banner information. Re-assignment will update the Venue Coordinator to match the Venue Coordinator defined in the Banner cache. ' . '<br>';
    $form['message']['#markup'] .= '<br>';
    $form['message']['#markup'] .= 'This request would be re-assigned from <span><strong><a href="' . $base_path . 'requests/users-0/view/' . $old_coordinator_id . '">' . check_plain($old_coordinator_name) . '</a></strong> <sup>[id: ' . $old_coordinator_id . ']</sup></span> to <span><strong><a href="' . $base_path . 'requests/users-0/view/' . $new_coordinator_id . '">' . check_plain($new_coordinator_name) . '</a></strong> <sup>[id: ' . $new_coordinator_id . ']</sup></span>.<br>';
    $form['message']['#markup'] .= '<br>';

    $form['message']['#markup'] .= 'Are you certain you want to re-assign? ';
  }

  $form['message']['#markup'] .= '</div>';

  $form['display']['submit'] = array(
    '#id' => 'request-reassign-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  if ($old_coordinator_id == $new_coordinator_id) {
    $form['display']['submit']['no'] = array(
      '#id' => 'request-reassign-submit-no',
      '#name' => 'request-reassign-submit-no',
      '#type' => 'submit',
      '#default_value' => t('Back to Request'),
    );
  }
  else {
    $form['display']['submit']['no'] = array(
      '#id' => 'request-reassign-submit-no',
      '#name' => 'request-reassign-submit-no',
      '#type' => 'submit',
      '#default_value' => t('No'),
    );

    $form['display']['submit']['yes'] = array(
      '#id' => 'request-reassign-submit-yes',
      '#name' => 'request-reassign-submit-yes',
      '#type' => 'submit',
      '#default_value' => t('Yes'),
    );
  }

  if ($old_coordinator_id == $new_coordinator_id) {
    $form['display']['submit']['yes']['#disabled'] = TRUE;
    $form['display']['submit']['yes']['#attributes']['title'] = 'Cannot re-assign the Venue Coordinator!';
  }

  return $form;
}

/**
 * Validation for mfcs_request_manage_0_reassign_coordinator_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_reassign_coordinator_form()
 */
function mfcs_request_manage_0_reassign_coordinator_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-reassign-submit-yes' && $form['form']['old_coordinator_id']['#value'] == $form['form']['new_coordinator_id']['#value']) {
    form_set_error('', 'The new coordinator would be the same as the old one. Either the Banner Database Cache is out of sync or the Venue Coordinator for this Room has not been updated in the Banner Database.');
  }
}

/**
 * Submit process for mfcs_request_manage_0_reassign_coordinator_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_reassign_coordinator_form()
 */
function mfcs_request_manage_0_reassign_coordinator_form_submit($form, &$form_state) {
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
    $extra['revisions']['message'] = 'The Venue Coordinator has been changed from ' . $form['form']['old_coordinator_name']['#value'] . ' [id: ' . $form['form']['old_coordinator_id']['#value'] . '] to ' . $form['form']['new_coordinator_name']['#value'] . ' [id: ' . $form['form']['new_coordinator_id']['#value'] . '].';
  }

  $reassigned = mfcs_workflow_set_venue_coordinator($request_id, $form['form']['new_coordinator_id']['#value'], NULL, $extra);

  if ($reassigned) {
    $review_extra = array(
      'never_requester' => TRUE,
      'never_coordinator' => TRUE,
    );

    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_REASSIGN_COORDINATOR, MFCS_REVIEW_RESTRICTIONS_MANAGERS, $extra['revisions']['message']);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['reassigned'] = TRUE;
    $parameters['reassigned_from'] = $form['form']['old_coordinator_id']['#value'];
    $parameters['reassigned_to'] = $form['form']['new_coordinator_id']['#value'];
    $parameters['reassigned_type'] = 'coordinator';
    mfcs_send_workflow_emails($request_id, $parameters);

    // perform re-check for coordinator problems.
    mfcs_include(MFCS_INCLUDE_PROBLEMS);
    mfcs_request_problems_0_request_perform_recheck($request_id, MFCS_REQUEST_PROBLEM_COORDINATOR);
  }
  else {
    form_set_error('form', 'An error occurred while trying to re-assign the coordinator. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to re-assign the coordinator (@user_id) for the request: @request_id.', array('@request_id' => $request_id, '@user_id' => $form['form']['old_coordinator_id']['#value']), WATCHDOG_ERROR);

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
      $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request reassign requester page.
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
function mfcs_request_manage_0_reassign_requester_form($form, &$form_state, $request_id) {
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

  mfcs_add_canonical_headers('requests/manage-0/reassign/requester/' . $request_id);

  $page_title = 'Re-Assign Request Requester';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Re-Assign Request Requester: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  global $conf;
  global $base_path;

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  $old_requester_id = NULL;
  if (isset($request['mer']['user_id'][0]->value)) {
    $old_requester_id = (int) $request['mer']['user_id'][0]->value;
  }

  $old_requester_user = mfcs_load_basic_user_settings_by_user_id($old_requester_id);
  $old_requester_name = NULL;
  if (!empty($old_requester_user['name'])) {
    if (!empty($old_requester_user['first_name']) && !empty($old_requester_user['last_name'])) {
      $old_requester_name = $old_requester_user['first_name'] . ' ' . $old_requester_user['last_name'];
    }
    else {
      $old_requester_name = $old_requester_user['name'];
    }
  }

  $request_title = NULL;
  if (isset($request['information']['title'][0]->value)) {
    $request_title = $request['information']['title'][0]->value;
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


  $form['form']['contained'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['form']['old_requester_id'] = array(
    '#type' => 'value',
    '#value' => $old_requester_id,
  );

  $form['form']['old_requester_name'] = array(
    '#type' => 'value',
    '#value' => $old_requester_name,
  );

  $form['message'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );


  $form['message']['#markup'] .= '<div class="reassign-requester-description">';
  $form['message']['#markup'] .= 'Assigning a new requester will change who recieves e-mails and who is allowed to perform edit operations as a normal requester (not as a reviewer or manager).';
  $form['message']['#markup'] .= '<br>';
  $form['message']['#markup'] .= 'The current requester assign to request <strong>' . check_plain($request_title) . '</strong> <sup>[id: ' . $request_id . ']</sup> is <strong><a href="' . $base_path . 'requests/users-0/view/' . $old_requester_id . '">' . check_plain($old_requester_name) . '</a></strong> <sup>[id: ' . $old_requester_id . ']</sup>.<br>';
  $form['message']['#markup'] .= '<br>';
  $form['message']['#markup'] .= 'You can <a href="' . $base_path . 'requests/users-0/list">search through a comprehensive list of system users</a> to find the username or id and specify it below.';
  $form['message']['#markup'] .= '</div>';

  $form['requester'] = array(
    '#tree' => TRUE,
  );

  $form['requester']['user_id'] = array(
    '#id' => 'requester-user_id',
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => 'New Requester',
    '#size' => 96,
    '#autocomplete_path' => 'requests/manage-0/reassign/requester/autocomplete/user_id',
    '#required' => TRUE,
  );

  $form['display']['submit'] = array(
    '#id' => 'request-reassign-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['display']['submit']['cancel'] = array(
    '#id' => 'request-reassign-submit-cancel',
    '#name' => 'request-reassign-submit-cancel',
    '#type' => 'submit',
    '#default_value' => t('Cancel'),
  );

  $form['display']['submit']['submit'] = array(
    '#id' => 'request-reassign-submit-submit',
    '#name' => 'request-reassign-submit-submit',
    '#type' => 'submit',
    '#default_value' => t('Submit'),
  );

  return $form;
}

/**
 * Validation for mfcs_request_manage_0_reassign_requester_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_reassign_requester_form()
 */
function mfcs_request_manage_0_reassign_requester_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request-reassign-submit-cancel') {
    return;
  }

  $user_id = NULL;
  if (cf_is_integer($form_state['values']['requester']['user_id'])) {
    $user_id = (int) $form_state['values']['requester']['user_id'];
  }
  else {
    $matches = array();
    $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['requester']['user_id'], $matches);

    if ($matched && !empty($matches[1])) {
      $user_id = (int) $matches[1];
    }
  }

  if (!is_null($user_id)) {
    $user = user_load($user_id);

    if (is_object($user)) {
      if ($user->status == 0) {
        form_set_error('requester][user_id', 'No unlocked user found with a user id of ' . $user_id . '. Please select a valid user id.');
      }
      elseif (!in_array('requester', $user->roles)) {
        form_set_error('requester][user_id', 'Cannot assign user ' . $user->name . ' [id:' . $user->uid . '], the user does not have the appropraite requester role.');
      }
    }
    else {
      form_set_error('requester][user_id', 'No unlocked user found with a user id of ' . $user_id . '. Please select a valid user id.');
    }
  }
}

/**
 * Submit process for mfcs_request_manage_0_reassign_requester_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_manage_0_reassign_requester_form()
 */
function mfcs_request_manage_0_reassign_requester_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if ($clicked_id == 'request-reassign-submit-cancel') {
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

  $user_id = NULL;
  if (cf_is_integer($form_state['values']['requester']['user_id'])) {
    $user_id = (int) $form_state['values']['requester']['user_id'];
  }
  else {
    $matches = array();
    $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['requester']['user_id'], $matches);

    if ($matched && !empty($matches[1])) {
      $user_id = (int) $matches[1];
    }
  }

  $new_requester_user = mfcs_load_basic_user_settings_by_user_id($user_id);
  $new_requester_name = NULL;
  if (!empty($new_requester_user['name'])) {
    if (!empty($new_requester_user['first_name']) && !empty($new_requester_user['last_name'])) {
      $new_requester_name = $new_requester_user['first_name'] . ' ' . $new_requester_user['last_name'];
    }
    else {
      $new_requester_name = $new_requester_user['name'];
    }
  }

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  $extra = array();
  $extra['revisions']['message'] = 'The Requester has been changed from ' . $form['form']['old_requester_name']['#value'] . ' [id: ' . $form['form']['old_requester_id']['#value'] . '] to ' . $new_requester_name . ' [id: ' . $user_id . '].';

  $reassigned = mfcs_workflow_set_requester($request_id, $user_id, NULL, $extra);

  if ($reassigned) {
    $review_extra = array(
      'never_requester' => TRUE,
      'never_coordinator' => TRUE,
    );

    mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_REASSIGN_REQUESTER, MFCS_REVIEW_RESTRICTIONS_MANAGERS, $extra['revisions']['message'], $review_extra);

    $parameters = array();
    $parameters['changed_by'] = $user->uid;
    $parameters['changed_type'] = 'user';
    $parameters['reassigned'] = TRUE;
    $parameters['reassigned_from'] = $form['form']['old_requester_id']['#value'];
    $parameters['reassigned_to'] = $user_id;
    $parameters['reassigned_type'] = 'requester';
    mfcs_send_workflow_emails($request_id, $parameters);

    // perform re-check for requester problems.
    mfcs_include(MFCS_INCLUDE_PROBLEMS);
    mfcs_request_problems_0_request_perform_recheck($request_id, MFCS_REQUEST_PROBLEM_REQUESTER);
  }
  else {
    form_set_error('form', 'An error occurred while trying to re-assign the requester. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to re-assign the requester (@user_id) for the request: @request_id.', array('@request_id' => $request_id, '@user_id' => $user_id), WATCHDOG_ERROR);

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
