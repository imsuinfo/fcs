<?php

/**
 * @file
 * Defines McNeese facilities use event calendar page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use event calendar month page.
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
function mfcs_event_calendar_month_0_page($year = NULL, $month = NULL) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($month) && !is_string($month)) {
    cf_error::invalid_string('month');

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

  if (is_null($year) && is_null($month)) {
    $month_start = strtotime('midnight first day of', REQUEST_TIME);
    $month_end = strtotime('midnight last day of', REQUEST_TIME);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }
  else {
    if (is_null($month)) {
      $month = date('F', REQUEST_TIME);
    }
    else {
      $month_stamp = strtotime($month);
      $month_name = date('F', $month_stamp);

      if ($month_name === FALSE) {
        cf_error::invalid_string('month');

        drupal_not_found();
        drupal_exit();
      }

      $month_left = strtolower($month);
      $month_right = strtolower($month_name);

      if ($month_left != $month_right) {
        cf_error::invalid_string('month');

        drupal_not_found();
        drupal_exit();
      }

      $month = $month_name;
    }

    if (is_null($year)) {
      $year = date('Y', REQUEST_TIME);
    }
    else {
      $year_stamp = strtotime($month . ' ' . $year);
      $year_name = date('Y', $year_stamp);

      if ($year_name === FALSE) {
        cf_error::invalid_string('year');

        drupal_not_found();
        drupal_exit();
      }

      if ($year != $year_name) {
        cf_error::invalid_string('year');

        drupal_not_found();
        drupal_exit();
      }

      $year = $year_name;
    }

    $month_start = strtotime('midnight first day of ' . $month . ' ' . $year);
    $month_end = strtotime('midnight last day of ' . $month . ' ' . $year);
    $month_stop = strtotime('midnight tomorrow', $month_end);
  }

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

  $results = mfcs_event_load_listing($search, FALSE, 0, 0, $sorting);

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

        if (!isset($rows[$month_day][$item->id])) {
          $item_title = check_plain($item->title);
          $rows[$month_day][$item->id] = array(
            'href' => $base_path . 'events/view-0/' . $item->id,
            'title' => $item_title,
            'tooltip' => '[' . check_plain($item->location_name) . '] ' . check_plain($item->building_name) . ' ' . check_plain($item->room_name) . ': ' . $item_title,
            'date' => $date,
            'month' => $month,
            'day' => $day,
          );
          unset($item_title);
        }
      }
    }
  }

  $markup = '';
  $title = '<h3>' . "Events for " . date("F, Y", $month_start) . '</h3>';
  $result = mfcs_build_calendar_month_markup($rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title);
  if ($result !== FALSE) {
    $markup .= $result;
  }
  unset($result);

  return $markup;
}

/**
 * Provides the facilities use event calendar month page.
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
function mfcs_event_calendar_day_0_page($year = NULL, $month = NULL, $day = NULL) {
  if (!is_null($year) && !cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($month) && !cf_is_string($month)) {
    cf_error::invalid_string('month');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year) && !is_null($month)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_path;

  $user = cf_current_user();

