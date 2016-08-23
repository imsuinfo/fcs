<?php

/**
 * @file
 * Defines McNeese facilities use manager users functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request holiday main page(s).
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   The form array.
 */
function mfcs_request_users_0_form($form, &$form_state, $page_number = NULL) {
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

  mfcs_add_canonical_headers('requests/users-0/list/' . $page_number);

  $user = cf_current_user();
  $is_manager = user_access('mfcs manage', $user);
  $is_administer = user_access('mfcs administer', $user);

  if ($is_manager || $is_administer) {
    $page_title = "Manage Users";
  }
  else {
    $page_title = "View Users";
  }
  drupal_set_title($page_title);

  mfcs_include(MFCS_INCLUDE_MANAGEMENT);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  // load sorting information
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtolower($_GET['sort']);

    if ($sort != 'asc' && $sort != 'desc') {
      $sort = NULL;
    }
  }

  $class_id = '';
  $class_name = '';
  $class_email = '';
  $class_first_name = '';
  $class_last_name = '';
  $class_status = 'no-script-hidden script-hidden ';
  $class_banner_id = 'no-script-hidden script-hidden ';
  $class_ldap_duplicate = 'no-script-hidden script-hidden ';
  $class_primary_account = 'no-script-hidden script-hidden ';
  $class_roles = ' ';
  $class_operations = 'no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = array('direction' => 'desc');
    $order_column = array(0 => $sorting['id']['direction']);
    $class_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(0 => $sorting[$order]['direction']);
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'name') {
      $sorting['name'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(1 => $sorting[$order]['direction']);
      $class_name .= 'sorting_1 ';
    }
    elseif ($order == 'email') {
      $sorting['email'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(2 => $sorting[$order]['direction']);
      $class_email .= 'sorting_1 ';
    }
    elseif ($order == 'first_name') {
      $sorting['first_name'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(3 => $sorting[$order]['direction']);
      $class_fist_name .= 'sorting_1 ';
    }
    elseif ($order == 'last_name') {
      $sorting['last_name'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(4 => $sorting[$order]['direction']);
      $class_last_name .= 'sorting_1 ';
    }
    elseif ($order == 'status') {
      $sorting['status'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(5 => $sorting[$order]['direction']);
      $class_status .= 'sorting_1 ';
    }
    elseif ($order == 'banner_id') {
      $sorting['banner_id'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(6 => $sorting[$order]['direction']);
      $class_banner_id .= 'sorting_1 ';
    }
    elseif ($order == 'ldap_duplicate') {
      $sorting['ldap_duplicate'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(7 => $sorting[$order]['direction']);
      $class_ldap_duplicate .= 'sorting_1 ';
    }
    elseif ($order == 'primary_account') {
      $sorting['primary_account'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(8 => $sorting[$order]['direction']);
      $class_primary_account .= 'sorting_1 ';
    }
    elseif ($order == 'roles') {
      $sorting['roles'] = array('direction' => is_null($sort) ? 'desc' : $sort);
      $order_column = array(9 => $sorting[$order]['direction']);
      $class_roles .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = array('direction' => 'asc');
      $order_column = array(0 => $sorting['id']['direction']);
      $class_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 40;
  $items_start = $page_number * $rows_per_page;
  $locked = FALSE;

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  $load_results = NULL;
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $load_results = FALSE;
  }

  if ($is_manager || $is_administer) {
    $items_returned = mfcs_request_users_get_users('', $sorting, $items_start, $rows_per_page, $locked, $load_results);
  }
  else {
    $user_ids = mfcs_get_related_user_ids_from_user_banner_id($user);
    if (!is_array($user_ids)) {
      $user_ids = array($user->uid => $user->uid);
    }

    $items_returned = mfcs_request_users_get_users('', $sorting, $items_start, $rows_per_page, $locked, $load_results, $user_ids);
    unset($user_ids);
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($items_returned === FALSE) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $items_total = 0;
  if (is_array($items_returned) && array_key_exists('count', $items_returned)) {
    $items_total = $items_returned['count'];
  }

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($items_total > 0 && $items_start >= $items_total || $items_total == 0 && $items_start > $items_total) {
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

    $row['name'] = array(
      'data' => check_plain($item->name),
      'class' => $class_name,
    );

    $row['email'] = array(
      'data' => check_plain($item->email),
      'class' => $class_email,
    );

    $row['first_name'] = array(
      'data' => check_plain($item->first_name),
      'class' => $class_first_name,
    );

    $row['last_name'] = array(
      'data' => check_plain($item->last_name),
      'class' => $class_last_name,
    );

    $row['status'] = array(
      'data' => ($item->status == 0 ? 'Locked' : 'Available'),
      'class' => $class_status,
    );

    $row['banner_id'] = array(
      'data' => check_plain($item->banner_id),
      'class' => $class_banner_id,
    );

    $row['ldap_duplicate'] = array(
      'data' => ($item->ldap_duplicate ? 'Is Duplicate' : 'Not Duplicate'),
      'class' => $class_ldap_duplicate,
    );

    $row['primary_account'] = array(
      'data' => ($item->primary_account ? 'Primary Account' : 'Alternate Account'),
      'class' => $class_primary_account,
    );

    if ($item->roles == '{NULL}') {
      $row['roles'] = array(
        'data' => '',
        'class' => $class_roles,
      );
    }
    else {
      $roles = str_replace('{', '', $item->roles);
      $roles = str_replace('}', '', $roles);
      $roles = str_replace('"', '', $roles);
      $roles = str_replace(',', ', ', $roles);
      $roles = str_replace(' role', '', $roles);
      $row['roles'] = array(
        'data' => check_plain($roles),
        'class' => $class_roles,
      );
      unset($roles);
    }

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    $row['operations']['data'] .= '<a href="' . $base_path . 'requests/users-0/view/' . $item->id . $url_arguments . '" class="operation-link operation-link-view" title="View User">View</a> ';

    if ($is_manager || $is_administer) {
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/users-0/edit/' . $item->id . $url_arguments . '" class="operation-link operation-link-view" title="Edit User">Edit</a>' ;
    }

    $rows[] = $row;
  }


  $users_list = array();

  if ($is_manager || $is_administer) {
    $users_list['caption'] = "Manage Users";
  }
  else {
    $users_list['caption'] = "View Users";
  }

  $users_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'name' => array('field' => 'name', 'data' => "Name", 'align' => 'center'),
    'email' => array('field' => 'email', 'data' => "E-Mail", 'align' => 'center', 'class' => array('script-hidden')),
    'first_name' => array('field' => 'first_name', 'data' => "First Name", 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => "Last Name", 'align' => 'center'),
    'status' => array('field' => 'status', 'data' => "Status", 'align' => 'center', 'class' => array('script-hidden')),
    'banner_id' => array('field' => 'banner_id', 'data' => "Banner ID", 'align' => 'center', 'class' => array('script-hidden')),
    'ldap_duplicate' => array('field' => 'ldap_duplicate', 'data' => "LDAP Duplicate", 'align' => 'center', 'class' => array('script-hidden')),
    'primary_account' => array('field' => 'primary_account', 'data' => "Primary Account", 'align' => 'center', 'class' => array('script-hidden')),
    'roles' => array('field' => 'roles', 'data' => "Roles", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $users_list['attributes'] = array();
  $users_list['attributes']['id'] = 'mfcs-manage_users_form';
  $users_list['attributes']['class'] = array(
    'no_default',
    'mfcs-manage_users_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $users_list['attributes']['summary'] = 'List of users you are able to manage.';

  $users_list['rows'] = $rows;

  if ($is_manager || $is_administer) {
    $users_list['empty'] = t("There are no user accounts available for you to manage.");
  }
  else {
    $users_list['empty'] = t("There are no user accounts available for you to view.");
  }

  $datatable_settings = array(
    'sort' => $sort,
    'order' => $order_column,
    #'stateSave' => TRUE,
    'lengthMenu' => array(
       5 => '5',
      10 => '10',
      20 => '20',
      40 => '40',
      80 => '80',
      100 => '100',
      -1 => '"All"',
    ),
    'processing' => TRUE,
    'serverSide' => TRUE,
    'ajax' => array(
      'url' => "'" . $base_path . 'requests/users-0/list/ajax' . $url_arguments . "'",
      'type' => "'POST'",
    ),
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'name', 'name' => "Name", 'className' => 'column-name'),
      array('id' => 2, 'column' => 'email', 'name' => "E-Mail", 'className' => 'column-email', 'visible' => FALSE),
      array('id' => 3, 'column' => 'first_name', 'name' => "First Name", 'className' => 'column-first_name'),
      array('id' => 4, 'column' => 'last_name', 'name' => "Last Name", 'className' => 'column-last_name'),
      array('id' => 5, 'column' => 'status', 'name' => "Status", 'className' => 'column-status', 'visible' => FALSE),
      array('id' => 6, 'column' => 'banner_id', 'name' => "Banner ID", 'className' => 'column-banner_id', 'visible' => FALSE),
      array('id' => 7, 'column' => 'ldap_duplicate', 'name' => "LDAP Duplicate", 'className' => 'column-ldap_duplicate', 'visible' => FALSE),
      array('id' => 8, 'column' => 'primary_account', 'name' => "Primary Account", 'className' => 'column-primary_account', 'visible' => FALSE),
      array('id' => 9, 'column' => 'roles', 'name' => "Roles", 'className' => 'column-roles'),
      array('id' => 10, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $items_start,
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $users_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $users_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $form = array();

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/users-0/list' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/users-0/list' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/users-0/list/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/users-0/list/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $items_start + 1;
  $navigation_total = $items_start + $rows_per_page;

  if ($items_total == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $items_total) {
    $navigation_total = $items_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $items_total . " entries.";
  $navigation_bottom .= '</div>';

  if ($items_total > ($items_start + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/users-0/list/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a> ';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/users-0/list/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a> ';
  }

  $form = array();
  $form['manage_users_list']['#markup'] = '';
  #$form['manage_users_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['manage_users_list']['#markup'] .= mcneese_libraries_theme_table($users_list, $datatable_settings);
  #$form['manage_users_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  if ($is_manager || $is_administer) {
    $form['manage_users_list']['#markup'] .= '<div class="mfcs-manage_users_form-add_user no-print">';
    $form['manage_users_list']['#markup'] .= '<a href="' . $base_path . 'requests/users-0/add" title="Add new user">Add User</a>';
    $form['manage_users_list']['#markup'] .= '</div>';
  }

  return $form;
}

/**
 * Provides a view for a given user.
 *
 * @param int $user_id
 *   The id of the user to view.
 *
 * @return string
 *   The HTML markup to be renderred.
 */
function mfcs_request_users_0_view_page($user_id) {
  if (!cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');

    drupal_not_found();
    drupal_exit();
  }

  $account = user_load($user_id);
  if (!is_object($account)) {
    drupal_not_found();
    drupal_exit();
  }

  $user = cf_current_user();
  $is_manager = user_access('mfcs manage', $user);
  $is_administer = user_access('mfcs administer', $user);

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }
  mfcs_add_canonical_headers('requests/users-0/view/' . $user_id);


  $name = $account->name;
  if (!empty($account->field_user_first_name['und'][0]['value'])) {
    $name = $account->field_user_first_name['und'][0]['value'];

    if (!empty($account->field_user_last_name['und'][0]['value'])) {
      $name .= ' ' . $account->field_user_last_name['und'][0]['value'];
    }

    $name .= ' (' . $account->name . ')';
  }

  drupal_set_title("View User: " . $name);

  $name = NULL;
  if (!empty($account->field_user_first_name['und'][0]['safe_value'])) {
    $name .= $account->field_user_first_name['und'][0]['safe_value'];
  }

  if (!empty($account->field_user_last_name['und'][0]['safe_value'])) {
    if (!is_null($name)) {
      $name .= ' ';
    }
    $name .= $account->field_user_last_name['und'][0]['safe_value'];
  }

  $banner_id = '';
  if (!empty($account->field_user_banner_id['und'][0]['safe_value'])) {
    $banner_id = $account->field_user_banner_id['und'][0]['safe_value'];
  }

  $phone = '';
  if (!empty($account->field_user_phone_number['und'][0]['safe_value'])) {
    $phone = $account->field_user_phone_number['und'][0]['safe_value'];
  }

  $roles_basic = mfcs_request_users_get_roles(TRUE, FALSE, FALSE, FALSE);
  $roles_venue = mfcs_request_users_get_roles(FALSE, TRUE, FALSE, FALSE);
  $roles_special = mfcs_request_users_get_roles(FALSE, FALSE, TRUE, FALSE);
  $roles_administer = mfcs_request_users_get_roles(FALSE, FALSE, FALSE, TRUE);

  $bannerless_exception_role_id = array_search('bannerless exception', $roles_special);

  asort($roles_basic);
  asort($roles_venue);
  asort($roles_special);
  asort($roles_administer);

  $basic = NULL;
  foreach ($roles_basic as $role_id => $role) {
    if (array_key_exists($role_id, $account->roles)) {
      $basic .= '<div class="view_user-item view_user-item-roles-basic"><span class="item-information item-information-roles-basic">' . $role . '</span></div>';
    }
  }
  unset($role);

  if (is_null($basic)) {
    $basic = '<div class="view_user-item view_user-item-roles-basic"><span class="item-information item-information-roles-basic">No roles assigned</span></div>';
  }

  $venue = NULL;
  foreach ($roles_venue as $role_id => $role) {
    if (array_key_exists($role_id, $account->roles)) {
      $venue .= '<div class="view_user-item view_user-item-roles-venue"><span class="item-information item-information-roles-venue">' . str_replace(' role', '', $role) . '</span></div>';
    }
  }
  unset($role);
  if (is_null($venue)) {
    $venue = '<div class="view_user-item view_user-item-roles-venue"><span class="item-information item-information-roles-venue">No roles assigned</span></div>';
  }

  $special = NULL;
  foreach ($roles_special as $role_id => $role) {
    if (array_key_exists($role_id, $account->roles)) {
      $special .= '<div class="view_user-item view_user-item-roles-special"><span class="item-information item-information-roles-special">' . $role . '</span></div>';
    }
  }
  unset($role);
  if (is_null($special)) {
    $special = '<div class="view_user-item view_user-item-roles-special"><span class="item-information item-information-roles-special">No roles assigned</span></div>';
  }

  $administer_text = NULL;
  foreach ($roles_administer as $role_id => $role) {
    if (array_key_exists($role_id, $account->roles)) {
      $administer_text .= '<div class="view_user-item view_user-item-roles-administer"><span class="item-information item-information-roles-administer">' . $role . '</span></div>';
    }
  }
  unset($role);
  if (is_null($administer_text)) {
    $administer_text = '<div class="view_user-item view_user-item-roles-administer"><span class="item-information item-information-roles-administer">No roles assigned</span></div>';
  }

  $primary = 'This is the <strong>Primary</strong> account.';
  if (isset($account->field_user_primary_account['und'][0]['value']) && $account->field_user_primary_account['und'][0]['value'] == 0) {
    $primary = 'This is an <strong>Alternative</strong> account.';
  }

  $locked = 'Account is <strong>Enabled</strong>.';
  if ($account->status == 0) {
    $locked = 'Account is <strong>Locked</strong>.';
  }

  $duplicate = NULL;
  if (isset($account->field_user_ldap_duplicate['und'][0]['value'])) {
    if ($account->field_user_ldap_duplicate['und'][0]['value'] == 1 || $account->field_user_ldap_duplicate['und'][0]['value'] == 'true' || $account->field_user_ldap_duplicate['und'][0]['value'] == 'TRUE') {
      $duplicate = 'There are <strong>Duplicate</strong> accounts for this user.';
    }
  }

  $email = NULL;
  if (isset($account->mail)) {
    $email = check_plain($account->mail);
  }

  $banner_information = '';
  if (isset($account->field_user_banner_id['und']['0']['value'])) {
    try {
      $event_roles = mfcs_oracle_select('select role from ucs.msu_event_role where id = :banner_id', array('banner_id' => $account->field_user_banner_id['und']['0']['value']));

      if (is_array($event_roles)) {
        if (empty($event_roles)) {
          $banner_information = '<div class="view_user-item view_user-item-roles-unavailable">No banner roles found.</div>';
        }
        else {
          foreach ($event_roles as $event_role) {
            $banner_information .= '<div class="view_user-item view_user-item-banner_role"><span class="item-information item-information-banner_role">' . $event_role['role'] . '</span></div>';
          }
          unset($event_role);
        }
      }
      else {
        $banner_information = '<div class="view_user-item view_user-item-roles-unavailable">Banner information is currently unavailable.</div>';
      }
    }
    catch (Error $e) {
      $banner_information = '<div class="view_user-item view_user-item-roles-unavailable">Banner information is currently unavailable.</div>';
    }
    catch (Exception $e) {
      $banner_information = '<div class="view_user-item view_user-item-roles-unavailable">Banner information is currently unavailable.</div>';
    }
  }
  else {
    $banner_information = '<div class="view_user-item view_user-item-roles-no_banner_id">No Banner ID assigned to this account.</div>';
  }

  $proxy_information = NULL;
  $proxy_accounts = mfcs_get_proxy_venue_coordinators(array('mpvc.proxy_id' => $account->uid));
  if (!empty($proxy_accounts['results'])) {
    $name_information = NULL;
    foreach ($proxy_accounts['results'] as $proxy_account) {
      if (!isset($proxy_account->user_id)) continue;
      $proxy_user = user_load($proxy_account->user_id);

      if (is_object($proxy_user)) {
        $generated_name = mfcs_generate_form_user_name($proxy_user);

        if ($is_manager || $is_administer) {
          $name_information .= '<div class="view_user-item-proxy-item view_user-item-proxy-as-item"><a href="' . $base_path . 'requests/users-0/view/' . $proxy_user->uid . $url_arguments . '" class="view_user-item-proxy-as-item no_print_links_as_link" title="View user information">' . $generated_name . '</a></div>';
        }
        else {
          $name_information .= '<div class="view_user-item-proxy-item view_user-item-proxy-as-item">' . $generated_name . '</div>';
        }
      }
    }
    unset($proxy_account);
    unset($proxy_user);

    if (!is_null($name_information)) {
      $proxy_information .= '<div class="view_user-item view_user-item-proxy-as">';
      $proxy_information .= '<div class="view_user-item-label view_user-item-proxy-as-label">May Proxy As:</div>';
      $proxy_information .= $name_information;
      $proxy_information .= '</div>';
    }
    unset($name_information);
  }

  $proxy_accounts = mfcs_get_proxy_venue_coordinators(array('mpvc.user_id' => $account->uid));
  if (!empty($proxy_accounts['results'])) {
    $name_information = NULL;
    foreach ($proxy_accounts['results'] as $proxy_account) {
      if (!isset($proxy_account->proxy_id)) continue;
      $proxy_user = user_load($proxy_account->proxy_id);

      if (is_object($proxy_user)) {
        $generated_name = mfcs_generate_form_user_name($proxy_user);

        if ($is_manager || $is_administer) {
          $name_information .= '<div class="view_user-item-proxy-item view_user-item-proxy-by-item"><a href="' . $base_path . 'requests/users-0/view/' . $proxy_user->uid . $url_arguments . '" class="view_user-item-proxy-by-item no_print_links_as_link" title="View user information">' . $generated_name . '</a></div>';
        }
        else {
          $name_information .= '<div class="view_user-item-proxy-item view_user-item-proxy-by-item">' . $generated_name . '</div>';
        }
      }
    }
    unset($proxy_account);
    unset($proxy_user);

    if (!is_null($name_information)) {
      if (!is_null($proxy_information)) {
        $proxy_information .= '<br>';
      }

      $proxy_information .= '<div class="view_user-item view_user-item-proxy-by">';
      $proxy_information .= '<div class="view_user-item-label view_user-item-proxy-by-label">Is Proxied By:</div>';
      $proxy_information .= $name_information;
      $proxy_information .= '</div>';
    }
    unset($name_information);
  }

  if (is_null($proxy_information)) {
    $proxy_information = '<div class="view_user-item view_user-item-proxy-none">Not a proxy and has no proxies.</div>';
  }

  // drupal only stores the account creation, so a custom field has been added to store when the account was last changed.
  // because it is non-standard, fallback to the account creation date in case the update data is not present.
  $updated = $account->created;
  if (isset($account->data['mfcs_updated']) && $account->data['mfcs_updated'] > $account->created) {
    $updated = $account->data['mfcs_updated'];
  }


  // build presentation.
  $markup = '<div id="mfcs-request-users-view-page" class="manage-users_0-view">';

  $markup .= '<div class="view_user-group view_user-group-personal">';
  $markup .= '<h3 class="view_user-group-header view_user-group-header-personal">Personal Information</h3>';
  $markup .= '<div class="view_user-item view_user-item-user_id"><span class="item-label item-label-user_id strong">User ID:</span> <span class="item-value item-value-user_id">' . $account->uid . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-account"><span class="item-label item-label-account strong">Account:</span> <span class="item-value item-value-account">' . $account->name . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-name"><span class="item-label item-label-name strong">Name:</span> <span class="item-value item-value-name">' . $name . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-email"><span class="item-label item-label-email strong">E-mail:</span> <span class="item-value item-value-email">' . $email . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-banner_id"><span class="item-label item-label-banner_id strong">Banner ID:</span> <span class="item-value item-value-banner_id">' . $banner_id . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-phone"><span class="item-label item-label-phone strong">Phone:</span> <span class="item-value item-value-phone">' . $phone . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-updated_on"><span class="item-label item-label-updated_on strong">Updated On:</span> <span class="item-value item-value-updated_on">' . date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $updated) . '</span></div>';
  $markup .= '</div>';

  $markup .= '<div class="view_user-group view_user-group-status">';
  $markup .= '<h3 class="view_user-group-header view_user-group-header-status">Status Information</h3>';
  $markup .= '<div class="view_user-item view_user-item-primary"><span class="item-information item-information-primary">' . $primary . '</span></div>';
  $markup .= '<div class="view_user-item view_user-item-locked"><span class="item-information item-information-locked">' . $locked . '</span></div>';

  if (!is_null($duplicate)) {
    $markup .= '<div class="view_user-item view_user-item-duplicate"><span class="item-information item-information-duplicate">' . $duplicate . '</span></div>';
  }

  if ($is_administer) {
    $markup .= '<div class="view_user-item view_user-item-view_profile no-print"><span class="item-information item-information-view_profile"><a href="' . $base_path . 'user/' . $account->uid . '">' . "View User Profile" . '</a></span></div>';
  }

  $markup .= '</div>';

  if ($is_manager || $is_administer) {
    $markup .= '<div class="view_user-group view_user-group-banner_information">';
    $markup .= '<h3 class="view_user-group-header view_user-group-header-banner_information">Banner Information</h3>';
    $markup .= $banner_information;
    $markup .= '</div>';
  }

  $markup .= '<div class="view_user-group view_user-group-proxy_information">';
  $markup .= '<h3 class="view_user-group-header view_user-group-header-proxy_information">Proxy Information</h3>';
  $markup .= $proxy_information;
  $markup .= '</div>';

  $markup .= '<br>';

  $markup .= '<div class="view_user-group view_user-group-roles-basic">';
  $markup .= '<h3 class="view_user-group-header view_user-group-header-roles-basic">Basic Roles</h3>';
  $markup .= $basic;
  $markup .= '</div>';

  $markup .= '<div class="view_user-group view_user-group-roles-venue">';
  $markup .= '<h3 class="view_user-group-header view_user-group-header-roles-venue">Venue-Specific Roles</h3>';
  $markup .= $venue;
  $markup .= '</div>';

  if ($is_manager || $is_administer) {
    $markup .= '<div class="view_user-group view_user-group-roles-special">';
    $markup .= '<h3 class="view_user-group-header view_user-group-header-roles-special">Special/Exceptional Roles</h3>';
    $markup .= $special;
    $markup .= '</div>';

    $markup .= '<div class="view_user-group view_user-group-roles-administer">';
    $markup .= '<h3 class="view_user-group-header view_user-group-header-roles-administer">Administration Roles</h3>';
    $markup .= $administer_text;
    $markup .= '</div>';

    // load any problems associated with the given user.
    $problems_detected = NULL;

    $mfcs_users = mfcs_load_users($account->uid, FALSE, TRUE);
    if (is_array($mfcs_users) && array_key_exists($account->uid, $mfcs_users) && !empty($mfcs_users[$account->uid])) {
      mfcs_include(MFCS_INCLUDE_MANAGEMENT);
      $problems_list = mfcs_management_get_problems_user_taxonomy();

      foreach ($problems_list as $problem_id => $problem_text) {
        if (($mfcs_users[$account->uid]->problems & $problem_id) != 0) {
          $problems_detected .= '<div class="view_user-item view_user-item-problems"><a href="' . $base_path . 'requests/problems-0/users/user/' . $account->uid . $url_arguments . '" class="problem problem-' . ((int) $problem_id) . ' no_print_links_as_link">' . $problem_text . '</a></div>';
        }
      }
      unset($problem_text);
      unset($problem_id);
    }
    unset($mfcs_users);

    if (!is_null($problems_detected)) {
      $markup .= '<br>';
      $markup .= '<div class="view_user-group view_user-group-problems">';
      $markup .= '<h3 class="view_user-group-header view_user-group-header-problems">Problems Detected</h3>';
      $markup .= $problems_detected;
      $markup .= '</div>';
    }
    unset($problems_detected);

    $markup .= '<div class="view_user-buttons">';
    $markup .= '<a href="' . $base_path . 'requests/users-0/edit/' . $user_id . $url_arguments . '" class="view_user-button view_user-edit">Edit</a>' ;
    $markup .= '</div>';
  }

  $markup .= '</div>';

  return $markup;
}

/**
 * Provides the ability to manually add new users.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @return array
 *   The form array.
 */
function mfcs_request_users_0_add_form($form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form($form, $form_state);
}

/**
 * Pre-Validation for mfcs_request_users_0_add_Form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_add_Form()
 */
function mfcs_request_users_0_add_Form_pre_validate(&$form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form_pre_validate($form, $form_state);
}

/**
 * Validation for mfcs_request_users_0_add_Form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_add_Form()
 */
function mfcs_request_users_0_add_Form_validate($form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form_validate($form, $form_state);
}

/**
 * Submit for mfcs_request_users_0_add_Form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_add_Form()
 */
function mfcs_request_users_0_add_Form_submit($form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form_submit($form, $form_state);
}

/**
 * Provides the ability to manually add new users.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $user_id
 *   When specified, this an edit form is created.
 *   Otherwise, an add form is created.
 *
 * @return array
 *   The form array.
 */
function mfcs_request_users_0_edit_form($form, &$form_state, $user_id) {
  // The edit form requires the user id, because the add_or_edit function treats this as optional, force it to be required.
  if (is_null($user_id)) {
    cf_error::invalid_array('user_id');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  return mfcs_request_users_0_add_or_edit_form($form, $form_state, $user_id);
}

/**
 * Pre-Validation for mfcs_request_users_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_edit_form()
 */
function mfcs_request_users_0_edit_form_pre_validate(&$form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form_pre_validate($form, $form_state, TRUE);
}

/**
 * Validation for mfcs_request_users_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_edit_form()
 */
function mfcs_request_users_0_edit_form_validate($form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form_validate($form, $form_state, TRUE);
}

/**
 * Submit for mfcs_request_users_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_edit_form()
 */
function mfcs_request_users_0_edit_form_submit($form, &$form_state) {
  return mfcs_request_users_0_add_or_edit_form_submit($form, $form_state, TRUE);
}

/**
 * Provides the ability to manually add new users.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $user_id
 *   (optional) When specified, this an edit form is created.
 *   Otherwise, an add form is created.
 *
 * @return array
 *   The form array.
 */
function mfcs_request_users_0_add_or_edit_form($form, &$form_state, $user_id = NULL) {
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

  if (!is_null($user_id) && !cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $roles_basic = mfcs_request_users_get_roles(TRUE, FALSE, FALSE, FALSE);
  $roles_venue = mfcs_request_users_get_roles(FALSE, TRUE, FALSE, FALSE);
  $roles_special = mfcs_request_users_get_roles(FALSE, FALSE, TRUE, FALSE);
  $roles_administer = mfcs_request_users_get_roles(FALSE, FALSE, FALSE, TRUE);

  $bannerless_exception_role_id = array_search('bannerless exception', $roles_special);

  asort($roles_basic);
  asort($roles_venue);
  asort($roles_special);
  asort($roles_administer);

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $account = NULL;
  if (is_null($user_id)) {
    mfcs_add_canonical_headers('requests/users-0/add');

    drupal_set_title("Add User");
  }
  else {
    $account = user_load($user_id);
    mfcs_add_canonical_headers('requests/users-0/add');

    $user_name = $account->name;
    if (!empty($account->field_user_first_name['und'][0]['value'])) {
      $user_name = $account->field_user_first_name['und'][0]['value'];

      if (!empty($account->field_user_last_name['und'][0]['value'])) {
        $user_name .= ' ' . $account->field_user_last_name['und'][0]['value'];
      }

      $user_name .= ' (' . $account->name . ')';
    }

    drupal_set_title("Edit User: " . $user_name);
  }

  $form['roles'] = array(
    '#tree' => TRUE,
  );

  $form['roles']['basic'] = array(
    '#type' => 'value',
    '#value' => $roles_basic,
  );

  $form['roles']['venue'] = array(
    '#type' => 'value',
    '#value' => $roles_venue,
  );

  $form['roles']['special'] = array(
    '#type' => 'value',
    '#value' => $roles_special,
  );

  $form['roles']['administer'] = array(
    '#type' => 'value',
    '#value' => $roles_administer,
  );

  // clean up presentation by not displaying 'role' (which would be redundant).
  foreach ($roles_venue as &$venue_role) {
    $venue_role = str_replace(' role', '', $venue_role);
  }
  unset($venue_role);

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['personal'] = array(
    '#id' => 'manage-users_0-add-fieldset-personal',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => "Personal Information",
    '#attributes' => array(
      'class' => array(
        'manage-users_0-add-fieldset-personal',
        'fieldset_print_as_div',
      ),
    ),
  );

  $form['form']['personal']['account'] = array(
    '#type' => 'textfield',
    '#title' => "Account Name",
    '#default_value' => '',
    '#description' => "This is normally identical to their LDAP/Portal name.",
    '#required' => TRUE,
  );

  $form['form']['personal']['name_first'] = array(
    '#type' => 'textfield',
    '#title' => "First Name",
    '#default_value' => '',
    '#description' => "Unless bannerless exception is specified, this field is auto-populated and can be ignored (values will get overwritten).",
    '#required' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="form[roles][special][' . $bannerless_exception_role_id . ']"]' => array('checked' => TRUE),
      ),
      'invisible' => array(
        ':input[name="form[roles][special][' . $bannerless_exception_role_id . ']"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['form']['personal']['name_last'] = array(
    '#type' => 'textfield',
    '#title' => "Last Name",
    '#default_value' => '',
    '#description' => "Unless bannerless exception is specified, this field is auto-populated and should be ignored (values will get overwritten).",
    '#required' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="form[roles][special][' . $bannerless_exception_role_id . ']"]' => array('checked' => TRUE),
      ),
      'invisible' => array(
        ':input[name="form[roles][special][' . $bannerless_exception_role_id . ']"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['form']['personal']['account'] = array(
    '#type' => 'textfield',
    '#title' => "Account Name",
    '#default_value' => '',
    '#description' => "This is normally identical to their LDAP/Portal name. This cannot be changed once an account is created.",
    '#required' => TRUE,
  );

  $form['form']['personal']['email'] = array(
    '#type' => 'emailfield',
    '#title' => "E-Mail Address",
    '#default_value' => '',
    '#description' => "This must be a McNeese e-mail address. If not specified, then this is auto-populated from the account name.",
    '#required' => FALSE,
  );

  $form['form']['personal']['banner_id'] = array(
    '#type' => 'numberfield',
    '#title' => "Banner ID",
    '#default_value' => '',
    '#min' => 0,
    '#description' => "The Banner ID will be auto-populated unless the user is assigned the <em>Bannerless Exception</em> role.",
    '#required' => TRUE,
  );

  $form['form']['personal']['phone'] = array(
    '#type' => 'telfield',
    '#title' => "Phone Number",
    '#default_value' => '',
    '#required' => FALSE,
  );

  $form['form']['roles'] = array(
    '#id' => 'manage-users_0-add-fieldset-roles',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => "User Roles",
    '#attributes' => array(
      'class' => array(
        'manage-users_0-add-fieldset-roles',
        'fieldset_print_as_div',
      ),
    ),
  );

  $form['form']['roles']['basic'] = array(
    '#type' => 'checkboxes',
    '#title' => "Basic Roles",
    '#default_value' => array(10),
    '#options' => $roles_basic,
    '#description' => "These roles are auto-populated from Banner unless either <em>Manager</em> or <em>Bannerless Exception</em> roles are assigned.",
  );

  $form['form']['roles']['venue'] = array(
    '#type' => 'checkboxes',
    '#title' => "Venue-Specific Roles",
    '#default_value' => array(),
    '#options' => $roles_venue,
    '#description' => "These roles are auto-populated from Banner unless either <em>Manager</em> or <em>Bannerless Exception</em> roles are assigned.",
  );

  $form['form']['roles']['special'] = array(
    '#type' => 'checkboxes',
    '#title' => "Special/Exceptional Roles",
    '#default_value' => array(),
    '#options' => $roles_special,
    '#description' => "Special case roles. For example, if the <em>Bannerless Exception</em> role is specified, then the Venue-Specific Roles are not auto-enforced.",
  );

  $form['form']['roles']['administer'] = array(
    '#type' => 'checkboxes',
    '#title' => "Administration Roles",
    '#default_value' => array(),
    '#options' => $roles_administer,
    '#description' => "These roles are related to administration and management tasks. These may only be applied to users with the <em>Manager</em> role.",
  );

  $form['form']['status'] = array(
    '#id' => 'manage-users_0-add-fieldset-status',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => "User Status",
    '#attributes' => array(
      'class' => array(
        'manage-users_0-add-fieldset-status',
        'fieldset_print_as_div',
      ),
    ),
    '#tree' => TRUE,
  );

  $form['form']['status']['primary'] = array(
    '#type' => 'checkbox',
    '#title' => "Primary Account",
    '#default_value' => TRUE,
    '#description' => "When enabled, this user account is the preferred account that the user is expected to log in as. This only matters when dealing with users who have multiple ldap names (such as when they have name changes or they have both student and non-student accounts).",
  );

  $form['form']['status']['locked'] = array(
    '#type' => 'checkbox',
    '#title' => "Lock Account",
    '#default_value' => FALSE,
    '#description' => "When enabled, the user account will be locked and the user is denied login access to the system.",
  );

  $form['buttons'] = array(
    '#tree' => TRUE,
  );

  $form['buttons']['cancel'] = array(
    '#id' => 'submit-buttons-cancel',
    '#name' => 'submit-buttons-cancel',
    '#type' => 'submit',
    '#default_value' => t("Cancel"),
  );

  $form['buttons']['submit'] = array(
    '#id' => 'submit-buttons-submit',
    '#name' => 'submit-buttons-submit',
    '#type' => 'submit',
    '#default_value' => t("Submit"),
  );

  if (is_object($account)) {
    $form['user'] = array(
      '#type' => 'value',
      '#value' => $account,
    );

    $form['form']['personal']['account']['#default_value'] = $account->name;
    $form['form']['personal']['account']['#readonly'] = TRUE;
    $form['form']['personal']['account']['#required'] = FALSE;

    $form['form']['personal']['email']['#default_value'] = $account->mail;
    $form['form']['personal']['name_first']['#default_value'] = isset($account->field_user_first_name['und'][0]['value']) ? $account->field_user_first_name['und'][0]['value'] : '';
    $form['form']['personal']['name_last']['#default_value'] = isset($account->field_user_last_name['und'][0]['value']) ? $account->field_user_last_name['und'][0]['value'] : '';
    $form['form']['personal']['banner_id']['#default_value'] = isset($account->field_user_banner_id['und'][0]['value']) ? $account->field_user_banner_id['und'][0]['value'] : '';
    $form['form']['personal']['phone']['#default_value'] = isset($account->field_user_phone_number['und'][0]['value']) ? $account->field_user_phone_number['und'][0]['value'] : '';
    $form['form']['status']['primary']['#default_value'] = isset($account->field_user_primary_account['und'][0]['value']) ? $account->field_user_primary_account['und'][0]['value'] : TRUE;
    $form['form']['status']['locked']['#default_value'] = $account->status == 0 ? TRUE : FALSE;

    $form['form']['roles']['basic']['#default_value'] = array();
    $form['form']['roles']['venue']['#default_value'] = array();
    $form['form']['roles']['special']['#default_value'] = array();
    $form['form']['roles']['administer']['#default_value'] = array();

    foreach ($roles_basic as $role_id => $role_name) {
      if (array_key_exists($role_id, $account->roles)) {
        $form['form']['roles']['basic']['#default_value'][$role_id] = $role_id;
      }
    }
    unset($role_id);
    unset($role_name);

    foreach ($roles_venue as $role_id => $role_name) {
      if (array_key_exists($role_id, $account->roles)) {
        $form['form']['roles']['venue']['#default_value'][$role_id] = $role_id;
      }
    }
    unset($role_id);
    unset($role_name);

    foreach ($roles_special as $role_id => $role_name) {
      if (array_key_exists($role_id, $account->roles)) {
        $form['form']['roles']['special']['#default_value'][$role_id] = $role_id;
      }
    }
    unset($role_id);
    unset($role_name);

    foreach ($roles_administer as $role_id => $role_name) {
      if (array_key_exists($role_id, $account->roles)) {
        $form['form']['roles']['administer']['#default_value'][$role_id] = $role_id;
      }
    }
    unset($role_id);
    unset($role_name);
  }

  return $form;
}

/**
 * Pre-Validation for mfcs_request_users_0_add_or_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param bool $edit
 *   (optional) Set to TRUE to operate as an edit form.
 *   Otherwise operate as an add form.
 *
 * @see: mfcs_request_users_0_add_or_edit_form()
 */
function mfcs_request_users_0_add_or_edit_form_pre_validate(&$form, &$form_state, $edit = FALSE) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-buttons-cancel') {
    // no fields should be required for cancelling.
    $form['form']['personal']['account']['#required'] = FALSE;
    $form['form']['personal']['banner_id']['#required'] = FALSE;

    return;
  }

  $banner_id = NULL;
  $user_name = NULL;
  $ldap_user = FALSE;

  if (isset($form_state['values']['form']['personal']['account']) && strlen($form_state['values']['form']['personal']['account']) > 0) {
    $user_name = $form_state['values']['form']['personal']['account'];
    $ldap_user = ldap_servers_get_user_ldap_data($user_name);
  }

  if (isset($form_state['values']['form']['personal']['banner_id'])) {
    $banner_id = (int) $form_state['values']['form']['personal']['banner_id'];
    if ($banner_id < 1) {
      $banner_id = NULL;
    }
  }

  $bannerless_exception_role_id = array_search('bannerless exception', $form['roles']['special']['#value']);

  $bannerless_exception = FALSE;
  if (is_int($bannerless_exception_role_id) && isset($form_state['values']['form']['roles']['special'][$bannerless_exception_role_id]) && $form_state['values']['form']['roles']['special'][$bannerless_exception_role_id] == $bannerless_exception_role_id) {
    $bannerless_exception = TRUE;
  }

  // populate banner id from ldap information if the user name is specified (report any conflicting data).
  if (isset($ldap_user['attr']['employeenumber'][0])) {
    if (is_null($banner_id)) {
      $banner_id = (int) $ldap_user['attr']['employeenumber'][0];
      $form_state['values']['form']['personal']['banner_id'] = '' . $banner_id;
      $form['form']['personal']['banner_id']['#value'] = '' . $banner_id;
    }
    else {
      if (!$bannerless_exception) {
        $ldap_banner_id = (int) $ldap_user['attr']['employeenumber'][0];
        if ($banner_id != $ldap_banner_id) {
          form_set_error('form][personal][banner_id', "The specified Banner ID (" . $banner_id . ") does not match the Banner ID associated with the account '" . $user_name . "' (" . $ldap_banner_id . ")");
        }
      }
    }
  }

  if ($bannerless_exception) {
    $form['form']['personal']['email']['#required'] = TRUE;
    $form['form']['personal']['banner_id']['#required'] = FALSE;
  }
  else {
    // either the account name or the banner id may be specified.
    // if one is specified, then the other can be retrieved.
    if (is_null($user_name)) {
      if ($banner_id > 0) {
        // populate user name from banner id if the user name is not specified but the banner id is specified.
        $results = mfcs_get_ldap_user_name_by_banner_id($banner_id);
        if (is_string($results)) {
          $user_name = $results;

          $form_state['values']['form']['personal']['account'] = $user_name;
          $form['form']['personal']['account']['#value'] = $user_name;
        }
        unset($results);
      }
    }
    else {
      if ($ldap_user === FALSE) {
        form_set_error('form][personal][account', "Cannot " . ($edit ? "edit" : "create") . " the account '" . $user_name . "'. Only valid McNeese Portal or McNeese E-mail accounts may be specified.");
      }
    }
  }

  // auto-populate the e-mail address from the account name if none is specified.
  if (!isset($form_state['values']['form']['personal']['email']) || strlen($form_state['values']['form']['personal']['email']) == 0) {
    if (!is_null($user_name)) {
      $form_state['values']['form']['personal']['email'] = $user_name . '@mcneese.edu';
      $form['form']['personal']['email']['#value'] = $user_name . '@mcneese.edu';
    }
  }

  // only an administrator can modify the administrator role for any given user.
  global $user;
  $is_administer = user_access('mfcs administer', $user);
  $is_manager = user_access('mfcs manage', $user);

  if ($is_administer) {
    if (isset($form['user']['#value']) && is_object($form['user']['#value']) && $form['user']['#value']->uid == $user->uid) {
      $administer_role_id = array_search('administrator', $form['roles']['administer']['#value']);
      if (!isset($form_state['values']['form']['roles']['administer'][$administer_role_id]) || $form_state['values']['form']['roles']['administer'][$administer_role_id] != $administer_role_id) {
        form_set_error('form][roles][administer', "Will not remove administration role from your own account.");
      }
    }
  }
  else {
    $administer_role_id = array_search('administrator', $form['roles']['administer']['#value']);
    if (isset($form['form']['roles']['administer']['#default_value'][$administer_role_id])) {
      if (isset($form_state['values']['form']['roles']['administer'][$administer_role_id]) && $form['form']['roles']['administer']['#default_value'][$administer_role_id] != $form_state['values']['form']['roles']['administer'][$administer_role_id]) {
        form_set_error('form][roles][administer', "Only a system administrator may add/remove the administration role from users.");
      }
    }
    else {
      if (isset($form_state['values']['form']['roles']['administer'][$administer_role_id]) && $form_state['values']['form']['roles']['administer'][$administer_role_id] == $administer_role_id) {
        form_set_error('form][roles][administer', "Only a system administrator may add/remove the administration role from users.");
      }
    }
  }

  // also prevent removal of manager role for a user editing their own account (when that user is not an administrator)
  if (isset($form['user']['#value']) && is_object($form['user']['#value']) && $form['user']['#value']->uid == $user->uid) {
    if (!$is_administer && $is_manager) {
      $manager_role_id = array_search('manager', $form['roles']['basic']['#value']);
      if (!isset($form_state['values']['form']['roles']['basic'][$manager_role_id]) || $form_state['values']['form']['roles']['basic'][$manager_role_id] != $manager_role_id) {
        form_set_error('form][roles][administer', "Will not remove manager role from your own account.");
      }
    }
  }
}

/**
 * Validation for mfcs_request_users_0_add_or_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param bool $edit
 *   (optional) Set to TRUE to operate as an edit form.
 *   Otherwise operate as an add form.
 *
 * @see: mfcs_request_users_0_add_or_edit_form()
 */
function mfcs_request_users_0_add_or_edit_form_validate($form, &$form_state, $edit = FALSE) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-buttons-cancel') {
    // do nothing.
    return;
  }

  $masquerade_role_id = array_search('masquerade', $form['roles']['administer']['#value']);
  $administer_role_id = array_search('administrator', $form['roles']['administer']['#value']);
  $manager_role_id = array_search('manager', $form['roles']['basic']['#value']);

  if (isset($form_state['values']['form']['roles']['administer'][$masquerade_role_id]) && $form_state['values']['form']['roles']['administer'][$masquerade_role_id] > 0) {
    $allow_masquerade_role = FALSE;

    if (isset($form_state['values']['form']['roles']['basic'][$manager_role_id]) && $form_state['values']['form']['roles']['basic'][$manager_role_id] == $manager_role_id) {
      $allow_masquerade_role = TRUE;
    }

    // Let administers be also allowed, but only silently (because this interface does not manage users with the administration role).
    if (isset($form_state['values']['form']['roles']['administer'][$administer_role_id]) && $form_state['values']['form']['roles']['administer'][$administer_role_id] == $administer_role_id) {
      $allow_masquerade_role = TRUE;
    }

    if (!$allow_masquerade_role) {
      form_set_error('form][roles][administer', "The masquerade role may only be given to users with the manager role.");
    }

    unset($allow_masquerade_role);
  }

  // check to see if the account already exists.
  if (!$edit) {
    try {
      $query = db_select('users', 'u');
      $query->addField('u', 'uid', 'id');
      $query->condition('u.name', $form_state['values']['form']['personal']['account']);

      $field = $query->execute()->fetchField();
      unset($query);

      if (!empty($field)) {
        form_set_error('form][personal][account', "The account '" . $form_state['values']['form']['personal']['account'] . "' already exists.");
      }
      unset($field);
    }
    catch (Error $e) {
      cf_error::on_query_execution($e);

      form_set_error('', "An error occured while accessing the database. Please try again. If this problem persists, please contact the technical support staff.");
    }
    catch (Exception $e) {
      cf_error::on_query_execution($e);

      form_set_error('', "An error occured while accessing the database. Please try again. If this problem persists, please contact the technical support staff.");
    }
  }
}

/**
 * Submit for mfcs_request_users_0_add_or_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_users_0_add_or_edit_form()
 */
function mfcs_request_users_0_add_or_edit_form_submit($form, &$form_state, $edit = FALSE) {
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

  // don't redirect on error.
  if ($form_state['submitted'] === FALSE) {
    return;
  }

  $saved_account = FALSE;
  if ($clicked_id == 'submit-buttons-submit') {
    global $base_path;

    $new_account = new stdClass();
    $new_account->name = $form_state['values']['form']['personal']['account'];
    $new_account->mail = $form_state['values']['form']['personal']['email'];
    $new_account->roles = array();

    if (!empty($form_state['values']['form']['roles']['basic'])) {
      foreach ($form_state['values']['form']['roles']['basic'] as $role_id) {
        if ($role_id > 0 && array_key_exists($role_id, $form['roles']['basic']['#value'])) {
          $new_account->roles[$role_id] = $role_id;
        }
      }
      unset($role_id);
    }

    if (!empty($form_state['values']['form']['roles']['venue'])) {
      foreach ($form_state['values']['form']['roles']['venue'] as $role_id) {
        if ($role_id > 0 && array_key_exists($role_id, $form['roles']['venue']['#value'])) {
          $new_account->roles[$role_id] = $role_id;
        }
      }
      unset($role_id);
    }

    if (!empty($form_state['values']['form']['roles']['special'])) {
      foreach ($form_state['values']['form']['roles']['special'] as $role_id) {
        if ($role_id > 0 && array_key_exists($role_id, $form['roles']['special']['#value'])) {
          $new_account->roles[$role_id] = $role_id;
        }
      }
      unset($role_id);
    }

    if (!empty($form_state['values']['form']['roles']['administer'])) {
      foreach ($form_state['values']['form']['roles']['administer'] as $role_id) {
        if ($role_id > 0 && array_key_exists($role_id, $form['roles']['administer']['#value'])) {
          $new_account->roles[$role_id] = $role_id;
        }
      }
      unset($role_id);
    }

    $new_account->status = 1;
    if ($form_state['values']['form']['status']['locked'] == 1) {
      $new_account->status = 0;
    }

    $bannerless_exception_role_id = array_search('bannerless exception', $form['roles']['special']['#value']);
    if (is_int($bannerless_exception_role_id) && array_key_exists($bannerless_exception_role_id,  $new_account->roles)) {
      $new_account->field_user_first_name['und'][0]['value'] = NULL;
      if (isset($form_state['values']['form']['personal']['name_first']) && strlen($form_state['values']['form']['personal']['name_first']) > 0) {
        $new_account->field_user_first_name['und'][0]['value'] = $form_state['values']['form']['personal']['name_first'];
      }

      $new_account->field_user_last_name['und'][0]['value'] = NULL;
      if (isset($form_state['values']['form']['personal']['name_last']) && strlen($form_state['values']['form']['personal']['name_last']) > 0) {
        $new_account->field_user_last_name['und'][0]['value'] = $form_state['values']['form']['personal']['name_last'];
      }
    }

    $new_account->field_user_banner_id['und'][0]['value'] = NULL;
    if (isset($form_state['values']['form']['personal']['banner_id']) && $form_state['values']['form']['personal']['banner_id'] > 0) {
      $new_account->field_user_banner_id['und'][0]['value'] = mfcs_banner_id_int_to_string($form_state['values']['form']['personal']['banner_id']);
    }

    $new_account->field_user_phone_number['und'][0]['value'] = NULL;
    if (isset($form_state['values']['form']['personal']['phone']) && !empty($form_state['values']['form']['personal']['phone'])) {
      $new_account->field_user_phone_number['und'][0]['value'] = $form_state['values']['form']['personal']['phone'];
    }

    $primary_account = TRUE;
    if (isset($form_state['values']['form']['status']['primary'])) {
      if ($form_state['values']['form']['status']['primary']) {
        $primary_account = TRUE;
      }
      else {
        $primary_account = FALSE;
      }
    }

    if ($primary_account) {
      $new_account->field_user_primary_account['und'][0]['value'] = 1;
    }
    else {
      $new_account->field_user_primary_account['und'][0]['value'] = 0;
    }

    if ($edit) {
      if (isset($form['user']['#value']) && is_object($form['user']['#value'])) {
        $new_account->name = $form['user']['#value']->name;

        foreach ($form['user']['#value']->roles as $role_id => $role_name) {
          if (!array_key_exists($role_id, $new_account->roles)) {
            $new_account->roles[$role_id] = 0;
          }
        }

        $updates = (array) $new_account;

        unset($updates['name']);

        if ($new_account->status == $form['user']['#value']->status) {
          unset($updates['status']);
        }

        if ($new_account->mail == $form['user']['#value']->mail) {
          unset($updates['mail']);
        }

        $saved_account = user_save($form['user']['#value'], $updates);
      }
    }
    else {
      $saved_account = user_save($new_account);

      // new accounts need to be manually synchronized.
      if (is_object($saved_account)) {
        mfcs_synchronize_user_settings($saved_account);
      }
    }

    if (is_object($saved_account)) {
      if ($edit) {
        drupal_set_message('Successfully saved user <a href="' . $base_path . 'requests/users-0/view/' . $saved_account->uid . '">' . $saved_account->name . '</a>.');
      }
      else {
        drupal_set_message('Successfully saved user <a href="' . $base_path . 'requests/users-0/view/' . $saved_account->uid . '">' . $saved_account->name . '</a>, generated user id is: ' . $saved_account->uid . '.');
      }
    }
    else {
      form_set_error('', "Failed to save user '" . $updates->name . "'.");
      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;
      return;
    }
  }
  elseif ($clicked_id == 'submit-buttons-cancel') {
    if (isset($form['user']['#value']) && is_object($form['user']['#value'])) {
      $saved_account = $form['user']['#value'];
    }
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (is_object($saved_account)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/users-0/view/' . $saved_account->uid);
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/users-0/list');
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Load all users that can be managed by managers on this system.
 *
 * This is a user_load() function for multiple users specific to the FCS system design.
 * To save time, some values are hard-coded, this will need to be changed at some point.
 *
 * This will perform a count query.
 *
 * Available column names:
 *   id, name, email, banner_id, first_name, last_name, ldap_duplicate, primary_account, roles.
 *
 * @param string|int $search
 *   (optional) A string to search the entire list by, all values are auto-wrapped in % and are treated as ilike searches.
 *   May be an integer to specify loading of a single user by the user id.
 * @param array $order_by
 *   (optional) An array containing rules to perform SQL order by with the following structure:
 *   Each array key is the column name.
 *   Each array value is an array with the following structure:
 *   - 'direction': either 'asc' or 'desc'.
 *   - 'using': the using operator.
 *   - 'nulls': Appends 'NULLS' sql option. Must be either 'first' or 'last'.
* @param int $offset
 *   (optional) The starting row to begin loading results.
 * @param int|null $limit
 *   (optional) When an integer, this represents the total number of rows to select.
 *   When NULL, there is no limit enforced.
 * @param bool|null $locked
 *   (optional) When TRUE, only load users who's accounts are locked/disabled/deleted.
 *   When FALSE, only load users who's accounts are not locked/disabled/deleted.
 *   When NULL, load all users.
 * @param bool|null $results
 *   (optional) When NULL both result and count queries are performed.
 *   When FALSE only count query is performed.
 *   When TRUE only result query is performed.
 * @param array $users
 *   (optional) An array of user ids to limit the search by.
 *   This is intended for non-admin users such that a smaller list is filtered from in addition to the $search argument.
 *
 * @param array|bool
 *   An array containing all rows (each row is an object).
 *   FALSE is returned on error.
 */
function mfcs_request_users_get_users($search = '', $order_by = array(), $offset = 0, $limit = 40, $locked = FALSE, $results = NULL, $users = array()) {
  if (!is_string($search) && !is_int($search)) {
    cf_error::invalid_string('search');

    return FALSE;
  }

  if (!is_array($order_by)) {
    cf_error::invalid_array('order_by');

    return FALSE;
  }

  if (!cf_is_integer($offset)) {
    cf_error::invalid_integer('offset');

    return FALSE;
  }

  if (!is_null($limit) && !cf_is_integer($limit)) {
    cf_error::invalid_integer('limit');

    return FALSE;
  }

  if (!is_null($locked) && !is_bool($locked)) {
    cf_error::invalid_bool('locked');

    return FALSE;
  }

  if (!is_null($results) && !is_bool($results)) {
    cf_error::invalid_bool('results');

    return FALSE;
  }

  if (!is_array($users)) {
    cf_error::invalid_array('users');

    return FALSE;
  }


  // do not load the administer role (@fixme: role id is hardcoded).
  $role_administer = 3;

  $query_string = 'with the_users_roles as (select ur.uid, r.rid, r.name from users_roles ur inner join role r on ur.rid = r.rid order by r.name desc) ';
  $count_string = $query_string;

  $query_string .= 'select u.uid as id, u.name as name, u.mail as email, u.status as status, ';
  $query_string .= 'fdfubi.field_user_banner_id_value as banner_id, ';
  $query_string .= 'fdfufn.field_user_first_name_value as first_name, ';
  $query_string .= 'fdfuln.field_user_last_name_value as last_name, ';
  $query_string .= 'fdfuld.field_user_ldap_duplicate_value as ldap_duplicate, ';
  $query_string .= 'fdfupa.field_user_primary_account_value as primary_account, ';
  $query_string .= 'array_agg(tur.name) as roles ';
  $query_string .= 'from users u ';

  $count_string .= 'select count(distinct u.uid) from users u ';

  $join_string = 'left join field_data_field_user_banner_id fdfubi on u.uid = fdfubi.entity_id ';
  $join_string .= 'left join field_data_field_user_first_name fdfufn on u.uid = fdfufn.entity_id ';
  $join_string .= 'left join field_data_field_user_last_name fdfuln on u.uid = fdfuln.entity_id ';
  $join_string .= 'left join field_data_field_user_ldap_duplicate fdfuld on u.uid = fdfuld.entity_id ';
  $join_string .= 'left join field_data_field_user_primary_account fdfupa on u.uid = fdfupa.entity_id ';
  $join_string .= 'left join the_users_roles tur on u.uid = tur.uid ';
  $join_string .= 'left join mfcs_users mu on u.uid = mu.user_id ';

  $query_string .= $join_string;
  $count_string .= $join_string;
  unset($join_string);

  $where_string = 'where ';

  // prohibit loading users with administer roles.
  $where_string .= 'not u.uid in (select uid from the_users_roles where rid = :role_administer) ';

  // prohibit loading the system accounts.
  $where_string .= 'and not u.uid < 2 ';

  // limit list to the specified user ids.
  if (!empty($users)) {
    $users_list = NULL;
    foreach ($users as $u) {
      if (!is_numeric($u)) {
        continue;
      }

      if (is_null($users_list)) {
        $users_list = '' . (int) $u;
      }
      else {
        $users_list .= ', ' . (int) $u;
      }
    }
    unset($u);

    if (!is_null($users_list)) {
      $where_string .= 'and u.uid in (' . $users_list . ') ';
    }
    unset($users_list);
  }

  // restrict based on locked status.
  if ($locked === TRUE) {
    $where_string .= 'and u.status = 0 ';
  }
  elseif ($locked === FALSE) {
    $where_string .= 'and u.status <> 0 ';
  }

  $arguments = array();
  if (is_string($search) && strlen($search) > 0) {
    $regex_safe_search = preg_quote($search, '/');

    $limit_users_by_status = array();
    if (preg_match('/' . $regex_safe_search . '/i', 'Locked') > 0) {
      $limit_users_by_status[0] = 0;
    }
    if (preg_match('/' . $regex_safe_search . '/i', 'Available') > 0) {
      $limit_users_by_status[1] = 1;
    }

    $limit_users_by_ldap_duplicate = array();
    if (preg_match('/' . $regex_safe_search . '/i', 'Not Duplicate') > 0) {
      $limit_users_by_ldap_duplicate[0] = 0;
    }
    if (preg_match('/' . $regex_safe_search . '/i', 'Is Duplicate') > 0) {
      $limit_users_by_ldap_duplicate[1] = 1;
    }

    $limit_users_by_primary_account = array();
    if (preg_match('/' . $regex_safe_search . '/i', 'Alternate Account') > 0) {
      $limit_users_by_primary_account[0] = 0;
    }
    if (preg_match('/' . $regex_safe_search . '/i', 'Primary Account') > 0) {
      $limit_users_by_primary_account[1] = 1;
    }
    unset($regex_safe_search);

    $where_string .= 'and ( ';
    $where_string .= 'u.uid::text ilike :id ';
    $where_string .= 'or ';
    $where_string .= 'u.name ilike :name ';
    $where_string .= 'or ';
    $where_string .= 'u.mail ilike :email ';
    $where_string .= 'or ';
    $where_string .= 'fdfubi.field_user_banner_id_value ilike :banner_id ';
    $where_string .= 'or ';
    $where_string .= 'fdfufn.field_user_first_name_value ilike :first_name ';
    $where_string .= 'or ';
    $where_string .= 'fdfuln.field_user_last_name_value ilike :last_name ';
    $where_string .= 'or ';
    $where_string .= 'tur.name ilike :roles ';

    if (!empty($limit_users_by_ldap_duplicate)) {
      $where_string .= 'or ';
      $where_string .= 'fdfuld.field_user_ldap_duplicate_value in (:ldap_duplicate) ';

      $arguments[':ldap_duplicate'] = $limit_users_by_ldap_duplicate;
    }

    if (!empty($limit_users_by_primary_account)) {
      $where_string .= 'or ';
      $where_string .= 'fdfupa.field_user_primary_account_value in (:primary_account) ';

      $arguments[':primary_account'] = $limit_users_by_primary_account;
    }

    if (!empty($limit_users_by_status)) {
      $where_string .= 'or ';
      $where_string .= 'u.status in (:status) ';

      $arguments[':status'] = $limit_users_by_status;
    }

    $where_string .= ') ';

    $liked = db_like($search);
    $arguments[':id'] = '%' . $liked . '%';
    $arguments[':name'] = '%' . $liked . '%';
    $arguments[':email'] = '%' . $liked . '%';
    $arguments[':banner_id'] = '%' . $liked . '%';
    $arguments[':first_name'] = '%' . $liked . '%';
    $arguments[':last_name'] = '%' . $liked . '%';
    $arguments[':roles'] = '%' . $liked . '%';
  }
  elseif (is_int($search)) {
    $where_string .= 'and u.uid = :id ';
    $arguments[':id'] = $search;
  }

  $arguments[':role_administer'] = $role_administer;

  $query_string .= $where_string;
  $count_string .= $where_string;
  unset($where_string);

  $query_string .= 'group by u.uid, fdfubi.field_user_banner_id_value, fdfufn.field_user_first_name_value, fdfuln.field_user_last_name_value, fdfuld.field_user_ldap_duplicate_value, fdfupa.field_user_primary_account_value ';

  // default to order by user id.
  if (empty($order_by)) {
    $order_by = array(
      'id' => array(
        'direction' => 'asc',
      ),
    );
  }

  $order_by_string = NULL;
  foreach ($order_by as $sort_name => $sort_options) {
    switch ($sort_name) {
      case 'id':
      case 'name':
      case 'email':
      case 'status':
      case 'banner_id':
      case 'first_name':
      case 'last_name':
      case 'ldap_duplicate':
      case 'primary_account':
      case 'roles':
        break;
      default;
        continue;
    }

    if (!is_null($order_by_string)) {
      $order_by_string .= ', ';
    }

    $order_by_string .= $sort_name . ' ';
    if (is_array($sort_options)) {
      if (array_key_exists('direction', $sort_options)) {
        if ($sort_options['direction'] == 'asc') {
          $order_by_string .= 'asc ';
        }
        elseif ($sort_options['direction'] == 'desc') {
          $order_by_string .= 'desc ';
        }
      }

      if (array_key_exists('using', $sort_options)) {
        // as far as I can tell, only < and > are allowed with 'using'.
        if ($sort_options['using'] === MFCS_OPERATOR_LESS_THAN) {
          $order_by_string .= 'using < ';
        }
        elseif ($sort_options['using'] === MFCS_OPERATOR_GREATER_THAN) {
          $order_by_string .= 'using > ';
        }
      }

      if (array_key_exists('nulls', $sort_options)) {
        if ($sort_options['nulls'] == 'first') {
          $order_by_string .= 'nulls first ';
        }
        elseif ($sort_options['nulls'] == 'last') {
          $order_by_string .= 'nulls last ';
        }
      }
    }
  }
  unset($sort_name);
  unset($sort_options);

  if (!is_null($order_by_string)) {
    $query_string .= 'order by ' . $order_by_string;
  }
  unset($order_by_string);

  if (!is_null($limit)) {
    $query_string .= 'limit ' . $limit . ' ';

    if ($offset > 0) {
      $query_string .= 'offset ' . $offset . ' ';
    }
  }

  $return = array(
    'results' => NULL,
    'count' => NULL,
  );

  try {
    if (is_null($results) || $results === FALSE) {
      $query = db_query($count_string, $arguments);
      unset($count_string);

      $return['count'] = $query->fetchField();
      unset($query);
    }

    if (is_null($results) || $results === TRUE) {
      $query = db_query($query_string, $arguments);
      unset($query_string);
      unset($arguments);

      $return['results'] = $query->fetchAll();
      unset($query);
    }
  }
  catch (Error $e) {
    cf_error::on_query_execution($e);

    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);

    return FALSE;
  }

  return $return;
}

/**
 * Returns an array of roles and role names a user may belong to.
 *
 * @fixme: currently the role values are all hardcoded.
 *
 * @param bool $standard
 *   (optional) When TRUE, standard roles are returned.
 *   When FALSE, such roles are omitted.
 * @param bool $venue
 *   (optional) When TRUE, venue-related roles are returned.
 *   When FALSE, such roles are omitted.
 * @param bool $special
 *   (optional) When TRUE, special roles are returned.
 *   When FALSE, such roles are omitted.
 * @param bool $administer
 *   (optional) When TRUE, administer-based roles are returned.
 *   When FALSE, such roles are omitted.
 *
 * @return array
 *   An array containing the roles.
 */
function mfcs_request_users_get_roles($standard = TRUE, $venue = TRUE, $special = TRUE, $administer = FALSE) {
  if (!is_bool($standard)) {
    cf_error::invalid_bool('standard');
    return array();
  }

  if (!is_bool($venue)) {
    cf_error::invalid_bool('venue');
    return array();
  }

  if (!is_bool($special)) {
    cf_error::invalid_bool('special');
    return array();
  }

  if (!is_bool($administer)) {
    cf_error::invalid_bool('administer');
    return array();
  }

  $roles = array();

  if ($standard) {
    foreach (array('reviewer', 'manager', 'requester', 'financer') as $role_name) {
      $role = user_role_load_by_name($role_name);
      if (is_object($role)) {
        $roles[$role->rid] = $role->name;
      }
    }
  }

  if ($venue) {
    foreach (array('student role', 'camp role', 'employee role', 'external role') as $role_name) {
      $role = user_role_load_by_name($role_name);
      if (is_object($role)) {
        $roles[$role->rid] = $role->name;
      }
    }
  }

  if ($special) {
    foreach (array('bannerless exception', 'locked exception', 'reviewer exception') as $role_name) {
      $role = user_role_load_by_name($role_name);
      if (is_object($role)) {
        $roles[$role->rid] = $role->name;
      }
    }
  }

  if ($administer) {
    foreach (array('administrator', 'masquerade') as $role_name) {
      $role = user_role_load_by_name($role_name);
      if (is_object($role)) {
        $roles[$role->rid] = $role->name;
      }
    }
  }

  ksort($roles);
  return $roles;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
