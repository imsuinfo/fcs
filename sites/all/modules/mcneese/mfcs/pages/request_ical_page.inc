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

  global $base_path;
  global $base_url;

  $user = cf_current_user();

  mfcs_include(4);

  $parameter_year = FALSE;
  $parameter_month = FALSE;

  if (is_null($year) && is_null($month)) {
    $month_start = strtotime('midnight first day of', REQUEST_TIME);
    $month_end = strtotime('midnight last day of', REQUEST_TIME);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }
  else {
    if (is_null($month)) {
      $month = date('F', REQUEST_TIME);
    }
    else {
      $parameter_month = TRUE;
      $month_stamp = strtotime($month);
      $month_name = date('F', $month_stamp);

      if ($month_name === FALSE) {
        cf_error::invalid_string('month');

        drupal_not_found();
        drupal_exit();
      }

      $month_left = strtolower($month);
      $month_right = strtolower($month_name);

      if ($month_left != $month_right) {
        cf_error::invalid_string('month');

        drupal_not_found();
        drupal_exit();
      }

      $month = $month_name;
    }

    if (is_null($year)) {
      $year = date('Y', REQUEST_TIME);
    }
    else {
      $parameter_year = TRUE;
      $year_stamp = strtotime($month . ' ' . $year);
      $year_name = date('Y', $year_stamp);

      if ($year_name === FALSE) {
        cf_error::invalid_string('year');

        drupal_not_found();
        drupal_exit();
      }

      if ($year != $year_name) {
        cf_error::invalid_string('year');

        drupal_not_found();
        drupal_exit();
      }

      $year = $year_name;
    }

    $month_start = strtotime('midnight first day of ' . $month . ' ' . $year);
    $month_end = strtotime('midnight last day of ' . $month . ' ' . $year);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }

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

  $target_field_name = 'field_dates-date-start-0';
  $search[$target_field_name]['group_name'] = 'dates';
  $search[$target_field_name]['field_name'] = 'date';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = $absolute_start;
  $search[$target_field_name]['type'] = 'date';
  $search[$target_field_name]['multiple'] = TRUE;
  $search[$target_field_name]['operator'] = MFCS_OPERATOR_GREATER_THAN_EQUAL;

  $target_field_name = 'field_dates-date-stop-0';
  $search[$target_field_name]['group_name'] = 'dates';
  $search[$target_field_name]['field_name'] = 'date';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = $absolute_stop;
  $search[$target_field_name]['type'] = 'date';
  $search[$target_field_name]['multiple'] = TRUE;
  $search[$target_field_name]['operator'] = MFCS_OPERATOR_LESS_THAN;

  $target_field_name = 'field_top-status-0';
  $search[$target_field_name]['group_name'] = 'top';
  $search[$target_field_name]['field_name'] = 'status';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = MFCS_EVENT_STATUS_CLOSED_ACCEPTED;
  $search[$target_field_name]['type'] = 'options';
  $search[$target_field_name]['multiple'] = FALSE;
  $search[$target_field_name]['operator'] = MFCS_OPERATOR_EQUAL;

  $sorting = array(
    'order' => 'date',
    'sort' => 'ASC',
  );

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

  $results = mfcs_request_load_listing($search, FALSE, 0, 0, $sorting, NULL, $extra_fields);

  $ical = FALSE;
  if ($results !== FALSE) {
    mfcs_include(8);

    $ical = mfcs_generate_ical($results);
  }

  if ($ical !== FALSE) {
    $GLOBALS['devel_shutdown'] = FALSE;

    // make sure to perform output buffer (if possible) to reduce chances of "headers already sent" issues.
    $ob_level = ob_get_level();
    for ($i = 0; $i < $ob_level; $i++) {
      ob_clean();
    }
    ob_start();

    $filename = 'mcneese-facilities_use.ics';
    $content_location = $base_url . '/requests/ical-0/month';

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

  return '';
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

  if (!is_null($month) && !cf_is_string($month)) {
    cf_error::invalid_string('month');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year) && !is_null($month)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_path;

  $user = cf_current_user();

  mfcs_include(4);

  // @todo: finish this.
  return '';
}

/**
 * @} End of '@addtogroup mfcs'.
 */