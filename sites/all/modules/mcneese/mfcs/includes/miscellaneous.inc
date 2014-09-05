<?php

/**
 * @file
 * Defines McNeese facilities use miscellaneous functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Determine the year/month/day according to the parameters.
 *
 * @param int $year
 *   The year. Should be 4-digits to avoid php date() and strtotime()
 *   function issues.
 * @param string $month
 *   The month name, such as January. Use name instead of digits to avoid
 *   php date() and strtotime() function issues.
 * @param int $day
 *   The day. Use 2-digits to avoid php date() and strtotime() function
 *   issues.
 *
 * @return array|bool
 *   An array containing the Year/Month/Day values or FALSE on error.
 *   The array will include timestamps and other information.
 */
function mfcs_miscellaneous_determine_current_date($year, $month, $day) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    return FALSE;
  }

  if (!is_null($month) && !is_string($month)) {
    cf_error::invalid_string('month');

    return FALSE;
  }

  if (!is_null($day) && !cf_is_integer($day)) {
    cf_error::invalid_integer('day');

    return FALSE;
  }

  $current_date = array();


  // process month & day
  if (is_null($month)) {
    $month_stamp = strtotime('midnight first day of this month', REQUEST_TIME);
    $current_date['day'] = date('d', $month_stamp);
    $current_date['day_stamp'] = $month_stamp;
  }
  else {
    if (is_null($day)) {
      $day_stamp = strtotime('midnight first day of ' . $month . ' ' . $year);
    }
    else {
      $day_stamp = strtotime($month . ' ' . $day);
    }

    $current_date['day'] = date('d', $day_stamp);
    $current_date['day_stamp'] = $day_stamp;

    $month_stamp = strtotime($month . ' ' . $current_date['day']);
  }

  $current_date['month'] = date('F', $month_stamp);
  $current_date['month_stamp'] = $month_stamp;


  // process year
  if ($year < 1000) {
    if ($year < 100) {
      if ($year < 10) {
        if ($year < 0) {
          $year = '0000';
        }
        else {
          $year = '000' . $year;
        }
      }
      else {
        $year = '00' . $year;
      }
    }
    else {
      $year = '0' . $year;
    }
  }

  $year_stamp = strtotime($current_date['month'] . ' ' . $year);

  $current_date['year'] = date('Y', $year_stamp);
  $current_date['year_stamp'] = $year_stamp;


  return $current_date;
}

/**
 * Generate a search by date and status array.
 *
 * This is intendend to be used with mfcs_request_load_listing() for loading
 * a listing of requests.
 *
 * @param int $start
 *   The unix timestamp the date starts on.
 *   A >= operator is used on this.
 * @param int $stop
 *   The unix timestamp the date stops on.
 *   A < operator is used on this.
 * @param int|null $status
 *   The request status id to search for.
 *   If NULL, then the status is not included in the search.
 *
 * @return array|bool
 *   An array containing the generated search values.
 *
 * @see: mfcs_request_load_listing()
 */
function mfcs_miscellaneous_generate_requests_by_date_search($start, $stop, $status = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED) {
  if (!cf_is_integer($start)) {
    cf_error::invalid_integer('start');
    return FALSE;
  }

  if (!cf_is_integer($stop)) {
    cf_error::invalid_integer('stop');
    return FALSE;
  }

  if (!cf_is_integer($status)) {
    cf_error::invalid_integer('status');
    return FALSE;
  }

  $search = array();

  $target_field_name = 'field_dates-date-start-0';
  $search[$target_field_name]['group_name'] = 'dates';
  $search[$target_field_name]['field_name'] = 'date';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = $start;
  $search[$target_field_name]['type'] = 'date';
  $search[$target_field_name]['multiple'] = TRUE;
  $search[$target_field_name]['operator'] = MFCS_OPERATOR_GREATER_THAN_EQUAL;

  $target_field_name = 'field_dates-date-stop-0';
  $search[$target_field_name]['group_name'] = 'dates';
  $search[$target_field_name]['field_name'] = 'date';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = $stop;
  $search[$target_field_name]['type'] = 'date';
  $search[$target_field_name]['multiple'] = TRUE;
  $search[$target_field_name]['operator'] = MFCS_OPERATOR_LESS_THAN;

  if (!is_null($status)) {
    $target_field_name = 'field_top-status-0';
    $search[$target_field_name]['group_name'] = 'top';
    $search[$target_field_name]['field_name'] = 'status';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $status;
    $search[$target_field_name]['type'] = 'options';
    $search[$target_field_name]['multiple'] = FALSE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_EQUAL;
  }

  return $search;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
