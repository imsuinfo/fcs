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

  $timestamp = REQUEST_TIME;
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
    if (array_key_exists('count', $reviewers_returned)) {
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
    if (array_key_exists('count', $proxies_returned)) {
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
    $proxies = array();
    if (isset($proxies_returned['results']) && is_array($proxies_returned['results'])) {
      $proxies = $proxies_returned['results'];
    }

    $response['recordsTotal'] = $proxies_returned['count'];
    $response['recordsFiltered'] = $proxies_returned['count'];
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

  mfcs_ajax_listing_0($options);
}

/**
 * Ajax function call for loading request list datatables.
 *
 * @param array $override_options
 *   (optional) An array containing the operations to override.
 */
function mfcs_ajax_listing_0($override_options = array()) {
  global $base_path;
  global $mfcs_determined;

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

    $status_options = mfcs_get_request_status_list_options();
    $step_options = mfcs_get_request_steps_list_options();

    $room_options = mfcs_get_request_room_list_options(NULL, NULL, TRUE);
    $building_options = mfcs_get_request_building_list_options(NULL, NULL, TRUE);

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
        $sorting['title'] = $order_direction;
      }
      elseif ($order_column == 2) {
        $sorting['building'] = $order_direction;
      }
      elseif ($order_column == 3) {
        $sorting['room'] = $order_direction;
      }
      elseif ($order_column == 4) {
        $sorting['date'] = $order_direction;
      }
      elseif ($order_column == 5) {
        $sorting['status'] = $order_direction;
      }
      elseif ($order_column == 6) {
        $sorting['step'] = $order_direction;
      }
      elseif ($order_column == 7) {
        $sorting['submitted_by_id'] = $order_direction;
      }
      elseif ($order_column == 8) {
        $sorting['submitted_by_name'] = $order_direction;
      }
      elseif ($order_column == 9) {
        $sorting['created'] = $order_direction;
      }
    }
  }

  // always provide the unique id in the ordering to guarantee consistency in range results.
  if (!isset($sorting['id'])) {
    $sorting['id'] = 'DESC';
  }

  $options['sort'] = $sorting;

  $conditions = array();

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

    if (!empty($limit_status)) {
      $conditions['or_1']['value_status'] = array(
        'field' => 'mer.status',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $limit_status,
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
    if (array_key_exists('count', $items_returned)) {
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
 * @} End of '@addtogroup mfcs'.
 */
