<?php

/**
 * @file
 * Defines redirects page functions.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Provides the group leader settings form page.
 */
function mcneese_management_redirects_settings_page() {
  $dependency = mcneese_management_check_group_leader_dependencies();
  if (!is_null($dependency)) {
    return $dependency;
  }

  $output = '';
  $output .= '<div>' . t("The redirects provided on this page are special case redirects for controlling when to and when not to redirect based on the settings defined here.") . '</div>';
  $output .= '<br>';

  $query = db_select('mcneese_management_redirects', 'mmr');
  $query->fields('mmr', array('id', 'source', 'destination', 'status', 'logged_in'));
  $query->orderBy('source');
  $executed = $query->execute();
  $redirects = (array) $executed->fetchAll();

  $settings_path = 'admin/content/management/redirects/';

  $rows = array();
  foreach ($redirects as $redirect) {
    $row = array();
    $row[] = $redirect->id;
    $row[] = check_plain($redirect->source);
    $row[] = check_plain($redirect->destination);

    if ($redirect->status == 1) {
      $row[] = t("Enabled, Redirect");
    }
    elseif ($redirect->status == 2) {
      $row[] = t("Enabled, Maintenance");
    }
    elseif ($redirect->status == 3) {
      $row[] = t("Enabled, Alert");
    }
    else {
      $row[] = t("Disabled");
    }

    if ($redirect->logged_in == 0) {
      // don't redirect, don't display failsafe.
      $row[] = t("No Failsafe");
    }
    elseif ($redirect->logged_in == 1) {
      $row[] = t("Failsafe");
    }
    elseif ($redirect->logged_in == 2) {
      $row[] = t("Normal");
    }

    $operations = '';
    $operations .= l("Edit", $settings_path . 'update/' . $redirect->id);
    $operations .= ' ';
    $operations .= l("Delete", $settings_path . 'delete/' . $redirect->id);
    $row[] = $operations;
    $rows[] = $row;
  }

  $header = array(t("ID"), t("Source"), t("Destination"), t("Status"), t("Logged In"), t("Operations"));
  $attributes = array();
  $attributes['id'] = 'mcneese_management_redirects';
  $attributes['summary'] = t("A listing of redirects with the basic settings displaying the status of different settings and also a selection of operations to perform on a given redirect.");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t("Redirects"), 'attributes' => $attributes, 'empty' => t("There are no redirects available.")));

  $output .= '<br>';
  $output .= l("Add Redirect", $settings_path . 'add');

  return $output;
}

/**
 * Provides the redirects update form page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $id
 *   The numeric id representing a specific redirect to update.
 *   When this is NULL, then a new redirect is to be added.
 */
