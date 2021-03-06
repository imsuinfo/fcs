<?php

/**
 * @file
 * Defines McNeese facilities use request manage page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request manage page.
 *
 * @param int $request_id
 *   The unique identifier for a request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_manage_0_page($request_id) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    cf_error::invalid_array('request');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $user = cf_current_user();

  $can_override_request = mfcs_request_access('manage', $request, $user);
  if (!$can_override_request) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/manage-0/' . $request_id);

  $page_title = 'Manage Request';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Manage Request: ' . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  global $base_path;
  global $mfcs_determined;
  global $user;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $id_id = (int) $request_id;
  $id_step = (int) $request['top']['step'][0]->value;
  $id_status = (int) $request['top']['status'][0]->value;
  $id_room = (int) $request['top']['room'][0]->value;
  $id_building = (int) $request['top']['building'][0]->value;
  $id_location = (int) $request['top']['location'][0]->value;
  $id_creator = (int) $request['mer']['creator'][0]->value;
  $id_requester = (int) $request['mer']['user_id'][0]->value;
  $id_venue_coordinator = (int) $request['venue_coordinator']['user_id'][0]->value;

  $locks_auto_cancel = 'First Time';
  $request_locks = mfcs_load_request_locks($request_id);
  if (is_object($request_locks) && isset($request_locks->auto_cancel)) {
    if ($request_locks->auto_cancel === TRUE) {
      $locks_auto_cancel = 'Prohibit';
    }
    else {
      $locks_auto_cancel = 'Always';
    }
  }

  $user_creator = mfcs_load_basic_user_settings_by_user_id($id_creator);
  $user_requester = mfcs_load_basic_user_settings_by_user_id($id_requester);
  $user_venue_coordinator = mfcs_load_basic_user_settings_by_user_id($id_venue_coordinator);

  $coordinator_problems = mfcs_get_request_venue_coordinator_inconsistencies($request);

  $disabled_requester = ($user_requester['status'] == 0);
  $disabled_venue_coordinator = ($user_venue_coordinator['status'] == 0);

  $user_banner_coordinator_room = NULL;
  $user_banner_coordinator_venue = NULL;
  $disabled_banner_coordinator_room = FALSE;
  $disabled_banner_coordinator_venue = FALSE;
  $room_information = mfcs_load_rooms(NULL, NULL, $id_room, TRUE);
  if (isset($room_information->room_coordinator_id)) {
    $user_banner_coordinator_room = mfcs_load_basic_user_settings_by_user_id((int)$room_information->room_coordinator_id);
    $disabled_banner_coordinator_room = ($user_banner_coordinator_room['status'] == 0);
  }
  if (isset($room_information->venue_coordinator_id)) {
    $user_banner_coordinator_venue = mfcs_load_basic_user_settings_by_user_id((int)$room_information->venue_coordinator_id);
    $disabled_banner_coordinator_venue = ($user_banner_coordinator_venue['status'] == 0);
  }
  unset($room_information);

  $can_cancel_request = mfcs_request_access('cancel', $request, $user);
  $can_uncancel_request = mfcs_request_access('uncancel', $request, $user);
  $can_move_request = mfcs_request_access('move', $request, $user);
  $can_reassign_request_coordinator = mfcs_request_access('reassign-coordinator', $request, $user);
  $can_reassign_request_requester = mfcs_request_access('reassign-requester', $request, $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options_step = mfcs_get_review_steps_list_options(NULL, FALSE, TRUE);
  $options_status = mfcs_get_request_status_list_options(NULL, FALSE, TRUE);
  $options_room = mfcs_get_request_room_list_options($id_location, NULL, FALSE, TRUE);
  $options_building = mfcs_get_request_building_list_options($id_location, NULL, FALSE, TRUE);
  $options_location = mfcs_get_request_location_list_options(NULL, FALSE, TRUE);

  $string_id = '' . $id_id;
  $string_step = '';
  $string_status = '';
  $string_room = '';
  $string_building = '';
  $string_location = '';

  if (array_key_exists($id_step, $options_step) && is_string($options_step[$id_step])) {
    $string_step = $options_step[$id_step];
  }

  if (array_key_exists($id_status, $options_status) && is_string($options_status[$id_status])) {
    $string_status = $options_status[$id_status];
  }

  if (array_key_exists($id_room, $options_room) && is_string($options_room[$id_room])) {
    $string_room = $options_room[$id_room];
  }

  if (array_key_exists($id_building, $options_building) && is_string($options_building[$id_building])) {
    $string_building = $options_building[$id_building];
  }

  if (array_key_exists($id_location, $options_location) && is_string($options_location[$id_location])) {
    $string_location = $options_location[$id_location];
  }

  $markup = '';
  $markup .= '<div id="mfcs-request-manage-0-page" class="request_manage">';


  // general information.
  $class_name = 'general_information';
  $markup .= '<div id="mfcs-request-manage-0-page-' . $class_name . '" class="section section-' . $class_name . '">';
  $markup .= '<h3 class="header header-' . $class_name . '">General Information</h3>';
  $markup .= '<div class="content content-' . $class_name . '">';
  $markup .= mfcs_request_manage_0_page_create_item_and_value('ID:', $string_id, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Step:',  $string_step, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Status:', $string_status, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Room:',  $string_room, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Building:',  $string_building, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Location:',  $string_location, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Creator:',  $user_creator, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Requester:',  $user_requester, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Venue Coordinator:',  $user_venue_coordinator, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Banner <span title="Room Coordinator">RC</span>:',  $user_banner_coordinator_room, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Banner <span title="Venue Coordinator">VC</span>:',  $user_banner_coordinator_venue, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Auto-Cancel:',  $locks_auto_cancel, $class_name);

  $markup .= '</div>';
  $markup .= '</div>';


  // user information.
  $class_name = 'user_information';
  $markup .= '<div id="mfcs-request-manage-0-page-' . $class_name . '" class="section section-' . $class_name . '">';
  $markup .= '<h3 class="header header-' . $class_name . '">User Information</h3>';
  $markup .= '<div class="content content-' . $class_name . '">';
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Requester Status:', ($disabled_requester ? 'Account Disabled/Deleted' : 'Account Enabled'), $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_banner_id('Requester Banner ID:',  $user_requester, $class_name);

  $markup .= mfcs_request_manage_0_page_create_item_and_value('Venue Coordinator Status:', ($disabled_venue_coordinator ? 'Account Disabled / Deleted' : 'Account Enabled'), $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_banner_id('Venue Coordinator Banner ID:',  $user_venue_coordinator, $class_name);

  $markup .= mfcs_request_manage_0_page_create_item_and_value('Banner <span title="Room Coordinator">RC</span> Status:', ($disabled_banner_coordinator_room ? 'Account Disabled / Deleted' : 'Account Enabled'), $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_banner_id('Banner <span title="Room Coordinator">RC</span> Banner ID:',  $user_banner_coordinator_room, $class_name);

  $markup .= mfcs_request_manage_0_page_create_item_and_value('Banner <span title="Venue Coordinator">VC</span> Status:', ($disabled_banner_coordinator_venue ? 'Account Disabled / Deleted' : 'Account Enabled'), $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_banner_id('Banner <span title="Venue Coordinator">VC</span> Banner ID:',  $user_banner_coordinator_venue, $class_name);

  $markup .= '</div>';
  $markup .= '</div>';


  // Management Actions
  $class_name = 'management_actions';
  $markup .= '<div id="mfcs-request-manage-0-page-' . $class_name . '" class="section section-' . $class_name . '">';
  $markup .= '<h3 class="header header-' . $class_name . '">Management Actions</h3>';
  $markup .= '<div class="content content-' . $class_name . '">';

  $markup .= mfcs_request_manage_0_page_create_item_message('<a href="' . $base_path . 'requests/manage-0/override/' . $id_id . $url_arguments . '" class="link link-override" title="Override Request\'s Step or Status">Manually Override</a> the current requests review step or status.', $class_name);

  if ($can_cancel_request) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The request may be <a href="' . $base_path . 'requests/cancel-0/' . $id_id . $url_arguments . '" class="link link-cancel" title="Cancel Request">Cancelled</a>.', $class_name);
  }

  if ($can_uncancel_request) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The request may be <a href="' . $base_path . 'requests/uncancel-0/' . $id_id . $url_arguments . '" class="link link-uncancel" title="Uncancel Request">Uncancelled</a>.', $class_name);
  }

  $markup .= mfcs_request_manage_0_page_create_item_message('The <a href="' . $base_path . 'requests/manage-0/locks/' . $id_id . $url_arguments . '" class="link link-override" title="Toggle Request Locks">Request\'s Locks</a> may be manually updated.', $class_name);

  if ($can_move_request) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The request may be <a href="' . $base_path . 'requests/manage-0/move/' . $id_id . $url_arguments . '" class="link link-uncancel" title="Move Request">Moved</a>.', $class_name);
  }

  if ($can_reassign_request_coordinator) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The Venue Coordinator may be <a href="' . $base_path . 'requests/manage-0/reassign/coordinator/' . $id_id . $url_arguments . '" class="link link-uncancel" title="Re-Assign Coordinator"">Re-Assigned</a>.', $class_name);
  }

  if ($can_reassign_request_requester) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The Requester may be <a href="' . $base_path . 'requests/manage-0/reassign/requester/' . $id_id . $url_arguments . '" class="link link-uncancel" title="Re-Assign Requester">Re-Assigned</a>.', $class_name);
  }

  $markup .= '</div>';
  $markup .= '</div>';


  // Request History
  $class_name = 'request_history';
  $markup .= '<div id="mfcs-request-manage-0-page-' . $class_name . '" class="section section-' . $class_name . '">';
  $markup .= '<h3 class="header header-' . $class_name . '">Request History</h3>';
  $markup .= '<div class="content content-' . $class_name . '">';

  $markup .= mfcs_request_manage_0_page_create_item_message('View the <a href="' . $base_path . 'requests/view-0/' . $id_id . '/' . MFCS_VIEW_MODE_LOG . $url_arguments . '" class="link link-review_log">Review Log</a>.', $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_message('View the <a href="' . $base_path . 'requests/history-0/' . $id_id . $url_arguments . '" class="link link-history">Request History</a>.', $class_name);

  $markup .= '</div>';
  $markup .= '</div>';


  // Request Problems
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $conditions = array(
    'value_request_id' => array(
      'field' => 'mpr.request_id',
      'operator' => MFCS_OPERATOR_EQUAL,
      'search' => $id_id
    ),
  );
  $request_problems_by_id = mfcs_miscellaneous_problems_request_search($conditions, array('count' => FALSE));
  unset($conditions);

  if (!empty($request_problems_by_id['results'])) {
    $class_name = 'request_problems';
    $markup .= '<div id="mfcs-request-manage-0-page-' . $class_name . '" class="section section-' . $class_name . '">';
    $markup .= '<h3 class="header header-' . $class_name . '">Request Problems</h3>';
    $markup .= '<div class="content content-' . $class_name . '">';

    foreach ($request_problems_by_id['results'] as $problem_id => $problem_data) {
      $markup .= mfcs_request_manage_0_page_create_item_message('<a href="' . $base_path . '/problems-0/requests/problem/' . ((int)$problem_id) . $url_arguments . '" class="link link-request_problem">' . check_plain($problem_data->reason) . '</a>.', $class_name);
    }
    unset($problem_id);
    unset($problem_data);

    $markup .= '</div>';
    $markup .= '</div>';
  }
  unset($request_problems_by_id);


  $markup .= '</div>';

  return $markup;
}

/**
 * Given an item message, return a standardize markup string.
 *
 * This is meant to be called internally only.
 *
 * @param string $message
 *   The label of the item.
 * @param string $section
 *   The section name (used to generate class name).
 *
 * @return string
 *   The processed string.
 */
