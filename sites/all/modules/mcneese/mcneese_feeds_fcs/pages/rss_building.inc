<?php

/**
 * @file
 * Defines rss feed listing building functions.
 *
 */

/**
 * @addtogroup mcneese_feeds_fcs
 * @{
 */

/**
 * Output the RSS Feed "Room".
 *
 * @param string $buildings
 *   A string containing comma separated numeric building ids.
 * @param string $date
 *   A string containing one of the following:
 *     'all', 'past', 'present', 'future', 'Y-m-d', 'y-m', 'y'.
 * @param string $limit
 *   A string or number representing the limits.
 *   The string 'all' represents no limit.
 */
function mcneese_feeds_fcs_rss_building($buildings, $date, $limit) {
  if (!is_string($buildings) || empty($buildings)) {
    cf_error::invalid_string('buildings');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_string($date) || empty($date)) {
    cf_error::invalid_string('date');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_string($limit) || is_numeric($limit) && $limit < 0 || empty($limit)) {
    cf_error::invalid_string('limit');
    drupal_not_found();
    drupal_exit();
  }

  $parts_buildings = explode(',', $buildings);
  $all_buildings = array();
  foreach ($parts_buildings as $part_building) {
    if (is_numeric($part_building)) {
      $all_buildings[(int) $part_building] = (int) $part_building;
    }
  }
  unset($parts_buildings);

  $date_year = NULL;
  $date_month = NULL;
  $date_day = NULL;
  $date_start = NULL;
  $date_stop = NULL;
  if ($date != 'all' && $date != 'past' && $date != 'present' && $date != 'future') {
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

    $parts_date = explode('-', $date);
    if (count($parts_date) > 3 || !is_numeric($parts_date[0]) || $parts_date[0] < 2015 || $parts_date[0] > 2065) {
      cf_error::invalid_string('date');
      drupal_not_found();
      drupal_exit();
    }

    $date_year = (int) $parts_date[0];
    if (isset($parts_date[1])) {
      if (is_numeric($parts_date[1])) {
        if ($parts_date[1] < 1 || $parts_date[1] > 12) {
          cf_error::invalid_string('date');
          drupal_not_found();
          drupal_exit();
        }

        $date_month = $valid_month_names[(int) $parts_date[1]];
      }
      else {
        if (!in_array(strtolower($parts_date[1]), $valid_month_names)) {
          cf_error::invalid_string('date');
          drupal_not_found();
          drupal_exit();
        }

        $date_month = $parts_date[1];
      }
    }

    if (isset($parts_date[2])) {
      if (!is_numeric($parts_date[2]) || $parts_date[2] < 1 || $parts_date[2] > 31) {
        cf_error::invalid_string('date');
        drupal_not_found();
        drupal_exit();
      }

      $date_day = (int) $parts_date[2];
    }

    if (is_null($date_month)) {
      $date_start = strtotime('midnight january 1, ' . $date_year);
      $date_stop = strtotime('next year' . $date_year, $date_start);
    }
    elseif (is_null($date_day)) {
      $date_start = strtotime('midnight ' . $date_month . ' 1, ' . $date_year);
      $date_stop = strtotime('next month' . $date_year, $date_start);
    }
    else {
      $date_start = strtotime('midnight ' . $date_month . ' ' . $date_day . ', ' . $date_year);
      $date_stop = strtotime('tomorrow' . $date_year, $date_start);
    }
  }
  else {
    if ($date == 'past') {
      $date_stop = strtotime('now');
    }
    elseif ($date == 'present') {
      $date_start = strtotime('midnight today');
      $date_stop = strtotime('midnight tomorrow');
    }
    elseif ($date == 'future') {
      $date_start = strtotime('now');
    }
  }

  global $base_url;
  global $base_path;
  global $user;


  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

  $type_options = mfcs_get_request_type_list_options();
  $room_options = mfcs_get_request_room_list_options(NULL, NULL, TRUE);
  $building_options = mfcs_get_request_building_list_options(NULL, NULL, TRUE);
  $location_options = mfcs_get_request_location_list_options(NULL, FALSE, TRUE);
  $request_classifications = mfcs_get_request_classification_list_options();


  require_once(drupal_get_path('module', 'mcneese_feeds') . '/classes/rss.inc');

  $headers = array();
  $output = '';
  $rows = array();
  $categories = array();
  $feed_url = 'rss/feed/building/' . $buildings. '/' . $date . '/' . $limit;

  $channel = new McNeeseFeedsRssChannel();
  $channel->assign_common_required(MCNEESE_FEEDS_FCS_TITLE . " - Building Feed", $base_url, "A listing of reserved buildings based on the requested url arguments.");
  $channel->assign_common_optional(NULL, REQUEST_TIME, date("Y", REQUEST_TIME) . "© McNeese State University");

  // The date of the most recent item on the feed should be used here.
  $original_timezone = date_default_timezone_get();

  try {
    $conditions = mfcs_miscellaneous_generate_requests_by_date_search($date_start, $date_stop);

    $conditions['and_1']['value_building'] = array(
        'field' => 'mer.building',
        'operator' => MFCS_OPERATOR_IS_IN,
        'search' => $all_buildings,
    );

    $options = array(
      'sort' => array('date' => 'ASC'),
      'distinct_id' => TRUE,
    );

    if ($limit == 'all') {
      $options['limit'] = 0;
    }
    else {
      $options['limit'] = (int) $limit;
    }

    $items_returned = mfcs_request_load_listing($conditions, $options);
    $items = array();
    if (isset($items_returned['results']) && is_array($items_returned['results'])) {
      $items = $items_returned['results'];
    }

    $categories = array(
      'event',
    );

    $channel->set_title(MCNEESE_FEEDS_FCS_TITLE . " - Building Feed");
    $channel->set_description("A listing of reserved buildings based on the requested url arguments.");

    if (!empty($items)) {
      foreach ($items as $key => $item) {
        $feed_item = new McNeeseFeedsRssItem();

        date_default_timezone_set('UTC');
        date_default_timezone_set($original_timezone);

        $location = "";
        if (!empty($item->location)) {
          if (isset($location_options[$item->location])) {
            $location = check_plain($location_options[$item->location]);
          }
        }

        $building = "";
        if (!empty($item->building)) {
          if (isset($building_options[$item->building])) {
            $building = check_plain($building_options[$item->building]);
          }
        }

        $room = "";
        if (!empty($item->room)) {
          if (isset($room_options[$item->room])) {
            $room = check_plain($room_options[$item->room]);
          }
        }

        $dates = '';
        if (isset($item->date) && is_array($item->date)) {
          foreach ($item->date as $delta => $date) {
            $dates .= '<li>';
            $dates .= check_plain(date("Y/m/d", $date));
            if (array_key_exists($delta, $item->time_start) && !is_null($item->time_start[$delta]) && array_key_exists($delta, $item->time_stop) && !is_null($item->time_stop[$delta])) {
              $dates .= ' ' . check_plain(date("h:i a", $item->time_start[$delta]));
              $dates .= ' to ' . check_plain(date("h:i a", $item->time_stop[$delta]));
            }
            $dates .= '</li>' . "\n";
          }
        }

        $markup = '';
        $markup .= '<strong>Request ID:</strong><ul><li>' . $item->id .'</li></ul>' . "<br>\n";
        $markup .= '<strong>Request Name:</strong><ul><li>' . check_plain($item->title) . '</li></ul>' . "<br>\n";
        $markup .= '<strong>Location:</strong><ul><li>' . $location . '</li></ul>' . "<br>\n";
        $markup .= '<strong>Building:</strong><ul><li>' . $building . '</li></ul>' . "<br>\n";
        $markup .= '<strong>Room:</strong><ul><li>' . $room . '</li></ul>' . "<br>\n";
        $markup .= '<strong>Dates:</strong><ul>' . $dates . '</ul>' . "<br>\n";
        $markup .= '<strong>Request URL:</strong><ul><li><a href="' . $base_url . '/requests/view-0/' . $item->id .'">' . $base_url . '/requests/view-0/' . $item->id . '</a></li></ul>' . "<br>\n";

        $feed_item->assign_common_required($item->title, $base_url . '/requests/view-0/' . $item->id, $markup);
        $feed_item->assign_common_optional($markup, $item->created, date("Y", $item->updated) . "© McNeese State University", $categories);
        $feed_item->assign_optional('request:' . $item->id, FALSE);

        $channel->append_item($feed_item);
      }
    }
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    watchdog('mcneese_feeds_fcs', "Failed to generate RSS Feed Channel", array(), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    watchdog('mcneese_feeds_fcs', "Failed to generate RSS Feed Channel", array(), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  $channel->assign_required($base_url . '/');
  $channel->assign_optional('en-US', $base_url . '/' . $feed_url, TRUE, REQUEST_TIME, array('url' => $base_url . '/sites/all/themes/mcneese/images/logo-blue.png', 'title' => "McNeese State University", 'link' => $base_url . '/'));

  $output = $channel->get_feed();
  if ($output === FALSE) {
    watchdog('mcneese_feeds_fcs', "Failed to generate RSS Feed Channel", array(), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  mcneese_feeds_send_output($feed_url, $output);
}

/**
 * @} End of '@addtogroup mcneese_feeds_fcs'.
 */
