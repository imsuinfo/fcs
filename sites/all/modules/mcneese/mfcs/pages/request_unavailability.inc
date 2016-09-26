<?php

/**
 * @file
 * Defines McNeese facilities use request unavailability page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */


/**
 * @} End of '@addtogroup mfcs'.
 */

/**
 * Provides the facilities use request unavailability main page(s).
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_unavailability_0_rooms_page() {
  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }
  mfcs_add_canonical_headers('requests/unavailable-0');
  drupal_set_title("Room Unavailability");

  $user = cf_current_user();

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


  $class_building_id = '';
  $class_building_code = '';
  $class_building_name = '';
  $class_room_id = '';
  $class_room_number = '';
  $class_room_name = '';
  $class_operations = 'no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['room_id'] = 'ASC';
    $order_column = array(3 => strtolower($sorting['room_id']));
    $class_room_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'building_id') {
      $sorting['building_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_building_id .= 'sorting_1 ';
    }
    elseif ($order == 'building_code') {
      $sorting['building_code'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_building_id .= 'sorting_1 ';
    }
    elseif ($order == 'building_name') {
      $sorting['building_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_building_name .= 'sorting_1 ';
    }
    elseif ($order == 'room_id') {
      $sorting['room_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_room_id .= 'sorting_1 ';
    }
    elseif ($order == 'room_number') {
      $sorting['room_number'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_room_number .= 'sorting_1 ';
    }
    elseif ($order == 'room_name') {
      $sorting['room_name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_room_name .= 'sorting_1 ';
    }
    else {
      $sorting['room_id'] = 'ASC';
      $order_column = array(3 => strtolower($sorting['room_id']));
      $class_room_id .= 'sorting_1 ';
    }
  }

  $is_manager = user_access('mfcs manage', $user);
  $is_administer = user_access('mfcs administer', $user);
  $can_troubleshoot = mfcs_page_generic_access('troubleshoot', NULL, $user);

  $items = mfcs_unavailability_rooms();
  if ($items === FALSE) {
    $items = array();
  }

  if ($is_manager || $is_administer) {
    $items_editable = &$items;
  }
  else {
    $items_editable = mfcs_unavailability_rooms($user->uid);
    if ($items_editable === FALSE) {
      $items_editable = array();
    }
  }

  $rows = array();
  foreach ($items as $item) {
    $row = array();

    $row['building_id'] = array(
      'data' => check_plain($item->building_id),
      'class' => $class_building_id,
    );

    $row['building_code'] = array(
      'data' => check_plain($item->building_code),
      'class' => $class_building_code,
    );

    $row['building_name'] = array(
      'data' => check_plain($item->building_name),
      'class' => $class_building_name,
    );

    $row['room_id'] = array(
      'data' => (int) $item->id,
      'class' => $class_room_id,
    );

    $row['room_number'] = array(
      'data' => check_plain($item->room_number),
      'class' => $class_room_number,
    );

    $row['room_name'] = array(
      'data' => check_plain($item->room_name),
      'class' => $class_room_name,
    );

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    $row['operations']['data'] = '';

    if ($can_troubleshoot) {
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/troubleshoot-0/buildings/' . $item->building_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Building Information">Building</a> ';
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $item->id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Room Information">Room</a> ';
    }

    if (array_key_exists($item->id, $items_editable)) {
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/unavailable-0/' . $item->id . $url_arguments . '" class="operation-link operation-link-manage" title="Manage Unavailability for this Room">Manage</a> ';
    }
    else {
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/unavailable-0/' . $item->id . $url_arguments . '" class="operation-link operation-link-manage" title="View Unavailability for this Room">View</a> ';
    }

    $rows[] = $row;
  }


  $availability_list = array();
  $availability_list['caption'] = "Room Unavailability";
  $availability_list['header'] = array(
    'building_id' => array('field' => 'building_id', 'data' => "Building ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'building_code' => array('field' => 'building_code', 'data' => "Building Code", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'building_name' => array('field' => 'building_name', 'data' => "Building Name", 'align' => 'center'),
    'room_id' => array('field' => 'room_id', 'data' => "Room ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
    'room_number' => array('field' => 'room_number', 'data' => "Room Number", 'align' => 'center'),
    'room_name' => array('field' => 'room_name', 'data' => "Room Name", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $availability_list['attributes'] = array();
  $availability_list['attributes']['id'] = 'mfcs-unavailability_manage_page';
  $availability_list['attributes']['class'] = array(
    'no_default',
    'mfcs-unavailability_manage_page',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $availability_list['attributes']['summary'] = 'List of Rooms to Manage Unavailability of.';

  $availability_list['rows'] = $rows;
  $availability_list['empty'] = "There are no rooms for you to view or manage the unavailability of.";

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
    'processing' => FALSE,
    'serverSide' => FALSE,
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'building_id', 'name' => "Building ID", 'className' => 'column-building_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 1, 'column' => 'building_code', 'name' => "Building Code", 'className' => 'column-building_code', 'visible' => FALSE),
      array('id' => 2, 'column' => 'building_name', 'name' => "Building Name", 'className' => 'column-building_name'),
      array('id' => 3, 'column' => 'room_id', 'name' => "Room ID", 'className' => 'column-room_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 4, 'column' => 'room_number', 'name' => "Room Number", 'className' => 'column-room_number'),
      array('id' => 5, 'column' => 'room_name', 'name' => "Room Name", 'className' => 'column-room_name'),
      array('id' => 6, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => 0,
    'pageLength' => 40,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $availability_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $availability_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $markup = '<div id="mfcs-unavailable_0-manage_rooms-page">';

  $markup .= '<div class="no-print margin-bottom-15">';
  $markup .= 'You may also <a href="' . $base_path . 'requests/unavailable-0/all' . $url_arguments . '" class="operation-link operation-link-manage">view and manage all unavavailabilities</a> instead of viewing and managing unavailabilities per room.';
  $markup .= '</div>';

  $markup .= mcneese_libraries_theme_table($availability_list, $datatable_settings);
  $markup .= '<br class="clear">';
  $markup .= '</div>';

  return $markup;
}

/**
 * Provides the facilities use request unavailability room page(s).
 *
 * @param int|NULL $room_id
 *   The ID of the room to render a page for.
 *   When set to NULL, all possible rooms are loaded.
 * @param int|null $year
 *   (optional) The year to load the room unavailability listing for.
 *   When NULL the current year is used.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_unavailability_0_room_page($room_id, $year = NULL) {
  if (!cf_is_integer($room_id) && !is_null($room_id)) {
    cf_error::invalid_integer('room_id');

    drupal_not_found();
    drupal_exit();
  }
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $instance = mfcs_instance();
  $current_year = date('Y', $instance);
  if (is_null($year)) {
    $year = $current_year;
  }

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  // do not provide a page for a non-existent room id.
  if (is_null($room_id)) {
    mfcs_add_canonical_headers('requests/unavailable-0/all/' . $year);

    drupal_set_title($year . ' Room Unavailabilities');
  }
  else {
    $room = mfcs_load_rooms(NULL, NULL, $room_id, TRUE);
    if (!is_object($room)) {
      drupal_not_found();
      drupal_exit();
    }

    mfcs_add_canonical_headers('requests/unavailable-0/' . $room_id . '/' . $year);

    drupal_set_title($year . ' Room Unavailability for ' . $room->building_code . ' ' . $room->room_number);
  }

  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_MANAGEMENT);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

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

  $class_id = '';
  $class_date = '';
  $class_title = '';
  $class_type = '';
  $class_enabled = '';
  $class_building_id = '';
  $class_building_code = '';
  $class_building_name = '';
  $class_room_id = '';
  $class_room_number = '';
  $class_room_name = '';
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
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    elseif ($order == 'title') {
      $sorting['title'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_title .= 'sorting_1 ';
    }
    elseif ($order == 'type') {
      $sorting['type'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_type .= 'sorting_1 ';
    }
    elseif ($order == 'enabled') {
      $sorting['enabled'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_enabled .= 'sorting_1 ';
    }
    else {
      if (is_null($room_id)) {
        if ($order == 'building_id') {
          $sorting['building_id'] = is_null($sort) ? 'DESC' : $sort;
          $order_column = array(5 => strtolower($sorting[$order]));
          $class_building_id .= 'sorting_1 ';
        }
        elseif ($order == 'building_code') {
          $sorting['building_code'] = is_null($sort) ? 'DESC' : $sort;
          $order_column = array(6 => strtolower($sorting[$order]));
          $class_building_id .= 'sorting_1 ';
        }
        elseif ($order == 'building_name') {
          $sorting['building_name'] = is_null($sort) ? 'DESC' : $sort;
          $order_column = array(7 => strtolower($sorting[$order]));
          $class_building_name .= 'sorting_1 ';
        }
        elseif ($order == 'room_id') {
          $sorting['room_id'] = is_null($sort) ? 'DESC' : $sort;
          $order_column = array(8 => strtolower($sorting[$order]));
          $class_room_id .= 'sorting_1 ';
        }
        elseif ($order == 'room_number') {
          $sorting['room_number'] = is_null($sort) ? 'DESC' : $sort;
          $order_column = array(9 => strtolower($sorting[$order]));
          $class_room_number .= 'sorting_1 ';
        }
        elseif ($order == 'room_name') {
          $sorting['room_name'] = is_null($sort) ? 'DESC' : $sort;
          $order_column = array(10 => strtolower($sorting[$order]));
          $class_room_name .= 'sorting_1 ';
        }
        else {
          $sorting['id'] = 'ASC';
          $order_column = array(0 => strtolower($sorting['id']));
          $class_id .= 'sorting_1 ';
        }
      }
      else {
        $sorting['id'] = 'ASC';
        $order_column = array(0 => strtolower($sorting['id']));
        $class_id .= 'sorting_1 ';
      }
    }
  }

  $is_reviewer = user_access('mfcs manage', $user);
  $is_manager = user_access('mfcs manage', $user);
  $is_administer = user_access('mfcs administer', $user);
  $can_troubleshoot = mfcs_page_generic_access('troubleshoot', NULL, $user);

  if (is_null($room_id)) {
    $can_create = FALSE;
  }
  else {
    $can_create = mfcs_unavailability_request_access('create', $year, $room_id, NULL, $user->uid);
  }

  $unavailability_types = mfcs_get_unavailability_type_list_options('combined');

  $items_enabled = TRUE;
  if ($is_reviewer || $is_manager || $is_administer) {
    $items_enabled = NULL;
  }

  $items = mfcs_unavailability_rooms_room($room_id, $year, NULL, $items_enabled);
  if ($items === FALSE) {
    $items = array();
  }

  if ($is_manager || $is_administer) {
    $items_editable = &$items;
  }
  else {
    $items_editable = mfcs_unavailability_rooms_room($room_id, $year, $user->uid, $items_enabled);
    if ($items_editable === FALSE) {
      $items_editable = array();
    }
  }

  $rows = array();
  foreach ($items as $item) {
    $row = array();
    $row['id'] = array(
      'data' => (int) $item->id,
      'class' => $class_id,
    );

    $date = '';
    $date_start = (int) $item->date_start;
    $date_stop = (int) $item->date_stop;

    if ($date_start == strtotime('midnight today', $date_start) && $date_stop == strtotime('midnight tomorrow', $date_start)) {
      $date = date('l, F jS, Y', $date_start);
    }
    else {
      if (strtotime('midnight today', $date_start) == strtotime('midnight today', $date_stop)) {
        $date = date(MFCS_DATE_FORMAT_TIME, $date_start) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $date_stop) . ' on ' . date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_start);
      }
      elseif ($date_start != strtotime('midnight today', $date_start) || $date_stop != strtotime('midnight today', $date_stop)) {
        $date = date(MFCS_DATE_FORMAT_DATE_TIME_SQUEEZE, $date_start) . ' to<br>' . date(MFCS_DATE_FORMAT_DATE_TIME_SQUEEZE, $date_stop);
      }
      else {
        // For whatever reason, the culture treats the stop date inclusively when it has no time (but then treats it inclusively when there is a time!).
        // Make sure the stop date that is printed to the screen is an inclusive date (this therefore cannot be used for any calculations)
        $date = date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $date_start) . ' to<br>' . date(MFCS_DATE_FORMAT_DATE_SQUEEZE, strtotime('midnight yesterday', $date_stop));
      }
    }

    $row['title'] = array(
      'data' => check_plain($item->title),
      'class' => $class_title,
    );

    $row['type'] = array(
      'data' => (array_key_exists($item->type, $unavailability_types) ? check_plain($unavailability_types[$item->type]) : 'Unknown'),
      'class' => $class_type,
    );

    $row['date'] = array(
      'data' => $date,
      'class' => $class_date,
    );
    unset($date);

    $row['enabled'] = array(
      'data' => $item->enabled ? 'Yes' : 'No',
      'class' => $class_enabled,
    );

    if (is_null($room_id)) {
      $row['building_id'] = array(
        'data' => check_plain($item->building_id),
        'class' => $class_building_id,
      );

      $row['building_code'] = array(
        'data' => check_plain($item->building_code),
        'class' => $class_building_code,
      );

      $row['building_name'] = array(
        'data' => check_plain($item->building_name),
        'class' => $class_building_name,
      );

      $row['room_id'] = array(
        'data' => (int) $item->id,
        'class' => $class_room_id,
      );

      $row['room_number'] = array(
        'data' => check_plain($item->room_number),
        'class' => $class_room_number,
      );

      $row['room_name'] = array(
        'data' => check_plain($item->room_name),
        'class' => $class_room_name,
      );
    }

    $row['operations']['data'] = '';
    $row['operations']['data'] .= '<a href="' . $base_path . 'requests/unavailable-0/view/' . $year . '/' . $item->room_id . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-view" title="View Unavailability">View</a> ';

    if ($year >= $current_year && array_key_exists($item->id, $items_editable) && is_null($item->class_id)) {
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/unavailable-0/edit/' . $year . '/' . $item->room_id . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-edit" title="Edit Unavailability">Edit</a> ';
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/unavailable-0/create/' . $year . '/' . $item->room_id . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-copy" title="Edit Unavailability">Copy</a> ';
      $row['operations']['data'] .= '<a href="' . $base_path . 'requests/unavailable-0/delete/' . $year . '/' . $item->room_id . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-delete" title="Delete Unavailability">Delete</a> ';
    }

    $rows[] = $row;
  }

  $availability_list = array();
  $columns = array();
  if (is_null($room_id)) {
    $availability_list['caption'] = 'All ' . $year . ' Room Unavailabilities';

    $availability_list['header'] = array(
      'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'title' => array('field' => 'title', 'data' => "Title", 'align' => 'center'),
      'type' => array('field' => 'type', 'data' => "Type", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
      'enabled' => array('field' => 'enabled', 'data' => "Enabled", 'align' => 'center'),
      'building_id' => array('field' => 'building_id', 'data' => "Building ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'building_code' => array('field' => 'building_code', 'data' => "Building Code", 'align' => 'center'),
      'building_name' => array('field' => 'building_name', 'data' => "Building Name", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'room_id' => array('field' => 'room_id', 'data' => "Room ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'room_number' => array('field' => 'room_number', 'data' => "Room Number", 'align' => 'center'),
      'room_name' => array('field' => 'room_name', 'data' => "Room Name", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
    );

    $columns = array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 1, 'column' => 'title', 'name' => "Title", 'className' => 'column-title'),
      array('id' => 2, 'column' => 'type', 'name' => "Type", 'className' => 'column-type', 'visible' => FALSE),
      array('id' => 3, 'column' => 'date', 'name' => "Date", 'className' => 'column-date'),
      array('id' => 4, 'column' => 'enabled', 'name' => "Enabled", 'className' => 'column-enabled'),
      array('id' => 5, 'column' => 'building_id', 'name' => "Building ID", 'className' => 'column-building_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 6, 'column' => 'building_code', 'name' => "Building Code", 'className' => 'column-building_code'),
      array('id' => 7, 'column' => 'building_name', 'name' => "Building Name", 'className' => 'column-building_name', 'visible' => FALSE),
      array('id' => 8, 'column' => 'room_id', 'name' => "Room ID", 'className' => 'column-room_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 9, 'column' => 'room_number', 'name' => "Room Number", 'className' => 'column-room_number'),
      array('id' => 10, 'column' => 'room_name', 'name' => "Room Name", 'className' => 'column-room_name', 'visible' => FALSE),
      array('id' => 11, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    );
  }
  else {
    $availability_list['caption'] = '(' . check_plain($room->building_name) . ') ' . check_plain($room->room_name) . ' Unavailability';

    $availability_list['header'] = array(
      'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center', 'class' => array('no-script-hidden', 'script-hidden')),
      'title' => array('field' => 'title', 'data' => "Title", 'align' => 'center'),
      'type' => array('field' => 'type', 'data' => "Type", 'align' => 'center'),
      'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
      'enabled' => array('field' => 'enabled', 'data' => "Enabled", 'align' => 'center'),
      'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
    );

    $columns = array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 1, 'column' => 'title', 'name' => "Title", 'className' => 'column-title'),
      array('id' => 2, 'column' => 'type', 'name' => "Type", 'className' => 'column-type'),
      array('id' => 3, 'column' => 'date', 'name' => "Date", 'className' => 'column-date'),
      array('id' => 4, 'column' => 'enabled', 'name' => "Enabled", 'className' => 'column-enabled'),
      array('id' => 5, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    );
  }

  $availability_list['attributes'] = array();
  $availability_list['attributes']['id'] = 'mfcs-unavailability_manage_room_page';
  $availability_list['attributes']['class'] = array(
    'no_default',
    'mfcs-unavailability_manage_room_page',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $availability_list['attributes']['summary'] = 'List of dates marked as unavailable for the specified room.';

  $availability_list['rows'] = $rows;
  $availability_list['empty'] = t("There are no dates marked as unavailable for this room.");

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
    'processing' => FALSE,
    'serverSide' => FALSE,
    'deferRender' => TRUE,
    'columns' => $columns,
    'displayStart' => 0,
    'pageLength' => 40,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $availability_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $availability_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $markup = '<div id="mfcs-unavailable_0-manage_room-page">';

  if (is_null($room_id)) {
    $markup .= '<div class="unavailable_0_links-year no-print">';
    $markup .= '<a href="' . $base_path . 'requests/unavailable-0/' . ($year - 1) . '/all' . $url_arguments . '" class="year-previous inline-block margin-10 float-left" title="Unavailabilities for ' . ($year - 1) . '">Previous Year</a> ';
    $markup .= '<a href="' . $base_path . 'requests/unavailable-0/' . ($year + 1) . '/all' . $url_arguments . '" class="year-next inline-block margin-10 float-right" title="Unavailabilities for ' . ($year + 1) . '">Next Year</a> ';
    $markup .= '</div>';
    $markup .= '<br class="clear">';
  }
  else {
    if ($can_troubleshoot) {
      $building_value = '<a href="' . $base_path . 'requests/troubleshoot-0/buildings/' . $room->building_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Building Information">' . check_plain($room->building_name) . '</a>';
      $room_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Room Information">' . check_plain($room->room_name) . '</a>';
    }
    else {
      $building_value = check_plain($room->building_name);
      $room_value = check_plain($room->room_name);
    }

    $markup .= '<div class="room_information-wrapper">';
    $markup .= '<div class="room_information-item item-building_name">';
    $markup .= '<div class="room_information-item-label">Building Name:</div>';
    $markup .= '<div class="room_information-item-value">' . $building_value . '</div>';
    $markup .= '</div>';
    $markup .= '<div class="room_information-item item-room_name">';
    $markup .= '<div class="room_information-item-label">Room Name:</div>';
    $markup .= '<div class="room_information-item-value">' . $room_value . '</div>';
    $markup .= '</div>';
    $markup .= '</div>';

    if ($can_create) {
      $markup .= '<div class="no-print">';
      $markup .= 'You may <a href="' . $base_path . 'requests/unavailable-0/create/' . $year . '/' . $room_id . $url_arguments . '">create a new unavailable date and time</a> for this room.<br>';
      $markup .= 'You may <a href="' . $base_path . 'requests/unavailable-0/copy/' . $year . '/' . $room_id . $url_arguments . '">copy all unavailable dates and times</a> for this room from another year (Type \'Class\' is never copied).';
      $markup .= '</div>';
    }

    $markup .= '<div class="unavailable_0_links-year no-print">';
    $markup .= '<a href="' . $base_path . 'requests/unavailable-0/' . ($year - 1) . '/' . $room_id . $url_arguments . '" class="year-previous inline-block margin-10 float-left" title="Unavailabilities for ' . ($year - 1) . '">Previous Year</a> ';
    $markup .= '<a href="' . $base_path . 'requests/unavailable-0/' . ($year + 1) . '/' . $room_id . $url_arguments . '" class="year-next inline-block margin-10 float-right" title="Unavailabilities for ' . ($year + 1) . '">Next Year</a> ';
    $markup .= '</div>';
    $markup .= '<br class="clear">';
  }

  $markup .= mcneese_libraries_theme_table($availability_list, $datatable_settings);
  $markup .= '<br class="clear">';
  $markup .= '</div>';

  return $markup;
}

/**
 * Renders a single unavailability entry.
 *
 * @param int $year
 *   The year associated with the unavailability entry.
 * @param int $room_id
 *   The ID of the room to render a page for.
 * @param int $unavailability_id
 *   The unique id associated with a specific unavailability entry.
 */
