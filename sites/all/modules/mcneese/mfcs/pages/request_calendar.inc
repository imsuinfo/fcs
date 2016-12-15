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

  $instance = mfcs_instance();

  if (is_null($year)) {
    $year = date('Y', $instance);
    $month = strtolower(date('F', $instance));
  }
  elseif (is_null($month)) {
    $month = 'january';
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

  if (!is_null($month) && !in_array(strtolower($month), $valid_month_names)) {
    if (cf_is_integer($month)) {
      $month = (int) $month;

      if (array_key_exists($month, $valid_month_names)) {
        $month = $valid_month_names[$month];
      }
      else {
        cf_error::invalid_variable('month', 'Must be a valid month name.');

        drupal_not_found();
        drupal_exit();
      }
    }
    else {
      cf_error::invalid_variable('month', 'Must be a valid month name.');

      drupal_not_found();
      drupal_exit();
    }
  }

  global $base_path;
  global $mfcs_determined;

  $user = cf_current_user();


  // load customization options.
  $show_holidays = TRUE;
  if (isset($user->mfcs) && is_array($user->mfcs) && array_key_exists('calendar_holidays', $user->mfcs) && $user->mfcs['calendar_holidays'] === FALSE) {
    $show_holidays = FALSE;
  }

  $show_unavailabilities = TRUE;
  if (isset($user->mfcs) && is_array($user->mfcs) && array_key_exists('calendar_unavailabilities', $user->mfcs) && $user->mfcs['calendar_unavailabilities'] === FALSE) {
    $show_unavailabilities = FALSE;
  }


  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

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

  $page_title = 'Request Calendar - ' . date('F Y', $current_date['month_stamp']);
  drupal_set_title($page_title);

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

  $room_ids = NULL;
  if (!empty($rooms)) {
    $room_ids = array_keys($rooms);
  }

  $search_building_ids = NULL;
  if (!empty($mfcs_determined['filters']['field_top-building-0']['search'])) {
    $search_building_ids = $mfcs_determined['filters']['field_top-building-0']['search'];
  }


  // conditionally load holidays.
  $holidays = NULL;
  $show_holidays = TRUE;
  if (isset($user->mfcs['calendar_holidays']) && $user->mfcs['calendar_holidays'] === FALSE) {
    $show_holidays = FALSE;
  }

  if ($show_holidays) {
    $holidays = mfcs_get_holidays($year, $absolute_start, $absolute_stop, TRUE, FALSE, TRUE);
  }

  if (!is_array($holidays)) {
    $holidays = array();
  }


  // conditionally load unavailabilities.
  $unavailabilities = NULL;
  $show_unavailabilities = TRUE;
  if (isset($user->mfcs['calendar_unavailabilities']) && $user->mfcs['calendar_unavailabilities'] === FALSE) {
    $show_unavailabilities = FALSE;
  }

  if ($show_unavailabilities) {
    $unavailabilities = mfcs_get_unavailabilities($year, NULL, $search_building_ids, $absolute_start, $absolute_stop, TRUE, FALSE, TRUE);
  }

  if (!is_array($unavailabilities)) {
    $unavailabilities = array();
  }


  $unavailability_type_options = mfcs_get_unavailability_type_list_options('combined', TRUE);

  // Holidays and Unavailabilities are setup on a per-year basis.
  // If absolute_start is a previous or next year, additional function calls are required.
  $year_start = strtotime('midnight january 1 ' . $year);
  $year_stop = strtotime('next year', $year_start);
  if ($absolute_start < $year_start) {
    if ($show_holidays) {
      $holidays_previous = mfcs_get_holidays($year - 1, $absolute_start, $year_start);
      if (is_array($holidays_previous)) {
        foreach ($holidays_previous as $holiday_key => $holiday_value) {
          $holidays[$holiday_key] = $holiday_value;
        }
      }
      unset($holiday_key);
      unset($holiday_value);
      unset($holidays_previous);
    }

    if ($show_unavailabilities) {
      $unavailabilities_previous = mfcs_get_unavailabilities($year - 1, NULL, $search_building_ids, $absolute_start, $year_start, TRUE, FALSE, TRUE);
      if (is_array($unavailabilities_previous)) {
        foreach ($unavailabilities_previous as $unavailabilities_key => $unavailabilities_value) {
          $holidays[$unavailabilities_key] = $unavailabilities_value;
        }
      }
      unset($unavailabilities_key);
      unset($unavailabilities_value);
      unset($unavailabilities_previous);
    }
  }

  if ($absolute_stop > $year_stop) {
    if ($show_holidays) {
      $holidays_next = mfcs_get_holidays($year + 1, $year_stop, $absolute_stop);
      if (is_array($holidays_next)) {
        foreach ($holidays_next as $holiday_key => $holiday_value) {
          $holidays[$holiday_key] = $holiday_value;
        }
      }
      unset($holiday_key);
      unset($holiday_value);
      unset($holidays_next);
    }

    if ($show_unavailabilities) {
      $unavailabilities_next = mfcs_get_unavailabilities($year + 1, NULL, $search_building_ids, $year_stop, $absolute_stop, TRUE, FALSE, TRUE);
      if (is_array($unavailabilities_next)) {
        foreach ($unavailabilities_next as $unavailabilities_key => $unavailabilities_value) {
          $holidays[$unavailabilities_key] = $unavailabilities_value;
        }
      }
      unset($unavailabilities_key);
      unset($unavailabilities_value);
      unset($unavailabilities_next);
    }
  }

  foreach ($holidays as $holiday) {
    $day = date('j', $holiday->date);
    $month = date('n', $holiday->date);
    $month_day = $month . '-' . $day;
    if (!isset($rows[$month_day])) {
      $rows[$month_day] = array();
    }

    $rows[$month_day]['holiday-' . $holiday->id] = array(
      'is' => 'holiday',
      'id' => 'holiday-' . $holiday->id,
      'title' => check_plain($holiday->title),
      'title-raw' => $holiday->title,
      'type' => $holiday->type,
      'type_name' => $holiday->type_human_name,
      'tooltip' => str_replace('"', '&quot;', check_plain($holiday->description)),
      'year' => $year,
      'month' => $month,
      'day' => $day,
      'date' => $holiday->date,
    );
  }
  unset($holiday);
  unset($holidays);

  foreach ($unavailabilities as $unavailability) {
    $building_code = check_plain($unavailability->building_code);
    $building_name = check_plain($unavailability->building_name);
    $room_number = check_plain($unavailability->room_number);
    $room_name = check_plain($unavailability->room_name);
    $description = check_plain($unavailability->description);
    $tooltip = $building_code . ' ' . $room_number . ': ' . $description;

    if ($unavailability->type == MFCS_UNAVAILABILITY_TYPE_CLASS) {
      $tooltip = $building_code . ' ' . $room_number . ': unavailable due to class ' . check_plain($unavailability->title) . '.';
    }

    if ($unavailability->date_start == $unavailability->date_stop) {
      $day = date('j', $unavailability->date_start);
      $month = date('n', $unavailability->date_start);
      $month_day = $month . '-' . $day;
      if (!isset($rows[$month_day])) {
        $rows[$month_day] = array();
      }

      $rows[$month_day]['unavailability-' . $unavailability->id] = array(
        'is' => 'unavailable',
        'id' => 'unavailability-' . ((int) $unavailability->id),
        'href' => $base_path . 'requests/unavailable-0/view/' . $year . '/' . $unavailability->room_id . '/' . $unavailability->id,
        'title' => check_plain($unavailability->title),
        'title-raw' => $unavailability->title,
        'tooltip' => str_replace('"', '&quot;', $tooltip),
        'description' => str_replace("\n", '<br>', $description),
        'year' => $year,
        'month' => $month,
        'day' => $day,
        'date' => $unavailability->date_start,
        'building' => $unavailability->building_id,
        'room' => $unavailability->room_id,
        'building_code' => $building_code,
        'building_name' => $building_name,
        'room_number' => $room_number,
        'room_name' => $room_name,
        'type' => $unavailability->type,
        'type_name' => isset($unavailability_type_options[$unavailability->type]) ? $unavailability_type_options[$unavailability->type] : 'Unknown',
      );
    }
    else {
      $date_current = $unavailability->date_start;
      while ($date_current < $unavailability->date_stop) {
        $day = date('j', $date_current);
        $month = date('n', $date_current);
        $month_day = $month . '-' . $day;
        if (!isset($rows[$month_day])) {
          $rows[$month_day] = array();
        }

        $rows[$month_day]['unavailability-' . $unavailability->id] = array(
          'is' => 'unavailable',
          'id' => 'unavailability-' . ((int) $unavailability->id),
          'href' => $base_path . 'requests/unavailable-0/view/' . $year . '/' . $unavailability->room_id . '/' . $unavailability->id,
          'title' => check_plain($unavailability->title),
          'title-raw' => $unavailability->title,
          'tooltip' => str_replace('"', '&quot;', $tooltip),
          'description' => str_replace("\n", '<br>', $description),
          'year' => $year,
          'month' => $month,
          'day' => $day,
          'date' => $date_current,
          'date_range_start' => $unavailability->date_start,
          'date_range_stop' => $unavailability->date_stop,
          'building' => $unavailability->building_id,
          'room' => $unavailability->room_id,
          'building_code' => $building_code,
          'building_name' => $building_name,
          'room_number' => $room_number,
          'room_name' => $room_name,
          'type' => $unavailability->type,
          'type_name' => isset($unavailability_type_options[$unavailability->type]) ? $unavailability_type_options[$unavailability->type] : 'Unknown',
        );

        $date_current = strtotime('midnight tomorrow', $date_current);
      }
    }
    unset($date_current);
    unset($building_code);
    unset($building_name);
    unset($room_number);
    unset($room_name);
    unset($description);
    unset($tooltip);
  }

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
        if (!isset($item->date)) {
          watchdog(MFCS_WATCHDOG_ID, 'Unable to find \'date\' property in for item, skipping. item = @item.', array('@item' => print_r($item, TRUE)), WATCHDOG_ERROR);
          continue;
        }

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
            $rows[$month_day][$item->id]['times'][$delta]['start'] = $item->time_start[$delta];
            $rows[$month_day][$item->id]['times'][$delta]['stop'] = $item->time_stop[$delta];
          }
          else {
            $room_number = '';
            $room_name = '';
            if (array_key_exists($item->room, $rooms)) {
              $room_number = check_plain($rooms[$item->room]->room_number);
              $room_name = check_plain($rooms[$item->room]->room_name);
            }

            $building_code = '';
            $building_name = '';
            if (array_key_exists($item->building, $buildings)) {
              $building_code = check_plain($buildings[$item->building]->building_code);
              $building_name = check_plain($buildings[$item->building]->building_name);
            }

            $month_day_count = count($rows[$month_day]);
            if ($options['limit'] > 0 && $month_day_count == $options['limit']) {
              $month_name = date('F', $date);
              $month_name_lower = strtolower($month_name);

              $rows[$month_day]['more'] = array(
                'is' => 'more',
                'href' => $base_path . 'requests/calendar-0/day/' . $year . '/' . $month_name_lower . '/' . $day . $url_arguments,
                'title' => '. . .',
                'title-raw' => '. . .',
                'tooltip' => 'View more requests on ' . $month_name . ' ' . $day . ', ' . $year,
                'date' => $date,
                'month' => $month,
                'day' => $day,
                'times' => array(),
              );

              $rows[$month_day][$item->id]['times'][$delta]['start'] = $item->time_start[$delta];
              $rows[$month_day][$item->id]['times'][$delta]['stop'] = $item->time_stop[$delta];

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
                'is' => 'event',
                'href' => $base_path . 'requests/view-0/' . $item->id . $url_arguments,
                'title' => $item_title,
                'title-raw' => $item->title,
                'tooltip' => '[' . $item->id . '] ' . $building_code . ' ' . $room_number . ': ' . $item_title,
                'date' => $date,
                'year' => $year,
                'month' => $month,
                'day' => date('j', $date),
                'times' => array(),
                'location' => $location,
                'building' => $building,
                'room' => $room,
                'type' => $type,
                'building_code' => $building_code,
                'building_name' => $building_name,
                'room_number' => $room_number,
                'room_name' => $room_name,
              );
              unset($item_title);
            }

            $rows[$month_day][$item->id]['times'][$delta]['start'] = $item->time_start[$delta];
            $rows[$month_day][$item->id]['times'][$delta]['stop'] = $item->time_stop[$delta];
          }
        }
      }
    }

    $current_date = $next_date;
  }

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $markup = '';
  $title = '<h3>' . 'Requests for ' . date('F, Y', $month_start) . '</h3>';
  $result = mfcs_build_calendar_month_markup($rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, 'mfcs-calendar-0-month');
  if ($result !== FALSE) {
    $markup .= $result;
  }
  unset($result);

  return $markup;
}

