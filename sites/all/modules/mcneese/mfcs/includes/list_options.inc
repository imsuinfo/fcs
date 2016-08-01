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
 * Returns an array of facilities use status options for populating lists.
 *
 * @fixme: status currently has deleted set to 0 and therefore none and deleted conflict.
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
  $options_all[MFCS_REVIEW_STEP_MAKE_DECISIONS] = 'Resolve Conflicts / Make Decisions';
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

  if ($hidden) {
    $options_all[MFCS_REQUEST_CLASSIFICATION_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_CLASSIFICATION_STUDENT] = 'Student Organization';
  $options_all[MFCS_REQUEST_CLASSIFICATION_CAMPS] = 'Camps';
  $options_all[MFCS_REQUEST_CLASSIFICATION_FACULTY] = 'Faculty / Staff / Athletics';
  $options_all[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] = 'Externals';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  if (is_object($user)) {
    foreach ($options_all as $option_id => $option_name) {
      if (in_array($option_id, $user->roles)) {
        $options[$option_id] = $option_name;
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

  foreach ($buildings as &$building) {
    $options[$building->building_id] = $building->building_name;
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
    foreach ($rooms as $room) {
      $title = '<div class="building_desc">' . check_plain($room->building_name) . '</div>';
      $title .= ' <div class="room_number"><span class="room_number-label">Room</span> <span class="room_number-data">' . check_plain($room->room_number) . '</span></div>';

      if (!empty($room->capacity_normal)) {
        $title .= '<div class="room_capacity"><span class="capacity-label">Capacity:</span> <span class="capacity-data">' . check_plain($room->capacity_normal) . '</span></div>';
      }

      $room_name = 'No information available.';
      if (!empty($room->room_name)) {
        $room_name = check_plain($room->room_name);
      }
      $title .= '<div class="room_description"><span class="room_description-label"></span> <span class="room_description-data">' . $room_name . '</span></div>';

      $options[$room->room_id] = $title;
    }
  }
  else {
    foreach ($rooms as $room) {
      $options[$room->room_id] = empty($room->room_name) ? $room->room_number : $room->room_name;
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
  $options_all[MFCS_REQUEST_DATES_CATEGORY_SETUP] = 'Rehearsel / Setup';
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

  $options_all = array();

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
  $options_all[MFCS_REQUEST_TYPE_TALENT] = 'Talent';
  $options_all[MFCS_REQUEST_TYPE_CEREMONY] = 'Ceremony';
  $options_all[MFCS_REQUEST_TYPE_FAIR] = 'Fair';
  $options_all[MFCS_REQUEST_TYPE_ATHLETIC] = 'Athletic';
  $options_all[MFCS_REQUEST_TYPE_OTHER] = 'Other';
  $options_all[MFCS_REQUEST_TYPE_QUICK_MEETING] = 'Quick Meeting';

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
    $options_all[MFCS_REQUEST_TECHNICAL_EQUIPMENT_MICROPHONE] = 'equipment';
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
    $options_all[MFCS__NONE] = 'None';
  }

  $options_all[MFCS_REVIEW_DECISION_MANAGER_RECHECK] = 'Manager Recheck';
  $options_all[MFCS_REVIEW_DECISION_AMENDMENT] = 'Amendment';
  $options_all[MFCS_REVIEW_DECISION_APPROVE] = 'Approve';
  $options_all[MFCS_REVIEW_DECISION_COMMENT] = 'Comment';
  $options_all[MFCS_REVIEW_DECISION_COMMENT_ALL] = 'Comment';
  $options_all[MFCS_REVIEW_DECISION_DENY] = 'Deny';
  $options_all[MFCS_REVIEW_DECISION_WAIVE] = 'Waive';
  $options_all[MFCS_REVIEW_DECISION_REQUIREMENT] = 'Requirement';
  $options_all[MFCS_REVIEW_DECISION_AVAILABLE] = 'Available';
  $options_all[MFCS_REVIEW_DECISION_UNAVAILABLE] = 'Unavailable';
  $options_all[MFCS_REVIEW_DECISION_ISSUES] = 'Issues';
  $options_all[MFCS_REVIEW_DECISION_ISSUES_NONE] = 'No Issues';

  if ($option == 'select' || $option == 'review_step') {
    $options[''] = '- Select -';
  }

  if (!is_int($restriction) || $restriction < MFCS_RESTRICTION_DECISION_NONE || $restriction > MFCS_RESTRICTION_DECISION_COMMENT_ONLY) {
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
    $options[MFCS_REVIEW_DECISION_COMMENT_ALL] = $options_all[MFCS_REVIEW_DECISION_COMMENT_ALL];
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_VENUE_COORDINATOR) {
    foreach (array(MFCS_REVIEW_DECISION_AVAILABLE, MFCS_REVIEW_DECISION_UNAVAILABLE, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      $options[$option_id] = $options_all[$option_id];
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_REVIEWER) {
    foreach (array(MFCS_REVIEW_DECISION_ISSUES, MFCS_REVIEW_DECISION_ISSUES_NONE, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      $options[$option_id] = $options_all[$option_id];
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_APPROVER) {
    foreach (array(MFCS_REVIEW_DECISION_APPROVE, MFCS_REVIEW_DECISION_DENY, MFCS_REVIEW_DECISION_REQUIREMENT, MFCS_REVIEW_DECISION_COMMENT) as $option_id) {
      $options[$option_id] = $options_all[$option_id];
    }
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_MANAGER) {
    $options[MFCS_REVIEW_DECISION_COMMENT] = $options_all[MFCS_REVIEW_DECISION_COMMENT];
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_ADMINISTER) {
    $options[MFCS_REVIEW_DECISION_COMMENT] = $options_all[MFCS_REVIEW_DECISION_COMMENT];
  }
  elseif ($restriction == MFCS_RESTRICTION_DECISION_COMMENT_ONLY) {
    $options[MFCS_REVIEW_DECISION_COMMENT] = $options_all[MFCS_REVIEW_DECISION_COMMENT];
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
    $options_all[MFCS_REQUEST_STEP_NONE] = 'None';
  }

  $options_all[MFCS_REQUEST_STEP_VENUE_AVAILABLE] = 'Venue Available';
  $options_all[MFCS_REQUEST_STEP_RESOLVE_CONFLICTS] = 'Resolve Conflicts / Make Decisions';
  $options_all[MFCS_REQUEST_STEP_TENTATIVE_HOLD] = 'Release Tentative Hold';
  $options_all[MFCS_REQUEST_STEP_REVIEW] = 'Review';
  $options_all[MFCS_REQUEST_STEP_REQUIREMENTS] = 'Requirements';
  $options_all[MFCS_REQUEST_STEP_COMPLETED] = 'Completed';
  $options_all[MFCS_REQUEST_STEP_CHANGES_REQUIRED] = 'Changes Required';

  if ($option == 'select') {
    $options[''] = '- Select -';
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_STEPS, 0, NULL, TRUE);

  if ($option == 'roles') {
    asort($options_all);

    if ($hidden) {
      $options_roles[MFCS_REQUEST_STEP_NONE] = array();
    }

    $options_roles[MFCS_REQUEST_STEP_VENUE_AVAILABLE] = array(1, 2, 3, 4);
    $options_roles[MFCS_REQUEST_STEP_RESOLVE_CONFLICTS] = array(1, 2, 3, 4);
    $options_roles[MFCS_REQUEST_STEP_TENTATIVE_HOLD] = array();
    $options_roles[MFCS_REQUEST_STEP_REVIEW] = array(1, 2, 3, 4);
    $options_roles[MFCS_REQUEST_STEP_REQUIREMENTS] = array(1, 2, 3, 4);
    $options_roles[MFCS_REQUEST_STEP_COMPLETED] = array();
    $options_roles[MFCS_REQUEST_STEP_CHANGES_REQUIRED] = array();

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
 * @} End of '@addtogroup mfcs'.
 */