function mfcs_request_unavailability_0_page($year, $room_id, $unavailability_id) {
  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($room_id)) {
    cf_error::invalid_integer('room_id');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($unavailability_id)) {
    cf_error::invalid_integer('unavailability_id');

    drupal_not_found();
    drupal_exit();
  }

  $user = cf_current_user();

  $is_reviewer = user_access('mfcs review', $user);
  $is_manager = user_access('mfcs manage', $user);
  $is_administer = user_access('mfcs administer', $user);

  $enabled_only = TRUE;
  if ($is_reviewer || $is_manager || $is_administer) {
    $enabled_only = NULL;
  }

  $unavailable = mfcs_get_unavailability($unavailability_id, $enabled_only);
  if (!is_object($unavailable)) {
    drupal_not_found();
    drupal_exit();
  }

  if ($year != $unavailable->year) {
    drupal_not_found();
    drupal_exit();
  }

  if ($room_id != $unavailable->room_id) {
    drupal_not_found();
    drupal_exit();
  }

  // do not provide a page for a non-existent room id.
  $room = mfcs_load_rooms(NULL, $unavailable->building_id, $unavailable->room_id, TRUE);
  if (!is_object($room)) {
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

  // report any detected unavailability to request conflicts.
  $conflicts = mfcs_unavailability_rooms_conflicts($unavailability_id);
  if (!empty($conflicts)) {
    $conflict_message = 'Warning: the following requests are scheduled on unavailable dates:<ul>';
    foreach ($conflicts as $conflict) {
      $conflict_message .= '<li><a href="' . $base_path . 'requests/view-0/' . $conflict['id'] . $url_arguments . '">' . check_plain($conflict['title']) . '</a></li>';
    }
    $conflict_message .= '</ul>';
    drupal_set_message($conflict_message, 'warning');
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $unavailability_types = mfcs_get_unavailability_type_list_options('combined');

  if ($unavailable->date_start == strtotime('midnight today', $unavailable->date_start) && $unavailable->date_stop == strtotime('midnight tomorrow', $unavailable->date_start)) {
    $date_label = 'Unavailable On:';
    $date_value = date('l, F jS, Y', $unavailable->date_start);
  }
  else {
    if (strtotime('midnight today', $unavailable->date_start) == strtotime('midnight today', $unavailable->date_stop)) {
      $date_label = 'Unavailable From:';
      $date_value = date(MFCS_DATE_FORMAT_TIME, $unavailable->date_start) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $unavailable->date_stop) . ' on ' . date('l, F jS, Y', $unavailable->date_start);
    }
    elseif ($unavailable->date_start != strtotime('midnight today', $unavailable->date_start) || $unavailable->date_stop != strtotime('midnight today', $unavailable->date_stop)) {
      $date_label = 'Unavailable From:';
      $date_value = date('h:ia l, F jS, Y', $unavailable->date_start) . ' to ' . date('h:ia l, F jS, Y', $unavailable->date_stop);
    }
    else {
      // For whatever reason, the culture treats the stop date inclusively when it has no time (but then treats it inclusively when there is a time!).
      // Make sure the stop date that is printed to the screen is an inclusive date (this therefore cannot be used for any calculations)
      $date_label = 'Unavailable From:';
      $date_value = '' . date('l, F jS, Y', $unavailable->date_start) . ' to ' . date('l, F jS, Y', strtotime('midnight yesterday', $unavailable->date_stop));
    }
  }

  $title = check_plain($unavailable->title);
  $description = str_replace("\n", '<br>', check_plain($unavailable->description));

  mfcs_add_canonical_headers('requests/unavailable-0/view/' . $year . '/' . $unavailable->room_id . '/' . $unavailability_id);
  drupal_set_title('' . $unavailable->year . " Room Unavailable: " . $unavailable->title);

  $can_troubleshoot = mfcs_page_generic_access('troubleshoot', NULL, $user);

  $markup = '<div id="mfcs-unavailable_0-view-page" class="mfcs-unavailable_0-view-page">';

  if ($can_troubleshoot) {
    $building_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Building Information">' . check_plain($room->building_name) . '</a>';
    $room_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Room Information">' . check_plain($room->room_name) . '</a>';
  }
  else {
    $building_value = check_plain($room->building_name);
    $room_value = check_plain($room->room_name);
  }

  $markup .= '<div class="room_information-wrapper">';
  $markup .= '<div class="room_information-item item-building_name">';
  $markup .= '<div class="room_information-item-label">Building Name:</div>';
  $markup .= '<div class="room_information-item-value">' . $building_value . '</div>';
  $markup .= '</div>';

  $markup .= '<div class="room_information-item item-room_name">';
  $markup .= '<div class="room_information-item-label">Room Name:</div>';
  $markup .= '<div class="room_information-item-value">' . $room_value . '</div>';
  $markup .= '</div>';

  $markup .= '<div class="room_information-item item-building_code">';
  $markup .= '<div class="room_information-item-label">Building Code:</div>';
  $markup .= '<div class="room_information-item-value">' . check_plain($room->building_code) . '</div>';
  $markup .= '</div>';

  $markup .= '<div class="room_information-item item-room_number">';
  $markup .= '<div class="room_information-item-label">Room Number:</div>';
  $markup .= '<div class="room_information-item-value">' . check_plain($room->room_number) . '</div>';
  $markup .= '</div>';

  $markup .= '<div class="room_information-item item-date">';
  $markup .= '<div class="room_information-item-label">' . $date_label . '</div>';
  $markup .= '<div class="room_information-item-value">' . $date_value . '</div>';
  $markup .= '</div>';

  $markup .= '<div class="room_information-item item-unavailability_type">';
  $markup .= '<div class="room_information-item-label">' . 'Type' . '</div>';
  $markup .= '<div class="room_information-item-value">' . (array_key_exists($unavailable->type, $unavailability_types) ? $unavailability_types[$unavailable->type] : 'Unknown') . '</div>';
  $markup .= '</div>';

  $markup .= '</div>';

  $markup .= '<div class="room_information-description">';
  $markup .= '<div class="room_information-description-label">Description:</div>';
  $markup .= '<div class="room_information-description-value">' . $description . '</div>';
  $markup .= '</div>';

  $can_edit = mfcs_unavailability_request_access('edit', $year, $unavailable->room_id, $unavailability_id, $user->uid);
  if ($can_edit) {
    $markup .= '<div class="room_information-status">';
    $markup .= '<div class="room_information-status-label">Status:</div>';
    $markup .= '<div class="room_information-status-value">' . ($unavailable->enabled ? 'Enabled' : 'Disabled') . '</div>';
    $markup .= '</div>';

    if (is_null($unavailable->class_id)) {
      $instance = mfcs_instance();
      $current_year = date('Y', $instance);
      if ($year >= $current_year) {
        $markup .= '<br class="no-print">';
        $markup .= '<div class="unavailability_page-operations no-print">';
        $markup .= '<a href="' . $base_path . 'requests/unavailable-0/edit/' . $year . '/' . $unavailable->room_id . '/' . $unavailability_id . $url_arguments . '" class="operation-edit inline-block margin-10 float-left">Edit this unavailability</a> ';
        $markup .= '<a href="' . $base_path . 'requests/unavailable-0/create/' . $year . '/' . $unavailable->room_id . '/' . $unavailability_id . $url_arguments . '" class="operation-copy inline-block margin-10 float-left">Copy this unavailability</a> ';
        $markup .= '<a href="' . $base_path . 'requests/unavailable-0/delete/' . $year . '/' . $unavailable->room_id . '/' . $unavailability_id . $url_arguments . '" class="operation-delete inline-block margin-10 float-right">Delete this unavailability</a>';
        $markup .= '</div>';
      }

      $markup .= '<br class="clear">';
    }
  }

  $markup .= '</div>';

  return $markup;
}

