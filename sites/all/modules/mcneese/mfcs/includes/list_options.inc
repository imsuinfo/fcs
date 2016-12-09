<?php

/**
 * @file
 * Defines McNeese facilities use list options functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Defines restrictions review decisions.
 */
 define('MFCS_RESTRICTION_DECISION_NONE', 0);
 define('MFCS_RESTRICTION_DECISION_REQUESTER', 1);
 define('MFCS_RESTRICTION_DECISION_VENUE_COORDINATOR', 2);
 define('MFCS_RESTRICTION_DECISION_REVIEWER', 3);
 define('MFCS_RESTRICTION_DECISION_APPROVER', 4);
 define('MFCS_RESTRICTION_DECISION_MANAGER', 5);
 define('MFCS_RESTRICTION_DECISION_ADMINISTER', 6);
 define('MFCS_RESTRICTION_DECISION_COMMENT_ONLY', 7);
 define('MFCS_RESTRICTION_DECISION_FINANCER', 8);
 define('MFCS_RESTRICTION_DECISION_INSURER', 9);

/**
 * Defines Taxonomy TIDs for request revenue.
 */
 define('MFCS_REQUEST_REVENUE_NONE', 0);
 define('MFCS_REQUEST_REVENUE_TICKET_SALES', 1);
 define('MFCS_REQUEST_REVENUE_MERCHANDISE_SALES', 2);
 define('MFCS_REQUEST_REVENUE_CONCESSION_SALES', 3);

/**
 * Defines Taxonomy TIDs for request publicity.
 */
 define('MFCS_REQUEST_PUBLICITY_NONE', 0);
 define('MFCS_REQUEST_PUBLICITY_CAMPUS_DIGEST', 1);
 define('MFCS_REQUEST_PUBLICITY_STUDENT_DIGEST', 2);
 define('MFCS_REQUEST_PUBLICITY_WEBSITE', 3);
 define('MFCS_REQUEST_PUBLICITY_SOCIAL_MEDIA', 4);
 define('MFCS_REQUEST_PUBLICITY_AXIS_TVS', 5);
 define('MFCS_REQUEST_PUBLICITY_PRESS_RELEASE', 6);

/**
 * Defines Taxonomy TIDs for request designing material.
 */
 define('MFCS_REQUEST_DESIGNING_MATERIALS_NONE', 0);
 define('MFCS_REQUEST_DESIGNING_MATERIALS_PUBLIC_RELATIONS', 1);
 define('MFCS_REQUEST_DESIGNING_MATERIALS_MARKETING', 2);
 define('MFCS_REQUEST_DESIGNING_MATERIALS_OTHER', 3);

/**
 * Defines Taxonomy TIDs for request technical equipment.
 */
 define('MFCS_REQUEST_TECHNICAL_EQUIPMENT_NONE', 0);
 define('MFCS_REQUEST_TECHNICAL_EQUIPMENT_SCREEN', 1);
 define('MFCS_REQUEST_TECHNICAL_EQUIPMENT_COMPUTER', 2);
 define('MFCS_REQUEST_TECHNICAL_EQUIPMENT_SOUND', 3);
 define('MFCS_REQUEST_TECHNICAL_EQUIPMENT_MICROPHONE', 4);

/**
 * Defines Taxonomy TIDs for request classifications.
 */
 define('MFCS_REQUEST_AUDIENCE_NONE', 0);
 define('MFCS_REQUEST_AUDIENCE_FACULTY_AND_STAFF', 1);
 define('MFCS_REQUEST_AUDIENCE_GUESTS_ONLY', 2);
 define('MFCS_REQUEST_AUDIENCE_PUBLIC', 3);
 define('MFCS_REQUEST_AUDIENCE_STUDENTS_AND_EMPLOYEES', 4);
 define('MFCS_REQUEST_AUDIENCE_STUDENTS_ONLY', 5);

/**
 * Defines problems log codes.
 */
define('MFCS_LOG_PROBLEMS_NONE', 0);
define('MFCS_LOG_PROBLEMS_INSERT', 1);
define('MFCS_LOG_PROBLEMS_UPDATE', 2);
define('MFCS_LOG_PROBLEMS_DELETE', 3);
define('MFCS_LOG_PROBLEMS_RESYNC', 4);
define('MFCS_LOG_PROBLEMS_IGNORE', 5);
define('MFCS_LOG_PROBLEMS_UNIGNORE', 6);
define('MFCS_LOG_PROBLEMS_RECHECK', 7);
define('MFCS_LOG_PROBLEMS_REASSIGN_VENUE_COORDINATOR', 8);
define('MFCS_LOG_PROBLEMS_REASSIGN_REQUESTER', 9);
define('MFCS_LOG_PROBLEMS_REASSIGN_REBUILD_CACHE', 10);

/**
 * Unavailability Types
 */
define('MFCS_UNAVAILABILITY_TYPE_NONE', 0);
define('MFCS_UNAVAILABILITY_TYPE_RENOVATION', 1);
define('MFCS_UNAVAILABILITY_TYPE_REPAIR', 2);
define('MFCS_UNAVAILABILITY_TYPE_REFURBISHMENT', 3);
define('MFCS_UNAVAILABILITY_TYPE_SPECIAL_EVENT', 4);
define('MFCS_UNAVAILABILITY_TYPE_OUTAGE', 5);
define('MFCS_UNAVAILABILITY_TYPE_EMERGENCY', 6);
define('MFCS_UNAVAILABILITY_TYPE_CLASS', 7);

