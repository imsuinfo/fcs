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
  global $mfcs_determined;

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

  if (!empty($mfcs_determined['filters'])) {
    foreach ($mfcs_determined['filters'] as $filter_key => $filter) {
      $search[$filter_key] = $filter;
    }
  }

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  // load rooms and buildings separately to avoid performing joining on tables.
  $rooms = mfcs_load_rooms();
  $buildings = mfcs_load_buildings();

  if (!is_array($rooms)) {
    $rooms = array();
  }

  if (!is_array($buildings)) {
    $buildings = array();
  }

  $rows = array();
  $current_date = $absolute_start;
  while ($current_date < $absolute_stop) {
    $next_date = strtotime('+1 days', $current_date);

    $search['field_dates-date-start-0']['search'] = $current_date;
    $search['field_dates-date-stop-0']['search'] = $next_date;

    $options = array(
      'limit' => 0,
      'sort' => array('date' => 'ASC'),
    );

    $items_returned = mfcs_request_load_listing($search, $options);

    $items = array();
    if (isset($items_returned['results']) && is_array($items_returned['results'])) {
      $items = $items_returned['results'];
    }

    if (!empty($items)) {
      foreach ($items as $item) {
        foreach ($item->date as $delta => $date) {
          if ($date < $current_date || $date >= $next_date) {
            continue;
          }

          $year = date('Y', $date);
          $day = date('j', $date);
          $month = date('n', $date);
          $month_day = $month . '-' . $day;
          if (!isset($rows[$month_day])) {
            $rows[$month_day] = array();
          }

          if (isset($rows[$month_day][$item->id])) {
            $rows[$month_day][$item->id]['time_start'][] = array('start' => $item->time_start, 'stop' => $item->time_stop);
          }
          else {
            $room_name = '';
            if (array_key_exists($item->room, $rooms)) {
              $room_name = check_plain($rooms[$item->room]->room_name);
            }

            $building_name = '';
            if (array_key_exists($item->building, $buildings)) {
              $building_name = check_plain($buildings[$item->building]->building_name);
            }

            $month_day_count = count($rows[$month_day]);
            if ($options['limit'] > 0 && $month_day_count == $options['limit']) {
              $month_name = date('F', $date);
              $month_name_lower = strtolower($month_name);

              $rows[$month_day][$item->id] = array(
                'href' => $base_path . 'requests/calendar-0/day/' . $year . '/' . $month_name_lower . '/' . $day . $url_arguments,
                'title' => '. . .',
                'tooltip' => 'View more requests on ' . $month_name . ' ' . $day . ', ' . $year,
                'date' => $date,
                'month' => $month,
                'day' => $day,
                'times' => NULL,
              );

              unset($year);
              unset($month_name);
              unset($month_name_lower);
            }
            else {
              $item_title = check_plain($item->title);

              $location = (int) $item->location;
              $building = (int) $item->building;
              $room = (int) $item->room;
              $type = (int) $item->type;

              $rows[$month_day][$item->id] = array(
                'href' => $base_path . 'requests/view-0/' . $item->id . $url_arguments,
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
            }
          }
        }
      }
    }

    $current_date = $next_date;
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
  global $mfcs_determined;
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

  if (!empty($mfcs_determined['filters'])) {
    foreach ($mfcs_determined['filters'] as $filter_key => $filter) {
      $search[$filter_key] = $filter;
    }
  }

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  // load rooms and buildings separately to avoid performing joining on tables.
  $rooms = mfcs_load_rooms();
  $buildings = mfcs_load_buildings();

  if (!is_array($rooms)) {
    $rooms = array();
  }

  if (!is_array($buildings)) {
    $buildings = array();
  }

  $options = array(
    'limit' => 0,
    'sort' => array('date' => 'ASC'),
  );

  $items_returned = mfcs_request_load_listing($search, $options);

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }

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

  if (!empty($items)) {
    foreach ($items as $item) {
      foreach ($item->date as $delta => $date) {
        if ($date < $day_start || $date >= $day_stop) {
          continue;
        }

        foreach ($item->time_start as $time_delta => $time_start) {
          if (!is_null($time_start) && ($time_start < $day_start || $time_start >= $day_stop)) {
            continue;
          }

          $item_title = check_plain($item->title);

          $room_name = '';
          if (array_key_exists($item->room, $rooms)) {
            $room_name = check_plain($rooms[$item->room]->room_name);
          }

          $building_name = '';
          if (array_key_exists($item->building, $buildings)) {
            $building_name = check_plain($buildings[$item->building]->building_name);
          }

          $location = (int) $item->location;
          $building = (int) $item->building;
          $room = (int) $item->room;
          $type = (int) $item->type;

          if (is_null($time_start)) {
            if (!isset($rows['all_day'][$date])) {
              $rows['all_day'][$date] = array();
            }

            $rows['all_day'][$date][$item->id] = array(
              'href' => $base_path . 'requests/view-0/' . $item->id . $url_arguments,
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
              'href' => $base_path . 'requests/view-0/' . $item->id . $url_arguments,
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
        }
      }
    }
  }

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $markup = '';
  $title = '<h3>' . "Requests for " . '<a href="' . $base_path . 'requests/calendar-0/month/' . date("Y/F", $day_start) . $url_arguments . '" title="Requests for ' . date("F, Y", $day_start) . '">' . date("F", $day_start) . '</a> ' . date("j, Y", $day_start) . '</h3>';

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