/**
 * Provides the facilities use request unavailability copy form.
 *
 * This copies all unavailabilities from one year to another (and not individual holidays).
 * To copy individual unavailabilities use mfcs_request_unavailability_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $year
 *   The unavailability year to operate on.
 * @param int $room_id
 *   The Room ID associated with the unavailability item.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see: mfcs_request_unavailability_0_edit_form()
 */
function mfcs_request_unavailability_0_copy_form($form, &$form_state, $year, $room_id) {
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

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($room_id)) {
    cf_error::invalid_integer('room_id');

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

  $user = cf_current_user();

  $is_manager = FALSE;
  if (user_access('mfcs manage', $user)) {
    $is_manager = TRUE;
  }

  $is_administer = FALSE;
  if (user_access('mfcs administer', $user)) {
    $is_administer = TRUE;
  }

  $can_troubleshoot = mfcs_page_generic_access('troubleshoot', NULL, $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  // do not provide a page for a non-existent room id.
  $room = mfcs_load_rooms(NULL, NULL, $room_id, TRUE);
  if (!is_object($room)) {
    drupal_not_found();
    drupal_exit();
  }

  mfcs_add_canonical_headers('requests/unavailable-0/copy/' . $year . '/' . $room_id);

  $page_title = "Copy Unavailable for " . $room->building_code . ' ' . $room->room_number . " to " . $year;
  $form['#id'] = 'mfcs-unavailable_0-copy-page';
  drupal_set_title($page_title);

  // a year offset is necessary to create the date field for a specific year.
  // the date field needs to be limited to a certain year and it works based off the offset to the current year.
  $instance = mfcs_instance();
  $year_offset = $year - ((int) date('Y', $instance));

  if ($year_offset > 0) {
    $year_offset = '+' . $year_offset;
  }
  elseif ($year_offset < 0) {
    $year_offset = '-' . abs($year_offset);
  }
  else {
    $year_offset = '0';
  }

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['year'] = array(
    '#id' => 'field-form-year',
    '#type' => 'value',
    '#value' => (int) $year,
  );

  $form['form']['room_id'] = array(
    '#id' => 'field-form-room_id',
    '#type' => 'value',
    '#value' => (int) $room_id,
  );

  $form['form']['building_id'] = array(
    '#id' => 'field-form-building_id',
    '#type' => 'value',
    '#value' => $room->building_id,
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  $form['unavailable'] = array(
    '#tree' => TRUE,
  );

  $form['unavailable']['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => '',
    '#weight' => -100,
  );

  if ($can_troubleshoot) {
    $building_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Building Information">' . check_plain($room->building_name) . '</a>';
    $room_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Room Information">' . check_plain($room->room_name) . '</a>';
  }
  else {
    $building_value = check_plain($room->building_name);
    $room_value = check_plain($room->room_name);
  }

  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-wrapper">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item item-building_name">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-label">Building Name:</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-value">' . $building_value . '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item item-room_name">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-label">Room Name:</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-value">' . $room_value . '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';

  $form['unavailable']['year'] = array(
    '#id' => 'field-unavailable-year',
    '#type' => 'numberfield',
    '#title' => "Year",
    '#required' => TRUE,
    '#description' => "Select a year to copy all unavailabilities of.",
  );

  $form['unavailable']['include_disabled'] = array(
    '#id' => 'field-unavailable-include_disabled',
    '#type' => 'checkbox',
    '#title' => "Include Disabled",
    '#required' => FALSE,
    '#description' => "When copying unavailabilities, include disabled unavailabilities.",
  );

  $form['unavailable']['as_disabled'] = array(
    '#id' => 'field-unavailable-as_disabled',
    '#type' => 'checkbox',
    '#title' => "As Disabled",
    '#required' => FALSE,
    '#default_value' => TRUE,
    '#description' => "When copying unavailabilities, set the copied unavailabilities to disabled.",
  );

  $form['unavailable']['submit'] = array(
    '#id' => 'mfcs-unavailable-copy-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['unavailable']['submit']['cancel'] = array(
    '#id' => 'mfcs-unavailable-copy-submit-cancel',
    '#name' => 'mfcs-unavailable-copy-submit-cancel',
    '#type' => 'submit',
    '#default_value' => t("Cancel"),
  );

  $form['unavailable']['submit']['submit'] = array(
    '#id' => 'mfcs-unavailable-copy-submit-submit',
    '#name' => 'mfcs-unavailable-copy-submit-submit',
    '#type' => 'submit',
    '#default_value' => t("Submit"),
  );

  return $form;
}

/**
 * Pre-Validate process for mfcs_request_unavailable_0_copy_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailable_0_copy_form()
 */
function mfcs_request_unavailability_0_copy_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-unavailable-copy-submit-cancel') {
    $form['unavailable']['year']['#required'] = FALSE;
  }
}

