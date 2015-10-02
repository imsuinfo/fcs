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

  $instance = mfcs_instance();


  // process month & day
  if (is_null($month)) {
    $month_stamp = strtotime('midnight first day of this month', $instance);
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
 *   (optional) The unix timestamp the date starts on.
 *   A >= operator is used on this.
 *   If NULL, then the start is not included in the search.
 *   One of $start or $stop is required.
 * @param int $stop
 *   (optional) The unix timestamp the date stops on.
 *   A < operator is used on this.
 *   If NULL, then the start is not included in the search.
 *   One of $start or $stop is required.
 * @param int|null $status
 *   (optional) The request status id to search for.
 *   If NULL, then the status is not included in the search.
 * @param int|null $step
 *   (optional) The review step id to search for.
 *   If NULL, then the step is not included in the search.
 *
 * @return array|bool
 *   An array containing the generated search values.
 *
 * @see: mfcs_request_load_listing()
 */
function mfcs_miscellaneous_generate_requests_by_date_search($start = NULL, $stop = NULL, $status = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED, $step = MFCS_REVIEW_STEP_COMPLETED) {
  if (!is_null($start) && !cf_is_integer($start)) {
    cf_error::invalid_integer('start');
    return FALSE;
  }

  if (!is_null($stop) && !cf_is_integer($stop)) {
    cf_error::invalid_integer('stop');
    return FALSE;
  }

  if (is_null($start) && is_null($stop)) {
    cf_error::invalid_integer('start');
    return FALSE;
  }

  if (!cf_is_integer($status)) {
    cf_error::invalid_integer('status');
    return FALSE;
  }

  $search = array();

  if (!is_null($start)) {
    $target_field_name = 'field_dates-date-start-0';
    $search[$target_field_name]['group_name'] = 'dates';
    $search[$target_field_name]['field_name'] = 'date';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $start;
    $search[$target_field_name]['type'] = 'date';
    $search[$target_field_name]['multiple'] = TRUE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_GREATER_THAN_EQUAL;
  }

  if (!is_null($stop)) {
    $target_field_name = 'field_dates-date-stop-0';
    $search[$target_field_name]['group_name'] = 'dates';
    $search[$target_field_name]['field_name'] = 'date';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $stop;
    $search[$target_field_name]['type'] = 'date';
    $search[$target_field_name]['multiple'] = TRUE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_LESS_THAN;
  }

  if (!is_null($status)) {
    $target_field_name = 'field_top-status-0';
    $search[$target_field_name]['group_name'] = 'mer';
    $search[$target_field_name]['field_name'] = 'status';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $status;
    $search[$target_field_name]['type'] = 'options';
    $search[$target_field_name]['multiple'] = FALSE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_EQUAL;
  }

  if (!is_null($step)) {
    $target_field_name = 'field_top-step-0';
    $search[$target_field_name]['group_name'] = 'mer';
    $search[$target_field_name]['field_name'] = 'step';
    $search[$target_field_name]['column'] = 'value';
    $search[$target_field_name]['search'] = $step;
    $search[$target_field_name]['type'] = 'options';
    $search[$target_field_name]['multiple'] = FALSE;
    $search[$target_field_name]['operator'] = MFCS_OPERATOR_EQUAL;
  }

  return $search;
}

/**
 * Load all requests that have dates within a certain range of the timestamps.
 *
 * This function will not search for conflicting dates. It only searches for
 * times that begin or end near the specified timestamps.
 *
 * @param int $timestamp_start
 *   The timestamp in which to start at.
 * @param int $timestamp_stop
 *   The timestamp in which to end at.
 * @param int $room
 *   The unique id for the room being selected.
 * @param int|array $status
 *   (optional) The status code to limit search results by. Can be an array of
 *   status codes.
 * @param int $nearby_range_seconds
 *   (optional) The range (in seconds) of requests that are within the
 *   specified range.
 *
 * @return array|false
 *   An array containing:
 *   - 'before': Requests that appear immediately before specified time.
 *   - 'after': Requests that appear immediately after specified time.
 *   False is returned on error.
 */
function mfcs_miscellaneous_nearby_requests($timestamp_start, $timestamp_stop, $room, $status = MFCS_REQUEST_STATUS_CLOSED_ACCEPTED, $nearby_range_seconds = MFCS_NEARBY_REQUESTS_RANGE_SECONDS) {
  if (!cf_is_integer($timestamp_start)) {
    cf_error::invalid_integer('timestamp_start');
    return FALSE;
  }

  if (!cf_is_integer($timestamp_stop)) {
    cf_error::invalid_integer('timestamp_stop');
    return FALSE;
  }

  if (!cf_is_integer($room)) {
    cf_error::invalid_integer('room');
    return FALSE;
  }

  if (!cf_is_integer($status) && !is_array($status)) {
    cf_error::invalid_integer('status');
    return FALSE;
  }

  if (!cf_is_integer($nearby_range_seconds)) {
    cf_error::invalid_integer('nearby_range_seconds');
    return FALSE;
  }

  $nearby_events = array(
    'before' => array(),
    'after' => array(),
  );

  try {
    // the start range must look at stop values to find nearby ranges.
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_dates_time_stop', 'mfdtsp', 'mc.request_id = mfdtsp.request_id AND mc.dates_time_start = mfdtsp.revision');

    $range_start = strtotime('-' . $nearby_range_seconds . ' seconds', $timestamp_start);
    $range_stop = $timestamp_start;

    $query->condition('mfdtsp.value', $range_start, '>=');
    $query->condition('mfdtsp.value', $range_stop, '<=');
    $query->condition('mer.room', $room);

    if (is_array($status)) {
      $query->condition('mer.status', $status, 'IN');
    }
    else {
      $query->condition('mer.status', $status);
    }

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');

    $results = $query->execute()->fetchAllKeyed();
    if (is_array($results)) {
      $nearby_events['before'] = $results;
    }
    else {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while searching for nearby requests in room @room with status @status and range of @range seconds before @time.", array('@time' => date('Y/m/d h:i:s a', $timestamp_start), '@room' => $room, '@status' => $status, '@range' => $nearby_range_seconds), WATCHDOG_WARNING);
    }

    // the stop range must look at start values to find nearby ranges.
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_dates_time_start', 'mfdts', 'mc.request_id = mfdts.request_id AND mc.dates_time_start = mfdts.revision');

    $range_start = $timestamp_stop;
    $range_stop = strtotime('+' . $nearby_range_seconds . ' seconds', $timestamp_stop);

    $query->condition('mfdts.value', $range_start, '>=');
    $query->condition('mfdts.value', $range_stop, '<=');
    $query->condition('mer.room', $room);

    if (is_array($status)) {
      $query->condition('mer.status', $status, 'IN');
    }
    else {
      $query->condition('mer.status', $status);
    }

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');

    $results = $query->execute()->fetchAllKeyed();
    if (is_array($results)) {
      $nearby_events['after'] = $results;
    }
    else {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while searching for nearby requests in room @room with status @status and range of @range seconds after @time.", array('@time' => date('Y/m/d h:i:s a', $timestamp_stop), '@room' => $room, '@status' => $status, '@range' => $nearby_range_seconds), WATCHDOG_WARNING);
    }
  } catch (Exception $ex) {
    cf_error::on_exception($ex);
    watchdog(MFCS_WATCHDOG_ID, "While searching for nearby requests in room @room with status @status and range of @range seconds before @before or after @after, the following error eoccured: @error.", array('@before' => date('Y/m/d h:i:s a', $timestamp_start), '@after' => date('Y/m/d h:i:s a', $timestamp_stop), '@room' => $room, '@status' => $status, '@range' => $nearby_range_seconds, '@error' => $ex->getMessage()), WATCHDOG_ERROR);

    return FALSE;
  }

  return $nearby_events;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
