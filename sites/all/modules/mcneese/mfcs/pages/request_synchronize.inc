<?php

/**
 * @file
 * Defines McNeese facilities use request synchronize page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */
/**
 * Provides the facilities use request synchronize main page(s).
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
function mfcs_request_synchronize_0_form($form, &$form_state, $page_number = 0) {
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

  // require that page number be only an integer.
  if (!cf_is_integer($page_number)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/synchronize-0/' . $page_number);

  $page_title = "Synchronize";
  drupal_set_title($page_title);

  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_MANAGEMENT);

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
  $class_user_id = 'no-script-hidden ';
  $class_name = '';
  $class_created = '';
  $class_updated = '';
  $class_password = '';
  $class_type = '';
  $class_operations = 'no-print ';


  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'DESC';
    $order_column = array(0 => strtolower($sorting['id']));
    $class_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'account id') {
      $sorting['user_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_user_id .= 'sorting_1 ';
    }
    elseif ($order == 'account') {
      $sorting['name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_name .= 'sorting_1 ';
    }
    elseif ($order == 'created') {
      $sorting['created'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_created .= 'sorting_1 ';
    }
    elseif ($order == 'updated') {
      $sorting['updated'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_updated .= 'sorting_1 ';
    }
    elseif ($order == 'password') {
      $sorting['password'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_password .= 'sorting_1 ';
    }
    elseif ($order == 'type') {
      $sorting['type'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(6 => strtolower($sorting[$order]));
      $class_type .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'DESC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 20;
  $options = array(
    'sort' => $sorting,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
    'fields' => array(
      'id' => 'id',
      'user_id' => 'user_id',
      'created' => 'created',
      'updated' => 'updated',
      'password' => 'password',
      'type' => 'type',
    ),
    'associative' => FALSE,
  );

  $options['extra_fields']['users'] = array(
    'alias' => 'u',
    'join' => 'inner',
    'on' => 'ms.user_id = u.uid',
    'columns' => array(
      'user_id' => 'uid',
      'name' => 'name',
    ),
  );

  $conditions = array();

  $table_name = 'mfcs_synchronize';
  $table_alias = 'ms';
  $primary_key = 'id';

  $returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions, $options);

  $returned_total = 0;
  if (is_array($returned) && array_key_exists('count', $returned)) {
    $returned_total = $returned['count'];
  }

  $items = array();
  if (isset($returned['results']) && is_array($returned['results'])) {
    $items = $returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($returned_total > 0 && $options['start'] >= $returned_total) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $rows = array();
  foreach ($items as $id => $item) {
    $row = array();

    $row['id'] = array(
      'data' => (int) $item->id,
      'class' => $class_id,
    );

    $row['account_id'] = array(
      'data' => (int) $item->user_id,
      'class' => $class_user_id,
    );

    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'user/' . $item->user_id . $url_arguments . '" class="user-link">' . check_plain($item->name) . '</a>' . ' <span class="user-id script-hidden">(id: ' .  $item->user_id . ')</span>',
      'class' => $class_name,
    );

    $row['created'] = array(
      'data' => check_plain(date("Y/m/d h:i:s a", $item->created)),
      'class' => $class_created,
    );

    $row['updated'] = array(
      'data' => check_plain(date("Y/m/d h:i:s a", $item->updated)),
      'class' => $class_updated,
    );

    $row['password'] = array(
      'data' => check_plain($item->password),
      'class' => $class_password,
    );

    $row['type'] = array(
      'data' => "Unknown",
      'class' => $class_type,
    );

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    if ($item->type == MFCS_SYNCHRONIZE_TYPE_ICAL) {
      $row['type']['data'] = "iCal";
      $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/synchronize-0/ical/' . rawurlencode($item->password) . $url_arguments . '" class="operation-link operation-link-view" title="Synchronization URL">Link</a>';
    }

    $rows[] = $row;
  }


  $synchronize_list = array();
  $synchronize_list['caption'] = t("Synchronize");
  $synchronize_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'account_id' => array('field' => 'user_id', 'data' => "Account ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'account' => array('field' => 'name', 'data' => "Account", 'align' => 'center', 'class' => array('script-hidden')),
    'created' => array('field' => 'created', 'data' => "Created", 'align' => 'center'),
    'updated' => array('field' => 'updated', 'data' => "Updated", 'align' => 'center'),
    'password' => array('field' => 'password', 'data' => "Password", 'align' => 'center'),
    'type' => array('field' => 'type', 'data' => "Type", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $synchronize_list['attributes'] = array();
  $synchronize_list['attributes']['id'] = 'mfcs-synchronize_form';
  $synchronize_list['attributes']['class'] = array(
    'no_default',
    'mfcs-synchronize_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $synchronize_list['attributes']['summary'] = 'List of synchronization URLs.';

  $synchronize_list['rows'] = $rows;
  $synchronize_list['empty'] = t("This system has no synchronization URLs defined.");

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
    'ajax' => "'" . $base_path . 'requests/synchronize-0/ajax' . $url_arguments . "'",
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'account_id', 'name' => "Account ID", 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 2, 'column' => 'account', 'name' => "Account", 'className' => 'column-account', 'visible' => FALSE),
      array('id' => 3, 'column' => 'created', 'name' => "Created", 'className' => 'column-created'),
      array('id' => 4, 'column' => 'updated', 'name' => "Updated", 'className' => 'column-updated'),
      array('id' => 5, 'column' => 'password', 'name' => "Password", 'className' => 'column-password'),
      array('id' => 6, 'column' => 'type', 'name' => "Type", 'className' => 'column-type'),
      array('id' => 7, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter: "',
    ),
    'searchDelay' => 1200,
  );

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/synchronize-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/synchronize-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/synchronize-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/synchronize-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($navigation_total > $returned_total) {
    $navigation_total = $returned_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">' . "\n";
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $returned_total . " entries.\n";
  $navigation_bottom .= '</div>' . "\n";

  if ($returned_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/synchronize-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/synchronize-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['synchronize_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['synchronize_list']['#markup'] .= mcneese_libraries_theme_table($synchronize_list, $datatable_settings);
  $form['synchronize_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  return $form;
}

/**
 * Renders the iCal sync content.
 *
 * On valid pages, this generates an iCal feed from the current moment in time and forward.
 *
 * @param string $password
 *   The password needed to be granted anonymous access to the public iCal information.
 *   This may be used in the future to control what information gets loaded.
 */
