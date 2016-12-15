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
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_synchronize_0_form($form, &$form_state, $page_number = NULL) {
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
  if (!is_null($page_number) && !cf_is_integer($page_number)) {
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

  mfcs_add_canonical_headers('requests/synchronize-0/' . $page_number);

  $page_title = 'Synchronize';
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

  $class_id = 'column-id ';
  $class_user_id = 'column-account_id no-script-hidden ';
  $class_name = 'column-account ';
  $class_created = 'column-created ';
  $class_updated = 'column-updated ';
  $class_password = 'column-password ';
  $class_type = 'column-type ';
  $class_address = 'column-address ';
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
    elseif ($order == 'address') {
      $sorting['address'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(7 => strtolower($sorting[$order]));
      $class_address .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'ASC';
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
      'address' => 'address',
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

  $count_options = $options;
  $count_options['start'] = 0;
  $count_options['results'] = FALSE;
  unset($count_options['limit']);

  $conditions = array();

  $table_name = 'mfcs_synchronize';
  $table_alias = 'ms';
  $primary_key = 'id';

  $items_returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions, $count_options);

  $items_total = 0;
  if (is_array($items_returned) && array_key_exists('count', $items_returned)) {
    $items_total = $items_returned['count'];
  }

  $items_returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions, $options);

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

    // note: previously the id was added as a no-js failsafe, but it causes an annoying flicker on load (because the load speed is sufficiently fast enough to flicker but slow enough to see).
    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'users-0/view/' . $item->user_id . $url_arguments . '" class="user-link">' . check_plain($item->name) . '</a>',
      'class' => $class_name,
    );

    $row['created'] = array(
      'data' => check_plain(date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $item->created)),
      'class' => $class_created,
    );

    $row['updated'] = array(
      'data' => check_plain(date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $item->updated)),
      'class' => $class_updated,
    );

    $row['password'] = array(
      'data' => check_plain($item->password),
      'class' => $class_password,
    );

    $row['type'] = array(
      'data' => 'Unknown',
      'class' => $class_type,
    );

    $row['address'] = array(
      'data' => check_plain($item->address),
      'class' => $class_address,
    );

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    if ($item->type == MFCS_SYNCHRONIZE_TYPE_ICAL) {
      $row['type']['data'] = 'iCalendar';
      $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/synchronize-0/ical/' . rawurlencode($item->password) . $url_arguments . '" class="operation-link operation-link-view" title="Synchronization URL">Link</a>';
    }

    $rows[] = $row;
  }


  $synchronize_list = array();
  $synchronize_list['caption'] = 'Synchronize';
  $synchronize_list['header'] = array(
    'id' => array('field' => 'id', 'data' => 'ID', 'align' => 'center'),
    'account_id' => array('field' => 'user_id', 'data' => 'Account ID', 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'account' => array('field' => 'name', 'data' => 'Account', 'align' => 'center', 'class' => array('script-hidden')),
    'created' => array('field' => 'created', 'data' => 'Created', 'align' => 'center', 'class' => array('script-hidden')),
    'updated' => array('field' => 'updated', 'data' => 'Updated', 'align' => 'center', 'class' => array('script-hidden')),
    'password' => array('field' => 'password', 'data' => 'Password', 'align' => 'center'),
    'type' => array('field' => 'type', 'data' => 'Type', 'align' => 'center'),
    'address' => array('field' => 'address', 'data' => 'Address', 'align' => 'center'),
    'operations' => array('data' => 'Operations', 'align' => 'center', 'class' => array('no-print')),
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
  $synchronize_list['empty'] = 'This system has no synchronization URLs defined.';

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
      'url' => '\'' . $base_path . 'requests/synchronize-0/ajax' . $url_arguments . '\'',
      'type' => '\'POST\'',
    ),
    'deferRender' => TRUE,
    'deferLoading' => (int) $items_total,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => 'ID', 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'account_id', 'name' => 'Account ID', 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 2, 'column' => 'account', 'name' => 'Account', 'className' => 'column-account', 'visible' => FALSE),
      array('id' => 3, 'column' => 'created', 'name' => 'Created', 'className' => 'column-created', 'visible' => FALSE),
      array('id' => 4, 'column' => 'updated', 'name' => 'Updated', 'className' => 'column-updated', 'visible' => FALSE),
      array('id' => 5, 'column' => 'password', 'name' => 'Password', 'className' => 'column-password'),
      array('id' => 6, 'column' => 'type', 'name' => 'Type', 'className' => 'column-type'),
      array('id' => 7, 'column' => 'address', 'name' => 'Address', 'className' => 'column-address'),
      array('id' => 8, 'column' => NULL, 'name' => 'Operations', 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
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
      'sEmptyTable' => '"' . $synchronize_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $synchronize_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
    'responsive' => TRUE,
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
    $navigation_top .= '<a href="' . $base_path . 'requests/synchronize-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/synchronize-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['synchronize_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['synchronize_list']['#markup'] .= mcneese_libraries_theme_table($synchronize_list, $datatable_settings);
  #$form['synchronize_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';
  $form['synchronize_list']['#markup'] .=  '<br><div>';
  $form['synchronize_list']['#markup'] .=  'The <strong>iCalendar</strong> synchronize process is intended to be used by <strong>zimbra calendar</strong> for processing only the approved requests from the current day forward. ';
  $form['synchronize_list']['#markup'] .=  'Other ip addresses may be added for exceptional, testing, or training purposes. ';
  $form['synchronize_list']['#markup'] .=  '</div>';

  return $form;
}

