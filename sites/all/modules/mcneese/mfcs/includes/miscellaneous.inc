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

  if (!is_null($status) && !cf_is_integer($status)) {
    cf_error::invalid_integer('status');
    return FALSE;
  }

  if (!is_null($step) && !cf_is_integer($step)) {
    cf_error::invalid_integer('step');
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
 * Check requests for conflicts.
 *
 * Requests are considered in conflict if they have overlapping dates and times for the same room (in the same building and therefore location).
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_CONFLICT.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests the requests.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_conflict($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  // this is complex, so only check upcoming requests to reduce the resources used during this task.
  $instance = mfcs_instance();

  try {
    // drupal api does not support with statements, so use a manually created query string for this test.
    $sql_with = 'with requests as (';
    $sql_with .= 'select mer.id, mer.title, mer.location, mer.building, mer.room, mfdd.delta as delta, mfdd.value as date, mfdts.value as time_start, mfdtsp.value as time_stop';
    $sql_with .= ', mbl.machine_name as location_machine_name, mbl.human_name as location_human_name, mbl.date as location_date, mbl.disabled as location_disabled';
    $sql_with .= ', mbb.machine_name as building_machine_name, mbb.human_name as building_human_name, mbb.date as building_date, mbb.disabled as building_disabled';
    $sql_with .= ', mbr.machine_name as room_machine_name, mbr.human_name as room_human_name, mbr.date as room_date, mbr.disabled as room_disabled';
    $sql_with .= ' from mfcs_requests mer';
    $sql_with .= ' inner join mfcs_current mc on mer.id = mc.request_id inner join mfcs_field_dates_date mfdd on (mc.request_id = mfdd.request_id and mc.dates_date = mfdd.revision)';
    $sql_with .= ' left join mfcs_field_dates_time_start mfdts on mfdd.request_id = mfdts.request_id and mfdd.revision = mfdts.revision and mfdd.delta = mfdts.delta';
    $sql_with .= ' left join mfcs_field_dates_time_stop mfdtsp on mfdd.request_id = mfdtsp.request_id and mfdd.revision = mfdtsp.revision and mfdd.delta = mfdtsp.delta';
    $sql_with .= ' left join mfcs_banner_locations mbl on mer.location = mbl.id';
    $sql_with .= ' left join mfcs_banner_buildings mbb on mer.building = mbb.id';
    $sql_with .= ' left join mfcs_banner_rooms mbr on mer.room = mbr.id';
    $sql_with .= ' where mer.status in (:locked, :unlocked, :accepted) and (mfdd.value > :instance or (mfdts.value is not null and mfdts.value > :instance) or (mfdtsp.value is not null and mfdtsp.value > :instance))) ';

    $sql_on_1 = 'r.time_start is null and r2.time_start is null and r.date = r2.date';
    $sql_on_2 = 'r.time_start is null and r2.time_start is not null and r.date >= r2.time_start and r.date < r2.time_stop';
    $sql_on_3 = 'r.time_start is not null and r2.time_start is not null and ((r.time_start >= r2.time_start and r.time_start < r2.time_stop) or (r.time_stop >= r2.time_start and r.time_stop < r2.time_stop))';
    $sql_on_4 = 'r.time_start is not null and r2.time_start is null and r2.date >= r.time_start and r2.date < r.time_stop';

    $sql_main = 'select r.*, r2.id as conflict_id, r2.title as conflict_title, r2.date as conflict_date, r2.time_start as conflict_time_start, r2.time_stop as conflict_time_stop, r2.delta as conflict_delta';
    $sql_main .= ' from requests r';
    $sql_main .= ' inner join requests r2 on (r.id <> r2.id and r.room = r2.room and ((' . $sql_on_1 . ') or (' . $sql_on_2 . ') or (' . $sql_on_3 . ') or (' . $sql_on_4 . ')))';

    $arguments = array(
      ':locked' => MFCS_REQUEST_STATUS_LOCKED,
      ':unlocked' => MFCS_REQUEST_STATUS_UNLOCKED,
      ':accepted' => MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
      ':instance' => $instance,
    );

    if (!is_null($request_id)) {
      $arguments[':request_id'] = $request_id;

      $sql_main .= ' where r.id = :request_id';
    }

    $results = db_query($sql_with . $sql_main, $arguments)->fetchAll();

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        if (!array_key_exists($result->id, $requests)) {
          $requests[$result->id] = array(
            'id' => $result->id,
            'title' => $result->title,
            'location' => array(
              'id' => $result->location,
              'machine_name' => $result->location_machine_name,
              'human_name' => $result->location_human_name,
              'date' => $result->location_date,
              'disabled' => $result->location_disabled,
            ),
            'building' => array(
              'id' => $result->building,
              'machine_name' => $result->building_machine_name,
              'human_name' => $result->building_human_name,
              'date' => $result->building_date,
              'disabled' => $result->building_disabled,
            ),
            'room' => array(
              'id' => $result->room,
              'machine_name' => $result->room_machine_name,
              'human_name' => $result->room_human_name,
              'date' => $result->room_date,
              'disabled' => $result->room_disabled,
            ),
            'date' => $result->date,
            'time_start' => $result->time_start,
            'time_stop' => $result->time_stop,
            'conflict' => array(
            ),
          );
        }

        if (!array_key_exists($result->conflict_id, $requests[$result->id]['conflict'])) {
          $requests[$result->id]['conflict'][$result->conflict_id] = array(
            'title' => $result->conflict_title,
            'date' => array(),
            'time_start' => array(),
            'time_stop' => array(),
          );
        }

        $requests[$result->id]['conflict'][$result->conflict_id]['date'][$result->conflict_delta] = $result->conflict_date;
        $requests[$result->id]['conflict'][$result->conflict_id]['time_start'][$result->conflict_delta] = $result->conflict_time_start;
        $requests[$result->id]['conflict'][$result->conflict_id]['time_stop'][$result->conflict_delta] = $result->conflict_time_stop;
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for stale requests.
 *
 * Stale requests are requests whose dates (one or more) are in the past but the request itself is not yet approved or denied.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_STALE.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests the requests.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_stale($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();
  $instance = mfcs_instance();

  try {
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_dates_date', 'mfdd', 'mc.request_id = mfdd.request_id and mc.dates_date = mfdd.revision');
    $query->leftJoin('mfcs_field_dates_time_start', 'mfdts', 'mfdd.request_id = mfdts.request_id and mfdd.revision = mfdts.revision and mfdd.delta = mfdts.delta');

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');
    $query->addField('mer', 'location', 'location');
    $query->addField('mer', 'building', 'building');
    $query->addField('mer', 'room', 'room');
    $query->addField('mer', 'status', 'status');
    $query->addField('mfdd', 'delta', 'delta');
    $query->addField('mfdd', 'value', 'date');
    $query->addField('mfdts', 'value', 'time_start');

    $status = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
    );

    $query->condition('mer.status', $status, 'IN');

    if (!is_null($request_id)) {
      $query->condition('mer.id', $request_id);
    }

    $or = db_or();
    $or->condition('mfdd.value', $instance, '<=');

    $and = db_and();
    $and->isNotNull('mfdts.value');
    $and->condition('mfdts.value', $instance, '<=');
    $or->condition($and);

    $query->condition($or);

    $results = $query->execute()->fetchAllAssoc('id');

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        if (!array_key_exists($result->id, $requests)) {
          $requests[$result->id] = array(
            'id' => $result->id,
            'title' => $result->title,
            'location' => $result->location,
            'building' => $result->building,
            'room' => $result->room,
            'date' => array(),
            'time_start' => array(),
            'status' => $result->status,
          );
        }

        $requests[$result->id]['date'][$result->delta] = $result->date;
        $requests[$result->id]['time_start'][$result->delta] = $result->time_start;
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for stuck requests.
 *
 * Stuck requests are requests that are no longer able to proceed in the review system when they otherwise should.
 * These are requests than cannot change step or state when they otherwise should.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_STUCK.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests the requests.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 * @see: mfcs_get_requests_current_review_decisions()
 * @see: mfcs_request_troubleshoot_0_request_review_recheck()
 */
function mfcs_miscellaneous_check_requests_stuck($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  // @todo: implement this.
  return FALSE;
}

/**
 * Check for blocked requests.
 *
 * Blocked requests are requests that are no longer able to proceed in the review system when they otherwise should.
 * These are requests than cannot change step or state when they otherwise should.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_BLOCKED.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_blocked($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  // @todo: implement this.
  return FALSE;
}

/**
 * Check for requests without valid venue coordinator.
 *
 * Look for requests where the venue coordinator is disabled, deleted, locked out, or is otherwise invalid.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_COORDINATOR.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_coordinator($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  try {
    // only join if request classification and user classification would both be the same.
    // this will allow for a simple is NULL test to see if the user is not allowed to utilize the content.
    #$complex_join = 'and (';
    #$complex_join .= ' (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_STUDENT . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_STUDENT . ')';
    #$complex_join .= ' or (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_CAMPS . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_CAMPS . ')';
    #$complex_join .= ' or (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_FACULTY . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_FACULTY . ')';
    #$complex_join .= ' or (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_EXTERNAL . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_EXTERNAL . ')';
    #$complex_join .= ' )';

    // only join if request classiciation is valid and the coordinator has at least one banner role.
    $complex_join = 'and (';
    $complex_join .= ' fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_STUDENT;
    $complex_join .= ' or fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_CAMPS;
    $complex_join .= ' or fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_FACULTY;
    $complex_join .= ' or fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_EXTERNAL;
    $complex_join .= ' )';

    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_venue_coordinator_user_id', 'mfvcui', 'mc.request_id = mfvcui.request_id and mc.venue_coordinator_user_id = mfvcui.revision');
    $query->innerJoin('users', 'u', 'mfvcui.value = u.uid');
    $query->leftJoin('field_data_field_user_banner_id', 'fdfubi', 'u.uid = fdfubi.entity_id');
    $query->leftJoin('field_data_field_user_first_name', 'fdfufn', 'u.uid = fdfufn.entity_id');
    $query->leftJoin('field_data_field_user_last_name', 'fdfuln', 'u.uid = fdfuln.entity_id');
    $query->leftJoin('field_data_field_user_event_roles', 'fdfuer', 'u.uid = fdfuer.entity_id ' . $complex_join);

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');
    $query->addField('mer', 'location', 'location');
    $query->addField('mer', 'building', 'building');
    $query->addField('mer', 'room', 'room');
    $query->addField('u', 'uid', 'user_id');
    $query->addField('u', 'name', 'name');
    $query->addField('u', 'status', 'status');
    $query->addField('fdfubi', 'field_user_banner_id_value', 'banner_id');
    $query->addField('fdfufn', 'field_user_first_name_value', 'first_name');
    $query->addField('fdfuln', 'field_user_last_name_value', 'last_name');
    $query->addField('fdfuer', 'field_user_event_roles_tid', 'user_classification');

    $status = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,

      // do not include closed and accepted because the users classifications and status may have long since changed.
      // enabling this would likely result in numerous false positives.
      #MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    );

    $query->condition('mer.status', $status, 'IN');

    if (!is_null($request_id)) {
      $query->condition('mer.id', $request_id);
    }

    $or = db_or();
    $or->condition('u.status', 1, '<>');
    $or->isNull('fdfubi.field_user_banner_id_value');
    $or->isNull('fdfuer.field_user_event_roles_tid');

    $query->condition($or);

    $results = $query->execute()->fetchAll();

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        $requests[$result->id] = array(
          'id' => $result->id,
          'title' => $result->title,
          'location' => $result->location,
          'building' => $result->building,
          'room' => $result->room,
          'coordinator' => array(
            'user_id' => $result->user_id,
            'name' => $result->name,
            'first_name' => $result->first_name,
            'last_name' => $result->last_name,
            'banner_id' => $result->banner_id,
            'classification' => $result->user_classification,
            'status' => $result->status,
          ),
        );
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for requests without valid requester.
 *
 * Look for requests where the requester is disabled, deleted, locked out, or is otherwise invalid.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_REQUESTER.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_requester($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  try {
    // only join if request classification and user classification would both be the same.
    // this will allow for a simple is NULL test to see if the user is not allowed to utilize the content.
    $complex_join = 'and (';
    $complex_join .= ' (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_STUDENT . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_STUDENT . ')';
    $complex_join .= ' or (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_CAMPS . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_CAMPS . ')';
    $complex_join .= ' or (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_FACULTY . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_FACULTY . ')';
    $complex_join .= ' or (mer.classification = ' . MFCS_REQUEST_CLASSIFICATION_EXTERNAL . ' and fdfuer.field_user_event_roles_tid = ' . MFCS_USER_CLASSIFICATION_EXTERNAL . ')';
    $complex_join .= ' )';

    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('users', 'u', 'mer.user_id = u.uid');
    $query->leftJoin('field_data_field_user_banner_id', 'fdfubi', 'u.uid = fdfubi.entity_id');
    $query->leftJoin('field_data_field_user_first_name', 'fdfufn', 'u.uid = fdfufn.entity_id');
    $query->leftJoin('field_data_field_user_last_name', 'fdfuln', 'u.uid = fdfuln.entity_id');
    $query->leftJoin('field_data_field_user_event_roles', 'fdfuer', 'u.uid = fdfuer.entity_id ' . $complex_join);

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');
    $query->addField('mer', 'location', 'location');
    $query->addField('mer', 'building', 'building');
    $query->addField('mer', 'room', 'room');
    $query->addField('u', 'uid', 'user_id');
    $query->addField('u', 'name', 'name');
    $query->addField('u', 'status', 'status');
    $query->addField('fdfubi', 'field_user_banner_id_value', 'banner_id');
    $query->addField('fdfufn', 'field_user_first_name_value', 'first_name');
    $query->addField('fdfuln', 'field_user_last_name_value', 'last_name');
    $query->addField('fdfuer', 'field_user_event_roles_tid', 'user_classification');

    $status = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,

      // do not include closed and accepted because the users classifications and status may have long since changed.
      // enabling this would likely result in numerous false positives.
      #MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    );

    $query->condition('mer.status', $status, 'IN');

    if (!is_null($request_id)) {
      $query->condition('mer.id', $request_id);
    }

    $or = db_or();
    $or->condition('u.status', 1, '<>');
    $or->isNull('fdfubi.field_user_banner_id_value');
    $or->isNull('fdfuer.field_user_event_roles_tid');

    $query->condition($or);

    $results = $query->execute()->fetchAll();

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        $requests[$result->id] = array(
          'id' => $result->id,
          'title' => $result->title,
          'location' => $result->location,
          'building' => $result->building,
          'room' => $result->room,
          'requester' => array(
            'user_id' => $result->user_id,
            'name' => $result->name,
            'first_name' => $result->first_name,
            'last_name' => $result->last_name,
            'banner_id' => $result->banner_id,
            'classification' => $result->user_classification,
            'status' => $result->status,
          ),
        );
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for requests without valid room.
 *
 * Look for requests where the room is disabled, deleted, or is otherwise invalid.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_ROOM.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *     Each request id will be associated with a room id.
 *     If the room id has NULL for a value, then the supplied room is invalid.
 *     If the room id has an array for a value, then the problem can be determined by the values (such as 'disabled' being set to 1).
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_room($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  try {
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_room', 'mftr', 'mc.request_id = mftr.request_id and mc.top_room = mftr.revision');
    $query->leftJoin('mfcs_banner_rooms', 'mbr', 'mftr.value = mbr.id');
    $query->leftJoin('mfcs_banner_locations', 'mbl', 'mer.location = mbl.id');
    $query->leftJoin('mfcs_banner_buildings', 'mbb', 'mer.building = mbb.id');

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');

    $query->addField('mer', 'location', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_machine_name');
    $query->addField('mbl', 'human_name', 'location_human_name');
    $query->addField('mbl', 'date', 'location_date');
    $query->addField('mbl', 'disabled', 'location_disabled');

    $query->addField('mer', 'building', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_machine_name');
    $query->addField('mbb', 'human_name', 'building_human_name');
    $query->addField('mbb', 'date', 'building_date');
    $query->addField('mbb', 'disabled', 'building_disabled');

    $query->addField('mftr', 'value', 'room_id');
    $query->addField('mbr', 'machine_name', 'room_machine_name');
    $query->addField('mbr', 'human_name', 'room_human_name');
    $query->addField('mbr', 'date', 'room_date');
    $query->addField('mbr', 'disabled', 'room_disabled');
    $query->addField('mbr', 'coordinator_id', 'room_coordinator_id');

    $status = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    );

    $query->condition('mer.status', $status, 'IN');

    if (!is_null($request_id)) {
      $query->condition('mer.id', $request_id);
    }

    $or = db_or();
    $or->isNull('mbr.id');
    $or->condition('mbr.disabled', 1);

    $query->condition($or);

    $results = $query->execute()->fetchAll();

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        $requests[$result->id] = array(
          'id' => $result->id,
          'title' => $result->title,
          'location' => array(
            'id' => $result->location_id,
            'machine_name' => $result->location_machine_name,
            'human_name' => $result->location_human_name,
            'date' => $result->location_date,
            'disabled' => $result->location_disabled,
          ),
          'building' => array(
            'id' => $result->building_id,
            'machine_name' => $result->building_machine_name,
            'human_name' => $result->building_human_name,
            'date' => $result->building_date,
            'disabled' => $result->building_disabled,
          ),
          'room' => array(
            'id' => $result->room_id,
            'machine_name' => $result->room_machine_name,
            'human_name' => $result->room_human_name,
            'date' => $result->room_date,
            'disabled' => $result->room_disabled,
          ),
        );
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for requests without valid building.
 *
 * Look for requests where the building is disabled, deleted, or is otherwise invalid.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_BUILDING.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *     Each request id will be associated with a location id.
 *     If the building id has NULL for a value, then the supplied building is invalid.
 *     If the building id has an array for a value, then the problem can be determined by the values (such as 'disabled' being set to 1).
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_building($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  try {
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_building', 'mftb', 'mc.request_id = mftb.request_id and mc.top_building = mftb.revision');
    $query->leftJoin('mfcs_banner_buildings', 'mbb', 'mftb.value = mbb.id');
    $query->leftJoin('mfcs_banner_locations', 'mbl', 'mer.location = mbl.id');

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');

    $query->addField('mer', 'location', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_machine_name');
    $query->addField('mbl', 'human_name', 'location_human_name');
    $query->addField('mbl', 'date', 'location_date');
    $query->addField('mbl', 'disabled', 'location_disabled');

    $query->addField('mftb', 'value', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_machine_name');
    $query->addField('mbb', 'human_name', 'building_human_name');
    $query->addField('mbb', 'date', 'building_date');
    $query->addField('mbb', 'disabled', 'building_disabled');

    $query->addField('mer', 'room', 'room_id');

    $status = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    );

    $query->condition('mer.status', $status, 'IN');

    if (!is_null($request_id)) {
      $query->condition('mer.id', $request_id);
    }

    $or = db_or();
    $or->isNull('mbb.id');
    $or->condition('mbb.disabled', 1);

    $query->condition($or);

    $results = $query->execute()->fetchAll();

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        $requests[$result->id] = array(
          'id' => $result->id,
          'title' => $result->title,
          'location' => array(
            'id' => $result->location_id,
            'machine_name' => $result->location_machine_name,
            'human_name' => $result->location_human_name,
            'date' => $result->location_date,
            'disabled' => $result->location_disabled,
          ),
          'building' => array(
            'id' => $result->building_id,
            'machine_name' => $result->building_machine_name,
            'human_name' => $result->building_human_name,
            'date' => $result->building_date,
            'disabled' => $result->building_disabled,
          ),
          'room' => $result->room_id,
        );
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for requests without valid location.
 *
 * Look for requests where the location is disabled, deleted, or is otherwise invalid.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_LOCATION.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *     Each request id will be associated with a location id.
 *     If the location id has NULL for a value, then the supplied location is invalid.
 *     If the location id has an array for a value, then the problem can be determined by the values (such as 'disabled' being set to 1).
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_location($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  try {
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_location', 'mftl', 'mc.request_id = mftl.request_id and mc.top_location = mftl.revision');
    $query->leftJoin('mfcs_banner_locations', 'mbl', 'mftl.value = mbl.id');

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');

    $query->addField('mftl', 'value', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_machine_name');
    $query->addField('mbl', 'human_name', 'location_human_name');
    $query->addField('mbl', 'date', 'location_date');
    $query->addField('mbl', 'disabled', 'location_disabled');

    $query->addField('mer', 'building', 'building_id');
    $query->addField('mer', 'room', 'room_id');

    $status = array(
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_UNLOCKED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    );

    $query->condition('mer.status', $status, 'IN');

    if (!is_null($request_id)) {
      $query->condition('mer.id', $request_id);
    }

    $or = db_or();
    $or->isNull('mbl.id');
    $or->condition('mbl.disabled', 1);

    $query->condition($or);

    $results = $query->execute()->fetchAll();

    if (!is_array($results)) {
      return FALSE;
    }

    if (!empty($results)) {
      foreach ($results as $result) {
        $requests[$result->id] = array(
          'id' => $result->id,
          'title' => $result->title,
          'location' => array(
            'id' => $result->location_id,
            'machine_name' => $result->location_machine_name,
            'human_name' => $result->location_human_name,
            'date' => $result->location_date,
            'disabled' => $result->location_disabled,
          ),
          'building' => $result->building_id,
          'room' => $result->room_id,
        );
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check for requests where the cached data is out of sync.
 *
 * Look for requests where the request cache (mfcs_requests) is inconsistent with the current revision as specified by mfcs_current.
 *
 * These requests should be considered for MFCS_REQUEST_PROBLEM_CACHE_REQUEST.
 *
 * @param int|null $request_id
 *   (optional) If specified, check for conflicts against the specified request.
 *
 * @return array|bool
 *   An array containing requests that are in conflict on success.
 *   FALSE is returned on error.
 *
 * @see: mfcs_requests_set_problem()
 */
function mfcs_miscellaneous_check_requests_problem_cache_requests($request_id = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  $requests = array();

  $cached_fields = array(
    'status' => array(
      'table' => 'mfcs_field_top_status',
      'current' => 'top_status',
      'alias' => 'mfts',
    ),
    'step' => array(
      'table' => 'mfcs_field_top_step',
      'current' => 'top_step',
      'alias' => 'mftsp',
    ),
    'type' => array(
      'table' => 'mfcs_field_information_type',
      'current' => 'information_type',
      'alias' => 'mfit',
    ),
    'classification' => array(
      'table' => 'mfcs_field_request_coordinator_classification',
      'current' => 'request_coordinator_classification',
      'alias' => 'mfrcc',
    ),
    'location' => array(
      'table' => 'mfcs_field_top_location',
      'current' => 'top_location',
      'alias' => 'mftl',
    ),
    'building' => array(
      'table' => 'mfcs_field_top_building',
      'current' => 'top_building',
      'alias' => 'mftb',
    ),
    'room' => array(
      'table' => 'mfcs_field_top_room',
      'current' => 'top_room',
      'alias' => 'mftr',
    ),
    'venue_coordinator' => array(
      'table' => 'mfcs_field_venue_coordinator_user_id',
      'current' => 'venue_coordinator_user_id',
      'alias' => 'mfvcui',
    ),
    'title' => array(
      'table' => 'mfcs_field_information_title',
      'current' => 'information_title',
      'alias' => 'mfit',
    ),
  );

  try {
    foreach ($cached_fields as $cache_name => $uncached) {
      $query = db_select('mfcs_requests', 'mer');
      $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
      $query->innerJoin($uncached['table'], $uncached['alias'], 'mc.request_id = ' . $uncached['alias'] . '.request_id and mc.' . $uncached['current'] . ' = ' . $uncached['alias'] . '.revision');
      $query->leftJoin('mfcs_banner_rooms', 'mbr', 'mer.room = mbr.id');
      $query->leftJoin('mfcs_banner_locations', 'mbl', 'mer.location = mbl.id');
      $query->leftJoin('mfcs_banner_buildings', 'mbb', 'mer.building = mbb.id');

      $query->addField('mer', 'id', 'id');
      $query->addField('mer', 'title', 'title');

      $query->addField('mer', 'location', 'location_id');
      $query->addField('mbl', 'machine_name', 'location_machine_name');
      $query->addField('mbl', 'human_name', 'location_human_name');
      $query->addField('mbl', 'date', 'location_date');
      $query->addField('mbl', 'disabled', 'location_disabled');

      $query->addField('mer', 'building', 'building_id');
      $query->addField('mbb', 'machine_name', 'building_machine_name');
      $query->addField('mbb', 'human_name', 'building_human_name');
      $query->addField('mbb', 'date', 'building_date');
      $query->addField('mbb', 'disabled', 'building_disabled');

      $query->addField('mer', 'room', 'room_id');
      $query->addField('mbr', 'machine_name', 'room_machine_name');
      $query->addField('mbr', 'human_name', 'room_human_name');
      $query->addField('mbr', 'date', 'room_date');
      $query->addField('mbr', 'disabled', 'room_disabled');
      $query->addField('mbr', 'coordinator_id', 'room_coordinator_id');

      $query->addField('mer', $cache_name, 'cache');
      $query->addField($uncached['alias'], 'value', 'real');

      $query->where('mer.' . $cache_name . ' <> ' . $uncached['alias'] . '.value');

      if (!is_null($request_id)) {
        $query->condition('mer.id', $request_id);
      }

      $results = $query->execute()->fetchAll();

      if (!is_array($results)) {
        return FALSE;
      }

      if (!empty($results)) {
        foreach ($results as $result) {
          if (!array_key_exists($result->id, $requests)) {
            $requests[$result->id] = array(
              'id' => $result->id,
              'title' => $result->title,
              'location' => array(
                'id' => $result->location_id,
                'machine_name' => $result->location_machine_name,
                'human_name' => $result->location_human_name,
                'date' => $result->location_date,
                'disabled' => $result->location_disabled,
              ),
              'building' => array(
                'id' => $result->building_id,
                'machine_name' => $result->building_machine_name,
                'human_name' => $result->building_human_name,
                'date' => $result->building_date,
                'disabled' => $result->building_disabled,
              ),
              'room' => array(
                'id' => $result->room_id,
                'machine_name' => $result->room_machine_name,
                'human_name' => $result->room_human_name,
                'date' => $result->room_date,
                'disabled' => $result->room_disabled,
              ),
              'fields' => array(),
            );
          }

          $requests[$result->id]['fields'][$cache_name] = array(
            'table' => $uncached['table'],
            'current' => $uncached['current'],
            'cache' => $result->cache,
            'real' => $result->real,
          );
        }
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $requests;
}

/**
 * Check requests for problems.
 *
 * @param int|null $request_id
 *   (optional) If not NULL, check for problems against the specified request.
 * @param int|null $problem_code
 *   (optional) If not NULL, only perform the check based on the specific code.
 *
 * @return array|bool
 *   An array containing requests the requests, populated for easy SQL insert to {mfcs_problems_requests}.
 *   FALSE is returned on error.
 *
 * @see: mfcs_miscellaneous_check_requests_conflict()
 * @see: mfcs_miscellaneous_check_requests_stale()
 * @see: mfcs_miscellaneous_check_requests_stuck()
 * @see: mfcs_miscellaneous_check_requests_blocked()
 * @see: mfcs_miscellaneous_check_requests_coordinator()
 * @see: mfcs_miscellaneous_check_requests_requester()
 * @see: mfcs_miscellaneous_check_requests_room()
 * @see: mfcs_miscellaneous_check_requests_building()
 * @see: mfcs_miscellaneous_check_requests_location()
 * @see: mfcs_miscellaneous_check_requests_problem_cache_requests()
 */
function mfcs_miscellaneous_check_requests($request_id = NULL, $problem_code = NULL) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  if (!is_null($problem_code) && !cf_is_integer($problem_code)) {
    cf_error::invalid_integer('problem_code');
    return FALSE;
  }

  if (!is_null($problem_code)) {
    $problem_code = (int) $problem_code;
  }

  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $instance = mfcs_instance();
  $problems = array();

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_CONFLICT) {
    $results = mfcs_miscellaneous_check_requests_conflict($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $reason = 'Scheduling Conflict in ';
        $reason .= check_plain($result['room']['human_name']) . ' of ';
        $reason .= check_plain($result['building']['human_name']) . ' for ';
        $reason .= check_plain(date("l, F j, Y", $result['date'])) . ' with requests: ';

        $processed = FALSE;
        foreach ($result['conflict'] as $conflict_id => $conflict_data) {
          if ($processed) {
            $reason .= ', ';
          }
          else {
            $processed = TRUE;
          }

          $reason .= check_plain($conflict_data['title']) . ' [id: ' . check_plain($conflict_id) . ']';
        }
        unset($processed);
        unset($conflict_id);
        unset($conflict_data);

        $reason .= '.';

        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_CONFLICT,
          'date' => $instance,
          'reason' => $reason,
          'details' => serialize(array_keys($result['conflict'])),
        );
        unset($reason);
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_STALE) {
    $results = mfcs_miscellaneous_check_requests_stale($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_STALE,
          'date' => $instance,
          'reason' => 'Request references dates in the past but is not closed.',
          'details' => '',
        );
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_STUCK) {
    $results = mfcs_miscellaneous_check_requests_stuck($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_STUCK,
          'date' => $instance,
          'reason' => 'Request is stuck and cannot continue in the review process.',
          'details' => '',
        );
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_BLOCKED) {
    $results = mfcs_miscellaneous_check_requests_blocked($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_BLOCKED,
          'date' => $instance,
          'reason' => 'Request is blocked from further progress in the review process.',
          'details' => '',
        );
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_COORDINATOR) {
    $results = mfcs_miscellaneous_check_requests_coordinator($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $reason = 'The venue coordinator ';
        $reason .= $result['coordinator']['first_name'] . ' ' . $result['coordinator']['last_name'];
        $reason .= ' [id: ' . check_plain($result['coordinator']['user_id']) . ']';

        if ($result['coordinator']['user_id']['status'] == 0) {
          $reason .= ' account is disabled.';
        }
        elseif (empty($result['coordinator']['user_id']['classification'])) {
          $reason .= ' lacks the appropriate banner roles.';
        }
        else {
          $reason .= ' assignment is invalid.';
        }

        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_COORDINATOR,
          'date' => $instance,
          'reason' => $reason,
          'details' => '',
        );
        unset($reason);
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_REQUESTER) {
    $results = mfcs_miscellaneous_check_requests_requester($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $reason = 'The request coordinator ';
        $reason .= $result['requester']['first_name'] . ' ' . $result['requester']['last_name'];
        $reason .= ' [id: ' . $result['requester']['user_id'] . ']';

        if ($result['requester']['status'] == 0) {
          $reason .= ' account is disabled.';
        }
        elseif (empty($result['requester']['user_id']['classification'])) {
          $reason .= ' lacks the appropriate banner roles.';
        }
        else {
          $reason .= ' assignment is invalid.';
        }

        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_REQUESTER,
          'date' => $instance,
          'reason' => $reason,
          'details' => '',
        );
        unset($reason);
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_ROOM) {
    $results = mfcs_miscellaneous_check_requests_room($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_ROOM,
          'date' => $instance,
          'reason' => 'The room ' . $result['room']['human_name'] . ' of ' . $result['building']['human_name'] . ' is unavailable, deleted, or otherwise invalid.',
          'details' => '',
        );
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_BUILDING) {
    $results = mfcs_miscellaneous_check_requests_building($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_BUILDING,
          'date' => $instance,
          'reason' => 'The building ' . $result['building']['human_name'] . ' is unavailable, deleted, or otherwise invalid.',
          'details' => '',
        );
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_LOCATION) {
    $results = mfcs_miscellaneous_check_requests_location($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_LOCATION,
          'date' => $instance,
          'reason' => 'The location ' . $result['location']['human_name'] . ' is unavailable, deleted, or otherwise invalid.',
          'details' => '',
        );
      }
      unset($result);
    }
    unset($results);
  }

  if (is_null($problem_code) || $problem_code === MFCS_REQUEST_PROBLEM_CACHE_REQUEST) {
    $results = mfcs_miscellaneous_check_requests_problem_cache_requests($request_id);
    if (is_array($results) && !empty($results)) {
      foreach ($results as $result) {
        $reason = 'Inconsistencies in the following fields have been detected: ';

        $processed = FALSE;
        foreach ($result['fields'] as $field_name => $field_data) {
          if ($processed) {
            $reason .= ', ';
          }
          else {
            $processed = TRUE;
          }
          $reason .= check_plain($field_name) . ' (cache = ' . check_plain($field_data['cache']) . ' vs real = ' . check_plain($field_data['real']) . ')';
        }
        unset($processed);
        unset($field_name);
        unset($field_data);

        $reason .= '.';

        $problems[] = array(
          'request_id' => $result['id'],
          'problem' => MFCS_REQUEST_PROBLEM_CACHE_REQUEST,
          'date' => $instance,
          'reason' => $reason,
          'details' => '',
        );

        unset($reason);
      }
      unset($result);
    }
    unset($results);
  }

  return $problems;
}

/**
 * Returns an array of already detected request problems.
 *
 * @param int|null $problem_id
 *   (optional) When specified, is an integer representing a single problem to load by the problem id.
 * @param bool|null $include_ignored
 *   (optional) When TRUE, include ignored problems in listing.
 *   When FALSE, problems that are ignored are not including in the listing.
 *
 * @param array|bool
 *   An array of problems is returned on success.
 *   FALSE is returned on error.
 */
function mfcs_miscellaneous_problems_request_load($problem_id = NULL, $include_ignored = FALSE) {
  if (!is_null($problem_id) && !cf_is_integer($problem_id)) {
    cf_error::invalid_integer('problem_id');
    return FALSE;
  }

  if (!is_bool($include_ignored)) {
    cf_error::invalid_bool('include_ignored');
    return FALSE;
  }

  $problems = array();
  try {
    $query = db_select('mfcs_problems_requests', 'mpr');

    $query->addField('mpr', 'id', 'id');
    $query->addField('mpr', 'problem', 'problem');
    $query->addField('mpr', 'request_id', 'request_id');
    $query->addField('mpr', 'date', 'date');
    $query->addField('mpr', 'reason', 'reason');
    $query->addField('mpr', 'details', 'details');
    $query->addField('mpr', 'ignore', 'ignore');

    if (!is_null($problem_id)) {
      $query->condition('mpr.id', $problem_id);
    }

    if (!$include_ignored) {
      $query->condition('mpr.ignore', 0);
    }

    $problems = $query->execute()->fetchAllAssoc('id');
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $problems;
}

/**
 * Returns an array of already detected request problems.
 *
 * @param array $conditions
 *   An array containing select conditions, such as:
 *   - 'mpr.id': problem id.
 *   - 'mpr.problem': problem type.
 *   The each key may be an array such as:
 *     - array('value' => '1%', 'operator' => 'LIKE')
 *     - array('value' => array(0, 1, 3), 'operator' => 'IN')
 *     - array('value' => 1, 'operator' => '>=')
 * @param array $options
 *   (optional) When specified, representing an array containing:
 *     'start': The starting location.
 *     'limit': The number of rows to select, aka: length.
 *     'count': If TRUE, an unfiltered count query is processed.
 *     'results': If FALSE, the main query is not performed.
 *     'sort': An array of database sorting columns and their sort orders.
 *     'sensitive': TRUE for case sensitive and FALSE otherwise.
 *     'extra_fields': An array of extra fields that should be loaded.
 *                     The array will have a key of the table name and a value
 *                     of an array. The value array will contain an 'alias'
 *                     string, an 'on' string, a 'join' string, and a 'columns'
 *                     array.
 *     'distinct': Process the entire query as distinct.
 *     'distinct_id': Custom option to load mer.id as distinct. This overrides
 *                    'distinct' when TRUE.
 *     'include_ignored': When TRUE will load ignored problems.
 *
 * @return array|false
 *   An array of objects containing the mfcs-specific user fields.
 *
 *   FALSE is returned on error.
 *
 * @see: mfcs_request_process_query_search_array()
 */
function mfcs_miscellaneous_problems_request_search($conditions = array(), $options = array()) {
  if (!is_array($conditions)) {
    cf_error::invalid_array('conditions');
    return FALSE;
  }

  if (!is_array($options)) {
    cf_error::invalid_array('options');
    return FALSE;
  }

  $start = 0;
  if (array_key_exists('start', $options) && cf_is_integer($options['start'])) {
    $start = (int) $options['start'];
  }

  $limit = NULL;
  if (array_key_exists('limit', $options) && cf_is_integer($options['limit'])) {
    $limit = (int) $options['limit'];
  }

  $count = TRUE;
  if (array_key_exists('count', $options) && is_bool($options['count'])) {
    $count = $options['count'];
  }

  $results = TRUE;
  if (array_key_exists('results', $options) && is_bool($options['results'])) {
    $results = $options['results'];
  }

  $sort = NULL;
  if (array_key_exists('sort', $options) && is_array($options['sort'])) {
    $sort = $options['sort'];
  }

  $sensitive = FALSE;
  if (array_key_exists('sensitive', $options) && is_bool($options['sensitive'])) {
    $sensitive = $options['sensitive'];
  }

  $extra_fields = FALSE;
  if (array_key_exists('extra_fields', $options) && is_array($options['extra_fields'])) {
    $extra_fields = $options['extra_fields'];
  }

  $include_ignored = FALSE;
  if (array_key_exists('include_ignored', $options) && is_bool($options['include_ignored'])) {
    $include_ignored = $options['include_ignored'];
  }

  $distinct = FALSE;
  if (isset($options['distinct']) && is_bool($options['distinct'])) {
    $distinct = $options['distinct'];
  }

  $distinct_id = FALSE;
  if (isset($options['distinct_id']) && is_bool($options['distinct_id'])) {
    $distinct_id = $options['distinct_id'];
    if ($distinct_id) {
      $distinct = FALSE;
    }
  }


  $return = array(
    'results' => NULL,
    'count' => NULL,
  );

  $s = 'i';
  $like = 'ILIKE';
  if ($sensitive) {
    $s = '';
    $like = 'LIKE';
  }

  $already_joined_count = 0;
  $already_joined = array();

  $already_added = array();

  $problems = array();
  try {
    $outermost_and = NULL;

    $query = db_select('mfcs_problems_requests', 'mpr');
    $already_joined['mfcs_problems_requests'] = 'mpr';

    if (!$include_ignored) {
      $query->condition('mpr.ignore', 0);
    }

    $count_query = NULL;
    if ($count) {
      $count_query = clone($query);

      if ($distinct_id) {
        $count_query->addExpression('count(distinct mpr.user_id)');
      }
      else {
        $count_query->addExpression('count(mpr.id)');
      }
    }

    if ($distinct) {
      $query->distinct();
    }

    if ($distinct_id) {
      $query->distinct(array('field' => 'mpr.id', 'alias' => 'id'));
    }
    else {
      $query->addField('mpr', 'id', 'id');
    }

    $query->addField('mpr', 'problem', 'problem');
    $query->addField('mpr', 'request_id', 'request_id');
    $query->addField('mpr', 'date', 'date');
    $query->addField('mpr', 'reason', 'reason');
    $query->addField('mpr', 'ignore', 'ignore');

    $already_added['id'] = 'mpr.id';
    $already_added['problem'] = 'mpr.problem';
    $already_added['request_id'] = 'mpr.request_id';
    $already_added['date'] = 'mpr.date';
    $already_added['reason'] = 'mpr.reason';
    $already_added['ignore'] = 'mpr.ignore';


    // add custom fields to the select results as requested by the caller.
    if (!empty($extra_fields)) {
      foreach ($extra_fields as $extra_table_name => $extra_values) {
        if (!array_key_exists('alias', $extra_values) || !array_key_exists('join', $extra_values) || !array_key_exists('columns', $extra_values)) {
          continue;
        }

        if (array_key_exists($extra_values['alias'], $already_joined)) {
          continue;
        }

        if ($extra_values['join'] == 'inner') {
          $query->innerJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);

          if ($count) {
            $count_query->innerJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);
          }
        }
        elseif ($extra_values['join'] == 'left') {
          $query->leftJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);

          if ($count) {
            $count_query->leftJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);
          }
        }
        elseif ($extra_values['join'] == 'right') {
          $query->rightJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);

          if ($count) {
            $count_query->rightJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);
          }
        }

        $already_joined[$extra_values['alias']] = $extra_table_name;
        foreach ($extra_values['columns'] as $column_external_name => $column_internal_name) {
          if (array_key_exists($column_external_name, $already_added)) {
            continue;
          }

          $query->addField($extra_values['alias'], $column_internal_name, $column_external_name);
          $already_added[$column_external_name] = $extra_values['alias'] . '.' . $column_internal_name;
        }
      }
    }

    if (!empty($conditions)) {
      $operand = db_and();
      $result = mfcs_request_process_query_search_array($query, $count_query, $operand, $already_joined, $already_joined_count, $already_added, $conditions, 1, $like);

      if ($result) {
        if (is_null($outermost_and)) {
          $outermost_and = db_and();
        }
        $outermost_and->condition($operand);
      }

      unset($operand);
    }

    if ($limit > 0) {
      $query->range($start, $limit);
    }

    if (!is_null($outermost_and)) {
      $query->condition($outermost_and);
      if ($count) {
        $count_query->condition($outermost_and);
      }
    }

    if (is_null($sort)) {
      // default to sorting/ordering by request id.
      if ($distinct || $distinct_id) {
        // when a query is distinct, the order by values must be defined in the select columns.
        $query->orderBy('id' , 'DESC');
      }
      else {
        $query->orderBy('mpr.id', 'DESC');
      }
    }
    else {
      if (!empty($sort)) {
        // When selecting distinct on a specific column/alias, it must be specified in order by if order by is used.
        // And distinct field must be the first column to be ordered by.
        if ($distinct_id) {
          if (array_key_exists('id', $sort) && !empty($sort['id'])) {
            $query->orderBy('id' , $sort['id']);
          }
          else {
            $query->orderBy('id' , 'DESC');
          }
        }

        foreach ($sort as $sort_name => $sort_value) {
          if ($sort_name == 'id') {
            // when a query is distinct, the order by values must be defined in the select columns.
            if ($distinct_id) {
              continue;
            }
            elseif ($distinct) {
              $query->orderBy('id' , is_null($sort_value) ? 'ASC' : $sort_value);
            }
            else {
              $query->orderBy('mpr.id', is_null($sort_value) ? 'ASC' : $sort_value);
            }
          }
          elseif (!empty($sort_name)) {
            $query->orderBy($sort_name, $sort_value);
          }
        }
      }
    }

    if ($count) {
      $return['count'] = $count_query->execute()->fetchField();
    }

    if ($results) {
      $return['results'] = $query->execute()->fetchAllAssoc('id');
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $return;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
