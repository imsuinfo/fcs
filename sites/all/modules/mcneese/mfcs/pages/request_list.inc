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
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   An array containing the generated form.
 */
function mfcs_request_list_0_form($form, &$form_state, $page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/list-0');

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

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
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  $order_column = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  $class_id = '';
  $class_title = '';
  $class_building = '';
  $class_room = '';
  $class_date = '';
  $class_status = '';
  $class_step = 'no-script-hidden ';
  $class_submitted_by = '';
  $class_submitted_on = '';
  $class_operations = 'no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'DESC';
    $order_column = 0;
    $class_title .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 0;
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'title') {
      $sorting['title'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = 1;
      $class_title .= 'sorting_1 ';
    }
    elseif ($order == 'building') {
      $sorting['building'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = 2;
      $class_building .= 'sorting_1 ';
    }
    elseif ($order == 'room') {
      $sorting['room'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = 3;
      $class_room .= 'sorting_1 ';
    }
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 4;
      $class_date .= 'sorting_1 ';
    }
    elseif ($order == 'status') {
      $sorting['status'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = 5;
      $class_status .= 'sorting_1 ';
    }
    elseif ($order == 'step') {
      $sorting['step'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = 6;
      $class_step .= 'sorting_1 ';
    }
    elseif ($order == 'submitted by') {
      $sorting['submitted_by'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = 7;
      $class_submitted_by .= 'sorting_1 ';
    }
    elseif ($order == 'submitted on') {
      $sorting['submitted_on'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = 8;
      $class_submitted_on .= 'sorting_1 ';
    }
  }

  $rows_per_page = 25;
  $options = array(
    'sort' => $sorting,
    'join' => TRUE,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
  );


  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $items_returned = mfcs_request_load_listing(array(), array('start' => $options['start'], 'limit' => $rows_per_page, 'sort' => $sort, 'order' => $order));

  $count = 0;
  if (isset($items_returned['count'])) {
    $count = $items_returned['count'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($options['start'] > $count) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }


  $rows = array();
  $total_results = count($items);

  $table = mfcs_build_request_table($items, $rows_per_page, $columns);
  $table['attributes']['class'] = array('mfcs-table-requests', 'mfcs-review_page-table');

  if (is_null($order) && isset($table['header']['id'])) {
    $table['header']['id']['sort'] = 'DESC';
  }

  $navigation = "";
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation .= '<a href="' . $base_path . 'requests/list-0' . $url_arguments . '" class="request-listing-page-choice request-listing-page-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation .= '<a href="' . $base_path . 'requests/list-0/' . $previous_page . $url_arguments . '" class="request-listing-page-choice request-listing-page-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  if ($total_results > $rows_per_page) {
    $next_page = $page_number + 1;
    $navigation .= '<a href="' . $base_path . 'requests/list-0/' . $next_page . $url_arguments . '" class="request-listing-page-choice request-listing-page-next no-print" title="Next Page">Next</a>';
  }

  $output .= '<div class="request-listing-page-navigator request-listing-page-navigator-top no-print">' . $navigation . '</div>';
  $output .= theme('table', $table);
  $output .= '<div class="request-listing-page-navigator request-listing-page-navigator-bottom no-print">' . $navigation . '</div>';

  return $form;
}

/**
 * @} End of '@addtogroup mfcs'.
 */