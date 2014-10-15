<?php

/**
 * @file
 * Defines McNeese facilities use request proxy page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request proxy page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_proxy_0_form($form, &$form_state) {
  mfcs_add_canonical_headers('requests/proxy-0');

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $form = array();

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  // load sorting information
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'ASC';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'account') {
      $sorting['account'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'first name') {
      $sorting['first_name'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'last name') {
      $sorting['last_name'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'proxy_account') {
      $sorting['proxy_account'] = is_null($sort) ? 'DESC' : $sort;
    }
  }

  $proxies = mfcs_get_proxy_venue_coordinators(array('mpvc.disabled' => 0), TRUE, $sorting);

  if (!is_array($proxies)) {
    $proxies = array();
  }

  $rows = array();
  foreach ($proxies as $proxy) {
    $row = array();

    $proxy_user = mfcs_get_user_information($proxy->proxy_id);

    if ($proxy_user === FALSE) continue;

    $row['id'] = $proxy->id;
    $row['proxy_account'] = '<a href="' . $base_path . 'user/' . $proxy->proxy_id . $url_arguments . '" class="user-link">' . check_plain($proxy_user->name) . '</a>' . ' <span class="user-id">(id: ' .  $proxy->proxy_id . ')</span>';
    $row['proxy_first_name'] = check_plain($proxy_user->first_name);
    $row['proxy_last_name'] = check_plain($proxy_user->last_name);
    $row['account'] = '<a href="' . $base_path . 'user/' . $proxy->user_id . $url_arguments . '" class="user-link">' . check_plain($proxy->name) . '</a>' . ' <span class="user-id">(id: ' .  $proxy->user_id . ')</span>';
    $row['first_name'] = check_plain($proxy->first_name);
    $row['last_name'] = check_plain($proxy->last_name);

    $row['operations'] = '<a href="' . $base_path . 'requests/proxy-0/delete/' . $proxy->id . $url_arguments . '" class="operation-link operation-link-delete">Delete</a>';

    $rows[] = $row;
  }

  $proxy_list = array();
  $proxy_list['caption'] = t("Venue Coordinator Proxies");
  $proxy_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'proxy_account' => array('field' => 'proxy_account', 'data' => "Proxy Account", 'align' => 'center'),
    'proxy_first_name' => array('field' => 'proxy_first_name', 'data' => "Proxy First Name", 'align' => 'center'),
    'proxy_last_name' => array('field' => 'proxy_last_name', 'data' => "Proxy Last Name", 'align' => 'center'),
    'account' => array('field' => 'account', 'data' => "Account", 'align' => 'center'),
    'first_name' => array('field' => 'first_name', 'data' => "First Name", 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => "Last Name", 'align' => 'center'),
    t("Operations"),
  );

  $proxy_list['attributes'] = array();
  $proxy_list['attributes']['id'] = 'mfcs-proxy_form';
  $proxy_list['attributes']['class'] = array ('mfcs-proxy_form');
  $proxy_list['attributes']['summary'] = 'Collection of users who are allowed to proxy as another user.';

  $proxy_list['rows'] = $rows;
  $proxy_list['empty'] = t("There are no users assigned to proxy as a venue coordinator.");

  $form['proxy_list']['#markup'] = theme('table', $proxy_list);


  $form['proxy'] = array(
    '#id' => 'proxy',
    '#type' => 'fieldset',
    '#title' => 'Add Reviewer',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['proxy']['user_id'] = array(
    '#id' => 'proxy-user_id',
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => 'User',
    '#description' => t("This is the user who is going to be proxied as."),
    '#size' => 96,
    '#autocomplete_path' => 'requests/proxy-0/autocomplete/user_id',
    '#required' => TRUE,
  );

  $form['proxy']['proxy_id'] = array(
    '#id' => 'proxy-proxy_id',
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => 'Proxy',
    '#description' => t("This is the user who doing the proxying."),
    '#size' => 96,
    '#autocomplete_path' => 'requests/proxy-0/autocomplete/user_id',
    '#required' => TRUE,
  );

  $form['proxy']['add'] = array(
    '#id' => 'submit-proxy-add',
    '#type' => 'submit',
    '#default_value' => t("Add Proxy"),
  );

  return $form;
}

/**
 * Validation for mfcs_request_proxy_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_proxy_0_form()
 */