/**
 * Validate process for mfcs_request_unavailability_0_copy_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailability_0_copy_form()
 */
function mfcs_request_unavailability_0_copy_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-unavailable-copy-submit-cancel') {
    return;
  }
  elseif ($clicked_id != 'mfcs-unavailable-copy-submit-submit') {
    form_set_error('', "Unknown submit process detected.");
    return;
  }

  if (isset($form_state['values']['unavailable']['year']) && is_numeric($form_state['values']['unavailable']['year'])) {
    $year = (int) $form_state['values']['unavailable']['year'];
  }
  else {
    form_set_error('unavailable][year', "Please specify a valid year.");
    return;
  }

  $enabled = TRUE;
  if (isset($form_state['values']['unavailable']['include_disabled'])) {
    if ($form_state['values']['unavailable']['include_disabled'] == 1) {
      $enabled = NULL;
    }
  }

  // check to see if there are any years to copy.
  $unavailabilities = mfcs_get_unavailabilities($year, $form['form']['room_id']['#value'], NULL, NULL, $enabled);
  if ($unavailabilities === FALSE) {
    form_set_error('', "Internal Error. An unknown error has been detected while trying to process this request. Please contact the appropriate support staff.");

    $user = cf_current_user();
    watchdog(MFCS_WATCHDOG_ID, "Internal Error. Error while calling mfcs_get_unavailabilities() for the user %user_name [%user_id].", array('%user_name' => $user->name, '%user_id' => $user->uid), WATCHDOG_ERROR);
  }
  elseif (empty($unavailabilities)) {
    form_set_error('unavailable][year', "No unavailabilities were found for the year " . $year . ". There is nothing to copy.");
  }
}

/**
 * Submit process for mfcs_request_unavailability_0_copy_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailability_0_copy_form()
 */
