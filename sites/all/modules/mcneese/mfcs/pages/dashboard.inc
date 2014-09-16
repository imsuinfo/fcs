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
 *
 * @see mfcs_blocks_at_path()
 */
function mfcs_dashboard_page() {
  global $base_path;
  $user = cf_current_user();

  mfcs_add_canonical_headers('requests');

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
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/create-0" class="item-link">New Request</a></h4>';
    $help = '<div class="item-description inline-block">Make a request to use the facilities or host special events at McNeese State University or related facilities.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_list) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/list-0" class="item-link">List Requests</a></h4>';
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
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/review-0" class="item-link">Review Requests</a></h4>';
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
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/search-0" class="item-link">Search Requests</a></h4>';
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
  $markup .= '<h3 class="mfcs-dashboard_page-choices-header">Facilities Use and Special Event Request Form</h3>';

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

    $month_start = strtotime('midnight first day of', REQUEST_TIME);
    $month_end = strtotime('midnight last day of', REQUEST_TIME);
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

    mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);
    $search = mfcs_miscellaneous_generate_requests_by_date_search($absolute_start, $absolute_stop);

    $sorting = array(
      'order' => 'date',
      'sort' => 'ASC',
    );

    $results = mfcs_request_load_listing($search, FALSE, 0, 0, $sorting);

    $rows = array();
    $rows_count = 0;
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

          if (isset($rows[$month_day][$item->id])) {
            $rows[$month_day][$item->id]['time_start'][] = array('start' => $item->time_start, 'stop' => $item->time_stop);
          }
          else {
            $item_title = check_plain($item->title);
            $rows[$month_day][$item->id] = array(
              'href' => $base_path . 'requests/view-0/' . $item->id,
              'title' => $item_title,
              'tooltip' => '[' . $item->id . '] ' . check_plain($item->building_name) . ' ' . check_plain($item->room_name) . ': ' . $item_title,
              'date' => $date,
              'month' => $month,
              'day' => $day,
              'times' => array(array('start' => $item->time_start, 'stop' => $item->time_stop)),
            );
            unset($item_title);
          }
        }
      }
    }

    mfcs_include(MFCS_INCLUDE_OUTPUT);

    $title = '<h3><a href="' . $base_path . 'requests/calendar-0/month">' . "Requests for " . date("F, Y", $month_start) . '</a></h3>';
    $result = mfcs_build_calendar_month_markup($rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title, 'mfcs-calendar-0-month');
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