<?php

/**
 * @file
 * Defines McNeese facilities use request list page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request list page.
 *
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   An array containing the generated form.
 */
function mfcs_request_list_0_page($form, &$form_state, $page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/list-0');

  global $base_path;

  $rows_per_page = 25;
  $start_range = $page_number * $rows_per_page;

  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];

  $user = cf_current_user();

  $can_create = mfcs_page_request_access('create', NULL, $user);
  $can_view = mfcs_page_request_access('view', NULL, $user);
  $can_list = mfcs_page_request_access('list', NULL, $user);
  $can_search = mfcs_page_request_access('search', NULL, $user);
  $can_review = mfcs_page_request_access('review', NULL, $user);

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);


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

  mfcs_include(4);

  $items = mfcs_request_load_listing(NULL, FALSE, $page_number * $rows_per_page, $rows_per_page, $sorting);

  $rows = array();
  $total_results = 0;
  if (!empty($items)) {
    $item_ids = array_keys($items);

    $can_create_requests = mfcs_page_request_access('create', $item_ids, $user);
    $can_edit_requests = mfcs_page_request_access('edit', $item_ids, $user);
    $can_review_requests = mfcs_page_request_access('review', $item_ids, $user);
    $can_log_requests = mfcs_page_request_access('log', $item_ids, $user);
    $can_history_requests = mfcs_page_request_access('history', $item_ids, $user);
    $can_delete_requests = mfcs_page_request_access('delete', $item_ids, $user);

    if (!is_array($can_create_requests)) {
      $can_create_requests = array();
    }

    if (!is_array($can_edit_requests)) {
      $can_edit_requests = array();
    }

    if (!is_array($can_review_requests)) {
      $can_review_requests = array();
    }

    if (!is_array($can_log_requests)) {
      $can_log_requests = array();
    }

    if (!is_array($can_history_requests)) {
      $can_history_requests = array();
    }

    if (!is_array($can_delete_requests)) {
      $can_delete_requests = array();
    }

    $total_results = count($items);
    $rows_count = 0;

    foreach ($items as $item) {
      $row = array();
      $rows_count++;

      // Rows per page + 1 is loaded so that it can be detected whether or not a 'next' link needs to be provided.
      if ($rows_count > $rows_per_page) {
        break;
      }

      $row['id'] = $item->id;
      $row['title'] = check_plain($item->title);
      $row['location'] = check_plain($item->location_name);
      $row['building'] = check_plain($item->building_name);
      $row['room'] = check_plain($item->room_name);

      if (empty($item->date)) {
        $row['date'] = '';
      }
      else {
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
      }

      $status_options = mfcs_get_request_status_list_options();

      $row['status'] = $status_options[$item->status];

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

        if ($manager) {
          $row['submitter_name'] = '<a href="' . $base_path . 'user/' . $submitter->uid . '">' . $name . '</a>';
        }
        else {
          $row['submitter_name'] = $name;
        }
      }
      else {
        watchdog(MFCS_WATCHDOG_ID, "Failed to load the submitter a user id of %user_id. No submitter name can be provided.", array('%user_id' => $item->user_id), WATCHDOG_WARNING);
        $row['submitter_name'] = '';
      }

      $date_stamp = $item->created;
      $row['created'] = check_plain(date("Y/m/d h:ia", $date_stamp));

      $row['operations'] = '';

      $row['operations'] .= '<a href="' . $base_path . 'requests/view-0/' . $item->id . '" class="operation-link operation-link-view">View</a> ';

      if (array_key_exists($item->id, $can_edit_requests) && $can_edit_requests[$item->id]) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/edit-0/' . $item->id . '/request" class="operation-link operation-link-edit">Edit</a> ';
      }

      if (array_key_exists($item->id, $can_review_requests) && $can_review_requests[$item->id]) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/view-0/' . $item->id . '/3" class="operation-link operation-link-review">Review</a> ';
      }

      if (array_key_exists($item->id, $can_log_requests) && $can_log_requests[$item->id]) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/view-0/' . $item->id . '/2" class="operation-link operation-link-log">Log</a> ';
      }

      if (array_key_exists($item->id, $can_history_requests) && $can_history_requests[$item->id]) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/history-0/' . $item->id . '" class="operation-link operation-link-history">History</a> ';
      }

      if (array_key_exists($item->id, $can_delete_requests) && $can_delete_requests[$item->id]) {
        // @todo: implement this.
        #$row['operations'] .= '<a href="' . $base_path . 'requests/delete-0/' . $item->id . '" class="operation-link operation-link-delete">Delete</a> ';
      }

      if (array_key_exists($item->id, $can_create_requests) && $can_create_requests[$item->id]) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/create-0/' . $item->id . '" class="operation-link operation-link-clone">Copy</a> ';
      }

      $rows[$item->id] = $row;
    }
  }

  $table = array();
  $table['caption'] = "Requests";
  $table['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'title' => array('field' => 'title', 'data' => "Title", 'align' => 'center'),
    'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
    'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
    'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
    'status' => array('field' => 'status', 'data' => "Status", 'align' => 'center'),
    'submitted_by' => array('field' => 'submitted_by', 'data' => "Submitted By", 'align' => 'center'),
    'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
    t("Operations"),
  );

  $table['attributes'] = array();
  $table['attributes']['summary'] = "A listing of requests associated by ID.";

  $table['rows'] = $rows;
  $table['empty'] = "There are no requests available for to your account.";

  $output .= theme('table', $table);

  $output .= '<div class="request-listing-page-navigator no-print">';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $output .= '<a href="' . $base_path . 'requests/list-0" class="request-listing-page-choice request-listing-page-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $output .= '<a href="' . $base_path . 'requests/list-0/' . $previous_page . '" class="request-listing-page-choice request-listing-page-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  if ($total_results > $rows_per_page) {
    $next_page = $page_number + 1;
    $output .= '<a href="' . $base_path . 'requests/list-0/' . $next_page . '" class="request-listing-page-choice request-listing-page-next no-print" title="Next Page">Next</a>';
  }
  $output .= '</div>';

  return $form;
}

/**
 * @} End of '@addtogroup mfcs'.
 */