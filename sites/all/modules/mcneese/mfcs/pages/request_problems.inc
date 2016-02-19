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

  $manager = user_access('mfcs manage', $user);
  $administer = user_access('mfcs administer', $user);

  $items_1 = array();
  $items_2 = array();

  $items_array = &$items_1;
  $items_array_current = 0;

  if ($manager) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/problems-0/users' . $url_arguments . '" class="item-link">User Problems</a></h4>';
    $help = '<div class="item-description inline-block">Manage what users are allowed to approve/deny requests by request type.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

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

  $page_title = "Facilities Use Problems";
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
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(6 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    else {
      $sorting['user_id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['user_id']));
      $class_user_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 20;
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

  $taxonomy_problems = mfcs_management_get_problems_taxonomy();
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
          $additional_operations .= " <a href=\"" . $base_path . 'requests/problems-0/users/disabled_in_use/' . $problem->user_id . "\">Requests</a>";
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

    // @todo: add date search

    $row['account_id'] = array(
      'data' => $problem->user_id,
      'class' => $class_user_id,
    );

    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'user/' . $problem->user_id . $url_arguments . '" class="user-link">' . check_plain($problem->name) . '</a>' . ' <span class="user-id script-hidden">[id: ' .  $problem->user_id . ']</span>',
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

    $row['date'] = array(
      'data' => check_plain(date("Y/m/d h:i:s a", $problem->date)),
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
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
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
    'ajax' => "'" . $base_path . 'requests/problems-0/users/ajax' . $url_arguments . "'",
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'account_id', 'name' => "Account ID", 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 1, 'column' => 'account', 'name' => "Account", 'className' => 'column-account'),
      array('id' => 2, 'column' => 'first_name', 'name' => "First Name", 'className' => 'column-first_name'),
      array('id' => 3, 'column' => 'last_name', 'name' => "Last Name", 'className' => 'column-last_name'),
      array('id' => 4, 'column' => 'problem_account', 'name' => "Problems Account", 'className' => 'column-problem_account'),
      array('id' => 5, 'column' => 'problem_forms', 'name' => "Problems Forms", 'className' => 'column-problem_forms'),
      array('id' => 6, 'column' => 'problem_other', 'name' => "Problems Other", 'className' => 'column-problem_other'),
      array('id' => 7, 'column' => 'date', 'name' => "Date", 'className' => 'column-date'),
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

  $navigation_bottom .= '<div class="table_listing-comment">' . "\n";
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $problems_total . " entries.\n";
  $navigation_bottom .= '</div>' . "\n";

  if ($problems_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['problem_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['problem_list']['#markup'] .= mcneese_libraries_theme_table($problem_list, $datatable_settings);
  $form['problem_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

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
    return array();
  }

  $mfcs_users = mfcs_load_users($problem_user_id, FALSE, TRUE);
  if (!is_array($mfcs_users) || !array_key_exists($problem_user_id, $mfcs_users)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $mfcs_user = $mfcs_users[$problem_user_id];
  unset($mfcs_users);
  $problem_user = user_load($problem_user_id);

  global $user;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/problems-0/users/user/' . $problem_user_id);

  $user_name = $problem_user->name;
  if (!empty($problem_user->field_user_first_name['und'][0]['safe_value']) && !empty($problem_user->field_user_last_name['und'][0]['safe_value'])) {
    $user_name = $problem_user->field_user_first_name['und'][0]['safe_value'] . ' ' . $problem_user->field_user_last_name['und'][0]['safe_value'];
  }

  $banner_id = NULL;
  if (!empty($problem_user->field_user_banner_id['und'][0]['value']) && is_numeric($problem_user->field_user_banner_id['und'][0]['value'])) {
    $banner_id = (int) $problem_user->field_user_banner_id['und'][0]['value'];
  }

  $page_title = "Facilities Use Problems: " . $user_name . " [id: " . $problem_user_id . "]";
  drupal_set_title($page_title);

  $user_is_manager = user_access('mfcs manage', $user);
  $user_is_administer = user_access('mfcs administer', $user);


  $form = array();
  $form['values'] = array(
    '#tree' => TRUE,
  );

  $form['values']['mfcs_user'] = array(
    '#type' => 'value',
    '#value' => $mfcs_user,
  );

  $form['values']['problem_user'] = array(
    '#type' => 'value',
    '#value' => $problem_user,
  );

  $form['values']['problem_list'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $message = "The system has detected problems for <strong>" . $user_name . "</strong>.<br>\n";
  $message .= "This page provides details of the problems and the ability to reset the problem flags for each of these problems.<br>\n";
  $message .= "Some of these problems can be reset, others cannot.<br>\n";
  $message .= "<br>\n";
  $message .= "The <a href=\"" . $base_path . 'user/' . $problem_user_id . "\" title=\"View Profile for " . $user_name . " (id #" . $problem_user_id . ")\">user's information</a> and problem flags were last updated on: <strong>" . date("Y/m/d h:i:s a", $mfcs_user->date) . "</strong>.<br>\n";
  $message .= "<br>\n";

  $form['message'] = array(
    '#markup' => '<div class="problem-message">' . $message . '</div>' . "\n",
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

  if (isset($problem_user->field_user_banner_id['und']['0']['value'])) {
    try {
      $event_roles = mfcs_oracle_select('select * from ucs.msu_event_role where id = :banner_id', array('banner_id' => $problem_user->field_user_banner_id['und']['0']['value']));

      if (is_array($event_roles)) {
        $form['banner_information']['details']['#markup'] .= '<strong>Banner Roles</strong>:' . "\n";
        $form['banner_information']['details']['#markup'] .= '<ul>' . "\n";

        if (!empty($event_roles)) {
          foreach ($event_roles as $event_role) {
            $form['banner_information']['details']['#markup'] .= '<li>' . $event_role['role'] . '</li>' . "\n";
          }
        }

        $form['banner_information']['details']['#markup'] .= '</ul>' . "\n";
      }
      else {
        $form['banner_information']['details']['#markup'] .= 'Banner information is currently unavailable.';
      }
    }
    catch (Exception $ex) {
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
      'label' => "Generic System Related Problems",
      'explanation' =>
        "This flag gets set when a generic system error is detected while the specified user was accessing this system.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_MFCS => array(
      'label' => "Generic Facilities Use Problems",
      'explanation' =>
        "This flag gets set when an error is detected for the facilities use specific functionality while the specified user was accessing this system.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE => array(
      'label' => "Banner Unavailable During Account Creation",
      'explanation' =>
        "The banner system was unavailable during account creation (first login) and the appropriate access controls could not be auto-assigned.<br>\n" .
        "The next time the user logs in this flag will be cleared and the account will load the appropriate banner roles.<br>\n" .
        "The user must log out before the banner information can be updated.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => FALSE,
      'custom_action' => TRUE,
      'custom_action-message' => "Re-synchronize banner information for account."
    ),
    MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE => array(
      'label' => "Banner Unavailable During Login Synchronizeation",
      'explanation' =>
        "The banner system was unavailable during the login process and the account could not be updated if necessary.<br>\n" .
        "This flag will be reset the next time the user logs in and banner is available.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => FALSE,
      'custom_action' => TRUE,
      'custom_action-message' => "Re-synchronize banner information for account."
    ),
    MFCS_USER_PROBLEM_ACCOUNT_LOGIN_PROBLEMS_MULTIPLE => array(
      'label' => "Multiple Login Problems",
      'explanation' =>
        "This flag gets set when a user has multiple problems logging in or multiple failed attempts logging in.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED => array(
      'label' => "Unable to Assign Reviewer Role",
      'explanation' =>
        "This flag gets set when an attempt is made to assign or unassign the reviewer role to a user when there is no reference to that use as a venue coordinator or room coordinator.<br>\n" .
        "<br>\n" .
        "This may be caused by the banner database cache being out of date.<br>\n" .
        "You may need to re-synchronizing the banner cache.<br>\n" .
        "<br>\n" .
        "If the user is not in banner, then ask a administrator to manually grant the review role for this user.<br>\n" .
        "<br>\n" .
        "Administrators, managers, and users without banner ids should not get this error.<br>\n",
      'resettable' => TRUE,
      'custom_action' => TRUE,
      'custom_action-message' => "Re-process Reviewer Role."
    ),
    MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES => array(
      'label' => "Old/Duplicate Account",
      'explanation' =>
        "This flag gets set when a user has logged in and the <strong>LDAP</strong> has the <strong>isDuplicate</strong> flag set to <strong>TRUE</strong> and there is no primary account (or this is not the primary account).<br>\n" .
        "<br>\n" .
        "A specific account must be manually assigned by an administer to designate which account is the primary account.<br>\n",
        "Only the primary account will be allowed access to this system.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_FORM_ERROR_USER => array(
      'label' => "Facilities Use Form Problems",
      'explanation' =>
        "This flag gets set when an error is detected while the users is processing a form.<br>\n" .
        "This could be something as simple as not adding in the date fields or something technical such as a PHP error.<br>\n" .
        "This flag refers to facilities use specific functionality.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_FORM_ERROR_SYSTEM => array(
      'label' => "System Form Problems",
      'explanation' =>
        "This flag gets set when an error is detected while the users is processing a form.<br>\n" .
        "This flag refers to the drupal core form functionality and not the facilities use specific functionality.<br>\n" .
        "<br>\n" .
        "This system currently does not provide the details of these problems, but in the future it may.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE => array(
      'label' => "Deleted/Disabled User Assigned Requests",
      'explanation' =>
        "This flag gets set when a deleted or disabled user account is assigned as a venue coordinator or similar access roles.<br>\n" .
        "<br>\n" .
        "This is generally casued when an account is deleted but <a href=\"" . $base_path . 'requests/problems-0/users/disabled_in_use/' . $problem_user_id . "\">active or on-going requests have not been re-assigned</a>.<br>\n" .
        "<br>\n" .
        "Requests will need to be re-assigned.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID => array(
      'label' => "Invalid Banner ID Assigned to User",
      'explanation' =>
        "This flag gets set when a user has a banner id that is unknown to the system.<br>\n" .
        "<br>\n" .
        "The user either needs to have their banner id fixed, removed, or the account should be disabled.<br>\n",
      'resettable' => FALSE,
      'custom_action' => FALSE,
    ),
    MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS => array(
      'label' => "Insufficient Banner Access",
      'explanation' =>
        "This flag gets set when a user logs into the system but has not been granted any banner roles to perform any actions, but the account is still technically enabled/unlocked.<br>\n" .
        "<br>\n" .
        "Either the account needs to be manually disabled/locked or the account needs to have its banner roles fixed.<br>\n",
        "<br>\n" .
        "This may also be triggered if a new user tries to login while banner is down and there is no locally cached data associated with the new account to properly assign its banner roles.<br>\n",
      'resettable' => TRUE,
      'custom_action' => FALSE,
    ),
  );

  // if the user does not have a banner id, then do not provide banner sync custom action support.
  if (empty($form['values']['problem_user']['#value']->field_user_banner_id['und'][0]['value']) || !is_numeric($form['values']['problem_user']['#value']->field_user_banner_id['und'][0]['value'])) {
    $form['values']['problem_list']['#value'][MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE]['custom_action'] = FALSE;
    $form['values']['problem_list']['#value'][MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE]['custom_action'] = FALSE;
  }

  foreach ($form['values']['problem_list']['#value'] as $problem_id => $problem_settings) {
    if (($mfcs_user->problems & $problem_id) > 0) {
      $form['problems'][$problem_id] = array(
        '#id' => 'problem-fieldset-' . $problem_id,
        '#type' => 'fieldset',
        '#title' => $problem_settings['label'] . " Detected",
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
          '#title' => "Reset " . $problem_settings['label'] . " Flag",
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
        '#markup' => '<div class="problem-explanation problem-explanation-' . $problem_id . '">' . $problem_settings['explanation'] . '</div>' . "\n",
      );

      // jira ticket: fcs-58.
      if ($problem_id == MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES) {
        $all_accounts = mfcs_users_load_all_users_with_banner_id($banner_id);
        if (!empty($all_accounts)) {
          $form['problems'][$problem_id]['explanation']['#markup'] .= "<br>\n";
          $form['problems'][$problem_id]['explanation']['#markup'] .= "The following accounts have the same banner id";

          if ($user_is_manager || $user_is_administer) {
            $form['problems'][$problem_id]['explanation']['#markup'] .= " (<strong>" . check_plain($banner_id) . "</strong>)";
          }

          $form['problems'][$problem_id]['explanation']['#markup'] .= ":\n<ul>\n";

          foreach ($all_accounts as $account_id => $account_name) {
            $form['problems'][$problem_id]['explanation']['#markup'] .= '<li><a href="' . $base_path . 'user/' . $account_id . $url_arguments . '">' . $account_name . '</a></li>' . "\n";
          }
          $form['problems'][$problem_id]['explanation']['#markup'] .= "</ul>\n<br>\n";
        }
      }
    }
  }

  if ($do_something) {
    $form['problems']['reset'] = array(
      '#id' => 'submit-problems-perform_actions',
      '#type' => 'submit',
      '#default_value' => t("Perform Actions / Reset Problems"),
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

  $problems = $form['values']['mfcs_user']['#value']->problems;
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
    $results = mfcs_save_user($form['values']['mfcs_user']['#value']->user_id, array('problems' => $problems));

    if ($results === FALSE) {
      form_set_error('form', "An error occurred while trying to reset the user problem flags. Please contact the support staff.");
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to reset the user problem flags, user: @user_id, problems: @problem_code.", array('@user_id' => $form['values']['mfcs_user']['#value']->user_id, $problems), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;

      return;
    }
  }

  if ($perform_custom_action) {
    foreach ($custom_actions as $custom_action_id => $custom_action_value) {
      if ($custom_action_id == MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE || $custom_action_id == MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE) {
        if (!empty($form['values']['problem_user']['#value']->field_user_banner_id['und'][0]['value']) && is_numeric($form['values']['problem_user']['#value']->field_user_banner_id['und'][0]['value'])) {
          $result = mfcs_synchronize_user_settings($form['values']['problem_user']['#value']);
          if ($result === TRUE) {
            if (($problems & $custom_action_id) != 0) {
              $problems -= $custom_action_id;
              mfcs_save_user($form['values']['problem_user']['#value']->uid, array('problems' => $problems));
            }
          }
          else {
            form_set_error('form', "An error occurred while trying to synchronize the user settings. Please contact the support staff.");
            watchdog(MFCS_WATCHDOG_ID, "An error occurred while trying to synchronize the user settings, user: @user_id, problems: @problem_code.", array('@user_id' => $form['values']['mfcs_user']['#value']->user_id, $problems), WATCHDOG_ERROR);

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

        $changes_made = FALSE;
        $system_roles = (array) $form['values']['problem_user']['#value']->roles;

        // do not alter reviewer role settings for users with 'manager', 'administrator', or 'bannerless_exception' roles.
        $process_reviewer_role = TRUE;
        if (array_key_exists($manager_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }
        elseif (array_key_exists($administrator_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }
        elseif (array_key_exists($bannerless_exception_role_id->rid, $system_roles)) {
          $process_reviewer_role = FALSE;
        }

        if ($process_reviewer_role) {
          $all_coordinators = mfcs_load_room_coordinators(NULL, TRUE);
          if (array_key_exists($form['values']['problem_user']['#value']->uid, $all_coordinators)) {
            if (!array_key_exists($reviewer_role_id->rid, $system_roles)) {
              $system_roles[$reviewer_role_id->rid] = 'reviewer';
              $changes_made = TRUE;
            }
          }
          else {
            if (array_key_exists($reviewer_role_id->rid, $system_roles)) {
              unset($system_roles[$reviewer_role_id->rid]);
              $changes_made = TRUE;
            }
          }
        }

        if ($changes_made) {
          if (!user_save($form['values']['problem_user']['#value'],  array('roles' => $system_roles))) {
            form_set_error('form', "An error occurred while trying to re-assign or re-unassign the manager role. Please contact the support staff.");
            watchdog(MFCS_WATCHDOG_ID, "An error occurred while trying to re-assign or re-unassign the manager role, user: @user_id, problems: @problem_code.", array('@user_id' => $form['values']['mfcs_user']['#value']->user_id, $problems), WATCHDOG_ERROR);

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
 * @para int $user_id
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

  $page_title = "Facilities Use Problems - Resynchronize User";
  drupal_set_title($page_title);

  $result = mfcs_synchronize_user_settings($user);
  if ($result === TRUE) {
    $redirect_array = mfcs_build_redirect_array('requests/problems-0/users');

    drupal_goto('requests/problems-0/users', array('query' => drupal_get_query_parameters()));
    return '';
  }

  $markup = '<div>' . "\n";
  $markup .= 'Failed to re-synchronize user.<br>' . "\n";
  $markup .= 'You may go back to the <a href="' . $base_path . 'requests/problems-0/users">user problems management page</a>.' . "\n";
  $markup .= '</div>' . "\n";

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

  $navigation_bottom .= '<div class="table_listing-comment">' . "\n";
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $count . " entries.\n";
  $navigation_bottom .= '</div>' . "\n";

  if ($count > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/problems-0/users/disabled_in_use/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $output = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $output .= mcneese_libraries_theme_table($table, $table['datatable_settings']);
  $output .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

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

  $banner_id_string = mfcs_banner_id_int_to_string($banner_id);

  $names = array();

  $query = db_query('select u.uid as uid, u.name as name from field_data_field_user_banner_id fdfubi inner join users u on fdfubi.entity_id = u.uid where field_user_banner_id_value = :banner_id', array(':banner_id' => $banner_id_string));
  $results = $query->fetchAll();
  foreach ($results as $result) {
    $names[$result->uid] = $result->name;
  }

  return $names;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
