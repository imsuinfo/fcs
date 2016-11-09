<?php

/**
 * @file
 * Defines McNeese facilities use request problems page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request manage problems page.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_management_problems_page() {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  mfcs_add_canonical_headers('requests/problems-0');

  $items_1 = array();
  $items_2 = array();

  $items_array = &$items_1;

  $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/problems-0/users' . $url_arguments . '" class="item-link">User Problems</a></h4>';
  $help = '<div class="item-description inline-block">Access a list of user accounts in which problems have been detected.</div>';
  $items_array[] = array(
    'data' => $link . '<br>' . $help,
    'class' => array(
    ),
  );

  $items_array = &$items_2;

  $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/problems-0/requests' . $url_arguments . '" class="item-link">Request Problems</a></h4>';
  $help = '<div class="item-description inline-block">Access a list of requests in which problems have been detected.</div>';
  $items_array[] = array(
    'data' => $link . '<br>' . $help,
    'class' => array(
    ),
  );

  $c_1 = count($items_1);
  $c_2 = count($items_2);
  if ($c_1 == $c_2) {
    $items_array = &$items_1;
  }
  elseif ($c_1 > $c_2) {
    $items_array = &$items_2;
  }
  else {
    $items_array = &$items_1;
  }

  $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/troubleshoot-0' . $url_arguments . '" class="item-link">Troubleshooting Tools</a></h4>';
  $help = '<div class="item-description inline-block">Utilize troubleshooting tools to help fix problems on the system.</div>';
  $items_array[] = array(
    'data' => $link . '<br>' . $help,
    'class' => array(
    ),
  );

  $markup = '<div id="mfcs-management_page-choices" class="mfcs-management_page-choices">';

  if (!empty($items_1)) {
    $item_list_1 = theme('item_list', array('items' => $items_1, 'type' => 'ul'));
    $markup .= '<div class="mfcs-management_page-choices-list_1">' . $item_list_1 . '</div>';
  }

  if (!empty($items_2)) {
    $item_list_2 = theme('item_list', array('items' => $items_2, 'type' => 'ul'));
    $markup .= '<div class="mfcs-management_page-choices-list_2">' . $item_list_2 . '</div>';
  }

  $markup .= '</div>';

  return $markup;
}

/**
 * View and manage problems associated with a specific user.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 */
