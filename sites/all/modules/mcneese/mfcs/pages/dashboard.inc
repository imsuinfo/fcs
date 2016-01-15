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

  // access check is performed here instead of via the hook_menu() so that different responses may be provided based on access control situations.
  $user_account_new_banner_access_problem = FALSE;
  if (mfcs_dashboard_page_access() === FALSE) {
    $mfcs_users = mfcs_load_users($user->uid);

    if (is_array($mfcs_users) && !empty($mfcs_users[$user->uid]) && is_object($mfcs_users[$user->uid]) && ($mfcs_users[$user->uid]->problem & MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE) != 0) {
      $user_account_new_banner_access_problem = TRUE;
    }
    else {
      drupal_access_denied();
      drupal_exit();
      return FALSE;
    }
  }

  $can_create = mfcs_page_request_access('create', NULL, $user);
  $can_list = mfcs_page_request_access('list', NULL, $user);
  $can_search = mfcs_page_request_access('search', NULL, $user);
  $can_review = mfcs_page_request_access('review', NULL, $user);

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

  $markup = '<div id="mfcs-dashboard_page-choices" class="mfcs-dashboard_page-choices">' . "\n";

  if (!$can_create && !$can_list && !$can_search && !$can_review) {
    watchdog('insufficient access', "The user :user_name (:user_id) does not have sufficient access to use the system.", array(':user_name' => $user->name, ':user_id' => $user->uid), WATCHDOG_NOTICE);
    $markup .= '<h3 class="mfcs-dashboard_page-choices-header">Insufficient Access</h3>' . "\n";

    $markup .= '<div>' . "\n";
    $markup .= 'You lack sufficient access to view or interact with the content on this website.<br>' . "\n";
    $markup .= '<br>' . "\n";
    $markup .= '<strong>Have you registered for the current or upcoming semester?</strong><br>' . "\n";
    $markup .= 'Between semesters, students who have not regestered for the next semester may not have access to the system until they have completed their registration.<br>' . "\n";
    $markup .= '<br>' . "\n";

    if ($user_account_new_banner_access_problem) {
      $markup .= 'The system has detected that there were problems creating your account.<br>' . "\n";
      $markup .= 'This <em>account creation process</em> relies on additional technology that may be unavailable between 4:00am and 5:30am.<br>' . "\n";
      $markup .= '<br>' . "\n";
      $markup .= '<strong>Please try re-logging in before 4:00am or after 5:30am for your account to be properly created.</strong><br>' . "\n";
      $markup .= '<br>' . "\n";
      $markup .= 'Once your account is properly created, you can log in at any time to access the system.<br>' . "\n";
    }
    else {
      $markup .= 'If you believe this is in error, then please contact the support staff through the proper channels.' . "\n";
    }

    $markup .= '</div>' . "\n";
  }
  else {
    $markup .= '<h3 class="mfcs-dashboard_page-choices-header">Facilities Use and Special Event Request System</h3>' . "\n";
  }

  if (!empty($items_1)) {
    $markup .= '<div class="mfcs-dashboard_page-choices-list_1">' . $item_list_1 . '</div>' . "\n";
  }

  if (!empty($items_2)) {
    $markup .= '<div class="mfcs-dashboard_page-choices-list_2">' . $item_list_2 . '</div>' . "\n";
  }

  if (!empty($items_3)) {
    $markup .= '<div class="mfcs-dashboard_page-choices-list_3">' . $item_list_3 . '</div>' . "\n";
  }

  $markup .= '</div>' . "\n";


  if ($can_list) {
    mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

    $instance = mfcs_instance();

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

    $rows = array();
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
              if (array_key_exists('limit', $options) && $options['limit'] > 0 && $month_day_count == $options['limit']) {
                $year = date('Y', $date);
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

    $title = '<h3><a href="' . $base_path . 'requests/calendar-0/month' . $url_arguments . '">' . "Requests for " . date("F, Y", $month_start) . '</a></h3>';
    $result = mfcs_build_calendar_month_markup($rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, 'mfcs-calendar-0-month', 3);
    if ($result !== FALSE) {
      $markup .= $result;
    }
    unset($result);
  }

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
