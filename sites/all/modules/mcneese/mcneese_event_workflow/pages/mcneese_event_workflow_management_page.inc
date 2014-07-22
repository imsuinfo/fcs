<?php

/**
 * @file
 * Defines McNeese event workflow management page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event manage page.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see mcneese_event_workflow_blocks_at_path()
 */
function mcneese_event_workflow_management_page() {
  global $base_path;
  $user = cf_current_user();

  $can_create = mcneese_event_workflow_management_page_event_access('create');
  $can_edit = mcneese_event_workflow_management_page_event_access('edit');
  $can_view = mcneese_event_workflow_management_page_event_access('view');
  $can_list = mcneese_event_workflow_management_page_event_access('list');
  $can_search = mcneese_event_workflow_management_page_event_access('search');
  $can_delete = mcneese_event_workflow_management_page_event_access('delete');
  $can_log = mcneese_event_workflow_management_page_event_access('log');
  $can_review = mcneese_event_workflow_management_page_event_access('review');

  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $items_1 = array();
  $items_2 = array();
  $items_3 = array();
  $items_4 = array();

  if ($can_create) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/create-0" class="item-link">Request Event</a></h4>';
    $help = '<div class="item-description inline-block">Make a request to use the facilities or host special events at McNeese State University.</div>';
    $items_1[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_list) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/list-0" class="item-link">List Requests</a></h4>';
    $help = '<div class="item-description inline-block">View a list of event requests available to the current user.</div>';
    $items_1[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_search) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/search-0" class="item-link">Search Requests</a></h4>';
    $help = '<div class="item-description inline-block">Use advanced search tools to help locate and find one or more event requests.</div>';
    $items_3[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($can_review) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/review-0" class="item-link">Review Requests</a></h4>';
    $help = '<div class="item-description inline-block">Manage requests the current user is allowed to review.</div>';
    $items_2[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($manager) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'events/reviewers-0" class="item-link">Manage Reviewers</a></h4>';
    $help = '<div class="item-description inline-block">Manage what users are allowed to approve/deny event requests by event type.</div>';
    $items_2[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  $item_list_1 = theme('item_list', array('items' => $items_1, 'type' => 'ul'));
  $item_list_2 = theme('item_list', array('items' => $items_2, 'type' => 'ul'));
  $item_list_3 = theme('item_list', array('items' => $items_3, 'type' => 'ul'));

  $markup = '<div id="mcneese_event_workflow-management_page-choices" class="mcneese_event_workflow-management_page-choices">';
  $markup .= '<h3 class="mcneese_event_workflow-management_page-choices-header">Facilities Use and Special Event Request Form</h3>';
  $markup .= '<div class="mcneese_event_workflow-management_page-choices-list_1">' . $item_list_1 . '</div>';
  $markup .= '<div class="mcneese_event_workflow-management_page-choices-list_2">' . $item_list_2 . '</div>';
  $markup .= '<div class="mcneese_event_workflow-management_page-choices-list_3">' . $item_list_3 . '</div>';
  $markup .= '</div>';


  if ($can_list) {
    mcneese_event_workflow_include(4);

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

    $and = array();

    $target_field_name = 'field_dates-date-start-0';
    $and[$target_field_name]['group_name'] = 'dates';
    $and[$target_field_name]['field_name'] = 'date';
    $and[$target_field_name]['column'] = 'value';
    $and[$target_field_name]['search'] = $absolute_start;
    $and[$target_field_name]['type'] = 'date';
    $and[$target_field_name]['multiple'] = TRUE;
    $and[$target_field_name]['operator'] = MEW_OPERATOR_GREATER_THAN_EQUAL;

    $target_field_name = 'field_dates-date-stop-0';
    $and[$target_field_name]['group_name'] = 'dates';
    $and[$target_field_name]['field_name'] = 'date';
    $and[$target_field_name]['column'] = 'value';
    $and[$target_field_name]['search'] = $absolute_stop;
    $and[$target_field_name]['type'] = 'date';
    $and[$target_field_name]['multiple'] = TRUE;
    $and[$target_field_name]['operator'] = MEW_OPERATOR_LESS_THAN;

    $search['and_0_field'] = $and;

    $sorting = array(
      'order' => 'date',
      'sort' => 'ASC',
    );

    $results = mcneese_event_workflow_event_load_listing($search, FALSE, 0, 0, $sorting);

    $rows = array();
    $rows_count = 0;
    if (!empty($results)) {
      foreach ($results as $item) {
        $can_view_event = mcneese_event_workflow_management_page_event_access('view', $item->id);
        if (!$can_view_event) {
          continue;
        }

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
              'href' => $base_path . 'events/view-0/' . $item->id,
              'title' => $item_title,
              'tooltip' => '[' . check_plain($item->location_name) . '] ' . check_plain($item->building_name) . ' ' . check_plain($item->room_name) . ': ' . $item_title,
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

    $title = '<h3><a href="' . $base_path . 'events/calendar-0/month">' . "Events for " . date("F, Y", $month_start) . '</a></h3>';
    $result = mcneese_event_workflow_build_calendar_month_markup($rows, $month_start, $month_stop, $absolute_start, $absolute_stop, $title);
    if ($result !== FALSE) {
      $markup .= $result;
    }
    unset($result);
  }

  return $markup;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
