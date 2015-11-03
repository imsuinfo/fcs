<?php

/**
 * @file
 * Defines McNeese facilities use ajax functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Ajax initialization function used to prepare request variables.
 *
 * @return array
 *   An array of variables in the following order:
 *   - draw: draw setting.
 *   - columns: table columns settings.
 *   - order: table order settings.
 *   - options: table options.
 *   - timestamp: timestamp variable.
 */
function mfcs_ajax_setup_request_settings() {
  $draw = 1;
  if (isset($_GET['draw'])) {
    $draw += (int) $_GET['draw'];
  }

  $columns = NULL;
  if (isset($_GET['columns']) && is_array($_GET['columns'])) {
    $columns = $_GET['columns'];
  }

  $order = NULL;
  if (isset($_GET['order']) && is_array($_GET['order'])) {
    $order = $_GET['order'];
  }

  $options = array(
    'join' => TRUE,
    'start' => 0,
    'limit' => NULL,
    'count' => TRUE,
  );

  if (isset($_GET['start']) && is_numeric($_GET['start'])) {
    $options['start'] = (int) $_GET['start'];
  }

  if (isset($_GET['length']) && is_numeric($_GET['length'])) {
    $options['limit'] = (int) $_GET['length'];
  }

  // A length of -1 means all values, set limit to 0 which means all values.
  if ($options['limit'] < 0) {
    $options['limit'] = 0;
  }

  $timestamp = mfcs_instance();
  if (isset($_GET['_']) && is_array($_GET['_'])) {
    $timestamp = $_GET['_'];
  }

  return array($draw, $columns, $order, $options, $timestamp);
}

/**
 * Ajax function call for loading reviewers list datatable.
 *
 * @param int $classification
 *   Number representing the request classification.
 *   This is not the reviewer classification.
 * @param int $review_step
 *   Number representing the request review step.
 */
