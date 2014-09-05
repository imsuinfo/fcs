<?php

/**
 * @file
 * Defines McNeese facilities use request calendar page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request calendar month page.
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
function mfcs_request_calendar_month_0_page($year = NULL, $month = NULL) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

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

  if (is_null($year)) {
    $month_start = strtotime('midnight first day of', REQUEST_TIME);
    $month_end = strtotime('midnight last day of', REQUEST_TIME);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }
  else {
    $current_date = mfcs_request_calendar_determine_current_date($year, $month, NULL);

    $month_start = strtotime('midnight first day of ' . $current_date['month'] . ' ' . $current_date['year']);
    $month_end = strtotime('midnight last day of ' . $current_date['month'] . ' ' . $current_date['year']);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }

  $month = strtolower(date('F', $month_start));
  $year = date('Y', $month_start);
  mfcs_add_canonical_headers('requests/calendar-0/month/' . $year . '/' . $month);

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

  $results = mfcs_request_load_listing($search, FALSE, 0, 0, $sorting);

  $rows = array();
  if (!empty($results)) {
    foreach ($results as $item) {
      foreach ($item->date as $delta => $date) {
        if ($date < $absolute_start || $date >= $absolute_stop) {
          continue;
        }

        $day = date('j', $date);
        $month = date('n', $date);
        $month_day = $month . '-' . $day;
        if (!isset($rows[$month_day])) {
          $rows[$month_day] = array();
        }

        $is_current_month = TRUE;
        if ($date < $month_start || $date >= $month_stop) {
          $is_current_month = FALSE;
        }

        if (!isset($rows[$month_day][$item->id])) {
          $item_title = check_plain($item->title);
          $location_name = check_plain($item->location_name);
          $building_name = check_plain($item->building_name);
          $room_name = check_plain($item->room_name);

          $rows[$month_day][$item->id] = array(
            'href' => $base_path . 'requests/view-0/' . $item->id,
            'title' => $item_title,
            'tooltip' => '[' . $item->id . '] (' . $location_name . ') ' . $building_name . ' ' . $room_name . ': ' . $item_title,
            'date' => $date,
            'year' => $year,
            'month' => $month,
            'day' => $day,
            'location' => $location_name,
            'building' => $building_name,
            'room' => $room_name,
          );

          unset($item_title);
          unset($location_name);
          unset($building_name);
          unset($room_name);
        }
      }
    }
  }

  mfcs_include(8);

  $markup = '';
  $title = '<h3>' . "Requests for " . date("F, Y", $month_start) . '</h3>';
  $result = mfcs_build_calendar_month_markup($rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, 'mfcs-calendar-0-month');
  if ($result !== FALSE) {
    $markup .= $result;
  }
  unset($result);

  return $markup;
}

/**
 * Provides the facilities use request calendar month page.
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
function mfcs_request_calendar_day_0_page($year = NULL, $month = NULL, $day = NULL) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year) && !is_null($month) || is_null($year) && is_null($month) && !is_null($day)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_path;
  $module_path = drupal_get_path('module', 'mfcs');

  drupal_add_js($module_path . '/js/calendar-day-zindex.js', array('type' => 'file', 'group' => JS_DEFAULT, 'preprocess' => TRUE));

  $user = cf_current_user();

  mfcs_include(4);


  if (is_null($year)) {
    $day_start = strtotime('midnight today', REQUEST_TIME);
    $day_stop = strtotime('midnight tomorrow', $day_start);
  }
  else {
    $current_date = mfcs_request_calendar_determine_current_date($year, $month, $day);

    $day_start = strtotime('midnight ' . $current_date['month'] . ' ' . $current_date['day'] . ' ' . $current_date['year']);
    $day_stop = strtotime('midnight tomorrow', $day_start);
  }


  $month = strtolower(date('F', $day_start));
  $year = date('Y', $day_start);
  $day = date('d', $day_start);
  mfcs_add_canonical_headers('requests/calendar-0/month/' . $year . '/' . $month . '/' . $day);


  $target_field_name = 'field_dates-date-start-0';
  $search[$target_field_name]['group_name'] = 'dates';
  $search[$target_field_name]['field_name'] = 'date';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = $day_start;
  $search[$target_field_name]['type'] = 'date';
  $search[$target_field_name]['multiple'] = TRUE;
  $search[$target_field_name]['operator'] = MFCS_OPERATOR_GREATER_THAN_EQUAL;

  $target_field_name = 'field_dates-date-stop-0';
  $search[$target_field_name]['group_name'] = 'dates';
  $search[$target_field_name]['field_name'] = 'date';
  $search[$target_field_name]['column'] = 'value';
  $search[$target_field_name]['search'] = $day_stop;
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

  $results = mfcs_request_load_listing($search, FALSE, 0, 0, $sorting);

  $rows = array(
    'all_day' => array(),
    'range' => array(),
  );

  // pre-populate range at 1-hour intevals
  $range_interval = "+1 hours";

  $current_time = $day_start;
  do {
    $rows['range'][$current_time] = array();
    $current_time = strtotime($range_interval, $current_time);
  } while ($current_time < $day_stop);

  if (!empty($results)) {
    foreach ($results as $item) {
      foreach ($item->date as $delta => $date) {
        if ($date < $day_start || $date >= $day_stop) {
          continue;
        }

        foreach ($item->time_start as $time_delta => $time_start) {
          $item_title = check_plain($item->title);
          $location_name = check_plain($item->location_name);
          $building_name = check_plain($item->building_name);
          $room_name = check_plain($item->room_name);

          $location = (int) $item->location;
          $building = (int) $item->building;
          $room = (int) $item->room;
          $type = (int) $item->type;

          if (is_null($time_start)) {
            if (!isset($rows['all_day'][$date])) {
              $rows['all_day'][$date] = array();
            }

            $rows['all_day'][$date][$item->id] = array(
              'href' => $base_path . 'requests/view-0/' . $item->id,
              'title' => $item_title,
              'tooltip' => '[' . $item->id . '] (' . $location_name . ') ' . $building_name . ' ' . $room_name . ': ' . $item_title,
              'date' => $date,
              'year' => $year,
              'month' => $month,
              'day' => date('j', $date),
              'location' => $location,
              'building' => $building,
              'room' => $room,
              'type' => $type,
              'location_name' => $location_name,
              'building_name' => $building_name,
              'room_name' => $room_name,
            );
          }
          else {
            // floor() to nearest hour.
            $time_floor = $time_start - ($time_start % 3600);

            $rows['range'][$time_floor][$item->id] = array(
              'href' => $base_path . 'requests/view-0/' . $item->id,
              'title' => $item_title,
              'tooltip' => '[' . $item->id . '] (' . $location_name . ') ' . $building_name . ' ' . $room_name . ': ' . $item_title,
              'date' => $date,
              'year' => $year,
              'month' => $month,
              'day' => date('j', $date),
              'location' => $location,
              'building' => $building,
              'room' => $room,
              'type' => $type,
              'location_name' => $location_name,
              'building_name' => $building_name,
              'room_name' => $room_name,
              'time_start' => $time_start,
              'time_stop' => $item->time_stop[$time_delta],
            );
          }

          unset($item_title);
          unset($location_name);
          unset($building_name);
          unset($room_name);

          unset($location);
          unset($building);
          unset($room);
          unset($type);
        }
      }
    }
  }

  mfcs_include(8);

  $markup = '';
  $title = '<h3>' . "Requests for " . date("F j, Y", $day_start) . '</h3>';
  $result = mfcs_build_calendar_day_markup($rows, $day_start, $day_stop, $title, 'mfcs-calendar-0-day');
  if ($result !== FALSE) {
    $markup .= $result;
  }
  unset($result);

  return $markup;
}

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
function mfcs_request_calendar_determine_current_date($year, $month, $day) {
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
 * @} End of '@addtogroup mfcs'.
 */