function mfcs_request_synchronize_0_ical_page($password) {
  if (!is_string($password) || empty($password)) {
    cf_error::invalid_string('password');
    return;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_ICAL);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  mfcs_add_canonical_headers('requests/synchronize-0/' . $password);

  $instance = mfcs_instance();

  global $base_url;
  global $mfcs_determined;

  $instance = mfcs_instance();
  $search = mfcs_miscellaneous_generate_requests_by_date_search($instance);

  if (!empty($mfcs_determined['filters'])) {
    foreach ($mfcs_determined['filters'] as $filter_key => $filter) {
      $search[$filter_key] = $filter;
    }
  }

  $options = array(
    'limit' => 0,
    'sort' => array('date' => 'ASC'),
    'extra_fields' => mfcs_request_ical_generate_extra_fields(),
    'distinct_id' => TRUE,
    'user_id' => MFCS_USER_REQUEST,
  );

  $items_returned = mfcs_request_load_listing($search, $options);

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }
  elseif ($items_returned === FALSE) {
    $items = FALSE;
  }

  $ical = FALSE;
  if ($items !== FALSE) {
    mfcs_include(MFCS_INCLUDE_ICAL);

    $ical = mfcs_ical_generate($items, $instance);
  }

  if ($ical === FALSE) {
    drupal_not_found();
    drupal_exit();
  }

  $GLOBALS['devel_shutdown'] = FALSE;

  // make sure to perform output buffer (if possible) to reduce chances of "headers already sent" issues.
  $ob_level = ob_get_level();
  for ($i = 0; $i < $ob_level; $i++) {
    ob_clean();
  }
  ob_start();

  $filename = 'mcneese-facilities_use.ics';
  $content_location = $base_url . '/synchronize-0';

  header('HTTP/1.1 200 OK', TRUE, 200);
  header('Content-Type: text/plain');

  // if available, use drupal_bytes_in_string() to protect from potentialy multibyte issues.
  if (function_exists('drupal_bytes_in_string')) {
    header('Content-Length: ' . drupal_bytes_in_string($ical));
  }
  else {
    header('Content-Length: ' . strlen($ical));
  }

  header('Content-Transfer-Encoding: binary');
  #header('Content-type: text/plain; charset=utf-8');
  header('Content-type: text/calendar; charset=utf-8');
  header('Content-Disposition: inline; filename="' . $filename . '"');
  header('Content-Description: File Transfer');
  header('Content-Location: ' . $content_location);
  header('Last-Modified: ' . gmdate(DATE_RFC7231, $instance));
  header('Date: ' . gmdate(DATE_RFC7231, $instance));

  // flush buffer after headers have been processed (to hopefully avoid "headers already sent" issues).
  ob_end_flush();

  print($ical);
  exit();
}

/**
 * @} End of '@addtogroup mfcs'.
 */
