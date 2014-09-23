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

  // load sorting information
  $url_extras = NULL;
  $sorting = array();
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
    $sorting['order'] = $order;

    if (array_key_exists($order, $columns)) {
      $url_extras = 'order=' . $columns[$order];
    }
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
    else {
      $sorting['sort'] = $sort;

      if (is_null($url_extras)) {
        $url_extras = 'sort=' . $_GET['sort'];
      }
      else {
        $url_extras = 'sort=' . $_GET['sort'] . '&' . $url_extras;
      }
    }
  }

  if (!empty($url_extras)) {
    $url_extras = '?' . $url_extras;
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $items = mfcs_request_load_listing(NULL, FALSE, $page_number * $rows_per_page, $rows_per_page, $sorting);

  $rows = array();
  $total_results = count($items);

  $table = mfcs_build_request_table($items, $rows_per_page, $columns);
  $table['attributes']['class'] = array('mfcs-table-requests', 'mfcs-review_page-table');

  $navigation = "";
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation .= '<a href="' . $base_path . 'requests/list-0' . $url_extras . '" class="request-listing-page-choice request-listing-page-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation .= '<a href="' . $base_path . 'requests/list-0/' . $previous_page . $url_extras . '" class="request-listing-page-choice request-listing-page-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  if ($total_results > $rows_per_page) {
    $next_page = $page_number + 1;
    $navigation .= '<a href="' . $base_path . 'requests/list-0/' . $next_page . $url_extras . '" class="request-listing-page-choice request-listing-page-next no-print" title="Next Page">Next</a>';
  }

  $output .= '<div class="request-listing-page-navigator request-listing-page-navigator-top no-print">' . $navigation . '</div>';
  $output .= theme('table', $table);
  $output .= '<div class="request-listing-page-navigator request-listing-page-navigator-bottom no-print">' . $navigation . '</div>';

  return $form;
}

/**
 * @} End of '@addtogroup mfcs'.
 */