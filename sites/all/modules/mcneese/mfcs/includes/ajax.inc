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
 * @param bool $post_method
 *   When TRUE, the setup script expects and utilizes HTTP Post instead of HTTP Get.
 *
 * @return array
 *   An array of variables in the following order:
 *   - draw: draw setting.
 *   - columns: table columns settings.
 *   - order: table order settings.
 *   - options: table options.
 *   - timestamp: timestamp variable.
 *   - post_method: the post method used.
 */
function mfcs_ajax_setup_request_settings($post_method = TRUE) {
  if (!is_bool($post_method)) {
    cf_error::invalid_bool('post_method');

    // force post method on invalid variable instead of returning.
    $post_method = TRUE;
  }

  $draw = 1;
  $columns = NULL;
  $order = NULL;

  if ($post_method) {
    if (isset($_POST['draw'])) {
      $draw += (int) $_POST['draw'];
    }

    if (isset($_POST['columns']) && is_array($_POST['columns'])) {
      $columns = $_POST['columns'];
    }

    if (isset($_POST['order']) && is_array($_POST['order'])) {
      $order = $_POST['order'];
    }
  }
  else {
    if (isset($_GET['draw'])) {
      $draw += (int) $_GET['draw'];
    }

    if (isset($_GET['columns']) && is_array($_GET['columns'])) {
      $columns = $_GET['columns'];
    }

    if (isset($_GET['order']) && is_array($_GET['order'])) {
      $order = $_GET['order'];
    }
  }

  $options = array(
    'join' => TRUE,
    'start' => 0,
    'limit' => NULL,
    'count' => TRUE,
    'rowId' => '0',
  );

  $timestamp = mfcs_instance();

  if ($post_method) {
    if (isset($_POST['start']) && is_numeric($_POST['start'])) {
      $options['start'] = (int) $_POST['start'];
    }

    if (isset($_POST['length']) && is_numeric($_POST['length'])) {
      $options['limit'] = (int) $_POST['length'];
    }

    if (isset($_POST['_']) && is_array($_POST['_'])) {
      $timestamp = $_POST['_'];
    }
  }
  else {
    if (isset($_GET['start']) && is_numeric($_GET['start'])) {
      $options['start'] = (int) $_GET['start'];
    }

    if (isset($_GET['length']) && is_numeric($_GET['length'])) {
      $options['limit'] = (int) $_GET['length'];
    }

    if (isset($_GET['_']) && is_array($_GET['_'])) {
      $timestamp = $_GET['_'];
    }
  }

  // A length of -1 means all values, set limit to 0 which means all values.
  if ($options['limit'] < 0) {
    $options['limit'] = 0;
  }

  return array($draw, $columns, $order, $options, $timestamp, $post_method);
}

/**
 * Ajax function call for loading request list datatables.
 *
 * @param string|int $argument_1
 *   Some string or number to send as the first argument.
 */
function mfcs_ajax_list_0($argument_1 = NULL) {
  $user = cf_current_user();

  $manager = FALSE;
  $reviewer = FALSE;
  $requester = FALSE;
  if ($user->uid === MFCS_USER_REQUEST) {
    // this is a simulated user account and must be handled as a special case.
    $requester = TRUE;
  }
  else {
    $manager = user_access('mfcs manage', $user);
    $reviewer = user_access('mfcs review', $user);
    $requester = user_access('mfcs request', $user);
  }

  $show_my_request_only_default = FALSE;
  if ($requester && !($manager || $reviewer)) {
    $show_my_request_only_default = TRUE;
  }

  $options = array();
  if ($argument_1 == 'my_requests') {
    $options['only_requester'] = TRUE;
  }
  elseif ($argument_1 == 'all_requests') {
    $options['only_requester'] = FALSE;
  }
  else {
    $options['only_requester'] = $show_my_request_only_default;
  }

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
  list($draw, $columns, $order, $options, $timestamp, $post_method) = mfcs_ajax_setup_request_settings();

  if ($post_method) {
    $get_post = &$_POST;
  }
  else {
    $get_post = &$_GET;
  }

  foreach ($override_options as $override_option_name => $override_option_value) {
    $options[$override_option_name] = $override_option_value;
  }

  $search = FALSE;
  $user_ids = array();
  if (isset($get_post['search']['value']) && is_string($get_post['search']['value']) && !empty($get_post['search']['value'])) {
    $search = TRUE;

    // search through the already loaded lists, the database stores the values as integers and not strings.
    $regex_safe_search = preg_quote($get_post['search']['value'], '/');

    $type_options = mfcs_get_request_type_list_options();
    $status_options = mfcs_get_request_status_list_options();
    $step_options = mfcs_get_review_steps_list_options();

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

    // find a list of user ids to search for to avoid additional inner joins on an already complex SQL query..
    $found_users = mfcs_search_users($get_post['search']['value']);
    if (is_array($found_users)) {
      $user_ids = array_keys($found_users);
      unset($found_users);
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
    $order_direction = isset($order[0]['dir']) && is_string($order[0]['dir']) ? strtolower($order[0]['dir']) : 'desc';

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
      'search' => $get_post['search']['value'],
      'expression' => TRUE,
    );

    $conditions['or_1']['value_title'] = array(
      'field' => 'mer.title',
      'operator' => MFCS_OPERATOR_LIKE,
      'search' => $get_post['search']['value'],
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
    #  'search' => $get_post['search']['value'],
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
      'search' => $get_post['search']['value'],
      'expression' => TRUE,
    );

    if (!empty($user_ids)) {
      $conditions['or_1']['value_user_ids'] = array(
        'field' => 'mer.user_id',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $user_ids,
      );
    }

    $count_options['results'] = FALSE;
    $items_returned = mfcs_request_load_listing($count_conditions, $count_options);
    $items_total = 0;
    if (is_array($items_returned) && array_key_exists('count', $items_returned)) {
      $items_total = $items_returned['count'];
    }

    // count cannot be disabled here, otherewise the recordsFiltered results would be undefined or inaccurate when an SQL limit is applied.
    #$options['count'] = FALSE;
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
 * Ajax function call for loading request review list datatables.
 *
 * @param string|int $argument_1
 *   Some string or number to send as the first argument.
 */
function mfcs_ajax_review_0($argument_1 = NULL) {
  $user = cf_current_user();

  $manager = FALSE;
  if ($user->uid === MFCS_USER_REQUEST) {
    // this is a simulated user account and must be handled as a special case.
  }
  else {
    $manager = user_access('mfcs manage', $user);
  }

  $options = array(
    'review_restrictions' => MFCS_REVIEW_RESTRICTION_REVIEW,
  );

  if ($manager) {
    $options['only_reviewer'] = TRUE;
    $options['table_caption'] = "Requests (As Reviewer)";

    if ($argument_1 == 'as_reviewer') {
      $options['only_reviewer'] = TRUE;
      $options['table_caption'] = "Requests (As Reviewer)";
    }
    elseif ($argument_1 == 'as_manager') {
      $options['only_reviewer'] = FALSE;
      $options['table_caption'] = "Requests (As Manager)";
    }
  }

  mfcs_ajax_listing_0(array(), $options);
}

/**
 * @} End of '@addtogroup mfcs'.
 */
