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
 * Returns an array of event workflow state options for populating lists.
 *
 * @param string|null $option
 *   (optional) A string designating how the list will be populated.
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_workflow_states_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(2, 0, NULL, TRUE);
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
 *
 * @return array
 *   An array of options available.
 */
function event_workflow_get_event_classification_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $options_tree = taxonomy_get_tree(1, 0, NULL, TRUE);
  foreach ($options_tree as $branch) {
    $options[$branch->tid] = $branch->name;
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
function event_workflow_get_location_list_options($option = NULL) {
  $options = array();

  if ($option == 'select') {
    $options[''] = "- Select -";
  }

  $system_column_name = FALSE;
  if ($option == 'column_name') {
    $system_column_name = TRUE;
  }

  $options_tree = taxonomy_get_tree(7, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(9, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(15, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(8, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(11, 0, NULL, TRUE);
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
function event_workflow_get_room_list_options($location = NULL, $option = 'select') {
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

  $options_tree = taxonomy_get_tree(16, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(10, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(17, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(18, 0, NULL, TRUE);
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

  $options_tree = taxonomy_get_tree(19, 0, NULL, TRUE);

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
