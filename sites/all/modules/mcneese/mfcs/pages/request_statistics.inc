<?php

/**
 * @file
 * Defines McNeese facilities use request statistics page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request statistics main page(s).
 *
 * @param int $year
 *   (optional) When specified, represents the year.
 * @param string $month
 *   (optional) When specified, represents the month.
 *   Causes $year to be required.
 * @param string $day
 *   (optional) When specified, represents the day.
 *   Causes $year and $month to be required.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_statistics_0_page($year = NULL, $month = NULL, $day = NULL) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($month) && !is_string($month)) {
    cf_error::invalid_string('month');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($day) && !cf_is_integer($day)) {
    cf_error::invalid_integer('day');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year) && !is_null($month) || is_null($year) && is_null($month) && !is_null($day)) {
    drupal_not_found();
    drupal_exit();
  }

  $valid_month_names = array(
    1 => 'january',
    2 => 'february',
    3 => 'march',
    4 => 'april',
    5 => 'may',
    6 => 'june',
    7 => 'july',
    8 => 'august',
    9 => 'september',
    10 => 'october',
    11 => 'november',
    12 => 'december',
  );

  if (!is_null($month)) {
    if (cf_is_integer($month)) {
      if (!array_key_exists($month, $valid_month_names)) {
        drupal_not_found();
        drupal_exit();
      }

      $month = $valid_month_names[$month];
    }
    elseif (is_string($month)) {
      if (!in_array($month, $valid_month_names)) {
        drupal_not_found();
        drupal_exit();
      }
    }
    else {
      drupal_not_found();
      drupal_exit();
    }
  }

  if (!is_null($day) && (!cf_is_integer($day) || $day < 1 || $day > 31)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_path;

  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $day_start = NULL;
  $day_stop = NULL;
  if (!is_null($year)) {
    $year_start = strtotime('midnight january 1,  ' . $year);
    $year_stop = strtotime('next year', $year_start);
    $year_last = strtotime('previous year', $year_start);

    if (is_null($month)) {
      // nothing to do.
    }
    elseif (is_null($day)) {
      $month_start = strtotime('midnight ' . $month . ' 1,  ' . $year);
      $month_stop = strtotime('next month', $month_start);
      $month_name = date('F', $month_start);
    }
    else {
      $month_start = strtotime('midnight ' . $month . ' 1,  ' . $year);
      $month_stop = strtotime('next month', $month_start);
      $month_name = date('F', $month_start);

      $day_start = strtotime('midnight ' . $month . ' ' . $day . ' ' . $year);
      $day_stop = strtotime('midnight tomorrow', $day_start);
    }

  }
  else {
    $instance = mfcs_instance();
    $year = date('Y', $instance);
    $year_start = strtotime('midnight january 1, ' . $year);
    $year_stop = strtotime('next year', $year_start);
    $year_last = strtotime('previous year', $year_start);

    $month = NULL;
    $day = NULL;
  }

  if (is_null($year) || is_null($month)) {
    $previous = $year_last;
    $next = $year_stop;

    $path_previous = date('Y', $previous);
    $path_next = date('Y', $next);

    $string_previous = date('Y', $previous);
    $string_next = date('Y', $next);
  }
  elseif (is_null($day)) {
    $previous = strtotime('previous month', $month_start);
    $next = $month_stop;

    $month_name_previous = date('F', $previous);
    $month_name_next = date('F', $next);

    $path_previous = date('Y', $previous) . '/' . strtolower($month_name_previous);
    $path_next = date('Y', $next) . '/' . strtolower($month_name_next);

    $string_previous = $month_name_previous . ' ' . date('Y', $previous);
    $string_next = $month_name_next . ' ' . date('Y', $next);
  }
  else {
    $previous = strtotime('previous day', $day_start);
    $next = $day_stop;

    $month_name_previous = date('F', $previous);
    $month_name_next = date('F', $next);

    $path_previous = date('Y', $previous) . '/' . strtolower($month_name_previous) . '/' . date('d', $previous);
    $path_next = date('Y', $next) . '/' . strtolower($month_name_next) . '/' . date('d', $next);

    $string_previous = $month_name_previous . ' ' . date('d', $previous) . ', ' . date('Y', $previous);
    $string_next = $month_name_next . ' ' . date('d', $next) . ', ' . date('Y', $next);
  }

  $status_options = mfcs_get_request_status_list_options();
  $steps_options = mfcs_get_review_steps_list_options();
  $type_options = mfcs_get_request_type_list_options();
  $classification_options = mfcs_get_request_classification_list_options();

  $now = date("Y/m/d h:i:s a");

  $markup = '<div id="mfcs-statistics-page" class="statistics-page">';

  // next/prev years
  $markup .= '<div class="statistics-navigation position-relative no-print clear">';
  $markup .= '  <a href="' . $base_path . 'requests/statistics-0/' . $path_previous . '" class="statistics-navigation-link link-previous inline-block" title="Statistics for ' . $string_previous . '">Prev</a>';
  $markup .= '  <a href="' . $base_path . 'requests/statistics-0/' . $path_next . '" class="statistics-navigation-link link-previous inline-block float-right" title="Statistics for for ' . $string_next . '">Next</a>';
  $markup .= '</div>';

  $markup .= '<div class="statistics-title">';
  if (is_null($month)) {
    mfcs_add_canonical_headers('requests/statistics-0/' . $year);
    $markup .= '<h3 class="statistics-title-text">' . "Request Statistics for the year " . $year . ' <span class="statistics-title-text-note">(generated on ' . $now . ')</span></h3>';

    $counts = mfcs_request_statistics_load_counts($year_start, $year_stop);
  }
  elseif (is_null($day)) {
    $month = strtolower(date('F', $month_start));
    $year = date('Y', $month_start);
    mfcs_add_canonical_headers('requests/statistics-0/' . $year . '/' . $month);
    $markup .= '<h3 class="statistics-title-text">' . "Request Statistics for " . $month_name . " " . $year . ' <span class="statistics-title-text-note">(generated on ' . $now . ')</span></h3>';

    $counts = mfcs_request_statistics_load_counts($month_start, $month_stop);
  }
  else {
    $month = strtolower(date('F', $day_start));
    $year = date('Y', $day_start);
    $day = date('d', $day_start);

    mfcs_add_canonical_headers('requests/statistics-0/' . $year . '/' . $month . '/' . $day);
    $markup .= '<h3 class="statistics-title-text">' . "Request Statistics for " . $month_name . " " . $day . ", " . $year . ' <span class="statistics-title-text-note">(generated on ' . $now . ')</span></h3>';

    $counts = mfcs_request_statistics_load_counts($day_start, $day_stop);
  }
  $markup .= '</div>'; // statistics-title

  $markup .= mfcs_request_statistics_render_request_classification($counts, $classification_options);
  $markup .= '<hr>';
  $markup .= mfcs_request_statistics_render_request_type($counts, $type_options);
  $markup .= '<hr>';
  $markup .= mfcs_request_statistics_render_request_step($counts, $steps_options);
  $markup .= '<hr>';
  $markup .= mfcs_request_statistics_render_request_status($counts, $status_options);

  $markup .= '</div>'; // mfcs-statistics-page

  return $markup;
}

/**
 * Load main statistics numbers.
 *
 * @param int|null $start
 *   When not NULL, is the unix timestamp for start time.
 *   A >= operation is performed on this.
 * @param int|null $stop
 *   When not NULL, is the unix timestamp for stop time.
 *   A < operation is performed on this.
 *
 * @return array|bool
 *   An array of main statistics or FALSE on failure.
 */
