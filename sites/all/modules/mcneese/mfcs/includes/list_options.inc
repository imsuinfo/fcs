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
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_status_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options[MFCS_REQUEST_STATUS_DELETED] = "Deleted";
  $options[MFCS_REQUEST_STATUS_LOCKED] = "Queued (Locked)";
  $options[MFCS_REQUEST_STATUS_UNLOCKED] = "Queued (Unlocked)";
  $options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] = "Accepted";
  $options[MFCS_REQUEST_STATUS_CLOSED_DENIED] = "Denied";
  $options[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] = "Room Unavailable";
  $options[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] = "Room Unavailable (due to lock)";
  $options[MFCS_REQUEST_STATUS_CANCELLED] = "Cancelled";
  $options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] = "Accepted (Cancelled)";

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
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_steps_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_STEPS, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
  }

  if ($option == 'search') {
    unset($options[121]);
    unset($options[120]);
    unset($options[119]);
    unset($options[118]);
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
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_classification_list_options($option = NULL, $user = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_CLASSIFICATION, 0, NULL, TRUE);

  if (is_object($user)) {
    foreach ($options_tree as $branch) {
      if (isset($branch->field_role_name['und'][0]['value'])) {
        if (in_array($branch->field_role_name['und'][0]['value'], $user->roles)) {
          $options[$branch->tid] = $branch->name;
        }
      }
    }
  }
  else {
    foreach ($options_tree as $branch) {
      $options[$branch->tid] = $branch->name;
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
 * @param bool $taxonomy
 *   (optional) When TRUE, will load the taxonomy term data. When FALSE will
 *   load the {mfcs_banner_locations} data.
 * @param bool $disabled
 *   (optional) When TRUE, load disabled rooms in addition to existing rooms.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_location_list_options($option = NULL, $taxonomy = FALSE, $disabled = FALSE) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  if ($taxonomy) {
    $system_column_name = FALSE;
    if ($option == 'column_name') {
      $system_column_name = TRUE;
    }

    $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_LOCATION, 0, NULL, TRUE);
    foreach ($options_tree as $branch) {
      if ($system_column_name) {
        if (!empty($branch->field_system_column_name['und'][0]['value'])) {
          $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
        }
      }
      else {
        $branch_category = '';
        if (isset($branch->field_column_venue_category['und'][0]['value'])) {
          $branch_category = $branch->field_column_venue_category['und'][0]['value'];
        }

        $options[$branch_category] = $branch->name;
      }
    }

    asort($options);
  }
  else {
    $locations = mfcs_load_locations(NULL, $disabled);

    if (is_array($locations)) {
      foreach ($locations as $location) {
        $options[$location->location_id] = $location->location_name;
      }
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
 * @param bool $disabled
 *   (optional) When TRUE, load disabled rooms in addition to existing rooms.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_building_list_options($location = NULL, $option = NULL, $disabled = FALSE) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
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
 * @param bool $disabled
 *   (optional) When TRUE, load disabled rooms in addition to existing rooms.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_room_list_options($location = NULL, $option = NULL, $disabled = FALSE) {
  $options = array();

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
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_dates_category_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_DATES_CATEGORY, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request intended audience options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_audience_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_AUDIENCE, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    if ($system_column_name) {
      if (!empty($branch->field_system_column_name['und'][0]['value'])) {
        $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
      }
    }
    else {
      $options[$branch->tid] = $branch->name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request revenue generated options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_revenue_generated_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_REVENUE_GENERATED, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    if ($system_column_name) {
      if (!empty($branch->field_system_column_name['und'][0]['value'])) {
        $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
      }
    }
    else {
      $options[$branch->tid] = $branch->name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request information type options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_request_type_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_REQUEST_TYPE, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    if ($system_column_name) {
      if (!empty($branch->field_system_column_name['und'][0]['value'])) {
        $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
      }
    }
    else {
      $options[$branch->tid] = $branch->name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request publicity details options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_publicity_details_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_PUBLICITY_DETAILS, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    if ($system_column_name) {
      if (!empty($branch->field_system_column_name['und'][0]['value'])) {
        $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
      }
    }
    else {
      $options[$branch->tid] = $branch->name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request desiging materials options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_designing_material_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_DESIGNING_MATERIAL, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    if ($system_column_name) {
      if (!empty($branch->field_system_column_name['und'][0]['value'])) {
        $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
      }
    }
    else {
      $options[$branch->tid] = $branch->name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request technical equipment details options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_technical_equipment_details_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_TECHNICAL_EQUIPMENT_DETAILS, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    if ($system_column_name) {
      if (!empty($branch->field_system_column_name['und'][0]['value'])) {
        $options[$branch->tid] = $branch->field_system_column_name['und'][0]['value'];
      }
    }
    else {
      $options[$branch->tid] = $branch->name;
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of review classification options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param bool $include_hidden
 *   (optional) When TRUE, will include hidden/special case classifications
 *   that are not normally exposed through the select list options.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_reviewer_classification_list_options($option = NULL, $include_hidden = FALSE) {
  if (!is_bool($include_hidden)) {
    $include_hidden = FALSE;
  }

  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_REVIEWER_CLASSIFICATION, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
  }

  if ($include_hidden) {
    $options[MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR] = "Venue Coordinator";
    $options[MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR] = "System Administrator";
    $options[MFCS_REVIEWER_CLASSIFICATION_MANAGER] = "Manager";
    $options[MFCS_REVIEWER_CLASSIFICATION_REQUESTER] = "Requester";
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of review decision options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_reviewer_decision_list_options($option = NULL) {
  $options = array();

  if ($option == 'select' || $option == 'review_step') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_REVIEWER_DECISION, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
  }

  // these options are hidden from the users.
  if ($option == 'review_step') {
    unset($options[MFCS_REVIEW_DECISION_AMENDMENT]);
    unset($options[MFCS_REVIEW_DECISION_WAVE]);
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of request review step options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function mfcs_get_review_step_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MFCS_VID_REQUEST_STEPS, 0, NULL, TRUE);

  if ($option == 'object') {
    $unsorted = array();
    $sorted = array();
    foreach ($options_tree as $branch) {
      $unsorted[$branch->tid] = $branch;
      $sorted[$branch->tid] = $branch->name;
    }

    asort($sorted);

    foreach ($sorted as $tid => $name) {
      $options[$tid] = $unsorted[$tid];
    }
  }
  else {
    foreach ($options_tree as $branch) {
      $options[$branch->tid] = $branch->name;
    }

    asort($options);
  }

  return $options;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