function mcneese_management_redirects_update_form($form, &$form_state, $id) {
  if (!is_null($id) && !cf_is_integer($id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('id');
    }

    drupal_not_found();
    drupal_exit();
  }

  // load existing data, if any.
  if (is_null($id)) {
    $default_source = '';
    $default_destination = '';
    $default_status = 0;
    $default_logged_in = 0;
    $default_maintenance_message = '';
    $default_alert_message = '';
    $default_failsafe_message = '';

    $submit_label = t("Create");

    drupal_set_title(t("Add New Redirect"), PASS_THROUGH);
  }
  else {
    $query = db_select('mcneese_management_redirects', 'mmr');
    $query->fields('mmr');
    $query->condition('mmr.id', $id);
    $executed = $query->execute();
    $redirect = $executed->fetchAssoc();

    if (empty($redirect)) {
      drupal_not_found();
      drupal_exit();
    }

    $default_source = $redirect['source'];
    $default_destination = $redirect['destination'];
    $default_status = $redirect['status'];
    $default_logged_in = $redirect['logged_in'];
    $default_maintenance_message = unserialize($redirect['maintenance_message']);
    $default_alert_message = unserialize($redirect['alert_message']);
    $default_failsafe_message = unserialize($redirect['failsafe_message']);

    $submit_label = t("Update");

    drupal_set_title(t("Update Redirect %source (%id)", array('%source' => $default_source, '%id' => $id)), PASS_THROUGH);
  }

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['settings']['source'] = array(
    '#type' => 'textfield',
    '#title' => t("Source"),
    '#description' => t("Provide the local path that will be redirected. Only local paths will be supported. Do not use the leading /."),
    '#default_value' => $default_source,
    '#required' => TRUE,
  );

  $form['settings']['destination'] = array(
    '#type' => 'textfield',
    '#title' => t("Destination"),
    '#description' => t("Provide the remote path that the source will be redirected to. Only remote paths will be supported."),
    '#default_value' => $default_destination,
    '#required' => TRUE,
  );

  $status_options = array();
  $status_options[0] = t("Disabled");
  $status_options[1] = t("Enabled");
  $status_options[2] = t("Display Maintenance Message");
  $status_options[3] = t("Display Alert Message");
  $form['settings']['status'] = array(
    '#type' => 'select',
    '#title' => t("Status"),
    '#description' => t("Defines whether or not the redirect is processed or what messages are displayed. Disabled performs no actions. Enabled performs redirect. Displaying messages will display a message instead of redirecting."),
    '#default_value' => $default_status,
    '#options' => $status_options,
    '#required' => TRUE,
  );

  $logged_in_options = array();
  $logged_in_options[0] = t("No Failsafe");
  $logged_in_options[1] = t("Failsafe");
  $logged_in_options[2] = t("Normal");
  $form['settings']['logged_in'] = array(
    '#type' => 'select',
    '#title' => t("Logged In"),
    '#description' => t("Defines behavior for handling redirect for logged in users. For 'No Failsafe', the original path is shown. For 'Failsafe', the failsafe message is displayed."),
    '#default_value' => $default_logged_in,
    '#options' => $logged_in_options,
    '#required' => TRUE,
  );

  $form['settings']['maintenance_message'] = array(
    '#type' => 'text_format',
    '#title' => t("Maintenance Message"),
    '#description' => t("This message is displayed instead of a page for some given path when status is 'Display Maintenance Messsage'. HTML is allowed and encouraged."),
    '#default_value' => $default_maintenance_message['value'],
    '#format' => $default_maintenance_message['format'],
    '#rows' => 15,
    '#resizable' => TRUE,
    '#wysiwyg' => TRUE,
  );

  $form['settings']['alert_message'] = array(
    '#type' => 'text_format',
    '#title' => t("Alert Message"),
    '#description' => t("This message is displayed instead of a page for some given path when status is 'Display Alert Messsage'. HTML is allowed and encouraged."),
    '#default_value' => $default_alert_message['value'],
    '#format' => $default_alert_message['format'],
    '#rows' => 15,
    '#resizable' => TRUE,
    '#wysiwyg' => TRUE,
  );

  $form['settings']['failsafe_message'] = array(
    '#type' => 'text_format',
    '#title' => t("Failsafe Message"),
    '#description' => t("This message is displayed when the page is unable to perform an HTTP redirect or when logged in users are supposed to get this message. HTML is allowed and encouraged."),
    '#default_value' => $default_failsafe_message['value'],
    '#format' => $default_failsafe_message['format'],
    '#rows' => 15,
    '#resizable' => TRUE,
    '#wysiwyg' => TRUE,
    '#required' => TRUE,
  );

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/redirects';
  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  $form['actions'] = array();
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $submit_label,
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  $destination,
    '#weight' => 100,
  );

  return $form;
}

