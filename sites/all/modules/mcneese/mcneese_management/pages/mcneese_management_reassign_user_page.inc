<?php

/**
 * @file
 * Defines user re-assignment page functions.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Provides the group leader settings form page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mcneese_management_reassign_user_form($form, &$form_state) {
  $dependency = mcneese_management_check_group_leader_dependencies();
  if (!is_null($dependency)) {
    drupal_not_found();
    drupal_exit();
  }

  $form = array();

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['settings']['from_user'] = array(
    '#type' => 'textfield',
    '#title' => t("From User"),
    '#description' => t("Select a user whose content will be assign to another user."),
    '#autocomplete_path' => 'admin/content/management/reassign_user/ajax/autocomplete/reassign_user',
    '#required' => TRUE,
  );

  $form['settings']['to_user'] = array(
    '#type' => 'textfield',
    '#title' => t("To User"),
    '#description' => t("Select a user who will be assigned content from another user."),
    '#autocomplete_path' => 'admin/content/management/reassign_user/ajax/autocomplete/reassign_user',
    '#required' => TRUE,
  );

  $cancel_destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management';
  $submit_destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/reassign_user';

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $submit_destination,
  );

  $form['actions'] = array();
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Re-Assign"),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  $cancel_destination,
    '#weight' => 100,
  );

  return $form;
}

/**
 * Validation for mcneese_management_reassign_user_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mcneese_management_reassign_user_form_validate($form, &$form_state) {
  $user_id_from = mcneese_management_validate_user($form_state['values']['settings']['from_user']);
  if ($user_id_from === FALSE) {
    form_set_error('settings][from_user', t("The user '%user' does not exist."), array('%user' => $form_state['values']['settings']['from_user']));
  }

  $user_id_to = mcneese_management_validate_user($form_state['values']['settings']['to_user']);
  if ($user_id_to === FALSE) {
    form_set_error('settings][to_user', t("The user '%user' does not exist."), array('%user' => $form_state['values']['settings']['to_user']));
  }

  if ($user_id_from == $user_id_to) {
    form_set_error('settings][to_user', t("The same user may not be used in both the from and to fields."));
  }
}

/**
 * Submit process for mcneese_management_reassign_user_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mcneese_management_reassign_user_form_submit($form, &$form_state) {
  $from_user = $form_state['values']['settings']['from_user'];
  $from_user_id = NULL;

  if (cf_is_integer($from_user)) {
    $from_user_id = $from_user;
  }
  else {
    $matches = array();
    $matched = preg_match('/ \[uid:\s*(\d+)]\s*$/i', $from_user, $matches);

    if ($matched) {
      $from_user_id = $matches[1];
    }
    else {
     return;
    }
  }

  $to_user = $form_state['values']['settings']['to_user'];
  $to_user_id = NULL;

  if (cf_is_integer($to_user)) {
    $to_user_id = $to_user;
  }
  else {
    $matches = array();
    $matched = preg_match('/ \[uid:\s*(\d+)]\s*$/i', $to_user, $matches);

    if ($matched) {
      $to_user_id = $matches[1];
    }
    else {
      return;
    }
  }

  // load all nids to re-assign.
  $nids = (array) mcneese_management_load_assigned_nids($from_user_id);

  if (empty($nids)) {
    drupal_set_message(t("The user %from_user has no associated nodes to re-assign.", array('%from_user' => $from_user)), 'warning');
    return TRUE;
  }


  $changed = mcneese_management_assign_node_user($nids, $to_user_id);

  if ($changed === FALSE) {
    watchdog('alert', "Failed to assign %to_user to all nodes associated with %from_user.", array('%to_user' => $to_user, '%from_user' => $from_user));
  }


  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/reasign_user';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mcneese_management'.
 */