function mfcs_request_manage_0_page_create_item_message($message, $section) {
  if (!is_string($message) && !is_string($section)) {
    return '';
  }

  $markup = '';
  $markup .= '<div class="item item-' . $section . '">';
  $markup .= '<span class="item-message item-message-' . $section . '">' . $message . '</span>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Given an item name and value, return a standardize markup string.
 *
 * This is meant to be called internally only.
 *
 * @param string $label
 *   The label of the item.
 * @param string $value
 *   The value of the item.
 * @param string $section
 *   The section name (used to generate class name).
 *
 * @return string
 *   The processed string.
 */
function mfcs_request_manage_0_page_create_item_and_value($label, $value, $section) {
  if (!is_string($label) && !is_string($value) && !is_string($section)) {
    return '';
  }

  $markup = '';
  $markup .= '<div class="item item-' . $section . '">';
  $markup .= '<span class="item-label item-label-' . $section . '">' . $label . '</span>';
  $markup .= '<span class="item-value item-value-' . $section . '">' . $value . '</span>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Given an a user array, return a standardize markup string consisting of the user name and user id.
 *
 * This is meant to be called internally only.
 *
 * @param string $label
 *   The label of the item.
 * @param array $user
 *   The populated user array.
 * @param string $section
 *   The section name (used to generate class name).
 *
 * @return string
 *   The processed string.
 *
 * @see: mfcs_load_basic_user_settings_by_user_id()
 */
function mfcs_request_manage_0_page_create_item_user_name($label, $user, $section) {
  if (!is_string($label) || !is_array($user) || !array_key_exists('user_id', $user) || !is_int($user['user_id']) || !is_string($section)) {
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $string = NULL;

  if (array_key_exists('first_name', $user) && is_string($user['first_name']) && array_key_exists('last_name', $user) && is_string($user['last_name'])) {
    $string = $user['first_name'] . ' ' . $user['last_name'];
  }

  if (array_key_exists('name', $user) && is_string($user['name'])) {
    if (is_null($string)) {
      $string = $user['name'];
    }
    else {
      $string .= ' (' . $user['name'] . ')';
    }
  }

  if (is_null($string)) {
    $string = '';
  }
  else {
    $string = '<a href="' . $base_path . 'users-0/view/' . $user['user_id'] . $url_arguments . '" title="View User Profile">' . $string . '</a> <span class="user_id">[id: ' . $user['user_id'] . ']</span>';
  }

  return mfcs_request_manage_0_page_create_item_and_value($label, $string, $section);
}

/**
 * Given an a user id, return a standardize markup string consisting of the users banner id.
 *
 * This is meant to be called internally only.
 *
 * @param string $label
 *   The label of the item.
 * @param array $user
 *   The populated user array.
 * @param string $section
 *   The section name (used to generate class name).
 *
 * @return string
 *   The processed string.
 *
 * @see: mfcs_load_basic_user_settings_by_user_id()
 */
function mfcs_request_manage_0_page_create_item_user_banner_id($label, $user, $section) {
  if (!is_string($label) || !is_array($user) || !array_key_exists('user_id', $user) || !is_int($user['user_id']) || !is_string($section)) {
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $string = '';
  if (array_key_exists('banner_id', $user)) {
    $string = $user['banner_id'];
  }

  return mfcs_request_manage_0_page_create_item_and_value($label, $string, $section);
}

/**
 * @} End of '@addtogroup mfcs'.
 */
