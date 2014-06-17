<?php

/**
 * @file
 * Defines McNeese event workflow management page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event manage page.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see mcneese_event_workflow_blocks_at_path()
 */
function mcneese_event_workflow_management_page() {
  global $base_path;
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

  $items_1 = array();
  $items_2 = array();
  $items_3 = array();
  $items_4 = array();

  if ($can_create) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/create-0" class="item-link">Request Event</a></h4>';
    $help = '<div class="item-description inline-block">Make a request to use the facilities or host special events at McNeese State University.</div>';
    $items_1[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_list) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/list-0" class="item-link">List Requests</a></h4>';
    $help = '<div class="item-description inline-block">View a list of event requests available to the current user.</div>';
    $items_1[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_search) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/search-0" class="item-link">Search Requests</a></h4>';
    $help = '<div class="item-description inline-block">Use advanced search tools to help locate and find one or more event requests.</div>';
    $items_3[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_review) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/review-0" class="item-link">Review Requests</a></h4>';
    $help = '<div class="item-description inline-block">Manage requests the current user is allowed to review.</div>';
    $items_2[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($administer || $manager) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/reviewers-0" class="item-link">Manage Reviewers</a></h4>';
    $help = '<div class="item-description inline-block">Manage what users are allowed to approve/deny event requests by event type.</div>';
    $items_2[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  $item_list_1 = theme('item_list', array('items' => $items_1, 'type' => 'ul'));
  $item_list_2 = theme('item_list', array('items' => $items_2, 'type' => 'ul'));
  $item_list_3 = theme('item_list', array('items' => $items_3, 'type' => 'ul'));

  $markup = '<div id="mcneese_event_workflow-management_page-choices" class="mcneese_event_workflow-management_page-choices">';
  $markup .= '<h3 class="mcneese_event_workflow-management_page-choices-header">Facilities Use and Special Event Request Form</h3>';
  $markup .= '<div class="mcneese_event_workflow-management_page-choices-list_1">' . $item_list_1 . '</div>';
  $markup .= '<div class="mcneese_event_workflow-management_page-choices-list_2">' . $item_list_2 . '</div>';
  $markup .= '<div class="mcneese_event_workflow-management_page-choices-list_3">' . $item_list_3 . '</div>';
  $markup .= '</div>';


  if ($can_list) {
    $total_items = 5;

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

    $results = mcneese_event_workflow_event_load_listing(NULL, FALSE, 0, $total_items, $sorting, array(), 1);

    // @todo: apply sorting for all fields that are integers in the database but are displayed as strings.

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

    $quick_list = array();
    $quick_list['caption'] = t("Most Recent Event Requests");
    $quick_list['header'] = array(
      'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
      'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
      'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
      'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
      'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
      'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
      t("Operations"),
    );

    $quick_list['attributes'] = array();
    $quick_list['attributes']['id'] = 'mcneese_event_workflow-management_page-quick_list';
    $quick_list['attributes']['class'] = array ('mcneese_event_workflow-management_page-quick_list');
    $quick_list['attributes']['summary'] = 'A short version of the list, limited to the ' . $total_items . ' most recent items.';

    $quick_list['rows'] = $rows;
    $quick_list['empty'] = t("There are no event requests available to your account.");

    $markup .= theme('table', $quick_list);
  }

  return $markup;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
