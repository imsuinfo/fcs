<?php

/**
 * @file
 * Defines McNeese event workflow event review page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

 /**
 * Provides the event workflow event review page.
 *
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_review_0_page($page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  global $base_path;

  $rows_per_page = 50;
  $start_range = $page_number * $rows_per_page;

  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];

  $user = cf_current_user();

  $can_create = mcneese_event_workflow_management_page_event_access('create');
  $can_edit = mcneese_event_workflow_management_page_event_access('edit');
  $can_view = mcneese_event_workflow_management_page_event_access('view');
  $can_list = mcneese_event_workflow_management_page_event_access('list');
  $can_search = mcneese_event_workflow_management_page_event_access('search');
  $can_delete = mcneese_event_workflow_management_page_event_access('delete');
  $can_log = mcneese_event_workflow_management_page_event_access('log');
  $can_review = mcneese_event_workflow_management_page_event_access('review');

  $administer = user_access('mcneese event workflow administer');
  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');


  // load sorting information
  $sorting = array();
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
    $sorting['order'] = $order;
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
    else {
      $sorting['sort'] = $sort;
    }
  }


  mcneese_event_workflow_include(4);

  $results = mcneese_event_workflow_event_load_listing(NULL, FALSE, $page_number * $rows_per_page, $rows_per_page, $sorting, array(), FALSE);

  $rows = array();
  if (!empty($results)) {
    foreach ($results as $item) {
      $row = array();

      $can_view_event = mcneese_event_workflow_management_page_event_access('view', $item->id);
      $can_edit_event = mcneese_event_workflow_management_page_event_access('edit', $item->id);
      $can_review_event = mcneese_event_workflow_management_page_event_access('review', $item->id);
      $can_log_event = mcneese_event_workflow_management_page_event_access('log', $item->id);

      $row['id'] = '<a href="' . $base_path . 'events/view-0/' . $item->id . '">' . $item->id . '</a>';

      $location_options = event_workflow_get_event_location_list_options();
      $building_options = event_workflow_get_event_building_list_options($item->location);
      $room_options = event_workflow_get_event_room_list_options();

      $location = "";
      if (empty($item->location)) {
        $location = "Unsure";
      }
      else {
        if (isset($location_options[$item->location])) {
          $location = check_plain($location_options[$item->location]);
        }
      }

      $building = "";
      if (!empty($item->building)) {
        if (isset($building_options[$item->building])) {
          $building = check_plain($building_options[$item->building]);
        }
      }

      $room = "";
      if (!empty($item->room)) {
        if (isset($room_options[$item->room])) {
          $room = check_plain($room_options[$item->room]);
        }
      }

      $row['location'] = $location;
      $row['building'] = $building;
      $row['room'] = $room;

      foreach ($item->date as $delta => $date) {
        if (!isset($row['date'])) {
          $row['date'] = '';
        }
        else {
          $row['date'] .= '<br>';
        }

        $row['date'] .= check_plain(date("Y/m/d", $date));

        if (array_key_exists($delta, $item->time_start) && !is_null($item->time_start[$delta]) && array_key_exists($delta, $item->time_stop) && !is_null($item->time_stop[$delta])) {
          $row['date'] .= ' ' . check_plain(date("h:ia", $item->time_start[$delta]));
          $row['date'] .= ' to ' . check_plain(date("h:ia", $item->time_stop[$delta]));
        }
      }

      $step_options = event_workflow_get_event_steps_list_options();

      $row['step'] = $step_options[$item->step];

      $submitter = user_load($item->user_id);

      if (is_object($submitter)) {
        $name = $submitter->name;

        if (!empty($submitter->field_user_first_name['und'][0]['value'])) {
          $name = $submitter->field_user_first_name['und'][0]['value'];

          if (!empty($submitter->field_user_last_name['und'][0]['value'])) {
            $name .= ' ' . $submitter->field_user_last_name['und'][0]['value'];
          }

          $name .= ' (' . $submitter->name . ')';
        }

        if ($administer || $manager) {
          $row['submitter_name'] = '<a href="' . $base_path . 'user/' . $submitter->uid . '">' . $name . '</a>';
        }
        else {
          $row['submitter_name'] = $name;
        }
      }
      else {
        // @todo: this is an error, report it.
        $row['submitter_name'] = '';
      }

      $date_stamp = $item->created;
      $row['created'] = check_plain(date("Y/m/d h:ia", $date_stamp));

      $row['operations'] = '';

      if ($can_view_event) {
        $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $item->id . '" class="operation-link operation-link-view">View</a> ';
      }

      if ($can_edit_event) {
        $row['operations'] .= '<a href="' . $base_path . 'events/edit-0/' . $item->id . '/request" class="operation-link operation-link-edit">Edit</a> ';
      }

      if ($can_review_event) {
        $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $item->id . '/3" class="operation-link operation-link-review">Review</a> ';
      }

      if ($can_log_event) {
        $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $item->id . '/2" class="operation-link operation-link-log">Log</a> ';
      }

      if ($can_delete) {
        // @todo: implement this.
        #$row['operations'] .= '<a href="' . $base_path . 'events/delete-0/' . $item->id . '" class="operation-link operation-link-delete">Delete</a> ';
      }

      $rows[$item->id] = $row;
    }
  }

  $table = array();
  $table['caption'] = "Event Requests";
  $table['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
    'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
    'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
    'step' => array('field' => 'step', 'data' => "Step", 'align' => 'center'),
    'submitted_by' => array('field' => 'submitted_by', 'data' => "Submitted By", 'align' => 'center'),
    'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
    t("Operations"),
  );

  $table['attributes'] = array();
  $table['attributes']['summary'] = "A listing of event requests associated by ID.";

  $table['rows'] = $rows;
  $table['empty'] = "There are no event requests available for to your account.";

  $output .= theme('table', $table);

  return $form;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
