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

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $instance = mfcs_instance();

  if (is_null($year)) {
    $month_start = strtotime('midnight first day of', $instance);
    $month_end = strtotime('midnight last day of', $instance);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }
  else {
    $current_date = mfcs_miscellaneous_determine_current_date($year, $month, NULL);

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

  $search = mfcs_miscellaneous_generate_requests_by_date_search($absolute_start, $absolute_stop);

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
          $building_name = check_plain($item->building_name);
          $room_name = check_plain($item->room_name);

          $location = (int) $item->location;
          $building = (int) $item->building;
          $room = (int) $item->room;
          $type = (int) $item->type;

          $rows[$month_day][$item->id] = array(
            'href' => $base_path . 'requests/view-0/' . $item->id,
            'title' => $item_title,
            'tooltip' => '[' . $item->id . '] ' . $building_name . ' ' . $room_name . ': ' . $item_title,
            'date' => $date,
            'year' => $year,
            'month' => $month,
            'day' => date('j', $date),
            'location' => $location,
            'building' => $building,
            'room' => $room,
            'type' => $type,
            'building_name' => $building_name,
            'room_name' => $room_name,
          );

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

  mfcs_include(MFCS_INCLUDE_OUTPUT);

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

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $instance = mfcs_instance();

  if (is_null($year)) {
    $day_start = strtotime('midnight today', $instance);
    $day_stop = strtotime('midnight tomorrow', $day_start);
  }
  else {
    $current_date = mfcs_miscellaneous_determine_current_date($year, $month, $day);

    $day_start = strtotime('midnight ' . $current_date['month'] . ' ' . $current_date['day'] . ' ' . $current_date['year']);
    $day_stop = strtotime('midnight tomorrow', $day_start);
  }


  $month = strtolower(date('F', $day_start));
  $year = date('Y', $day_start);
  $day = date('d', $day_start);
  mfcs_add_canonical_headers('requests/calendar-0/day/' . $year . '/' . $month . '/' . $day);

  $search = mfcs_miscellaneous_generate_requests_by_date_search($day_start, $day_stop);

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
              'tooltip' => '[' . $item->id . '] ' . $building_name . ' ' . $room_name . ': ' . $item_title,
              'date' => $date,
              'year' => $year,
              'month' => $month,
              'day' => date('j', $date),
              'location' => $location,
              'building' => $building,
              'room' => $room,
              'type' => $type,
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
              'tooltip' => '[' . $item->id . '] ' . $building_name . ' ' . $room_name . ': ' . $item_title,
              'date' => $date,
              'year' => $year,
              'month' => $month,
              'day' => date('j', $date),
              'location' => $location,
              'building' => $building,
              'room' => $room,
              'type' => $type,
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

  mfcs_include(MFCS_INCLUDE_OUTPUT);

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
 * @} End of '@addtogroup mfcs'.
 */