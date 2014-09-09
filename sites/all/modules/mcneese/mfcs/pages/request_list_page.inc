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

  $columns = array(
    'id' => "ID",
    'title' => "Title",
    'building' => "Building",
    'room' => "Room",
    'date' => "Date",
    'status' => "Status",
    'submitted_by' => "Submitted By",
    'submitted_on' => "Submitted On",
    'operations' => "Operations"
  );

  $table = mfcs_build_request_table($items, $rows_per_page, $columns);
  $table['attributes']['class'] = array('mfcs-table-requests', 'mfcs-review_page-table');

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