function mfcs_request_statistics_load_counts($start, $stop) {
  if (!is_null($start) && !cf_is_integer($start) || is_null($start) && !is_null($stop)) {
    cf_error::invalid_integer('start');
    return FALSE;
  }

  if (!is_null($stop) && !cf_is_integer($stop) || !is_null($start) && is_null($stop)) {
    cf_error::invalid_integer('stop');
    return FALSE;
  }

  $counts = array();

  // main
  $counts['main'] = array(
    'total' => NULL,
  );

  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->addExpression('count(mer.id)');

    $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

    // total
    $the_query = clone($query);

    if (!is_null($start)) {
      $the_query->condition('mer.created', $start, '>=');
      $the_query->condition('mer.created', $stop, '<');
    }

    $counts['main']['total'] = $the_query->execute()->fetchField();


    // status
    $counts['status'] = array(
      'locked' => NULL,
      'unlocked' => NULL,
      'accepted' => NULL,
      'denied' => NULL,
      'unavailable' => NULL,
      'due_to_lock' => NULL,
      'cancelled' => NULL,
      'accepted_cancelled' => NULL,
    );

    $query = db_select('mfcs_requests', 'mer');

    $query->addExpression('count(mer.id)');

    $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

    if (!is_null($start)) {
      $query->condition('mer.created', $start, '>=');
      $query->condition('mer.created', $stop, '<');
    }

    // status (locked)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_LOCKED);
    $counts['status']['locked'] = $the_query->execute()->fetchField();

    // status (unlocked)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_UNLOCKED);
    $counts['status']['unlocked'] = $the_query->execute()->fetchField();

    // status (closed, accepted)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_ACCEPTED);
    $counts['status']['accepted'] = $the_query->execute()->fetchField();

    // status (closed, denied)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_DENIED);
    $counts['status']['denied'] = $the_query->execute()->fetchField();

    // status (closed, unavailable)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE);
    $counts['status']['unavailable'] = $the_query->execute()->fetchField();

    // status (closed, due to lock)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK);
    $counts['status']['due_to_lock'] = $the_query->execute()->fetchField();

    // status (cancelled)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_CANCELLED);
    $counts['status']['cancelled'] = $the_query->execute()->fetchField();

    // status (accepted, but cancelled)
    $the_query = clone($query);
    $the_query->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED);
    $counts['status']['accepted_cancelled'] = $the_query->execute()->fetchField();


    // step
    $counts['step'] = array(
      'changes_required' => NULL,
      'completed' => NULL,
      'make_decisions' => NULL,
      'release_hold' => NULL,
      'review' => NULL,
      'requirements' => NULL,
      'venue_available' => NULL,
    );

    $query = db_select('mfcs_requests', 'mer');

    $query->addExpression('count(mer.id)');

    $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

    if (!is_null($start)) {
      $query->condition('mer.created', $start, '>=');
      $query->condition('mer.created', $stop, '<');
    }

    // step (changes required)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_CHANGES_REQUIRED);
    $counts['step']['changes_required'] = $the_query->execute()->fetchField();

    // step (completed)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_COMPLETED);
    $counts['step']['completed'] = $the_query->execute()->fetchField();

    // step (final approval)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_FINAL_APPROVAL);
    $counts['step']['make_decisions'] = $the_query->execute()->fetchField();

    // step (release hold)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_RELEASE_HOLD);
    $counts['step']['release_hold'] = $the_query->execute()->fetchField();

    // step (review)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_REVIEW);
    $counts['step']['review'] = $the_query->execute()->fetchField();

    // step (requirements)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_REQUIREMENTS);
    $counts['step']['requirements'] = $the_query->execute()->fetchField();

    // step (venue available)
    $the_query = clone($query);
    $the_query->condition('mer.step', MFCS_REVIEW_STEP_VENUE_AVAILABLE);
    $counts['step']['venue_available'] = $the_query->execute()->fetchField();


    // request classification
    $counts['classification'] = array(
      'student' => NULL,
      'camps' => NULL,
      'facility' => NULL,
      'external' => NULL,
    );

    $query = db_select('mfcs_requests', 'mer');

    $query->addExpression('count(mer.id)');

    $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

    if (!is_null($start)) {
      $query->condition('mer.created', $start, '>=');
      $query->condition('mer.created', $stop, '<');
    }

    // classification (student)
    $the_query = clone($query);
    $the_query->condition('mer.classification', MFCS_REQUEST_CLASSIFICATION_STUDENT);
    $counts['classification']['student'] = $the_query->execute()->fetchField();

    // classification (camps)
    $the_query = clone($query);
    $the_query->condition('mer.classification', MFCS_REQUEST_CLASSIFICATION_CAMPS);
    $counts['classification']['camps'] = $the_query->execute()->fetchField();

    // classification (faculty)
    $the_query = clone($query);
    $the_query->condition('mer.classification', MFCS_REQUEST_CLASSIFICATION_FACULTY);
    $counts['classification']['faculty'] = $the_query->execute()->fetchField();

    // classification (external)
    $the_query = clone($query);
    $the_query->condition('mer.classification', MFCS_REQUEST_CLASSIFICATION_EXTERNAL);
    $counts['classification']['external'] = $the_query->execute()->fetchField();


    // request type
    $counts['type'] = array(
      'athletic' => NULL,
      'banquery' => NULL,
      'ceremony' => NULL,
      'fair' => NULL,
      'lecture' => NULL,
      'meeting' => NULL,
      'other' => NULL,
      'performance' => NULL,
      'quick_meeting' => NULL,
      'talent' => NULL,
      'workshop' => NULL,
    );

    $query = db_select('mfcs_requests', 'mer');

    $query->addExpression('count(mer.id)');

    $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

    if (!is_null($start)) {
      $query->condition('mer.created', $start, '>=');
      $query->condition('mer.created', $stop, '<');
    }

    // type (athletic)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_ATHLETIC);
    $counts['type']['athletic'] = $the_query->execute()->fetchField();

    // type (banquet)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_BANQUET);
    $counts['type']['banquet'] = $the_query->execute()->fetchField();

    // type (ceremony)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_CEREMONY);
    $counts['type']['ceremony'] = $the_query->execute()->fetchField();

    // type (fair)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_FAIR);
    $counts['type']['fair'] = $the_query->execute()->fetchField();

    // type (lecture)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_LECTURE);
    $counts['type']['lecture'] = $the_query->execute()->fetchField();

    // type (meeting)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_MEETING);
    $counts['type']['meeting'] = $the_query->execute()->fetchField();

    // type (other)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_OTHER);
    $counts['type']['other'] = $the_query->execute()->fetchField();

    // type (performance)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_PERFORMANCE);
    $counts['type']['performance'] = $the_query->execute()->fetchField();

    // type (quick_meeting)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_QUICK_MEETING);
    $counts['type']['quick_meeting'] = $the_query->execute()->fetchField();

    // type (reception)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_RECEPTION);
    $counts['type']['reception'] = $the_query->execute()->fetchField();

    // type (talent)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_TALENT);
    $counts['type']['talent'] = $the_query->execute()->fetchField();

    // type (workshop)
    $the_query = clone($query);
    $the_query->condition('mer.type', MFCS_REQUEST_TYPE_WORKSHOP);
    $counts['type']['workshop'] = $the_query->execute()->fetchField();
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    return FALSE;
  }

  return $counts;
}