function mfcs_request_problems_0_users_form($form, &$form_state, $page_number = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $no_js_fallback = TRUE;
  if (isset($conf['no_js_fallback']) && is_bool($conf['no_js_fallback'])) {
    $no_js_fallback = $conf['no_js_fallback'];
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/problems-0/users/' . $page_number);

  $page_title = "Facilities Use Problems - Users";
  drupal_set_title($page_title);


  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
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

  $class_user_id = 'no-script-hidden script-hidden ';
  $class_name = '';
  $class_first_name = '';
  $class_last_name = '';
  $class_problems_account = '';
  $class_problems_forms = '';
  $class_problems_other = '';
  $class_date = '';
  $class_operations = 'no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['user_id'] = 'ASC';
    $order_column = array(0 => strtolower($sorting['user_id']));
    $class_user_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'account id') {
      $sorting['user_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_user_id .= 'sorting_1 ';
    }
    elseif ($order == 'account') {
      $sorting['name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_name .= 'sorting_1 ';
    }
    elseif ($order == 'first name') {
      $sorting['first_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_first_name .= 'sorting_1 ';
    }
    elseif ($order == 'last name') {
      $sorting['last_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_last_name .= 'sorting_1 ';
    }
    elseif ($order == 'problems account') {
      $sorting['problems_account'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_problems_account .= 'sorting_1 ';
    }
    elseif ($order == 'problems forms') {
      $sorting['problems_forms'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_problems_forms .= 'sorting_1 ';
    }
    elseif ($order == 'problems other') {
      $sorting['problems_other'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(6 => strtolower($sorting[$order]));
      $class_problems_other .= 'sorting_1 ';
    }
    elseif ($order == 'updated_problems') {
      $sorting['updated_problems'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(6 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    else {
      $sorting['user_id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['user_id']));
      $class_user_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 40;
  $options = array(
    'sort' => $sorting,
    'join' => TRUE,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
    'include_disabled' => TRUE,
  );

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $options['results'] = FALSE;
  }

  $conditions = array();
  $conditions['and_0']['value_only_problems'] = array(
    'field' => 'mu.problems',
    'operator' => MFCS_OPERATOR_NOT_EQUAL,
    'search' => MFCS_USER_PROBLEM_NONE,
  );

  $problems_returned = mfcs_management_search_users($conditions, $options);

  $problems_total = 0;
  if (is_array($problems_returned) && array_key_exists('count', $problems_returned)) {
    $problems_total = $problems_returned['count'];
  }

  $problems = array();
  if (isset($problems_returned['results']) && is_array($problems_returned['results'])) {
    $problems = $problems_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($problems_total > 0 && $options['start'] >= $problems_total || $problems_total == 0 && $options['start'] > $problems_total) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // do not show count values when no fallback is available.
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $count = 0;
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $taxonomy_problems = mfcs_management_get_problems_user_taxonomy();
  $problems_mask = 0;

  $rows = array();
  foreach ($problems as $user_id => $problem) {
    $row = array();
    $additional_operations = '';

    $problem_account = NULL;
    foreach (array(MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE, MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE, MFCS_USER_PROBLEM_ACCOUNT_LOGIN_PROBLEMS_MULTIPLE, MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED, MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES, MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE, MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID, MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS) as $taxonomy_id) {
      if (($problem->problems & $taxonomy_id) > 0) {
        if ($taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE || $taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE || $taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS) {
          $additional_operations .= ' <a href="' . $base_path . 'requests/problems-0/users/resynchronize/' . $problem->user_id . $url_arguments . '" class="operation-link operation-link-resynchronize" title="Re-Synchronize the user\'s settings.">Re-Sync</a>';
        }

        if ($taxonomy_id == MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) {
          $additional_operations .= ' <a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $problem->user_id . '">Requests</a>';
        }

        if (is_null($problem_account)) {
          $problem_account = '';
        }
        else {
          $problem_account .= ', ';
        }

        $problem_account .= $taxonomy_problems[$taxonomy_id];
      }
    }

    $problem_forms = NULL;
    foreach (array(MFCS_USER_PROBLEM_FORM_ERROR_USER, MFCS_USER_PROBLEM_FORM_ERROR_USER) as $taxonomy_id) {
      if (($problem->problems & $taxonomy_id) > 0) {
        if (is_null($problem_forms)) {
          $problem_forms = '';
        }
        else {
          $problem_forms .= ', ';
        }

        $problem_forms .= $taxonomy_problems[$taxonomy_id];
      }
    }

    $problem_other = NULL;
    foreach (array(MFCS_USER_PROBLEM_SYSTEM, MFCS_USER_PROBLEM_MFCS) as $taxonomy_id) {
      if (($problem->problems & $taxonomy_id) > 0) {
        if (is_null($problem_other)) {
          $problem_other = '';
        }
        else {
          $problem_other .= ', ';
        }

        $problem_other .= $taxonomy_problems[$taxonomy_id];
      }
    }

    $row['account_id'] = array(
      'data' => $problem->user_id,
      'class' => $class_user_id,
    );

    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'requests/users-0/view/' . $problem->user_id . $url_arguments . '" class="user-link">' . check_plain($problem->name) . '</a>' . ' <span class="user-id script-hidden">[id: ' .  $problem->user_id . ']</span>',
      'class' => $class_name,
    );

    $row['first_name'] = array(
      'data' => check_plain($problem->first_name),
      'class' => $class_first_name,
    );

    $row['last_name'] = array(
      'data' => check_plain($problem->last_name),
      'class' => $class_last_name,
    );

    $row['problems_account'] = array(
      'data' => $problem_account,
      'class' => $class_problems_account,
    );

    $row['problems_forms'] = array(
      'data' => $problem_forms,
      'class' => $class_problems_forms,
    );

    $row['problems_other'] = array(
      'data' => $problem_other,
      'class' => $class_problems_other,
    );

    $row['updated_problems'] = array(
      'data' => check_plain(date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $problem->updated_problems)),
      'class' => $class_date,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/problems-0/users/user/' . $problem->user_id . $url_arguments . '" class="operation-link operation-link-view">View</a>' . $additional_operations,
      'class' => $class_operations,
    );

    $rows[] = $row;
  }

  $problem_list = array();
  $problem_list['caption'] = t("User Problems");
  $problem_list['header'] = array(
    'account_id' => array('field' => 'user_id', 'data' => "Account ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'account' => array('field' => 'name', 'data' => "Account", 'align' => 'center'),
    'first_name' => array('field' => 'first_name', 'data' => "First Name", 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => "Last Name", 'align' => 'center'),
    'problems_account' => array('field' => 'problems', 'data' => "Problems Account", 'align' => 'center'),
    'problems_form' => array('field' => 'problems', 'data' => "Problems Forms", 'align' => 'center'),
    'problems_other' => array('field' => 'problems', 'data' => "Problems Other", 'align' => 'center'),
    'updated_problems' => array('field' => 'updated_problems', 'data' => "Date", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $problem_list['attributes'] = array();
  $problem_list['attributes']['id'] = 'mfcs-problem_form';
  $problem_list['attributes']['class'] = array(
    'no_default',
    'mfcs-problem_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $problem_list['attributes']['summary'] = 'Collection of users in which the system has detected problems associated with.';

  $problem_list['rows'] = $rows;
  $problem_list['empty'] = t("There are no users with system detected problems.");

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
      'url' => "'" . $base_path . 'requests/problems-0/users/ajax' . $url_arguments . "'",
      'type' => "'POST'",
    ),
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'account_id', 'name' => "Account ID", 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 1, 'column' => 'account', 'name' => "Account", 'className' => 'column-account'),
      array('id' => 2, 'column' => 'first_name', 'name' => "First Name", 'className' => 'column-first_name'),
      array('id' => 3, 'column' => 'last_name', 'name' => "Last Name", 'className' => 'column-last_name'),
      array('id' => 4, 'column' => 'problem_account', 'name' => "Problems Account", 'className' => 'column-problem_account'),
      array('id' => 5, 'column' => 'problem_forms', 'name' => "Problems Forms", 'className' => 'column-problem_forms'),
      array('id' => 6, 'column' => 'problem_other', 'name' => "Problems Other", 'className' => 'column-problem_other'),
      array('id' => 7, 'column' => 'updated_problems', 'name' => "Date", 'className' => 'column-updated_problems'),
      array('id' => 8, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'rowId' => '0',
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $problem_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $problem_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($problems_total == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $problems_total) {
    $navigation_total = $problems_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $problems_total . " entries.";
  $navigation_bottom .= '</div>';

  if ($problems_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['problem_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['problem_list']['#markup'] .= mcneese_libraries_theme_table($problem_list, $datatable_settings);
  #$form['problem_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  return $form;
}

/**
 * Validation for mfcs_request_problems_0_users_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_users_form()
 */
function mfcs_request_problems_0_users_form_validate($form, &$form_state) {
  form_set_error('', "Currently this form has nothing to submit.");
}

/**
 * Submit for mfcs_request_problems_0_users_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_users_form()
 */
function mfcs_request_problems_0_users_form_submit($form, &$form_state) {
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

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/problems-0/users');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * View and manage problems associated with a specific user.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 */
function mfcs_request_problems_0_user_form($form, &$form_state, $problem_user_id = NULL) {
  if (!cf_is_integer($problem_user_id) || $problem_user_id < 0) {
    drupal_not_found();
    drupal_exit();
  }

  $problem_user = user_load($problem_user_id);
  if (!is_object($problem_user)) {
    drupal_not_found();
    drupal_exit();
  }

  if ($problem_user->mfcs['problems'] == MFCS_USER_PROBLEM_NONE) {
    drupal_not_found();
    drupal_exit();
  }

  global $user;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/problems-0/users/user/' . $problem_user_id);

  $user_name = $problem_user->name;
  if (!empty($problem_user->mfcs['first_name']) && !empty($problem_user->mfcs['last_name'])) {
    $user_name = check_plain($problem_user->mfcs['first_name']) . ' ' . check_plain($problem_user->mfcs['last_name']);
  }

  $banner_id = NULL;
  if (!empty($problem_user->mfcs['banner_id']) && is_numeric($problem_user->mfcs['banner_id'])) {
    $banner_id = (int) $problem_user->mfcs['banner_id'];
  }

  $page_title = 'Facilities Use Problems: ' . $user_name . ' [id: ' . $problem_user_id . ']';
  drupal_set_title($page_title);

  $user_is_manager = user_access('mfcs manage', $user);
  $user_is_administer = user_access('mfcs administer', $user);


  $form = array();
  $form['values'] = array(
    '#tree' => TRUE,
  );

  $form['values']['mfcs_user'] = array(
    '#type' => 'value',
    '#value' => $problem_user->mfcs,
  );

  $form['values']['problem_user'] = array(
    '#type' => 'value',
    '#value' => $problem_user,
  );

  $form['values']['problem_list'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $message = 'The system has detected problems for <strong>' . $user_name . '</strong>.<br>';
  $message .= 'This page provides details of the problems and the ability to reset the problem flags for each of these problems.<br>';
  $message .= 'Some of these problems can be reset, others cannot.<br>';
  $message .= '<br>';
  $message .= 'The <a href="' . $base_path . 'requests/users-0/view/' . $problem_user_id . '" title="View Profile for ' . $user_name . ' (id #' . $problem_user_id . ')">user\'s information</a> and problem flags were last updated on: <strong>' . date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $problem_user->mfcs['updated_problems']) . '</strong>.<br>';
  $message .= '<br>';

  $form['message'] = array(
    '#markup' => '<div class="problem-message">' . $message . '</div>',
  );

  $form['banner_information'] = array(
    '#markup' => "Banner Information",
    '#id' => 'banner_information-fieldset',
    '#type' => 'fieldset',
    '#title' => "Banner Information",
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'banner_information-fieldset',
      ),
    ),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['banner_information']['details'] = array(
    '#markup' => '',
  );

  if (!is_null($banner_id)) {
    $banner_id_string = mfcs_banner_id_int_to_string($banner_id);
    $form['banner_information']['details']['#markup'] .= '<strong>Banner ID</strong>: ' . $banner_id_string . '<br>';

    try {
      $event_roles = mfcs_oracle_select('select * from ucs.msu_event_role where id = :banner_id', array('banner_id' => $banner_id_string));

      if (is_array($event_roles)) {
        $form['banner_information']['details']['#markup'] .= '<strong>Banner Roles</strong>:';
        $form['banner_information']['details']['#markup'] .= '<ul>';

        if (!empty($event_roles)) {
          foreach ($event_roles as $event_role) {
            $form['banner_information']['details']['#markup'] .= '<li>' . $event_role['role'] . '</li>';
          }
        }

        $form['banner_information']['details']['#markup'] .= '</ul>';
      }
      else {
        $form['banner_information']['details']['#markup'] .= 'Banner information is currently unavailable.';
      }
    }
    catch (Error $e) {
      $form['banner_information']['details']['#markup'] .= 'Banner information is currently unavailable.';
    }
    catch (Exception $e) {
      $form['banner_information']['details']['#markup'] .= 'Banner information is currently unavailable.';
    }
  }
  else {
    $form['banner_information']['details']['#markup'] .= 'User has no banner id.';
  }

  $form['problems'] = array(
    '#tree' => TRUE,
  );

  $do_something = FALSE;

  $form['values']['problem_list']['#value'] = array(
    MFCS_USER_PROBLEM_SYSTEM => array(
      'label' => 'Generic System Related Problems',
      'explanation' =>
        'This flag gets set when a generic system error is detected while the specified user was accessing this system.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_MFCS => array(
      'label' => 'Generic Facilities Use Problems',
      'explanation' =>
        'This flag gets set when an error is detected for the facilities use specific functionality while the specified user was accessing this system.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE => array(
      'label' => 'Banner Unavailable During Account Creation',
      'explanation' =>
        'The banner system was unavailable during account creation (first login) and the appropriate access controls could not be auto-assigned.<br>' .
        'The next time the user logs in this flag will be cleared and the account will load the appropriate banner roles.<br>' .
        'The user must log out before the banner information can be updated.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => FALSE,
      'custom_action' => TRUE,
      'custom_action-message' => 'Re-synchronize banner information for account.',
    ),
    MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE => array(
      'label' => 'Banner Unavailable During Login Synchronizeation',
      'explanation' =>
        'The banner system was unavailable during the login process and the account could not be updated if necessary.<br>' .
        'This flag will be reset the next time the user logs in and banner is available.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => FALSE,
      'custom_action' => TRUE,
      'custom_action-message' => 'Re-synchronize banner information for account.',
    ),
    MFCS_USER_PROBLEM_ACCOUNT_LOGIN_PROBLEMS_MULTIPLE => array(
      'label' => 'Multiple Login Problems',
      'explanation' =>
        'This flag gets set when a user has multiple problems logging in or multiple failed attempts logging in.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED => array(
      'label' => 'Reviewer/Venue-Coordinator Role Not Allowed',
      'explanation' =>
        'This flag gets set when an attempt is made to assign or unassign the reviewer role to a user when there is no reference to that user as a venue coordinator or a room coordinator.<br>' .
        '<br>' .
        'This may be caused by the banner database cache being out of date.<br>' .
        'You may need to re-synchronizing the banner cache.<br>' .
        '<br>' .
        'If the user is not in banner, then ask a administrator to manually grant the review role for this user.<br>' .
        '<br>' .
        'Administrators, managers, and users without banner ids should not get this error.<br>',
      'resettable' => TRUE,
      'custom_action' => TRUE,
      'custom_action-message' => 'Re-process Reviewer/Venue-Coordinator Role.',
    ),
    MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES => array(
      'label' => 'Old/Duplicate Account',
      'explanation' =>
        'This flag gets set when a user has logged in and the <strong>LDAP</strong> has the <strong>isDuplicate</strong> flag set to <strong>TRUE</strong> and there is no primary account (or this is not the primary account).<br>' .
        '<br>' .
        'A specific account must be manually assigned by an administer to designate which account is the primary account.<br>',
        'Only the primary account will be allowed access to this system.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_FORM_ERROR_USER => array(
      'label' => 'Facilities Use Form Problems',
      'explanation' =>
        'This flag gets set when an error is detected while the users is processing a form.<br>' .
        'This could be something as simple as not adding in the date fields or something technical such as a PHP error.<br>' .
        'This flag refers to facilities use specific functionality.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_FORM_ERROR_SYSTEM => array(
      'label' => 'System Form Problems',
      'explanation' =>
        'This flag gets set when an error is detected while the users is processing a form.<br>' .
        'This flag refers to the drupal core form functionality and not the facilities use specific functionality.<br>' .
        '<br>' .
        'This system currently does not provide the details of these problems, but in the future it may.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE => array(
      'label' => 'Deleted/Disabled User Assigned Requests',
      'explanation' =>
        'This flag gets set when a deleted or disabled user account is assigned as a venue coordinator or similar access roles.<br>' .
        '<br>' .
        'This is generally casued when an account is deleted but <a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $problem_user_id . '">active or on-going requests have not been re-assigned</a>.<br>' .
        '<br>' .
        'Requests will need to be re-assigned.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID => array(
      'label' => 'Invalid Banner ID Assigned to User',
      'explanation' =>
        'This flag gets set when a user has a banner id that is unknown to the system.<br>' .
        '<br>' .
        'The user either needs to have their banner id fixed, removed, or the account should be locked.<br>',
      'resettable' => FALSE,
      'custom_action' => TRUE,
      'custom_action-message' => 'Lock the user account.',
    ),
    MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS => array(
      'label' => 'Insufficient Banner Access',
      'explanation' =>
        'This flag gets set when a user logs into the system but has not been granted any banner roles to perform any actions, but the account is still technically enabled/unlocked.<br>' .
        '<br>' .
        'Either the account needs to be manually disabled/locked or the account needs to have its banner roles fixed.<br>',
        '<br>' .
        'This may also be triggered if a new user tries to login while banner is down and there is no locally cached data associated with the new account to properly assign its banner roles.<br>',
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
  );

  // if the user does not have a banner id, then do not provide banner sync custom action support.
  if (is_null($banner_id)) {
    $form['values']['problem_list']['#value'][MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE]['custom_action'] = FALSE;
    $form['values']['problem_list']['#value'][MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE]['custom_action'] = FALSE;
  }

  foreach ($form['values']['problem_list']['#value'] as $problem_id => $problem_settings) {
    if (($problem_user->mfcs['problems'] & $problem_id) > 0) {
      $form['problems'][$problem_id] = array(
        '#id' => 'problem-fieldset-' . $problem_id,
        '#type' => 'fieldset',
        '#title' => $problem_settings['label'] . ' Detected',
        '#tree' => TRUE,
        '#attributes' => array(
          'class' => array(
            'problem-fieldset',
            'problem-fieldset-' . $problem_id,
          ),
        ),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      if ($problem_settings['resettable']) {
        $do_something = TRUE;

        $form['problems'][$problem_id]['reset'] = array(
          '#id' => 'problem-field-reset-' . $problem_id,
          '#type' => 'checkbox',
          '#title' => 'Reset ' . $problem_settings['label'] . ' Flag',
          '#default_value' => FALSE,
          '#required' => FALSE,
          '#attributes' => array(
            'class' => array(
              'problem-field-reset',
              'problem-field-reset-' . $problem_id,
            ),
          ),
        );
      }

      if ($problem_settings['custom_action'] && !empty($problem_settings['custom_action-message'])) {
        $do_something = TRUE;

        $form['problems'][$problem_id]['custom_action'] = array(
          '#id' => 'problem-field-custom_action-' . $problem_id,
          '#type' => 'checkbox',
          '#title' => $problem_settings['custom_action-message'],
          '#default_value' => FALSE,
          '#required' => FALSE,
          '#attributes' => array(
            'class' => array(
              'problem-field-custom_action',
              'problem-field-custom_action-' . $problem_id,
            ),
          ),
        );

        if (!empty($problem_settings['custom_action-description'])) {
          $form['problems'][$problem_id]['custom_action']['#description'] = $problem_settings['custom_action-description'];
        }
      }

      $form['problems'][$problem_id]['explanation'] = array(
        '#markup' => '<div class="problem-explanation problem-explanation-' . $problem_id . '">' . $problem_settings['explanation'] . '</div>',
      );

      // jira ticket: fcs-58.
      if ($problem_id == MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES) {
        $all_accounts = mfcs_users_load_all_users_with_banner_id($banner_id);
        if (!empty($all_accounts)) {
          $form['problems'][$problem_id]['explanation']['#markup'] .= '<br>';
          $form['problems'][$problem_id]['explanation']['#markup'] .= 'The following accounts have the same banner id';

          if ($user_is_manager || $user_is_administer) {
            $form['problems'][$problem_id]['explanation']['#markup'] .= ' (<strong>' . $banner_id_string . '</strong>)';
          }

          $form['problems'][$problem_id]['explanation']['#markup'] .= ':<ul>';

          foreach ($all_accounts as $account_id => $account_name) {
            $form['problems'][$problem_id]['explanation']['#markup'] .= '<li><a href="' . $base_path . 'requests/users-0/view/' . $account_id . $url_arguments . '">' . $account_name . '</a></li>';
          }
          $form['problems'][$problem_id]['explanation']['#markup'] .= '</ul><br>';
        }
      }
    }
  }

  if ($do_something) {
    $form['problems']['reset'] = array(
      '#id' => 'submit-problems-perform_actions',
      '#type' => 'submit',
      '#default_value' => 'Perform Actions / Reset Problems',
      '#attributes' => array(
        'class' => array(
          'problem-submit-perform_actions',
        ),
      ),
    );
  }

  return $form;
}

/**
 * Validation for mfcs_request_problems_0_user_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_user_form()
 */
function mfcs_request_problems_0_user_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $handle_problems = array();
  if (isset($form_state['values']['problems'])) {
    $handle_problems = &$form_state['values']['problems'];
  }

  $do_something = FALSE;
  foreach ($form['values']['problem_list']['#value'] as $problem_id => $problem_settings) {
    if (!$problem_settings['resettable'] && !$problem_settings['custom_action']) {
      if (!array_key_exists($problem_id, $handle_problems)) {
        continue;
      }
    }

    if (isset($handle_problems[$problem_id]['reset']) && $handle_problems[$problem_id]['reset'] == 1) {
      $do_something = TRUE;
    }
    elseif (isset($handle_problems[$problem_id]['custom_action']) && $handle_problems[$problem_id]['custom_action'] == 1) {
      $do_something = TRUE;
    }
  }

  if (!$do_something) {
    form_set_error('', "You have not specified any action to perform.");
    return;
  }
}

/**
 * Submit for mfcs_request_problems_0_user_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_user_form()
 */
function mfcs_request_problems_0_user_form_submit($form, &$form_state) {
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

  $actions = array();
  if (isset($form_state['values']['problems'])) {
    $actions = &$form_state['values']['problems'];
  }

  $perform_reset = FALSE;
  $perform_custom_action = FALSE;

  $problems = $form['values']['mfcs_user']['#value']['problems'];
  $custom_actions = array();
  foreach ($form['values']['problem_list']['#value'] as $problem_id => $problem_settings) {
    if ($problem_settings['resettable'] && array_key_exists($problem_id, $actions) && array_key_exists('reset', $actions[$problem_id])) {
      if ($actions[$problem_id]['reset'] == 1) {
        if (($problems & $problem_id) != 0) {
          $problems -= $problem_id;
        }
        $perform_reset = TRUE;
      }
    }

    if ($problem_settings['custom_action'] && array_key_exists($problem_id, $actions) && array_key_exists('custom_action', $actions[$problem_id])) {
      if ($actions[$problem_id]['custom_action'] == 1) {
        $custom_actions[$problem_id] = TRUE;
        $perform_custom_action = TRUE;
      }
    }
  }

  if ($perform_reset) {
    $results = mfcs_save_user($form['values']['problem_user']['#value']->uid, array('problems' => $problems));

    if ($results === FALSE) {
      form_set_error('form', "An error occurred while trying to reset the user problem flags. Please contact the support staff.");
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to reset the user problem flags, user: @user_id, problems: @problems.", array('@user_id' => $form['values']['mfcs_user']['#value']['user_id'], '@problems' => print_r($problems, TRUE)), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
  }

  if ($perform_custom_action) {
    foreach ($custom_actions as $custom_action_id => $custom_action_value) {
      if ($custom_action_id == MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE || $custom_action_id == MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE) {
        if (!empty($form['values']['problem_user']['#value']->mfcs['banner_id']) && is_numeric($form['values']['problem_user']['#value']->mfcs['banner_id'])) {
          $result = mfcs_synchronize_user_settings($form['values']['problem_user']['#value']);
          if ($result === TRUE) {
            if (($problems & $custom_action_id) != 0) {
              $problems -= $custom_action_id;
              mfcs_save_user($form['values']['problem_user']['#value']->uid, array('problems' => $problems));
            }
          }
          else {
            form_set_error('form', "An error occurred while trying to synchronize the user settings. Please contact the support staff.");
            watchdog(MFCS_WATCHDOG_ID, "An error occurred while trying to synchronize the user settings, user: @user_id, problems: @problems.", array('@user_id' => $form['values']['mfcs_user']['#value']['user_id'], '@problems' => print_r($problems, TRUE)), WATCHDOG_ERROR);

            $form_state['rebuild'] = TRUE;
            $form_state['redirect'] = FALSE;
            $form_state['submitted'] = FALSE;
          }
        }
      }
      elseif ($custom_action_id == MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED) {
        // warning: the 'reviewer', 'manager', etc.. role names are assumed to be define in the system.
        $reviewer_role_id = user_role_load_by_name('reviewer');
        $manager_role_id = user_role_load_by_name('manager');
        $administrator_role_id = user_role_load_by_name('administrator');
        $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
        $reviewer_exception_role_id = user_role_load_by_name('reviewer exception');

        $changes_made = FALSE;
        $system_roles = (array) $form['values']['problem_user']['#value']->roles;

        // do not alter reviewer role settings for users with 'manager', 'administrator', 'bannerless_exception', or 'reviewer_exception' roles.
        $process_reviewer_role = TRUE;
        if (isset($manager_role_id->rid) && array_key_exists($manager_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }
        elseif (isset($administrator_role_id->rid) && array_key_exists($administrator_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }
        elseif (isset($bannerless_exception_role_id->rid) && array_key_exists($bannerless_exception_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }
        elseif (isset($reviewer_exception_role_id->rid) && array_key_exists($reviewer_exception_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }

        if ($process_reviewer_role) {
          $all_coordinators = mfcs_load_room_coordinators(NULL, TRUE);
          if (array_key_exists($form['values']['problem_user']['#value']->uid, $all_coordinators)) {
            if (isset($reviewer_role_id->rid) && !array_key_exists($reviewer_role_id->rid, $system_roles)) {
              $system_roles[$reviewer_role_id->rid] = 'reviewer';
              $changes_made = TRUE;
            }
          }
          else {
            if (isset($reviewer_role_id->rid) && array_key_exists($reviewer_role_id->rid, $system_roles)) {
              unset($system_roles[$reviewer_role_id->rid]);
              $changes_made = TRUE;
            }
          }
        }

        if ($changes_made) {
          if (!user_save($form['values']['problem_user']['#value'],  array('roles' => $system_roles))) {
            form_set_error('form', 'An error occurred while trying to re-assign or re-unassign the manager role. Please contact the support staff.');
            watchdog(MFCS_WATCHDOG_ID, 'An error occurred while trying to re-assign or re-unassign the manager role, user: @user_id, problems: @problems.', array('@user_id' => $form['values']['mfcs_user']['#value']['user_id'], '@problems' => print_r($problems, TRUE)), WATCHDOG_ERROR);

            $form_state['rebuild'] = TRUE;
            $form_state['redirect'] = FALSE;
            $form_state['submitted'] = FALSE;
          }
          else {
            if (($problems & $custom_action_id) != 0) {
              $problems -= $custom_action_id;
              mfcs_save_user($form['values']['problem_user']['#value']->uid, array('problems' => $problems));
            }
          }
        }
      }
      elseif ($custom_action_id == MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID) {
        if (!user_save($form['values']['problem_user']['#value'],  array('status' => 0))) {
          form_set_error('form', 'An error occurred while trying to lock the user account. Please contact the support staff.');
          watchdog(MFCS_WATCHDOG_ID, 'An error occurred while trying to lock the user account, user: @user_id, problems: @problems.', array('@user_id' => $form['values']['mfcs_user']['#value']['user_id'], '@problems' => print_r($problems, TRUE)), WATCHDOG_ERROR);

          $form_state['rebuild'] = TRUE;
          $form_state['redirect'] = FALSE;
          $form_state['submitted'] = FALSE;
        }
        else {
          if (($problems & $custom_action_id) != 0) {
            $problems -= $custom_action_id;
            mfcs_save_user($form['values']['problem_user']['#value']->uid, array('problems' => $problems));
          }
        }
      }
    }
  }

  // don't redirect on error.
  if ($form_state['submitted'] === FALSE) {
    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/problems-0/users');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Re-synchronize user settings for a given user page.
 *
 * This page auto-returns.
 *
 * @param int $user_id
 *   Id of the user to resynchronize
 */
function mfcs_request_problems_0_users_resynchronize($user_id) {
  if (!cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');

    drupal_not_found();
    drupal_exit();
  }

  $user = user_load($user_id);
  if (!is_object($user)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/problems-0/users/resynchronize/' . $user_id);

  $page_title = 'Facilities Use Problems - Resynchronize User';
  drupal_set_title($page_title);

  $result = mfcs_synchronize_user_settings($user);
  if ($result === TRUE) {
    $redirect_array = mfcs_build_redirect_array('requests/problems-0/users');

    drupal_goto('requests/problems-0/users', array('query' => drupal_get_query_parameters()));
    return '';
  }

  $markup = '<div>';
  $markup .= 'Failed to re-synchronize user.<br>';
  $markup .= 'You may go back to the <a href="' . $base_path . 'requests/problems-0/users">user problems management page</a>.';
  $markup .= '</div>';

  return $markup;
}

/**
 * A list of requests associated with a disabled user account.
 *
 * Enable accounts will return 404 for this page.
 *
 * This does not include closed or cancelled requests.
 *
 * @param int $user_id
 *   Id of the disabled user.
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return markup
 *   The HTML markup representing the page to be renderred.
 */
function mfcs_request_problems_0_users_disabled_in_use($user_id, $page_number = NULL) {
  if (!cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');

    drupal_not_found();
    drupal_exit();
  }

  $user = user_load($user_id);
  if (!is_object($user) || $user->status != 0) {
    drupal_not_found();
    drupal_exit();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $no_js_fallback = TRUE;
  if (isset($conf['no_js_fallback']) && is_bool($conf['no_js_fallback'])) {
    $no_js_fallback = $conf['no_js_fallback'];
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/problems-0/users/disabled_in_use/' . $user_id);

  $page_title = "Facilities Use Problems - Disabled Account In Use - " . $user->name;
  drupal_set_title($page_title);


  // load sorting information
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  $columns_default = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
      $order = NULL;
    }
  }

  mfcs_include(MFCS_INCLUDE_REQUEST_LIST);
  $columns = mfcs_request_list_0_columns();

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'DESC';
    $columns_default = array(0 => strtolower($sorting['id']));
    $columns['id']['class'][] = 'sorting_1';
  }
  else {
    if ($order == 'id') {
      $sorting[$order] = is_null($sort) ? 'DESC' : $sort;
      $columns_default = array(0 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'type') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(1 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'title') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(2 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'building') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(3 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'room') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(4 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'date') {
      $sorting[$order] = is_null($sort) ? 'DESC' : $sort;
      $columns_default = array(5 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'status') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(6 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'step') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(7 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'submitted by') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(8 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'submitter name') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(9 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'submitted on') {
      $sorting[$order] = is_null($sort) ? 'DESC' : $sort;
      $columns_default = array(10 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
  }

  $rows_per_page = 25;
  $conditions = array();
  $options = array(
    'join' => TRUE,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
    'extra_fields' => array(),
  );

  if (!is_null($order)) {
    $options['sort'] = $sorting;
  }

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $options['results'] = FALSE;
  }

  $options['extra_fields']['users'] = array(
    'alias' => 'u',
    'join' => 'inner',
    'on' => 'mer.venue_coordinator = u.uid',
    'columns' => array(
      'venue_coordinator_status' => 'status',
    ),
  );

  $conditions['value_problem_status'] = array(
    'field' => 'mer.status',
    'operator' => MFCS_OPERATOR_IS_IN,
    'search' => array(MFCS_REQUEST_STATUS_LOCKED, MFCS_REQUEST_STATUS_UNLOCKED),
  );

  $conditions['value_problem_venue_coordinator'] = array(
    'field' => 'mer.venue_coordinator',
    'operator' => MFCS_OPERATOR_EQUAL,
    'search' => $user_id,
  );

  $conditions['value_problem_venue_coordinator_status'] = array(
    'field' => 'u.status',
    'operator' => MFCS_OPERATOR_EQUAL,
    'search' => 0,
  );

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $items_returned = mfcs_request_load_listing($conditions, $options);

  $count = 0;
  if (isset($items_returned['count'])) {
    $count = $items_returned['count'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($count > 0 && $options['start'] >= $count || $count == 0 && $options['start'] > $count) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // do not show count values when no fallback is available.
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $count = 0;
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $options['columns'] = $columns;
  $options['columns_default'] = $columns_default;

  $options['ajax_path'] = "'" . $base_path . 'requests/problems-0/users/disabled_in_use/ajax/' . $user_id . $url_arguments . "'";

  $table = mfcs_build_request_ajax_table($items_returned, $conditions, $options);
  $table['attributes']['class'][] = 'mfcs-table-requests';
  $table['attributes']['class'][] = 'mfcs-table-problems-disabled_in_use';
  $table['attributes']['id'] = 'mfcs-problems_0-disabled_in_use';

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($count == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $count) {
    $navigation_total = $count;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $count . " entries.";
  $navigation_bottom .= '</div>';

  if ($count > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $output = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $output .= mcneese_libraries_theme_table($table, $table['datatable_settings']);
  #$output .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  return $output;
}

/**
 * Return an array of user names who have the given banner id.
 *
 * jira ticket: fcs-58.
 *
 * @param int $banner_id
 *   The banner id integer.
 *
 * @return array
 *   An array with the user ids as the key and the user names as the value.
 */
function mfcs_users_load_all_users_with_banner_id($banner_id) {
  if (!cf_is_integer($banner_id)) {
    return array();
  }

  $query = db_query('select u.uid as uid, u.name as name from mfcs_users mu inner join users u on mu.user_id = u.uid where mu.banner_id = :banner_id', array(':banner_id' => $banner_id));
  $results = $query->fetchAll();

  $names = array();
  foreach ($results as $result) {
    $names[$result->uid] = $result->name;
  }

  return $names;
}

/**
 * View and manage problems associated with a specific user.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 */
function mfcs_request_problems_0_requests_form($form, &$form_state, $page_number = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $no_js_fallback = TRUE;
  if (isset($conf['no_js_fallback']) && is_bool($conf['no_js_fallback'])) {
    $no_js_fallback = $conf['no_js_fallback'];
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/problems-0/requests/' . $page_number);

  $page_title = "Facilities Use Problems - Requests";
  drupal_set_title($page_title);


  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_problems_by_id = mfcs_get_request_problems_list_options(NULL, FALSE, TRUE);

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

  $class_id = ' ';
  $class_problem = '';
  $class_request_id = '';
  $class_date = '';
  $class_reason = '';
  $class_ignore = 'no-script-hidden script-hidden';
  $class_operations = 'no-print ';

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
    elseif ($order == 'problem') {
      $sorting['problem'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_problem .= 'sorting_1 ';
    }
    elseif ($order == 'request id') {
      $sorting['request_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_request_id .= 'sorting_1 ';
    }
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    elseif ($order == 'reason') {
      $sorting['reason'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_reason .= 'sorting_1 ';
    }
    elseif ($order == 'ignore') {
      $sorting['ignore'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_ignore .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 40;
  $options = array(
    'sort' => $sorting,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
    'include_ignored' => FALSE,
  );

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $options['results'] = FALSE;
  }

  $conditions = array();

  $problems_returned = mfcs_miscellaneous_problems_request_search($conditions, $options);

  $problems_total = 0;
  if (is_array($problems_returned) && array_key_exists('count', $problems_returned)) {
    $problems_total = $problems_returned['count'];
  }

  $problems = array();
  if (isset($problems_returned['results']) && is_array($problems_returned['results'])) {
    $problems = $problems_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($problems_total > 0 && $options['start'] >= $problems_total || $problems_total == 0 && $options['start'] > $problems_total) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // do not show count values when no fallback is available.
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $count = 0;
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $rows = array();
  foreach ($problems as $id => $problem) {
    $row = array();
    $additional_operations = '';

    $row['id'] = array(
      'data' => '' . (int) $problem->id,
      'class' => $class_id,
    );

    $row['problem'] = array(
      'data' => '',
      'class' => $class_problem,
    );

    if (array_key_exists($problem->problem,  $request_problems_by_id)) {
      $row['problem']['data'] = $request_problems_by_id[$problem->problem];
    }
    else {
      $row['problem']['data'] = 'Unknown Code';
    }

    $row['request_id'] = array(
      'data' => '' . (int) $problem->request_id,
      'class' => $class_request_id,
    );

    $row['date'] = array(
      'data' => check_plain(date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $problem->date)),
      'class' => $class_date,
    );

    $row['reason'] = array(
      'data' => (isset($problem->reason) ? check_plain($problem->reason) : ''),
      'class' => $class_reason,
    );

    $row['ignore'] = array(
      'data' => (isset($problem->ignore) && $problem->ignore == 1 ? 'Yes' : 'No'),
      'class' => $class_ignore,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/problems-0/requests/request/' . $problem->id . $url_arguments . '" class="operation-link operation-link-details">Details</a>',
      'class' => $class_operations,
    );

    $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/view-0/' . $problem->request_id . $url_arguments . '" class="operation-link operation-link-request">Request</a>';
    $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/problems-0/requests/request/' . $problem->id . '/recheck' . $url_arguments . '" class="operation-link operation-link-recheck white-space-nowrap">Re-Check</a>';

    $rows[] = $row;
  }

  $problem_list = array();
  $problem_list['caption'] = t("Request Problems");
  $problem_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'problem' => array('field' => 'problem', 'data' => "Problem", 'align' => 'center'),
    'request_id' => array('field' => 'request_id', 'data' => "Request ID", 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
    'reason' => array('field' => 'reason', 'data' => "Reason", 'align' => 'center'),
    'ignore' => array('field' => 'ignore', 'data' => "Ignore", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $problem_list['attributes'] = array();
  $problem_list['attributes']['id'] = 'mfcs-problem_form';
  $problem_list['attributes']['class'] = array(
    'no_default',
    'mfcs-problem_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $problem_list['attributes']['summary'] = 'Collection of requests in which the system has detected problems with.';

  $problem_list['rows'] = $rows;
  $problem_list['empty'] = t("There are no requests with problems detected by the system.");

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
      'url' => "'" . $base_path . 'requests/problems-0/requests/ajax' . $url_arguments . "'",
      'type' => "'POST'",
    ),
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id'),
      array('id' => 1, 'column' => 'problem', 'name' => "Problem", 'className' => 'column-problem'),
      array('id' => 2, 'column' => 'request_id', 'name' => "Request ID", 'className' => 'column-request_id'),
      array('id' => 3, 'column' => 'date', 'name' => "Date", 'className' => 'column-date'),
      array('id' => 4, 'column' => 'reason', 'name' => "Reason", 'className' => 'column-reason'),
      array('id' => 5, 'column' => 'ignore', 'name' => "Ignore", 'className' => 'column-ignore', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 6, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'rowId' => '0',
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $problem_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $problem_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $ajax_path = $base_path . 'requests/problems-0/requests/ajax';

  $including_ignored = "function (e, dt, node, config) {" . "\n";
  $including_ignored .= "  var data = dt.ajax.params();" . "\n";
  $including_ignored .= "  var url = dt.ajax.url();" . "\n";

  $including_ignored .= "  if (url == '" . $ajax_path . "/show_ignored' || url == '" . $ajax_path . "/show_ignored" . $url_arguments . "') {" . "\n";
  $including_ignored .= "    dt.ajax.url('" . $ajax_path . "/hide_ignored" . $url_arguments . "');" . "\n";
  $including_ignored .= "    dt.buttons('.datatable-including_ignored').text('Show Ignored Problems');" . "\n";
  $including_ignored .= "  }" . "\n";
  $including_ignored .= "  else if (url == '" . $ajax_path . "/hide_ignored' || url == '" . $ajax_path . "/hide_ignored" . $url_arguments . "') {" . "\n";
  $including_ignored .= "    dt.ajax.url('" . $ajax_path . "/show_ignored" . $url_arguments . "');" . "\n";
  $including_ignored .= "    dt.buttons('.datatable-including_ignored').text('Hide Ignored Problems');" . "\n";
  $including_ignored .= "  }" . "\n";
  $including_ignored .= "  else {" . "\n";
  $including_ignored .= "    dt.ajax.url('" . $ajax_path . "/show_ignored" . $url_arguments . "');" . "\n";
  $including_ignored .= "    dt.buttons('.datatable-including_ignored').text('Hide Ignored Problems');" . "\n";
  $including_ignored .= "  }" . "\n";

  $including_ignored .= "  dt.ajax.reload();" . "\n";
  $including_ignored .= "}" . "\n";

  $datatable_settings['dom'] = 'lfCBrtip';
  $datatable_settings['buttons'] = array();
  $datatable_settings['buttons']['Include Ignored'] = array(
    'text' => "'Show Ignored Problems'",
    'action' => $including_ignored,
    'className' => "'datatable-including_ignored'",
  );
  unset($including_ignored);

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/requests' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/requests' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/requests/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/requests/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($problems_total == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $problems_total) {
    $navigation_total = $problems_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $problems_total . " entries.";
  $navigation_bottom .= '</div>';

  if ($problems_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/requests/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/requests/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['problem_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['problem_list']['#markup'] .= mcneese_libraries_theme_table($problem_list, $datatable_settings);
  #$form['problem_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  return $form;
}

/**
 * Validation for mfcs_request_problems_0_requests_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_requests_form()
 */
function mfcs_request_problems_0_requests_form_validate($form, &$form_state) {
  form_set_error('', "Currently this form has nothing to submit.");
}

/**
 * Submit for mfcs_request_problems_0_requests_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_requests_form()
 */
function mfcs_request_problems_0_requests_form_submit($form, &$form_state) {
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

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/problems-0/requests');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * View and manage problems associated with a specific request.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $problem_id
 *   The id of the request problem to load.
 */
function mfcs_request_problems_0_request_form($form, &$form_state, $problem_id) {
  if (!cf_is_integer($problem_id) || $problem_id < 0) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $problem_id = (int) $problem_id;

  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  mfcs_add_canonical_headers('requests/problems-0/requests/request/' . $problem_id);

  $problem_results = mfcs_miscellaneous_problems_request_load($problem_id, TRUE, TRUE);
  if (empty($problem_results)) {
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

  $problem = array_pop($problem_results);
  unset($problem_results);

  $request_problems_by_id = mfcs_get_request_problems_list_options(NULL, FALSE, TRUE);
  $status_options = mfcs_get_request_status_list_options(NULL, FALSE, TRUE);

  $problem_code_name = 'Unknown';
  if (array_key_exists($problem->problem, $request_problems_by_id)) {
    $problem_code_name = $request_problems_by_id[$problem->problem];
  }

  $page_title = "Facilities Use Problems - Request Problem: " . $problem_id . " (" . $problem_code_name . " Problem)";
  drupal_set_title($page_title);

  $user = cf_current_user();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['problem'] = array(
    '#type' => 'value',
    '#value' => $problem,
  );

  $form['form']['problem_id'] = array(
    '#type' => 'value',
    '#value' => $problem_id,
  );

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );

  $form['actions'] = array(
    '#tree' => TRUE,
  );

  $form['actions']['action'] = array(
    '#type' => 'select',
    '#title' => 'Actions',
    '#default_value' => NULL,
    '#options' => array(
      NULL => '- Select Action -',
      'recheck' => 'Re-Check Problem',
    ),
    '#required' => FALSE,
    '#attributes' => array(
      'class' => array(
        'no-print',
      ),
    ),
  );

  $form['submit'] = array(
    '#id' => 'request_problems-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'request_problems-submit',
      ),
    ),
  );

  $form['submit']['back'] = array(
    '#id' => 'request_problems-submit-back',
    '#name' => 'request_problems-submit-back',
    '#type' => 'submit',
    '#default_value' => 'Back',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'request_problems-submit-back',
      ),
    ),
  );

  $form['submit']['submit'] = array(
    '#id' => 'request_problems-submit-submit',
    '#name' => 'request_problems-submit-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'request_problems-submit-submit',
      ),
    ),
  );

  if ($problem->ignore == 1) {
    $form['actions']['action']['#options']['unignore'] = 'Unignore Problem';
  }
  else {
    $form['actions']['action']['#options']['ignore'] = 'Ignore Problem';
  }


  $form['markup']['#markup'] .= '<div class="request_problem-summary">';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-id">Problem ID:</span> <span class="request_problem-summary_row-value value-id">' . $problem_id . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-request_id">Request ID:</span> <span class="request_problem-summary_row-value value-request_id">' . (int) $problem->request_id . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-request_title">Request Name:</span> <span class="request_problem-summary_row-value value-request_title"><a href="' . $base_path . 'requests/view-0/' . (int) $problem->request_id . $url_arguments . '" class="operation-link operation-link-request">' . check_plain($problem->request_title) . '</a></span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-request_title">Request Status:</span> <span class="request_problem-summary_row-value value-request_status">' . (array_key_exists($problem->request_status, $status_options) ? $status_options[$problem->request_status] : 'Unknown') . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-problem">Problem:</span> <span class="request_problem-summary_row-value value-problem">' . $problem_code_name . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-date_detected">Date Detected:</span> <span class="request_problem-summary_row-value value-date_detected">' . date('l, F j, Y h:i:s a', $problem->date) . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-ignored">Ignored:</span> <span class="request_problem-summary_row-value value-ignored">' . (isset($problem->ignore) && $problem->ignore == 1 ? 'Yes' : 'No') . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="request_problem-summary_row"><span class="request_problem-summary_row-label label-reason">Reason:</span> <span class="request_problem-summary_row-value value-reason">' . (isset($problem->reason) ? check_plain($problem->reason) : '') . '</span></div>';
  $form['markup']['#markup'] .= '</div>';

  $form['markup']['#markup'] .= '<br>';

  $form['markup']['#markup'] .= 'This problem is catagorized as a <strong>' . $problem_code_name . '</strong> problem.<br>';
  $form['markup']['#markup'] .= '<br>';

  if ($problem->problem == MFCS_REQUEST_PROBLEM_CONFLICT) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where one or more requests are scheduled on the same day and time at the same location, building, and room.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    $form['markup']['#markup'] .= '<a href="' . $base_path . 'requests/view-0/' . (int) $problem->request_id . $url_arguments . '" class="request_problem-extra-conflict-link" title="View Request ' . (int) $problem->request_id . '">Request ' . (int) $problem->request_id . '</a> is in conflict with the following requests:';
    $form['markup']['#markup'] .= '<ul class="request_problem-extra-conflict-items">';
    foreach ($details as $detail) {
      $form['markup']['#markup'] .= '<li class="request_problem-extra-conflict-item"><a href="' . $base_path . 'requests/view-0/' . $detail . $url_arguments . '" class="request_problem-extra-conflict-item-link" title="View Request ' . $detail . '">Request ' . $detail . '</a></li>';
    }
    $form['markup']['#markup'] .= '</ul>';
    $form['markup']['#markup'] .= '<br>';
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_STALE) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where a request has one or more of its scheduled dates in the past but is still open for review.' . '<br>';
    $form['markup']['#markup'] .= 'This is primarily informative, but some stale requests may need to be cancelled.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_STUCK) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the request is somehow being prevented from progressing in the review process when it otherwise should be able to do so.' . '<br>';
    $form['markup']['#markup'] .= 'This could happen when a reviewer position is altered during the review process.' . '<br>';
    $form['markup']['#markup'] .= 'This could happen when a venue coordinator account is no longer valid and must be re-assigned.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_BLOCKED) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where a request is being prevented from progressing further in the review process.' . '<br>';
    $form['markup']['#markup'] .= 'This generally applies when multiple requests are made for the same date and time (only one request will make it into the review process and the others will be stuck).' . '<br>';
    $form['markup']['#markup'] .= 'Stuck requests should be cancelled at the venue coordinators discretion.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_COORDINATOR) {
    $form['actions']['action']['#options']['reassign_venue_coordinator'] = 'Re-Assign Venue Coordinator';

    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the venue coordinator assigned to a request no longer exists, is disabled, or is otherwise invalid.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $form['markup']['#markup'] .= 'The <strong>' . $form['actions']['action']['#options']['reassign_venue_coordinator'] . ' </strong> action will attempt to auto-reassign the venue coordinator.' . '<br>';
    $form['markup']['#markup'] .= 'The re-assigmment functionality only refreshes the banner information.<br>';
    $form['markup']['#markup'] .= 'If that information has not changed for the venue coordinator, then this will do nothing.<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['coordinator'])) {
      $form['markup']['#markup'] .= '<strong>Coordinator ID:</strong> ' . (int) $details['coordinator']['id'] . '<br>';
      $form['markup']['#markup'] .= '<strong>Coordinator Account:</strong> <a href="' . $base_path . 'requests/users-0/view/' . (int) $details['coordinator']['id'] . $url_arguments . '">' . check_plain($details['coordinator']['name']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>Coordinator First Name:</strong> ' . check_plain($details['coordinator']['first_name']) . '<br>';
      $form['markup']['#markup'] .= '<strong>Coordinator Last Name:</strong> ' . check_plain($details['coordinator']['last_name']) . '<br>';
    }
    elseif (isset($details['current_coordinator']) && isset($details['new_coordinator'])) {
      $form['markup']['#markup'] .= '<strong>Old Coordinator ID:</strong> ' . (int) $details['current_coordinator']['id'] . '<br>';
      $form['markup']['#markup'] .= '<strong>Old Coordinator Account:</strong> <a href="' . $base_path . 'requests/users-0/view/' . (int) $details['current_coordinator']['id'] . $url_arguments . '">' . check_plain($details['current_coordinator']['name']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>Old Coordinator First Name:</strong> ' . check_plain($details['current_coordinator']['first_name']) . '<br>';
      $form['markup']['#markup'] .= '<strong>Old Coordinator Last Name:</strong> ' . check_plain($details['current_coordinator']['last_name']) . '<br>';

      $form['markup']['#markup'] .= '<br>';

      $form['markup']['#markup'] .= '<strong>New Coordinator ID:</strong> ' . (int) $details['new_coordinator']['id'] . '<br>';
      $form['markup']['#markup'] .= '<strong>New Coordinator Account:</strong> <a href="' . $base_path . 'requests/users-0/view/' . (int) $details['new_coordinator']['id'] . $url_arguments . '">' . check_plain($details['new_coordinator']['name']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>New Coordinator First Name:</strong> ' . check_plain($details['new_coordinator']['first_name']) . '<br>';
      $form['markup']['#markup'] .= '<strong>New Coordinator Last Name:</strong> ' . check_plain($details['new_coordinator']['last_name']) . '<br>';
    }

    unset($details);
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_REQUESTER) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the requester assigned to a request no longer exists, is disabled, or is otherwise invalid.' . '<br>';
    $form['markup']['#markup'] .= 'The requester is the account used to create the request and is viewable under the <em>created by</em> section of the request.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['requester'])) {
      $form['markup']['#markup'] .= '<strong>Requester ID:</strong> ' . (int) $details['requester']['user_id'] . '<br>';
      $form['markup']['#markup'] .= '<strong>Requester Account:</strong> <a href="' . $base_path . 'requests/users-0/view/' . (int) $details['requester']['user_id'] . $url_arguments . '">' . check_plain($details['requester']['name']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>Requester First Name:</strong> ' . check_plain($details['requester']['first_name']) . '<br>';
      $form['markup']['#markup'] .= '<strong>Requester Last Name:</strong> ' . check_plain($details['requester']['last_name']) . '<br>';
    }

    unset($details);
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_ROOM) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the room assigned to a request no longer exists, is disabled, or is otherwise invalid.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
    $form['markup']['#markup'] .= 'You can may want to <a href="' . $base_path . 'requests/troubleshoot-0/rooms' . $url_arguments . '">look at the listing of rooms according to the local banner information</a>.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['room']) && isset($details['building'])) {
      $form['markup']['#markup'] .= '<strong>Room Unavailabilities:</strong> <a href="' . $base_path . 'requests/unavailable-0/' . (int) $details['room']['id'] . $url_arguments . '">' . check_plain($details['building']['machine_name']) . ' ' . check_plain($details['room']['machine_name']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>Room Information:</strong> <a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . (int) $details['room']['id'] . $url_arguments . '">' . check_plain($details['room']['human_name']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>Building Information:</strong> <a href="' . $base_path . 'requests/troubleshoot-0/buildings/' . (int) $details['building']['id'] . $url_arguments . '">' . check_plain($details['building']['human_name']) . '</a><br>';
    }

    unset($details);
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_BUILDING) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the building assigned to a request no longer exists, is disabled, or is otherwise invalid.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
    $form['markup']['#markup'] .= 'You can may want to <a href="' . $base_path . 'requests/troubleshoot-0/buildings' . $url_arguments . '">look at the listing of buildings according to the local banner information</a>.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['building'])) {
      $form['markup']['#markup'] .= '<strong>Building Information:</strong> <a href="' . $base_path . 'requests/troubleshoot-0/buildings/' . $details['building']['id'] . $url_arguments . '">' . check_plain($details['building']['human_name']) . '</a><br>';
    }

    unset($details);
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_LOCATION) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the location assigned to a request no longer exists, is disabled, or is otherwise invalid.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
    $form['markup']['#markup'] .= 'You can may want to <a href="' . $base_path . 'requests/troubleshoot-0/locations' . $url_arguments . '">look at the listing of locations according to the local banner information</a>.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['location'])) {
      $form['markup']['#markup'] .= '<strong>Location Information:</strong> <a href="' . $base_path . 'requests/troubleshoot-0/locations/' . $details['location']['id'] . $url_arguments . '">' . check_plain($details['location']['human_name']) . '</a><br>';
    }

    unset($details);
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_CACHE_REQUEST) {
    $form['actions']['action']['#options']['rebuild_request_cache'] = 'Rebuild Request Cache';

    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where internal database cache is inconsistent with the stored values.' . '<br>';
    $form['markup']['#markup'] .= 'This issue will cause inconsistencies with the values associated with the request.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
    $form['markup']['#markup'] .= 'The <strong>' . $form['actions']['action']['#options']['rebuild_request_cache'] . '</strong> action should fix this issue by rebuilding the cache associated with the specific request.' . '<br>';
    $form['markup']['#markup'] .= '<br>';
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_HOLIDAY) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the request is assigned on the same date as a holiday.' . '<br>';
    $form['markup']['#markup'] .= 'This is not necesserily a problem because some events may appropriately happen on holidays.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['holiday'])) {
      $holiday_date = reset($details['dates']);
      $form['markup']['#markup'] .= '<strong>Holiday:</strong> <a href="' . $base_path . 'requests/holiday-0/view/' . (int) $details['holiday']['year'] . '/' . (int) $details['holiday']['id'] . $url_arguments . '">' . check_plain($details['holiday']['title']) . '</a><br>';
      $form['markup']['#markup'] .= '<strong>Date of Holiday:</strong> ' . check_plain(date(MFCS_DATE_FORMAT_DATE, $holiday_date)) . '<br>';
      unset($holiday_date);
    }

    unset($details);
  }
  elseif ($problem->problem == MFCS_REQUEST_PROBLEM_UNAVAILABLE) {
    $form['markup']['#markup'] .= 'A <strong>' . $problem_code_name . '</strong> problem is a problem where the request is assigned during times in which a given room is unavailable.' . '<br>';
    $form['markup']['#markup'] .= '<br>';

    $details = unserialize($problem->details);

    if (isset($details['unavailabilities'])) {
      $form['markup']['#markup'] .= '<strong>Unavailability:</strong><ul>';
      foreach ($details['unavailabilities'] as $unavailability) {
        $unavailability_date_start = $unavailability['date_start'];
        $unavailability_date_stop = $unavailability['date_stop'];

        $date_string = 'Unknown';
        if ($unavailability_date_start == strtotime('midnight today', $unavailability_date_start) && $unavailability_date_stop == strtotime('midnight tomorrow', $unavailability_date_start)) {
          $date_string = check_plain(date(MFCS_DATE_FORMAT_DATE, $unavailability_date_start));
        }
        else {
          $date_string = check_plain(date(MFCS_DATE_FORMAT_DATE_TIME, $unavailability_date_start)) . ' to ' . check_plain(date(MFCS_DATE_FORMAT_DATE_TIME, $unavailability_date_stop));
        }
        unset($unavailability_date_start);
        unset($unavailability_date_stop);

        $form['markup']['#markup'] .= '<li><a href="' . $base_path . 'requests/unavailable-0/view/' . $unavailability['year'] . '/' . $details['room_id']  . '/' . $unavailability['id'] . $url_arguments . '">' . check_plain($unavailability['title']) . '</a> (' . $date_string .')</li>';
      }
      $form['markup']['#markup'] .= '</ul>';
      unset($date_string);
    }

    unset($details);
  }

  return $form;
}

/**
 * Validation for mfcs_request_problems_0_request_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_request_form()
 */
function mfcs_request_problems_0_request_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'request_problems-submit-back') {
    // no processing necessary here.
    return;
  }

  if (!isset($form['form']['problem_id']['#value'])) {
    // should never happen.
    $user = cf_current_user();
    form_set_error('', 'Invalid problem data detected, cannot continue.');
    watchdog(MFCS_WATCHDOG_ID, 'Unable to submit request problems form because the problem id is missing or invalid, user: @user_id.', array('@user_id' => $user->uid), WATCHDOG_ERROR);
    return;
  }

  if (!isset($form['form']['problem']['#value']) || !is_object($form['form']['problem']['#value'])) {
    // should never happen.
    $user = cf_current_user();
    form_set_error('', 'Invalid problem data detected, cannot continue.');
    watchdog(MFCS_WATCHDOG_ID, 'Unable to submit request problems form because the problem object is missing or invalid, user: @user_id, problem_id: @problem_id.', array('@user_id' => $user->uid, '@problem_id' => $form['form']['problem_id']), WATCHDOG_ERROR);
    return;
  }

  global $conf;
  global $base_path;

  $do_something = FALSE;

  if (isset($form_state['values']['actions']['action'])) {
    $request_problems_by_id = mfcs_get_request_problems_list_options(NULL, FALSE, TRUE);
    if (array_key_exists($form['form']['problem']['#value']->problem, $request_problems_by_id) && $form['form']['problem']['#value']->problem != MFCS_REQUEST_PROBLEM_NONE) {
      if ($form_state['values']['actions']['action'] == 'recheck') {
        $do_something = TRUE;
      }
      elseif ($form_state['values']['actions']['action'] == 'reassign_venue_coordinator') {
        $do_something = TRUE;

        $request = mfcs_load_request_by_id($form['form']['problem']['#value']->request_id);
        if (isset($request['venue_coordinator']['user_id'][0]->value)) {
          mfcs_include(MFCS_INCLUDE_WORKFLOW);

          if (empty($conf['fcs_failsafe_coordinator']) || !is_integer($conf['fcs_failsafe_coordinator'])) {
            watchdog(MFCS_WATCHDOG_ID, 'Failed to load \'fcs_failsafe_coordinator\' settings. Your settings.php does not contain the failsafe coordinator user id.', array(), WATCHDOG_CRITICAL);
            $new_coordinator_id = 1; // as a last resort fall back to the default drupal admin account.
          }
          else {
            $new_coordinator_id = $conf['fcs_failsafe_coordinator'];
          }

          $extra_text = '';
          if (isset($request['top']['room'][0]->value)) {
            $room_information = mfcs_load_rooms(NULL, NULL, $request['top']['room'][0]->value);
            if (isset($room_information->room_coordinator_id)) {
              $new_coordinator_id = $room_information->room_coordinator_id;
            }
            $extra_text .= ' Room Information: ' . $room_information->room_name . ' (' . $room_information->room_number . '). Building Information: ' . $room_information->building_name . ' (' . $room_information->building_code . ').';
          }

          $coordinator = user_load($room_information->room_coordinator_id);
          $extra_text .= ' Coordinator Information: <a href="' . $base_path . 'requests/users-0/view/' . $room_information->room_coordinator_id . '" title="View user account">' . check_plain($coordinator->mfcs['first_name']) . ' ' . check_plain($coordinator->mfcs['last_name']) . '</a>.';

          if ($new_coordinator_id == $request['venue_coordinator']['user_id'][0]->value) {
            form_set_error('actions][action', 'Unable to re-assign room/venue coordinator, the banner information remains the same.' . $extra_text);
          }
        }
      }
      elseif ($form_state['values']['actions']['action'] == 'rebuild_request_cache') {
        if ($form['form']['problem']['#value']->problem == MFCS_REQUEST_PROBLEM_CACHE_REQUEST) {
          $do_something = TRUE;
        }
        else {
          form_set_error('actions][action', 'Cache will only be re-built for a problem with a cache request problem code.' . $extra_text);
        }
      }
      elseif ($form_state['values']['actions']['action'] == 'ignore') {
        if ($form['form']['problem']['#value']->ignore == 1) {
          form_set_error('actions][action', 'This request is already being ignored.');
        }
        else {
          $do_something = TRUE;
        }
      }
      elseif ($form_state['values']['actions']['action'] == 'unignore') {
        if ($form['form']['problem']['#value']->ignore == 0) {
          form_set_error('actions][action', 'This request is already not being ignored.');
        }
        else {
          $do_something = TRUE;
        }
      }
    }
    else {
      form_set_error('', 'An invalid problem code was specified, cannot continue.');
      watchdog(MFCS_WATCHDOG_ID, 'Unable to submit request problems form because the problem code invalid, user: @user_id, problem_id: @problem_id, problem: @problem.', array('@user_id' => $user->uid, $form['form']['problem_id'], '@problem' => $form['form']['problem']['#value']->problem), WATCHDOG_ERROR);
    }
  }

  if (!$do_something) {
    form_set_error('', 'You have not specified any action to perform.');
    return;
  }
}

/**
 * Submit for mfcs_request_problems_0_request_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_problems_0_request_form()
 */
function mfcs_request_problems_0_request_form_submit($form, &$form_state) {
  global $conf;
  global $mfcs_determined;
  $user = cf_current_user();

  $instance = mfcs_instance();

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $return_to_problem_page = TRUE;
  $request_id = NULL;
  if (isset($form['form']['problem']['#value']->request_id) && is_numeric($form['form']['problem']['#value']->request_id)) {
    $request_id = (int) $form['form']['problem']['#value']->request_id;
  }

  if ($clicked_id == 'request_problems-submit-back') {
    $return_to_problem_page = FALSE;
  }
  elseif ($form_state['values']['actions']['action'] == 'recheck') {
    $recheck_result = mfcs_request_problems_0_request_perform_recheck($request_id, $form['form']['problem']['#value']->problem);
    if ($recheck_result === TRUE) {
      $return_to_problem_page = FALSE;
    }

    // note: re-check function already writes to the log, so there should be no manual log entry query here.
  }
  elseif ($form_state['values']['actions']['action'] == 'reassign_venue_coordinator') {
    mfcs_include(MFCS_INCLUDE_WORKFLOW);

    $failure = TRUE;
    $request = mfcs_load_request_by_id($request_id);
    if (is_array($request) && isset($request['venue_coordinator']['user_id'][0]->value)) {
      $old_coordinator_id = $request['venue_coordinator']['user_id'][0]->value;

      if (empty($conf['fcs_failsafe_coordinator']) || !is_integer($conf['fcs_failsafe_coordinator'])) {
        $new_coordinator_id = 1; // as a last resort fall back to the default drupal admin account.
      }
      else {
        $new_coordinator_id = $conf['fcs_failsafe_coordinator'];
      }

      $room_id = (int) $request['top']['room'][0]->value;
      $room_information = mfcs_load_rooms(NULL, NULL, $room_id, TRUE);
      if (isset($room_information->room_coordinator_id)) {
        $new_coordinator_id = $room_information->room_coordinator_id;
      }

      $old_coordinator_user = mfcs_load_basic_user_settings_by_user_id($old_coordinator_id);
      $old_coordinator_name = NULL;
      if (!empty($old_coordinator_user['name'])) {
        if (!empty($old_coordinator_user['first_name']) && !empty($old_coordinator_user['last_name'])) {
          $old_coordinator_name = $old_coordinator_user['first_name'] . ' ' . $old_coordinator_user['last_name'];
        }
        else {
          $old_coordinator_name = $old_coordinator_user['name'];
        }
      }

      $new_coordinator_user = mfcs_load_basic_user_settings_by_user_id($new_coordinator_id);
      $new_coordinator_name = NULL;
      if (!empty($new_coordinator_user['name'])) {
        if (!empty($new_coordinator_user['first_name']) && !empty($new_coordinator_user['last_name'])) {
          $new_coordinator_name = $new_coordinator_user['first_name'] . ' ' . $new_coordinator_user['last_name'];
        }
        else {
          $new_coordinator_name = $new_coordinator_user['name'];
        }
      }

      $extra = array();
      $extra['revisions']['message'] = 'The Venue Coordinator has been changed from ' . $old_coordinator_name . ' [id: ' . $old_coordinator_id . '] to ' . $new_coordinator_name . ' [id: ' . $new_coordinator_id . '].';

      $reassigned = mfcs_workflow_set_venue_coordinator($request_id, $new_coordinator_id, NULL, $extra);
      if ($reassigned) {
        $failure = FALSE;
      }
    }

    $transaction = db_transaction();
    try {
      $fields = array(
        'code' => MFCS_LOG_PROBLEMS_REASSIGN_VENUE_COORDINATOR,
        'request_id' => $request_id,
        'user_id' => $user->uid,
        'date' => $instance,
      );

      $query = db_insert('mfcs_log_problems_requests');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }

    if ($failure) {
      form_set_error('form', 'An error occurred while trying to re-assign the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to re-assign the request: @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
    else {
      $return_to_problem_page = FALSE;

      $review_extra = array(
        'never_requester' => TRUE,
        'never_coordinator' => TRUE,
      );

      mfcs_workflow_step_review($request_id, $user, MFCS_REVIEW_DECISION_REASSIGN_COORDINATOR, MFCS_REVIEW_RESTRICTIONS_MANAGERS, $extra['revisions']['message'], $review_extra);

      $parameters = array();
      $parameters['changed_by'] = $user->uid;
      $parameters['changed_type'] = 'user';
      $parameters['reassigned'] = TRUE;
      $parameters['reassigned_from'] = $old_coordinator_id;
      $parameters['reassigned_to'] = $new_coordinator_id;
      $parameters['reassigned_type'] = 'coordinator';
      mfcs_send_workflow_emails($request_id, $parameters);
    }
  }
  elseif ($form_state['values']['actions']['action'] == 'rebuild_request_cache') {
    $recached = mfcs_request_problem_0_request_perform_recache($request_id);

    $transaction = db_transaction();
    try {
      $fields = array(
        'code' => MFCS_LOG_PROBLEMS_REASSIGN_REBUILD_CACHE,
        'request_id' => $request_id,
        'user_id' => $user->uid,
        'date' => $instance,
      );

      $query = db_insert('mfcs_log_problems_requests');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }

    // once the cache is rebuilt, re-check the request to ensure the problem is corrected.
    if ($recached) {
      $recheck_result = mfcs_request_problems_0_request_perform_recheck($request_id, $form['form']['problem']['#value']->problem);
      if ($recheck_result === TRUE) {
        $return_to_problem_page = FALSE;
      }
    }
    else {
      form_set_error('form', 'An error occurred while trying to re-cache the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to re-cache the request: @request_id.', array('@request_id' => $request_id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
  }
  elseif ($form_state['values']['actions']['action'] == 'ignore') {
    $transaction = db_transaction();
    try {
      $fields = array(
        'code' => MFCS_LOG_PROBLEMS_IGNORE,
        'request_id' => $request_id,
        'user_id' => $user->uid,
        'date' => $instance,
      );

      $query = db_insert('mfcs_log_problems_requests');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }

    $transaction = db_transaction();
    try {
      $query = db_update('mfcs_problems_requests');
      $query->condition('id', $form['form']['problem']['#value']->id);
      $query->fields(array('ignore' => 1));
      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      form_set_error('form', 'An error occurred while trying to ignore the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to ignore the problem: @problem_id.', array('@problem_id' => $form['form']['problem']['#value']->id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      form_set_error('form', 'An error occurred while trying to ignore the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to ignore the problem: @problem_id.', array('@problem_id' => $form['form']['problem']['#value']->id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
  }
  elseif ($form_state['values']['actions']['action'] == 'unignore') {
    $transaction = db_transaction();
    try {
      $fields = array(
        'code' => MFCS_LOG_PROBLEMS_UNIGNORE,
        'request_id' => $request_id,
        'user_id' => $user->uid,
        'date' => $instance,
      );

      $query = db_insert('mfcs_log_problems_requests');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
    }

    $transaction = db_transaction();
    try {
      $query = db_update('mfcs_problems_requests');
      $query->condition('id', $form['form']['problem']['#value']->id);
      $query->fields(array('ignore' => 0));
      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      form_set_error('form', 'An error occurred while trying to unignore the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to unignore the problem: @problem_id.', array('@problem_id' => $form['form']['problem']['#value']->id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      form_set_error('form', 'An error occurred while trying to unignore the request. Please contact the support staff.');
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to unignore the problem: @problem_id.', array('@problem_id' => $form['form']['problem']['#value']->id), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
  }

  // don't redirect on error.
  if ($form_state['submitted'] === FALSE) {
    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if ($return_to_problem_page) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/problems-0/requests/request/' . $form['form']['problem']['#value']->id);
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/problems-0/requests');
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Re-synchronize a specific problem detected for a request.
 *
 * This page auto-returns.
 *
 * @param int $problem_id
 *   Id of the problem to re-check.
 *
 * @see: mfcs_request_problems_0_request_perform_recheck()
 */
function mfcs_request_problems_0_request_recheck($problem_id) {
  if (!cf_is_integer($problem_id)) {
    cf_error::invalid_integer('problem_id');

    drupal_not_found();
    drupal_exit();
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/problems-0/requests/request/' . $problem_id . '/recheck');

  $page_title = "Facilities Use Problems - Re-Check Problem";
  drupal_set_title($page_title);

  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $problem_results = mfcs_miscellaneous_problems_request_load($problem_id, TRUE);
  if (empty($problem_results)) {
    $result = FALSE;
  }
  else {
    $problem = array_pop($problem_results);
    $result = mfcs_request_problems_0_request_perform_recheck($problem->request_id, $problem->problem);
  }

  if ($result !== FALSE) {
    $redirect_array = mfcs_build_redirect_array('requests/problems-0/requests');

    drupal_goto('requests/problems-0/requests', array('query' => drupal_get_query_parameters()));
    return '';
  }

  $markup = '<div>';
  $markup .= 'Failed to re-check problem.<br>';
  $markup .= 'You may go back to the <a href="' . $base_path . 'requests/problems-0/requests">request problems management page</a>.';
  $markup .= '</div>';

  return $markup;
}

/**
 * Given a request and problem code, re-performs the appropriate check.
 *
 * This will remove any existing entries in the request problems database if the problem fails.
 * This assumes that the problem was previously detected and does not auto-add the problem back.
 *
 * @param int $request_id
 *   The id of the request to perform re-check for problems.
 * @param int $problem_code
 *   The code of the specific problem to re-check.
 *
 * @param bool|null
 *   TRUE on success.
 *   NULL is returned when the problem still exists, but there was no problem in the re-check process.
 *   FALSE is returned on failure.
 */
function mfcs_request_problems_0_request_perform_recheck($request_id, $problem_code) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  if (!cf_is_integer($problem_code)) {
    cf_error::invalid_integer('problem_code');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $user = cf_current_user();
  $instance = mfcs_instance();

  $transaction = db_transaction();
  try {
    $fields = array(
      'code' => MFCS_LOG_PROBLEMS_RECHECK,
      'request_id' => $request_id,
      'user_id' => $user->uid,
      'date' => $instance,
    );

    $query = db_insert('mfcs_log_problems_requests');
    $query->fields($fields);
    $query->execute();

    // enforce transaction execution.
    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);
  }

  $results = mfcs_miscellaneous_check_requests($request_id, $problem_code);
  if ($results === FALSE) {
    return FALSE;
  }

  // when the results are empty, then the problem no longer exists and can be removed from the database.
  if (empty($results)) {
    $transaction = db_transaction();
    try {
      $return_to_problem_page = FALSE;
      $query = db_delete('mfcs_problems_requests');

      $query->condition('request_id', $request_id);
      $query->condition('problem', $problem_code);

      $query->execute();

      // enforce transaction execution.
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      return FALSE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      return FALSE;
    }
  }
  else {
    return NULL;
  }

  return TRUE;
}

/**
 * Rebuild the requests cache table.
 *
 * The main table mfcs_requests essentially contains cached data.
 * Should something happen for that cached data to become inconsistent, this can be used to manually re-sync the cache.
 *
 * @param int $request_id
 *   The id of the request to recache.
 *
 * @param bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_request_problem_0_request_perform_recache($request_id) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  try {
    $query = db_select('mfcs_current', 'mc');
    $query->innerJoin('mfcs_field_top_status', 'mfts', 'mc.request_id = mfts.request_id and mc.top_status = mfts.revision');
    $query->innerJoin('mfcs_field_top_step', 'mftsp', 'mc.request_id = mftsp.request_id and mc.top_step = mftsp.revision');
    $query->innerJoin('mfcs_field_information_type', 'mfit', 'mc.request_id = mfit.request_id and mc.information_type = mfit.revision');
    $query->innerJoin('mfcs_field_request_coordinator_classification', 'mfrcc', 'mc.request_id = mfrcc.request_id and mc.request_coordinator_classification = mfrcc.revision');
    $query->innerJoin('mfcs_field_top_location', 'mftl', 'mc.request_id = mftl.request_id and mc.top_location = mftl.revision');
    $query->innerJoin('mfcs_field_top_building', 'mftb', 'mc.request_id = mftb.request_id and mc.top_building = mftb.revision');
    $query->innerJoin('mfcs_field_top_room', 'mftr', 'mc.request_id = mftr.request_id and mc.top_building = mftr.revision');
    $query->innerJoin('mfcs_field_venue_coordinator_user_id', 'mfvcui', 'mc.request_id = mfvcui.request_id and mc.venue_coordinator_user_id = mfvcui.revision');
    $query->innerJoin('mfcs_field_information_title', 'mfite', 'mc.request_id = mfite.request_id and mc.information_title = mfite.revision');

    $query->addField('mfts', 'value', 'status');
    $query->addField('mftsp', 'value', 'step');
    $query->addField('mfit', 'value', 'type');
    $query->addField('mfrcc', 'value', 'classification');
    $query->addField('mftl', 'value', 'location');
    $query->addField('mftb', 'value', 'building');
    $query->addField('mftr', 'value', 'room');
    $query->addField('mfvcui', 'value', 'venue_coordinator');
    $query->addField('mfite', 'value', 'title');

    $query->condition('mc.request_id', $request_id);

    $latest = $query->execute()->fetchObject();

    if (!is_object($latest)) {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to perform a recache on the request: @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
      return FALSE;
    }

    $query = db_update('mfcs_requests');
    $query->fields((array)$latest);
    $query->condition('id', $request_id);
    $query->execute();
  }
  catch (Error $e) {
    cf_error::on_query_execution($e);

    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);

    return FALSE;
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
