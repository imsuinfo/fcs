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
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_proxy_0_form($form, &$form_state, $page_number = 0) {
  mfcs_add_canonical_headers('requests/proxy-0/' . $page_number);

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if (!cf_is_integer($page_number)) {
    $page_number = 0;
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

  $class_id = '';
  $class_proxy_account_id = 'no-script-hidden script-hidden ';
  $class_proxy_account = '';
  $class_proxy_first_name = '';
  $class_proxy_last_name = '';
  $class_account_id = 'no-script-hidden script-hidden ';
  $class_account = '';
  $class_first_name = '';
  $class_last_name = '';
  $class_operations = '';

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'ASC';
    $order_column = 0;
    $class_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 0;
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'proxy account id') {
      $sorting['proxy_account_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 1;
      $class_proxy_account_id .= 'sorting_1 ';
    }
    elseif ($order == 'proxy account') {
      $sorting['proxy_account'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 2;
      $class_proxy_account .= 'sorting_1 ';
    }
    elseif ($order == 'proxy first name') {
      $sorting['proxy_first_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 3;
      $class_proxy_first_name .= 'sorting_1 ';
    }
    elseif ($order == 'proxy last name') {
      $sorting['proxy_last_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 4;
      $class_proxy_last_name .= 'sorting_1 ';
    }
    elseif ($order == 'account id') {
      $sorting['account_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 5;
      $class_account_id .= 'sorting_1 ';
    }
    elseif ($order == 'account') {
      $sorting['account'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 6;
      $class_account .= 'sorting_1 ';
    }
    elseif ($order == 'first name') {
      $sorting['first_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 7;
      $class_first_name .= 'sorting_1 ';
    }
    elseif ($order == 'last name') {
      $sorting['last_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 8;
      $class_last_name .= 'sorting_1 ';
    }
  }

  $rows_per_page = 10;
  $options = array(
    'sort' => $sorting,
    'join' => TRUE,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
  );

  $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.disabled' => 0), $options);
  $proxies = array();
  if (isset($proxies_returned['results']) && is_array($proxies_returned['results'])) {
    $proxies = $proxies_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($options['start'] >= $proxies_returned['count']) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $rows = array();
  foreach ($proxies as $proxy) {
    $row = array();

    $row['id'] = array(
      'data' => $proxy->id,
      'class' => $class_id,
    );

    $row['proxy_account_id'] = array(
      'data' => $proxy->proxy_id,
      'class' => $class_proxy_account_id,
    );

    $row['proxy_account'] = array(
      'data' => '<a href="' . $base_path . 'user/' . $proxy->proxy_id . $url_arguments . '" class="user-link">' . check_plain($proxy->proxy_name) . '</a>' . ' <span class="user-id script-hidden">(id: ' .  $proxy->proxy_id . ')</span>',
      'class' => $class_proxy_account,
    );

    $row['proxy_first_name'] = array(
      'data' => check_plain($proxy->proxy_first_name),
      'class' => $class_proxy_first_name,
    );

    $row['proxy_last_name'] = array(
      'data' => check_plain($proxy->proxy_last_name),
      'class' => $class_proxy_last_name,
    );

    $row['account_id'] = array(
      'data' => $proxy->user_id,
      'class' => $class_account_id,
    );

    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'user/' . $proxy->user_id . $url_arguments . '" class="user-link">' . check_plain($proxy->name) . '</a>' . ' <span class="user-id script-hidden">(id: ' .  $proxy->user_id . ')</span>',
      'class' => $class_account,
    );

    $row['first_name'] = array(
      'data' => check_plain($proxy->first_name),
      'class' => $class_first_name,
    );

    $row['last_name'] = array(
      'data' => check_plain($proxy->last_name),
      'class' => $class_last_name,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/proxy-0/delete/' . $proxy->id . $url_arguments . '" class="operation-link operation-link-delete">Delete</a>',
      'class' => $class_operations,
    );

    $rows[] = $row;
  }

  $proxy_list = array();
  $proxy_list['caption'] = t("Venue Coordinator Proxies");
  $proxy_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'proxy_account_id' => array('field' => 'proxy_account_id', 'data' => "Proxy Account ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'proxy_account' => array('field' => 'proxy_account', 'data' => "Proxy Account", 'align' => 'center'),
    'proxy_first_name' => array('field' => 'proxy_first_name', 'data' => "Proxy First Name", 'align' => 'center'),
    'proxy_last_name' => array('field' => 'proxy_last_name', 'data' => "Proxy Last Name", 'align' => 'center'),
    'account_id' => array('field' => 'account_id', 'data' => "Account ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'account' => array('field' => 'account', 'data' => "Account", 'align' => 'center'),
    'first_name' => array('field' => 'first_name', 'data' => "First Name", 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => "Last Name", 'align' => 'center'),
    t("Operations"),
  );

  $proxy_list['attributes'] = array();
  $proxy_list['attributes']['id'] = 'mfcs-proxy_form';
  $proxy_list['attributes']['class'] = array(
    'no_default',
    'mfcs-proxy_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $proxy_list['attributes']['summary'] = 'Collection of users who are allowed to proxy as another user.';

  $proxy_list['rows'] = $rows;
  $proxy_list['empty'] = t("There are no users assigned to proxy as a venue coordinator.");

  $datatable_settings = array(
    'sort' => strtolower($sort),
    'order' => $order_column,
    #'stateSave' => TRUE,
    'lengthMenu' => array(
       5 => '5',
      10 => '10',
      20 => '20',
      40 => '40',
      80 => '80',
      -1 => '"All"',
    ),
    'processing' => TRUE,
    'serverSide' => TRUE,
    'ajax' => "'" . $base_path . 'requests/proxy-0/ajax' . "'",
    'deferRender' => TRUE,
    'columns' => array(
      array('name' => 'id', 'className' => 'column-id'),
      array('name' => 'proxy_account_id', 'className' => 'column-proxy_account_id', 'visible' => FALSE),
      array('name' => 'proxy_account', 'className' => 'column-proxy_account'),
      array('name' => 'proxy_first_name', 'className' => 'column-proxy_first_name'),
      array('name' => 'proxy_last_name', 'className' => 'column-proxy_last_name'),
      array('name' => 'account_id', 'className' => 'column-account_id', 'visible' => FALSE),
      array('name' => 'account', 'className' => 'column-account'),
      array('name' => 'first_name', 'className' => 'column-first_name'),
      array('name' => 'last_name', 'className' => 'column-last_name'),
      array('name' => 'operations', 'className' => 'column-operations'),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => TRUE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
  );

  $form['proxy_list']['#markup'] = theme('table', $proxy_list);
  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/proxy-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/proxy-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/proxy-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/proxy-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($navigation_total > $proxies_returned['count']) {
    $navigation_total = $proxies_returned['count'];
  }

  $navigation_bottom .= '<div class="table_listing-comment">' . "\n";
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $proxies_returned['count'] . " entries.\n";
  $navigation_bottom .= '</div>' . "\n";

  if ($proxies_returned['count'] > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/proxy-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/proxy-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form['proxy_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['proxy_list']['#markup'] .= mcneese_libraries_theme_table($proxy_list, $datatable_settings);
  $form['proxy_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

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

    $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.proxy_id' => $proxy_id, 'mpvc.user_id' => $user_id, 'mpvc.disabled' => 0));
    if (!empty($proxies_returned['results'])) {
      form_set_error('proxy][proxy_id', "This proxy is already assigned to the specified user.");
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
  $proxies_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.id' => $form['form']['id']['#value'], 'mpvc.disabled' => 0));

  $proxy = NULL;
  if (!empty($proxies_returned['results'])) {
    $proxy = array_pop($proxies_returned['results']);
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