/**
 * E-Mail Types
 */
define('MFCS_EMAIL_TYPE_NONE', 0);
define('MFCS_EMAIL_TYPE_AMENDED', 1);
define('MFCS_EMAIL_TYPE_APPROVED', 2);
define('MFCS_EMAIL_TYPE_CANCELLED', 3);
define('MFCS_EMAIL_TYPE_CANCELLING', 4);
define('MFCS_EMAIL_TYPE_COMMENT', 5);
define('MFCS_EMAIL_TYPE_DELETED', 6);
define('MFCS_EMAIL_TYPE_DENIED', 7);
define('MFCS_EMAIL_TYPE_FINAL_DECISION', 8);
define('MFCS_EMAIL_TYPE_FORCED', 9);
define('MFCS_EMAIL_TYPE_INSURANCE_PROVIDED', 10);
define('MFCS_EMAIL_TYPE_INSURANCE_REQUIRED', 11);
define('MFCS_EMAIL_TYPE_ISSUES', 12);
define('MFCS_EMAIL_TYPE_MODIFIED', 13);
define('MFCS_EMAIL_TYPE_MOVED', 14);
define('MFCS_EMAIL_TYPE_NEEDS_REVIEW', 15);
define('MFCS_EMAIL_TYPE_ISSUES_NONE', 16);
define('MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR', 17);
define('MFCS_EMAIL_TYPE_REASSIGN_REQUESTER', 18);
define('MFCS_EMAIL_TYPE_REQUESTED', 19);
define('MFCS_EMAIL_TYPE_REQUIREMENTS', 20);
define('MFCS_EMAIL_TYPE_REVIEWED', 21);
define('MFCS_EMAIL_TYPE_UNAVAILABLE', 22);
define('MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED', 23);
define('MFCS_EMAIL_TYPE_UNCANCELLED', 24);
define('MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST', 25);

/**
 * Defines Taxonomy TIDs for request revenue.
 */
 define('MFCS_HOLIDAY_TYPE_NONE', 0);
 define('MFCS_HOLIDAY_TYPE_HOLIDAY', 1);
 define('MFCS_HOLIDAY_TYPE_FORCED_CLOSURE', 2);
 define('MFCS_HOLIDAY_TYPE_BREAK', 3);