/**
 * Builds the HTML markup containing the classification statistics block.
 *
 * @param array $counts
 *   The collection of all calculated statistics as generated by
 *   mfcs_request_statistics_load_counts().
 * @param array $classification_options
 *   The array of classification options as generated by
 *   mfcs_get_request_classification_list_options().
 *
 * @return string|bool
 *   The HTML markup string or FALSE on error.
 *
 * @see: mfcs_request_statistics_load_counts()
 * @see: mfcs_get_request_classification_list_options()
 */
function mfcs_request_statistics_render_request_classification($counts, $classification_options) {
  if (!is_array($counts) || empty($counts)) {
    cf_error::invalid_string('counts');
    return FALSE;
  }

  if (!is_array($classification_options) || empty($classification_options)) {
    cf_error::invalid_string('classification_options');
    return FALSE;
  }

  global $base_path;

  $user = cf_current_user();
  $module_path = drupal_get_path('module', 'mfcs');


  // render chart
  $options = array(
    'total' => $counts['main']['total'],
    'rows' => array(),
    'text_title' => "Requests by Classification",
    'text_description' => 'A bar chart representing the number of requests for each classification they belong to. There are a total of ' . $counts['main']['total'] . 'requests.',
    'text_x' => "Requests",
    'text_y' => "Classifications",
    'chart_height' => 220,
    'line_start_x' => 75,
    'line_start_y' => 25,
    'name' => 'classification',
  );

  $options['rows']['student'] = array(
    'total' => $counts['classification']['student'],
    'text' => $classification_options[MFCS_REQUEST_CLASSIFICATION_STUDENT],
    'label' => "Student Org.",
    'color' => '#5f71a6',
  );

  $options['rows']['faculty'] = array(
    'total' => $counts['classification']['faculty'],
    'text' => $classification_options[MFCS_REQUEST_CLASSIFICATION_FACULTY],
    'label' => "Faculty, Staff,",
    'label_2' => "Athletics",
    'color' => '#5d9ea3',
  );

  $options['rows']['camps'] = array(
    'total' => $counts['classification']['camps'],
    'text' => $classification_options[MFCS_REQUEST_CLASSIFICATION_CAMPS],
    'label' => $classification_options[MFCS_REQUEST_CLASSIFICATION_CAMPS],
    'color' => '#ab6fa3',
  );

  $options['rows']['external'] = array(
    'total' => $counts['classification']['external'],
    'text' => $classification_options[MFCS_REQUEST_CLASSIFICATION_EXTERNAL],
    'label' => $classification_options[MFCS_REQUEST_CLASSIFICATION_EXTERNAL],
    'color' => '#ab6f06',
  );

  $chart_markup = mfcs_request_statistics_render_chart($options);

  $width = 450;
  $height = 207;
  $svg_options = ' version="1.1" viewBox="0 0 ' . $options['chart_width'] . ' ' . $options['chart_height'] . '" preserveAspectRatio="xMaxYMax meet" xmlns="//www.w3.org/2000/svg"';


  // render the markup
  $markup = '<div class="statistics-block statistics-block-main">';
  $markup .= '<div id="' . 'mfcs-statistics-' . $options['name'] . '-wrapper" class="statistics-block-chart">';
  $markup .= '  <svg id="' . 'mfcs-statistics-' . $options['name'] . '" class="statistics-block-chart-main" title="Requests by Classification" width="' . $width . '" height="' . $height . '"' . $svg_options . '>' . $chart_markup . '</svg>';
  $markup .= '</div>';

  $markup .= '<div class="statistics-block-text">';

  if (isset($counts['main']['total'])) {
    $markup .= '<div class="statistics-block-text-item statistics-block-text-item-created">';
    $markup .= "There have been <em>" . $counts['main']['total'] . "</em> requests created.";
    $markup .= '</div>';
  }

  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['classification']['student'] . '</em> <strong>' . $classification_options[MFCS_REQUEST_CLASSIFICATION_STUDENT] . "</strong> (<em>" . $options['rows']['student']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['classification']['faculty'] . '</em> <strong>' . $classification_options[MFCS_REQUEST_CLASSIFICATION_FACULTY] . "</strong> (<em>" . $options['rows']['faculty']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['classification']['camps'] . '</em> <strong>' . $classification_options[MFCS_REQUEST_CLASSIFICATION_CAMPS] . "</strong> (<em>" . $options['rows']['camps']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['classification']['external'] . '</em> <strong>' . $classification_options[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] . "</strong> (<em>" . $options['rows']['external']['percent'] . '%</em> of requests). ' . '</div>';

  $markup .= '</div>'; // statistics-block-text
  $markup .= '</div>'; // statistics-block-main

  return $markup;
}