function mfcs_request_proxy_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-proxy-add') {
    $proxy_id = NULL;
    if (cf_is_integer($form_state['values']['proxy']['proxy_id'])) {
      $proxy_id = $form_state['values']['proxy']['proxy_id'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['proxy']['proxy_id'], $matches);

      if ($matched && !empty($matches[1])) {
        $proxy_id = $matches[1];
      }
    }

    if (!is_null($proxy_id)) {
      $proxy = user_load($proxy_id);

      if (!is_object($proxy)) {
        form_set_error('proxy][proxy_id', "An invalid Proxy has been specified. Please select a valid Proxy.");
      }
    }

    $user_id = NULL;
    if (cf_is_integer($form_state['values']['proxy']['user_id'])) {
      $user_id = $form_state['values']['proxy']['user_id'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['proxy']['user_id'], $matches);

      if ($matched && !empty($matches[1])) {
        $user_id = $matches[1];
      }
    }

    if (!is_null($user_id)) {
      $user = user_load($user_id);

      if (!is_object($user)) {
        form_set_error('proxy][user_id', "An invalid User has been specified. Please select a valid User.");
      }
    }

    $existing = mfcs_get_proxy_venue_coordinators(array('mpvc.proxy_id' => $proxy_id, 'mpvc.user_id' => $user_id, 'mpvc.disabled' => 0));

    if (!empty($existing)) {
      form_set_error('proxy][proxy_id', "This proxy is already assigned to the specified user");
    }
  }
  else {
    form_set_error('', "Unknown submit process detected.");
  }
}

/**
 * Submit process for mfcs_request_proxy_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_proxy_0_form()
 */
function mfcs_request_proxy_0_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-proxy-add') {
    $values = array();

    if (cf_is_integer($form_state['values']['proxy']['proxy_id'])) {
      $values['proxy_id'] = $form_state['values']['proxy']['proxy_id'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['proxy']['proxy_id'], $matches);

      if ($matched && !empty($matches[1])) {
        $values['proxy_id'] = $matches[1];
      }
    }

    if (cf_is_integer($form_state['values']['proxy']['user_id'])) {
      $values['user_id'] = $form_state['values']['proxy']['user_id'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['proxy']['user_id'], $matches);

      if ($matched && !empty($matches[1])) {
        $values['user_id'] = $matches[1];
      }
    }

    $failure = FALSE;
    $transaction = db_transaction();
    try {
      $query = db_select('mfcs_proxy_venue_coordinator', 'mpvc');
      $query->addField('mpvc', 'id', 'id');
      $query->condition('mpvc.proxy_id', $values['proxy_id']);
      $query->condition('mpvc.user_id', $values['user_id']);
      $query->condition('mpvc.disabled', 1);

      $result = $query->execute()->fetchField();

      if ($result > 0) {
        $query = db_update('mfcs_proxy_venue_coordinator');
        $query->fields(array('disabled' => 0));
        $query->condition('id', $result);
        $query->execute();
      }
      else {
        $query = db_insert('mfcs_proxy_venue_coordinator');
        $query->fields($values);
        $query->execute();
      }
    }
    catch (Exception $ex) {
      $transaction->rollback();
      cf_error::on_query_execution($ex);

      $failure = TRUE;
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/proxy-0');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request proxy delete page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $id
 *   Number representing the proxy id.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_proxy_0_delete_form($form, &$form_state, $id) {
  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $proxys = mfcs_get_proxy_venue_coordinators(array('mpvc.id' => $id));
  $proxy = NULL;
  if (!empty($proxys)) {
    $proxy = array_pop($proxys);
  }

  if (!is_object($proxy)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/proxy-0/delete/' . $id);

  $proxy_user = user_load($proxy->proxy_id);

  $proxy_name = '';
  if (is_object($proxy_user)) {
    $proxy_name = $proxy_user->name;
  }

  $user = user_load($proxy->user_id);

  $user_name = '';
  if (is_object($user)) {
    $user_name = $user->name;
  }

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  if (empty($form_state['values']['redirect_to'])) {
    $destination = 'requests/proxy-0';
  }
  else {
    $destination = $form_state['values']['redirect_to'];
  }

  return confirm_form($form,
    t("Are you sure you want to delete the proxy %proxy_name (%proxy_id) from being a venue coordinator proxy for the user %user_name (%user_id)?", array('%proxy_name' => $proxy_name, '%proxy_id' => $proxy_user->uid, '%user_name' => $user_name, '%user_id' => $user->uid)),
    $destination,
    t("This action cannot be undone."),
    t("Delete"),
    t("Cancel")
  );
}

/**
 * Validation for mfcs_request_proxy_0_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_proxy_0_delete_form()
 */
function mfcs_request_proxy_0_delete_form_validate($form, &$form_state) {
  $existing = mfcs_get_proxy_venue_coordinators(array('mpvc.id' => $form['form']['id']['#value'], 'mpvc.disabled' => 0));
  $proxy = NULL;
  if (!empty($existing)) {
    $proxy = array_pop($existing);
  }

  if (!is_object($proxy)) {
    form_set_error('', "An invalid proxy id has been specified.");
  }
}

/**
 * Submit process for mfcs_request_proxy_0_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_proxy_0_delete_form()
 */
function mfcs_request_proxy_0_delete_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_proxy_venue_coordinator');
    $query->fields(array('disabled' => 1));
    $query->condition('id', $form['form']['id']['#value']);
    $query->execute();
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/proxy-0');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
