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

  $page_title = "Manage Request";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Manage Request: " . $request['information']['title'][0]->value;
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
  $id_requester = (int) $request['mer']['user_id'][0]->value;
  $id_venue_coordinator = (int) $request['request_coordinator']['name'][0]->user_id;

  $locks_auto_cancel = 'Default';
  $request_locks = mfcs_load_request_locks($request_id);
  if (is_object($request_locks) && isset($request_locks->auto_cancel)) {
    if ($request_locks->auto_cancel === TRUE) {
      $locks_auto_cancel = 'Locked';
    }
    else {
      $locks_auto_cancel = 'Unlocked';
    }
  }

  $user_requester = mfcs_load_basic_user_settings_by_user_id($id_requester);
  $user_venue_coordinator = mfcs_load_basic_user_settings_by_user_id($id_venue_coordinator);

  $disabled_requester = mfcs_disabled_requesters_with_active_content($id_requester);
  $disabled_venue_coordinator = mfcs_disabled_venue_coordinators_with_active_content($id_venue_coordinator);

  if (is_array($disabled_requester)) {
    if (empty($disabled_requester)) {
      $disabled_requester = FALSE;
    }
    else {
      $disabled_requester = TRUE;
    }
  }

  if (is_array($disabled_venue_coordinator)) {
    if (empty($disabled_venue_coordinator)) {
      $disabled_venue_coordinator = FALSE;
    }
    else {
      $disabled_venue_coordinator = TRUE;
    }
  }

  $can_cancel_request = mfcs_request_access('cancel', $request, $user);
  $can_uncancel_request = mfcs_request_access('uncancel', $request, $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options_step = mfcs_get_review_steps_list_options();
  $options_status = mfcs_get_request_status_list_options();
  $options_room = mfcs_get_request_room_list_options($id_location);
  $options_building = mfcs_get_request_building_list_options($id_location);
  $options_location = mfcs_get_request_location_list_options();

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
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Requester:',  $user_requester, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_user_name('Venue Coordinator:',  $user_venue_coordinator, $class_name);
  $markup .= mfcs_request_manage_0_page_create_item_and_value('Auto-Cancel Lock:',  $locks_auto_cancel, $class_name);

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

  $markup .= '</div>';
  $markup .= '</div>';


  // Management Actions
  $class_name = 'management_actions';
  $markup .= '<div id="mfcs-request-manage-0-page-' . $class_name . '" class="section section-' . $class_name . '">';
  $markup .= '<h3 class="header header-' . $class_name . '">Management Actions</h3>';
  $markup .= '<div class="content content-' . $class_name . '">';

  $markup .= mfcs_request_manage_0_page_create_item_message('<a href="' . $base_path . 'requests/manage-0/override/' . $id_id . $url_arguments . '" class="link link-override">Manually Override</a> the current requests review step or status.', $class_name);

  if ($disabled_venue_coordinator) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The venue coordinator account should be <a href="' . $base_path . 'requests/manage-0/reassign/' . $id_id . $url_arguments . '" class="link link-reassign">Re-Assigned</a>.', $class_name);
  }

  if ($can_cancel_request) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The request may be <a href="' . $base_path . 'requests/cancel-0/' . $id_id . $url_arguments . '" class="link link-cancel">Cancelled</a>.', $class_name);
  }

  if ($can_uncancel_request) {
    $markup .= mfcs_request_manage_0_page_create_item_message('The request may be <a href="' . $base_path . 'requests/uncancel-0/' . $id_id . $url_arguments . '" class="link link-uncancel">Uncancelled</a>.', $class_name);
  }

  $markup .= mfcs_request_manage_0_page_create_item_message('The <a href="' . $base_path . 'requests/manage-0/locks/' . $id_id . $url_arguments . '" class="link link-override">Request\'s Locks</a> may be manually updated.', $class_name);

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
    $string = '<a href="' . $base_path . 'requests/users-0/view/' . $user['user_id'] . $url_arguments . '" title="View User Profile">' . $string . '</a> <span class="user_id">[id: ' . $user['user_id'] . ']</span>';
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
  if (array_key_exists('banner_id', $user) && is_string($user['banner_id'])) {
    $string = $user['banner_id'];
  }

  return mfcs_request_manage_0_page_create_item_and_value($label, $string, $section);
}

/**
 * @} End of '@addtogroup mfcs'.
 */