/**
 * Builds the of HTML markup containing the by type statistics block.
 *
 * @param array $counts
 *   The collection of all calculated statistics as generated by
 *   mfcs_request_statistics_load_counts().
 * @param array $type_options
 *   The array of request type options as generated by
 *   mfcs_get_request_type_list_options().
 *
 * @return string|bool
 *   The HTML markup string or FALSE on error.
 *
 * @see: mfcs_request_statistics_load_counts()
 * @see: mfcs_get_request_type_list_options()
 */
function mfcs_request_statistics_render_request_type($counts, $type_options) {
  if (!is_array($counts) || empty($counts)) {
    cf_error::invalid_string('counts');
    return FALSE;
  }

  if (!is_array($type_options) || empty($type_options)) {
    cf_error::invalid_string('type_options');
    return FALSE;
  }

  global $base_path;

  $user = cf_current_user();

  // render chart
  $options = array(
    'total' => $counts['main']['total'],
    'rows' => array(),
    'text_title' => "Requests by Type",
    'text_description' => 'A bar chart representing the number of requests for each type they belong to.',
    'text_x' => "Requests",
    'text_y' => "Type",
    'chart_height' => 220,
    'line_start_x' => 90,
    'line_start_y' => 25,
    'name' => 'type',
  );

  $options['rows']['athletic'] = array(
    'total' => $counts['type']['athletic'],
    'text' => $type_options[MFCS_REQUEST_TYPE_ATHLETIC],
    'label' => $type_options[MFCS_REQUEST_TYPE_ATHLETIC],
    'color' => '#f9c7c7',
  );

  $options['rows']['banquet'] = array(
    'total' => $counts['type']['banquet'],
    'text' => $type_options[MFCS_REQUEST_TYPE_BANQUET],
    'label' => $type_options[MFCS_REQUEST_TYPE_BANQUET],
    'color' => '#cdcd81',
  );

  $options['rows']['ceremony'] = array(
    'total' => $counts['type']['ceremony'],
    'text' => $type_options[MFCS_REQUEST_TYPE_CEREMONY],
    'label' => "Ceremony / ",
    'label_2' => "Presentation",
    'color' => '#aedeae',
  );

  $options['rows']['fair'] = array(
    'total' => $counts['type']['fair'],
    'text' => $type_options[MFCS_REQUEST_TYPE_FAIR],
    'label' => $type_options[MFCS_REQUEST_TYPE_FAIR],
    'color' => '#deacde',
  );

  $options['rows']['lecture'] = array(
    'total' => $counts['type']['lecture'],
    'text' => $type_options[MFCS_REQUEST_TYPE_LECTURE],
    'label' => $type_options[MFCS_REQUEST_TYPE_LECTURE],
    'color' => '#ecc3ae',
  );

  $options['rows']['meeting'] = array(
    'total' => $counts['type']['meeting'],
    'text' => $type_options[MFCS_REQUEST_TYPE_MEETING],
    'label' => $type_options[MFCS_REQUEST_TYPE_MEETING],
    'color' => '#add3e3',
  );

  $options['rows']['other'] = array(
    'total' => $counts['type']['other'],
    'text' => $type_options[MFCS_REQUEST_TYPE_OTHER],
    'label' => $type_options[MFCS_REQUEST_TYPE_OTHER],
    'color' => '#9ed0d0',
  );

  $options['rows']['performance'] = array(
    'total' => $counts['type']['performance'],
    'text' => $type_options[MFCS_REQUEST_TYPE_PERFORMANCE],
    'label' => $type_options[MFCS_REQUEST_TYPE_PERFORMANCE],
    'color' => '#c8c8e3',
  );

  $options['rows']['quick_meeting'] = array(
    'total' => $counts['type']['quick_meeting'],
    'text' => $type_options[MFCS_REQUEST_TYPE_QUICK_MEETING],
    'label' => $type_options[MFCS_REQUEST_TYPE_QUICK_MEETING],
    'color' => '#cddeae',
  );

  $options['rows']['reception'] = array(
    'total' => $counts['type']['reception'],
    'text' => $type_options[MFCS_REQUEST_TYPE_RECEPTION],
    'label' => $type_options[MFCS_REQUEST_TYPE_RECEPTION],
    'color' => '#f1b157',
  );

  $options['rows']['talent'] = array(
    'total' => $counts['type']['talent'],
    'text' => "Talent / ",
    'label' => "Variety Show",
    'color' => '#ee8fab',
  );

  $options['rows']['workshop'] = array(
    'total' => $counts['type']['workshop'],
    'text' => $type_options[MFCS_REQUEST_TYPE_WORKSHOP],
    'label' => $type_options[MFCS_REQUEST_TYPE_WORKSHOP],
    'color' => '#a2b9df',
  );

  $chart_markup = mfcs_request_statistics_render_chart($options);

  $width = 450;
  $height = 472;
  $svg_options = ' version="1.1" viewBox="0 0 ' . $options['chart_width'] . ' ' . $options['chart_height'] . '" preserveAspectRatio="xMaxYMax meet" xmlns="//www.w3.org/2000/svg"';

  // render the markup
  $markup = '<div class="statistics-block statistics-block-request_type">';

  $markup .= '<div class="statistics-block-text">';

  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['athletic'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_ATHLETIC] . "</strong> (<em>" . $options['rows']['athletic']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['banquet'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_BANQUET] . "</strong> (<em>" . $options['rows']['banquet']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['ceremony'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_CEREMONY] . "</strong> (<em>" . $options['rows']['ceremony']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['fair'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_FAIR] . "</strong> (<em>" . $options['rows']['fair']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['lecture'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_LECTURE] . "</strong> (<em>" . $options['rows']['lecture']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['meeting'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_MEETING] . "</strong> (<em>" . $options['rows']['meeting']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['other'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_OTHER] . "</strong> (<em>" . $options['rows']['other']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['performance'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_PERFORMANCE] . "</strong> (<em>" . $options['rows']['performance']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['quick_meeting'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_QUICK_MEETING] . "</strong> (<em>" . $options['rows']['quick_meeting']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['reception'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_RECEPTION] . "</strong> (<em>" . $options['rows']['reception']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['talent'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_TALENT] . "</strong> (<em>" . $options['rows']['talent']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['type']['workshop'] . '</em> <strong>' . $type_options[MFCS_REQUEST_TYPE_WORKSHOP] . "</strong> (<em>" . $options['rows']['workshop']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '</div>'; // statistics-block-text

  $markup .= '<div id="' . 'mfcs-statistics-' . $options['name'] . '-wrapper" class="statistics-block-chart">';
  $markup .= '  <svg id="' . 'mfcs-statistics-' . $options['name'] . '" class="statistics-block-chart-request_type" title="Requests by Type" width="' . $width . '" height="' . $height . '"' . $svg_options . '>' . $chart_markup . '</svg>';
  $markup .= '</div>';

  $markup .= '</div>'; // statistics-block

  return $markup;
}