function mfcs_request_unavailability_0_copy_form_submit($form, &$form_state) {
  global $mfcs_determined;
  global $base_path;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $instance = mfcs_instance();
  $year_this = date('Y', $instance);

  $room_id = $form['form']['room_id']['#value'];
  $year_current = (int) $form['form']['year']['#value'];
  $year_previous = (int) $form_state['values']['unavailable']['year'];

  if ($clicked_id == 'mfcs-unavailable-copy-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/' . $form['form']['year']['#value'] . '/' . $room_id);
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $enabled = TRUE;
  if ($form_state['values']['unavailable']['include_disabled'] == 1) {
    $enabled = NULL;
  }

  $as_disabled = TRUE;
  if ($form_state['values']['unavailable']['as_disabled'] == 0) {
    $as_disabled = FALSE;
  }

  $unavailabilities = mfcs_get_unavailabilities($year_previous, $room_id, NULL, NULL, $enabled);

  $transaction = db_transaction();
  $success = TRUE;
  try {
    if (!empty($unavailabilities)) {
      foreach ($unavailabilities as $unavailability) {
        // classrooms must never be copied.
        if (!is_null($unavailable->class_id)) {
          continue;
        }

        // copy the unavailability.
        $current_date_start = '' . $year_current . date('/m/d h:i:s a', $unavailability->date_start);
        $current_date_stop = '' . $year_current . date('/m/d h:i:s a', $unavailability->date_stop);

        $fields = array();
        $fields['type'] = $unavailability->type;
        $fields['year'] = $year_current;
        $fields['room_id'] = $room_id;
        $fields['building_id'] = $unavailability->building_id;
        $fields['date_start'] = strtotime($current_date_start);
        $fields['date_stop'] = strtotime($current_date_stop);
        $fields['title'] = $unavailability->title;
        $fields['description'] = $unavailability->description;
        $fields['created'] = $instance;
        $fields['updated'] = $instance;

        if ($as_disabled) {
          $fields['enabled'] = 0;
        }
        else {
          $fields['enabled'] = $unavailability->enabled;
        }

        $query = db_insert('mfcs_unavailability');
        $query->fields($fields);
        $id = $query->execute();

        if (is_numeric($id)) {
          $id = (int) $id;
        }

        // write to the log.
        $fields = array();
        $fields['unavailability_id'] = $id;
        $fields['user_id'] = $user->uid;
        $fields['date'] = $instance;
        $fields['change'] = 'copied from unavailability ' . $unavailability->id . '.';

        $query = db_insert('mfcs_log_unavailability');
        $query->fields($fields);
        $query->execute();

        unset($id);
        unset($current_date);
      }
    }
    else {
      $success = FALSE;
    }

    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }

  if (!$success) {
    form_set_error('form', 'An error occurred while trying to copy the unavailabilities. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to copy a unavailabilities.', array(), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/' . $form['form']['year']['#value'] . '/' . $room_id);
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request unavailability create, edit, and copy forms.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $year
 *   The unavailability year to operate on.
 * @param int $room_id
 *   The Room ID associated with the unavailability item.
 * @param int|null $unavailability_id
 *   The id of the unavailability to load.
 *   If NULL, then a new unavailability is being created.
 * @param bool $copy
 *   If TRUE, then the specified unavailability id is "cloned/copied" instead of edited.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_unavailability_0_edit_form($form, &$form_state, $year, $room_id, $unavailability_id = NULL, $copy = FALSE) {
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

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($room_id)) {
    cf_error::invalid_integer('room_id');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($unavailability_id) && !cf_is_integer($unavailability_id)) {
    cf_error::invalid_integer('unavailability_id');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_bool($copy)) {
    cf_error::invalid_integer('copy');

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

  $user = cf_current_user();

  $is_manager = FALSE;
  if (user_access('mfcs manage', $user)) {
    $is_manager = TRUE;
  }

  $is_administer = FALSE;
  if (user_access('mfcs administer', $user)) {
    $is_administer = TRUE;
  }

  $can_troubleshoot = mfcs_page_generic_access('troubleshoot', NULL, $user);

  $unavailable = NULL;
  if (!is_null($unavailability_id)) {
    $unavailable = mfcs_get_unavailability($unavailability_id, NULL);

    if (!is_object($unavailable)) {
      drupal_not_found();
      drupal_exit();
    }

    if ($unavailable->room_id != $room_id) {
      drupal_not_found();
      drupal_exit();
    }

    // class types are managed via banner.
    if (!is_null($unavailable->class_id)) {
      drupal_not_found();
      drupal_exit();
    }
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  // do not provide a page for a non-existent room id.
  $room = mfcs_load_rooms(NULL, NULL, $room_id, TRUE);
  if (!is_object($room)) {
    drupal_not_found();
    drupal_exit();
  }

  if (is_null($unavailability_id)) {
    mfcs_add_canonical_headers('requests/unavailable-0/create/' . $year . '/' . $room_id);

    $page_title = "Create " . $year . " Room Unavailable for " . $room->building_code . ' ' . $room->room_number;

    $form['#id'] = 'mfcs-unavailable_0-create-page';
  }
  elseif ($copy) {
    mfcs_add_canonical_headers('requests/unavailable-0/create/' . $year . '/' . $room_id . '/' . $unavailability_id);

    $page_title = "Copy " . $year . " Room Unavailable: " . $unavailable->title;

    $form['#id'] = 'mfcs-unavailable_0-copy-page';
  }
  else {
    mfcs_add_canonical_headers('requests/unavailable-0/edit/' . $year . '/' . $room_id . '/' . $unavailability_id);

    $page_title = "Edit " . $year . " Room Unavailable: " . $unavailable->title;

    $form['#id'] = 'mfcs-unavailable_0-edit-page';
  }

  drupal_set_title($page_title);

  $unavailability_types = mfcs_get_unavailability_type_list_options('select');

  // class rooms are never directly assigned/copied, but are instead handled via banner.
  unset($unavailability_types[MFCS_UNAVAILABILITY_TYPE_CLASS]);

  // a year offset is necessary to create the date field for a specific year.
  // the date field needs to be limited to a certain year and it works based off the offset to the current year.
  $instance = mfcs_instance();
  $year_offset = $year - ((int) date('Y', $instance));

  if ($year_offset > 0) {
    $year_offset = '+' . $year_offset;
  }
  elseif ($year_offset < 0) {
    $year_offset = '-' . abs($year_offset);
  }
  else {
    $year_offset = '0';
  }

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['unavailable'] = array(
    '#id' => 'field-form-unavailable',
    '#type' => 'value',
    '#value' => $unavailable,
  );

  $form['form']['year'] = array(
    '#id' => 'field-form-year',
    '#type' => 'value',
    '#value' => $year,
  );

  $form['form']['room_id'] = array(
    '#id' => 'field-form-room_id',
    '#type' => 'value',
    '#value' => $room_id,
  );

  $form['form']['building_id'] = array(
    '#id' => 'field-form-building_id',
    '#type' => 'value',
    '#value' => $room->building_id,
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  $form['form']['copy'] = array(
    '#id' => 'field-form-copy',
    '#type' => 'value',
    '#value' => $copy,
  );

  $form['unavailable'] = array(
    '#tree' => TRUE,
  );

  $form['unavailable']['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => '',
    '#weight' => -100,
  );

  if ($can_troubleshoot) {
    $building_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Building Information">' . check_plain($room->building_name) . '</a>';
    $room_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Room Information">' . check_plain($room->room_name) . '</a>';
  }
  else {
    $building_value = check_plain($room->building_name);
    $room_value = check_plain($room->room_name);
  }

  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-wrapper">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item item-building_name">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-label">Building Name:</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-value">' . $building_value . '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item item-room_name">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-label">Room Name:</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-value">' . $room_value . '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';

  $form['unavailable']['title'] = array(
    '#id' => 'field-unavailable-title',
    '#type' => 'textfield',
    '#title' => "Title",
    '#required' => TRUE,
    '#default_value' => isset($unavailable->title) ? $unavailable->title : '',
  );

  $form['unavailable']['type'] = array(
    '#id' => 'field-unavailable-type',
    '#type' => 'select',
    '#title' => "Type",
    '#options' => $unavailability_types,
    '#required' => TRUE,
    '#default_value' => (isset($unavailable->type) ? $unavailable->type : NULL),
  );

  $form['unavailable']['description'] = array(
    '#id' => 'field-unavailable-description',
    '#type' => 'textarea',
    '#title' => "Description",
    '#default_value' => isset($unavailable->description) ? $unavailable->description : '',
  );


  $date_start_day = '';
  $date_start_time = '';
  $date_stop_day = '';
  $date_stop_time = '';
  if (isset($unavailable->date_start) && isset($unavailable->date_stop)) {
    $date_start_day = date('Y-m-d', $unavailable->date_start);

    if ($unavailable->date_start != strtotime('midnight today', $unavailable->date_start)) {
      $date_start_time = date(MFCS_DATE_FORMAT_TIME, $unavailable->date_start);
    }

    if ($unavailable->date_start != $unavailable->date_stop) {
      if ($unavailable->date_stop < strtotime('midnight tomorrow', $unavailable->date_start)) {
        $date_start_time = date(MFCS_DATE_FORMAT_TIME, $unavailable->date_start);
      }

      $date_stop_day = date('Y-m-d', $unavailable->date_stop);
      if ($unavailable->date_stop != strtotime('midnight today', $unavailable->date_stop)) {
        $date_stop_time = date(MFCS_DATE_FORMAT_TIME, $unavailable->date_stop);
      }
    }
  }

  $form['unavailable']['date_start'] = array(
    '#id' => 'section-unavailable-date_start',
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => "Start Date and Time",
    '#description' => "The start day and time is inclusive.",
  );

  $form['unavailable']['date_start']['day'] = array(
    '#id' => 'field-unavailable-date_start-day',
    '#type' => 'date_popup',
    '#title' => "Start Day",
    '#date_format' => "M d",
    '#date_label_position' => 'invisible',
    '#date_year_range' => '' . $year_offset . ':' . $year_offset,
    '#datepicker_options' => array(
      'changeYear' => FALSE,
    ),
    '#expected_format' => array('date' => "M d"),
    '#required' => TRUE,
    '#default_value' => $date_start_day,
    '#wrapper_attributes' => array(
      'class' => array(
        'container-inline-date',
        'form-item-unavailable-date_start-day',
      ),
    ),
  );

  $form['unavailable']['date_start']['time'] = array(
    '#id' => 'field-unavailable-date_start-time',
    '#type' => 'textfield',
    '#title' => "Start Time",
    '#size' => 8,
    '#maxlength' => 8,
    '#description' => "E.g, 12:31pm",
    '#expected_format' => array('date' => MFCS_DATE_FORMAT_TIME),
    '#required' => FALSE,
    '#default_value' => $date_start_time,
  );

  $form['unavailable']['date_stop'] = array(
    '#id' => 'section-unavailable-date_stop',
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => "Stop Date and Time",
    '#description' => "The stop day and time is exclusive.",
  );

  $form['unavailable']['date_stop']['day'] = array(
    '#id' => 'field-unavailable-date_stop-day',
    '#type' => 'date_popup',
    '#title' => "Stop Date",
    '#date_format' => "M d",
    '#date_label_position' => 'invisible',
    '#date_year_range' => '' . $year_offset . ':' . $year_offset,
    '#datepicker_options' => array(
      'changeYear' => FALSE,
    ),
    '#expected_format' => array('date' => "M d"),
    '#required' => FALSE,
    '#default_value' => $date_stop_day,
    '#wrapper_attributes' => array(
      'class' => array(
        'container-inline-date',
        'form-item-unavailable-date_stop-day',
      ),
    ),
  );

  $form['unavailable']['date_stop']['time'] = array(
    '#id' => 'field-unavailable-date_stop-time',
    '#type' => 'textfield',
    '#title' => "Stop Time",
    '#size' => 8,
    '#maxlength' => 8,
    '#description' => "E.g, 12:31pm",
    '#expected_format' => array('date' => MFCS_DATE_FORMAT_TIME),
    '#required' => FALSE,
    '#default_value' => $date_stop_time,
  );

  $form['unavailable']['enabled'] = array(
    '#id' => 'field-unavailable-enabled',
    '#type' => 'select',
    '#title' => "Enabled",
    '#options' => array(0 => 'No', 1 => 'Yes'),
    '#default_value' => isset($unavailable->enabled) ? $unavailable->enabled : 1,
  );

  $form['unavailable']['help'] = array(
    '#id' => 'section-help',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => "Help",
  );

  $form['unavailable']['help']['inclusiveness'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );

  $form['unavailable']['help']['inclusiveness']['#markup'] .= '<div class="inclusiveness">';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= '<h3 class="inclusiveness-title">Inclusiveness and Exclusiveness</h3>';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= '<div class="inclusiveness-text">';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= 'The start day (and time) is <em>inclusive</em>, but the stop day (and time) is <em>exclusive</em>.<br>';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= 'What this means is that if you say that start day is July 29th and the stop day is July 30th, then the unavailability ends on the second before July 30th begins (aka: ends on 11:59:59pm).<br>';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= 'If you want to ensure that both July 29th and July 30th are included, then the stop day must be July 31st.<br>';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= '</div>';
  $form['unavailable']['help']['inclusiveness']['#markup'] .= '</div>';

  $form['unavailable']['submit'] = array(
    '#id' => 'mfcs-unavailable-edit-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['unavailable']['submit']['cancel'] = array(
    '#id' => 'mfcs-unavailable-edit-submit-cancel',
    '#name' => 'mfcs-unavailable-edit-submit-cancel',
    '#type' => 'submit',
    '#default_value' => t("Cancel"),
  );

  $form['unavailable']['submit']['submit'] = array(
    '#id' => 'mfcs-unavailable-edit-submit-submit',
    '#name' => 'mfcs-unavailable-edit-submit-submit',
    '#type' => 'submit',
    '#default_value' => t("Submit"),
  );

  return $form;
}

/**
 * Pre-Validate process for mfcs_request_unavailability_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailability_0_edit_form()
 */
function mfcs_request_unavailability_0_edit_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-unavailable-edit-submit-cancel') {
    $form['unavailable']['title']['#required'] = FALSE;
    $form['unavailable']['type']['#required'] = FALSE;
    $form['unavailable']['date_start']['day']['#required'] = FALSE;
  }
}

/**
 * Validate process for mfcs_request_unavailability_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailability_0_edit_form()
 */
function mfcs_request_unavailability_0_edit_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-unavailable-edit-submit-cancel') {
    return;
  }

  mfcs_include(MFCS_INCLUDE_VALIDATION);

  // when the field is 'required', but is empty, for some reason an array is provided instead of a string.
  if (is_array($form_state['values']['unavailable']['date_start']['day'])) {
    $form_state['values']['unavailable']['date_start']['day'] = NULL;
  }

  $date_start = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_start']['day']);
  $date_start = strtotime('midnight today', $date_start);

  if ($date_start === FALSE || date('m-d', $date_start) != $form_state['values']['unavailable']['date_start']['day']) {
    form_set_error('unavailable][date_start][day', "The specified start date day is not valid.");
  }
  elseif (isset($form_state['values']['unavailable']['date_start']['time']) && strlen($form_state['values']['unavailable']['date_start']['time']) > 0) {
    $date_start = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_start']['day'] . ' ' . $form_state['values']['unavailable']['date_start']['time']);
    if ($date_start === FALSE || !mfcs_validate_time_12hour($form_state['values']['unavailable']['date_start']['time'])) {
      form_set_error('unavailable][date_start][time', "The specified start date time is not valid.");
    }
  }

  if (isset($form_state['values']['unavailable']['date_stop']['day'])) {
    $date_stop = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_stop']['day']);
    $date_stop = strtotime('midnight today', $date_stop);

    if ($date_stop === FALSE || date('m-d', $date_stop) != $form_state['values']['unavailable']['date_stop']['day']) {
      form_set_error('unavailable][date_stop][day', "The specified stop date day is not valid.");
    }
    elseif (isset($form_state['values']['unavailable']['date_stop']['time']) && strlen($form_state['values']['unavailable']['date_stop']['time']) > 0) {
      $date_stop = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_stop']['day'] . ' ' . $form_state['values']['unavailable']['date_stop']['time']);
      if ($date_stop === FALSE || !mfcs_validate_time_12hour($form_state['values']['unavailable']['date_stop']['time'])) {
        form_set_error('unavailable][date_stop][time', "The specified stop date time is not valid.");
      }
    }

    if ($date_stop == $date_start) {
        form_set_error('unavailable][date_stop][day', "The specified stop date day (and time) cannot be the same as the start date (and time).");
    }

    if ($date_stop < $date_start) {
      form_set_error('unavailable][date_stop][day', "The specified stop date day (and time) cannot be before the start date (and time).");
    }
  }
  else {
    if (isset($form_state['values']['unavailable']['date_start']['time']) && strlen($form_state['values']['unavailable']['date_start']['time']) > 0) {
      form_set_error('unavailable][date_start][time', "The start date time may only be specified if a stop date is specified.");
    }

    if (isset($form_state['values']['unavailable']['date_stop']['time']) && strlen($form_state['values']['unavailable']['date_stop']['time']) > 0) {
      form_set_error('unavailable][date_stop][day', "The stop date day must be specified when the stop time is specified.");
    }
  }

  $length = strlen($form_state['values']['unavailable']['title']);
  if ($length > 255) {
    form_set_error('unavailable][title', "The title is " . $length . " characters long and cannot be longer than 255 characters.");
  }

  $length = strlen($form_state['values']['unavailable']['description']);
  if ($length > 255) {
    form_set_error('unavailable][description', "The description is " . $length . " characters long and cannot be longer than 255 characters.");
  }
}

/**
 * Submit process for mfcs_request_unavailability_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailability_0_edit_form()
 */
function mfcs_request_unavailability_0_edit_form_submit($form, &$form_state) {
  global $mfcs_determined;
  global $base_path;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $unavailable = NULL;
  if (isset($form['form']['unavailable']['#value']->id)) {
    $unavailable = $form['form']['unavailable']['#value'];
  }

  $instance = mfcs_instance();
  $year_this = date('Y', $instance);

  if ($clicked_id == 'mfcs-unavailable-edit-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      if (is_object($unavailable)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/view/' . $form['form']['year']['#value'] . '/' . $form['form']['room_id']['#value'] . '/' . $unavailable->id);
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/' . $form['form']['year']['#value'] . '/' . $form['form']['room_id']['#value']);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $transaction = db_transaction();

  $success = TRUE;
  try {
    $fields = array();
    $fields['title'] = $form_state['values']['unavailable']['title'];
    $fields['type'] = $form_state['values']['unavailable']['type'];
    $fields['description'] = str_replace("\r\n", "\n", $form_state['values']['unavailable']['description']);
    $fields['description'] = str_replace("\r", "\n", $fields['description']);
    $fields['room_id'] = (int) $form['form']['room_id']['#value'];
    $fields['building_id'] = (int) $form['form']['building_id']['#value'];
    $fields['enabled'] = (int) $form_state['values']['unavailable']['enabled'];
    $fields['updated'] = mfcs_instance();
    $fields['year'] = (int) $form['form']['year']['#value'];

    if (isset($form_state['values']['unavailable']['date_start']['time']) && strlen($form_state['values']['unavailable']['date_start']['time']) > 0) {
      $fields['date_start'] = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_start']['day'] . ' ' . $form_state['values']['unavailable']['date_start']['time']);
    }
    else {
      $fields['date_start'] = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_start']['day']);
      $fields['date_start'] = strtotime('midnight today', $fields['date_start']);
    }

    if (isset($form_state['values']['unavailable']['date_stop']['day']) && strlen($form_state['values']['unavailable']['date_stop']['day']) > 0) {
      if (isset($form_state['values']['unavailable']['date_stop']['time']) && strlen($form_state['values']['unavailable']['date_stop']['time']) > 0) {
        $fields['date_stop'] = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_stop']['day'] . ' ' . $form_state['values']['unavailable']['date_stop']['time']);
      }
      else {
        $fields['date_stop'] = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['unavailable']['date_stop']['day']);
        $fields['date_stop'] = strtotime('midnight today', $fields['date_stop']);
      }
    }
    else {
      // when date stop is not specified, then the event is an all day event.
      // setting the stop date to 'tomorrow' logically produces an all day event.
      $fields['date_stop'] = strtotime('midnight tomorrow', $fields['date_start']);
    }

    $id = NULL;
    if (is_object($unavailable)) {
      if ($form['form']['copy']['#value']) {
        $fields['created'] = $fields['updated'];

        $query = db_insert('mfcs_unavailability');
        $query->fields($fields);
        $id = $query->execute();

        if (is_numeric($id)) {
          $id = (int) $id;
        }

        $change = 'copied from availability ' . $unavailable->id . '.';
      }
      else {
        $id = (int) $unavailable->id;

        // check to see if there are any changes made.
        $changes = array();
        foreach ($fields as $key => $value) {
          // do not check updated key because it must be different.
          if ($key == 'updated') {
            continue;
          }

          if ($value != $unavailable->{$key}) {
            $changes[$key] = array(
              'from' => $unavailable->{$key},
              'to' => $value,
            );
          }
        }

        $change = '';
        if (!empty($changes)) {
          $query = db_update('mfcs_unavailability');
          $query->condition('id', $id);
          $query->fields($fields);
          $query->execute();

          $fields = array();
          $fields['unavailability_id'] = $id;
          $fields['date'] = mfcs_instance();
          $fields['user_id'] = $user->uid;
          $fields['change'] = '';

          foreach ($changes as $key => $value) {
            $change .= $key . ' has been changed from \'' . $value['from'] . '\' to \'' . $value['to'] . '\'. ';
          }
        }

        trim($change);
      }
    }
    else {
      $fields['created'] = $fields['updated'];

      $query = db_insert('mfcs_unavailability');
      $query->fields($fields);
      $id = $query->execute();

      if (is_numeric($id)) {
        $id = (int) $id;
      }

      $change = 'created.';
    }

    if (!is_null($id)) {
      $fields = array();
      $fields['unavailability_id'] = $id;
      $fields['date'] = mfcs_instance();
      $fields['user_id'] = $user->uid;
      $fields['change'] = $change;

      $query = db_insert('mfcs_log_unavailability');
      $query->fields($fields);
      $query->execute();
    }

    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }

  if (!$success) {
    form_set_error('form', "An error occurred while trying to edit the unavailability. Please contact the support staff.");

    if (is_object($unavailable)) {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to edit the unavailability: @unavailability_id.", array('@unavailability_id' => $unavailable->id), WATCHDOG_ERROR);
    }
    else {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to create an unavailability.", array(), WATCHDOG_ERROR);
    }

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (is_int($id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/view/' . $form['form']['year']['#value'] . '/' . $form['form']['room_id']['#value'] . '/' . $id);
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/' . $form['form']['year']['#value'] . '/' . $form['form']['room_id']['#value']);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request unavailability main page(s).
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $year
 *   The unavailability year to operate on.
 * @param int $room_id
 *   The Room ID associated with the unavailability item.
 * @param int $unavailability_id
 *   The id of the unavailability to load.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_unavailability_0_delete_form($form, &$form_state, $year, $room_id, $unavailability_id) {
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

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($room_id)) {
    cf_error::invalid_integer('room_id');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($unavailability_id)) {
    cf_error::invalid_integer('unavailability_id');

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

  $user = cf_current_user();

  $unavailable = mfcs_get_unavailability($unavailability_id, NULL);

  if (!is_object($unavailable)) {
    drupal_not_found();
    drupal_exit();
  }

  if ($year != $unavailable->year) {
    drupal_not_found();
    drupal_exit();
  }

  if ($room_id != $unavailable->room_id) {
    drupal_not_found();
    drupal_exit();
  }

  // class types are managed via banner.
  if (!is_null($unavailable->class_id)) {
    drupal_not_found();
    drupal_exit();
  }

  mfcs_add_canonical_headers('requests/unavailable-0/delete/' . $year . '/' . $room_id . '/' . $unavailability_id);

  // do not provide a page for a non-existent room id.
  $room = mfcs_load_rooms(NULL, $unavailable->building_id, $room_id, TRUE);
  if (!is_object($room)) {
    drupal_not_found();
    drupal_exit();
  }

  $can_troubleshoot = mfcs_page_generic_access('troubleshoot', NULL, $user);

  drupal_set_title("Delete " . $unavailable->year . " Unavailability: " . $unavailable->title);

  $form['#id'] = 'mfcs-unavailable_0-delete-page';
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['unavailable'] = array(
    '#id' => 'field-form-unavailable',
    '#type' => 'value',
    '#value' => $unavailable,
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  if ($can_troubleshoot) {
    $building_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Building Information">' . check_plain($room->building_name) . '</a>';
    $room_value = '<a href="' . $base_path . 'requests/troubleshoot-0/rooms/' . $room_id . $url_arguments . '" class="operation-link operation-link-view" title="View Technical Room Information">' . check_plain($room->room_name) . '</a>';
  }
  else {
    $building_value = check_plain($room->building_name);
    $room_value = check_plain($room->room_name);
  }

  $form['unavailable'] = array(
    '#tree' => TRUE,
  );

  $form['unavailable']['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );

  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-wrapper">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item item-building_name">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-label">Building Name:</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-value">' . $building_value . '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item item-room_name">';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-label">Room Name:</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '<div class="room_information-item-value">' . $room_value . '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';
  $form['unavailable']['pre_markup']['#markup'] .= '</div>';

  $form['unavailable']['confirmation'] = array(
    '#markup' => '',
  );

  $form['unavailable']['confirmation']['#markup'] .= '<h3>Are you sure you want to delete this unavailability?</h3> ';
  $form['unavailable']['confirmation']['#markup'] .= '<div>';
  $form['unavailable']['confirmation']['#markup'] .= '<strong>ID</strong>: ' . $unavailable->id . '<br>';
  $form['unavailable']['confirmation']['#markup'] .= '<strong>Title</strong>: ' . $unavailable->title . '<br>';
  $form['unavailable']['confirmation']['#markup'] .= '<strong>Year</strong>: ' . $unavailable->year . '<br>';

  if ($unavailable->date_start == $unavailable->date_stop) {
    $form['unavailable']['confirmation']['#markup'] .= '<strong>Start Date</strong>: ' . date(MFCS_DATE_FORMAT_DATE, $unavailable->date_start) . ' (All Day)<br>';
  }
  else {
    if ($unavailable->date_start == strtotime('midnight today', $unavailable->date_start)) {
      $form['unavailable']['confirmation']['#markup'] .= '<strong>Start Date</strong>: ' . date(MFCS_DATE_FORMAT_DATE, $unavailable->date_start) . '<br>';
    }
    else {
      $form['unavailable']['confirmation']['#markup'] .= '<strong>Start Date</strong>: ' . date(MFCS_DATE_FORMAT_DATE_TIME, $unavailable->date_start) . '<br>';
    }

    if ($unavailable->date_stop == strtotime('midnight today', $unavailable->date_stop)) {
      $form['unavailable']['confirmation']['#markup'] .= '<strong>Stop Date</strong>: ' . date(MFCS_DATE_FORMAT_DATE, $unavailable->date_stop) . '<br>';
    }
    else {
      $form['unavailable']['confirmation']['#markup'] .= '<strong>Stop Date</strong>: ' . date(MFCS_DATE_FORMAT_DATE_TIME, $unavailable->date_stop) . '<br>';
    }
  }
  $form['unavailable']['confirmation']['#markup'] .= '<strong>Status</strong>: ' . ($unavailable->enabled == 1 ? 'Enabled' : 'Disabled') . '<br>';

  $form['unavailable']['confirmation']['#markup'] .= '</div>';
  $form['unavailable']['confirmation']['#markup'] .= '<br>';

  $form['display']['submit'] = array(
    '#id' => 'mfcs-unavailable-delete-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'mfcs-unavailable-delete-submit-no',
    '#name' => 'mfcs-unavailable-delete-submit-no',
    '#type' => 'submit',
    '#default_value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'mfcs-unavailable-delete-submit-yes',
    '#name' => 'mfcs-unavailable-delete-submit-yes',
    '#type' => 'submit',
    '#default_value' => t("Yes"),
  );

  return $form;
}

/**
 * Submit process for mfcs_request_unavailability_0_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_unavailability_0_delete_form()
 */
function mfcs_request_unavailability_0_delete_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $id = (int) $form['form']['unavailable']['#value']->id;
  $year = (int) $form['form']['unavailable']['#value']->year;
  $room_id = (int) $form['form']['unavailable']['#value']->room_id;

  if ($clicked_id == 'mfcs-unavailable-delete-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/view/' . $year . '/' . $room_id . '/' . $id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $fields = array();
  $fields['deleted'] = 1;
  $fields['updated'] = mfcs_instance();

  $transaction = db_transaction();

  $deleted = TRUE;
  try {
    $query = db_update('mfcs_unavailability');
    $query->condition('id', $id);
    $query->fields($fields);
    $query->execute();

    $fields = array();
    $fields['unavailability_id'] = $id;
    $fields['date'] = mfcs_instance();
    $fields['user_id'] = $user->uid;
    $fields['change'] = 'deleted.';

    $query = db_insert('mfcs_log_unavailability');
    $query->fields($fields);
    $query->execute();

    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $deleted = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $deleted = FALSE;
  }

  if (!$deleted) {
    form_set_error('form', "An error occurred while trying to delete the unavailability. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to delete the unavailability: @id.", array('@id' => $id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/unavailable-0/' . $year . '/' . $room_id);
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Returns a list of rooms that are unavailable.
 *
 * @param int|NULL $room_id
 *   The ID of the room to render a page for.
 *   When NULL, all rooms are loaded (based on user id permissions).
 * @param int $year
 *   The year to load the room unavailability listing for.
 * @param integer $user_id
 *   (optional) The user to load the room listing by.
 *   If not specified, then all possibly values are loaded.
 * @param bool|null $enabled
 *   (optional) When TRUE, only load unavailabilities when the enabled column is 1.
 *   When FALSE, only load unavailabilities when the enabled column is 0.
 *   When NULL, ignore this column.
 *
 * @return array|bool
 *   An array containing the rooms that can be managed.
 *   FALSE is returned on error.
 */
function mfcs_unavailability_rooms_room($room_id, $year, $user_id = NULL, $enabled = TRUE) {
  if (!cf_is_integer($room_id) && !is_null($room_id)) {
    cf_error::invalid_integer('room_id');
    return FALSE;
  }

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');
    return FALSE;
  }

  if (!is_null($user_id) && !cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');
    return FALSE;
  }

  if (!is_null($user_id)) {
    $proxy_as = mfcs_get_proxy_venue_coordinator($user_id);
    if (!is_array($proxy_as)) {
      $proxy_as = array();
    }

    $proxy_as[$user_id] = $user_id;
  }

  if (!is_null($enabled) && !is_bool($enabled)) {
    cf_error::invalid_bool('enabled');
    return FALSE;
  }

  $rows = array();
  try {
    $query = db_select('mfcs_unavailability', 'mu');
    $query->innerJoin('mfcs_banner_rooms', 'mbr', 'mu.room_id = mbr.id');
    $query->innerJoin('mfcs_banner_buildings', 'mbb', 'mu.building_id = mbb.id');

    if (!is_null($user_id)) {
      $or = db_or();
      $or->condition('mbr.room_coordinator_id', $proxy_as, 'IN');
      $or->condition('mbr.venue_coordinator_id', $proxy_as, 'IN');
      $query->condition($or);
    }

    if ($enabled === TRUE) {
      $query->condition('mu.enabled', 1);
    }
    elseif ($enabled === FALSE) {
      $query->condition('mu.enabled', 0);
    }

    if (!is_null($room_id)) {
      $query->condition('mu.room_id', $room_id);
    }
    $query->condition('mu.year', $year);

    $query->condition('mu.deleted', 0);
    $query->condition('mbr.disabled', 0);
    $query->condition('mbb.disabled', 0);

    $query->addField('mu', 'id', 'id');
    $query->addField('mu', 'type', 'type');
    $query->addField('mu', 'year', 'year');
    $query->addField('mu', 'room_id', 'room_id');
    $query->addField('mu', 'building_id', 'building_id');
    $query->addField('mu', 'class_id', 'class_id');
    $query->addField('mu', 'date_start', 'date_start');
    $query->addField('mu', 'date_stop', 'date_stop');
    $query->addField('mu', 'title', 'title');
    $query->addField('mu', 'description', 'description');
    $query->addField('mu', 'created', 'created');
    $query->addField('mu', 'updated', 'updated');
    $query->addField('mu', 'enabled', 'enabled');

    $query->addField('mbr', 'machine_name', 'room_number');
    $query->addField('mbr', 'human_name', 'room_name');
    $query->addField('mbb', 'machine_name', 'building_code');
    $query->addField('mbb', 'human_name', 'building_name');
    $query->addField('mbr', 'room_coordinator_id', 'room_coordinator_id');
    $query->addField('mbr', 'venue_coordinator_id', 'venue_coordinator_id');

    $query->orderBy('mu', 'id', 'asc');

    $rows = $query->execute()->fetchAllAssoc('id');
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    return FALSE;
  }

  return $rows;
}

/**
 * Returns a list of rooms that the specified user can manage the unavailability of.
 *
 * @param integer $user_id
 *   (optional) The user to load the room listing by.
 *   If not specified, then all possibly values are loaded.
 *
 * @return array|bool
 *   An array containing the rooms that can be managed.
 *   FALSE is returned on error.
 */
function mfcs_unavailability_rooms($user_id = NULL) {
  if (!is_null($user_id) && !cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');

    return FALSE;
  }

  if (!is_null($user_id)) {
    $proxy_as = mfcs_get_proxy_venue_coordinator($user_id);
    if (!is_array($proxy_as)) {
      $proxy_as = array();
    }

    $proxy_as[$user_id] = $user_id;
  }

  $rows = array();
  try {
    $query = db_select('mfcs_banner_rooms', 'mbr');
    $query->innerJoin('mfcs_banner_buildings', 'mbb', 'mbr.building = mbb.id');

    if (!is_null($user_id)) {
      $or = db_or();
      $or->condition('mbr.room_coordinator_id', $proxy_as, 'IN');
      $or->condition('mbr.venue_coordinator_id', $proxy_as, 'IN');
      $query->condition($or);
    }

    $query->condition('mbr.disabled', 0);

    $query->addField('mbr', 'id', 'id');
    $query->addField('mbr', 'machine_name', 'room_number');
    $query->addField('mbr', 'human_name', 'room_name');
    $query->addField('mbb', 'id', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_code');
    $query->addField('mbb', 'human_name', 'building_name');
    $query->addField('mbr', 'room_coordinator_id', 'room_coordinator_id');
    $query->addField('mbr', 'venue_coordinator_id', 'venue_coordinator_id');

    $query->orderBy('mbb', 'id', 'asc');
    $query->orderBy('mbr', 'id', 'asc');

    $rows = $query->execute()->fetchAllAssoc('id');
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    return FALSE;
  }

  return $rows;
}

/**
 * Check for requests where the request is assigned on the same day and time as an unavailability.
 *
 * @param int|array $unavailability_id
 *   The id or an array of ids to to limit conflict checks by.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_unavailability_rooms_conflicts($unavailability_id) {
  if (is_array($unavailability_id)) {
    foreach ($unavailability_id as $id) {
      if (!cf_is_integer($id)) {
        cf_error::invalid_array('unavailability_id');
        return FALSE;
      }
    }
    unset($id);
  }
  elseif (!cf_is_integer($unavailability_id)) {
    cf_error::invalid_integer('unavailability_id');
    return FALSE;
  }

  $requests = array();

  try {
    $arguments = array();

    $not_these_status = array(
      MFCS_REQUEST_STATUS_DELETED,
      MFCS_REQUEST_STATUS_CLOSED_DENIED,
      MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
      MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
      MFCS_REQUEST_STATUS_CANCELLED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
    );

    $arguments[':status_1'] = MFCS_REQUEST_STATUS_DELETED;
    $arguments[':status_2'] = MFCS_REQUEST_STATUS_CLOSED_DENIED;
    $arguments[':status_3'] = MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE;
    $arguments[':status_4'] = MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK;
    $arguments[':status_5'] = MFCS_REQUEST_STATUS_CANCELLED;
    $arguments[':status_6'] = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED;
    $arguments[':unavailability_id'] = $unavailability_id;

    $query_string = 'with unavailabilities as ( ';
    $query_string .= 'select mu.id as unavailability_id, mu.title as unavailability_title, mu.type as unavailability_type, mu.year as unavailability_year, mu.date_start as unavailability_date_start, mu.date_stop as unavailability_date_stop, mu.title, mer.id, mer.title, mer.location as location_id, mer.building as building_id, mer.room as room_id, mer.status ';
    $query_string .= 'from mfcs_unavailability mu ';
    $query_string .= 'inner join mfcs_requests mer on (mu.room_id = mer.room and mu.building_id = mer.building) ';
    $query_string .= 'where mer.status not in (:status_1, :status_2, :status_3, :status_4, :status_5, :status_6) ';
    if (is_array($unavailability_id)) {
      $query_string .= 'and mu.id in (:unavailability_id) ';
    }
    else {
      $query_string .= 'and mu.id = :unavailability_id ';
    }
    $query_string .= ' and mu.deleted = 0 and mu.enabled = 1';
    $query_string .= ') ';

    $query_string .= 'select u.*, mfdd.delta, mfdd.value as date, mfdts.value as time_start, mfdtsp.value as time_stop from unavailabilities u ';
    $query_string .= 'inner join mfcs_current mc on (u.id = mc.request_id) ';
    $query_string .= 'inner join mfcs_field_dates_date mfdd on (mc.request_id = mfdd.request_id and mc.dates_date = mfdd.revision) ';
    $query_string .= 'left join mfcs_field_dates_time_start mfdts on (mc.request_id = mfdts.request_id and mc.dates_time_start = mfdts.revision and mfdd.delta = mfdts.delta) ';
    $query_string .= 'left join mfcs_field_dates_time_stop mfdtsp on (mc.request_id = mfdtsp.request_id and mc.dates_time_stop = mfdtsp.revision and mfdts.delta = mfdtsp.delta) ';

    $query_string .= 'where (mfdts.value is null and mfdtsp.value is null and ((u.unavailability_date_start < mfdd.value and u.unavailability_date_stop > mfdts.value) or (u.unavailability_date_start >= mfdd.value and u.unavailability_date_start < (mfdd.value + 86400)))) or ';
    $query_string .= '( not (mfdts.value is null and mfdtsp.value is null) and ( ';
    $query_string .= '(u.unavailability_date_start < mfdts.value and u.unavailability_date_stop > mfdts.value) or (u.unavailability_date_start >= mfdts.value and u.unavailability_date_start < mfdtsp.value) ';
    $query_string .= ') ) ';

    $results = db_query($query_string, $arguments)->fetchAll();
    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        if (!array_key_exists($result->id, $requests)) {
          $requests[(int) $result->id] = array(
            'id' => (int) $result->id,
            'title' => $result->title,
            'location' => (int) $result->location_id,
            'building' => (int) $result->building_id,
            'room' => (int) $result->room_id,
            'dates' => array(),
          );
        }

        $requests[$result->id]['dates'][(int) $result->delta] = array(
          'date' => $result->date,
          'time_start' => $result->time_start,
          'time_stop' => $result->time_stop,
        );
      }
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

  return $requests;
}
