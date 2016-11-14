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
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_proxy_0_form($form, &$form_state, $page_number = NULL) {
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

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/proxy-0/' . $page_number);

  $page_title = 'Facilities Use Proxies';
  drupal_set_title($page_title);

  $user = cf_current_user();

  $form = array();

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

  $class_id = 'column-id ';
  $class_proxy_account_id = 'column-proxy_account_id no-script-hidden ';
  $class_proxy_account = 'column-proxy_account ';
  $class_proxy_first_name = 'column-proxy_first_name ';
  $class_proxy_last_name = 'column-proxy_last_name ';
  $class_account_id = 'column-account_id no-script-hidden ';
  $class_account = 'column-account ';
  $class_first_name = 'column-first_name ';
  $class_last_name = 'column-last_name ';
  $class_operations = 'column-operations no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'ASC';
    $order_column = array(0 => strtolower($sorting['id']));
    $class_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'proxy account id') {
      $sorting['proxy_account_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_proxy_account_id .= 'sorting_1 ';
    }
    elseif ($order == 'proxy account') {
      $sorting['proxy_account'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_proxy_account .= 'sorting_1 ';
    }
    elseif ($order == 'proxy first name') {
      $sorting['proxy_first_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_proxy_first_name .= 'sorting_1 ';
    }
    elseif ($order == 'proxy last name') {
      $sorting['proxy_last_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_proxy_last_name .= 'sorting_1 ';
    }
    elseif ($order == 'account id') {
      $sorting['account_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_account_id .= 'sorting_1 ';
    }
    elseif ($order == 'account') {
      $sorting['account'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(6 => strtolower($sorting[$order]));
      $class_account .= 'sorting_1 ';
    }
    elseif ($order == 'first name') {
      $sorting['first_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(7 => strtolower($sorting[$order]));
      $class_first_name .= 'sorting_1 ';
    }
    elseif ($order == 'last name') {
      $sorting['last_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(8 => strtolower($sorting[$order]));
      $class_last_name .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
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

  $count_options = $options;
  $count_options['start'] = 0;
  $count_options['results'] = FALSE;
  unset($count_options['limit']);

  $items_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.disabled' => 0), $count_options);

  $items_total = 0;
  if (is_array($items_returned) && array_key_exists('count', $items_returned)) {
    $items_total = $items_returned['count'];
  }

  $items_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.disabled' => 0), $options);

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($items_total > 0 && $options['start'] >= $items_total || $items_total == 0 && $options['start'] > $items_total) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $rows = array();
  foreach ($items as $item) {
    $row = array();
    $row['id'] = array(
      'data' => $item->id,
      'class' => $class_id,
    );

    $row['proxy_account_id'] = array(
      'data' => $item->proxy_id,
      'class' => $class_proxy_account_id,
    );

    // note: previously the id was added as a no-js failsafe, but it causes an annoying flicker on load (because the load speed is sufficiently fast enough to flicker but slow enough to see).
    $row['proxy_account'] = array(
      'data' => '<a href="' . $base_path . 'requests/users-0/view/' . $item->proxy_id . $url_arguments . '" class="user-link">' . check_plain($item->proxy_name) . '</a>',
      'class' => $class_proxy_account,
    );

    $row['proxy_first_name'] = array(
      'data' => check_plain($item->proxy_first_name),
      'class' => $class_proxy_first_name,
    );

    $row['proxy_last_name'] = array(
      'data' => check_plain($item->proxy_last_name),
      'class' => $class_proxy_last_name,
    );

    $row['account_id'] = array(
      'data' => $item->user_id,
      'class' => $class_account_id,
    );

    // note: previously the id was added as a no-js failsafe, but it causes an annoying flicker on load (because the load speed is sufficiently fast enough to flicker but slow enough to see).
    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'requests/users-0/view/' . $item->user_id . $url_arguments . '" class="user-link">' . check_plain($item->name) . '</a>',
      'class' => $class_account,
    );

    $row['first_name'] = array(
      'data' => check_plain($item->first_name),
      'class' => $class_first_name,
    );

    $row['last_name'] = array(
      'data' => check_plain($item->last_name),
      'class' => $class_last_name,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/proxy-0/delete/' . $item->id . $url_arguments . '" class="operation-link operation-link-delete">Delete</a>',
      'class' => $class_operations,
    );

    $rows[] = $row;
  }

  $proxy_list = array();
  $proxy_list['caption'] = 'Venue Coordinator Proxies';
  $proxy_list['header'] = array(
    'id' => array('field' => 'id', 'data' => 'ID', 'align' => 'center'),
    'proxy_account_id' => array('field' => 'proxy_account_id', 'data' => 'Proxy Account ID', 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'proxy_account' => array('field' => 'proxy_account', 'data' => 'Proxy Account', 'align' => 'center'),
    'proxy_first_name' => array('field' => 'proxy_first_name', 'data' => 'Proxy First Name', 'align' => 'center'),
    'proxy_last_name' => array('field' => 'proxy_last_name', 'data' => 'Proxy Last Name', 'align' => 'center'),
    'account_id' => array('field' => 'account_id', 'data' => 'Account ID', 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'account' => array('field' => 'account', 'data' => 'Account', 'align' => 'center'),
    'first_name' => array('field' => 'first_name', 'data' => 'First Name', 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => 'Last Name', 'align' => 'center'),
    'operations' => array('data' => 'Operations', 'align' => 'center', 'class' => array('no-print')),
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
  $proxy_list['empty'] = 'There are no users assigned to proxy as a venue coordinator.';

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
    'ajax' => array(
      'url' => '\'' . $base_path . 'requests/proxy-0/ajax' . $url_arguments . '\'',
      'type' => '\'POST\'',
    ),
    'deferRender' => TRUE,
    'deferLoading' => (int) $items_total,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => 'ID', 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'proxy_account_id', 'name' => 'Proxy Account ID', 'className' => 'column-proxy_account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 2, 'column' => 'proxy_account', 'name' => 'Proxy Account', 'className' => 'column-proxy_account'),
      array('id' => 3, 'column' => 'proxy_first_name', 'name' => 'Proxy First Name', 'className' => 'column-proxy_first_name'),
      array('id' => 4, 'column' => 'proxy_last_name', 'name' => 'Proxy Last Name', 'className' => 'column-proxy_last_name'),
      array('id' => 5, 'column' => 'account_id', 'name' => 'Account ID', 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 6, 'column' => 'account', 'name' => 'Account', 'className' => 'column-account'),
      array('id' => 7, 'column' => 'first_name', 'name' => 'First Name', 'className' => 'column-first_name'),
      array('id' => 8, 'column' => 'last_name', 'name' => 'Last Name', 'className' => 'column-last_name'),
      array('id' => 9, 'column' => NULL, 'name' => 'Operations', 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $proxy_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $proxy_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

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

  if ($items_total == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $items_total) {
    $navigation_total = $items_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= 'Showing ' . $navigation_start . ' to ' . $navigation_total . ' of ' . $items_total . ' entries.';
  $navigation_bottom .= '</div>';

  if ($items_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/proxy-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/proxy-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form['proxy_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['proxy_list']['#markup'] .= mcneese_libraries_theme_table($proxy_list, $datatable_settings);
  #$form['proxy_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  $form['proxy'] = array(
    '#id' => 'proxy',
    '#type' => 'fieldset',
    '#title' => 'Add Proxy',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['proxy']['user_id'] = array(
    '#id' => 'proxy-user_id',
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => 'User',
    '#description' => 'This is the user who is going to be proxied as.',
    '#size' => 72,
    '#autocomplete_path' => 'requests/proxy-0/autocomplete/user_id',
    '#required' => TRUE,
  );

  $form['proxy']['proxy_id'] = array(
    '#id' => 'proxy-proxy_id',
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => 'Proxy',
    '#description' => 'This is the user who doing the proxying.',
    '#size' => 72,
    '#autocomplete_path' => 'requests/proxy-0/autocomplete/user_id',
    '#required' => TRUE,
  );

  $form['proxy']['add'] = array(
    '#id' => 'submit-proxy-add',
    '#type' => 'submit',
    '#default_value' => 'Add Proxy',
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

    if (is_null($proxy_id)) {
      form_set_error('proxy][proxy_id', 'An invalid Proxy has been specified. Please select a valid user (this requires the user id).');
    }
    else {
      $proxy = user_load($proxy_id);

      if (!is_object($proxy)) {
        form_set_error('proxy][proxy_id', 'An invalid Proxy has been specified. Please select a valid user (this requires the user id).');
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

    if (is_null($user_id)) {
      form_set_error('proxy][user_id', 'An invalid User has been specified. Please select a valid user (this requires the user id).');
    }
    else {
      $user = user_load($user_id);

      if (!is_object($user)) {
        form_set_error('proxy][user_id', 'An invalid User has been specified. Please select a valid user (this requires the user id).');
      }
    }

    $items_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.proxy_id' => $proxy_id, 'mpvc.user_id' => $user_id, 'mpvc.disabled' => 0));
    if (!empty($items_returned['results'])) {
      form_set_error('proxy][proxy_id', 'This proxy is already assigned to the specified user.');
    }
  }
  else {
    form_set_error('', 'Unknown submit process detected.');
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
  $user = cf_current_user();

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-proxy-add') {
    $values = array(
      'user_id' => NULL,
      'proxy_id' => NULL,
    );

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
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      form_set_error('form', 'An error occurred while trying to add the proxy. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to create the proxy \'%proxy_id\' to proxy as \'%user_id\'.', array('%proxy_id' => $values['proxy_id'], '%user_id' => $values['user_id']), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      form_set_error('form', 'An error occurred while trying to add the proxy. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to create the proxy \'%proxy_id\' to proxy as \'%user_id\'.', array('%proxy_id' => $values['proxy_id'], '%user_id' => $values['user_id']), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
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

  if (!cf_is_integer($id) || $id < 0) {
    cf_error::invalid_integer('id');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $items_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.id' => $id));
  $proxy = NULL;
  if (!empty($items_returned['results'])) {
    $proxy = array_pop($items_returned['results']);
  }

  if (!is_object($proxy)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
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

  $page_title = 'Facilities Use Proxies Delete: ' . $proxy_name . ' (id: ' . $proxy->proxy_id . ') proxied as ' . $user_name . ' (id: ' . $proxy->user_id . ')';
  drupal_set_title($page_title);

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  if (empty($form_state['values']['redirect_to'])) {
    $destination = 'requests/proxy-0' . $url_arguments;
  }
  else {
    $destination = $form_state['values']['redirect_to'] . $url_arguments;
  }

  return confirm_form($form,
    'Are you sure you want to delete the proxy %proxy_name (%proxy_id) from being a venue coordinator proxy for the user %user_name (%user_id)?', array('%proxy_name' => $proxy_name, '%proxy_id' => $proxy_user->uid, '%user_name' => $user_name, '%user_id' => $user->uid),
    $destination,
    'This action cannot be undone.',
    'Delete',
    'Cancel'
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
  $items_returned = mfcs_get_proxy_venue_coordinators(array('mpvc.id' => $form['form']['id']['#value'], 'mpvc.disabled' => 0));

  $proxy = NULL;
  if (!empty($items_returned['results'])) {
    $proxy = array_pop($items_returned['results']);
  }

  if (!is_object($proxy)) {
    form_set_error('', 'An invalid proxy id has been specified.');
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
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    form_set_error('form', 'An error occurred while trying to delete the proxy. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to delete the proxy \'%proxy_id\'.', array('%proxy_id' => $form['form']['id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    form_set_error('form', 'An error occurred while trying to delete the proxy. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to delete the proxy \'%proxy_id\'.', array('%proxy_id' => $form['form']['id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
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
