<?php

/**
 * @file
 * Defines McNeese facilities use request ical page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request ical month page.
 *
 * @param int $year
 *   (optional) When specified, represents the year.
 * @param string $month
 *   (optional) When specified, represents the month.
 *   Causes $year to be required.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_ical_month_0_page($year = NULL, $month = NULL) {
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

  if (is_null($year) && !is_null($month)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_url;

  mfcs_include(4);
  mfcs_include(9);

  $parameter_year = FALSE;
  $parameter_month = !is_null($month);

  if (is_null($year)) {
    $month_start = strtotime('midnight first day of', REQUEST_TIME);
    $month_end = strtotime('midnight last day of', REQUEST_TIME);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }
  else {
    $parameter_year = TRUE;
    $current_date = mfcs_miscellaneous_determine_current_date($year, $month, NULL);

    $month_start = strtotime('midnight first day of ' . $current_date['month'] . ' ' . $current_date['year']);
    $month_end = strtotime('midnight last day of ' . $current_date['month'] . ' ' . $current_date['year']);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }

  $month = strtolower(date('F', $month_start));
  $year = date('Y', $month_start);
  mfcs_add_canonical_headers('requests/ical-0/month/' . $year . '/' . $month);

  // determine the begin and end weeks for the month.
  $absolute_start = strtotime('midnight last sunday', $month_start);
  $absolute_stop = strtotime('midnight next saturday', $month_end);
  $absolute_stop = strtotime('midnight tomorrow', $absolute_stop);

  if (date('w', $month_start) == 0) {
    $absolute_start = $month_start;
  }

  if (date('w', $month_end) == 6) {
    $absolute_stop = $month_stop;
  }

  // ISO-8601 dates start with monday = 1 and ends with sunday = 7.
  #$absolute_start = strtotime('midnight last monday', $month_start);
  #$absolute_stop = strtotime('midnight next sunday', $month_stop);
  #$absolute_stop = strtotime('midnight tomorrow', $absolute_stop);
  #
  #if (date('w', $month_start) == 1) {
  #  $absolute_start = $month_start;
  #}
  #
  #if (date('w', $month_end) == 7) {
  #  $absolute_stop = $month_stop;
  #}

  $search = mfcs_miscellaneous_generate_requests_by_date_search($absolute_start, $absolute_stop);

  $sorting = array(
    'order' => 'date',
    'sort' => 'ASC',
  );

  $extra_fields = mfcs_request_ical_generate_extra_fields();

  $results = mfcs_request_load_listing($search, FALSE, 0, 0, $sorting, NULL, $extra_fields);

  $ical = FALSE;
  if ($results !== FALSE) {
    mfcs_include(8);

    $ical = mfcs_generate_ical($results, $absolute_start, $absolute_stop);
  }

  if ($ical === FALSE) {
    return '';
  }

  $GLOBALS['devel_shutdown'] = FALSE;

  // make sure to perform output buffer (if possible) to reduce chances of "headers already sent" issues.
  $ob_level = ob_get_level();
  for ($i = 0; $i < $ob_level; $i++) {
    ob_clean();
  }
  ob_start();

  $filename = 'mcneese-facilities_use.ics';
  $content_location = $base_url . '/requests/ical-0/day';

  if ($parameter_year) {
    $filename = 'mcneese-facilities_use-' . $year;
    $content_location .= '/' . $year;

    if ($parameter_month) {
      $filename .= '-' . $month;
      $content_location .= '/' . $month;
    }

    $filename .= '.ics';
  }

  header('HTTP/1.1 200 OK', TRUE, 200);
  header('Content-Type: text/plain');
  header('Content-Length: ' . strlen($ical));
  header('Content-Transfer-Encoding: binary');
  #header('Content-type: text/plain; charset=utf-8');
  header('Content-type: text/calendar; charset=utf-8');
  header('Content-Disposition: inline; filename="' . $filename . '"');
  header('Content-Description: File Transfer');
  header('Content-Location: ' . $content_location);
  header('Last-Modified: ' . date('r', REQUEST_TIME));
  header('Date: ' . date('r', REQUEST_TIME));

  // flush buffer after headers have been processed (to hopefully avoid "headers already sent" issues).
  ob_end_flush();

  print($ical);
  exit();
}

/**
 * Provides the facilities use request ical month page.
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
function mfcs_request_ical_day_0_page($year = NULL, $month = NULL, $day = NULL) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year) && !is_null($month) || is_null($year) && is_null($month) && !is_null($day)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_url;

  mfcs_include(4);

  $parameter_year = FALSE;
  $parameter_month = !is_null($month);
  $parameter_day = !is_null($day);

  if (is_null($year)) {
    $day_start = strtotime('midnight today', REQUEST_TIME);
    $day_stop = strtotime('midnight tomorrow', $day_start);
  }
  else {
    $parameter_year = TRUE;

    mfcs_include(9);
    $current_date = mfcs_miscellaneous_determine_current_date($year, $month, $day);

    $day_start = strtotime('midnight ' . $current_date['month'] . ' ' . $current_date['day'] . ' ' . $current_date['year']);
    $day_stop = strtotime('midnight tomorrow', $day_start);
  }

  $month = strtolower(date('F', $day_start));
  $year = date('Y', $day_start);
  $day = date('d', $day_start);
  mfcs_add_canonical_headers('requests/ical-0/day/' . $year . '/' . $month . '/' . $day);

  $search = mfcs_miscellaneous_generate_requests_by_date_search($day_start, $day_stop);

  $sorting = array(
    'order' => 'date',
    'sort' => 'ASC',
  );

  $extra_fields = mfcs_request_ical_generate_extra_fields();

  $results = mfcs_request_load_listing($search, FALSE, 0, 0, $sorting, NULL, $extra_fields);

  $ical = FALSE;
  if ($results !== FALSE) {
    mfcs_include(8);

    $ical = mfcs_generate_ical($results, $day_start, $day_stop);
  }

  if ($ical === FALSE) {
    return '';
  }

  $GLOBALS['devel_shutdown'] = FALSE;

  // make sure to perform output buffer (if possible) to reduce chances of "headers already sent" issues.
  $ob_level = ob_get_level();
  for ($i = 0; $i < $ob_level; $i++) {
    ob_clean();
  }
  ob_start();

  $filename = 'mcneese-facilities_use.ics';
  $content_location = $base_url . '/requests/ical-0/day';

  if ($parameter_year) {
    $filename = 'mcneese-facilities_use-' . $year;
    $content_location .= '/' . $year;

    if ($parameter_month) {
      $filename .= '-' . $month;
      $content_location .= '/' . $month;
    }

    if ($parameter_day) {
      $filename .= '-' . $day;
      $content_location .= '/' . $day;
    }

    $filename .= '.ics';
  }

  header('HTTP/1.1 200 OK', TRUE, 200);
  header('Content-Type: text/plain');
  header('Content-Length: ' . strlen($ical));
  header('Content-Transfer-Encoding: binary');
  #header('Content-type: text/plain; charset=utf-8');
  header('Content-type: text/calendar; charset=utf-8');
  header('Content-Disposition: inline; filename="' . $filename . '"');
  header('Content-Description: File Transfer');
  header('Content-Location: ' . $content_location);
  header('Last-Modified: ' . date('r', REQUEST_TIME));
  header('Date: ' . date('r', REQUEST_TIME));

  // flush buffer after headers have been processed (to hopefully avoid "headers already sent" issues).
  ob_end_flush();

  print($ical);
  exit();
}

/**
 * Generates an array containing extra fields used by ical.
 *
 * @return array
 *   extra fields array as used by mfcs_request_load_listing().
 *
 *  @see: mfcs_request_load_listing()
 */
function mfcs_request_ical_generate_extra_fields() {
  $extra_fields = array();
  $extra_fields['mfcs_current_request_coordinator'] = array(
    'alias' => 'mcrc',
    'join' => 'inner',
    'on' => 'mer.id = mcrc.request_id',
    'columns' => array(),
  );

  $extra_fields['mfcs_field_request_coordinator_name'] = array(
    'alias' => 'mfrcn',
    'join' => 'inner',
    'on' => 'mcrc.request_id = mfrcn.request_id AND mcrc.name = mfrcn.revision AND mfrcn.delta = 0',
    'columns' => array(
      'request_coordinator_name' => 'value',
    ),
  );

  $extra_fields['mfcs_field_request_coordinator_email'] = array(
    'alias' => 'mfrce',
    'join' => 'inner',
    'on' => 'mcrc.request_id = mfrce.request_id AND mcrc.name = mfrce.revision AND mfrce.delta = 0',
    'columns' => array(
      'request_coordinator_email' => 'value',
    ),
  );

  return $extra_fields;
}

/**
 * @} End of '@addtogroup mfcs'.
 */