/**
 * Returns an array of facilities use status options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_status_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_STATUS_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_STATUS_DELETED] = 'Deleted';
  $options_all[MFCS_REQUEST_STATUS_UNLOCKED] = 'Queued (Unlocked)';
  $options_all[MFCS_REQUEST_STATUS_LOCKED] = 'Queued (Locked)';
  $options_all[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] = 'Accepted';
  $options_all[MFCS_REQUEST_STATUS_CLOSED_DENIED] = 'Denied';
  $options_all[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] = 'Room Unavailable';
  $options_all[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] = 'Room Unavailable (due to lock)';
  $options_all[MFCS_REQUEST_STATUS_CANCELLED] = 'Cancelled';
  $options_all[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] = 'Accepted (Cancelled)';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  if ($option == 'search') {
    unset($options[MFCS_REQUEST_STATUS_DELETED]);
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of facilities use state options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_review_steps_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REVIEW_STEP_NONE] = 'None';
  }

  $options_all[MFCS_REVIEW_STEP_VENUE_AVAILABLE] = 'Venue Available';
  $options_all[MFCS_REVIEW_STEP_FINAL_DECISION] = 'Final Decision';
  $options_all[MFCS_REVIEW_STEP_RELEASE_HOLD] = 'Release Tentative Hold';
  $options_all[MFCS_REVIEW_STEP_REVIEW] = 'Review';
  $options_all[MFCS_REVIEW_STEP_REQUIREMENTS] = 'Requirements';
  $options_all[MFCS_REVIEW_STEP_CHANGES_REQUIRED] = 'Changes Required';
  $options_all[MFCS_REVIEW_STEP_COMPLETED] = 'Completed';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  if ($option == 'search') {
    unset($options[MFCS_REVIEW_STEP_RELEASE_HOLD]);
    unset($options[MFCS_REVIEW_STEP_REQUIREMENTS]);
    unset($options[MFCS_REVIEW_STEP_CHANGES_REQUIRED]);
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request classification options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param object|null $user
 *   (optional) When not NULL, the user to build the available options against.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_classification_list_options($option = NULL, $user = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();
  $options_to_roles = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_CLASSIFICATION_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_CLASSIFICATION_STUDENT] = 'Student Organization';
  $options_all[MFCS_REQUEST_CLASSIFICATION_CAMPS] = 'Camps';
  $options_all[MFCS_REQUEST_CLASSIFICATION_FACULTY] = 'Faculty, Staff, Athletics';
  $options_all[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] = 'Externals';

  $role = user_role_load_by_name('student role');
  if (is_object($role)) {
    $options_to_roles[$role->rid] = MFCS_REQUEST_CLASSIFICATION_STUDENT;
  }
  unset($role);

  $role = user_role_load_by_name('camp role');
  if (is_object($role)) {
    $options_to_roles[$role->rid] = MFCS_REQUEST_CLASSIFICATION_CAMPS;
  }
  unset($role);

  $role = user_role_load_by_name('employee role');
  if (is_object($role)) {
    $options_to_roles[$role->rid] = MFCS_REQUEST_CLASSIFICATION_FACULTY;
  }
  unset($role);

  $role = user_role_load_by_name('external role');
  if (is_object($role)) {
    $options_to_roles[$role->rid] = MFCS_REQUEST_CLASSIFICATION_EXTERNAL;
  }
  unset($role);

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if (is_object($user)) {
    $is_manager = user_access('mfcs manager', $user);
    $is_administer = user_access('mfcs administer', $user);

    if ($is_manager || $is_administer) {
      foreach ($options_all as $option_id => $option_name) {
        $options[$option_id] = $option_name;
      }
    }

    foreach ($options_to_roles as $role_id => $classification_id) {
      if (array_key_exists($role_id, $user->roles) && array_key_exists($classification_id, $options_all)) {
        $options[$classification_id] = $options_all[$classification_id];
      }
    }
  }
  else {
    foreach ($options_all as $option_id => $option_name) {
      $options[$option_id] = $option_name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request category options for populating lists.
 *
 * Note: ucs.msu_venue_list calls the building location 'category'.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_location_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if ($hidden) {
    $options[0] = 'None';
  }

  $locations = mfcs_load_locations(NULL, $disabled);
  if (is_array($locations)) {
    foreach ($locations as $location) {
      $options[(int) $location->location_id] = $location->location_name;
    }
  }

  return $options;
}

/**
 * Returns an array of request building list for populating lists.
 *
 * @param string|null $location
 *   (optional) When not NULL, select options based on provided location.
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_building_list_options($location = NULL, $option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if ($hidden) {
    $options[0] = 'None';
  }

  $buildings = mfcs_load_buildings($location, NULL, $disabled);

  if ($option == 'special') {
    global $base_path;
    $module_path = drupal_get_path('module', 'mfcs');

    foreach ($buildings as &$building) {
      // to simplify the design and to avoid having to alter the code later on, use the building and building ids as the name of the file.
      $image_file_path = NULL;

      // look for generic building-specific images.
      if (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/buildings/' . $building->building_id . '.png')) {
        $image_file_path = $base_path . $module_path . '/images/buildings/' . $building->building_id . '.png';
      }
      elseif (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/buildings/' . $building->building_id . '.jpg')) {
        $image_file_path = $base_path . $module_path . '/images/buildings/' . $building->building_id . '.jpg';
      }
      elseif (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/buildings/0.png')) {
        $image_file_path = $base_path . $module_path . '/images/buildings/0.png';
      }

      $building_name = NULL;
      if (!empty($building->building_name)) {
        $building_name = check_plain($building->building_name);
      }
      $building_name = trim($building_name);

      $text = '';
      if (!is_null($image_file_path)) {
        $text = '<img src="' . $image_file_path . '" alt="' . $building_name . '" width="65" class="building_image">';
      }

      $text .= '<div class="building_name">' . $building_name . '</div>';

      $options[$building->building_id] = $text;
    }

    // return now to avoid custom sorting.
    return $options;
  }
  else {
    foreach ($buildings as &$building) {
      $options[$building->building_id] = $building->building_name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request room list for populating lists.
 *
 * @param string|null $location
 *   (optional) When not NULL, select options based on provided location.
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_room_list_options($location = NULL, $option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if ($hidden) {
    $options[0] = 'None';
  }

  $rooms = mfcs_load_rooms($location, NULL, NULL, $disabled);

  if ($option == 'special') {
    global $base_path;
    $module_path = drupal_get_path('module', 'mfcs');

    foreach ($rooms as $room) {
      // to simplify the design and to avoid having to alter the code later on, use the building and room ids as the name of the file.
      $image_file_name = $room->building_id;
      $image_file_path = NULL;
      $image_alt_text = NULL;

      // first look for generic building-specific images.
      if (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/buildings/' . $image_file_name . '.png')) {
        $image_file_path = $base_path . $module_path . '/images/buildings/' . $image_file_name . '.png';
        $image_alt_text = FALSE;
      }
      elseif (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/buildings/' . $image_file_name . '.jpg')) {
        $image_file_path = $base_path . $module_path . '/images/buildings/' . $image_file_name . '.jpg';
        $image_alt_text = FALSE;
      }

      // then look for room-specific images.
      $image_file_name .= '-' . $room->room_id;
      if (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/rooms/' . $image_file_name . '.png')) {
        $image_file_path = $base_path . $module_path . '/images/rooms/' . $image_file_name . '.png';
        $image_alt_text = TRUE;
      }
      elseif (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/rooms/' . $image_file_name . '.jpg')) {
        $image_file_path = $base_path . $module_path . '/images/rooms/' . $image_file_name . '.jpg';
        $image_alt_text = TRUE;
      }
      unset($image_file_name);

      $room_name = NULL;
      if (!empty($room->room_name)) {
        $room_name = check_plain($room->room_name);
      }

      $building_name = NULL;
      if (!empty($room->building_name)) {
        $building_name = check_plain($room->building_name);
      }

      $room_number = NULL;
      if (!empty($room->room_number)) {
        $room_number = mcneese_fcs_text_capitalization($room->room_number);
        $room_number = check_plain($room_number);
      }

      $text = '';

      if (!is_null($image_file_path)) {
        if ($image_alt_text) {
          $image_alt_text = trim($building_name . ' ' . $room_name);
        }
        elseif ($image_alt_text === FALSE) {
          $image_alt_text = trim($building_name);
        }

        $text .= '<img src="' . $image_file_path . '" alt="' . $image_alt_text . '" width="65" class="room_image">';
      }
      unset($image_alt_text);

      $text .= '<div class="room_notes">';
      $text .= '<div class="room_number"><span class="room_number-label">Room:</span> <span class="room_number-data">' . $room_number . '</span></div>';

      if (!empty($room->capacity_normal)) {
        $text .= '<div class="room_capacity"><span class="capacity-label">Capacity:</span> <span class="capacity-data">' . check_plain($room->capacity_normal) . '</span></div>';
      }
      $text .= '</div>';

      $text .= '<div class="building_name">' . $building_name . '</div>';

      if (is_null($room_name)) {
        $room_name = 'No information available.';
      }

      $text .= '<div class="room_description"><span class="room_description-label"></span> <span class="room_description-data">' . $room_name . '</span></div>';

      $options[$room->room_id] = $text;
    }

    // return now to avoid custom sorting.
    return $options;
  }
  elseif ($option == 'room_with_building') {
    foreach ($rooms as $room) {
      $text = '(' . $room->building_code . ') ';
      if (empty($room->room_name) || is_numeric($room->room_number)) {
        $room_number = mcneese_fcs_text_capitalization($room->room_number);

        $text .= $room_number;
        unset($room_number);
      }
      else {
        $text .= mcneese_fcs_text_capitalization($room->room_name);
      }

      $options[$room->room_id] = $text;
    }
  }
  else {
    foreach ($rooms as $room) {
      if (empty($room->room_name) || is_numeric($room->room_number)) {
        $room_number = mcneese_fcs_text_capitalization($room->room_number);

        $options[$room->room_id] = $room_number;
        unset($room_number);
      }
      else {
        $options[$room->room_id] = $room->room_name;
      }
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of dates category options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_dates_category_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_DATES_CATEGORY_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_DATES_CATEGORY_ACTUAL] = 'Event / Meeting';
  $options_all[MFCS_REQUEST_DATES_CATEGORY_SETUP] = 'Rehearsal / Setup';
  $options_all[MFCS_REQUEST_DATES_CATEGORY_CLEANUP] = 'Cleanup / Breakdown';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request intended audience options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_audience_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_AUDIENCE_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_AUDIENCE_GUESTS_ONLY] = 'Invited Guests Only';
  $options_all[MFCS_REQUEST_AUDIENCE_STUDENTS_ONLY] = 'Students Only';
  $options_all[MFCS_REQUEST_AUDIENCE_STUDENTS_AND_EMPLOYEES] = 'Open to all Students and Employees';
  $options_all[MFCS_REQUEST_AUDIENCE_FACULTY_AND_STAFF] = 'Faculty and Staff Only';
  $options_all[MFCS_REQUEST_AUDIENCE_PUBLIC] = 'Open to the Public';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request revenue generated options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_revenue_generated_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_REVENUE_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_REVENUE_TICKET_SALES] = 'Ticket Sales';
  $options_all[MFCS_REQUEST_REVENUE_MERCHANDISE_SALES] = 'Merchandise Sales';
  $options_all[MFCS_REQUEST_REVENUE_CONCESSION_SALES] = 'Concession Sales';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }
  elseif ($option == 'column_name') {
    $options_all[MFCS_REQUEST_REVENUE_TICKET_SALES] = 'ticket';
    $options_all[MFCS_REQUEST_REVENUE_MERCHANDISE_SALES] = 'merchandise';
    $options_all[MFCS_REQUEST_REVENUE_CONCESSION_SALES] = 'concession';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request information type options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_type_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_TYPE_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_TYPE_MEETING] = 'Meeting';
  $options_all[MFCS_REQUEST_TYPE_PERFORMANCE] = 'Performance';
  $options_all[MFCS_REQUEST_TYPE_LECTURE] = 'Lecture';
  $options_all[MFCS_REQUEST_TYPE_RECEPTION] = 'Reception';
  $options_all[MFCS_REQUEST_TYPE_BANQUET] = 'Banquet';
  $options_all[MFCS_REQUEST_TYPE_WORKSHOP] = 'Workshop';
  $options_all[MFCS_REQUEST_TYPE_TALENT] = 'Talent / Variety Show';
  $options_all[MFCS_REQUEST_TYPE_CEREMONY] = 'Ceremony / Presentation';
  $options_all[MFCS_REQUEST_TYPE_FAIR] = 'Fair / Expo';
  $options_all[MFCS_REQUEST_TYPE_ATHLETIC] = 'Athletic';
  $options_all[MFCS_REQUEST_TYPE_OTHER] = 'Other';
  $options_all[MFCS_REQUEST_TYPE_QUICK_MEETING] = 'Quick Meeting';
  $options_all[MFCS_REQUEST_TYPE_CLUB_MEETING] = 'Club Meeting';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request publicity details options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_publicity_details_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_PUBLICITY_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_PUBLICITY_CAMPUS_DIGEST] = 'Campus Digest';
  $options_all[MFCS_REQUEST_PUBLICITY_STUDENT_DIGEST] = 'Student Digest';
  $options_all[MFCS_REQUEST_PUBLICITY_WEBSITE] = 'Website';
  $options_all[MFCS_REQUEST_PUBLICITY_SOCIAL_MEDIA] = 'Social Media';
  $options_all[MFCS_REQUEST_PUBLICITY_AXIS_TVS] = 'Axis TV\'s';
  $options_all[MFCS_REQUEST_PUBLICITY_PRESS_RELEASE] = 'Press Release';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }
  elseif ($option == 'column_name') {
    $options_all[MFCS_REQUEST_PUBLICITY_CAMPUS_DIGEST] = 'campus_digest';
    $options_all[MFCS_REQUEST_PUBLICITY_STUDENT_DIGEST] = 'student_digest';
    $options_all[MFCS_REQUEST_PUBLICITY_WEBSITE] = 'website';
    $options_all[MFCS_REQUEST_PUBLICITY_SOCIAL_MEDIA] = 'social_media';
    $options_all[MFCS_REQUEST_PUBLICITY_AXIS_TVS] = 'axis_tv';
    $options_all[MFCS_REQUEST_PUBLICITY_PRESS_RELEASE] = 'press_release';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request desiging materials options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_designing_material_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_PUBLIC_RELATIONS] = 'McNeese Public Relations';
  $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_MARKETING] = 'McNeese Marketing';
  $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_OTHER] = 'Other';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }
  elseif ($option == 'column_name') {
    $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_PUBLIC_RELATIONS] = 'public_relations';
    $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_MARKETING] = 'marketing';
    $options_all[MFCS_REQUEST_DESIGNING_MATERIALS_OTHER] = 'other';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request technical equipment details options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_technical_equipment_details_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_SCREEN] = 'Projector / Screen';
  $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_COMPUTER] = 'Computer';
  $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_SOUND] = 'Sound for Presentation / Music / Etc.';
  $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_MICROPHONE] = 'Microphone';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }
  elseif ($option == 'column_name') {
    $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_SCREEN] = 'screen';
    $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_COMPUTER] = 'computer';
    $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_SOUND] = 'sound';
    $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_MICROPHONE] = 'microphone';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of review classification options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_reviewer_classification_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  if (!is_bool($hidden)) {
    $hidden = FALSE;
  }

  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REVIEWER_CLASSIFICATION_NONE] = 'None';
  }

  $options_all[MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR] = 'System Administrator';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = 'Manager';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_REQUESTER] = 'Requester';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_SYSTEM] = 'System';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = 'Venue Coordinator';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY] = 'Venue Coordinator Proxy';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_ADMINISTRATIVE_ACCOUNTING] = 'Administrative Accounting';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_CUSTODIAL] = 'Custodial';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_UNIVERSITY_EVENTS] = 'University Events';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_FACILITIES_CUSTODIAL] = 'Facilities / Custodial Services';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_FACILITIES] = 'Facilities';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_FACULTY_ADVISER] = 'Faculty Adviser';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_FOOD_SERVICES] = 'Food Services';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_HOUSING] = 'Housing';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_MAINTENANCE] = 'Maintenance';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_PURCHASING] = 'Purchasing / Insurance';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_SECURITY] = 'Security / University Police';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_STUDENT_ACTIVITIES_ADMINISTRATION] = 'Student Activities Administration';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_UNIVERSITY_SERVICES] = 'University Services';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_GROUNDS] = 'Grounds';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_FINANCIAL] = 'Financial';
  $options_all[MFCS_REVIEWER_CLASSIFICATION_INSURANCE] = 'Insurance';



  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  if (!$hidden) {
    unset($options[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR]);
    unset($options[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY]);
    unset($options[MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR]);
    unset($options[MFCS_REVIEWER_CLASSIFICATION_MANAGER]);
    unset($options[MFCS_REVIEWER_CLASSIFICATION_REQUESTER]);
    unset($options[MFCS_REVIEWER_CLASSIFICATION_SYSTEM]);
  }

  if (!$disabled) {
    unset($options[MFCS_REVIEWER_CLASSIFICATION_FACILITIES_CUSTODIAL]);
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of review decision options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param int $restriction
 *   (optional) An integer representing a set of restrictions in which determines the available list options.
 *   This option was added to address ticket fcs-79 without conflicting with existing behavior.
 *   Its sole purpose is to be used during the review process.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_reviewer_decision_list_options($option = NULL, $restriction = 0, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REVIEW_DECISION_NONE] = 'None';
    $options_all[MFCS_REVIEW_DECISION_CLOSE] = 'Close';
    $options_all[MFCS_REVIEW_DECISION_CANCEL] = 'Cancel';
    $options_all[MFCS_REVIEW_DECISION_UNCANCEL] = 'Uncancel';
    $options_all[MFCS_REVIEW_DECISION_REASSIGN_COORDINATOR] = 'Re-Assign Venue Coordinator';
    $options_all[MFCS_REVIEW_DECISION_OVERRIDE] = 'Override';
    $options_all[MFCS_REVIEW_DECISION_MOVE] = 'Move';
    $options_all[MFCS_REVIEW_DECISION_REASSIGN_REQUESTER] = 'Re-Assign Requester';
    $options_all[MFCS_REVIEW_DECISION_MANAGER_RECHECK] = 'Manager Recheck';
    $options_all[MFCS_REVIEW_DECISION_AMENDMENT] = 'Amendment';
  }

  if ($disabled) {
    $options_all[MFCS_REVIEW_DECISION_WAIVE] = 'Waive';
  }

  $options_all[MFCS_REVIEW_DECISION_APPROVE] = 'Approve';
  $options_all[MFCS_REVIEW_DECISION_COMMENT] = 'Comment';
  $options_all[MFCS_REVIEW_DECISION_COMMENT_ALL] = 'Comment';
  $options_all[MFCS_REVIEW_DECISION_DENY] = 'Deny';
  $options_all[MFCS_REVIEW_DECISION_REQUIREMENT] = 'Requirement';
  $options_all[MFCS_REVIEW_DECISION_AVAILABLE] = 'Available';
  $options_all[MFCS_REVIEW_DECISION_UNAVAILABLE] = 'Unavailable';
  $options_all[MFCS_REVIEW_DECISION_ISSUES] = 'Issues';
  $options_all[MFCS_REVIEW_DECISION_ISSUES_NONE] = 'No Issues';

  if ($option == 'select' || $option == 'review_step') {
    $options[''] = '- Select -';
  }

  if (!is_int($restriction)) {
    $restriction = MFCS_RESTRICTION_DECISION_NONE;
  }

  if ($restriction == MFCS_RESTRICTION_DECISION_NONE) {
    foreach ($options_all as $option_id => $option_name) {
      $options[$option_id] = $option_name;
    }

    // these options are hidden from the users.
    if ($option == 'review_step') {
      unset($options[MFCS_REVIEW_DECISION_AMENDMENT]);
      unset($options[MFCS_REVIEW_DECISION_WAIVE]);
    }

    asort($options);
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_REQUESTER) {
    if (array_key_exists(MFCS_REVIEW_DECISION_COMMENT_ALL, $options_all)) {
      $options[MFCS_REVIEW_DECISION_COMMENT_ALL] = $options_all[MFCS_REVIEW_DECISION_COMMENT_ALL];
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_VENUE_COORDINATOR) {
    foreach (array(MFCS_REVIEW_DECISION_AVAILABLE, MFCS_REVIEW_DECISION_UNAVAILABLE, MFCS_REVIEW_DECISION_COMMENT, MFCS_REVIEW_DECISION_REQUIREMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_REVIEWER) {
    foreach (array(MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_APPROVER) {
    foreach (array(MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_MANAGER) {
    foreach (array(MFCS_REVIEW_DECISION_AVAILABLE, MFCS_REVIEW_DECISION_UNAVAILABLE, MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY, MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_REQUIREMENT, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_ADMINISTER) {
    foreach (array(MFCS_REVIEW_DECISION_AVAILABLE, MFCS_REVIEW_DECISION_UNAVAILABLE, MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY, MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_REQUIREMENT, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_COMMENT_ONLY) {
    if (array_key_exists(MFCS_REVIEW_DECISION_COMMENT, $options_all)) {
      $options[MFCS_REVIEW_DECISION_COMMENT] = $options_all[MFCS_REVIEW_DECISION_COMMENT];
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_FINANCER) {
    foreach (array(MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_COMMENT, MFCS_REVIEW_DECISION_REQUIREMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_INSURER) {
    foreach (array(MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_COMMENT, MFCS_REVIEW_DECISION_REQUIREMENT) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }


  return $options;
}

/**
 * Returns an array of request review step options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_review_step_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REVIEW_STEP_NONE] = 'None';
  }

  $options_all[MFCS_REVIEW_STEP_VENUE_AVAILABLE] = 'Venue Available';
  $options_all[MFCS_REVIEW_STEP_FINAL_DECISION] = 'Final Decision';
  $options_all[MFCS_REVIEW_STEP_RELEASE_HOLD] = 'Release Tentative Hold';
  $options_all[MFCS_REVIEW_STEP_REVIEW] = 'Review';
  $options_all[MFCS_REVIEW_STEP_REQUIREMENTS] = 'Requirements';
  $options_all[MFCS_REVIEW_STEP_COMPLETED] = 'Completed';
  $options_all[MFCS_REVIEW_STEP_CHANGES_REQUIRED] = 'Changes Required';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if ($option == 'roles') {
    asort($options_all);

    if ($hidden) {
      $options_roles[MFCS_REVIEW_STEP_NONE] = array();
    }

    $options_roles[MFCS_REVIEW_STEP_VENUE_AVAILABLE] = array(1, 2, 3, 4);
    $options_roles[MFCS_REVIEW_STEP_FINAL_DECISION] = array(1, 2, 3, 4);
    $options_roles[MFCS_REVIEW_STEP_RELEASE_HOLD] = array();
    $options_roles[MFCS_REVIEW_STEP_REVIEW] = array(1, 2, 3, 4);
    $options_roles[MFCS_REVIEW_STEP_REQUIREMENTS] = array(1, 2, 3, 4);
    $options_roles[MFCS_REVIEW_STEP_COMPLETED] = array();
    $options_roles[MFCS_REVIEW_STEP_CHANGES_REQUIRED] = array();

    foreach ($options_all as $option_id => $option_name) {
      $options[$option_id] = array();
      $options[$option_id]['name'] = $option_name;
      $options[$option_id]['roles'] = $options_roles[$option_id];
    }
  }
  else {
    foreach ($options_all as $option_id => $option_name) {
      $options[$option_id] = $option_name;
    }

    asort($options);
  }

  return $options;
}

/**
 * Returns an array of request problem code names.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_problems_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_REQUEST_PROBLEM_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_PROBLEM_CONFLICT] = 'Conflict';
  $options_all[MFCS_REQUEST_PROBLEM_STALE] = 'Stale';
  $options_all[MFCS_REQUEST_PROBLEM_STUCK] = 'Stuck';
  $options_all[MFCS_REQUEST_PROBLEM_BLOCKED] = 'Blocked';
  $options_all[MFCS_REQUEST_PROBLEM_COORDINATOR] = 'Coordinator Invalid';
  $options_all[MFCS_REQUEST_PROBLEM_REQUESTER] = 'Requester Invalid';
  $options_all[MFCS_REQUEST_PROBLEM_ROOM] = 'Room Invalid';
  $options_all[MFCS_REQUEST_PROBLEM_BUILDING] = 'Building Invalid';
  $options_all[MFCS_REQUEST_PROBLEM_LOCATION] = 'Location Invalid';
  $options_all[MFCS_REQUEST_PROBLEM_CACHE_REQUEST] = 'Cache Invalid';
  $options_all[MFCS_REQUEST_PROBLEM_HOLIDAY] = 'Holiday Conflict';
  $options_all[MFCS_REQUEST_PROBLEM_UNAVAILABLE] = 'Room Unavailable';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request problem log code names.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param string|null $type
 *   (optional) A string designating a specific type to process, such as:
 *   - 'request'
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_log_problems_list_options($option = NULL, $type = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_LOG_PROBLEMS_NONE] = 'None';
  }

  $options_all[MFCS_LOG_PROBLEMS_INSERT] = 'Insert';
  $options_all[MFCS_LOG_PROBLEMS_UPDATE] = 'Update';
  $options_all[MFCS_LOG_PROBLEMS_DELETE] = 'Delete';
  $options_all[MFCS_LOG_PROBLEMS_RESYNC] = 'Re-Synchronize';
  $options_all[MFCS_LOG_PROBLEMS_IGNORE] = 'Ignore';
  $options_all[MFCS_LOG_PROBLEMS_UNIGNORE] = 'Unignore';
  $options_all[MFCS_LOG_PROBLEMS_RECHECK] = 'Re-Check';
  $options_all[MFCS_LOG_PROBLEMS_REASSIGN_VENUE_COORDINATOR] = 'Re-assign Venue Coordinator';
  $options_all[MFCS_LOG_PROBLEMS_REASSIGN_REQUESTER] = 'Re-assign Requester';
  $options_all[MFCS_LOG_PROBLEMS_REASSIGN_REBUILD_CACHE] = 'Rebuild Cache';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if ($type == 'requests') {
    foreach (array(MFCS_LOG_PROBLEMS_NONE, MFCS_LOG_PROBLEMS_IGNORE, MFCS_LOG_PROBLEMS_UNIGNORE, MFCS_LOG_PROBLEMS_RECHECK, MFCS_LOG_PROBLEMS_REASSIGN_VENUE_COORDINATOR, MFCS_LOG_PROBLEMS_REASSIGN_REQUESTER, MFCS_LOG_PROBLEMS_REASSIGN_REBUILD_CACHE) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  elseif ($type == 'basic') {
    foreach (array(MFCS_LOG_PROBLEMS_NONE, MFCS_LOG_PROBLEMS_INSERT, MFCS_LOG_PROBLEMS_UPDATE, MFCS_LOG_PROBLEMS_DELETE, MFCS_LOG_PROBLEMS_RESYNC, MFCS_LOG_PROBLEMS_IGNORE, MFCS_LOG_PROBLEMS_UNIGNORE, MFCS_LOG_PROBLEMS_RECHECK) as $option_id) {
      if (array_key_exists($option_id, $options_all)) {
        $options[$option_id] = $options_all[$option_id];
      }
    }
  }
  else {
    foreach ($options_all as $option_id => $option_name) {
      $options[$option_id] = $option_name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of facilities use unavailability type options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case types.
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_unavailability_type_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_UNAVAILABILITY_TYPE_NONE] = 'None';
  }

  $options_all[MFCS_UNAVAILABILITY_TYPE_RENOVATION] = 'Renovation';
  $options_all[MFCS_UNAVAILABILITY_TYPE_REPAIR] = 'Repair';
  $options_all[MFCS_UNAVAILABILITY_TYPE_REFURBISHMENT] = 'Planned Refurbishment';
  $options_all[MFCS_UNAVAILABILITY_TYPE_SPECIAL_EVENT] = 'Special Event';
  $options_all[MFCS_UNAVAILABILITY_TYPE_OUTAGE] = 'Outage';
  $options_all[MFCS_UNAVAILABILITY_TYPE_EMERGENCY] = 'Emergency';
  $options_all[MFCS_UNAVAILABILITY_TYPE_CLASS] = 'Class';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }
  elseif ($option == 'combined') {
    $options_all = array();
    $options_all[MFCS_UNAVAILABILITY_TYPE_RENOVATION] = 'Renovation / Repair';
    $options_all[MFCS_UNAVAILABILITY_TYPE_REPAIR] = 'Renovation / Repair';
    $options_all[MFCS_UNAVAILABILITY_TYPE_REFURBISHMENT] = 'Planned Refurbishment';
    $options_all[MFCS_UNAVAILABILITY_TYPE_SPECIAL_EVENT] = 'Special Event';
    $options_all[MFCS_UNAVAILABILITY_TYPE_OUTAGE] = 'Emergency / Outage';
    $options_all[MFCS_UNAVAILABILITY_TYPE_EMERGENCY] = 'Emergency / Outage';
    $options_all[MFCS_UNAVAILABILITY_TYPE_CLASS] = 'Class';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of facilities use insurance type options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case types.
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_insurance_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_INSURANCE_NONE] = 'None';
  }

  $options_all[MFCS_INSURANCE_REQUIRED_NOT] = 'Not Required';
  $options_all[MFCS_INSURANCE_REQUIRED] = 'Required';
  $options_all[MFCS_INSURANCE_PROVIDED] = 'Provided';
  $options_all[MFCS_INSURANCE_PROVIDED_NOT] = 'Not Provided';

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of e-mail types.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_email_type_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_EMAIL_TYPE_NONE] = 'None';
  }

  $options_all[MFCS_EMAIL_TYPE_AMENDED] = 'Amended';
  $options_all[MFCS_EMAIL_TYPE_APPROVED] = 'Approved';
  $options_all[MFCS_EMAIL_TYPE_CANCELLED] = 'Cancelled';
  $options_all[MFCS_EMAIL_TYPE_CANCELLING] = 'Cancelling';
  $options_all[MFCS_EMAIL_TYPE_COMMENT] = 'Comment';
  $options_all[MFCS_EMAIL_TYPE_DELETED] = 'Deleted';
  $options_all[MFCS_EMAIL_TYPE_DENIED] = 'Denied';
  $options_all[MFCS_EMAIL_TYPE_FINAL_DECISION] = 'Final Decision';
  $options_all[MFCS_EMAIL_TYPE_FORCED] = 'Forced';
  $options_all[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] = 'Insurance Provided';
  $options_all[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] = 'Insurance Required';
  $options_all[MFCS_EMAIL_TYPE_ISSUES] = 'Issues';
  $options_all[MFCS_EMAIL_TYPE_MODIFIED] = 'Modified';
  $options_all[MFCS_EMAIL_TYPE_MOVED] = 'Moved';
  $options_all[MFCS_EMAIL_TYPE_NEEDS_REVIEW] = 'Needs Review';
  $options_all[MFCS_EMAIL_TYPE_ISSUES_NONE] = 'No Issues';
  $options_all[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] = 'Re-Assign Coordinator';
  $options_all[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] = 'Re-Assign Requester';
  $options_all[MFCS_EMAIL_TYPE_REQUESTED] = 'Requested';
  $options_all[MFCS_EMAIL_TYPE_UNAVAILABLE] = 'Unavailable';
  $options_all[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] = 'Unavailable (Due to Lock)';
  $options_all[MFCS_EMAIL_TYPE_UNCANCELLED] = 'Uncancelled';
  $options_all[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] = 'Unreviewed Request';

  if ($disabled) {
    $options_all[MFCS_EMAIL_TYPE_REQUIREMENTS] = 'Requirements';
    $options_all[MFCS_EMAIL_TYPE_REVIEWED] = 'Reviewed';
  }

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of user calendars options.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case types.
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_user_calendar_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_USER_CALENDAR_NONE] = 'None';
  }

  $options_all[MFCS_USER_CALENDAR_ORIGINAL] = 'Original Calendar';
  $options_all[MFCS_USER_CALENDAR_FULL] = 'Full Calendar';

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of holiday types.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled options in addition to existing options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_holiday_type_list_options($option = NULL, $hidden = FALSE, $disabled = FALSE) {
  $options = array();
  $options_all = array();

  if ($hidden) {
    $options_all[MFCS_HOLIDAY_TYPE_NONE] = 'None';
  }

  $options_all[MFCS_HOLIDAY_TYPE_HOLIDAY] = 'Holiday';
  $options_all[MFCS_HOLIDAY_TYPE_FORCED_CLOSURE] = 'Forced Closure';
  $options_all[MFCS_HOLIDAY_TYPE_BREAK] = 'Break';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  foreach ($options_all as $option_id => $option_name) {
    $options[$option_id] = $option_name;
  }

  asort($options);

  return $options;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