/**
 * Builds the HTML markup containing the by step statistics block.
 *
 * @param array $counts
 *   The collection of all calculated statistics as generated by
 *   mfcs_request_statistics_load_counts().
 * @param array $step_options
 *   The array of request type options as generated by
 *   mfcs_get_request_type_list_options().
 *
 * @return string|bool
 *   The HTML markup string or FALSE on error.
 *
 * @see: mfcs_request_statistics_load_counts()
 * @see: mfcs_get_request_type_list_options()
 */
function mfcs_request_statistics_render_request_step($counts, $step_options) {
  if (!is_array($counts) || empty($counts)) {
    cf_error::invalid_string('counts');
    return FALSE;
  }

  if (!is_array($step_options) || empty($step_options)) {
    cf_error::invalid_string('step_options');
    return FALSE;
  }

  global $base_path;

  $user = cf_current_user();

  // render chart
  $options = array(
    'total' => $counts['main']['total'],
    'rows' => array(),
    'text_title' => "Requests by Step",
    'text_description' => 'A bar chart representing the number of requests for each step they are currently in.',
    'text_x' => "Requests",
    'text_y' => "Step",
    'chart_height' => 220,
    'line_start_x' => 120,
    'line_start_y' => 25,
    'name' => 'step',
  );

  // currently this is not used so do not expose it to the user.
  #$options['rows']['changes_required'] = array(
  #  'total' => $counts['step']['changes_required'],
  #  'text' => $step_options[MFCS_REVIEW_STEP_CHANGES_REQUIRED],
  #  'label' => $step_options[MFCS_REVIEW_STEP_CHANGES_REQUIRED],
  #  'color' => '#4adde9',
  #);

  $options['rows']['completed'] = array(
    'total' => $counts['step']['completed'],
    'text' => $step_options[MFCS_REVIEW_STEP_COMPLETED],
    'label' => $step_options[MFCS_REVIEW_STEP_COMPLETED],
    'color' => '#e94a50',
  );

  #$options['rows']['make_decisions'] = array(
  #  'total' => $counts['step']['make_decisions'],
  #  'text' => $step_options[MFCS_REVIEW_STEP_FINAL_APPROVAL],
  #  'label' => "Resolve Conflics / ",
  #  'label_2' => "Make Decisions",
  #  'color' => '#6Ce94a',
  #);

  // currently this is not used so do not expose it to the user.
  #$options['rows']['release_hold'] = array(
  #  'total' => $counts['step']['release_hold'],
  #  'text' => $step_options[MFCS_REVIEW_STEP_RELEASE_HOLD],
  #  'label' => "Release Tentative",
  #  'label_2' => "Hold",
  #  'color' => '#cae94a',
  #);

  $options['rows']['review'] = array(
    'total' => $counts['step']['review'],
    'text' => $step_options[MFCS_REVIEW_STEP_REVIEW],
    'label' => $step_options[MFCS_REVIEW_STEP_REVIEW],
    'color' => '#4a54e9',
  );

  $options['rows']['requirements'] = array(
    'total' => $counts['step']['requirements'],
    'text' => $step_options[MFCS_REVIEW_STEP_REQUIREMENTS],
    'label' => $step_options[MFCS_REVIEW_STEP_REQUIREMENTS],
    'color' => '#e9b64a',
  );

  $options['rows']['venue_available'] = array(
    'total' => $counts['step']['venue_available'],
    'text' => $step_options[MFCS_REVIEW_STEP_VENUE_AVAILABLE],
    'label' => $step_options[MFCS_REVIEW_STEP_VENUE_AVAILABLE],
    'color' => '#a94ae9',
  );


  $chart_markup = mfcs_request_statistics_render_chart($options);

  $width = 450;
  $height = 218;
  $svg_options = ' version="1.1" viewBox="0 0 ' . $options['chart_width'] . ' ' . $options['chart_height'] . '" preserveAspectRatio="xMaxYMax meet" xmlns="//www.w3.org/2000/svg"';

  // render the markup
  $markup = '<div class="statistics-block statistics-block-request_step">';

  $markup .= '<div id="' . 'mfcs-statistics-' . $options['name'] . '-wrapper" class="statistics-block-chart">';
  $markup .= '  <svg id="' . 'mfcs-statistics-' . $options['name'] . '" class="statistics-block-chart-request_step" title="Requests by Step" width="' . $width . '" height="' . $height . '"' . $svg_options . '>' . $chart_markup . '</svg>';
  $markup .= '</div>'; // statistics-block-chart

  $markup .= '<div class="statistics-block-text">';
  #$markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['changes_required'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_CHANGES_REQUIRED] . "</strong> (<em>" . $options['rows']['changes_required']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['completed'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_COMPLETED] . "</strong> (<em>" . $options['rows']['completed']['percent'] . '%</em> of requests). ' . '</div>';
  #$markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['make_decisions'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_FINAL_APPROVAL] . "</strong> (<em>" . $options['rows']['make_decisions']['percent'] . '%</em> of requests). ' . '</div>';
  #$markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['release_hold'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_RELEASE_HOLD] . "</strong> (<em>" . $options['rows']['release_hold']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['review'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_REVIEW] . "</strong> (<em>" . $options['rows']['review']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['requirements'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_REQUIREMENTS] . "</strong> (<em>" . $options['rows']['requirements']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['step']['venue_available'] . '</em> <strong>' . $step_options[MFCS_REVIEW_STEP_VENUE_AVAILABLE] . "</strong> (<em>" . $options['rows']['venue_available']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '</div>'; // statistics-block-text

  $markup .= '</div>'; // statistics-block

  return $markup;
}