/**
 * Renders the iCalendar sync content.
 *
 * On valid pages, this generates an iCalendar feed.
 *
 * @param string $password
 *   The password needed to be granted anonymous access to the public iCalendar information.
 *   This may be used in the future to control what information gets loaded.
 * @param string|null $custom
 *   When NULL, requests for the current moment in time and forward are loaded.
 *   When 'all', requests for the past, present, and future are loaded.
 * @param int $load_holidays
 *   When 2, holidays are not loaded.
 *   When 1, holidays are loaded.
 *   When 0, use user settings.
 * @param int $load_unavailabilities
 *   When 2, unavailabilities are not loaded.
 *   When 1, unavailabilities are loaded.
 *   When 0, use user settings.
 */
function mfcs_request_synchronize_0_ical_page($password, $custom = NULL, $load_holidays = 0, $load_unavailabilities = 2) {
  if (!is_string($password) || empty($password)) {
    cf_error::invalid_string('password');
    return;
  }

  if (!is_null($custom) && !is_string($custom)) {
    cf_error::invalid_string('custom');
    return;
  }

  switch($custom) {
    case NULL:
    case 'all':
      break;
    default:
      drupal_access_denied();
      drupal_exit();
      return;
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_ICAL);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  mfcs_add_canonical_headers('requests/synchronize-0/' . $password);

  $instance = mfcs_instance();
  $min_date = strtotime('midnight today', $instance);

  global $base_url;
  global $mfcs_determined;

  $user = cf_current_user();

  $instance = mfcs_instance();

  if (!is_null($load_holidays) && !is_numeric($load_holidays) && $load_holidays != 0 && $load_holidays != 1 && $load_holidays != 2) {
    cf_error::invalid_integer('load_holidays');
    return;
  }

  if ($load_holidays == 2) {
    $load_holidays = FALSE;
  }
  elseif ($load_holidays == 1) {
    $load_holidays = TRUE;
  }
  elseif (isset($user->mfcs['calendar_holidays']) && $user->mfcs['calendar_holidays'] === FALSE) {
    $show_holidays = FALSE;
  }
  else {
    $show_holidays = TRUE;
  }

  if (!is_null($load_unavailabilities) && !is_numeric($load_unavailabilities) && $load_unavailabilities != 0 && $load_unavailabilities != 1 && $load_unavailabilities != 2) {
    cf_error::invalid_integer('load_unavailabilities');
    return;
  }

  if ($load_unavailabilities == 2) {
    $load_unavailabilities = FALSE;
  }
  elseif ($load_unavailabilities == 1) {
    $load_unavailabilities = TRUE;
  }
  elseif (isset($user->mfcs['calendar_unavailabilities']) && $user->mfcs['calendar_unavailabilities'] === FALSE) {
    $show_unavailabilities = FALSE;
  }
  else {
    $show_unavailabilities = TRUE;
  }

  $search = array();
  if ($custom == 'all') {
    $min_date = NULL;
  }
  else {
    $target_field_name = 'field_dates-date-start-0';
    $search[$target_field_name]['group_name'] = 'dates';
    $search[$target_field_name]['field_name'] = 'date';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $instance;
    $search[$target_field_name]['type'] = 'date';
    $search[$target_field_name]['multiple'] = TRUE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_GREATER_THAN_EQUAL;

    $target_field_name = 'field_dates-date-stop-0';
    $search[$target_field_name]['group_name'] = 'dates';
    $search[$target_field_name]['field_name'] = 'date';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $instance;
    $search[$target_field_name]['type'] = 'date';
    $search[$target_field_name]['multiple'] = TRUE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_GREATER_THAN_EQUAL;
  }

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
    'cancelled' => MFCS_LOAD_CANCELLED_ALL,
  );

  $items_returned = mfcs_request_load_listing($search, $options);

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }
  elseif ($items_returned === FALSE) {
    $items = FALSE;
  }

  $search_building_ids = NULL;
  if (!empty($mfcs_determined['filters']['field_top-building-0']['search'])) {
    $search_building_ids = $mfcs_determined['filters']['field_top-building-0']['search'];
  }


  // conditionally load holidays.
  $holidays = NULL;
  $show_holidays = TRUE;
  if (isset($user->mfcs['calendar_holidays']) && $user->mfcs['calendar_holidays'] === FALSE) {
    $show_holidays = FALSE;
  }

  if ($show_holidays) {
    $holidays = mfcs_get_holidays($year, $min_date, NULL, FALSE, TRUE);
  }

  if (!is_array($holidays)) {
    $holidays = array();
  }


  // conditionally load unavailabilities.
  $unavailabilities = NULL;
  $show_unavailabilities = TRUE;
  if (isset($user->mfcs['calendar_unavailabilities']) && $user->mfcs['calendar_unavailabilities'] === FALSE) {
    $show_unavailabilities = FALSE;
  }

  if ($show_unavailabilities) {
    $unavailabilities = mfcs_get_unavailabilities($year, NULL, $search_building_ids, $min_date, NULL, TRUE, FALSE, TRUE);
  }

  if (!is_array($unavailabilities)) {
    $unavailabilities = array();
  }


  $ical = FALSE;
  if ($items !== FALSE) {
    mfcs_include(MFCS_INCLUDE_ICAL);

    $ical = mfcs_ical_generate($items, $holidays, $unavailabilities, $min_date);
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
  header('Content-Type: text/calendar; method="PUBLISH"; component="VEVENT"');
  header('Content-Length: ' . $ical['length']);
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

  foreach ($ical['content'] as $content) {
    print($content);
  }

  unset($ical);
  exit();
}

/**
 * @} End of '@addtogroup mfcs'.
 */
