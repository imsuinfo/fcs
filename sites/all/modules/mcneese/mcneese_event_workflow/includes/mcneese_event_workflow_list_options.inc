<?php

/**
 * @file
 * Defines McNeese event workflow list options functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Returns an array of event workflow status options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_status_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options[MEW_EVENT_STATUS_DELETED] = "Deleted";
  $options[MEW_EVENT_STATUS_LOCKED] = "Queued (Locked)";
  $options[MEW_EVENT_STATUS_UNLOCKED] = "Queued (Unlocked)";
  $options[MEW_EVENT_STATUS_CLOSED_ACCEPTED] = "Accepted";
  $options[MEW_EVENT_STATUS_CLOSED_REJECTED] = "Rejected";
  $options[MEW_EVENT_STATUS_CLOSED_UNAVAILABLE] = "Room Unavailable";
  $options[MEW_EVENT_STATUS_CLOSED_DUE_TO_LOCK] = "Room Unavailable (due to lock)";
  $options[MEW_EVENT_STATUS_CANCELLED] = "Cancelled";

  asort($options);

  return $options;
}

/**
 * Returns an array of event workflow state options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_steps_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_STEPS, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of event classification options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 * @param object|null $user
 *   (optional) When not NULL, the user to build the available options against.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_classification_list_options($option = NULL, $user = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_CLASSIFICATION, 0, NULL, TRUE);

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
 * Returns an array of event category options for populating lists.
 *
 * Note: ucs.msu_venue_list calls the building location 'category'.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_location_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_LOCATION, 0, NULL, TRUE);
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

  return $options;
}

/**
 * Returns an array of event building list for populating lists.
 *
 * @param string|null $location
 *   (optional) When not NULL, select options based on provided location.
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_building_list_options($location = NULL, $option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $buildings = mcneese_event_workflow_load_buildings($location);

  foreach ($buildings as &$building) {
    $options[$building['building_code']] = $building['building_desc'];
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of event room list for populating lists.
 *
 * @param string|null $location
 *   (optional) When not NULL, select options based on provided location.
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_room_list_options($location = NULL, $option = NULL) {
  $options = array();

  $buildings = mcneese_event_workflow_load_buildings($location);

  if ($option == 'special') {
    foreach ($buildings as &$building) {
      if (empty($location)) {
        $id = 'undecided_';
      }
      else {
        $id = check_plain($building['category']) . '_';
      }

      $id .= check_plain($building['building_code']) . '_' . check_plain($building['room_number']);
      $title = '<div class="building_desc">' . check_plain($building['building_desc']) . '</div>';
      $title .= ' <div class="room_number"><span class="room_number-label">Room</span> <span class="room_number-data">' . check_plain($building['room_number']) . '</span></div>';

      if (!empty($building['capacity'])) {
        $title .= '<div class="room_capacity"><span class="capacity-label">Capacity:</span> <span class="capacity-data">' . check_plain($building['capacity']) . '</span></div>';
      }

      $room_desc = 'No information available.';
      if (!empty($building['room_desc'])) {
        $room_desc = check_plain($building['room_desc']);
      }
      $title .= '<div class="room_description"><span class="room_description-label"></span> <span class="room_description-data">' . $room_desc . '</span></div>';

      $options[$id] = $title;
    }
  }
  else {
    foreach ($buildings as &$building) {
      $options[$building['room_number']] = empty($building['room_desc']) ? $building['room_number'] : $building['room_desc'];
    }
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of event intended audience options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_audience_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_AUDIENCE, 0, NULL, TRUE);
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
 * Returns an array of event revenue generated options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_revenue_generated_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_REVENUE_GENERATED, 0, NULL, TRUE);
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
 * Returns an array of event information type options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_information_type_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_INFORMATION_TYPE, 0, NULL, TRUE);
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
 * Returns an array of event publicity details options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_publicity_details_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_PUBLICITY_DETAILS, 0, NULL, TRUE);
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
 * Returns an array of event desiging materials options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_designing_material_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_DESIGNING_MATERIAL, 0, NULL, TRUE);
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
 * Returns an array of event technical equipment details options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_technical_equipment_details_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_TECHNICAL_EQUIPMENT_DETAILS, 0, NULL, TRUE);
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
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_reviewer_classification_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_REVIEWER_CLASSIFICATION, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
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
function event_workflow_get_reviewer_decision_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_REVIEWER_DECISION, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
  }

  asort($options);

  return $options;
}

/**
 * Returns an array of event review step options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_review_step_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(MEW_VID_EVENT_STEPS, 0, NULL, TRUE);

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
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