/**
 * Builds the HTML markup containing the by status statistics block.
 *
 * @param array $counts
 *   The collection of all calculated statistics as generated by
 *   mfcs_request_statistics_load_counts().
 * @param array $status_options
 *   The array of request type options as generated by
 *   mfcs_get_request_type_list_options().
 *
 * @return string|bool
 *   The HTML markup string or FALSE on error.
 *
 * @see: mfcs_request_statistics_load_counts()
 * @see: mfcs_get_request_type_list_options()
 */
function mfcs_request_statistics_render_request_status($counts, $status_options) {
  if (!is_array($counts) || empty($counts)) {
    cf_error::invalid_string('counts');
    return FALSE;
  }

  if (!is_array($status_options) || empty($status_options)) {
    cf_error::invalid_string('status_options');
    return FALSE;
  }

  global $base_path;

  $user = cf_current_user();

  // render chart
  $options = array(
    'total' => $counts['main']['total'],
    'rows' => array(),
    'text_title' => "Requests by Status",
    'text_description' => 'A bar chart representing the number of requests based on their current status.',
    'text_x' => "Requests",
    'text_y' => "Status",
    'chart_height' => 220,
    'line_start_x' => 120,
    'line_start_y' => 25,
    'name' => 'status',
  );

  $options['rows']['locked'] = array(
    'total' => $counts['status']['locked'],
    'text' => $status_options[MFCS_REQUEST_STATUS_LOCKED],
    'label' => $status_options[MFCS_REQUEST_STATUS_LOCKED],
    'color' => '#c4080e',
  );

  $options['rows']['unlocked'] = array(
    'total' => $counts['status']['unlocked'],
    'text' => $status_options[MFCS_REQUEST_STATUS_UNLOCKED],
    'label' => $status_options[MFCS_REQUEST_STATUS_UNLOCKED],
    'color' => '#08c4bf',
  );

  $options['rows']['accepted'] = array(
    'total' => $counts['status']['accepted'],
    'text' => $status_options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED],
    'label' => $status_options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED],
    'color' => '#39c408',
  );

  $options['rows']['denied'] = array(
    'total' => $counts['status']['denied'],
    'text' => $status_options[MFCS_REQUEST_STATUS_CLOSED_DENIED],
    'label' => $status_options[MFCS_REQUEST_STATUS_CLOSED_DENIED],
    'color' => '#9208c4',
  );

  $options['rows']['unavailable'] = array(
    'total' => $counts['status']['unavailable'],
    'text' => $status_options[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE],
    'label' => $status_options[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE],
    'color' => '#b6c408',
  );

  $options['rows']['due_to_lock'] = array(
    'total' => $counts['status']['due_to_lock'],
    'text' => $status_options[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK],
    'label' => $status_options[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE],
    'label_2' => "(due to lock)",
    'color' => '#081fc4',
  );

  $options['rows']['cancelled'] = array(
    'total' => $counts['status']['cancelled'],
    'text' => $status_options[MFCS_REQUEST_STATUS_CANCELLED],
    'label' => $status_options[MFCS_REQUEST_STATUS_CANCELLED],
    'color' => '#c47508',
  );

  $options['rows']['accepted_cancelled'] = array(
    'total' => $counts['status']['accepted_cancelled'],
    'text' => $status_options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED],
    'label' => $status_options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED],
    'color' => '#a6c408',
  );


  $chart_markup = mfcs_request_statistics_render_chart($options);

  $width = 450;
  $height = 282;
  $svg_options = ' version="1.1" viewBox="0 0 ' . $options['chart_width'] . ' ' . $options['chart_height'] . '" preserveAspectRatio="xMaxYMax meet" xmlns="//www.w3.org/2000/svg"';

  // render the markup
  $markup = '<div class="statistics-block statistics-block-request_step">';

  $markup .= '<div class="statistics-block-text">';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['locked'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_LOCKED] . "</strong> (<em>" . $options['rows']['locked']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['unlocked'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_UNLOCKED] . "</strong> (<em>" . $options['rows']['unlocked']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['accepted'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] . "</strong> (<em>" . $options['rows']['accepted']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['denied'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_CLOSED_DENIED] . "</strong> (<em>" . $options['rows']['denied']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['unavailable'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] . "</strong> (<em>" . $options['rows']['unavailable']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['due_to_lock'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] . "</strong> (<em>" . $options['rows']['due_to_lock']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['cancelled'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_CANCELLED] . "</strong> (<em>" . $options['rows']['cancelled']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '<div class="statistics-block-text-item"><em>' . $counts['status']['accepted_cancelled'] . '</em> <strong>' . $status_options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] . "</strong> (<em>" . $options['rows']['accepted_cancelled']['percent'] . '%</em> of requests). ' . '</div>';
  $markup .= '</div>'; // statistics-block-text

  $markup .= '<div id="' . 'mfcs-statistics-' . $options['name'] . '-wrapper" class="statistics-block-chart">';
  $markup .= '  <svg id="' . 'mfcs-statistics-' . $options['name'] . '" class="statistics-block-chart-request_status" title="Requests by Status" width="' . $width . '" height="' . $height . '"' . $svg_options . '>' . $chart_markup . '</svg>';
  $markup .= '</div>'; // statistics-block-chart

  $markup .= '</div>'; // statistics-block

  return $markup;
}

