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
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML representing the page.
 */
function mfcs_request_synchronize_0_page($page_number = NULL) {
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
  $class_disabled = 'column-disabled ';
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
    elseif ($order == 'disabled') {
      $sorting['disabled'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(8 => strtolower($sorting[$order]));
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
      'disabled' => 'disabled',
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

    $row['disabled'] = array(
      'data' => $item->disabled == 1 ? 'Disabled' : 'Enabled',
      'class' => $class_disabled,
    );

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    if ($item->type == MFCS_SYNCHRONIZE_TYPE_ICAL) {
      $row['type']['data'] = 'iCalendar';
      $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/synchronize-0/ical/' . rawurlencode($item->password) . $url_arguments . '" class="operation-link operation-link-view" title="Synchronization URL">Link</a>';
    }
    elseif ($item->type == MFCS_SYNCHRONIZE_TYPE_NONE) {
      $row['type']['data'] = 'None';
    }

    $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/synchronize-0/edit/' . ((int) $item->id) . $url_arguments . '" class="operation-link operation-link-edit">Edit</a>';
    $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/synchronize-0/create/' . ((int) $item->id) . $url_arguments . '" class="operation-link operation-link-copy">Copy</a>';

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
    'disabled' => array('field' => 'disabled', 'data' => 'Status', 'align' => 'center'),
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
      array('id' => 8, 'column' => 'disabled', 'name' => 'Status', 'className' => 'column-disbled'),
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

  $markup = '<div id="mfcs_request_synchronize_0_form">';
  $markup .= '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $markup .= mcneese_libraries_theme_table($synchronize_list, $datatable_settings);
  #markup .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';
  $markup .=  '<br><div>';
  $markup .=  'The <strong>iCalendar</strong> synchronize process is intended to be used by <strong>zimbra calendar</strong> for processing only the approved requests from the current day forward. ';
  $markup .=  'Other ip addresses may be added for exceptional, testing, or training purposes. ';
  $markup .=  '</div>';
  $markup .=  '<br><div>';
  $markup .= 'You may <a href="' . $base_path . 'requests/synchronize-0/create' . $url_arguments . '" class="operation-link operation-link-add">Add a new synchronization access</a>.';
  $markup .=  '</div>';
  $markup .=  '</div>';

  return $markup;
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
  $year = date('Y', $min_date);

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
    $holidays = mfcs_get_holidays($year, $min_date, NULL, FALSE, TRUE, FALSE, FALSE, 1);
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
    $unavailabilities = mfcs_get_unavailabilities($year, NULL, $search_building_ids, $min_date, NULL, TRUE, FALSE, TRUE, FALSE, 1);
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
 * Provides the facilities use request synchronize access create/copy/edit form.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $synchronize_id
 *   (optional) The ID of an existing synchronize access to copy from.
 *   If NULL, then a new synchronize access is created.
 * @param bool $new
 *   (optional) If TRUE, and synchronize_id is not NULL, then this is a copy form.
 *   Otherwise if synchronize_id is not NULL, then this is an edit form.
 *
 * @return array
 *   The settings representing the form.
 */
function mfcs_request_synchronize_0_create_form($form, &$form_state, $synchronize_id = NULL, $new = TRUE) {
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
  if (!is_null($synchronize_id) && !cf_is_integer($synchronize_id)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_bool($new)) {
    cf_error::invalid_bool('new');

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

  if (is_null($synchronize_id)) {
    mfcs_add_canonical_headers('requests/synchronize-0/create');
    $page_title = 'Create Synchronize Access';
  }
  else {
    if ($new) {
      mfcs_add_canonical_headers('requests/synchronize-0/create/' . $synchronize_id);
      $page_title = 'Copy Synchronize Access: ' . $synchronize_id;
    }
    else {
      mfcs_add_canonical_headers('requests/synchronize-0/edit/' . $synchronize_id);
      $page_title = 'Edit Synchronize Access: ' . $synchronize_id;
    }
  }

  drupal_set_title($page_title);

  $user = cf_current_user();

  // @todo: load existing settings for 'copy' process.
  $default_values = array(
    'type' => MFCS_SYNCHRONIZE_TYPE_ICAL,
    'password' => NULL,
    'address' => NULL,
    'status' => 0,
  );

  $item = NULL;
  if (!is_null($synchronize_id)) {
    $item = mfcs_request_synchronize_load_by_id($synchronize_id);

    if (is_object($item)) {
      $default_values['type'] = $item->type;
      $default_values['password'] = $item->password;
      $default_values['address'] = $item->address;
      $default_values['status'] = (int) $item->disabled;
    }
  }

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['synchronize_id'] = array(
    '#type' => 'value',
    '#value' => $synchronize_id,
  );

  $form['form']['user'] = array(
    '#type' => 'value',
    '#value' => $user,
  );

  $form['form']['new'] = array(
    '#type' => 'value',
    '#value' => $new,
  );

  $form['form']['item'] = array(
    '#type' => 'value',
    '#value' => $item,
  );

  $form['synchronize'] = array(
    '#tree' => TRUE,
  );

  $options = array(
    MFCS_SYNCHRONIZE_TYPE_NONE => 'None',
    MFCS_SYNCHRONIZE_TYPE_ICAL => 'Calendar',
  );

  $form['synchronize']['type'] = array(
    '#id' => 'synchronize-type',
    '#type' => 'select',
    '#title' => 'Type',
    '#options' => $options,
    '#default_value' => $default_values['type'],
    '#required' => TRUE,
    '#description' => 'The synchrinoze type.',
  );

  // this will not be copied.
  $form['synchronize']['password'] = array(
    '#id' => 'synchronize-password',
    '#type' => 'textfield',
    '#title' => 'Password',
    '#default_value' => $default_values['password'],
    '#required' => FALSE,
    '#description' => 'A unique password associated with the specified ip address. This is recommended to be a randomly generated long hash. Leave empty to randomly generate.',
  );

  $form['synchronize']['address'] = array(
    '#id' => 'synchronize-address',
    '#type' => 'textfield',
    '#title' => 'Address',
    '#default_value' => $default_values['address'],
    '#required' => TRUE,
    '#description' => 'A valid ip address to restrict this synchronization to.',
  );

  $form['synchronize']['status'] = array(
    '#id' => 'synchronize-status',
    '#type' => 'select',
    '#title' => 'Status',
    '#options' => array(0 => 'Enabled', 1 => 'Disabled'),
    '#default_value' => $default_values['status'],
    '#required' => TRUE,
    '#description' => 'Whether or not this access is disabled.',
  );

  $form['submit'] = array(
    '#id' => 'mfcs-synchronize-create-submit',
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $form['submit']['cancel'] = array(
    '#id' => 'mfcs-synchronize-create-submit-cancel',
    '#name' => 'mfcs-synchronize-create-submit-cancel',
    '#type' => 'submit',
    '#default_value' => 'Cancel',
  );

  $form['submit']['submit'] = array(
    '#id' => 'mfcs-synchronize-create-submit-submit',
    '#name' => 'mfcs-synchronize-create-submit-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
  );

  return $form;
}


/**
 * Provides the facilities use request synchronize access create/copy/edit form validation.
 */
function mfcs_request_synchronize_0_create_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-synchronize-create-submit-cancel') {
    $form['synchronize']['address']['#required'] = FALSE;
  }
}

/**
 * Provides the facilities use request synchronize access create/copy/edit form validation.
 */
function mfcs_request_synchronize_0_create_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-synchronize-create-submit-cancel') {
    return;
  }

  if (strlen($form_state['values']['synchronize']['password']) != 0 && preg_match('/\W/', $form_state['values']['synchronize']['password']) > 0) {
    form_set_error('synchronize][password', 'The <strong>password</strong> may only be letters, numbers, and underscores.');
  }

  if (@inet_pton($form_state['values']['synchronize']['address']) === FALSE) {
    form_set_error('synchronize][address', 'The provided <strong>address</strong> is not a valid IP address.');
  }
}

/**
 * Provides the facilities use request synchronize access create/copy/edit form submital.
 */
function mfcs_request_synchronize_0_create_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-synchronize-create-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/synchronize-0');
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  global $user;

  $instance = mfcs_instance();

  $values = array(
    'type' => $form_state['values']['synchronize']['type'],
    'address' => $form_state['values']['synchronize']['address'],
    'disabled' => $form_state['values']['synchronize']['status'],
    'updated' => $instance,
  );

  if (strlen($form_state['values']['synchronize']['password']) == 0) {
    $values['password'] = preg_replace('/\W/', '', drupal_random_key(40));
    if (strlen($values['password']) < 21) {
      $values['password'] .= preg_replace('/\W/', '', drupal_random_key(20));
    }
  }
  else {
    $values['password'] = $form_state['values']['synchronize']['password'];
  }

  $transaction = db_transaction();
  $failure = FALSE;
  try {
    if (is_numeric($form['form']['synchronize_id']['#value'])) {
      if ($form['form']['new']['#value']) {
        $query = db_insert('mfcs_synchronize');

        $values['user_id'] = $user->uid;
        $values['created'] = $instance;

        $action = 'copy';
      }
      else {
        $query = db_update('mfcs_synchronize');
        $query->condition('id', $form['form']['synchronize_id']['#value']);

        $values['id'] = $form['form']['synchronize_id']['#value'];

        $action = 'update';
      }
    }
    else {
      $query = db_insert('mfcs_synchronize');

      $values['user_id'] = $user->uid;
      $values['created'] = $instance;

      $action = 'create';
    }

    $query->fields($values);
    $query->execute();
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);
    $failure = TRUE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);
    $failure = TRUE;
  }

  if ($failure) {
    form_set_error('form', 'An error occurred while trying to ' . $action . ' the synchronizaton access. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to ' . $action . ' the synchronizaton access: @synchronize_id.', array('@synchronize_id' => $form['form']['synchronize_id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/synchronize-0');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request synchronize access delete form.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $synchronize_id
 *   The ID of an existing synchronize access to delete.
 *
 * @return array
 *   The settings representing the form.
 */
function mfcs_request_synchronize_0_delete_form($form, $form_state, $synchronize_id) {
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
  if (!is_null($synchronize_id) && !cf_is_integer($synchronize_id)) {
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

  $item = mfcs_request_synchronize_load_by_id($synchronize_id);

  if (!is_object($item)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $page_title = 'Delete Synchronize Access';
  drupal_set_title($page_title);

  $options = array(
    MFCS_SYNCHRONIZE_TYPE_NONE => 'None',
    MFCS_SYNCHRONIZE_TYPE_ICAL => 'Calendar',
  );

  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['synchronize_id'] = array(
    '#type' => 'value',
    '#value' => $synchronize_id,
  );

  $form['confirmation'] = array(
    '#markup' => '',
  );

  $form['confirmation']['#markup'] .= '<h3>Are you sure you want to delete this synchronization access?</h3> ';
  $form['confirmation']['#markup'] .= '<div>';
  $form['confirmation']['#markup'] .= '<strong>ID</strong>: ' . $item->id . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Type</strong>: ' . $options[$item->type] . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Password</strong>: ' . $item->password . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Address</strong>: ' . $item->address . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Status</strong>: ' . ($item->disabled == 1 ? 'Disabled' : 'Enabled') . '<br>';
  $form['confirmation']['#markup'] .= '</div>';
  $form['confirmation']['#markup'] .= '<br>';

  $form['display']['submit'] = array(
    '#id' => 'mfcs-synchronize-delete-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'mfcs-synchronize-delete-submit-no',
    '#name' => 'mfcs-synchronize-delete-submit-no',
    '#type' => 'submit',
    '#default_value' => 'No',
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'mfcs-synchronize-delete-submit-yes',
    '#name' => 'mfcs-synchronize-delete-submit-yes',
    '#type' => 'submit',
    '#default_value' => 'Yes',
  );

  return $form;
}

/**
 * Provides the facilities use request synchronize access delete form submital.
 */
function mfcs_request_synchronize_0_delete_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-synchronize-delete-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/synchronize-0');
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $transaction = db_transaction();
  $failure = FALSE;
  try {
    $query = db_delete('mfcs_synchronize');
    $query->condition('id', $form['form']['synchronize_id']['#value']);
    $query->execute();
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);
    $failure = TRUE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);
    $failure = TRUE;
  }

  if ($failure) {
    form_set_error('form', 'An error occurred while trying to delete the synchronizaton access. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to delete the synchronizaton access: @synchronize_id.', array('@synchronize_id' => $form['form']['synchronize_id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/synchronize-0');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Load the synchronize access by the specified id.
 *
 * @param int $id
 *   The synchronize id.
 *
 * @return object|FALSE
 *   The synchronize result item.
 *   FALSE is returned on error.
 */
function mfcs_request_synchronize_load_by_id($id) {
  // instead of writing a new load function, use mfcs_management_search_table() to get the data, but expect only a single result.
  mfcs_include(MFCS_INCLUDE_MANAGEMENT);

  $conditions = array();
  $options = array(
    'sort' => 'asc',
    'start' => 0,
    'limit' => 1,
    'count' => FALSE,
    'fields' => array(
      'id' => 'id',
      'user_id' => 'user_id',
      'created' => 'created',
      'updated' => 'updated',
      'password' => 'password',
      'type' => 'type',
      'address' => 'address',
      'disabled' => 'disabled',
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

  $items_returned = mfcs_management_search_table('mfcs_synchronize', 'ms', 'id', $conditions, $options);

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }

  // expect to load only a single item.
  if (count($items) != 1) {
    return FALSE;
  }

  return reset($items);
}

/**
 * @} End of '@addtogroup mfcs'.
 */
