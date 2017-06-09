<?php

/**
 * @file
 * Defines McNeese facilities use dashboard page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request dashboard page.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_dashboard_page() {
  global $base_path;
  global $mfcs_determined;
  $user = cf_current_user();

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests');

  $instance = mfcs_instance();

  // access check is performed here instead of via the hook_menu() so that different responses may be provided based on access control situations.
  $user_account_new_banner_access_problem = FALSE;
  if (mfcs_dashboard_page_access() === FALSE) {
    if (!empty($user->mfcs) && is_array($user->mfcs) && array_key_exists('problems', $user->mfcs)) {
      // continue to check access so long as the user has insufficient access.
      if (($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS) != 0) {
        // only due this when banner is expected to be available.
        $backup_begin = strtotime('12:00am', $instance);
        $backup_end = strtotime('4:00am', $instance);

        if ($instance < $backup_begin || $instance > $backup_end) {
          mfcs_synchronize_user_settings($user, array(), TRUE, TRUE, TRUE);
        }
      }

      if (($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE) != 0) {
        $user_account_new_banner_access_problem = TRUE;
      }
    }
  }

  $can_create = mfcs_page_request_access('create', NULL, $user);
  $can_list = mfcs_page_request_access('list', NULL, $user);
  $can_search = mfcs_page_request_access('search', NULL, $user);
  $can_review = mfcs_page_request_access('review', NULL, $user);
  $can_rss = mfcs_page_generic_access('rss'); // @todo: find a way to handle RSS feeds without implementing apaches HTTP login access (perhaps do it via PHP?).

  $items_1 = array();
  $items_2 = array();
  $items_3 = array();
  $items_4 = array();

  $items_array = &$items_1;
  $items_array_current = 1;

  if ($can_create) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/create-0' . $url_arguments . '" class="item-link">Create Request</a></h4>';
    $help = '<div class="item-description inline-block">Make a request to use the facilities or host special events at McNeese State University or related facilities.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_list) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/list-0' . $url_arguments . '" class="item-link">List Requests</a></h4>';
    $help = '<div class="item-description inline-block">View a list of requests available to the current user.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if (count($items_array) > 1) {
    if ($items_array_current == 1) {
      $items_array = &$items_2;
      $items_array_current++;
    }
    elseif ($items_array_current == 2) {
      $items_array = &$items_3;
      $items_array_current++;
    }
    elseif ($items_array_current == 3) {
      $items_array = &$items_4;
      $items_array_current++;
    }
  }

  if ($can_review) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/review-0' . $url_arguments . '" class="item-link">Review Requests</a></h4>';
    $help = '<div class="item-description inline-block">Manage requests the current user is allowed to review.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if (count($items_array) > 1) {
    if ($items_array_current == 1) {
      $items_array = &$items_2;
      $items_array_current++;
    }
    elseif ($items_array_current == 2) {
      $items_array = &$items_3;
      $items_array_current++;
    }
    elseif ($items_array_current == 3) {
      $items_array = &$items_4;
      $items_array_current++;
    }
  }

  if ($can_search) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/search-0' . $url_arguments . '" class="item-link">Search Requests</a></h4>';
    $help = '<div class="item-description inline-block">Use advanced search tools to help locate and find one or more requests.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if (count($items_array) > 1) {
    if ($items_array_current == 1) {
      $items_array = &$items_2;
      $items_array_current++;
    }
    elseif ($items_array_current == 2) {
      $items_array = &$items_3;
      $items_array_current++;
    }
    elseif ($items_array_current == 3) {
      $items_array = &$items_4;
      $items_array_current++;
    }
  }

  $item_list_1 = theme('item_list', array('items' => $items_1, 'type' => 'ul'));
  $item_list_2 = theme('item_list', array('items' => $items_2, 'type' => 'ul'));
  $item_list_3 = theme('item_list', array('items' => $items_3, 'type' => 'ul'));

  $markup = '<div id="mfcs-dashboard_page-choices" class="mfcs-dashboard_page-choices">';

  if (!$can_create && !$can_list && !$can_search && !$can_review) {
    watchdog('insufficient access', 'The user :user_name (:user_id) does not have sufficient access to use the system.', array(':user_name' => $user->name, ':user_id' => $user->uid), WATCHDOG_NOTICE);
    $markup .= '<h3 class="mfcs-dashboard_page-choices-header">Insufficient Access</h3>';

    $markup .= '<div>';
    $markup .= 'You lack sufficient access to view or interact with the content on this website.<br>';
    $markup .= '<br>';
    $markup .= '<strong>Have you registered for the current or upcoming semester?</strong><br>';
    $markup .= 'Between semesters, students who have not regestered for the next semester may not have access to the system until they have completed their registration.<br>';
    $markup .= '<br>';

    if ($user_account_new_banner_access_problem) {
      $markup .= 'The system has detected that there were problems creating your account.<br>';
      $markup .= 'This <em>account creation process</em> relies on additional technology that may be unavailable between 4:00am and 5:30am.<br>';
      $markup .= '<br>';
      $markup .= '<strong>Please try re-logging in before 4:00am or after 5:30am for your account to be properly created.</strong><br>';
      $markup .= '<br>';
      $markup .= 'Once your account is properly created, you can log in at any time to access the system.<br>';
    }
    else {
      $markup .= 'If you believe this is in error, then please contact the support staff through the proper channels.';
    }

    $markup .= '</div>';

    if ($user->status != 0) {
      if (!empty($user->mfcs) && is_array($user->mfcs) && array_key_exists('problems', $user->mfcs)) {

        // only assign no access problem flag when the user account has not been flagged as unable to sync with banner.
        if (($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE) == 0 && ($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE) == 0) {
          if (($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID) == 0 || ($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_BANNER_NOT_IN_LDAP) == 0 || ($user->mfcs['problems'] & MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES) == 0) {
            $user->mfcs['problems'] = $user->mfcs['problems'] | MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS;
            mfcs_save_user($user->uid, array('problems' => $user->mfcs['problems']));
          }
        }
      }
      else {
        mfcs_save_user($user->uid, array('problems' => MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS));
        $user->mfcs['problems'] = MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS;
      }
    }
  }
  else {
    $markup .= mfcs_dashboard_page_updated_1_2_message();
    $markup .= '<h3 class="mfcs-dashboard_page-choices-header">Facilities Use and Special Event Request System</h3>';
  }

  if (!empty($items_1)) {
    $markup .= '<div class="mfcs-dashboard_page-choices-list_1">' . $item_list_1 . '</div>';
  }

  if (!empty($items_2)) {
    $markup .= '<div class="mfcs-dashboard_page-choices-list_2">' . $item_list_2 . '</div>';
  }

  if (!empty($items_3)) {
    $markup .= '<div class="mfcs-dashboard_page-choices-list_3">' . $item_list_3 . '</div>';
  }

  $markup .= '</div>';


  if ($can_list) {
    mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

    $year = date('Y', $instance);

    $month_start = strtotime('midnight first day of', $instance);
    $month_end = strtotime('midnight last day of', $instance);
    $month_stop = strtotime('midnight tomorrow', $month_end);

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

    $search_building_ids = NULL;
    if (!empty($mfcs_determined['filters']['field_top-building-0']['search'])) {
      $search_building_ids = $mfcs_determined['filters']['field_top-building-0']['search'];
    }


    // load the default calendar
    $default_calendar = (int) variable_get('mfcs_user_calendar_default', MFCS_USER_CALENDAR_FULL);

    // dashboard assigns a separate theme for the original theme, however the full calander can and should display all unavailabilities.
    // a separate structure is used for holidays for both original and full calendar.
    $calendar = $default_calendar;
    if (isset($user->mfcs['calendar']) && is_numeric($user->mfcs['calendar'])) {
      $calendar = (int) $user->mfcs['calendar'];
    }

    if ($calendar === MFCS_USER_CALENDAR_NONE) {
      if ($default_calendar === MFCS_USER_CALENDAR_FULL) {
        $calendar = $default_calendar;
      }
      else {
        $calendar = $default_calendar;
      }
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
        $holidays_previous = mfcs_get_holidays($year - 1, $absolute_start, $year_start, TRUE, FALSE, FALSE, TRUE);
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
        $unavailabilities = mfcs_get_unavailabilities($year - 1, $search_building_ids, NULL, $absolute_start, $year_start, TRUE, FALSE, TRUE, TRUE);
        if (is_array($unavailabilities_previous)) {
          foreach ($unavailabilities_previous as $unavailabilities_key => $unavailabilities_value) {
            $holidays[$unavailabilities_key] = $unavailabilities_value;
          }
        }
        unset($unavailabilities_key);
        unset($unavailabilities_value);
      }
    }

    if ($absolute_stop > $year_stop) {
      if ($show_holidays) {
        $holidays_next = mfcs_get_holidays($year + 1, $year_stop, $absolute_stop, TRUE, FALSE, FALSE, TRUE);
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
        $unavailabilities = mfcs_get_unavailabilities($year + 1, NULL, $search_building_ids, $year_stop, $absolute_stop, TRUE, FALSE, TRUE, TRUE);
        if (is_array($unavailabilities_next)) {
          foreach ($unavailabilities_next as $unavailabilities_key => $unavailabilities_value) {
            $holidays[$unavailabilities_key] = $unavailabilities_value;
          }
        }
        unset($unavailabilities_key);
        unset($unavailabilities_value);
      }
    }

    $rows = array();
    foreach ($holidays as $holiday) {
      if ($calendar == MFCS_USER_CALENDAR_ORIGINAL || ($calendar == MFCS_USER_CALENDAR_NONE && $default_calendar === MFCS_USER_CALENDAR_ORIGINAL)) {
        $date_current = $holiday->date_start;
        while ($date_current < $holiday->date_stop) {
          $day = date('j', $date_current);
          $month = date('n', $date_current);
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
            //'description' => str_replace("\n", '<br>', check_plain($holiday->description)),
            'year' => $year,
            'month' => $month,
            'day' => $day,
            'date' => $date_current,
            'date_range_start' => $holiday->date_start,
            'date_range_stop' => $holiday->date_stop,
          );

          $date_current = strtotime('midnight tomorrow', $date_current);
        }
      }
      else {
        $date_current = $holiday->date_start;

        $day = date('j', $date_current);
        $month = date('n', $date_current);
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
          //'description' => str_replace("\n", '<br>', check_plain($holiday->description)),
          'year' => $year,
          'month' => $month,
          'day' => $day,
          'date' => $date_current,
          'date_range_start' => $holiday->date_start,
          'date_range_stop' => $holiday->date_stop,
        );
      }
    }
    unset($holiday);
    unset($holidays);

    if ($calendar == MFCS_USER_CALENDAR_ORIGINAL || ($calendar == MFCS_USER_CALENDAR_NONE && $default_calendar === MFCS_USER_CALENDAR_NONE)) {
      foreach ($unavailabilities as $unavailability) {
        $date_current = $unavailability->date_start;
        while ($date_current < $unavailability->date_stop) {
          $day = date('j', $date_current);
          $month = date('n', $date_current);
          $month_day = $month . '-' . $day;
          if (!isset($rows[$month_day])) {
            $rows[$month_day] = array();
          }

          $rows[$month_day]['unavailability-all'] = array(
            'is' => 'unavailable',
            'id' => 'unavailability-all',
            'title' => 'Some rooms are unavailable.',
            'title-raw' => 'Some rooms are unavailable.',
            'tooltip' => 'View the current day to see all room unavailabilities',
            'year' => $year,
            'month' => $month,
            'day' => $day,
            'date' => $unavailability->date_start,
            'type' => NULL,
          );

          $date_current = strtotime('midnight tomorrow', $date_current);
        }
        unset($date_current);
      }
    }
    else {
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
            'href' => $base_path . 'unavailable-0/view/' . $year . '/' . $unavailability->room_id . '/' . $unavailability->id,
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
        unset($date_current);
        unset($building_code);
        unset($building_name);
        unset($room_number);
        unset($room_name);
        unset($description);
        unset($tooltip);
      }
    }

    $current_date = $absolute_start;
    while ($current_date < $absolute_stop) {
      $next_date = strtotime('+1 days', $current_date);

      $search['field_dates-date-start-0']['search'] = $current_date;
      $search['field_dates-date-stop-0']['search'] = $next_date;

      $options = array(
        'limit' => 4,
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
              $rows[$month_day][$item->id]['times'][$delta]['start'] = $item->time_start[$delta];
              $rows[$month_day][$item->id]['times'][$delta]['stop'] = $item->time_stop[$delta];
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
              if (array_key_exists('limit', $options) && $options['limit'] > 0 && $month_day_count == $options['limit']) {
                $year = date('Y', $date);
                $month_name = date('F', $date);
                $month_name_lower = strtolower($month_name);

                $rows[$month_day][$item->id] = array(
                  'is' => 'more',
                  'href' => $base_path . 'requests/calendar-0/day/' . $year . '/' . $month_name_lower . '/' . $day . $url_arguments,
                  'title' => '. . .',
                  'title-raw' => '. . .',
                  'tooltip' => 'View more requests on ' . $month_name . ' ' . $day . ', ' . $year,
                  'date' => $date,
                  'month' => $month,
                  'day' => $day,
                  'times' => FALSE,
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
                  'times' => array(),
                );

                $rows[$month_day][$item->id]['times'][$delta]['start'] = $item->time_start[$delta];
                $rows[$month_day][$item->id]['times'][$delta]['stop'] = $item->time_stop[$delta];

                unset($item_title);
              }
            }
          }
        }
      }

      $current_date = $next_date;
    }

    mfcs_include(MFCS_INCLUDE_OUTPUT);

    $title = '<h3><a href="' . $base_path . 'requests/calendar-0/month' . $url_arguments . '">Requests for ' . date('F, Y', $month_start) . '</a></h3>';
    $result = mfcs_build_calendar_month_markup($calendar, $rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, 'mfcs-calendar-0-month', 3);
    if ($result !== FALSE) {
      $markup .= $result;
    }
    unset($result);
  }

  return $markup;
}

/**
 * Add a temporary update message.
 *
 * This will be displayed one time per user and then only if it is before July 1, 2017.
 *
 * This sort of abuses colorbox (which is generally intended for images) to quickly and easily display a full-page popup.
 *
 * @return string
 *   The generated HTML markup string.
 */
