<?php

/**
 * @file
 * Defines McNeese facilities use request review page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

 /**
 * Provides the facilities use request review page.
 *
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_review_0_page($form, &$form_state, $page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  global $base_path;

  $rows_per_page = 25;
  $start_range = $page_number * $rows_per_page;

  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];

  $user = cf_current_user();

  $can_create = mfcs_management_page_request_access('create');
  $can_edit = mfcs_management_page_request_access('edit');
  $can_view = mfcs_management_page_request_access('view');
  $can_list = mfcs_management_page_request_access('list');
  $can_search = mfcs_management_page_request_access('search');
  $can_delete = mfcs_management_page_request_access('delete');
  $can_log = mfcs_management_page_request_access('log');
  $can_review = mfcs_management_page_request_access('review');

  $manager = user_access('mfcs manage');
  $reviewer = user_access('mfcs review');
  $requester = user_access('mfcs request');


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

  $results = mfcs_request_load_listing(NULL, FALSE, $page_number * $rows_per_page, $rows_per_page, $sorting, TRUE);

  $rows = array();
  $total_results = 0;
  if (!empty($results)) {
    $total_results = count($results);
    $rows_count = 0;

    foreach ($results as $item) {
      $row = array();
      $rows_count++;

      // Rows per page + 1 is loaded so that it can be detected whether or not a 'next' link needs to be provided.
      if ($rows_count > $rows_per_page) {
        break;
      }

      $can_edit_request = mfcs_management_page_request_access('edit', $item->id);
      $can_review_request = mfcs_management_page_request_access('review', $item->id);
      $can_log_request = mfcs_management_page_request_access('log', $item->id);
      $can_history_request = mfcs_management_page_request_access('history', $item->id);

      $row['id'] = $item->id;
      $row['title'] = check_plain($item->title);
      $row['location'] = check_plain($item->location_name);
      $row['building'] = check_plain($item->building_name);
      $row['room'] = check_plain($item->room_name);

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

      $step_options = mfcs_get_request_steps_list_options();

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

        if ($manager) {
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
      $row['operations'] .= '<a href="' . $base_path . 'requests/view-0/' . $item->id . '" class="operation-link operation-link-view">View</a> ';

      if ($can_edit_request) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/edit-0/' . $item->id . '/request" class="operation-link operation-link-edit">Edit</a> ';
      }

      if ($can_review_request) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/view-0/' . $item->id . '/3" class="operation-link operation-link-review">Review</a> ';
      }

      if ($can_log_request) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/view-0/' . $item->id . '/2" class="operation-link operation-link-log">Log</a> ';
      }

      if ($can_history_request) {
        $row['operations'] .= '<a href="' . $base_path . 'requests/history-0/' . $item->id . '" class="operation-link operation-link-history">History</a> ';
      }

      if ($can_delete) {
        // @todo: implement this.
        #$row['operations'] .= '<a href="' . $base_path . 'requests/delete-0/' . $item->id . '" class="operation-link operation-link-delete">Delete</a> ';
      }

      if ($can_create) {
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
    'step' => array('field' => 'step', 'data' => "Step", 'align' => 'center'),
    'submitted_by' => array('field' => 'submitted_by', 'data' => "Submitted By", 'align' => 'center'),
    'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
    t("Operations"),
  );

  $table['attributes'] = array();
  $table['attributes']['summary'] = "A listing of requests associated by ID.";

  $table['rows'] = $rows;
  $table['empty'] = "There are no requests available for to your account.";

  $output .= theme('table', $table);

  $output .= '<div class="request-listing-page-navigator">';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $output .= '<a href="' . $base_path . 'requests/review-0" class="request-listing-page-choice request-listing-page-previous" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $output .= '<a href="' . $base_path . 'requests/review-0/' . $previous_page . '" class="request-listing-page-choice request-listing-page-previous" title="Previous Page">Prev</a>';
    }
  }

  if ($total_results > $rows_per_page) {
    $next_page = $page_number + 1;
    $output .= '<a href="' . $base_path . 'requests/review-0/' . $next_page . '" class="request-listing-page-choice request-listing-page-next" title="Next Page">Next</a>';
  }
  $output .= '</div>';

  return $form;
}

/**
 * @} End of '@addtogroup mfcs'.
 */