/**
 * Validation for mcneese_management_redirects_update_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mcneese_management_redirects_update_form_validate($form, &$form_state) {
  global $base_url, $base_path;

  $local_url = parse_url($base_url . $base_path);
  $source_url = parse_url($form_state['values']['settings']['source']);
  $destination_url = parse_url($form_state['values']['settings']['destination']);

  $bad_source = FALSE;
  if (!empty($source_url['scheme']) || !empty($source_url['host'])) {
    $bad_source = TRUE;
  }
  else {
    if (preg_match('@^/@i', $source_url['path'])) {
      $bad_source = TRUE;
    }
  }

  $bad_destination = FALSE;
  if (empty($destination_url['scheme']) || empty($destination_url['host'])) {
    $bad_destination = TRUE;
  }
  elseif ($local_url['host'] == $destination_url['host']) {
    if (preg_match('@^' . $base_path . '@i', $destination_url['path'])) {
      $bad_destination = TRUE;
    }
  }

  if ($bad_source) {
    form_set_error('settings][source', t("The path '@source' is not a local path. The source may only be a local path. Make sure the leading / is not included.", array('@source' => $form_state['values']['settings']['source'])));
  }

  if ($bad_destination) {
    form_set_error('settings][destination', t("The path '@destination' is not a remote path. The destination may only be a remote path.", array('@destination' => $form_state['values']['settings']['destination'])));
  }
}

/**
 * Submit process for mcneese_management_redirects_update_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mcneese_management_redirects_update_form_submit($form, &$form_state) {
  $id = NULL;
  if (isset($form['id']['#value'])) {
    $id = $form['id']['#value'];
  }

  $fields = array();
  $fields['source'] = $form_state['values']['settings']['source'];
  $fields['destination'] = $form_state['values']['settings']['destination'];
  $fields['status'] = $form_state['values']['settings']['status'];
  $fields['logged_in'] = $form_state['values']['settings']['logged_in'];
  $fields['maintenance_message'] = serialize($form_state['values']['settings']['maintenance_message']);
  $fields['alert_message'] = serialize($form_state['values']['settings']['alert_message']);
  $fields['failsafe_message'] = serialize($form_state['values']['settings']['failsafe_message']);

  $transaction = db_transaction();

  if (empty($id)) {
    try {
      // now perform the operation.
      $query = db_insert('mcneese_management_redirects');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution
      unset($transaction);
    }
    catch (Exception $e) {
      $transaction->rollback();

      if (class_exists('cf_error')) {
        cf_error::on_query_execution($e);
      }
    }
  }
  else {
    try {
      // now perform the operation.
      $query = db_update('mcneese_management_redirects');
      $query->condition('id', $id);
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution
      unset($transaction);
    }
    catch (Exception $e) {
      $transaction->rollback();

      if (class_exists('cf_error')) {
        cf_error::on_query_execution($e);
      }
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/redirects';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the redirects delete form page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $id
 *   The numeric id representing a specific redirect to update.
 *   When this is NULL, then a new redirect is to be added.
 */
function mcneese_management_redirects_delete_form($form, &$form_state, $id) {
  if (!is_null($id) && !cf_is_integer($id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('id');
    }

    drupal_not_found();
    drupal_exit();
  }

  // load existing data, if any.
  $query = db_select('mcneese_management_redirects', 'mmr');
  $query->fields('mmr', array('id', 'source'));
  $query->condition('mmr.id', $id);
  $executed = $query->execute();
  $redirect = $executed->fetchAssoc();

  if (empty($redirect)) {
    drupal_not_found();
    drupal_exit();
  }

  drupal_set_title(t("Delete Redirect %source (%id)", array('%source' => $redirect['source'], '%id' => $id)), PASS_THROUGH);

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['settings']['confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t("Confirm Deletion"),
    '#description' => t("You must enable this checkbox to delete this redirection."),
    '#default_value' => FALSE,
    '#required' => TRUE,
  );

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/redirects';
  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  $form['actions'] = array();
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Delete"),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  $destination,
    '#weight' => 100,
  );

  return $form;
}

/**
 * Submit process for mcneese_management_redirects_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mcneese_management_redirects_delete_form_submit($form, &$form_state) {
  $transaction = db_transaction();

  try {
    // now perform the operation.
    $query = db_delete('mcneese_management_redirects');
    $query->condition('id', $form['id']['#value']);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/redirects';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}


/**
 * @} End of '@addtogroup mcneese_management'.
 */