function mfcs_dashboard_page_updated_1_2_message() {
  $instance = mfcs_instance();

  if ($instance >= strtotime('July 1, 2017')) {
    return '';
  }

  $colorbox_path = drupal_get_path('module', 'colorbox');
  drupal_add_js($colorbox_path . '/js/colorbox.js', array('group' => JS_LIBRARY));

  $markup = '<div id="mfcs_dashboard_page_updated_1_2_message_popup" class="system_updated_notice_1_2 colorbox-inline">';
  $markup .= '<h4>The Facilities Use System has now be updated to version 1.2!</h4>';
  $markup .= '<br>';

  $markup .= '<div>This fixes a number of issues observed, improves the calendar interaction, looks even cleaner on phones and other small devices, improves the terminology, and more...</div>';
  $markup .= '<br>';

  $markup .= '<h5>Highlights</h5>';
  $markup .= '<br>';

  $markup .= '<strong>Small-device friendly improvements to the theme.</strong>';
  $markup .= '<ul>';
  $markup .= '<li>Requests, Calendars, Listings, all adjust automatically to better fit smaller screen sizes.</li>';
  $markup .= '<li> No need to install an app on your phone, it just works!</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<strong>Requests may now have their dates and times cancelled or uncancelled after a Request has already had a final decision made.</strong>';
  $markup .= '<ul>';
  $markup .= '<li>This means no more cancelling the entire request and creating a new one because a single date is incorrect.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<strong>Requests may now be changed during any part of the process (depending on your access privileges):</strong>';
  $markup .= '<ul>';
  $markup .= '<li>This means no more restarting the entire review process to make minor corrections.</li>';
  $markup .= '<li>More complicated changes may now be performed by Managers.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<strong>Holidays and Room Unavailabilities feature provides a way to see and catch conflicts.</strong>';
  $markup .= '<ul>';
  $markup .= '<li>Holidays display warning messages to inform the user that there may be a conflict, but they do not prevent request submission.</li>';
  $markup .= '<li>Room Unavailabilities display error messages and prevent request submission.</li>';
  $markup .= '<li>These messages are even displayed in the review section so that reviewers may more easily notice the conflicts.</li>';
  $markup .= '<li>Holidays and Room Unavailabilities (including scheduled class conflicts) are now displayed on the calendar.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<strong>If you are a reviewer, then you will notice that the words "Approve" and "Deny" have been replaced with "Issues" and "No Issues" when reviewing a request.</strong>';
  $markup .= '<ul>';
  $markup .= '<li>Reviewers may now specify "Issues" or "No Issues" during the Final Approval step.</li>';
  $markup .= '<li>Reviewers may not specify "Issues" or "No Issues" as many times as they like until the Final Decision is made.</li>';
  $markup .= '<li>Either "Issues" or "No Issues" may be specified at least one time to designate that you have "Reviewed" the request.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<div><strong>If reviewers have forgotten to review a request, reminder e-mails are now periodically sent out prior to the first requested date.</strong></div>';
  $markup .= '<br>';

  $markup .= '<strong>New and improved user management pages.</strong>';
  $markup .= '<ul>';
  $markup .= '<li>It is now easier than ever to view and customize your user settings.</li>';
  $markup .= '<li>You may switch back to/from the old calendar (called Original Calendar) and the new calendar (called Full Calendar) themes.</li>';
  $markup .= '<li>You may now setup default filter options so that every time you re-login, your filters are pre-assigned.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<strong>Managers may now forcibly change the status of any request.</strong>';
  $markup .= '<ul>';
  $markup .= '<li>This means no more re-creating requests to make corrections or handle exceptional cases.</li>';
  $markup .= '<li>If special case situations comes up, just add a comment asking for a manager to perform some task.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<div><strong>Managers may now add custom Room Unavailabilities.</strong></div>';
  $markup .= '<br>';

  $markup .= '<strong>New Help and Documentation pages have been added to the site (and can be downloaded as a PDF or printed very easily).</strong>';
  $markup .= '<ul>';
  $markup .= '<li>Quick Guides provide a short and simple step-by-step instructions to quickly perform a signle task.</li>';
  $markup .= '<li>Documentation provides details explanations on the functionality and terminology provided by the system.</li>';
  $markup .= '</ul>';
  $markup .= '<br>';

  $markup .= '<div><strong>Enjoy! - Kevin Day, Site Administrator</strong></div>';
  $markup .= '<br>';
  $markup .= '</div>';



  $javascript = '(function ($) {' . "\n";
  $javascript .= 'Drupal.behaviors.mfcs_dashboard_page_updated_1_2_message = {' . "\n";
  $javascript .= '  attach:function(context) {' . "\n";

  #$javascript .= '$.colorbox({html:"' . preg_replace('/"/', '\"', $markup) . '", loop: false});';
  $javascript .= '$("#mfcs_dashboard_page_updated_1_2_message_popup_open").colorbox({inline: true, href: "#mfcs_dashboard_page_updated_1_2_message_popup", loop: false});';

  $javascript .= '    }' . "\n";
  $javascript .= '  }' . "\n";
  $javascript .= '})(jQuery);' . "\n";

  drupal_add_js($javascript, array('type' => 'inline', 'group' => JS_THEME, 'preprocess' => TRUE, 'cache' => FALSE));


  $markup_wrapper = '<center><h3 class="inline-block border-1 border-style-solid margin-3 padding-3 align_center" style="width: 80%; background-color: #eeffb5;"><a id="mfcs_dashboard_page_updated_1_2_message_popup_open">The Facilities Use System has now be updated to version 1.2!</a></h3><br>';
  $markup_wrapper .= '<div class="mfcs_dashboard_page_updated_1_2_message_popup_overlay" style="display: none">';
  $markup_wrapper .= $markup;
  $markup_wrapper .= '</div></center>';

  return $markup_wrapper;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