function mfcs_ajax_reviewers_0_list($classification, $review_step) {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $response = array();

  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_classifications = mfcs_get_request_classification_list_options();
  $review_steps = mfcs_get_review_step_list_options();
  $reviewer_classifications = mfcs_get_reviewer_classification_list_options();
  $reviewer_classifications_list = mfcs_get_reviewer_classification_list_options('select');

  if (!array_key_exists($classification, $request_classifications)) {
    drupal_json_output($response);
    exit();
  }

  if (!array_key_exists($review_step, $review_steps)) {
    drupal_json_output($response);
    exit();
  }


  // load ajax/json request settings.
  list($draw, $columns, $order, $options, $timestamp) = mfcs_ajax_setup_request_settings();

  $search = FALSE;
  $limit_reviewer_classifications = NULL;
  if (isset($_GET['search']['value']) && is_string($_GET['search']['value']) && !empty($_GET['search']['value'])) {
    $search = TRUE;

    $regex_safe_search = preg_quote($_GET['search']['value'], '/');

    // search through the already loaded lists, the database stores the values as integers and not strings.
    $limit_reviewer_classifications = array();

    foreach ($reviewer_classifications as $key => $value) {
      if (preg_match('/' . $regex_safe_search . '/i', $value) > 0) {
        $limit_reviewer_classifications[$key] = $key;
      }
    }
  }

  // build ajax/json response data.
  $response = array(
    'draw' => $draw,
  );

  $sorting = array();
  if (!isset($order[0]['column'])) {
    $sorting['reviewer_classification_name'] = 'ASC';
  }
  else {
    $order_column = (int) $order[0]['column'];
    $order_direction = isset($order[0]['dir']) && is_string($order[0]['dir']) ? strtoupper($order[0]['dir']) : 'DESC';

    if ($order_column == 0) {
      $sorting['id'] = $order_direction;
    }
    elseif ($order_column == 1) {
      $sorting['reviewer_classification_name'] = $order_direction;
    }
    elseif ($order_column == 2) {
      $sorting['user_id'] = $order_direction;
    }
    elseif ($order_column == 3) {
      $sorting['u.name'] = $order_direction;
    }
    elseif ($order_column == 4) {
      $sorting['first_name'] = $order_direction;
    }
    elseif ($order_column == 5) {
      $sorting['last_name'] = $order_direction;
    }
  }

  // always provide the unique id in the ordering to guarantee consistency in range results.
  if (!isset($sorting['id'])) {
    $sorting['id'] = 'DESC';
  }

  if (!empty($sorting)) {
    $options['sort'] = $sorting;
  }

  $conditions = array(
    'mr.request_classification' => $classification,
    'mr.review_step' => $review_step,
    'mr.disabled' => 0,
  );

  if ($search) {
    $count_conditions = $conditions;
    $count_options = $options;

    $conditions['or_1'] = array();
    if (!empty($limit_reviewer_classifications)) {
      $conditions['or_1']['mr.reviewer_classification'] = array(
        'operator' => 'IN',
        'value' => $limit_reviewer_classifications,
      );
    }

    $conditions['or_1']['text(mr.id)'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
      'expression' => TRUE,
    );

    $conditions['or_1']['text(u.uid)'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
      'expression' => TRUE,
    );

    $conditions['or_1']['u.name'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['fdfufn.field_user_first_name_value'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['fdfuln.field_user_last_name_value'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $count_options['returned'] = FALSE;
    $reviewers_returned = mfcs_get_reviewers($count_conditions, $count_options);
    $reviewers_total = 0;
    if (is_array($reviewers_returned) && array_key_exists('count', $reviewers_returned)) {
      $reviewers_total = $reviewers_returned['count'];
    }

    $reviewers_returned = mfcs_get_reviewers($conditions, $options);
    $reviewers = array();
    if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
      $reviewers = $reviewers_returned['results'];
    }

    $response['recordsTotal'] = $reviewers_total;
    $response['recordsFiltered'] = $reviewers_returned['count'];
  }
  else {
    $reviewers_returned = mfcs_get_reviewers($conditions, $options);
    $reviewers = array();
    if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
      $reviewers = $reviewers_returned['results'];
    }

    $response['recordsTotal'] = $reviewers_returned['count'];
    $response['recordsFiltered'] = $reviewers_returned['count'];
  }

  if (!is_array($reviewers)) {
    drupal_json_output($response);
    exit();
  }

  $response['data'] = array();
  if (!empty($reviewers)) {
    foreach ($reviewers as $reviewer) {
      $row = array();

      $row[0] = $reviewer->id;

      if (isset($reviewer_classifications[$reviewer->reviewer_classification])) {
        $row[1] = $reviewer_classifications[$reviewer->reviewer_classification];
      }

      $row[2] = $reviewer->user_id;
      $row[3] = '<a href="' . $base_path . 'user/' . $reviewer->user_id . $url_arguments . '" class="user-link">' . check_plain($reviewer->name) . '</a>';
      $row[4] = check_plain($reviewer->first_name);
      $row[5] = check_plain($reviewer->last_name);

      $row[6] = '<a href="' . $base_path . 'requests/reviewers-0/delete/' . $reviewer->id . $url_arguments . '" class="operation-link operation-link-delete">Delete</a>';

      $response['data'][] = $row;
    }
  }

  drupal_json_output($response);
  exit();
}

/**
 * Ajax function call for loading proxy list datatable.
 */
function mfcs_ajax_proxy_0_list() {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $response = array();

  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  // load ajax/json request settings.
  list($draw, $columns, $order, $options, $timestamp) = mfcs_ajax_setup_request_settings();

  $search = FALSE;
  if (isset($_GET['search']['value']) && is_string($_GET['search']['value']) && !empty($_GET['search']['value'])) {
    $search = TRUE;
  }

  // build ajax/json response data.
  $response = array(
    'draw' => $draw,
  );

  $sorting = array();
  if (!isset($order[0]['column'])) {
    $sorting['id'] = 'ASC';
  }
  else {
    $order_column = (int) $order[0]['column'];
    $order_direction = isset($order[0]['dir']) && is_string($order[0]['dir']) ? strtoupper($order[0]['dir']) : 'DESC';

    if ($order_column == 0) {
      $sorting['id'] = $order_direction;
    }
    elseif ($order_column == 1) {
      $sorting['proxy_id'] = $order_direction;
    }
    elseif ($order_column == 2) {
      $sorting['proxy_name'] = $order_direction;
    }
    elseif ($order_column == 3) {
      $sorting['proxy_first_name'] = $order_direction;
    }
    elseif ($order_column == 4) {
      $sorting['proxy_last_name'] = $order_direction;
    }
    elseif ($order_column == 5) {
      $sorting['user_id'] = $order_direction;
    }
    elseif ($order_column == 6) {
      $sorting['name'] = $order_direction;
    }
    elseif ($order_column == 7) {
      $sorting['first_name'] = $order_direction;
    }
    elseif ($order_column == 8) {
      $sorting['last_name'] = $order_direction;
    }
  }

  // always provide the unique id in the ordering to guarantee consistency in range results.
  if (!isset($sorting['id'])) {
    $sorting['id'] = 'DESC';
  }

  if (!empty($sorting)) {
    $options['sort'] = $sorting;
  }

  $conditions = array(
    'mpvc.disabled' => 0,
  );

  if ($search) {
    $count_conditions = $conditions;
    $count_options = $options;

    $conditions['or_1'] = array();

    $conditions['or_1']['text(mpvc.id)'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
      'expression' => TRUE,
    );

    $conditions['or_1']['text(mpvc.proxy_id)'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
      'expression' => TRUE,
    );

    $conditions['or_1']['pu.name'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['pfdfufn.field_user_first_name_value'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['pfdfuln.field_user_last_name_value'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['text(mpvc.user_id)'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
      'expression' => TRUE,
    );

    $conditions['or_1']['u.name'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['fdfufn.field_user_first_name_value'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $conditions['or_1']['fdfuln.field_user_last_name_value'] = array(
      'operator' => 'ILIKE',
      'value' => '%' . db_like($_GET['search']['value']) . '%',
    );

    $count_options['returned'] = FALSE;
    $proxies_returned = mfcs_get_proxy_venue_coordinators($count_conditions, $count_options);

    $proxies_total = 0;
    if (is_array($proxies_returned) && array_key_exists('count', $proxies_returned)) {
      $proxies_total = $proxies_returned['count'];
    }

    $proxies_returned = mfcs_get_proxy_venue_coordinators($conditions, $options);
    $proxies = array();
    if (isset($proxies_returned['results']) && is_array($proxies_returned['results'])) {
      $proxies = $proxies_returned['results'];
    }

    $response['recordsTotal'] = $proxies_total;
    $response['recordsFiltered'] = $proxies_returned['count'];
  }
  else {
    $proxies_returned = mfcs_get_proxy_venue_coordinators($conditions, $options);

    $proxies_total = 0;
    if (is_array($proxies_returned) && array_key_exists('count', $proxies_returned)) {
      $proxies_total = $proxies_returned['count'];
    }

    $proxies = array();
    if (isset($proxies_returned['results']) && is_array($proxies_returned['results'])) {
      $proxies = $proxies_returned['results'];
    }

    $response['recordsTotal'] = $proxies_total;
    $response['recordsFiltered'] = $proxies_total;
  }

  if (!is_array($proxies)) {
    drupal_json_output($response);
    exit();
  }

  $response['data'] = array();
  if (!empty($proxies)) {
    foreach ($proxies as $proxy) {
      $row = array();

      $row[0] = $proxy->id;
      $row[1] = $proxy->proxy_id;
      $row[2] = '<a href="' . $base_path . 'user/' . $proxy->proxy_id . $url_arguments . '" class="user-link">' . check_plain($proxy->proxy_name) . '</a>';
      $row[3] = check_plain($proxy->proxy_first_name);
      $row[4] = check_plain($proxy->proxy_last_name);
      $row[5] = $proxy->user_id;
      $row[6] = '<a href="' . $base_path . 'user/' . $proxy->user_id . $url_arguments . '" class="user-link">' . check_plain($proxy->name) . '</a>';
      $row[7] = check_plain($proxy->first_name);
      $row[8] = check_plain($proxy->last_name);

      $row[9] = '<a href="' . $base_path . 'requests/proxy-0/delete/' . $proxy->id . $url_arguments . '" class="operation-link operation-link-delete">Delete</a>';

      $response['data'][] = $row;
    }
  }

  drupal_json_output($response);
  exit();
}

/**
 * Ajax function call for loading request review list datatables.
 */
function mfcs_ajax_list_0() {
  mfcs_ajax_listing_0();
}

/**
 * Ajax function call for loading request review list datatables.
 */
function mfcs_ajax_review_0() {
  $options = array(
    'review_restrictions' => MFCS_REVIEW_RESTRICTION_REVIEW,
  );

  mfcs_ajax_listing_0(array(), $options);
}

/**
 * Ajax function call for loading request list datatables.
 *
 * @param array $custom_conditions
 *   (optional) An array containing additional conditions to use.
 * @param array $override_options
 *   (optional) An array containing the operations to override.
 */
function mfcs_ajax_listing_0($custom_conditions = array(), $override_options = array()) {
  global $base_path;
  global $mfcs_determined;

  if (!is_array($custom_conditions)) {
    cf_error::invalid_array('custom_conditions');
    $custom_conditions = array();
  }

  if (!is_array($override_options)) {
    cf_error::invalid_array('override_options');
    $override_options = array();
  }

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $response = array();

  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);


  // load ajax/json request settings.
  list($draw, $columns, $order, $options, $timestamp) = mfcs_ajax_setup_request_settings();

  foreach ($override_options as $override_option_name => $override_option_value) {
    $options[$override_option_name] = $override_option_value;
  }

  $search = FALSE;
  if (isset($_GET['search']['value']) && is_string($_GET['search']['value']) && !empty($_GET['search']['value'])) {
    $search = TRUE;

    // search through the already loaded lists, the database stores the values as integers and not strings.
    $regex_safe_search = preg_quote($_GET['search']['value'], '/');

    $type_options = mfcs_get_request_type_list_options();
    $status_options = mfcs_get_request_status_list_options();
    $step_options = mfcs_get_request_steps_list_options();

    $room_options = mfcs_get_request_room_list_options(NULL, NULL, TRUE);
    $building_options = mfcs_get_request_building_list_options(NULL, NULL, TRUE);

    $limit_type = array();
    foreach ($type_options as $key => $value) {
      if (preg_match('/' . $regex_safe_search . '/i', $value) > 0) {
        $limit_type[$key] = $key;
      }
    }

    $limit_status = array();
    foreach ($status_options as $key => $value) {
      if (preg_match('/' . $regex_safe_search . '/i', $value) > 0) {
        $limit_status[$key] = $key;
      }
    }

    $limit_step = array();
    foreach ($step_options as $key => $value) {
      if (preg_match('/' . $regex_safe_search . '/i', $value) > 0) {
        $limit_step[$key] = $key;
      }
    }

    $limit_room = array();
    foreach ($room_options as $key => $value) {
      if (preg_match('/' . $regex_safe_search . '/i', $value) > 0) {
        $limit_room[$key] = $key;
      }
    }

    $limit_building = array();
    foreach ($building_options as $key => $value) {
      if (preg_match('/' . $regex_safe_search . '/i', $value) > 0) {
        $limit_building[$key] = $key;
      }
    }
  }

  // build ajax/json response data.
  $response = array(
    'draw' => $draw,
  );

  $sorting = array();
  if (!isset($order[0]['column'])) {
    $sorting['id'] = 'DESC';
  }
  else {
    $order_column = (int) $order[0]['column'];
    $order_direction = isset($order[0]['dir']) && is_string($order[0]['dir']) ? strtoupper($order[0]['dir']) : 'DESC';

    if ($order_direction == 'ASC' || $order_direction == 'DESC') {
      if ($order_column == 0) {
        $sorting['id'] = $order_direction;
      }
      elseif ($order_column == 1) {
        $sorting['type'] = $order_direction;
      }
      elseif ($order_column == 2) {
        $sorting['title'] = $order_direction;
      }
      elseif ($order_column == 3) {
        $sorting['building'] = $order_direction;
      }
      elseif ($order_column == 4) {
        $sorting['room'] = $order_direction;
      }
      elseif ($order_column == 5) {
        $sorting['date'] = $order_direction;
      }
      elseif ($order_column == 6) {
        $sorting['status'] = $order_direction;
      }
      elseif ($order_column == 7) {
        $sorting['step'] = $order_direction;
      }
      elseif ($order_column == 8) {
        $sorting['submitted_by_id'] = $order_direction;
      }
      elseif ($order_column == 9) {
        $sorting['submitted_by_name'] = $order_direction;
      }
      elseif ($order_column == 10) {
        $sorting['created'] = $order_direction;
      }
    }
  }

  // always provide the unique id in the ordering to guarantee consistency in range results.
  if (!isset($sorting['id'])) {
    $sorting['id'] = 'DESC';
  }

  $options['sort'] = $sorting;

  $conditions = $custom_conditions;

  if ($search) {
    $count_conditions = $conditions;
    $count_options = $options;

    $conditions['or_1'] = array();

    $conditions['or_1']['value_id'] = array(
      'field' => 'text(mer.id)',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
      'expression' => TRUE,
    );

    $conditions['or_1']['value_title'] = array(
      'field' => 'mer.title',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    if (!empty($limit_building)) {
      $conditions['or_1']['value_building'] = array(
        'field' => 'mer.building',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $limit_building,
      );
    }

    if (!empty($limit_room)) {
      $conditions['or_1']['value_room'] = array(
        'field' => 'mer.room',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $limit_room,
      );
    }

    if (!empty($limit_type)) {
      $conditions['or_1']['value_type'] = array(
        'field' => 'mer.type',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $limit_type,
      );
    }

    // Commented out because this is confusing because the date format will not match.
    #$conditions['or_1']['value_date'] = array(
    #  'field' => 'text(to_timestamp(mer.created))',
    #  'operator' => MFCS_OPERATOR_LIKE,
    #  'search' => $_GET['search']['value'],
    #  'expression' => TRUE,
    #);

    if (!empty($limit_status)) {
      $conditions['or_1']['value_status'] = array(
        'field' => 'mer.status',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $limit_status,
      );
    }

    if (!empty($limit_step)) {
      $conditions['or_1']['value_step'] = array(
        'field' => 'mer.step',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $limit_step,
      );
    }

    $conditions['or_1']['value_submitted_by_id'] = array(
      'field' => 'text(mer.user_id)',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
      'expression' => TRUE,
    );

    $conditions['or_1']['value_submitted_by_name'] = array(
      'field' => 'sb_u.name',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    $count_options['returned'] = FALSE;
    $items_returned = mfcs_request_load_listing($count_conditions, $count_options);
    $items_total = 0;
    if (is_array($items_returned) && array_key_exists('count', $items_returned)) {
      $items_total = $items_returned['count'];
    }

    $items_returned = mfcs_request_load_listing($conditions, $options);
    $items = array();
    if (isset($items_returned['results']) && is_array($items_returned['results'])) {
      $items = $items_returned['results'];
    }

    $response['recordsTotal'] = $items_total;
    $response['recordsFiltered'] = $items_returned['count'];
  }
  else {
    $items_returned = mfcs_request_load_listing($conditions, $options);

    $items = array();
    if (isset($items_returned['results']) && is_array($items_returned['results'])) {
      $items = $items_returned['results'];
    }

    $response['recordsTotal'] = $items_returned['count'];
    $response['recordsFiltered'] = $items_returned['count'];
  }

  if (!is_array($items)) {
    drupal_json_output($response);
    exit();
  }

  $response['data'] = array();

  if (empty($items)) {
    drupal_json_output($response);
    exit();
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_LIST);
  $columns = mfcs_request_list_0_columns();

  $rows = mfcs_build_request_ajax_table_process_rows($items, $columns, $user, TRUE);

  if (is_array($rows)) {
    $response['data'] = $rows;
  }
  unset($rows);

  drupal_json_output($response);
  exit();
}

/**
 * Ajax function call for loading users problem list datatable.
 */
function mfcs_ajax_problems_0_users_list() {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $response = array();

  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  mfcs_include(MFCS_INCLUDE_MANAGEMENT);

  // load ajax/json request settings.
  list($draw, $columns, $order, $options, $timestamp) = mfcs_ajax_setup_request_settings();

  $search = FALSE;
  if (isset($_GET['search']['value']) && is_string($_GET['search']['value']) && !empty($_GET['search']['value'])) {
    $search = TRUE;
  }

  // build ajax/json response data.
  $response = array(
    'draw' => $draw,
  );

  $sorting = array();
  if (!isset($order[0]['column'])) {
    $sorting['user_id'] = 'ASC';
  }
  else {
    $order_column = (int) $order[0]['column'];
    $order_direction = isset($order[0]['dir']) && is_string($order[0]['dir']) ? strtoupper($order[0]['dir']) : 'DESC';

    if ($order_column == 0) {
      $sorting['user_id'] = $order_direction;
    }
    elseif ($order_column == 1) {
      $sorting['name'] = $order_direction;
    }
    elseif ($order_column == 2) {
      $sorting['first_name'] = $order_direction;
    }
    elseif ($order_column == 3) {
      $sorting['last_name'] = $order_direction;
    }
    elseif ($order_column == 4 || $order_column == 5 || $order_column == 6) {
      $sorting['problems'] = $order_direction;
    }
    elseif ($order_column == 7) {
      $sorting['date'] = $order_direction;
    }
  }

  // always provide the unique id in the ordering to guarantee consistency in range results.
  if (!isset($sorting['user_id'])) {
    $sorting['user_id'] = 'DESC';
  }

  if (!empty($sorting)) {
    $options['sort'] = $sorting;
  }

  $options['join'] = TRUE;
  $options['include_disabled'] = TRUE;

  $conditions = array(
    'u.status' => 1,
  );

  $conditions['and_0']['value_only_problems'] = array(
    'field' => 'mu.problems',
    'operator' => MFCS_OPERATOR_NOT_EQUAL,
    'search' => MFCS_USER_PROBLEM_NONE,
  );

  $taxonomy_problems = mfcs_management_get_problems_taxonomy();
  $problems_mask = 0;

  if ($search) {
    $count_conditions = $conditions;
    $count_options = $options;

    $conditions['or_1'] = array();

    $conditions['or_1']['value_user_id'] = array(
      'field' => 'text(mu.user_id)',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
      'expression' => TRUE,
    );

    $conditions['or_1']['value_name'] = array(
      'field' => 'u.name',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    $conditions['or_1']['value_field_user_first_name_value'] = array(
      'field' => 'fdfufn.field_user_first_name_value',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    $conditions['or_1']['value_field_user_last_name_value'] = array(
      'field' => 'fdfuln.field_user_last_name_value',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    $regex_safe_search = preg_quote($_GET['search']['value'], '/');
    foreach ($taxonomy_problems as $problem_id => $problem_label) {
      if (preg_match('/' . $regex_safe_search . '/i', $problem_label) > 0) {
        $problems_mask += $problem_id;
      }
    }
    unset($regex_safe_search);

    if ($problems_mask > 0) {
      $conditions['or_1']['value_problems'] = array(
        'field' => '(mu.problems & ' . $problems_mask . ')',
        'operator' => MFCS_OPERATOR_GREATER_THAN,
        'search' => 0,
        'expression' => TRUE,
      );
    }

    // Commented out because this is confusing because the date format will not match.
    #$conditions['or_1']['value_date'] = array(
    #  'field' => 'text(to_timestamp(mu.date))',
    #  'operator' => MFCS_OPERATOR_LIKE,
    #  'search' => $_GET['search']['value'],
    #  'expression' => TRUE,
    #);

    $count_options['returned'] = FALSE;
    $problems_returned = mfcs_management_search_users($count_conditions, $count_options);

    $problems_total = 0;
    if (is_array($problems_returned) && array_key_exists('count', $problems_returned)) {
      $problems_total = $problems_returned['count'];
    }

    $problems_returned = mfcs_management_search_users($conditions, $options);
    $problems = array();
    if (isset($problems_returned['results']) && is_array($problems_returned['results'])) {
      $problems = $problems_returned['results'];
    }

    $response['recordsTotal'] = $problems_total;
    $response['recordsFiltered'] = $problems_returned['count'];
  }
  else {
    $problems_returned = mfcs_management_search_users($conditions, $options);

    $problems_total = 0;
    if (is_array($problems_returned) && array_key_exists('count', $problems_returned)) {
      $problems_total = $problems_returned['count'];
    }

    $problems = array();
    if (isset($problems_returned['results']) && is_array($problems_returned['results'])) {
      $problems = $problems_returned['results'];
    }

    $response['recordsTotal'] = $problems_total;
    $response['recordsFiltered'] = $problems_total;
  }

  if (!is_array($problems)) {
    drupal_json_output($response);
    exit();
  }

  $response['data'] = array();
  if (!empty($problems)) {
    foreach ($problems as $problem) {
      $row = array();

      $additional_operations = '';

      $row[0] = '' . $problem->user_id;
      $row[1] = '<a href="' . $base_path . 'user/' . $problem->user_id . $url_arguments . '" class="user-link">' . check_plain($problem->name) . '</a>';
      $row[2] = check_plain($problem->first_name);
      $row[3] = check_plain($problem->last_name);

      $row[4] = NULL;
      foreach (array(MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE, MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE, MFCS_USER_PROBLEM_ACCOUNT_LOGIN_PROBLEMS_MULTIPLE, MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED, MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES, MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) as $taxonomy_id) {
        if (($problem->problems & $taxonomy_id) > 0) {
          if ($taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE || $taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE) {
            $additional_operations .= ' <a href="' . $base_path . 'requests/problems-0/users/resynchronize/' . $problem->user_id . $url_arguments . '" class="operation-link operation-link-resynchronize" title="Re-Synchronize the user\'s settings.">Re-Sync</a>';
          }

          if ($taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) {
            $additional_operations .= " <a href=\"" . $base_path . 'requests/problems-0/users/disabled_in_use/' . $problem->user_id . "\">Requests</a>";
          }

          if (is_null($row[4])) {
            $row[4] = '';
          }
          else {
            $row[4] .= ', ';
          }

          $row[4] .= $taxonomy_problems[$taxonomy_id];
        }
      }

      $row[5] = NULL;
      foreach (array(MFCS_USER_PROBLEM_FORM_ERROR_USER, MFCS_USER_PROBLEM_FORM_ERROR_USER) as $taxonomy_id) {
        if (($problem->problems & $taxonomy_id) > 0) {
          if (is_null($row[5])) {
            $row[5] = '';
          }
          else {
            $row[5] .= ', ';
          }

          $row[5] .= $taxonomy_problems[$taxonomy_id];
        }
      }

      $row[6] = NULL;
      foreach (array(MFCS_USER_PROBLEM_SYSTEM, MFCS_USER_PROBLEM_MFCS) as $taxonomy_id) {
        if (($problem->problems & $taxonomy_id) > 0) {
          if (is_null($row[6])) {
            $row[6] = '';
          }
          else {
            $row[6] .= ', ';
          }

          $row[6] .= $taxonomy_problems[$taxonomy_id];
        }
      }

      unset($taxonomy_id);

      $row[7] = check_plain(date("Y/m/d h:i:s a", $problem->date));
      $row[8] = '<a href="' . $base_path . 'requests/problems-0/users/user/' . $problem->user_id . $url_arguments . '" class="operation-link operation-link-view">View</a>' . $additional_operations;

      $response['data'][] = $row;
    }
  }

  drupal_json_output($response);
  exit();
}

/**
 * Ajax function call for loading users problem list datatable.
 */
function mfcs_ajax_email_log_0_list() {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $response = array();

  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  mfcs_include(MFCS_INCLUDE_MANAGEMENT);

  // load ajax/json request settings.
  list($draw, $columns, $order, $options, $timestamp) = mfcs_ajax_setup_request_settings();

  $search = FALSE;
  if (isset($_GET['search']['value']) && is_string($_GET['search']['value']) && !empty($_GET['search']['value'])) {
    $search = TRUE;
  }

  // build ajax/json response data.
  $response = array(
    'draw' => $draw,
  );

  $sorting = array();
  if (!isset($order[0]['column'])) {
    $sorting['user_id'] = 'ASC';
  }
  else {
    $order_column = (int) $order[0]['column'];
    $order_direction = isset($order[0]['dir']) && is_string($order[0]['dir']) ? strtoupper($order[0]['dir']) : 'DESC';

    if ($order_column == 0) {
      $sorting['id'] = $order_direction;
    }
    elseif ($order_column == 1) {
      $sorting['user_id'] = $order_direction;
    }
    elseif ($order_column == 2) {
      $sorting['name'] = $order_direction;
    }
    elseif ($order_column == 3) {
      $sorting['date'] = $order_direction;
    }
    elseif ($order_column == 4) {
      $sorting['destination'] = $order_direction;
    }
    elseif ($order_column == 5) {
      $sorting['title'] = $order_direction;
    }
    elseif ($order_column == 6) {
      $sorting['code'] = $order_direction;
    }
  }

  // always provide the unique id in the ordering to guarantee consistency in range results.
  if (!isset($sorting['id'])) {
    $sorting['id'] = 'DESC';
  }

  if (!empty($sorting)) {
    $options['sort'] = $sorting;
  }

  $options['fields'] = array(
    'id' => 'id',
    'destination' => 'destination',
    'title' => 'title',
    'user_id' => 'user_id',
    'date' => 'date',
    'code' => 'code',
  );

  $options['extra_fields']['users'] = array(
    'alias' => 'u',
    'join' => 'inner',
    'on' => 'mle.user_id = u.uid',
    'columns' => array(
      'user_id' => 'uid',
      'name' => 'name',
    ),
  );

  $options['extra_fields2']['users'] = array(
    'alias' => 'd_u',
    'join' => 'left',
    'on' => 'mle.destination = d_u.mail',
    'columns' => array(
      'destination_user_id' => 'uid',
    ),
  );

  $conditions = array();

  $table_name = 'mfcs_log_emails';
  $table_alias = 'mle';
  $primary_key = 'id';

  if ($search) {
    $regex_safe_search = preg_quote($_GET['search']['value'], '/');

    $count_conditions = $conditions;
    $count_options = $options;

    $conditions['or_1'] = array();

    $conditions['or_1']['value_id'] = array(
      'field' => 'text(' . $table_alias . '.id)',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
      'expression' => TRUE,
    );

    $conditions['or_1']['value_user_id'] = array(
      'field' => 'text(mle.user_id)',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
      'expression' => TRUE,
    );

    $conditions['or_1']['value_name'] = array(
      'field' => 'u.name',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    // Commented out because this is confusing because the date format will not match.
    #$conditions['or_1']['value_date'] = array(
    #  'field' => 'text(to_timestamp(' . $table_alias . '.date))',
    #  'operator' => MFCS_OPERATOR_LIKE,
    #  'search' => $_GET['search']['value'],
    #  'expression' => TRUE,
    #);

    $conditions['or_1']['value_destination'] = array(
      'field' => $table_alias . '.destination',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    $conditions['or_1']['value_title'] = array(
      'field' => $table_alias . '.title',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $_GET['search']['value'],
    );

    $limit_email_codes = NULL;
    if (preg_match('/' . $regex_safe_search . '/i', 'normal') > 0) {
      $limit_email_codes = MFCS_EMAIL_CODE_NORMAL;
    }
    elseif (preg_match('/' . $regex_safe_search . '/i', 're-send') > 0) {
      $limit_email_codes = MFCS_EMAIL_CODE_RESEND;
    }
    elseif (preg_match('/' . $regex_safe_search . '/i', 'forward') > 0) {
      $limit_email_codes = MFCS_EMAIL_CODE_FORWARD;
    }

    if (!is_null($limit_email_codes)) {
      $conditions['or_1']['value_code'] = array(
        'field' => $table_alias . '.code',
        'operator' => MFCS_OPERATOR_EQUAL,
        'search' => $limit_email_codes,
      );
    }

    $count_options['returned'] = FALSE;
    $email_logs_returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $count_conditions, $count_options);

    $email_logs_total = 0;
    if (is_array($email_logs_returned) && array_key_exists('count', $email_logs_returned)) {
      $email_logs_total = $email_logs_returned['count'];
    }

    $email_logs_returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions, $options);
    $email_logs = array();
    if (isset($email_logs_returned['results']) && is_array($email_logs_returned['results'])) {
      $email_logs = $email_logs_returned['results'];
    }

    $response['recordsTotal'] = $email_logs_total;
    $response['recordsFiltered'] = $email_logs_returned['count'];
  }
  else {
    $email_logs_returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions, $options);

    $email_logs_total = 0;
    if (is_array($email_logs_returned) && array_key_exists('count', $email_logs_returned)) {
      $email_logs_total = $email_logs_returned['count'];
    }

    $email_logs = array();
    if (isset($email_logs_returned['results']) && is_array($email_logs_returned['results'])) {
      $email_logs = $email_logs_returned['results'];
    }

    $response['recordsTotal'] = $email_logs_total;
    $response['recordsFiltered'] = $email_logs_total;
  }

  if (!is_array($email_logs)) {
    drupal_json_output($response);
    exit();
  }

  $may_resend_log = mfcs_page_request_access('email_log-resend');

  $response['data'] = array();
  if (!empty($email_logs)) {
    foreach ($email_logs as $email_log) {
      $row = array();

      $row[0] = '' . $email_log->id;
      $row[1] = '' . $email_log->user_id;
      $row[2] = '<a href="' . $base_path . 'user/' . $email_log->user_id . $url_arguments . '" class="user-link">' . check_plain($email_log->name) . '</a>';
      $row[3] = check_plain(date("Y/m/d h:i:s a", $email_log->date));

      if (empty($email_log->user_id)) {
        $row[4] = check_plain($email_log->destination);
      }
      else {
        $row[4] = '<a href="' . $base_path . 'user/' . $email_log->user_id . $url_arguments . '" class="user-link">' . check_plain($email_log->destination) . '</a>';
      }

      $row[5] = check_plain($email_log->title);

      $row[6] = "Normal";
      if ($email_log->code == MFCS_EMAIL_CODE_RESEND) {
        $row[6] = "Re-sent";
      }
      elseif ($email_log->code == MFCS_EMAIL_CODE_FORWARD) {
        $row[6] = "Forwarded";
      }

      $row[7] = '<a href="' . $base_path . 'requests/email_log-0/view/' . $email_log->id . $url_arguments . '" class="operation-link operation-link-view">View</a>';

      if ($may_resend_log) {
        $row[7] .= ' <a href="' . $base_path . 'requests/email_log-0/resend/' . $email_log->id . $url_arguments . '" class="operation-link operation-link-resend">Re-send</a>';
      }

      $response['data'][] = $row;
    }
  }

  drupal_json_output($response);
  exit();
}

/**
 * Ajax function call for loading request review list datatables.
 */
function mfcs_ajax_problems_0_disabled_in_use($user_id) {
  if (!cf_is_integer($user_id)) {
    drupal_json_output(array());
    exit();
  }

  $conditions = array();
  $conditions['value_status'] = array(
    'field' => 'mer.status',
    'operator' => MFCS_OPERATOR_IS_IN,
    'search' => array(MFCS_REQUEST_STATUS_LOCKED, MFCS_REQUEST_STATUS_UNLOCKED),
  );

  $conditions['value_venue_coordinator'] = array(
    'field' => 'mer.venue_coordinator',
    'operator' => MFCS_OPERATOR_EQUAL,
    'search' => $user_id,
  );

  $conditions['value_problem_venue_coordinator_status'] = array(
    'field' => 'u.status',
    'operator' => MFCS_OPERATOR_EQUAL,
    'search' => 0,
  );


  $options = array();
  $options['extra_fields']['users'] = array(
    'alias' => 'u',
    'join' => 'inner',
    'on' => 'mer.venue_coordinator = u.uid',
    'columns' => array(
      'venue_coordinator_status' => 'status',
    ),
  );

  mfcs_ajax_listing_0($conditions, $options);
}

/**
 * @} End of '@addtogroup mfcs'.
 */
