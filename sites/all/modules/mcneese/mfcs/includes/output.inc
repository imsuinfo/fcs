<?php

/**
 * @file
 * Defines McNeese facilities use output functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */


/**
 * Build a month calendar display.
 *
 * This is a wrapper used to determine which calendar theme to use.
 * The specific calendar theme is based on each individual users settings.
 *
 * @param array $items
 *   An array of request items to link to.
 * @param int $month_start
 *   Unix timestamp for the start of the month.
 * @param int $month_stop
 *   Unix timestamp for the first day of the next month.
 * @param int $absolute_start
 *   Unix timestamp for the first day of the week in which the first day of
 *   the month may be found.
 * @param int $absolute_stop
 *   Unix timestamp for the last day of the week in which the last day of
 *   the month may be found.
 * @param string $title
 *   An HTML title for describing the calendar.
 * @param string $id
 *   (optional) A unique HTML id to associate with the calendar.
 * @param int|null $max_items
 *   (optional) Maximum number of items to display before a 'more' links is
 *   added. A value of NULL means no maximum limit.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_month_markup($items, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, $id = NULL, $max_items = NULL) {
  $user = cf_current_user();

  $calendar = MFCS_USER_CALENDAR_ORIGINAL;
  if (isset($user->mfcs['calendar']) && is_numeric($user->mfcs['calendar'])) {
    $calendar = (int) $user->mfcs['calendar'];
  }

  if ($calendar === MFCS_USER_CALENDAR_FULL) {
    $success = TRUE;

    // load the moment library (required by fullcalendar).
    $library = libraries_load('moment');
    if (isset($library['error'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load moment library, reason: @reason.', array('@reason' => $library['error']), WATCHDOG_ERROR);
    }
    elseif (empty($library['loaded'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load moment library, reason: unknown.', array(), WATCHDOG_ERROR);
    }
    unset($library);

    // load the fullcalendar library.
    $library = libraries_load('fullcalendar');
    if (isset($library['error'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load fullcalendar library, reason: @reason.', array('@reason' => $library['error']), WATCHDOG_ERROR);
    }
    elseif (empty($library['loaded'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load fullcalendar library, reason: unknown.', array(), WATCHDOG_ERROR);
    }
    unset($library);

    if ($success) {
      return mfcs_build_calendar_month_markup_full($items, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, $id, $max_items);
    }
  }

  // the original is the default or the fallback theme.
  return mfcs_build_calendar_month_markup_original($items, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, $id, $max_items);
}

/**
 * Build a month calendar display using the original calendar theme.
 *
 * @param array $items
 *   An array of request items to link to.
 * @param int $month_start
 *   Unix timestamp for the start of the month.
 * @param int $month_stop
 *   Unix timestamp for the first day of the next month.
 * @param int $absolute_start
 *   Unix timestamp for the first day of the week in which the first day of
 *   the month may be found.
 * @param int $absolute_stop
 *   Unix timestamp for the last day of the week in which the last day of
 *   the month may be found.
 * @param string $title
 *   An HTML title for describing the calendar.
 * @param string $id
 *   (optional) A unique HTML id to associate with the calendar.
 * @param int|null $max_items
 *   (optional) Maximum number of items to display before a 'more' links is
 *   added. A value of NULL means no maximum limit.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_month_markup_original($items, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, $id = NULL, $max_items = NULL) {
  if (!is_array($items)) {
    cf_error::invalid_array('items');
    return FALSE;
  }

  if (!cf_is_integer($month_start)) {
    cf_error::invalid_integer('month_start');
    return FALSE;
  }

  if (!cf_is_integer($month_stop)) {
    cf_error::invalid_integer('month_stop');
    return FALSE;
  }

  if (!cf_is_integer($absolute_start)) {
    cf_error::invalid_integer('absolute_start');
    return FALSE;
  }

  if (!cf_is_integer($absolute_stop)) {
    cf_error::invalid_integer('absolute_stop');
    return FALSE;
  }

  if (!is_string($title)) {
    cf_error::invalid_string('title');
    return FALSE;
  }

  $id_string = '';
  if (!empty($id)) {
    if (!is_string($id)) {
      cf_error::invalid_string('id');
      return FALSE;
    }

    $id_string = ' id="' . check_plain($id) . '"';
  }

  if (!is_null($max_items) && !cf_is_integer($max_items)) {
    cf_error::invalid_integer('max_items');
    $max_items = NULL;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $week_start = 0;
  $week_stop = 6;

  // for ISO-8601.
  #$week_start = 1;
  #$week_stop = 7;

  $days_of_week = array(
    0 => 'Sunday',
    1 => 'Monday',
    2 => 'Tuesday',
    3 => 'Wednesday',
    4 => 'Thursday',
    5 => 'Friday',
    6 => 'Saturday',
    7 => 'Sunday', // In the case of ISO-8601, Sunday = 7.
  );


  // begin wrappers
  $markup = '<div' . $id_string . ' class="calendar-month-wrapper"><div class="calendar-month">';


  // title
  $markup .= '<div class="calendar-title">' . $title . '</div>';


  // navigation
  $last_month = strtotime('midnight first day of last month', $month_start);
  $next_month = strtotime('midnight first day of next month', $month_start);

  $url_base_month = $base_path . 'requests/calendar-0/month';
  $url_base_day = $base_path . 'requests/calendar-0/day';

  $last_url = $url_base_month . '/' . date('Y', $last_month) . '/' . strtolower(date('F', $last_month)) . $url_arguments;
  $this_url = $url_base_month . $url_arguments;
  $next_url = $url_base_month . '/' . date('Y', $next_month) . '/' . strtolower(date('F', $next_month)) . $url_arguments;

  $markup .= '<div class="calendar-navigation">';
  $markup .= '<div class="calendar-navigation-previous">';
  $markup .= '<a href="' . $last_url . '" title="Go to ' . date('F Y', $last_month) . '">Previous</a>';
  $markup .= '</div>';

  $instance = mfcs_instance();
  if (date("Y/m", $instance) != date("Y/m", $month_start)) {
    $markup .= '<div class="calendar-navigation-this">';
    $markup .= '<a href="' . $this_url . '" title="Go to Current Month">This Month</a>';
    $markup .= '</div>';
  }

  $markup .= '<div class="calendar-navigation-next">';
  $markup .= '<a href="' . $next_url . '" title="Go to ' . date('F Y', $next_month) . '">Next</a>';
  $markup .= '</div>';
  $markup .= '</div>';


  // header
  $markup .= '<div class="calendar-header-wrapper"><div class="calendar-header">';
  $markup .= '<div class="calendar-week-wrapper week_number-0"><div class="calendar-week week_number-0">';
  $week_count = $week_start;
  while ($week_count <= $week_stop) {
    $markup .= '<div class="calendar-day-wrapper weekday-' . $week_count . '">';
    $markup .= '<div class="calendar-day weekday-' . $week_count . '">' . $days_of_week[$week_count] . '</div>';
    $markup .= '</div>';
    $week_count++;
  }
  $markup .= '</div></div></div></div>';


  // body
  $markup .= '<div class="calendar-body-wrapper"><div class="calendar-body">';
  $current_date = $absolute_start;
  $week_first = date('W', $current_date);
  $week_count = $week_start;

  $this_year = date('Y', $instance);
  $this_month = date('n', $instance);
  $this_day = date('j', $instance);

  $weeks_processed = array();
  while ($current_date < $absolute_stop) {
    $current_year = date('Y', $current_date);
    $current_week = date('W', $current_date);
    $current_month = date('n', $current_date);
    $current_month_name = strtolower(date('F', $current_date));
    $current_day = date('j', $current_date);
    $current_month_day = $current_month . '-' . $current_day;

    if ($week_count == $week_start && !isset($weeks_processed[$week_first])) {
      $markup .= '<div class="calendar-week-wrapper week_number-' . $current_week . '"><div class="calendar-week week_number-' . $current_week . '">';
      $weeks_processed[$week_first] = TRUE;
    }

    $extra_class = ' month-current';
    if ($current_date < $month_start) {
      $extra_class = ' month-previous';
    }
    elseif ($current_date >= $month_stop) {
      $extra_class = ' month-next';
    }

    if ($current_year == $this_year && $current_month == $this_month && $current_day == $this_day) {
      $extra_class .= ' today';
    }

    $markup .= '<div class="calendar-day-wrapper weekday-' . $week_count . ' day-' . $current_day . ' month-' . $current_month . $extra_class . '">';
    $markup .= '<div class="calendar-day weekday-' . $week_count . ' day-' . $current_day . ' month-' . $current_month . $extra_class . '">';
    $markup .= '<div class="calendar-label"><a href="' . $url_base_day . '/' . $current_year . '/' . $current_month_name . '/' . $current_day . $url_arguments. '">' . $current_day . '</a></div>';
    if (!empty($items[$current_month_day])) {
      $items_count = 0;
      if (is_null($max_items)) {
        foreach ($items[$current_month_day] as $item_id => $item) {
          if ($item['is'] == 'holiday') {
            $markup .= '<div class="strong calendar-item is-holiday year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</div>';
          }
          elseif ($item['is'] == 'unavailable') {
            if (is_null($item['type'])) {
              $markup .= '<div class="calendar-item is-unavailable unavailable-notice year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</div>';
            }
            else {
              $markup .= '<div class="calendar-item is-unavailable building-' . $item['building'] . ' room-' . $item['room'] . ' is-unavailable year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '" title="' . $item['tooltip'] . '"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';
            }
          }
          else {
            $markup .= '<div class="calendar-item is-event location-' . $item['location'] . ' building-' . $item['building'] . ' room-' . $item['room'] . ' type-' . $item['type'] . ' year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';
          }
        }
      }
      else {
        $items_count = 0;
        foreach ($items[$current_month_day] as $item_id => $item) {
          if ($item['is'] == 'holiday') {
            $markup .= '<div class="strong calendar-item is-holiday year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</div>';
          }
          elseif ($item['is'] == 'unavailable') {
            if (is_null($item['type'])) {
              $markup .= '<div class="calendar-item is-unavailable unavailable-notice year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</div>';
            }
            else {
              $markup .= '<div class="calendar-item is-unavailable building-' . $item['building'] . ' room-' . $item['room'] . ' is-unavailable year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '" title="' . $item['tooltip'] . '"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';
            }
          }
          else {
            $markup .= '<div class="calendar-item is-event location-' . $item['location'] . ' building-' . $item['building'] . ' room-' . $item['room'] . ' type-' . $item['type'] . ' year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';

            $items_count++;
            if ($items_count == $max_items) {
              $markup .= '<div class="calendar-item item-more year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'] . '"><a class="calendar-item-link" href="' . $url_base_day . '/' . $current_year . '/' . $current_month_name . '/' . $current_day . $url_arguments . '" title="View more.">More...</a></div>';
              break;
            }
          }
        }
      }
    }
    $markup .= '</div></div>';

    $current_date = strtotime('midnight tomorrow', $current_date);

    $week_count++;
    if ($week_count > $week_stop) {
      $markup .= '</div></div>';

      $week_first = date('W', $current_date);
      $week_count = $week_start;
    }
  }
  $markup .= '</div></div>';


  // end wrappers (calendar-month)
  $markup .= '</div>';


  // calendar links
  $markup .= '<div id="mfcs-links_and_filters" class="links_and_filters-wrapper no-print">';

  $markup .= '<div class="links_and_filters-group group-1">';
  $markup .= mfcs_build_calendar_ical_links_markup($month_start, TRUE, TRUE, FALSE);
  $markup .= mfcs_build_calendar_openpgp_links_markup();
  $markup .= '</div>'; // filter-group-1

  $markup .= mfcs_build_determined_filters_markup();
  $markup .= '</div>'; // #mfcs-links_and_filters


  // end wrapper (calendar-month-wrapper)
  $markup .= '</div>';

  return $markup;
}

/**
 * Build a month calendar display using the full calendar theme.
 *
 * @param array $items
 *   An array of request items to link to.
 * @param int $month_start
 *   Unix timestamp for the start of the month.
 * @param int $month_stop
 *   Unix timestamp for the first day of the next month.
 * @param int $absolute_start
 *   Unix timestamp for the first day of the week in which the first day of
 *   the month may be found.
 * @param int $absolute_stop
 *   Unix timestamp for the last day of the week in which the last day of
 *   the month may be found.
 * @param string $title
 *   An HTML title for describing the calendar.
 * @param string $id
 *   (optional) A unique HTML id to associate with the calendar.
 * @param int|null $max_items
 *   (optional) Maximum number of items to display before a 'more' links is
 *   added. A value of NULL means no maximum limit.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_month_markup_full($items, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, $id = NULL, $max_items = NULL) {
  if (!is_array($items)) {
    cf_error::invalid_array('items');
    return FALSE;
  }

  if (!cf_is_integer($month_start)) {
    cf_error::invalid_integer('month_start');
    return FALSE;
  }

  if (!cf_is_integer($month_stop)) {
    cf_error::invalid_integer('month_stop');
    return FALSE;
  }

  if (!cf_is_integer($absolute_start)) {
    cf_error::invalid_integer('absolute_start');
    return FALSE;
  }

  if (!cf_is_integer($absolute_stop)) {
    cf_error::invalid_integer('absolute_stop');
    return FALSE;
  }

  if (!is_string($title)) {
    cf_error::invalid_string('title');
    return FALSE;
  }

  // fullcalendar requires an id string to function.
  $id_string = 'mfcs-calendar-0-month';
  if (!empty($id)) {
    if (!is_string($id)) {
      cf_error::invalid_string('id');
      return FALSE;
    }

    $id_string = check_plain($id);
  }

  if (!is_null($max_items) && !cf_is_integer($max_items)) {
    cf_error::invalid_integer('max_items');
    $max_items = NULL;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $room_options = mfcs_get_request_room_list_options(NULL, NULL, FALSE, TRUE);
  $building_options = mfcs_get_request_building_list_options(NULL, NULL, FALSE, TRUE);
  $type_options = mfcs_get_request_type_list_options(NULL, NULL, FALSE, TRUE);

  $instance = mfcs_instance();
  $this_year = date('Y', $instance);
  $this_month = date('n', $instance);
  $this_day = date('j', $instance);


  // navigation
  $last_month = strtotime('midnight first day of last month', $month_start);
  $next_month = strtotime('midnight first day of next month', $month_start);

  $url_base_month = $base_path . 'requests/calendar-0/month';
  $url_base_day = $base_path . 'requests/calendar-0/day';

  $last_url = $url_base_month . '/' . date('Y', $last_month) . '/' . strtolower(date('F', $last_month)) . $url_arguments;
  $this_url = $url_base_month . $url_arguments;
  $next_url = $url_base_month . '/' . date('Y', $next_month) . '/' . strtolower(date('F', $next_month)) . $url_arguments;


  // full calendar javascript.
  $javascript = '(function ($) {' . "\n";
  $javascript .= 'Drupal.behaviors.mfcs_monthly_calendar_fullcalendar = {' . "\n";
  $javascript .= '  attach:function(context) {' . "\n";

  $javascript .= '$(\'#' . $id_string . '\').fullCalendar({' . "\n";

  // add custom buttons.
  $javascript .= '  customButtons: {' . "\n";
  $javascript .= '    previousMonthUrl: {' . "\n";
  $javascript .= '      text: \'Previous\',' . "\n";
  $javascript .= '      click: function() {' . "\n";
  $javascript .= '        location.assign(\'' . $last_url . '\');' . "\n";
  $javascript .= '        return false;' . "\n";
  $javascript .= '      }' . "\n";
  $javascript .= '    },' . "\n";
  $javascript .= '    nextMonthUrl: {' . "\n";
  $javascript .= '      text: \'Next\',' . "\n";
  $javascript .= '      click: function() {' . "\n";
  $javascript .= '        location.assign(\'' . $next_url . '\');' . "\n";
  $javascript .= '        return false;' . "\n";
  $javascript .= '      }' . "\n";
  $javascript .= '    }' . "\n";
  $javascript .= '  },' . "\n";

  // main settings
  $javascript .= '  header: {' . "\n";
  $javascript .= '    left: \'previousMonthUrl,nextMonthUrl\',' . "\n";
  $javascript .= '    center: \'title\',' . "\n";
  $javascript .= '    right: \'month,listMonth\'' . "\n";
  $javascript .= '  },' . "\n";
  $javascript .= '  defaultView: \'month\',' . "\n";
  $javascript .= '  defaultDate: \'' . date('Y-m-d', $month_start) . '\',' . "\n";
  $javascript .= '  height: \'auto\',' . "\n";
  $javascript .= '  contentHeight: \'auto\',' . "\n";
  #$javascript .= '  now: \'' . date('Y-m-d', $instance) . '\',' . "\n";
  $javascript .= '  navLinks: true,' . "\n";
  $javascript .= '  editable: false,' . "\n";
  $javascript .= '  eventLimit: true,' . "\n";
  $javascript .= '  allDaySlot: true,' . "\n";
  $javascript .= '  slotDuration: \'01:00:00\',' . "\n";
  $javascript .= '  slotEventOverlap: true,' . "\n";

  if (is_null($max_items)) {
    $javascript .= '  eventLimit: false,' . "\n";
  }
  else {
    $javascript .= '  eventLimit: ' . $max_items . ',' . "\n";
  }

  // add qtip support.
  $javascript .= '  eventRender: function(event, element) {' . "\n";
  $javascript .= '    element.qtip({' . "\n";
  $javascript .= '      content: { title: event.tooltipTitle, text: event.tooltipText },' . "\n";
  $javascript .= '      style: event.tooltipClass,' . "\n";
  $javascript .= '      position: {' . "\n";
  $javascript .= '        my: \'top center\',' . "\n";
  $javascript .= '        at: \'bottom center\'' . "\n";
  $javascript .= '      }' . "\n";
  $javascript .= '    });' . "\n";
  $javascript .= '  },' . "\n";

  // redesign the day click action.
  $javascript .= '  navLinkDayClick: function(date, jsEvent) {' . "\n";
  $javascript .= '    jsEvent.preventDefault();' . "\n";
  $javascript .= '    location.assign(\'' . $url_base_day . '/\' + date.format(\'Y/MM/D\'));' . "\n";
  $javascript .= '  },' . "\n";

  // add each event.
  $current_date = $absolute_start;

  $javascript_items = NULL;
  $date_range_items = array();
  $space = '      ';
  while ($current_date < $absolute_stop) {
    $current_year = date('Y', $current_date);
    $current_month = date('n', $current_date);
    $current_month_name = strtolower(date('F', $current_date));
    $current_day = date('j', $current_date);
    $current_month_day = $current_month . '-' . $current_day;

    $extra_class = ' month-current';
    if ($current_date < $month_start) {
      $extra_class = ' month-previous';
    }
    elseif ($current_date >= $month_stop) {
      $extra_class = ' month-next';
    }

    if ($current_year == $this_year && $current_month == $this_month && $current_day == $this_day) {
      $extra_class .= ' today';
    }

    if (!empty($items[$current_month_day])) {
      foreach ($items[$current_month_day] as $item_id => $item) {
        $javascript_item_common = '';

        $tooltip_text = '';
        $tooltip_text_suffix = '';
        $tooltip_class = 'tooltip-calendar-monthly tooltip-calendar-item';

        // fullcalendar performs sanitization, so send it raw title text.
        $processed_title_for_fullcalendar = str_replace('\'', '\\\'', $item['title-raw']);

        if ($item['is'] == 'holiday') {
          $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";

          $class_names = array(
            '\'calendar-item\'',
            '\'is-holiday\'',
            '\'year-' . $item['year'] . '\'',
            '\'month-' . $item['month'] . '\'',
            '\'day-' . $item['day'] . '\'',
          );

          if (array_key_exists('type', $item)) {
            $class_names[] = '\'type-' . $item['type'] . '\'';
          }

          $tooltip_class .= ' tooltip-is-holiday year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];

          if (!empty($item['tooltip'])) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-holiday_text">';
            $tooltip_text .= '<span class="tooltip-item-text">' . $item['tooltip'] . '</span>';
            $tooltip_text .= '</div>';
            $tooltip_text .= '<br>';
          }

          $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
        }
        elseif ($item['is'] == 'unavailable') {
          // the data is structured in a way to be compatibile with the original calendar.
          // this does not require multiple separate days, so instead use the entire date range as a single entry and skip all other (now duplicate) entries.
          if (array_key_exists($item['id'], $date_range_items)) {
            continue;
          }

          $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";
          $javascript_item_common .= $space . 'url: \'' . $item['href'] . '\',' . "\n";

          $class_names = array(
            '\'calendar-item\'',
            '\'is-unavailable\'',
            '\'building-' . $item['building'] . '\'',
            '\'room-' . $item['room'] . '\'',
            '\'year-' . $item['year'] . '\'',
            '\'month-' . $item['month'] . '\'',
            '\'day-' . $item['day'] . '\'',
          );

          if (array_key_exists('type', $item)) {
            $class_names[] = '\'type-' . $item['type'] . '\'';
          }

          if (array_key_exists('date_range_start', $item)) {
            $date_range_items[$item['id']] = TRUE;
          }

          $tooltip_class .= ' tooltip-is-unavailable year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];

          if (array_key_exists($item['building'], $building_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-building">';
            $tooltip_text .= '<span class="tooltip-item-name">Building:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($building_options[$item['building']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (array_key_exists($item['room'], $room_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-room">';
            $tooltip_text .= '<span class="tooltip-item-name">Room:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($room_options[$item['room']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (!empty($item['description'])) {
            if (!empty($tooltip_text)) {
              $tooltip_text .= '<br>';
            }

            $tooltip_text .= '<div class="tooltip-item tooltip-item-description">';
            $tooltip_text .= '<span class="tooltip-item-value">' . $item['description'] . '</span>';
            $tooltip_text .= '</div>';
          }

          if (!empty($tooltip_text)) {
            $tooltip_text .= '<br>';
          }

          $tooltip_text_suffix .= '<div class="tooltip-item tooltip-item-footer">';
          $tooltip_text_suffix .= '<span class="tooltip-item-unavailability_text">Room Unavailable</span> ';
          $tooltip_text_suffix .= '<span class="tooltip-item-unavailability_type">' . $item['type_name'] . '</span>';
          $tooltip_text_suffix .= '</div>';

          $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
        }
        elseif ($item['is'] == 'more') {
          // this is not used for full calendar, only for original calendar.
          continue;
        }
        else {
          $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";
          $javascript_item_common .= $space . 'url: \'' . $item['href'] . '\',' . "\n";

          if (array_key_exists($item['building'], $building_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-building">';
            $tooltip_text .= '<span class="tooltip-item-name">Building:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($building_options[$item['building']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (array_key_exists($item['room'], $room_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-room">';
            $tooltip_text .= '<span class="tooltip-item-name">Room:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($room_options[$item['room']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (!empty($tooltip_text)) {
            $tooltip_text .= '<br>';
          }

          $tooltip_text_suffix .= '<div class="tooltip-item tooltip-item-footer">';
          $tooltip_text_suffix .= '<span class="tooltip-item-request_id">' . (int) $item_id . '</span> ';
          $tooltip_text_suffix .= '<span class="tooltip-item-request_type">' . check_plain($type_options[$item['type']]) . '</span>';
          $tooltip_text_suffix .= '</div>';

          $class_names = array(
            '\'calendar-item\'',
            '\'is-event\'',
            '\'location-' . $item['location'] . '\'',
            '\'building-' . $item['building'] . '\'',
            '\'room-' . $item['room'] . '\'',
            '\'type-' . $item['type'] . '\'',
            '\'year-' . $item['year'] . '\'',
            '\'month-' . $item['month'] . '\'',
            '\'day-' . $item['day'] . '\'',
          );

          $tooltip_class .= ' tooltip-is-event year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];
          $tooltip_class .= ' tooltip-location-' . $item['location'] . ' tooltip-building-' . $item['building'] . ' tooltip-room-' . $item['room'];
          $tooltip_class .= ' tooltip-type-' . $item['type'];

          $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
        }

        if (array_key_exists('date_range_start', $item) && $item['date_range_start'] != $item['date_range_stop']) {
          $tooltip_text .= '<div class="tooltip-item tooltip-item-date">';
          $date_range_start_midnight = strtotime('midnight today', $item['date_range_start']);
          $date_range_stop_midnight = strtotime('midnight today', $item['date_range_stop']);

          $all_day = TRUE;
          if ($item['date_range_start'] == $date_range_start_midnight) {
            $tooltip_text .= date(MFCS_DATE_FORMAT_DATE, $item['date_range_start']);
          }
          else {
            $tooltip_text .= date(MFCS_DATE_FORMAT_DATE_TIME, $item['date_range_start']);
            $all_day = FALSE;
          }

          $tooltip_text .= ' to ';

          if ($item['date_range_stop'] == $date_range_stop_midnight) {
            $tooltip_text .= date(MFCS_DATE_FORMAT_DATE, $item['date_range_stop']);
          }
          else {
            $tooltip_text .= date(MFCS_DATE_FORMAT_DATE_TIME, $item['date_range_stop']);
            $all_day = FALSE;
          }

          unset($date_range_start_midnight);
          unset($date_range_stop_midnight);

          $tooltip_text .= '</div>';
          if (!is_null($javascript_items)) {
            $javascript_items .= ',' . "\n";
          }

          $javascript_items .= '    {' . "\n";

          if (is_numeric($item_id)) {
            $javascript_items .= $space . 'id: ' . (int) $item_id . ',' . "\n";
          }
          else {
            $javascript_items .= $space . 'id: \'' . $item_id . '\',' . "\n";
          }

          $javascript_items .= $space . 'start: \'' . date('Y-m-d', $item['date_range_start']) . '\',' . "\n";
          $javascript_items .= $space . 'end: \'' . date('Y-m-d', $item['date_range_stop']) . '\',' . "\n";
          $javascript_items .= $space . 'allDayDefault: ' . ($all_day ? 'true' : 'false') . ',' . "\n";
          $javascript_items .= $space . 'tooltipTitle: \'<strong>' . $item['title'] . '</strong>\',' . "\n";
          $javascript_items .= $space . 'tooltipText: \'' . $tooltip_text . $tooltip_text_suffix . '\',' . "\n";
          $javascript_items .= $space . 'tooltipClass: \'' . $tooltip_class . '\',' . "\n";
          $javascript_items .= $javascript_item_common;
          $javascript_items .= '    }';

          unset($all_day);
        }
        else {
          $tooltip_text .= '<div class="tooltip-item tooltip-item-date">';
          $tooltip_text .= date(MFCS_DATE_FORMAT_DATE, $item['date']);
          $tooltip_text .= '</div>';

          if (empty($item['times'])) {
            if (!is_null($javascript_items)) {
              $javascript_items .= ',' . "\n";
            }

            $javascript_items .= '    {' . "\n";

            if (is_numeric($item_id)) {
              $javascript_items .= $space . 'id: ' . (int) $item_id . ',' . "\n";
            }
            else {
              $javascript_items .= $space . 'id: \'' . $item_id . '\',' . "\n";
            }

            $javascript_items .= $space . 'start: \'' . date('Y-m-d', $item['date']) . '\',' . "\n";
            $javascript_items .= $space . 'end: \'' . date('Y-m-d', strtotime('tomorrow', $item['date'])) . '\',' . "\n";
            $javascript_items .= $space . 'allDayDefault: true,' . "\n";
            $javascript_items .= $space . 'tooltipTitle: \'<strong>' . $item['title'] . '</strong>\',' . "\n";
            $javascript_items .= $space . 'tooltipText: \'' . $tooltip_text . $tooltip_text_suffix . '\',' . "\n";
            $javascript_items .= $space . 'tooltipClass: \'' . $tooltip_class . '\',' . "\n";
            $javascript_items .= $javascript_item_common;
            $javascript_items .= '    }';
          }
          else {
            foreach ($item['times'] as $delta => $times) {
              if (!is_null($javascript_items)) {
                $javascript_items .= ',' . "\n";
              }

              $tooltip_text .= '<div class="tooltip-item tooltip-item-times tooltip-item-time-' . $delta . '">';
              $tooltip_text .= date(MFCS_DATE_FORMAT_TIME, $times['start']) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $times['stop']);
              $tooltip_text .= '</div>';

              $javascript_items .= '    {' . "\n";

              if (is_numeric($item_id)) {
                $javascript_items .= $space . 'id: ' . (int) $item_id . ',' . "\n";
              }
              else {
                $javascript_items .= $space . 'id: \'' . $item_id . '\',' . "\n";
              }

              $javascript_items .= $space . 'start: \'' . date('c', $times['start']) . '\',' . "\n";
              $javascript_items .= $space . 'end: \'' . date('c', $times['stop']) . '\',' . "\n";
              $javascript_items .= $space . 'allDayDefault: false,' . "\n";
              $javascript_items .= $space . 'tooltipTitle: \'<strong>' . $item['title'] . '</strong>\',' . "\n";
              $javascript_items .= $space . 'tooltipText: \'' . $tooltip_text . $tooltip_text_suffix . '\',' . "\n";
              $javascript_items .= $space . 'tooltipClass: \'' . $tooltip_class . '\',' . "\n";
              $javascript_items .= $javascript_item_common;
              $javascript_items .= '    }';
            }
          }
        }

        unset($javascript_item_common);
      }
    }

    $current_date = strtotime('midnight tomorrow', $current_date);
  }

  if (!is_null($javascript_items)) {
    $javascript .= '  events: [' . "\n" . $javascript_items . "\n" . '  ]' . "\n";
  }

  $javascript .= '});' . "\n";
  $javascript .= '    }' . "\n";
  $javascript .= '  }' . "\n";
  $javascript .= '})(jQuery);' . "\n";


  drupal_add_js($javascript, array('type' => 'inline', 'group' => JS_DEFAULT, 'preprocess' => FALSE, 'cache' => FALSE));


  // full calendar markup wrapper.
  $markup = '<div id="' . $id_string . '-wrapper" class="calendar-month-fullcalendar-wrapper">';

  // full calendar markup.
  $markup .= '<div id="' . $id_string . '" class="calendar-month-fullcalendar"></div>';

  // calendar links
  $markup .= '<div id="mfcs-links_and_filters" class="links_and_filters-wrapper no-print">';

  $markup .= '<div class="links_and_filters-group group-1">';
  $markup .= mfcs_build_calendar_ical_links_markup($month_start, TRUE, TRUE, FALSE);
  $markup .= mfcs_build_calendar_openpgp_links_markup();
  $markup .= '</div>'; // filter-group-1

  $markup .= mfcs_build_determined_filters_markup();
  $markup .= '</div>'; // #mfcs-links_and_filters

  // end full calendar markup wrapper.
  $markup .= '</div>';

  return $markup;
}

/**
 * Build a day calendar display.
 *
 * This is a wrapper used to determine which calendar theme to use.
 * The specific calendar theme is based on each individual users settings.
 *
 * @param array $items
 *   An array of request items to link to.
 * @param int $day_start
 *   Unix timestamp for the start of the time interval.
 *   A >= operator is used on this.
 * @param int $day_stop
 *   Unix timestamp for the stop of the time interval.
 *   A < operator is used on this.
 * @param string $title
 *   An HTML title for describing the calendar.
 * @param int $page_number
 *   The page_number to use.
 * @param string $id
 *   (optional) A unique HTML id to associate with the calendar.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_day_markup($items, $day_start, $day_stop, $title, $page_number, $id = NULL) {
  $user = cf_current_user();

  $calendar = MFCS_USER_CALENDAR_ORIGINAL;
  if (isset($user->mfcs['calendar']) && is_numeric($user->mfcs['calendar'])) {
    $calendar = (int) $user->mfcs['calendar'];
  }

  if ($calendar === MFCS_USER_CALENDAR_FULL) {
    $success = TRUE;

    // load the moment library (required by fullcalendar).
    $library = libraries_load('moment');
    if (isset($library['error'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load moment library, reason: @reason.', array('@reason' => $library['error']), WATCHDOG_ERROR);
    }
    elseif (empty($library['loaded'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load moment library, reason: unknown.', array(), WATCHDOG_ERROR);
    }
    unset($library);

    // load the fullcalendar library.
    $library = libraries_load('fullcalendar');
    if (isset($library['error'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load fullcalendar library, reason: @reason.', array('@reason' => $library['error']), WATCHDOG_ERROR);
    }
    elseif (empty($library['loaded'])) {
      $success = FALSE;
      watchdog(MFCS_WATCHDOG_ID, 'Unable to load fullcalendar library, reason: unknown.', array(), WATCHDOG_ERROR);
    }
    unset($library);

    if ($success) {
      return mfcs_build_calendar_day_markup_full($items, $day_start, $day_stop, $title, $page_number, $id);
    }
  }

  // the original is the default or the fallback theme.
  return mfcs_build_calendar_day_markup_original($items, $day_start, $day_stop, $title, $page_number, $id);
}

/**
 * Build a day calendar display using the original calendar theme.
 *
 * @param array $items
 *   An array of request items to link to.
 * @param int $day_start
 *   Unix timestamp for the start of the time interval.
 *   A >= operator is used on this.
 * @param int $day_stop
 *   Unix timestamp for the stop of the time interval.
 *   A < operator is used on this.
 * @param string $title
 *   An HTML title for describing the calendar.
 * @param int $page_number
 *   The page_number to use.
 * @param string $id
 *   (optional) A unique HTML id to associate with the calendar.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_day_markup_original($items, $day_start, $day_stop, $title, $page_number, $id = NULL) {
  if (!is_array($items)) {
    cf_error::invalid_array('items');
    return FALSE;
  }

  if (!cf_is_integer($day_start)) {
    cf_error::invalid_integer('day_start');
    return FALSE;
  }

  if (!cf_is_integer($day_stop)) {
    cf_error::invalid_integer('day_stop');
    return FALSE;
  }

  if (!is_string($title)) {
    cf_error::invalid_string('title');
    return FALSE;
  }

  if (!cf_is_integer($page_number)) {
    cf_error::invalid_integer('page_number');
    return FALSE;
  }

  $id_string = '';
  $id_string_css_name = '';
  if (!empty($id)) {
    if (!is_string($id)) {
      cf_error::invalid_string('id');
      return FALSE;
    }

    $id = check_plain($id);
    $id_string = ' id="' . $id . '"';
    $id_string_css_name = '#' . $id;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $module_path = drupal_get_path('module', 'mfcs');

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_type_options = mfcs_get_request_type_list_options();

  // Set range at 1-hour intervals.
  $range_interval = 3600; // 3600 = 1 hour
  $range_height = 90; // 90px = 1 hour
  $range_height_small = 144; // 144 = 1 hour
  $range_width_screen = 196;
  $range_width_portrait = 152;
  $range_width_landscape = 196;
  $range_width_small = 96;
  $range_difference = 40; // 3600 / 90 = 40, unit of time per pixel.
  $range_difference_small = 25; // 3600 / 144 = 40, unit of time per pixel.
  $range_height_constraint = 75; // minimum pixels needed to enforce height constraints.

  $range_height_link_screen_max = 32;
  $range_height_link_portrait_max = 28;
  $range_height_link_landscape_max = 28;
  $range_height_link_small_max = 28;

  $range_height_time_screen_max = 16;
  $range_height_time_portrait_max = 14;
  $range_height_time_landscape_max = 14;
  $range_height_time_small_max = 14;

  $range_height_type_screen_max = 16;
  $range_height_type_portrait_max = 14;
  $range_height_type_landscape_max = 14;
  $range_height_type_small_max = 14;

  $range_height_non_location_screen = $range_height_link_screen_max + $range_height_time_screen_max + $range_height_type_screen_max;
  $range_height_non_location_portrait = $range_height_link_portrait_max + $range_height_time_portrait_max + $range_height_type_portrait_max;
  $range_height_non_location_landscape = $range_height_link_landscape_max + $range_height_time_landscape_max + $range_height_type_landscape_max;
  $range_height_non_location_small = $range_height_link_small_max + $range_height_time_small_max + $range_height_type_small_max;

  $slots_per_page = 4;
  $slots_minimum = $page_number * $slots_per_page;
  $slots_page_max = $slots_minimum + $slots_per_page;

  // prepare the inline CSS markup
  $css_screen = '';
  $css_portrait = '@media (orientation:portrait) {' . "\n";
  $css_landscape = '@media (orientation:landscape) {' . "\n";
  $css_small = '@media screen and (max-width: 640px) {' . "\n";

  // begin wrappers (main)
  $markup = '<div' . $id_string . ' class="calendar-day-wrapper">';

  // begin wrappers (All Day)
  $markup .= '<div class="calendar-day calendar-all_day">';


  // title (All Day)
  $markup .= '<div class="calendar-title">' . $title . '</div>';


  // navigation
  $last_day = strtotime('midnight yesterday', $day_start);
  $next_day = strtotime('midnight tomorrow', $day_start);

  $url_base_day = $base_path . 'requests/calendar-0/day';

  $last_url = $url_base_day . '/' . date('Y', $last_day) . '/' . strtolower(date('F', $last_day)) . '/' . date('d', $last_day) . $url_arguments;
  $this_url = $url_base_day . $url_arguments;
  $next_url = $url_base_day . '/' . date('Y', $next_day) . '/' . strtolower(date('F', $next_day)) . '/' . date('d', $next_day) . $url_arguments;

  $markup .= '<div class="calendar-navigation">';
  $markup .= '<div class="calendar-navigation-previous">';
  $markup .= '<a href="' . $last_url . '" title="Go to ' . date('F d, Y', $last_day) . '">Previous</a>';
  $markup .= '</div>';

  $instance = mfcs_instance();
  if (date('Y/m/d', $instance) != date('Y/m/d', $day_start)) {
    $markup .= '<div class="calendar-navigation-this">';
    $markup .= '<a href="' . $this_url . '" title="Go to Current Day">Today</a>';
    $markup .= '</div>';
  }

  $markup .= '<div class="calendar-navigation-next">';
  $markup .= '<a href="' . $next_url . '" title="Go to ' . date('F d, Y', $next_day) . '">Next</a>';
  $markup .= '</div>';
  $markup .= '</div>';


  // header (All Day)
  $markup .= '<div class="calendar-header-wrapper calendar-all_day"><div class="calendar-header">';
  $markup .= '<div class="calendar-day-wrapper"><div class="calendar-day">';
  $markup .= 'All Day';
  $markup .= '</div></div></div></div>';


  // body (All Day)
  $markup .= '<div class="calendar-body-wrapper calendar-all_day"><div class="calendar-body">';

  if (empty($items['all_day'])) {
    $markup .= '<div class="calendar-item-wrapper even row-0"><div class="calendar-item">';
    $markup .= 'No requests found.';
    $markup .= '</div></div>';
  }
  else {
    $row_even = TRUE;
    $row_count = 0;
    foreach ($items['all_day'] as $floor_time => $all_day) {
      $row_class = 'row-' . $row_count . ' ';
      if ($row_even) {
        $row_class .= 'even ';
      }
      else {
        $row_class .= 'odd ';
      }

      $markup .= '<div class="calendar-time_slot-wrapper ' . $row_class . '"><div class="calendar-time_slot">';
      $markup .= '<div class="calendar-time_slot-request-wrapper"><div class="calendar-time_slot-request">';

      $item_even = TRUE;
      $item_count = 0;
      $left_offset = 0;
      foreach ($all_day as $item_id => $item) {
        if (is_null($id)) {
          $item_id_string = 'calendar-item-' . $item_id;
          $item_id_parent = '';
        }
        else {
          $item_id_string = $id . '-calendar-item-' . $item_id;
          $item_id_parent = '#' . $id . ' ';
        }

        $item_class = '';

        $item_class = 'row-' . $item_count . ' ';
        if ($item_even) {
         $item_class .= 'even ';
        }
        else {
          $item_class .= 'odd ';
        }

        if ($item['is'] == 'holiday') {
          $item_class .= 'is-holiday ';
        }
        elseif ($item['is'] == 'unavailable') {
          $item_class .= 'is-unavailable ';
          $item_class .= 'building-' . $item['building'] . ' ';
          $item_class .= 'room-' . $item['room'] . ' ';
        }
        else {
          $item_class .= 'is-event ';
          $item_class .= 'location-' . $item['location'] . ' ';
          $item_class .= 'building-' . $item['building'] . ' ';
          $item_class .= 'room-' . $item['room'] . ' ';
        }

        if (array_key_exists('type', $item)) {
          $item_class .= 'type-' . $item['type'] . ' ';
        }

        $item_class .= 'slot-none ';

        $item_style = array(
          'box-sizing' => 'box-sizing: border-box;',
          '-moz-box-sizing' => '-moz-box-sizing: border-box;',
          '-webkit-box-sizing' => '-webkit-box-sizing: border-box;',
          'max-height' => 'max-height: ' . $range_height . 'px;',
          'margin-top' => 'margin-top: 0px;',
          'margin-bottom' => 'margin-bottom: 0px;',
          'margin-right' => 'margin-right: 3px;',
          'overflow' => 'overflow: hidden;',
        );

        if ($item['is'] == 'unavailable') {
        }
        else {
          $item_style['border'] = 'border: 2px outset;';
        }

        $item_style_screen = $item_style;
        $item_style_portrait = $item_style;
        $item_style_landscape = $item_style;
        $item_style_small = $item_style;

        $item_style_small['max-height'] = 'max-height: ' . $range_height_small . 'px;';

        $item_style_screen['width'] = $range_width_screen . 'px;';
        $item_style_screen['min-width'] = $range_width_screen . 'px;';
        $item_style_screen['max-width'] = $range_width_screen . 'px;';

        $item_style_portrait['width'] = $range_width_portrait . 'px;';
        $item_style_portrait['min-width'] = $range_width_portrait . 'px;';
        $item_style_portrait['max-width'] = $range_width_portrait . 'px;';

        $item_style_landscape['width'] = $range_width_landscape . 'px;';
        $item_style_landscape['min-width'] = $range_width_landscape . 'px;';
        $item_style_landscape['max-width'] = $range_width_landscape . 'px;';

        $item_style_small['width'] = $range_width_small . 'px;';
        $item_style_small['min-width'] = $range_width_small . 'px;';
        $item_style_small['max-width'] = $range_width_small . 'px;';

        $css_screen .= "\n" . $id_string_css_name . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_screen) . "\n}\n";
        $css_portrait .= "\n" . $id_string_css_name . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_portrait) . "\n}\n";
        $css_landscape .= "\n" . $id_string_css_name . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_landscape) . "\n}\n";
        $css_small .= "\n" . $id_string_css_name . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_small) . "\n}\n";

        $markup .= '<div id="' . $item_id_string . '"  class="calendar-item-wrapper ' . $item_class . '"><div class="calendar-item">';
        if ($item['is'] == 'holiday') {
          $markup .= '<div class="calendar-item-link-wrapper" title="' . $item['tooltip'] . '">' . $item['title'] . '</div>';
          $markup .= '<div class="calendar-item-description-wrapper"><div class="calendar-item-description">' . $item['description'] . '</div></div>';
        }
        elseif ($item['is'] == 'unavailable') {
          $markup .= '<div class="calendar-item-link-wrapper"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';
          $markup .= '<div class="calendar-item-location-wrapper"><div class="calendar-item-location" title="Location: ' . $item['building_code'] . ' ' . $item['room_number'] . '">(' . $item['building_name'] . ') ' . $item['room_name'] . '</div></div>';
          $markup .= '<div class="calendar-item-description-wrapper"><div class="calendar-item-description">' . $item['description'] . '</div></div>';

          $markup .= '<div class="calendar-item-id-wrapper"><div class="calendar-item-id" title="Room Unavailable">Room Unavailable</div></div>';

          if (array_key_exists('type_name', $item)) {
            $markup .= '<div class="calendar-item-type-wrapper"><div class="calendar-item-type" title="Unavailability Type: ' . $item['type_name'] . '">' . $item['type_name'] . '</div></div>';
          }
        }
        else {
          $markup .= '<div class="calendar-item-link-wrapper"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';
          $markup .= '<div class="calendar-item-location-wrapper"><div class="calendar-item-location" title="Location: ' . $item['building_code'] . ' ' . $item['room_number'] . '">(' . $item['building_name'] . ') ' . $item['room_name'] . '</div></div>';

          $markup .= '<div class="calendar-item-id-wrapper"><div class="calendar-item-id" title="Request ID: ' . $item_id . '">' . $item_id . '</div></div>';

          if (!empty($request_type_options[$item['type']])) {
            $markup .= '<div class="calendar-item-type-wrapper"><div class="calendar-item-type" title="Request Type: ' . $request_type_options[$item['type']] . '">' . $request_type_options[$item['type']] . '</div></div>';
          }
        }
        $markup .= '</div></div>';

        $item_count++;
        $item_even = !$item_even;
      }

      $markup .= '</div></div></div></div>';
    }

    $row_count++;
    $row_even = !$row_even;
  }
  $markup .= '</div></div>';

  // end wrappers (All Day)
  $markup .= '</div>';


  // begin page links (Range)
  $markup .= '<nav class="calendar-day calendar-range_links">';
  $markup .= '<header class="calendar-range_links-header element-invisible"><hgroup><h2 class="calendar-range_links-header-2">Page Selection</h2></hgroup></header>';
  $markup .= '<ul class="calendar-day calendar-range_links-pages">';

  $slots_page = 0;
  $slots_count = 0;
  do {
    $page_url = $url_base_day . '/' . date('Y', $day_start) . '/' . strtolower(date('F', $day_start)) . '/' . date('d', $day_start) . '/' . $slots_page . $url_arguments;

    $class_active_page = '';
    if ($slots_page == $page_number) {
      $markup .= '<li class="calendar-range_links-page page-' . $slots_page . '"><div class="calendar-range_links-page-link page-' . $slots_page . ' current_page" title="Currently Viewing Requests on Page ' . ($slots_page + 1) . '">Page #' . ($slots_page + 1) . '</div></li>';
    }
    else {
      $markup .= '<li class="calendar-range_links-page page-' . $slots_page . '"><a href="' . $page_url . '" class="calendar-range_links-page-link page-' . $slots_page . '" title="View Requests on Page ' . ($slots_page + 1) . '">Page #' . ($slots_page + 1) . '</a></li>';
    }

    $slots_count += $slots_per_page;
    $slots_page++;
  } while ($slots_count < $items['slots_max']);

  // setup page_url to be used in the prev/next links later on.
  $page_url = $url_base_day . '/' . date('Y', $day_start) . '/' . strtolower(date('F', $day_start)) . '/' . date('d', $day_start) . '/';

  // end page links (Range)
  $markup .= '</ul></nav>';


  // begin wrappers (Range)
  $markup .= '<div class="calendar-day calendar-range">';

  // title (Range)
  $markup .= '<div class="calendar-title element-invisible">' . $title . '</div>';


  // header (Range)
  $markup .= '<div class="calendar-header-wrapper calendar-range"><div class="calendar-header">';
  $markup .= '<div class="calendar-time_slot-wrapper"><div class="calendar-time_slot">';
  $markup .= '<div class="calendar-time_slot-label-wrapper"><div class="calendar-time_slot-label">' . "Time" . '</div></div>';
  $markup .= '<div class="calendar-time_slot-request-wrapper"><div class="calendar-time_slot-request">' . "Requests" . '</div></div>';
  $markup .= '</div></div></div></div>';


  // body (Range)
  $markup .= '<div class="calendar-body-wrapper calendar-range"><div class="calendar-body">';

  $populated = array();
  $row_even = TRUE;
  $row_count = 0;
  foreach ($items['range'] as $floor_time => $slots) {
    $slots_total = count($slots);

    $row_id = 'time_slot-' . date('h_00a', $floor_time);
    $row_class = array(
      'calendar-time_slot-wrapper',
      'row-' . $row_count,
    );

    if ($row_even) {
      $row_class[] = 'even';
    }
    else {
      $row_class[] = 'odd';
    }

    // begin range row.
    $markup .= '<div id="' . $row_id . '" class="' . implode(' ', $row_class) . '">';
    $markup .= '<div class="calendar-time_slot">';

    $markup .= '<div class="calendar-time_slot-label-wrapper">';
    $markup .= '<div class="calendar-time_slot-label">' . date(MFCS_DATE_FORMAT_TIME, $floor_time) . '</div>';

    $display_prev = $page_number > 0 && $slots_total > 0;
    $display_next = ($slots_total - $slots_minimum) > $slots_per_page;
    if ($display_prev || $display_next) {
      $markup .= '<div class="calendar-time_slot-links-wrapper no-print">';

      if ($display_prev) {
        $markup .= '<a href="' . $page_url . ($page_number - 1) . $url_arguments . '#' . $row_id . '" class="calendar-time_slot-link calendar-time_slot-links-previous" title="There are more requests on the previous page for this time slot.">&laquo; Previous</a>';
      }

      if ($display_next) {
        $markup .= '<a href="' . $page_url . ($page_number + 1) . $url_arguments . '#' . $row_id . '" class="calendar-time_slot-link calendar-time_slot-links-next" title="There are more requests on the next page for this time slot.">Next &raquo;</a>';
      }

      $markup .= '</div>';
    }

    unset($display_prev);
    unset($display_next);
    unset($slots_total);

    $markup .= '</div>';

    $markup .= '<div class="calendar-time_slot-request-wrapper">';
    $markup .= '<div class="calendar-time_slot-request">';

    foreach ($slots as $slot => $range) {
      // only process slots used by the current page.
      if ($slot < $slots_minimum) {
        continue;
      }
      elseif ($slot >= $slots_page_max) {
        break;
      }

      foreach ($range as $item_id => $item) {
        $item_id_string = 'calendar-item-' . $item_id . '-slot-' . $slot;
        if (!is_null($id)) {
          $item_id_string = $id . '-calendar-item-' . $item_id . '-slot-' . $slot;
        }

        $item_class = array();
        $item_class[] = 'item-zindex-normal';

        if (array_key_exists('location', $item)) {
          $item_class[] = 'location-' . $item['location'];
        }

        if (array_key_exists('building', $item)) {
          $item_class[] = 'building-' . $item['building'];
        }

        if (array_key_exists('room', $item)) {
          $item_class[] = 'room-' . $item['room'];
        }

        if ($item['is'] == 'unavailable') {
          $item_class[] = 'is-unavailable';
        }
        elseif ($item['is'] == 'holiday') {
          $item_class[] = 'is-holiday';
        }
        else {
          $item_class[] = 'is-event';
        }

        if (array_key_exists('type', $item)) {
          $item_class[] = 'type-' . $item['type'];
        }

        $item_class[] = 'slot-' . $slot;

        $item_style = array(
          'box-sizing' => 'box-sizing: border-box;',
          '-moz-box-sizing' => '-moz-box-sizing: border-box;',
          '-webkit-box-sizing' => '-webkit-box-sizing: border-box;',
          'max-height' => 'max-height: ' . $range_height . 'px;',
          'margin-top' => 'margin-top: 0px;',
          'margin-bottom' => 'margin-bottom: 0px;',
          'margin-right' => 'margin-right: 3px;',
          'z-index' => 'z-index: 2;',
          'overflow' => 'overflow: hidden;',
        );

        $item_style_screen = $item_style;
        $item_style_portrait = $item_style;
        $item_style_landscape = $item_style;
        $item_style_small = $item_style;

        $item_style_small['max-height'] = 'max-height: ' . $range_height_small . 'px;';

        $location_style_screen = array();
        $location_style_portrait = array();
        $location_style_landscape = array();
        $location_style_small = array();

        $item_style_screen['width'] = 'width: ' . $range_width_screen . 'px;';
        $item_style_screen['min-width'] = 'min-width: ' . $range_width_screen . 'px;';
        $item_style_screen['max-width'] = 'max-width: ' .  $range_width_screen . 'px;';

        $item_style_portrait['width'] = 'width: ' . $range_width_portrait . 'px;';
        $item_style_portrait['min-width'] = 'wmin-idth: ' . $range_width_portrait . 'px;';
        $item_style_portrait['max-width'] = 'max-width: ' . $range_width_portrait . 'px;';

        $item_style_landscape['width'] ='width: ' .  $range_width_landscape . 'px;';
        $item_style_landscape['min-width'] = 'wmin-idth: ' . $range_width_landscape . 'px;';
        $item_style_landscape['max-width'] = 'max-width: ' . $range_width_landscape . 'px;';

        $item_style_small['width'] = 'width: ' . $range_width_small . 'px;';
        $item_style_small['min-width'] = 'min-width: ' . $range_width_small . 'px;';
        $item_style_small['max-width'] = 'max-width: ' .  $range_width_small . 'px;';

        $margin_left_screen = 0;
        $margin_left_portrait = 0;
        $margin_left_landscape = 0;
        $margin_left_small = 0;
        $slot_count = $slot - 1;
        while ($slot_count >= $slots_minimum) {
          if (empty($slots[$slot_count])) {
            // the +6 is for the margins.
            $margin_left_screen += $range_width_screen + 6;
            $margin_left_portrait += $range_width_portrait + 6;
            $margin_left_landscape += $range_width_landscape + 6;
            $margin_left_small += $range_width_small + 6;
          }
          else {
            break;
          }

          $slot_count--;
        }

        if ($margin_left_screen > 0) {
          // add the 3 left margin pixels.
          $margin_left_screen += 3;
          $margin_left_portrait += 3;
          $margin_left_landscape += 3;
          $margin_left_small += 3;
          $item_style_screen['margin-left'] = 'margin-left: ' . $margin_left_screen . 'px;';
          $item_style_portrait['margin-left'] = 'margin-left: ' . $margin_left_portrait . 'px;';
          $item_style_landscape['margin-left'] = 'margin-left: ' . $margin_left_landscape . 'px;';
          $item_style_small['margin-left'] = 'margin-left: ' . $margin_left_small . 'px;';
        }
        else {
          $item_style_screen['margin-left'] = 'margin-left: 3px;';
          $item_style_portrait['margin-left'] = 'margin-left: 3px;';
          $item_style_landscape['margin-left'] = 'margin-left: 3px;';
          $item_style_small['margin-left'] = 'margin-left: 3px;';
        }

        $item_text = '';
        if ($item['slot_start'] || $item['slot_stop']) {
          // the item height must be calculated by the offset_stop, which is a fraction of an hour.
          // turn the offset into a percentage to determine the item height.
          $item_height_stop_min = 21;
          $item_height_stop = $range_height;
          $item_height_stop_small = $range_height_small;

          if ($item['offset_stop'] > 0) {
            $item_height_stop = floor($item['offset_stop'] / $range_difference);
          }

          if ($item_height_stop < $item_height_stop_min) {
            $item_height_stop = $item_height_stop_min;
          }

          if ($item['offset_stop'] > 0) {
            $item_height_stop_small = floor($item['offset_stop'] / $range_difference_small);
          }

          if ($item_height_stop_small < $item_height_stop_min) {
            $item_height_stop_small = $item_height_stop_min;
          }

          $item_height_min = 69;
          $item_height_min_text_screen = 48;
          $item_height_min_text_portrait = 50;
          $item_height_min_text_landscape = 50;
          $item_height_min_text_small = 48;
          $item_margin_top = floor($item['offset_start'] / $range_difference);
          $item_margin_top_small = floor($item['offset_start'] / $range_difference_small);

          // minimum height is not as restrictive for non-slot_start item slots.
          if (!$item['slot_start']) {
            $item_height_min = $item_height_stop_min;
          }

          if ($item_margin_top + $item_height_min > $range_height) {
            $item_margin_top = $range_height - $item_height_min;
          }

          if ($item_margin_top_small + $item_height_min > $range_height_small) {
            $item_margin_top_small = $range_height_small - $item_height_min;
          }

          $item_height = $range_height;
          if ($item['slot_start']) {
            $item_height -= $item_margin_top;
          }

          if ($item['slot_stop']) {
            $item_height -= ($range_height_small - $item_height_stop);

            if ($item_height < $item_height_min) {
              $item_height = $item_height_min;
              $item_height_stop = $range_height;
            }
          }

          $item_height_small = $range_height_small;
          if ($item['slot_start']) {
            $item_height_small -= $item_margin_top_small;
          }

          if ($item['slot_stop']) {
            $item_height_small -= ($range_height_small - $item_height_stop_small);

            if ($item_height_small < $item_height_min) {
              $item_height_small = $item_height_min;
              $item_height_stop_small = $range_height_small;
            }
          }

          if ($item['slot_start']) {
            $item_class[] = 'slot-start';

            $item_style_screen['margin-top'] = 'margin-top: ' . $item_margin_top . 'px;';
            $item_style_screen['height'] = 'height: ' . $item_height . 'px;';
            $item_style_screen['min-height'] = 'min-height: ' . $item_height_min . 'px;';
            $item_style_portrait['margin-top'] = $item_style_screen['margin-top'];
            $item_style_portrait['height'] = $item_style_screen['height'];
            $item_style_portrait['min-height'] = $item_style_screen['min-height'];
            $item_style_landscape['margin-top'] = $item_style_screen['margin-top'];
            $item_style_landscape['height'] = $item_style_screen['height'];
            $item_style_landscape['min-height'] = $item_style_screen['min-height'];
            $item_style_small['margin-top'] = 'margin-top: ' . $item_margin_top_small . 'px;';
            $item_style_small['height'] = 'height: ' . $item_height_small . 'px;';
            $item_style_small['min-height'] = 'min-height: ' . $item_height_min . 'px;';

            if ($item['slot_start'] && !$item['slot_stop']) {
              $item_style_screen['z-index'] = 'z-index: 1;';
              $item_style_portrait['z-index'] = 'z-index: 1;';
              $item_style_landscape['z-index'] = 'z-index: 1;';
              $item_style_small['z-index'] = 'z-index: 1;';

              // create a max-height in such a way that it will never go past the slot_stop block.
              $item_height_max = $range_height * floor(($item['time_ceiling'] - $item['time_floor']) / 3600);

              $item_height_max_text_screen = $item_height_max - $item_margin_top - $range_height_non_location_screen - ($range_height - $item_height_stop);
              if ($item_height_max_text_screen > $item_height_min_text_screen) {
                $location_style_screen['max-height'] = 'max-height: ' . $item_height_max_text_screen . 'px';
              }
              else {
                $location_style_screen['max-height'] = 'max-height: ' . $item_height_min_text_screen . 'px';
              }

              $item_height_max_text_portrait = $item_height_max - $item_margin_top - $range_height_non_location_portrait - ($range_height - $item_height_stop);
              if ($item_height_max_text_portrait > $item_height_min_text_portrait) {
                $location_style_portrait['max-height'] = 'max-height: ' . $item_height_max_text_portrait . 'px';
              }
              else {
                $location_style_portrait['max-height'] = 'max-height: ' . $item_height_min_text_portrait . 'px';
              }

              $item_height_max_text_landscape = $item_height_max - $item_margin_top - $range_height_non_location_landscape - ($range_height - $item_height_stop);
              if ($item_height_max_text_landscape > $item_height_min_text_landscape) {
                $location_style_landscape['max-height'] = 'max-height: ' . $item_height_max_text_landscape . 'px';
              }
              else {
                $location_style_landscape['max-height'] = 'max-height: ' . $item_height_min_text_landscape . 'px';
              }

              $item_height_max_text_small = $item_height_max - $item_margin_top_small - $range_height_non_location_small - ($range_height_small - $item_height_stop_small);
              if ($item_height_max_text_small > $item_height_min_text_small) {
                $location_style_small['max-height'] = 'max-height: ' . $item_height_max_text_small . 'px';
              }
              else {
                $location_style_small['max-height'] = 'max-height: ' . $item_height_min_text_small . 'px';
              }
            }
            elseif ($item['slot_start'] && $item['slot_stop']) {
              // constrain dimensions if the request is too small to display everything.
              if ($item_height <= $range_height_constraint) {
                $item_class[] = 'constraint-height';
              }
            }

            $item_times = date(MFCS_DATE_FORMAT_TIME, $item['time_start']) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $item['time_stop']);

            $item_text .= '<div class="calendar-item-link-wrapper"><a class="calendar-item-link" href="' . $item['href'] . '" title="' . $item['tooltip'] . '">' . $item['title'] . '</a></div>';
            $item_text .= '<div class="calendar-item-time-wrapper"><div class="calendar-item-time" title="Time: ' . $item_times . '">' . $item_times . '</div></div>';
            $item_text .= '<div class="calendar-item-location-wrapper"><div class="calendar-item-location" title="Location: ' . $item['building_code'] . ' ' . $item['room_number'] . '">(' . $item['building_name'] . ') ' . $item['room_name'] . '</div></div>';

            unset($item_times);
          }

          if ($item['slot_stop']) {
            $item_class[] = 'slot-stop';

            $item_margin_bottom = $range_height - $item_height;
            $item_style_screen['margin-bottom'] = 'margin-bottom: ' . $item_margin_bottom . 'px;';
            $item_style_screen['height'] = 'height: ' . $item_height . 'px;';
            $item_style_screen['min-height'] = 'min-height: ' . $item_height_min . 'px;';

            $item_style_portrait['margin-bottom'] = 'margin-bottom: ' . $item_margin_bottom . 'px;';
            $item_style_portrait['height'] = $item_style_screen['height'];
            $item_style_portrait['min-height'] = $item_style_screen['min-height'];

            $item_style_landscape['margin-bottom'] = 'margin-bottom: ' . $item_margin_bottom . 'px;';
            $item_style_landscape['height'] = $item_style_screen['height'];
            $item_style_landscape['min-height'] = $item_style_screen['min-height'];

            $item_style_small['margin-bottom'] = 'margin-bottom: ' . $item_margin_bottom . 'px;';
            $item_style_small['height'] = 'height: ' . $item_height_small . 'px;';
            $item_style_small['min-height'] = 'min-height: ' . $item_height_min . 'px;';

            $item_text .= '<div class="calendar-item-footer-wrapper">';

            if ($item['is'] == 'event') {
              $item_text .= '<div class="calendar-item-id-wrapper"><div class="calendar-item-id" title="Request ID: ' . $item_id . '">' . $item_id . '</div></div>';

              if (array_key_exists('type', $item) && !empty($request_type_options[$item['type']])) {
                $item_text .= '<div class="calendar-item-type-wrapper"><div class="calendar-item-type" title="Request Type: ' . $request_type_options[$item['type']] . '">' . $request_type_options[$item['type']] . '</div></div>';
              }
            }
            elseif ($item['is'] == 'unavailable') {
              $item_text .= '<div class="calendar-item-id-wrapper"><div class="calendar-item-id" title="Room Unavailable">Room Unavailable</div></div>';

              if (array_key_exists('type_name', $item)) {
                $item_text .= '<div class="calendar-item-type-wrapper"><div class="calendar-item-type" title="Unavailability Type: ' . $item['type_name'] . '">' . $item['type_name'] . '</div></div>';
              }
            }
            $item_text .= '</div>';
          }
        }
        else {
          $item_class[] = 'slot-continue';

          $item_style_screen['height'] = 'height: ' . $range_height . 'px;';
          $item_style_screen['min-height'] = 'min-height: ' . $range_height . 'px;';
          $item_style_screen['z-index'] = 'z-index: 0;';

          $item_style_portrait['height'] = $item_style_screen['height'];
          $item_style_portrait['min-height'] = $item_style_screen['min-height'];
          $item_style_portrait['z-index'] = $item_style_screen['z-index'];

          $item_style_landscape['height'] = $item_style_screen['height'];
          $item_style_landscape['min-height'] = $item_style_screen['min-height'];
          $item_style_landscape['z-index'] = $item_style_screen['z-index'];

          $item_style_screen['height'] = 'height: ' . $range_height_small . 'px;';
          $item_style_screen['min-height'] = 'min-height: ' . $range_height_small . 'px;';
          $item_style_screen['z-index'] = 'z-index: 0;';
        }

        $markup .= '<div id="' . $item_id_string . '" class="calendar-item-wrapper ' . implode(' ', $item_class) . '"><div class="calendar-item">';
        $markup .= $item_text;
        $markup .= '</div></div>';
      }

      $css_screen .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_screen) . "\n}\n";
      $css_portrait .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_portrait) . "\n}\n";
      $css_landscape .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_landscape) . "\n}\n";
      $css_small .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . " {\n  " . implode("\n  ", $item_style_small) . "\n}\n";

      if (!empty($location_style_screen)) {
        $css_screen .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper,' . "\n";
        $css_screen .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper > .calendar-item-location {' . "\n  " . implode("\n  ", $location_style_screen) . "\n}\n";
      }

      if (!empty($location_style_portrait)) {
        $css_portrait .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper,' . "\n";
        $css_portrait .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper > .calendar-item-location {' . "\n  " . implode("\n  ", $location_style_portrait) . "\n}\n";
      }

      if (!empty($location_style_landscape)) {
        $css_landscape .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper,' . "\n";
        $css_landscape .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper > .calendar-item-location {' . "\n  " . implode("\n  ", $location_style_landscape) . "\n}\n";
      }

      if (!empty($location_style_small)) {
        $css_small .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper,' . "\n";
        $css_small .= "\n" . $id_string_css_name . ' #' . $row_id . ' #' . $item_id_string . ' > .calendar-item > .calendar-item-location-wrapper > .calendar-item-location {' . "\n  " . implode("\n  ", $location_style_small) . "\n}\n";
      }
    }

    // end range row.
    $markup .= '</div></div></div></div>';

    $row_count++;
    $row_even = !$row_even;
  }

  // end body (Range)
  $markup .= '</div></div>';


  // end wrappers (Range)
  $markup .= '</div>';


  // calendar links
  $markup .= '<div id="mfcs-links_and_filters" class="links_and_filters-wrapper no-print">';
  $markup .= '<div class="links_and_filters-group group-1">';
  $markup .= mfcs_build_calendar_ical_links_markup($day_start);
  $markup .= mfcs_build_calendar_openpgp_links_markup();
  $markup .= '</div>';

  $markup .= mfcs_build_determined_filters_markup();
  $markup .= '</div>'; // #mfcs-links_and_filters


  // end wrappers (main)
  $markup .= '</div>';

  // add the inline css
  $css_portrait .= "}\n";
  $css_landscape .= "}\n";
  $css_small .= "}\n";

  drupal_add_css($css_screen, array('type' => 'inline', 'group' => CSS_THEME, 'media' => 'screen,handheld,projection', 'preprocess' => FALSE));
  drupal_add_css($css_portrait, array('type' => 'inline', 'group' => CSS_THEME, 'media' => 'print,embossed,tv', 'preprocess' => FALSE));
  drupal_add_css($css_landscape, array('type' => 'inline', 'group' => CSS_THEME, 'media' => 'print,embossed,tv', 'preprocess' => FALSE));
  drupal_add_css($css_small, array('type' => 'inline', 'group' => CSS_THEME, 'media' => 'screen,handheld,projection', 'preprocess' => FALSE));

  return $markup;
}

/**
 * Build a day calendar display using the full calendar theme.
 *
 * @param array $items
 *   An array of request items to link to.
 * @param int $day_start
 *   Unix timestamp for the start of the time interval.
 *   A >= operator is used on this.
 * @param int $day_stop
 *   Unix timestamp for the stop of the time interval.
 *   A < operator is used on this.
 * @param string $title
 *   An HTML title for describing the calendar.
 * @param int $page_number
 *   The page_number to use.
 * @param string $id
 *   (optional) A unique HTML id to associate with the calendar.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_day_markup_full($items, $day_start, $day_stop, $title, $page_number, $id = NULL) {
  if (!is_array($items)) {
    cf_error::invalid_array('items');
    return FALSE;
  }

  if (!cf_is_integer($day_start)) {
    cf_error::invalid_integer('day_start');
    return FALSE;
  }

  if (!cf_is_integer($day_stop)) {
    cf_error::invalid_integer('day_stop');
    return FALSE;
  }

  if (!is_string($title)) {
    cf_error::invalid_string('title');
    return FALSE;
  }

  if (!cf_is_integer($page_number)) {
    cf_error::invalid_integer('page_number');
    return FALSE;
  }

  // fullcalendar requires an id string to function.
  $id_string = 'mfcs-calendar-0-day';
  if (!empty($id)) {
    if (!is_string($id)) {
      cf_error::invalid_string('id');
      return FALSE;
    }

    $id_string = check_plain($id);
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $room_options = mfcs_get_request_room_list_options(NULL, NULL, FALSE, TRUE);
  $building_options = mfcs_get_request_building_list_options(NULL, NULL, FALSE, TRUE);
  $type_options = mfcs_get_request_type_list_options(NULL, NULL, FALSE, TRUE);

  $instance = mfcs_instance();
  $this_year = date('Y', $instance);
  $this_month = date('n', $instance);
  $this_day = date('j', $instance);


  // navigation
  $last_day = strtotime('midnight yesterday', $day_start);
  $next_day = strtotime('midnight tomorrow', $day_start);

  $url_base_month = $base_path . 'requests/calendar-0/month';
  $url_base_day = $base_path . 'requests/calendar-0/day';

  $last_url = $url_base_day . '/' . date('Y', $last_day) . '/' . strtolower(date('F', $last_day)) . '/' . strtolower(date('j', $last_day)) . $url_arguments;
  $this_url = $url_base_day . $url_arguments;
  $next_url = $url_base_day . '/' . date('Y', $next_day) . '/' . strtolower(date('F', $next_day)) . '/' . strtolower(date('j', $next_day)) . $url_arguments;


  // full calendar javascript.
  $javascript = '(function ($) {' . "\n";
  $javascript .= 'Drupal.behaviors.mfcs_daily_calendar_fullcalendar = {' . "\n";
  $javascript .= '  attach:function(context) {' . "\n";

  $javascript .= '$(\'#' . $id_string . '\').fullCalendar({' . "\n";

  // add custom buttons.
  $javascript .= '  customButtons: {' . "\n";
  $javascript .= '    previousDayUrl: {' . "\n";
  $javascript .= '      text: \'Previous\',' . "\n";
  $javascript .= '      click: function() {' . "\n";
  $javascript .= '        location.assign(\'' . $last_url . '\');' . "\n";
  $javascript .= '        return false;' . "\n";
  $javascript .= '      }' . "\n";
  $javascript .= '    },' . "\n";
  $javascript .= '    nextDayUrl: {' . "\n";
  $javascript .= '      text: \'Next\',' . "\n";
  $javascript .= '      click: function() {' . "\n";
  $javascript .= '        location.assign(\'' . $next_url . '\');' . "\n";
  $javascript .= '        return false;' . "\n";
  $javascript .= '      }' . "\n";
  $javascript .= '    }' . "\n";
  $javascript .= '  },' . "\n";

  // main settings
  $javascript .= '  header: {' . "\n";
  $javascript .= '    left: \'previousDayUrl,nextDayUrl\',' . "\n";
  $javascript .= '    center: \'title\',' . "\n";
  $javascript .= '    right: \'agendaDay,listDay\'' . "\n";
  $javascript .= '  },' . "\n";
  $javascript .= '  defaultView: \'agendaDay\',' . "\n";
  $javascript .= '  defaultDate: \'' . date('Y-m-d', $day_start) . '\',' . "\n";
  $javascript .= '  height: \'auto\',' . "\n";
  $javascript .= '  contentHeight: \'auto\',' . "\n";
  #$javascript .= '  now: \'' . date('Y-m-d', $instance) . '\',' . "\n";
  $javascript .= '  navLinks: true,' . "\n";
  $javascript .= '  editable: false,' . "\n";
  $javascript .= '  eventLimit: true,' . "\n";
  $javascript .= '  allDaySlot: true,' . "\n";
  $javascript .= '  slotDuration: \'01:00:00\',' . "\n";
  $javascript .= '  slotEventOverlap: true,' . "\n";
  $javascript .= '  eventLimit: false,' . "\n";

  // add qtip support.
  $javascript .= '  eventRender: function(event, element) {' . "\n";
  $javascript .= '    element.qtip({' . "\n";
  $javascript .= '      content: { title: event.tooltipTitle, text: event.tooltipText },' . "\n";
  $javascript .= '      style: event.tooltipClass,' . "\n";
  $javascript .= '      position: {' . "\n";
  $javascript .= '        my: \'top center\',' . "\n";
  $javascript .= '        at: \'bottom center\'' . "\n";
  $javascript .= '      }' . "\n";
  $javascript .= '    });' . "\n";
  $javascript .= '  },' . "\n";


  // add each all day event.
  $javascript_items = NULL;
  $space = '      ';
  if (!empty($items['all_day'])) {
    foreach ($items['all_day'] as $date_items) {
      foreach ($date_items as $item_id => $item) {
        $javascript_item_common = '';

        $tooltip_text = '';
        $tooltip_text_suffix = '';
        $tooltip_class = 'tooltip-calendar-daily tooltip-calendar-item';

        // fullcalendar performs sanitization, so send it raw title text.
        $processed_title_for_fullcalendar = str_replace('\'', '\\\'', $item['title-raw']);

        if ($item['is'] == 'holiday') {
          $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";

          $class_names = array(
            '\'calendar-item\'',
            '\'is-holiday\'',
            '\'year-' . $item['year'] . '\'',
            '\'month-' . $item['month'] . '\'',
            '\'day-' . $item['day'] . '\'',
          );

          if (array_key_exists('type', $item)) {
            $class_names[] = '\'type-' . $item['type'] . '\'';
          }

          $tooltip_class .= ' tooltip-is-holiday year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];

          if (!empty($item['tooltip'])) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-holiday_text">';
            $tooltip_text .= '<span class="tooltip-item-text">' . $item['tooltip'] . '</span>';
            $tooltip_text .= '</div>';
            $tooltip_text .= '<br>';
          }

          $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
        }
        elseif ($item['is'] == 'unavailable') {
          $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";
          $javascript_item_common .= $space . 'url: \'' . $item['href'] . '\',' . "\n";

          $class_names = array(
            '\'calendar-item\'',
            '\'is-unavailable\'',
            '\'building-' . $item['building'] . '\'',
            '\'room-' . $item['room'] . '\'',
            '\'year-' . $item['year'] . '\'',
            '\'month-' . $item['month'] . '\'',
            '\'day-' . $item['day'] . '\'',
          );

          if (array_key_exists('type', $item)) {
            $class_names[] = '\'type-' . $item['type'] . '\'';
          }

          $tooltip_class .= ' tooltip-is-unavailable year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];

          if (array_key_exists($item['building'], $building_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-building">';
            $tooltip_text .= '<span class="tooltip-item-name">Building:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($building_options[$item['building']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (array_key_exists($item['room'], $room_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-room">';
            $tooltip_text .= '<span class="tooltip-item-name">Room:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($room_options[$item['room']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (!empty($item['description'])) {
            if (!empty($tooltip_text)) {
              $tooltip_text .= '<br>';
            }

            $tooltip_text .= '<div class="tooltip-item tooltip-item-description">';
            $tooltip_text .= '<span class="tooltip-item-value">' . $item['description'] . '</span>';
            $tooltip_text .= '</div>';
          }

          if (!empty($tooltip_text)) {
            $tooltip_text .= '<br>';
          }

          $tooltip_text_suffix .= '<div class="tooltip-item tooltip-item-footer">';
          $tooltip_text_suffix .= '<span class="tooltip-item-unavailability_text">Room Unavailable</span> ';
          $tooltip_text_suffix .= '<span class="tooltip-item-unavailability_type">' . $item['type_name'] . '</span>';
          $tooltip_text_suffix .= '</div>';

          $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
        }
        else {
          $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";
          $javascript_item_common .= $space . 'url: \'' . $item['href'] . '\',' . "\n";

          if (array_key_exists($item['building'], $building_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-building">';
            $tooltip_text .= '<span class="tooltip-item-name">Building:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($building_options[$item['building']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (array_key_exists($item['room'], $room_options)) {
            $tooltip_text .= '<div class="tooltip-item tooltip-item-room">';
            $tooltip_text .= '<span class="tooltip-item-name">Room:</span> ';
            $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($room_options[$item['room']]) . '</span>';
            $tooltip_text .= '</div>';
          }

          if (!empty($tooltip_text)) {
            $tooltip_text .= '<br>';
          }

          $tooltip_text_suffix .= '<div class="tooltip-item tooltip-item-footer">';
          $tooltip_text_suffix .= '<span class="tooltip-item-request_id">' . (int) $item_id . '</span> ';
          $tooltip_text_suffix .= '<span class="tooltip-item-request_type">' . check_plain($type_options[$item['type']]) . '</span>';
          $tooltip_text_suffix .= '</div>';

          $class_names = array(
            '\'calendar-item\'',
            '\'is-event\'',
            '\'location-' . $item['location'] . '\'',
            '\'building-' . $item['building'] . '\'',
            '\'room-' . $item['room'] . '\'',
            '\'type-' . $item['type'] . '\'',
            '\'year-' . $item['year'] . '\'',
            '\'month-' . $item['month'] . '\'',
            '\'day-' . $item['day'] . '\'',
          );

          $tooltip_class .= ' tooltip-is-event year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];
          $tooltip_class .= ' tooltip-location-' . $item['location'] . ' tooltip-building-' . $item['building'] . ' tooltip-room-' . $item['room'];
          $tooltip_class .= ' tooltip-type-' . $item['type'];

          $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
        }

        $tooltip_text .= '<div class="tooltip-item tooltip-item-date">';
        $tooltip_text .= date(MFCS_DATE_FORMAT_DATE, $item['date']);
        $tooltip_text .= '</div>';

        if (!is_null($javascript_items)) {
          $javascript_items .= ',' . "\n";
        }

        $javascript_items .= '    {' . "\n";

        if (is_numeric($item_id)) {
          $javascript_items .= $space . 'id: ' . (int) $item_id . ',' . "\n";
        }
        else {
          $javascript_items .= $space . 'id: \'' . $item_id . '\',' . "\n";
        }

        $javascript_items .= $space . 'start: \'' . date('Y-m-d', $item['date']) . '\',' . "\n";
        $javascript_items .= $space . 'end: \'' . date('Y-m-d', strtotime('tomorrow', $item['date'])) . '\',' . "\n";
        $javascript_items .= $space . 'allDayDefault: true,' . "\n";
        $javascript_items .= $space . 'tooltipTitle: \'<strong>' . $item['title'] . '</strong>\',' . "\n";
        $javascript_items .= $space . 'tooltipText: \'' . $tooltip_text . $tooltip_text_suffix . '\',' . "\n";
        $javascript_items .= $space . 'tooltipClass: \'' . $tooltip_class . '\',' . "\n";
        $javascript_items .= $javascript_item_common;
        $javascript_items .= '    }';

        unset($javascript_item_common);
      }
    }
  }
  unset($item);
  unset($item_id);
  unset($date_items);

  // add each range event.
  $processed_items = array();
  $space = '      ';
  if (!empty($items['range'])) {
    foreach ($items['range'] as $date_slots) {
      foreach ($date_slots as $time_slot => $date_items) {
        foreach ($date_items as $item_id => $item) {
          // the original them reserves multiple slots per item.
          // this is irrelevant for the fullcalendar theme.
          // there are still multiple copies of the same item, so make sure only one gets applied.
          if (array_key_exists($item_id, $processed_items)) {
            continue;
          }
          $processed_items[$item_id] = TRUE;

          $javascript_item_common = '';

          $tooltip_text = '';
          $tooltip_text_suffix = '';
          $tooltip_class = 'tooltip-calendar-daily tooltip-calendar-item';

          // fullcalendar performs sanitization, so send it raw title text.
          $processed_title_for_fullcalendar = str_replace('\'', '\\\'', $item['title-raw']);

          if ($item['is'] == 'holiday') {
            $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";

            $class_names = array(
              '\'calendar-item\'',
              '\'is-holiday\'',
              '\'year-' . $item['year'] . '\'',
              '\'month-' . $item['month'] . '\'',
              '\'day-' . $item['day'] . '\'',
            );

            if (array_key_exists('type', $item)) {
              $class_names[] = '\'type-' . $item['type'] . '\'';
            }

            $tooltip_class .= ' tooltip-is-holiday year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];

            if (!empty($item['tooltip'])) {
              $tooltip_text .= '<div class="tooltip-item tooltip-item-holiday_text">';
              $tooltip_text .= '<span class="tooltip-item-text">' . $item['tooltip'] . '</span>';
              $tooltip_text .= '</div>';
              $tooltip_text .= '<br>';
            }

            $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
          }
          elseif ($item['is'] == 'unavailable') {
            $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";
            $javascript_item_common .= $space . 'url: \'' . $item['href'] . '\',' . "\n";

            if (array_key_exists($item['building'], $building_options)) {
              $tooltip_text .= '<div class="tooltip-item tooltip-item-building">';
              $tooltip_text .= '<span class="tooltip-item-name">Building:</span> ';
              $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($building_options[$item['building']]) . '</span>';
              $tooltip_text .= '</div>';
            }

            if (array_key_exists($item['room'], $room_options)) {
              $tooltip_text .= '<div class="tooltip-item tooltip-item-room">';
              $tooltip_text .= '<span class="tooltip-item-name">Room:</span> ';
              $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($room_options[$item['room']]) . '</span>';
              $tooltip_text .= '</div>';
            }

            if (!empty($item['description'])) {
              if (!empty($tooltip_text)) {
                $tooltip_text .= '<br>';
              }

              $tooltip_text .= '<div class="tooltip-item tooltip-item-description">';
              $tooltip_text .= '<span class="tooltip-item-value">' . $item['description'] . '</span>';
              $tooltip_text .= '</div>';
            }

            if (!empty($tooltip_text)) {
              $tooltip_text .= '<br>';
            }

            $tooltip_text_suffix .= '<div class="tooltip-item tooltip-item-footer">';
            $tooltip_text_suffix .= '<span class="tooltip-item-unavailability_text">Room Unavailable</span> ';
            $tooltip_text_suffix .= '<span class="tooltip-item-unavailability_type">' . $item['type_name'] . '</span>';
            $tooltip_text_suffix .= '</div>';

            $class_names = array(
              '\'calendar-item\'',
              '\'is-unavailable\'',
              '\'building-' . $item['building'] . '\'',
              '\'room-' . $item['room'] . '\'',
              '\'year-' . $item['year'] . '\'',
              '\'month-' . $item['month'] . '\'',
              '\'day-' . $item['day'] . '\'',
            );

            if (array_key_exists('type', $item)) {
              $class_names[] = '\'type-' . $item['type'] . '\'';
            }

            $tooltip_class .= ' tooltip-is-unavailable year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];

            $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
          }
          else {
            $javascript_item_common .= $space . 'title: \'' . $processed_title_for_fullcalendar . '\',' . "\n";
            $javascript_item_common .= $space . 'url: \'' . $item['href'] . '\',' . "\n";

            if (array_key_exists($item['building'], $building_options)) {
              $tooltip_text .= '<div class="tooltip-item tooltip-item-building">';
              $tooltip_text .= '<span class="tooltip-item-name">Building:</span> ';
              $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($building_options[$item['building']]) . '</span>';
              $tooltip_text .= '</div>';
            }

            if (array_key_exists($item['room'], $room_options)) {
              $tooltip_text .= '<div class="tooltip-item tooltip-item-room">';
              $tooltip_text .= '<span class="tooltip-item-name">Room:</span> ';
              $tooltip_text .= '<span class="tooltip-item-value">' . check_plain($room_options[$item['room']]) . '</span>';
              $tooltip_text .= '</div>';
            }

            if (!empty($tooltip_text)) {
              $tooltip_text .= '<br>';
            }

            $tooltip_text_suffix .= '<div class="tooltip-item tooltip-item-footer">';
            $tooltip_text_suffix .= '<span class="tooltip-item-request_id">' . (int) $item_id . '</span> ';
            $tooltip_text_suffix .= '<span class="tooltip-item-request_type">' . check_plain($type_options[$item['type']]) . '</span>';
            $tooltip_text_suffix .= '</div>';

            $class_names = array(
              '\'calendar-item\'',
              '\'is-event\'',
              '\'location-' . $item['location'] . '\'',
              '\'building-' . $item['building'] . '\'',
              '\'room-' . $item['room'] . '\'',
              '\'type-' . $item['type'] . '\'',
              '\'year-' . $item['year'] . '\'',
              '\'month-' . $item['month'] . '\'',
              '\'day-' . $item['day'] . '\'',
            );

            $tooltip_class .= ' tooltip-is-event year-' . $item['year'] . ' month-' . $item['month'] . ' day-' . $item['day'];
            $tooltip_class .= ' tooltip-location-' . $item['location'] . ' tooltip-building-' . $item['building'] . ' tooltip-room-' . $item['room'];
            $tooltip_class .= ' tooltip-type-' . $item['type'];

            $javascript_item_common .= $space . 'className: [ ' . implode(', ', $class_names) . ' ]' . "\n";
          }

          $tooltip_text .= '<div class="tooltip-item tooltip-item-date">';
          $tooltip_text .= date(MFCS_DATE_FORMAT_DATE, $item['date']);
          $tooltip_text .= '</div>';

          $tooltip_text .= '<div class="tooltip-item tooltip-item-times tooltip-item-time-' . $time_slot . '">';
          $tooltip_text .= date(MFCS_DATE_FORMAT_TIME, $item['time_start']) . ' to ' . date(MFCS_DATE_FORMAT_TIME, $item['time_stop']);
          $tooltip_text .= '</div>';

          if (!is_null($javascript_items)) {
            $javascript_items .= ',' . "\n";
          }

          $javascript_items .= '    {' . "\n";

          if (is_numeric($item_id)) {
            $javascript_items .= $space . 'id: ' . (int) $item_id . ',' . "\n";
          }
          else {
            $javascript_items .= $space . 'id: \'' . $item_id . '\',' . "\n";
          }

          $javascript_items .= $space . 'start: \'' . date('c', $item['time_start']) . '\',' . "\n";
          $javascript_items .= $space . 'end: \'' . date('c', $item['time_stop']) . '\',' . "\n";
          $javascript_items .= $space . 'allDayDefault: false,' . "\n";
          $javascript_items .= $space . 'tooltipTitle: \'<strong>' . $item['title'] . '</strong>\',' . "\n";
          $javascript_items .= $space . 'tooltipText: \'' . $tooltip_text . $tooltip_text_suffix . '\',' . "\n";
          $javascript_items .= $space . 'tooltipClass: \'' . $tooltip_class . '\',' . "\n";
          $javascript_items .= $javascript_item_common;
          $javascript_items .= '    }';

          unset($javascript_item_common);
        }
      }
    }
  }

  if (!is_null($javascript_items)) {
    $javascript .= '  events: [' . "\n" . $javascript_items . "\n" . '  ]' . "\n";
  }

  $javascript .= '});' . "\n";
  $javascript .= '    }' . "\n";
  $javascript .= '  }' . "\n";
  $javascript .= '})(jQuery);' . "\n";


  drupal_add_js($javascript, array('type' => 'inline', 'group' => JS_DEFAULT, 'preprocess' => FALSE, 'cache' => FALSE));


  // full calendar markup wrapper.
  $markup = '<div id="' . $id_string . '-wrapper" class="calendar-day-fullcalendar-wrapper">';

  // full calendar markup.
  $markup .= '<div id="' . $id_string . '" class="calendar-day-fullcalendar"></div>';

  // calendar links
  $markup .= '<div id="mfcs-links_and_filters" class="links_and_filters-wrapper no-print">';

  $markup .= '<div class="links_and_filters-group group-1">';
  $markup .= mfcs_build_calendar_ical_links_markup($day_start, TRUE, TRUE, TRUE);
  $markup .= mfcs_build_calendar_openpgp_links_markup();
  $markup .= '</div>'; // filter-group-1

  $markup .= mfcs_build_determined_filters_markup();
  $markup .= '</div>'; // #mfcs-links_and_filters

  // end full calendar markup wrapper.
  $markup .= '</div>';

  return $markup;
}

/**
 * Build a list of links that change the url path to apply a filter.
 *
 * @param int $date_start
 *   The time start unix timestamp.
 * @param bool $year
 *   (optional) If TRUE, show year, FALSE hide year.
 * @param bool $month
 *   (optional) If TRUE, show month, FALSE hide month.
 * @param bool $day
 *   (optional) If TRUE, show day, FALSE hide day.
 * @param string|null $id
 *   (optional) The HTML id attribute to use.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_ical_links_markup($date_start, $year = TRUE, $month = TRUE, $day = TRUE, $id = NULL) {
  if (!cf_is_integer($date_start)) {
    cf_error::invalid_integer('date_start');
    return FALSE;
  }

  if (!is_bool($year)) {
    cf_error::invalid_bool('year');
    return FALSE;
  }

  if (!is_bool($month)) {
    cf_error::invalid_bool('month');
    return FALSE;
  }

  if (!is_bool($day)) {
    cf_error::invalid_bool('day');
    return FALSE;
  }

  if (!is_null($id) && !is_string($id)) {
    cf_error::invalid_string('id');
    return FALSE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if (!empty($id)) {
    $id = ' id="' . $id . '"';
  }

  $markup = '<div' . $id . ' class="links_and_filters ical">';
  $markup .= '<h3 class="links_and_filters-title ical">iCalendar Download Options</h3>';
  $markup .= '<ul class="links_and_filters-list ical">';

  if ($year) {
    $year = date('Y', $date_start);

    if ($month) {
      $month = date('F', $date_start);

      if ($day) {
        $day = date('d', $date_start);

        $markup .= '<li class="links_and_filters-list-item item-day ical">';
        $markup .= '<div class="links_and_filters-list-item-label item-day item-label ical">Day:</div> ';
        $markup .= '<a href="' . $base_path . 'requests/ical-0/day/' . $year . '/' . $month . '/' . $day . $url_arguments . '" title="Download iCalendar for ' . $month . ' ' . $day . ', ' . $year . '" class="links_and_filters-list-item-link item-day ical item-link">' . $month . ' ' . $day . ', ' . $year . '</a>';
        $markup .= '</li>';
      }

      $markup .= '<li class="links_and_filters-list-item item-month ical">';
      $markup .= '<div class="links_and_filters-list-item-label item-month item-label ical">Month:</div> ';
      $markup .= '<a href="' . $base_path . 'requests/ical-0/month/' . $year . '/' . $month . $url_arguments . '" title="Download iCalendar for the month of ' . $month . ' ' . $year . '" class="links_and_filters-list-item-link item-month ical item-link">' . $month . ' ' . $year . '</a>';
      $markup .= '</li>';
    }

    $markup .= '<li class="links_and_filters-list-item item-year ical">';
    $markup .= '<div class="links_and_filters-list-item-label item-year item-label ical">Year:</div> ';
    $markup .= '<a href="' . $base_path . 'requests/ical-0/year/' . $year . $url_arguments . '" title="Download iCalendar for the year ' . $year . '" class="links_and_filters-list-item-link item-year ical item-link">' . $year . '</a>';
    $markup .= '</li>';
  }

  $markup .= '</ul>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Build a list of links that contain the available openpgp public keys for this system.
 *
 * @param string|null $id
 *   (optional) The HTML id attribute to use.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_calendar_openpgp_links_markup($id = NULL) {
  if (!is_null($id) && !is_string($id)) {
    cf_error::invalid_string('id');
    return FALSE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $module_path = drupal_get_path('module', 'mfcs');

  if (!empty($id)) {
    $id = ' id="' . $id . '"';
  }

  $markup = '<div' . $id . ' class="links_and_filters openpgp">';
  $markup .= '<h3 class="links_and_filters-title openpgp">OpenPGP Download Options</h3>';
  $markup .= '<ul class="links_and_filters-list openpgp">';

  $markup .= '<li class="links_and_filters-list-item item-public_key openpgp">';
  $markup .= '<div class="links_and_filters-list-item-label item-public_key item-label openpgp">Public Key:</div> ';
  $markup .= '<a href="' . $module_path . '/keys/fcs-public-2014_to_2020.pgp' . $url_arguments . '" title="Download OpenPGP public key for the years 2014 to 2020." class="links_and_filters-list-item-link item-public_key openpgp item-link">2014 to 2020</a>';
  $markup .= '</li>';

  $markup .= '</ul>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Build a list of links that change the url path to apply a filter.
 *
 * @param string|null $id
 *   The HTML id attribute to use.
 *
 * @return string|bool
 *   The generate HTML markup or FALSE on error.
 */
