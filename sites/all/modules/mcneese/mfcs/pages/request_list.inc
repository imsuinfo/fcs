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
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   An array containing the generated form.
 */
function mfcs_request_list_0_form($form, &$form_state, $page_number = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $no_js_fallback = TRUE;
  if (isset($conf['no_js_fallback']) && is_bool($conf['no_js_fallback'])) {
    $no_js_fallback = $conf['no_js_fallback'];
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/list-0/' . $page_number);

  $user = cf_current_user();

  $administer = FALSE;
  $manager = FALSE;
  $reviewer = FALSE;
  $requester = FALSE;
  if ($user->uid === MFCS_USER_REQUEST) {
    // this is a simulated user account and must be handled as a special case.
    $requester = TRUE;
  }
  else {
    $administer = user_access('mfcs administer', $user);
    $manager = user_access('mfcs manage', $user);
    $reviewer = user_access('mfcs review', $user);
    $requester = user_access('mfcs request', $user);
  }

  $show_my_request_only_default = FALSE;
  if ($requester && !($reviewer || $manager || $administer)) {
    $show_my_request_only_default = TRUE;
  }

  $page_title = "Facilities Use Requests";
  drupal_set_title($page_title);

  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];

  // load sorting information
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  $columns_default = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
      $order = NULL;
    }
  }

  $columns = mfcs_request_list_0_columns();

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'DESC';
    $columns_default = array(0 => strtolower($sorting['id']));
    $columns['id']['class'][] = 'sorting_1';
  }
  else {
    if ($order == 'id') {
      $sorting[$order] = is_null($sort) ? 'DESC' : $sort;
      $columns_default = array(0 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'type') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(1 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'title') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(2 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'building') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(3 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'room') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(4 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'date') {
      $sorting[$order] = is_null($sort) ? 'DESC' : $sort;
      $columns_default = array(5 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'status') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(6 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'step') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(7 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'submitted by') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(8 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'submitter name') {
      $sorting[$order] = is_null($sort) ? 'ASC' : $sort;
      $columns_default = array(9 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
    elseif ($order == 'submitted on') {
      $sorting[$order] = is_null($sort) ? 'DESC' : $sort;
      $columns_default = array(10 => strtolower($sorting[$order]));
      $columns[$order]['class'][] = 'sorting_1';
    }
  }

  $rows_per_page = 25;
  $conditions = array();
  $options = array(
    'join' => TRUE,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
    'only_requester' => $show_my_request_only_default,
  );

  if (!is_null($order)) {
    $options['sort'] = $sorting;
  }

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $options['results'] = FALSE;
  }


  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $items_returned = mfcs_request_load_listing($conditions, $options);

  $count = 0;
  if (isset($items_returned['count'])) {
    $count = $items_returned['count'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($count > 0 && $options['start'] >= $count || $count == 0 && $options['start'] > $count) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // do not show count values when no fallback is available.
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $count = 0;
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $options['columns'] = $columns;
  $options['columns_default'] = $columns_default;

  $options['ajax_path'] = "'" . $base_path . 'requests/list-0/ajax' . $url_arguments . "'";

  $table = mfcs_build_request_ajax_table($items_returned, $conditions, $options);
  $table['attributes']['class'][] = 'mfcs-table-requests';
  $table['attributes']['class'][] = 'mfcs-review_page-table';
  $table['attributes']['id'] = 'mfcs-list_0-requests';

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/list-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/list-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/list-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/list-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($count == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $count) {
    $navigation_total = $count;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $count . " entries.";
  $navigation_bottom .= '</div>';

  if ($count > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/list-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/list-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $output = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $output .= mcneese_libraries_theme_table($table, $table['datatable_settings']);
  #$output .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  return $form;
}

/**
 * Returns an array of columns for the request list.
 *
 * @return array
 *   An array of columns for the request list.
 */
function mfcs_request_list_0_columns() {
  return array(
    'id' => array(
      'id' => 0,
      'column' => 'id',
      'name' => "ID",
      'class' => array('column-id'),
      'sanitize' => FALSE,
      'align' => 'center',
    ),
    'type' => array(
      'id' => 1,
      'column' => 'type',
      'name' => "Type",
      'class' => array('column-type', 'no-script-hidden'),
      'align' => 'center',
      'visible' => FALSE,
    ),
    'title' => array(
      'id' => 2,
      'column' => 'title',
      'name' => "Request Name",
      'class' => array('column-title'),
      'align' => 'center',
    ),
    'building' => array(
      'id' => 3,
      'column' => 'building',
      'name' => "Building",
      'class' => array('column-building'),
      'align' => 'center',
    ),
    'room' => array(
      'id' => 4,
      'column' => 'room',
      'name' => "Room",
      'class' => array('column-room'),
      'align' => 'center',
    ),
    'date' => array(
      'id' => 5,
      'column' => 'date',
      'name' => "Date",
      'class' => array('column-date'),
      'align' => 'center',
    ),
    'status' => array(
      'id' => 6,
      'column' => 'status',
      'name' => "Status",
      'class' => array('column-status'),
      'align' => 'center',
    ),
    'step' => array(
      'id' => 7,
      'column' => 'step',
      'name' => "Step",
      'class' => array('column-step', 'no-script-hidden'),
      'align' => 'center',
      'visible' => FALSE,
    ),
    'submitted_by_id' => array(
      'id' => 8,
      'column' => 'user_id',
      'name' => "Submitted By ID",
      'class' => array('column-submitted_by_id', 'no-script-hidden'),
      'sanitize' => FALSE,
      'visible' => FALSE,
      'align' => 'center',
    ),
    'submitted_by' => array(
      'id' => 9,
      'column' => 'user_id',
      'name' => "Submitted By",
      'class' => array('column-submitted_by'),
      'align' => 'center',
    ),
    'submitted_on' => array(
      'id' => 10,
      'column' => 'created',
      'name' => "Submitted On",
      'class' => array('column-submitted_on'),
      'align' => 'center',
    ),
    'operations' => array(
      'id' => 11,
      'column' => NULL,
      'name' => "Operations",
      'class' => array('column-operations', 'no-print'),
      'align' => 'center',
      'searchable' => FALSE,
      'orderable' => FALSE,
    ),
  );
}

/**
 * @} End of '@addtogroup mfcs'.
 */