/**
 * Provides PDF of the facilities use request calendar month page.
 *
 * @param int $year
 *   The year.
 * @param string $month
 *   The month.
 *   Causes $year to be required.
 *
 * @return exit
 *   This function should not return.
 */
function mfcs_request_calendar_month_0_page_pdf($year, $month) {
  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_string($month) && !cf_is_integer($month)) {
    cf_error::invalid_integer('month');
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

  if (!is_null($month) && !in_array(strtolower($month), $valid_month_names)) {
    if (cf_is_integer($month)) {
      $month = (int) $month;

      if (array_key_exists($month, $valid_month_names)) {
        $month = $valid_month_names[$month];
      }
      else {
        cf_error::invalid_variable('month', 'Must be a valid month name.');

        drupal_not_found();
        drupal_exit();
      }
    }
    else {
      cf_error::invalid_variable('month', 'Must be a valid month name.');

      drupal_not_found();
      drupal_exit();
    }
  }

  $month_stamp = strtotime($month . ' 1, ' . $year);

  mfcs_add_canonical_headers('pdf-0/calendar/' . $year . '/' . $month);

  $page_title = 'Request Calendar - ' . date('F Y', $month_stamp);
  drupal_set_title($page_title);

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mfcs_request-calendar_month-0-page-pdf',
      'class' => array(
        'mfcs_request-calendar_month-0-page-pdf',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $markup = mfcs_request_calendar_month_0_page($year, $month);
  if ($markup === FALSE) {
    drupal_not_found();
    drupal_exit();
  }

  $page['html']['content'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  unset($markup);

  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array(
    'page_title' => $page_title,
    'title' => $page_title,
  );

  $contained = mfcs_output_print_page_view($html, $parameters);

  if (!is_string($contained)) {
    drupal_not_found();
    drupal_exit();
  }

  $result = mfcs_request_pdf_0_page_using_wkhtmltopdf_generic('fcs-calendar-month-' . $year . '-' . $month . '.pdf', $contained, FALSE);
  if ($result) {
    exit();
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * Provides the facilities use request calendar day page.
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

  $instance = mfcs_instance();

  if (is_null($year)) {
    $year = date('Y', $instance);
    $month = strtolower(date('F', $instance));
    $day = date('d', $instance);
  }
  elseif (is_null($month)) {
    $month = 'january';
    $day = 1;
  }
  elseif (is_null($day)) {
    $day = 1;
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

  if (!is_null($month) && !in_array(strtolower($month), $valid_month_names)) {
    if (cf_is_integer($month)) {
      $month = (int) $month;

      if (array_key_exists($month, $valid_month_names)) {
        $month = $valid_month_names[$month];
      }
      else {
        cf_error::invalid_variable('month', 'Must be a valid month name.');

        drupal_not_found();
        drupal_exit();
      }
    }
    else {
      cf_error::invalid_variable('month', 'Must be a valid month name.');

      drupal_not_found();
      drupal_exit();
    }
  }

  if (!is_null($day) && ($day < 1 || $day > 31)) {
    cf_error::invalid_variable('day', 'Must be a valid day number.');

    drupal_not_found();
    drupal_exit();
  }

  global $base_path;
  global $mfcs_determined;

  $user = cf_current_user();


  // load customization options.
  $show_holidays = TRUE;
  if (isset($user->mfcs) && is_array($user->mfcs) && array_key_exists('calendar_holidays', $user->mfcs) && $user->mfcs['calendar_holidays'] === FALSE) {
    $show_holidays = FALSE;
  }

  $show_unavailabilities = TRUE;
  if (isset($user->mfcs) && is_array($user->mfcs) && array_key_exists('calendar_unavailabilities', $user->mfcs) && $user->mfcs['calendar_unavailabilities'] === FALSE) {
    $show_unavailabilities = FALSE;
  }


  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

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

  $page_title = 'Request Calendar - ' . date('F j, Y', $day_start);
  drupal_set_title($page_title);

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

  $room_ids = NULL;
  if (!empty($rooms)) {
    $room_ids = array_keys($rooms);
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


  // conditionally load holidays.
  $holidays = NULL;
  $show_holidays = TRUE;
  if (isset($user->mfcs['calendar_holidays']) && $user->mfcs['calendar_holidays'] === FALSE) {
    $show_holidays = FALSE;
  }

  if ($show_holidays) {
    $holidays = mfcs_get_holidays($year, $day_start, $day_stop, TRUE, FALSE, TRUE);
  }

  if (!is_array($holidays)) {
    $holidays = array();
  }

  foreach ($holidays as $holiday) {
    $date = strtotime('midnight today', $holiday->date);

    if (!isset($rows['all_day'][$date])) {
      $rows['all_day'][$date] = array();
    }

    $description = check_plain($holiday->description);
    $rows['all_day'][$date]['holiday-' . $holiday->id] = array(
      'is' => 'holiday',
      'id' => 'holiday-' . $holiday->id,
      'title' => check_plain($holiday->title),
      'title-raw' => $holiday->title,
      'type' => $holiday->type,
      'type_name' => $holiday->type_human_name,
      'tooltip' => str_replace('"', '&quot;', $description),
      'description' => str_replace("\n", '<br>', $description),
      'year' => $year,
      'month' => $month,
      'day' => $day,
      'date' => $holiday->date,
    );
    unset($description);
  }
  unset($holidays);
  unset($date);

  $search_building_ids = NULL;
  if (!empty($mfcs_determined['filters']['field_top-building-0']['search'])) {
    $search_building_ids = $mfcs_determined['filters']['field_top-building-0']['search'];
  }

  $processed_items = array();

  if (!empty($items)) {
    foreach ($items as $item) {
      if (!isset($item->date)) {
        watchdog(MFCS_WATCHDOG_ID, 'Unable to find \'date\' property in for item, skipping. item = @item.', array('@item' => print_r($item, TRUE)), WATCHDOG_ERROR);
        continue;
      }

      foreach ($item->date as $delta => $date) {
        if ($date < $day_start || $date >= $day_stop) {
          continue;
        }

        $item_title = check_plain($item->title);

        $room_number = '';
        $room_name = '';
        if (array_key_exists($item->room, $rooms)) {
          $room_number = check_plain($rooms[$item->room]->room_number);
          $room_name = check_plain($rooms[$item->room]->room_name);
        }

        $building_code = '';
        $building_name = '';
        if (array_key_exists($item->building, $buildings)) {
          $building_code = check_plain($buildings[$item->building]->building_code);
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
            'is' => 'event',
            'id' => $item->id,
            'href' => $base_path . 'requests/view-0/' . $item->id . $url_arguments,
            'title' => $item_title,
            'title-raw' => $item->title,
            'tooltip' => '[' . $item->id . '] ' . $building_code . ' ' . $room_number . ': ' . $item_title,
            'date' => $date,
            'year' => $year,
            'month' => $month,
            'day' => date('j', $date),
            'location' => $location,
            'building' => $building,
            'room' => $room,
            'type' => $type,
            'building_code' => $building_code,
            'building_name' => $building_name,
            'room_number' => $room_number,
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
          $time_zindex = 100 + (int) date('G', $time_floor);

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
              'is' => 'event',
              'id' => $item->id,
              'href' => $base_path . 'requests/view-0/' . $item->id . $url_arguments,
              'title' => $item_title,
              'title-raw' => $item->title,
              'tooltip' => '[' . $item->id . '] ' . $building_code . ' ' . $room_number . ': ' . $item_title,
              'date' => $date,
              'year' => $year,
              'month' => $month,
              'day' => date('j', $date),
              'location' => $location,
              'building' => $building,
              'room' => $room,
              'type' => $type,
              'building_code' => $building_code,
              'building_name' => $building_name,
              'room_number' => $room_number,
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

            $slots_max = count($rows['range'][$time_count]);
            if ($rows['slots_max'] < $slots_max) {
              $rows['slots_max'] = $slots_max;
            }

            $time_count += $range_interval;
          }
        }
      }
    }
  }


  // conditionally load unavailabilities.
  $unavailabilities = NULL;
  $show_unavailabilities = TRUE;
  if (isset($user->mfcs['calendar_unavailabilities']) && $user->mfcs['calendar_unavailabilities'] === FALSE) {
    $show_unavailabilities = FALSE;
  }

  if ($show_unavailabilities) {
    $unavailabilities = mfcs_get_unavailabilities($year, NULL, $search_building_ids, $day_start, $day_stop, TRUE, FALSE, TRUE);
  }

  if (!is_array($unavailabilities)) {
    $unavailabilities = array();
  }

  $unavailability_type_options = mfcs_get_unavailability_type_list_options('combined', TRUE);
  foreach ($unavailabilities as $unavailability) {
    $building_code = check_plain($unavailability->building_code);
    $building_name = check_plain($unavailability->building_name);
    $room_number = check_plain($unavailability->room_number);
    $room_name = check_plain($unavailability->room_name);
    $description = check_plain($unavailability->description);
    $tooltip = $building_code . ' ' . $room_number . ': ' . $description;

    if ($unavailability->type == MFCS_UNAVAILABILITY_TYPE_CLASS) {
      $tooltip = $building_code . ' ' . $room_number . ': unavailable due to class ' . check_plain($unavailability->title) . '.';
    }

    if ($unavailability->date_start == $unavailability->date_stop || ($unavailability->date_start <= $day_start && $unavailability->date_stop >= $day_stop)) {
      $date = strtotime('midnight today', $unavailability->date_start);

      if (!isset($rows['all_day'][$date])) {
        $rows['all_day'][$date] = array();
      }

      $rows['all_day'][$date]['unavailability-' . $unavailability->id] = array(
        'is' => 'unavailable',
        'id' => $unavailability->id,
        'href' => $base_path . 'requests/unavailable-0/view/' . $year . '/' . $unavailability->room_id . '/' . $unavailability->id,
        'title' => check_plain($unavailability->title),
        'title-raw' => $unavailability->title,
        'tooltip' => str_replace('"', '&quot;', $tooltip),
        'description' => str_replace("\n", '<br>', $description),
        'year' => $year,
        'month' => $month,
        'day' => $day,
        'date' => $unavailability->date_start,
        'building' => $unavailability->building_id,
        'room' => $unavailability->room_id,
        'building_code' => $building_code,
        'building_name' => $building_name,
        'room_number' => $room_number,
        'room_name' => $room_name,
        'type' => $unavailability->type,
        'type_name' => isset($unavailability_type_options[$unavailability->type]) ? $unavailability_type_options[$unavailability->type] : 'Unknown',
      );
    }
    else {
      // the dates may extend beyond the current day, therefore determine the nearest time value for the current date.
      $time_start = $unavailability->date_start;
      if ($unavailability->date_start < $day_start) {
        $time_start = $day_start;
      }

      $time_stop = $unavailability->date_stop;
      if ($unavailability->date_stop > $day_stop) {
        $time_stop = $day_stop;
      }

      // floor() and ceil() to nearest hours.
      $time_offset_start = $time_start % $range_interval;
      $time_offset_stop = $time_stop % $range_interval;
      $time_floor = $time_start - $time_offset_start;
      $time_ceiling = $time_stop - $time_offset_stop;
      if ($time_offset_stop > 0) {
        $time_ceiling += $range_interval;
      }
      $time_zindex = 100 + (int) date('G', $time_floor);

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
        $rows['range'][$time_count][$time_slot]['unavailable-' . $unavailability->id] = array(
          'is' => 'unavailable',
          'id' => $unavailability->id,
          'href' => $base_path . 'requests/unavailable-0/view/' . $year . '/' . $unavailability->room_id . '/' . $unavailability->id,
          'title' => check_plain($unavailability->title),
          'title-raw' => $unavailability->title,
          'tooltip' => str_replace('"', '&quot;', $tooltip),
          'description' => str_replace("\n", '<br>', $description),
          'year' => $year,
          'month' => $month,
          'day' => $day,
          'date' => $time_start,
          'building' => $unavailability->building_id,
          'room' => $unavailability->room_id,
          'building_code' => $building_code,
          'building_name' => $building_name,
          'room_number' => $room_number,
          'room_name' => $room_name,
          'time_start' => $time_start,
          'time_stop' => $time_stop,
          'time_floor' => $time_floor,
          'time_ceiling' => $time_ceiling,
          'slot_start' => FALSE,
          'slot_stop' => FALSE,
          'offset_start' => $time_offset_start,
          'offset_stop' => $time_offset_stop,
          'z-index' => $time_zindex,
          'type' => $unavailability->type,
          'type_name' => isset($unavailability_type_options[$unavailability->type]) ? $unavailability_type_options[$unavailability->type] : 'Unknown',
        );

        if ($time_count == $time_floor) {
          $rows['range'][$time_count][$time_slot]['unavailable-' . $unavailability->id]['slot_start'] = TRUE;
        }

        if (($time_count + $range_interval) == $time_ceiling) {
          $rows['range'][$time_count][$time_slot]['unavailable-' . $unavailability->id]['slot_stop'] = TRUE;
        }

        $slots_max = count($rows['range'][$time_count]);
        if ($rows['slots_max'] < $slots_max) {
          $rows['slots_max'] = $slots_max;
        }

        $time_count += $range_interval;
      }

      unset($time_start);
      unset($time_stop);
    }
  }
  unset($date_current);
  unset($building_code);
  unset($building_name);
  unset($room_number);
  unset($room_name);
  unset($description);
  unset($tooltip);

  foreach ($rows['range'] as $time_floor => &$time_slots) {
    ksort($time_slots);
  }


  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $markup = '';
  $title = '<h3>' . 'Requests for ' . date('l', $day_start) . ', <a href="' . $base_path . 'requests/calendar-0/month/' . date('Y/F', $day_start) . $url_arguments . '" title="Requests for ' . date('F, Y', $day_start) . '">' . date('F', $day_start) . '</a> ' . date('j, Y', $day_start) . ' (Page #' . ($page_number + 1) . ')</h3>';

  $result = mfcs_build_calendar_day_markup($rows, $day_start, $day_stop, $title, $page_number, 'mfcs-calendar-0-day');
  if ($result !== FALSE) {
    $markup .= $result;
  }
  unset($result);

  return $markup;
}

/**
 * Provides PDF of the facilities use request calendar day page.
 *
 * @param int $year
 *   The year.
 * @param string $month
 *   The month.
 *   Causes $year to be required.
 * @param string $day
 *   The day.
 *   Causes $year and $month to be required.
 * @param int $page_number
 *   The page number. Requests are broken up into different pages
 *   to better handle large numbers of requests per time slot.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_calendar_day_0_page_pdf($year, $month, $day, $page_number) {
  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_string($month) && !cf_is_integer($month)) {
    cf_error::invalid_integer('month');
    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($day)) {
    cf_error::invalid_integer('day');
    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($page_number)) {
    cf_error::invalid_integer('page_number');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($day) && ($day < 1 || $day > 31)) {
    cf_error::invalid_variable('day', 'Must be a valid day number.');

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

  if (!is_null($month) && !in_array(strtolower($month), $valid_month_names)) {
    if (cf_is_integer($month)) {
      $month = (int) $month;

      if (array_key_exists($month, $valid_month_names)) {
        $month = $valid_month_names[$month];
      }
      else {
        cf_error::invalid_variable('month', 'Must be a valid month name.');

        drupal_not_found();
        drupal_exit();
      }
    }
    else {
      cf_error::invalid_variable('month', 'Must be a valid month name.');

      drupal_not_found();
      drupal_exit();
    }
  }

  $day_stamp = strtotime($month . ' ' . $day . ', ' . $year);

  mfcs_add_canonical_headers('pdf-0/calendar/' . $year . '/' . $month . '/' . $day . '/' . $page_number);

  $page_title = 'Request Calendar - ' . date('F j, Y', $day_stamp);
  drupal_set_title($page_title);

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mfcs_request-calendar_day-0-page-pdf',
      'class' => array(
        'mfcs_request-calendar_day-0-page-pdf',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $markup = mfcs_request_calendar_day_0_page($year, $month, $day, $page_number);
  if ($markup === FALSE) {
    drupal_not_found();
    drupal_exit();
  }

  $page['html']['content'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  unset($markup);

  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array(
    'page_title' => $page_title,
    'title' => $page_title,
  );

  $contained = mfcs_output_print_page_view($html, $parameters);

  if (!is_string($contained)) {
    drupal_not_found();
    drupal_exit();
  }

  $result = mfcs_request_pdf_0_page_using_wkhtmltopdf_generic('fcs-calendar-day-' . $year . '-' . $month . '-' . $day . '.pdf', $contained, FALSE);
  if ($result) {
    exit();
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * @} End of '@addtogroup mfcs'.
 */