function mfcs_build_determined_filters_markup($id = NULL) {
  if (!is_null($id) && !is_string($id)) {
    cf_error::invalid_string('id');
    return FALSE;
  }

  global $base_path;
  global $mfcs_determined;

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_type_options = mfcs_get_request_type_list_options();
  $request_classification_options = mfcs_get_request_classification_list_options();
  $building_list_options = mfcs_get_request_building_list_options();

  $user = cf_current_user();


  $type_id = NULL;
  $classifications_id = NULL;
  if (!empty($id)) {
    $type_id = ' id="' . $id . '-types"';
    $classifications_id = ' id="' . $id . '-classifications"';
  }


  // build the url arguments outside of the loop to avoid repitition.
  $url_arguments_classifications = '';
  if (!empty($mfcs_determined['get']['classifications'])) {
    $url_arguments_classifications = 'classifications=' . implode(',', $mfcs_determined['get']['classifications']);
  }

  $url_arguments_types = '';
  if (!empty($mfcs_determined['get']['types'])) {
    $url_arguments_types = 'types=' . implode(',', $mfcs_determined['get']['types']);
  }

  $url_arguments_buildings = '';
  if (!empty($mfcs_determined['get']['buildings'])) {
    $url_arguments_buildings = 'buildings=' . implode(',', $mfcs_determined['get']['buildings']);
  }

  $url_arguments_remaining = $mfcs_determined['remaining'];


  // classifications
  $markup = '<div class="links_and_filters-group group-2">';
  $markup .= '<div' . $id . ' class="links_and_filters filter_classifications">';
  $markup .= '<h3 class="links_and_filters-title filter_classifications">Filter by Classification</h3>';
  $markup .= '  <ul class="links_and_filters-list filter_classifications">';

  // pre-process the classifications to ensure that classifications removed by the user customization settings do not get appended as a url argument.
  $pre_processed_classifications = array();
  if (!empty($mfcs_determined['get']['classifications'])) {
    $pre_processed_classifications = $mfcs_determined['get']['classifications'];
  }
  if (!empty($user->mfcs['filter_by_classification'])) {
    foreach ($user->mfcs['filter_by_classification'] as $classification_id) {
      unset($pre_processed_classifications[$classification_id]);
    }
    unset($classification_id);
  }

  // build the url arguments.
  foreach ($request_classification_options as $classification_id => $classification) {
    $current_get = $pre_processed_classifications;
    $link_cg = $base_path . $_GET['q'];

    $filtered_via_customization = FALSE;
    $filtered_via_url = FALSE;
    if (isset($user->mfcs['filter_by_classification']) && array_key_exists($classification_id, $user->mfcs['filter_by_classification'])) {
      if (isset($mfcs_determined['get']['classifications'][-$classification_id])) {
        unset($current_get[-$classification_id]);
        $link_action_css = 'link-add';
        $link_action_text = 'Filter: ';
      }
      else {
        $filtered_via_customization = TRUE;
        unset($current_get[$classification_id]);
        $current_get[- $classification_id] = -$classification_id;
        $link_action_css = 'link-remove';
        $link_action_text = 'Unfilter: ';
      }
    }
    elseif (isset($mfcs_determined['get']['classifications'][$classification_id])) {
      $filtered_via_url = TRUE;
      unset($current_get[$classification_id]);
      $link_action_css = 'link-remove';
      $link_action_text = 'Unfilter: ';
    }
    else {
      $current_get[$classification_id] = $classification_id;
      $link_action_css = 'link-add';
      $link_action_text = 'Filter: ';
    }


    // rebuild the url parameter for the link (in the case of unfiltering, the current valid id will be removed).
    $first = TRUE;
    $add_qmark = TRUE;

    if (!empty($current_get)) {
      $link_cg .= '?';
      $add_qmark = FALSE;

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= 'classifications=' . implode(',', $current_get);
    }

    if (!empty($url_arguments_types)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_types;
    }

    if (!empty($url_arguments_buildings)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_buildings;
    }

    if (!empty($url_arguments_remaining)) {
      if (!$add_qmark) {
        $link_cg .= '?';
        $add_qmark = TRUE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_remaining;
    }


    // build the output
    $markup .= '<li' . $id . ' class="links_and_filters-list-item item-' . $classification_id . ' filter_classifications">';
    $markup .= '<a href="' . $link_cg . '" class="links_and_filters-list-item-link item-' . $classification_id . ' item-link ' . $link_action_css . ' filter_classifications" title="' . $link_action_text . $classification . '">' . $link_action_text . $classification . '</a>';
    $markup .= '</li>';
  }

  $markup .= '</ul>';
  $markup .= '</div>';


  // types
  $markup .= '<div' . $id . ' class="links_and_filters filter_types">';
  $markup .= '<h3 class="links_and_filters-title filter_types">Filter by Type</h3>';
  $markup .= '  <ul class="links_and_filters-list filter_types">';

  // pre-process the types to ensure that types removed by the user customization settings do not get appended as a url argument.
  $pre_processed_types = array();
  if (!empty($mfcs_determined['get']['types'])) {
    $pre_processed_types = $mfcs_determined['get']['types'];
  }
  if (!empty($user->mfcs['filter_by_type'])) {
    foreach ($user->mfcs['filter_by_type'] as $type_id) {
      unset($pre_processed_types[$type_id]);
    }
    unset($type_id);
  }

  // build the url arguments.
  foreach ($request_type_options as $type_id => $type) {
    $current_get = $pre_processed_types;
    $link_cg = $base_path . $_GET['q'];

    $filtered_via_customization = FALSE;
    $filtered_via_url = FALSE;
    if (isset($user->mfcs['filter_by_type']) && array_key_exists($type_id, $user->mfcs['filter_by_type'])) {
      if (isset($mfcs_determined['get']['types'][-$type_id])) {
        unset($current_get[-$type_id]);
        $link_action_css = 'link-add';
        $link_action_text = 'Filter: ';
      }
      else {
        $filtered_via_customization = TRUE;
        unset($current_get[$type_id]);
        $current_get[- $type_id] = -$type_id;
        $link_action_css = 'link-remove';
        $link_action_text = 'Unfilter: ';
      }
    }
    elseif (isset($mfcs_determined['get']['types'][$type_id])) {
      $filtered_via_url = TRUE;
      unset($current_get[$type_id]);
      $link_action_css = 'link-remove';
      $link_action_text = 'Unfilter: ';
    }
    else {
      $current_get[$type_id] = $type_id;
      $link_action_css = 'link-add';
      $link_action_text = 'Filter: ';
    }


    // rebuild the url parameter for the link (in the case of unfiltering, the current valid id will be removed).
    $first = TRUE;
    $add_qmark = TRUE;

    if (!empty($url_arguments_classifications)) {
      $link_cg .= '?';
      $add_qmark = FALSE;

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_classifications;
    }

    if (!empty($current_get)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= 'types=' . implode(',', $current_get);
    }

    if (!empty($url_arguments_buildings)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_buildings;
    }

    if (!empty($url_arguments_remaining)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_remaining;
    }


    // build the output.
    $markup .= '<li' . $id . ' class="links_and_filters-list-item item-' . $type_id . ' filter_types">';
    $markup .= '<a href="' . $link_cg . '" class="links_and_filters-list-item-link item-' . $type_id . ' item-link ' . $link_action_css . ' filter_types" title="' . $link_action_text . $type . '">' . $link_action_text . $type . '</a>';
    $markup .= '</li>';
  }

  $markup .= '</ul>';
  $markup .= '</div>';

  $markup .= '</div>'; // links_and_filters-group group-2


  // buildings
  $markup .= '<div class="links_and_filters-group group-3">';
  $markup .= '<div' . $id . ' class="links_and_filters filter_buildings">';
  $markup .= '<h3 class="links_and_filters-title filter_buildings">Filter by Building</h3>';
  $markup .= '  <ul class="links_and_filters-list filter_buildings">';

  // pre-process the buildings to ensure that buildings removed by the user customization settings do not get appended as a url argument.
  $pre_processed_buildings = array();
  if (!empty($mfcs_determined['get']['buildings'])) {
    $pre_processed_buildings = $mfcs_determined['get']['buildings'];
  }
  if (!empty($user->mfcs['filter_by_building'])) {
    foreach ($user->mfcs['filter_by_building'] as $building_id) {
      unset($pre_processed_buildings[$building_id]);
    }
    unset($building_id);
  }

  // build the url arguments.
  foreach ($building_list_options as $building_id => $building) {
    $current_get = $pre_processed_buildings;
    $link_cg = $base_path . $_GET['q'];

    $filtered_via_customization = FALSE;
    $filtered_via_url = FALSE;
    if (isset($user->mfcs['filter_by_building']) && array_key_exists($building_id, $user->mfcs['filter_by_building'])) {
      if (isset($mfcs_determined['get']['buildings'][-$building_id])) {
        unset($current_get[-$building_id]);
        $link_action_css = 'link-add';
        $link_action_text = 'Filter: ';
      }
      else {
        $filtered_via_customization = TRUE;
        unset($current_get[$building_id]);
        $current_get[- $building_id] = -$building_id;
        $link_action_css = 'link-remove';
        $link_action_text = 'Unfilter: ';
      }
    }
    elseif (isset($mfcs_determined['get']['buildings'][$building_id])) {
      $filtered_via_url = TRUE;
      unset($current_get[$building_id]);
      $link_action_css = 'link-remove';
      $link_action_text = 'Unfilter: ';
    }
    else {
      $current_get[$building_id] = $building_id;
      $link_action_css = 'link-add';
      $link_action_text = 'Filter: ';
    }


    // rebuild the url parameter for the link (in the case of unfiltering, the current valid id will be removed).
    $first = TRUE;
    $add_qmark = TRUE;

    if (!empty($url_arguments_classifications)) {
      $link_cg .= '?';
      $add_qmark = FALSE;

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_classifications;
    }

    if (!empty($url_arguments_types)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_types;
    }

    if (!empty($current_get)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= 'buildings=' . implode(',', $current_get);
    }

    if (!empty($url_arguments_remaining)) {
      if ($add_qmark) {
        $link_cg .= '?';
        $add_qmark = FALSE;
      }

      if ($first) {
        $first = FALSE;
      }
      else {
        $link_cg .= '&';
      }

      $link_cg .= $url_arguments_remaining;
    }


    // build the output.
    $markup .= '<li' . $id . ' class="links_and_filters-list-item item-' . $type_id . ' filter_types">';
    $markup .= '<a href="' . $link_cg . '" class="links_and_filters-list-item-link item-' . $type_id . ' item-link ' . $link_action_css . ' filter_types" title="' . $link_action_text . $building . '">' . $link_action_text . $building . '</a>';
    $markup .= '</li>';
  }

  $markup .= '</ul>';
  $markup .= '</div>';

  $markup .= '</div>'; // links_and_filters-group group-3

  return $markup;
}

/**
 * Renders HTML for a self-contained request view page useful for printing.
 *
 * @param string $html
 *   The HTML markup to convert to a PDF.
 * @param array $parameters
 *   (optional) Change the default behavior on how the page is generated.
 *
 * @return string|bool
 *   Renderred markup on success, FALSE otherwise.
 */
function mfcs_output_print_page_view($html, $parameters = array()) {
  if (!is_string($html)) {
    cf_error::invalid_string('html');
    return FALSE;
  }

  if (!is_array($parameters)) {
    cf_error::invalid_array('parameters');
    return FALSE;
  }

  global $base_url;
  global $base_path;

  // determine the number of directories in the base path so that they can be stripped out properly.
  $base_path_count = count(explode('/', $base_path));
  $css_path_0 = $base_path_count;
  $css_path_1 = $base_path_count + 1;
  $css_path_2 = $base_path_count + 2;
  $css_path_3 = $base_path_count + 3;

  // Reconstruct the HTML output.
  $cf_dom = new cf_dom(TRUE, TRUE, TRUE, $html);

  $meta_tags = $cf_dom->get_body()->getElementsByTagName('meta');
  $style_tags = $cf_dom->get_body()->getElementsByTagName('style');
  $link_tags = $cf_dom->get_body()->getElementsByTagName('link');
  $title_tags = $cf_dom->get_body()->getElementsByTagName('title');
  $main_tag = $cf_dom->get_dom()->getElementById('mcneese-content-main');

  $head_markup = '';
  $body_markup = '';
  $css_markup = '';

  // meta tags
  foreach ($meta_tags as $tag) {
    $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
  }

  // style tags
  foreach ($style_tags as $tag) {
    // drupal generates css styles using @import, remove all of these.
    $matched = array();
    $matches = preg_match_all('/@import url\("([^"]*)/i', $tag->nodeValue, $matched);

    $media = NULL;
    if ($tag->hasAttribute('media')) {
      $media = $tag->getAttribute('media');
    }
    if (empty($media)) {
      $media = 'all';
    }

    if ($matches > 0) {
      if ($media == 'all' || $media == 'print,embossed,tv' || $media == 'print') {
        $match = array_pop($matched);

        foreach ($match as $m) {
          // skip past css uneeded css
          if ($media == 'all') {
            $parts = explode('/', $m);
            if (!isset($parts[$css_path_1])) {
              continue;
            }
            if ($parts[$css_path_0] == 'system') {
            }

            if (isset($parts[$css_path_2])) {
              if ($parts[$css_path_2] == 'mcneese') {
              }
              elseif ($parts[$css_path_2] == 'mcneese_fcs') {
              }
              elseif ($parts[$css_path_2] == 'mfcs') {
              }
              else {
                continue;
              }
            }
          }

          $fixed_path = preg_replace('/\?.*$/i', '', $m);
          $parts = explode('/', $fixed_path);

          $bpc = 2;
          while ($bpc <= $base_path_count) {
            array_shift($parts);
            $bpc++;
          }

          $fixed_path = implode('/', $parts);
          $content = drupal_load_stylesheet(DRUPAL_ROOT . '/' . $fixed_path);

          if (!empty($content)) {
            $css_markup .= $content;
          }

          unset($content);
          unset($parts);
          unset($fixed_path);
        }
      }
    }
    else {
      $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
    }
  }

  if (!empty($css_markup)) {
    $head_markup .= '<style>' . $css_markup . '</style>';
  }

  // link tags
  foreach ($link_tags as $tag) {
    $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
  }

  // title tags
  if (empty($parameters['page_title'])) {
    if ($parameters['page_title'] === FALSE) {
      $page_title = FALSE;
    }
    else {
      $page_title = 'Facilities Use Request';
    }
  }
  else {
    $page_title = $parameters['page_title'];
  }

  if (empty($parameters['title'])) {
    if (empty($parameters['page_title'])) {
      $head_markup .= '<title>Request</title>';
    }
    else {
      $head_markup .= '<title>' . $page_title . '</title>';
    }
  }
  else {
    $head_markup .= '<title>' . $parameters['title'] . '</title>';
  }

  $body_markup .= $cf_dom->get_dom()->saveHTML($main_tag);

  $body_class = 'mcneese';
  $cf = &drupal_static('cf_theme_get_variables', array());
  if (isset($cf['markup_css']['body']['class'])) {
    $body_class .= $cf['markup_css']['body']['class'];
  }

  if (!empty($parameters['body_class']) && is_array($parameters['body_class'])) {
    $body_class .= ' ' . implode(' ', $parameters['body_class']);
  }

  $html = '<!DOCTYPE html>' . "\n";
  $html .= '<html lang="en" dir="ltr">';

  $html .= '<head>' . $head_markup . '</head>';
  $html .= '<body class="' . $body_class . '">';
  $html .= '  <div id="mcneese-page" class="mcneese-page">';
  $html .= '    <div id="mcneese-page-content" class="mcneese-content full" role="main">';

  if ($page_title !== FALSE) {
    $html .= '    <header class="page-title html_tag-header">';
    $html .= '      <hgroup class="html_tag-hgroup">';
    $html .= '        <h1 class="page-title html_tag-heading" role="main">' . $page_title . '</h1>';
    $html .= '      </hgroup>';
    $html .= '    </header>';
  }

  $html .= '    ' . $body_markup;
  $html .= '    </div>';
  $html .= '  </div>';
  $html .= '</body>';
  $html .= '</html>';

  // Workaround: relative paths are not always working so change them to absolute paths.
  $html = preg_replace('|<img([^>]*) src="/fcs/|i', '<img$1 src="' . $base_url . '/', $html);
  $html = preg_replace('|<a([^>]*) href="/fcs/|i', '<a$1 href="' . $base_url . '/', $html);
  $html = preg_replace('|<link([^>]*) href="/fcs/|i', '<link$1 href="' . $base_url . '/', $html);

  return $html;
}

/**
 * Renders a PDF of a given request using the wkhtmltopdf library.
 *
 * @param int $request_id
 *   The request id, which is used to generate the filename.
 * @param int $revision_id
 *   The revision id, which is used to generate the filename.
 * @param string $html
 *   The HTML markup to convert to a PDF. Recommend converting the markup
 *   using mfcs_output_print_page_view().
 * @param bool $local
 *   (optional) If TRUE, then the file is saved to the local disk.
 *   If FALSE, then the file is print to stdout.
 * @param int $type
 *   (optional) What the PDF represents, such as a request or an agreement.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 * @see: mfcs_output_print_page_view()
 */
function mfcs_request_pdf_0_page_using_wkhtmltopdf($request_id, $revision_id, $html, $local = FALSE, $type = MFCS_PDF_TYPE_REQUEST) {
  if (!is_null($request_id) && !cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    return FALSE;
  }

  if (!is_string($html)) {
    cf_error::invalid_string('html');
    return FALSE;
  }

  if (!is_bool($local)) {
    cf_error::invalid_bool('local');
    return FALSE;
  }

  if (!cf_is_integer($type)) {
    cf_error::invalid_integer('type');
    return FALSE;
  }

  $library = libraries_load('phpwkhtmltopdf');
  if ($library === FALSE || empty($library['loaded'])) {
    return FALSE;
  }

  $wkpdf = new WkHtmlToPdf();

  $wk_options = array();

  $wk_options['binPath'] = '/usr/local/bin/wkhtmltopdf';
  $wk_options['binName'] = 'wkhtmltopdf';

  // when not ignored, the PDF does not get generated.
  $wk_options['ignoreWarnings'] = TRUE;

  $wrapper = file_stream_wrapper_get_instance_by_scheme('temporary');
  if (is_object($wrapper)) {
    $wk_options['tmp'] = $wrapper->getDirectoryPath();
  }

  // setup the default options
  $wkpdf->setOptions($wk_options);

  $wkpdf->addPage($html);

  if (is_null($request_id)) {
  }
  else {
    $filename = mfcs_build_filename($request_id, $revision_id, 0, $type);
  }

  if ($local) {
    // save the file, locally.
    $temporary_name = drupal_tempnam('temporary://', 'file');
    $wkpdf->saveAs($temporary_name);

    file_unmanaged_move($temporary_name, 'private://requests/local/' . $filename, FILE_EXISTS_REPLACE);
  }
  else {
    drupal_send_headers();

    // send to the browser
    #$result = $wkpdf->send($filename, TRUE);

    // send as a download
    $result = $wkpdf->send($filename, FALSE);
  }

  if (!$result) {
    watchdog(MFCS_WATCHDOG_ID, 'Error occured while generating the PDF of request @id: @error', array('@id' => $request_id, '@error' => $wkpdf->getError()), WATCHDOG_ERROR);
    return FALSE;
  }

  return TRUE;
}

/**
 * Renders a PDF of a given request using the wkhtmltopdf library.
 *
 * @param string $filename
 *   The name of the file to use.
 * @param string $html
 *   The HTML markup to convert to a PDF. Recommend converting the markup
 *   using mfcs_output_print_page_view().
 * @param bool $local
 *   (optional) If TRUE, then the file is saved to the local disk.
 *   If FALSE, then the file is print to stdout.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 * @see: mfcs_output_print_page_view()
 */
function mfcs_request_pdf_0_page_using_wkhtmltopdf_generic($filename, $html, $local = FALSE) {
  if (!is_string($filename) && strlen($filename) > 0) {
    cf_error::invalid_string('filename');
    return FALSE;
  }

  if (!is_string($html)) {
    cf_error::invalid_string('html');
    return FALSE;
  }

  if (!is_bool($local)) {
    cf_error::invalid_bool('local');
    return FALSE;
  }

  $library = libraries_load('phpwkhtmltopdf');
  if ($library === FALSE || empty($library['loaded'])) {
    return FALSE;
  }

  $wkpdf = new WkHtmlToPdf();

  $wk_options = array();

  $wk_options['binPath'] = '/usr/local/bin/wkhtmltopdf';
  $wk_options['binName'] = 'wkhtmltopdf';

  // when not ignored, the PDF does not get generated.
  $wk_options['ignoreWarnings'] = TRUE;

  $wrapper = file_stream_wrapper_get_instance_by_scheme('temporary');
  if (is_object($wrapper)) {
    $wk_options['tmp'] = $wrapper->getDirectoryPath();
  }

  // setup the default options
  $wkpdf->setOptions($wk_options);

  $wkpdf->addPage($html);

  if ($local) {
    // save the file, locally.
    $temporary_name = drupal_tempnam('temporary://', 'file');
    $wkpdf->saveAs($temporary_name);

    file_unmanaged_move($temporary_name, 'private://requests/local/' . $filename, FILE_EXISTS_REPLACE);
  }
  else {
    drupal_send_headers();

    // send to the browser
    #$result = $wkpdf->send($filename, TRUE);

    // send as a download
    $result = $wkpdf->send($filename, FALSE);
  }

  if (!$result) {
    watchdog(MFCS_WATCHDOG_ID, 'Error occured while generating the PDF of @filename: @error', array('@filename' => $filename, '@error' => $wkpdf->getError()), WATCHDOG_ERROR);
    return FALSE;
  }

  return TRUE;
}

/**
 * Builds the PDF filename string for an request.
 *
 * @param int $request_id
 *   The Request ID.
 * @param bool|int $revision_id
 *   (optional) Set to NULL for latest revision. Otherwise this is the number
 *   that represents the revision of the request to load.
 * @param int $mode
 *   (optional) Determines how the filename is generated:
 *   0 - use the local filename method.
 *   1 - use the BDM filename PDF.
 *   2 - use the BDM filename Meta.
 *   3 - use the Email filename marker.
 * @param int $type
 *   (optional) What the PDF represents, such as a request or an agreement.
 *
 * @return string|false
 *   The built filename string or FALSE on error.
 */
function mfcs_build_filename($request_id, $revision_id = NULL, $mode = MFCS_VIEW_MODE_NORMAL, $type = MFCS_PDF_TYPE_REQUEST) {
  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $revision_id);

  if (empty($request)) {
    return FALSE;
  }

  if (!cf_is_integer($mode) || $mode < MFCS_VIEW_MODE_NORMAL || $mode > MFCS_VIEW_MODE_OPERATIONS) {
    cf_error::invalid_integer('mode');
    return FALSE;
  }

  if (!cf_is_integer($type)) {
    cf_error::invalid_integer('type');
    return FALSE;
  }

  $name = 'unknown';
  if ($type === MFCS_PDF_TYPE_REQUEST) {
    $name = 'request';
  }
  elseif ($type === MFCS_PDF_TYPE_AGREEMENT) {
    $name = 'agreement';
  }
  elseif ($type === MFCS_PDF_TYPE_CALENDAR) {
    $name = 'calendar';
  }

  $fixed_title = preg_replace('/\W/i', '_', $request['information']['title'][0]->value);
  $fixed_title = preg_replace('/_+/i', '_', $fixed_title);

  // remove leading and trailing '_'.
  $fixed_title = preg_replace('/^_+/i', '', $fixed_title);
  $fixed_title = preg_replace('/_+$/i', '', $fixed_title);

  $extra = '';
  if ($mode == MFCS_VIEW_MODE_NORMAL) {
  }
  elseif ($mode == MFCS_VIEW_MODE_DISPLAY) {
    $extra = '-display';
  }
  elseif ($mode == MFCS_VIEW_MODE_LOG) {
    $extra = '-log';
  }
  elseif ($mode == MFCS_VIEW_MODE_OPERATIONS) {
    $extra = '-operations';
  }
  else {
    return FALSE;
  }

  if (!is_null($revision_id)) {
    $extra .= '-revision-' . $revision_id;
  }

  return sprintf('%\'010s', $request_id) . '-' . $name . '-' . $fixed_title . $extra . '.pdf';
}

/**
 * @} End of '@addtogroup mfcs'.
 */
