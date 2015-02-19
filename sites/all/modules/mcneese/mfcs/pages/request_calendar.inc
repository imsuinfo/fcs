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

  if (!is_null($month) && !is_string($month)) {
    cf_error::invalid_string('month');

    drupal_not_found();
    drupal_exit();
  }

  $valid_month_names = array(
    'january',
    'february',
    'march',
    'april',
    'may',
    'june',
    'july',
    'august',
    'september',
    'october',
    'november',
    'december',
  );

  if (!is_null($month) && !in_array(strtolower($month), $valid_month_names)) {
    cf_error::invalid_variable('month', "Must be a valid month name.");

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
      'distinct_id' => TRUE,
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
 * @param int $page_number
 *   (optional) The page number. Requests are broken up into different pages
 *   to better handle large numbers of requests per time slot.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_calendar_day_0_page($year = NULL, $month = NULL, $day = NULL, $page_number = 0) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year) && !is_null($month) || is_null($year) && is_null($month) && !is_null($day)) {
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

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  $valid_month_names = array(
    'january',
    'february',
    'march',
    'april',
    'may',
    'june',
    'july',
    'august',
    'september',
    'october',
    'november',
    'december',
  );

  if (!in_array(strtolower($month), $valid_month_names)) {
    cf_error::invalid_variable('month', "Must be a valid month name.");

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($day) && ($day < 1 || $day > 31)) {
    cf_error::invalid_variable('month', "Must be a valid day number.");

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
    'distinct_id' => TRUE,
  );

  $items_returned = mfcs_request_load_listing($search, $options);

  $items = array();
  if (isset($items_returned['results']) && is_array($items_returned['results'])) {
    $items = $items_returned['results'];
  }

  $rows = array(
    'all_day' => array(),
    'range' => array(),
    'slots_max' => 0,
  );

  // Set range at 1-hour intervals.
  $range_interval = 3600; // 3600 = +1 hour

  $current_time = $day_start;
  while ($current_time < $day_stop) {
    $rows['range'][$current_time] = array();
    $current_time += $range_interval;
  }
  unset($current_time);

  $processed_items = array();

  if (!empty($items)) {
    foreach ($items as $item) {
      foreach ($item->date as $delta => $date) {
        if ($date < $day_start || $date >= $day_stop) {
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

        if (is_null($item->time_start[$delta])) {
          if (!isset($rows['all_day'][$date])) {
            $rows['all_day'][$date] = array();
          }

          $rows['all_day'][$date][$item->id] = array(
            'id' => $item->id,
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
          // floor() and ceil() to nearest hours.
          $time_offset_start = $item->time_start[$delta] % $range_interval;
          $time_offset_stop = $item->time_stop[$delta] % $range_interval;
          $time_floor = $item->time_start[$delta] - $time_offset_start;
          $time_ceiling = $item->time_stop[$delta] - $time_offset_stop;
          if ($time_offset_stop > 0) {
            $time_ceiling += $range_interval;
          }
          $time_zindex = 100 + (int) date("G", $time_floor);

          // @fixme: Filter out overlapping times. This needs to be fixed so that the use case can never happen. (result of a bug on insert, not checking to see if multiple dates overlap.)
          if (isset($processed_items[$item->id][$time_floor])) {
            continue;
          }

          // preprocess the slots in an attempt to find the leftmost slot that is available for all date slots.
          $time_slot = 0;
          do {
            $time_slot_found = TRUE;
            $time_count = $time_floor;
            while ($time_count < $time_ceiling) {
              while (!empty($rows['range'][$time_count][$time_slot])) {
                $time_slot_found = FALSE;
                break;
              }

              $time_count += $range_interval;
            }

            if ($time_slot_found) {
              break;
            }

            $time_slot++;
          } while (TRUE);

          $time_count = $time_floor;
          while ($time_count < $time_ceiling) {
            $processed_items[$item->id][$time_count] = $time_slot;

            $rows['range'][$time_count][$time_slot][$item->id] = array(
              'id' => $item->id,
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
              'time_start' => $item->time_start[$delta],
              'time_stop' => $item->time_stop[$delta],
              'time_floor' => $time_floor,
              'time_ceiling' => $time_ceiling,
              'slot_start' => FALSE,
              'slot_stop' => FALSE,
              'offset_start' => $time_offset_start,
              'offset_stop' => $time_offset_stop,
              'z-index' => $time_zindex,
            );

            if ($time_count == $time_floor) {
              $rows['range'][$time_count][$time_slot][$item->id]['slot_start'] = TRUE;
            }

            if (($time_count + $range_interval) == $time_ceiling) {
              $rows['range'][$time_count][$time_slot][$item->id]['slot_stop'] = TRUE;
            }

            $time_count += $range_interval;
          }

          $slots_max = count($rows['range'][$time_floor]);
          if ($rows['slots_max'] < $slots_max) {
            $rows['slots_max'] = $slots_max;
          }
        }
      }
    }
  }

  foreach ($rows['range'] as $time_floor => &$time_slots) {
    ksort($time_slots);
  }


  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $markup = '';
  $title = '<h3>' . "Requests for " . '<a href="' . $base_path . 'requests/calendar-0/month/' . date("Y/F", $day_start) . $url_arguments . '" title="Requests for ' . date("F, Y", $day_start) . '">' . date("F", $day_start) . '</a> ' . date("j, Y", $day_start) . ' (Page #' . ($page_number + 1) . ')</h3>';

  $result = mfcs_build_calendar_day_markup($rows, $day_start, $day_stop, $title, $page_number, 'mfcs-calendar-0-day');
  if ($result !== FALSE) {
    $markup .= $result;
  }
  unset($result);

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