/**
 * Builds a string of HTML markup containing an inline SVG chart.
 *
 * @param array $options
 *   An array of properties associated with the chart.
 *   This is required to properly build the chart.
 *
 * @return string|bool
 *   The HTML markup string or FALSE on error.
 */
function mfcs_request_statistics_render_chart(&$options) {
  if (!is_array($options) || empty($options)) {
    cf_error::invalid_array('options');
    return FALSE;
  }

  // setup and calculations
  $highest = 0;
  $total_rows = 0;
  $invalid = array();
  foreach ($options['rows'] as $key => $row) {
    if (!isset($row['total']) || !isset($row['text']) || !isset($row['label']) || !isset($row['color'])) {
      $invalid[$key] = $key;
      continue;
    }

    if ($row['total'] > $highest) {
      $highest = $row['total'];
    }
    elseif ($options['total'] == 0) {
      $options['rows'][$key]['percent'] = 0;
      $total_rows++;
      continue;
    }

    $percent = ($row['total'] / $options['total']) * 100;
    if (floor($percent) == $percent) {
      $options['rows'][$key]['percent'] = sprintf('%01.0f', $percent);
    }
    else {
      $options['rows'][$key]['percent'] = sprintf('%01.3f', $percent);
    }

    $total_rows++;
  }

  foreach ($invalid as $key) {
    unset($options['rows'][$key]);
  }

  $increment_value = 1;
  $increment_width = 15;
  if ($highest < 5) {
    $increment_width = 56;
    $increment_split = 5;
  }
  elseif ($highest < 100) {
    $increment_width = 31;
    $increment_split = 9;
  }
  elseif ($highest < 1000) {
    $increment_width = 35;
    $increment_split = 8;
  }
  else {
    $increment_width = 40;
    $increment_split = 7;
  }

  if ($highest > 0) {
    $increment_value = ceil($highest / $increment_split);
  }

  $row_increment_y = 25;

  $row_padding_x = 0;
  $row_padding_y = 7;

  $options['chart_width'] = $options['line_start_x'] + (($increment_width + $row_padding_x) * ($increment_split + 1));
  $options['chart_height'] = $total_rows * ($row_increment_y + $row_padding_y) + 60;

  // add additional pixels to the width to reduce occurances of truncated numbers.
  $unpadded_width = $options['chart_width'];
  $options['chart_width'] += 12;

  $text_x_x_offset = ($unpadded_width / 2);
  $text_x_y_offset = $options['line_start_y'] + (($row_increment_y + $row_padding_y) * $total_rows) + 30;


  // build markup
  $chart_markup = '';

  $chart_markup .= '<title>' . $options['text_title'] . '</title>';
  $chart_markup .= '<desc>' . $options['text_description'] . '</desc>';
  $chart_markup .= '<text font-size="16" x="' . $options['line_start_x'] . '" y="15" style="font-weight: bold;">' . $options['text_title'] . '</text>';
  $chart_markup .= '<text x="' . $text_x_x_offset . '" y="' . $text_x_y_offset . '">' . $options['text_x'] . '</text>';
  #$chart_markup .= '<text x="0" y="0" transform="rotate(270,70,60)">' . $options['text_y'] . '</text>';

  $x1 = $options['line_start_x'];
  $x2 = $options['line_start_x'];
  $y1 = $options['line_start_y'];
  $y2 = $options['line_start_y'] + (($row_increment_y + $row_padding_y) * $total_rows);
  $chart_markup .= '<g><line x1="' . $x1 . '" y1="' . $y1 . '" x2="' . $x2 . '" y2="' . $y2 . '" style="stroke: #000000; stroke-width: 1;" /></g>';

  $y1 = $y2;
  $x2 = $options['chart_width'];
  $chart_markup .= '<g><line x1="' . $x1 . '" y1="' . $y2 . '" x2="' . $x2 . '" y2="' . $y2 . '" style="stroke: #000000; stroke-width: 1;" /></g>';

  $x1 = $options['line_start_x'] + $increment_width;
  $x2 = $x1;
  $y1 = $options['line_start_y'];
  $y2 = $options['line_start_y'] + (($row_increment_y + $row_padding_y) * $total_rows);
  for ($i = 1; $i <= $increment_split; $i++) {
    $chart_markup .= '<g>';
    $chart_markup .= '  <line x1="' . $x1 . '" y1="' . $y1 . '" x2="' . $x2 . '" y2="' . $y2 . '" style="stroke: #505050; stroke-width: 1; stroke-dasharray: 1,4;" />';
    $chart_markup .= '  <text font-size="11" x="' . ($x1 - 4) . '" y="' . ($y2 + 15) . '">' . ($i * $increment_value) . '</text>';
    $chart_markup .= '</g>';

    $x1 += $increment_width;
    $x2 = $x1;
  }


  // add rows
  $x1 = $options['line_start_x'];
  $y1 = $options['line_start_y'] + $row_padding_y;
  $y2 = $y1 + 10;
  $bar_height = 16;

  $row_name = '';
  if (!empty($options['name'])) {
    $row_name .= '-' . $options['name'];
  }

  foreach ($options['rows'] as $row_number => $row) {
    $bar_width = ($row['total'] * $increment_width) / $increment_value;

    $label_font_size = 12;
    $y2_offset = $y2;
    if (!empty($row['label_3'])) {
      $label_font_size = 11;
      $y2_offset -= 4;
    }

    $chart_markup .= '<g id="mfcs-statistics' . $row_name . '-bar-' . $row_number . '" class="row-bar">';
    $chart_markup .= '  <title>' . $row['text'] . ': ' . $row['total'] . ' (' . $row['percent'] . '%)' . '</title>';
    $chart_markup .= '  <text font-size="' . $label_font_size . '" x="0" y="' . $y2_offset . '" style="line-height: 12px;">' . $row['label'] . '</text>';

    if (!empty($row['label_2'])) {
      $chart_markup .= '  <text font-size="' . $label_font_size . '" x="0" y="' . ($y2_offset + 12) . '" style="line-height: 12px;">' . $row['label_2'] . '</text>';
    }

    if (!empty($row['label_3'])) {
      $chart_markup .= '  <text font-size="' . $label_font_size . '" x="0" y="' . ($y2_offset + 24) . '" style="line-height: 12px;">' . $row['label_3'] . '</text>';
    }

    $chart_markup .= '  <text font-size="14" x="' . ($x1 + $bar_width + 10) . '" y="' . ($y1 + 15) . '">' . $row['total'] . '</text>';
    $chart_markup .= '  <rect width="' . $bar_width . '" height="' . $bar_height . '" x="' . $x1 . '" y="' . $y1 . '" style="fill: ' . $row['color'] . '; stroke-width: 1; stroke: #000000;">';
    $chart_markup .= '</g>';

    $y1 = $y1 + $row_increment_y + $row_padding_y;
    $y2 = $y1 + 10;
  }

  return $chart_markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
