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

  $administer = user_access('mcneese event workflow administer');
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
    $help = '<div class="item-description inline-block">Manage what users are allowed to approve/deny event requests by event type.</div>';
    $items_2[] = array(
      'id' => '',
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($administer || $manager) {
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
    $total_items = 5;

    // load sorting information
    $order = NULL;
    if (isset($_GET['order'])) {
      $order = strtolower($_GET['order']);
    }

    $sort = NULL;
    if (isset($_GET['sort'])) {
      $sort = strtoupper($_GET['sort']);

      if ($sort != 'ASC' && $sort != 'DESC') {
        $sort = NULL;
      }
    }

    try {
      $query = db_select('mew_event_requests', 'mer');

      // so long as the fields being pulled here have only a single delta, this specific implementation should be fine.
      // having multiple values will change the behavior.
      // for now, default to only allowing delta = 0.
      $query->innerjoin('mew_current_top', 'mct', 'mer.id = mct.event_id');
      $query->innerjoin('mew_current_dates', 'mcd', 'mer.id = mcd.event_id AND mct.event_id = mcd.event_id');

      $query->innerjoin('mew_field_top_status', 'mfts', 'mct.event_id = mfts.event_id AND mcd.event_id = mfts.event_id AND mct.status = mfts.revision AND mfts.delta = 0');
      $query->innerjoin('mew_field_top_step', 'mftsp', 'mct.event_id = mftsp.event_id AND mcd.event_id = mftsp.event_id AND mct.step = mftsp.revision AND mftsp.delta = 0');
      $query->innerjoin('mew_field_top_location', 'mftc', 'mct.event_id = mftc.event_id AND mcd.event_id = mftc.event_id AND mct.location = mftc.revision AND mftc.delta = 0');
      $query->innerjoin('mew_field_top_building', 'mftb', 'mct.event_id = mftb.event_id AND mcd.event_id = mftb.event_id AND mct.building = mftb.revision AND mftb.delta = 0');
      $query->innerjoin('mew_field_top_room', 'mftr', 'mct.event_id = mftr.event_id AND mcd.event_id = mftr.event_id AND mct.room = mftr.revision AND mftr.delta = 0');

      $query->innerjoin('mew_field_dates_date', 'mfdd', 'mct.event_id = mfdd.event_id AND mcd.event_id = mfdd.event_id AND mcd.date = mfdd.revision AND mfdd.delta = 0');
      $query->innerjoin('mew_field_dates_time_start', 'mfdts', 'mct.event_id = mfdts.event_id AND mcd.event_id = mfdts.event_id AND mcd.time_start = mfdts.revision AND mfdts.delta = 0');
      $query->innerjoin('mew_field_dates_time_stop', 'mfdtsp', 'mct.event_id = mfdtsp.event_id AND mcd.event_id = mfdtsp.event_id AND mcd.time_stop = mfdtsp.revision AND mfdtsp.delta = 0');

      $query->addField('mftc', 'value', 'top_location');
      $query->addField('mftb', 'value', 'top_building');
      $query->addField('mftr', 'value', 'top_room');

      $query->addField('mfdd', 'value', 'date');
      $query->addField('mfdts', 'value', 'time_start');
      $query->addField('mfdtsp', 'value', 'time_stop');

      $query->fields('mer');

      $show_status = array(
        MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_LOCKED,
        MCNEESE_EVENT_WORKFLOW_EVENT_STATUS_UNLOCKED,
      );

      $query->condition('mfts.value', $show_status, 'IN');

      $show_step = array(
        116,
        132,
        133,
        117
      );

      $query->condition('mftsp.value', $show_step, 'IN');

      $query->range(0, $total_items);

      if (is_null($order)) {
        $query->orderBy('created', 'DESC');
      }
      else {
        if ($order == 'id') {
          $query->orderBy('mer.id', is_null($sort) ? 'DESC' : $sort);
        }
        elseif ($order == 'location') {
          $query->orderBy('top_location', is_null($sort) ? 'ASC' : $sort);
        }
        elseif ($order == 'building') {
          $query->orderBy('top_building', is_null($sort) ? 'ASC' : $sort);
        }
        elseif ($order == 'room') {
          $query->orderBy('top_room', is_null($sort) ? 'ASC' : $sort);
        }
        elseif ($order == 'date') {
          // @todo: find a way to sort by dates (due to multiple values, one cannot simply just sort delta = 0).
          $query->orderBy('date', is_null($sort) ? 'ASC' : $sort);
          $query->orderBy('time_start', is_null($sort) ? 'ASC' : $sort);
        }
        elseif ($order == 'submitted on') {
          $query->orderBy('created', is_null($sort) ? 'ASC' : $sort);
        }
      }

      if ($administer || $manager) {
        // load all available events.
      }
      elseif ($reviewer) {
        // load all events for catagories the reviewer is associated with.
        // @fixme: until the settings are added for individual reviewers, just make all available.
      }
      elseif ($requester) {
        // load all events created by the requester who is neither an admin nor a manager.
        if (!$administer && !$manager) {
          $query->condition('mer.user_id', $user->uid);
        }
      }
      else {
        // failsafe, should never happen.
        drupal_access_denied();
        drupal_exit();
      }

      $results = $query->execute()->fetchAll();


      // @todo: make sure a sort is applied during the database call.
      $rows = array();
      foreach ($results as $result) {
        $row = array();

        $event_id = intval($result->id);

        $can_view = mcneese_event_workflow_management_page_event_access('view', $event_id);
        $can_edit = mcneese_event_workflow_management_page_event_access('edit', $event_id);
        $can_review = mcneese_event_workflow_management_page_event_access('log', $event_id);

        $row['id'] = '<a href="' . $base_path . 'events/view-0/' . $event_id . '">' . $event_id . '</a>';

        $row['location'] = check_plain($result->top_location);
        $row['building'] = check_plain($result->top_building);
        $row['room'] = check_plain($result->top_room);


        // fetch all available dates:
        $sub_query = db_select('mew_current_dates', 'mcd');
        $sub_query->innerjoin('mew_field_dates_date', 'mfdd', 'mcd.event_id = mfdd.event_id AND mcd.date = mfdd.revision');
        $sub_query->innerjoin('mew_field_dates_time_start', 'mfdts', 'mcd.event_id = mfdts.event_id AND mcd.time_start = mfdts.revision and mfdd.event_id = mfdts.event_id and mfdd.delta = mfdts.delta');
        $sub_query->innerjoin('mew_field_dates_time_stop', 'mfdtsp', 'mcd.event_id = mfdtsp.event_id AND mcd.time_start = mfdtsp.revision and mfdd.event_id = mfdts.event_id and mfdd.delta = mfdtsp.delta');

        $sub_query->addField('mfdd', 'value', 'date');
        $sub_query->addField('mfdd', 'delta', 'delta');
        $sub_query->addField('mfdts', 'value', 'time_start');
        $sub_query->addField('mfdtsp', 'value', 'time_stop');

        $sub_query->condition('mfdd.event_id', $event_id);

        $sub_query->orderBy('date', 'ASC');
        $sub_query->orderBy('time_start', 'ASC');

        $sub_executed = $sub_query->execute();
        $sub_results = $sub_executed->fetchAll();

        foreach ($sub_results as $delta => $sub_result) {
          if (!isset($row['date'])) {
            $row['date'] = '';
          }
          else {
            $row['date'] .= '<br>';
          }

          $date_stamp = $sub_result->date;
          $row['date'] .= check_plain(date("Y/m/d", $date_stamp));

          if (!is_null($sub_result->time_start) && !is_null($sub_result->time_stop)) {
            $time_stamp = $sub_result->time_start;
            $row['date'] .= ' ' . check_plain(date("h:ia", $time_stamp));

            $time_stamp = $sub_result->time_stop;
            $row['date'] .= ' to ' . check_plain(date("h:ia", $time_stamp));
          }
        }

        $date_stamp = $result->created;
        $row['created'] = check_plain(date("Y/m/d h:ia", $date_stamp));

        $row['operations'] = '';

        if ($can_view) {
          $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $event_id . '" class="operation-link operation-link-view">View</a> ';
        }

        if ($can_edit) {
          $row['operations'] .= '<a href="' . $base_path . 'events/edit-0/' . $event_id . '/information" class="operation-link operation-link-edit">Edit</a> ';
        }

        if ($can_review) {
          $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $event_id . '/3" class="operation-link operation-link-review">Review</a> ';
        }

        if ($can_log) {
          $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $event_id . '/2" class="operation-link operation-link-log">Log</a> ';
        }

        if ($can_delete) {
          // @todo: implement this.
          #$row['operations'] .= '<a href="' . $base_path . 'events/delete-0/' . $event_id . '" class="operation-link operation-link-delete">Delete</a> ';
        }

        $rows[$event_id] = $row;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return $form;
    }

    $quick_list = array();
    $quick_list['caption'] = t("Most Recent Event Requests");
    $quick_list['header'] = array(
      'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
      'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
      'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
      'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
      'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
      'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
      t("Operations"),
    );

    $quick_list['attributes'] = array();
    $quick_list['attributes']['id'] = 'mcneese_event_workflow-management_page-quick_list';
    $quick_list['attributes']['class'] = array ('mcneese_event_workflow-management_page-quick_list');
    $quick_list['attributes']['summary'] = 'A short version of the list, limited to the ' . $total_items . ' most recent items.';

    $quick_list['rows'] = $rows;
    $quick_list['empty'] = t("There are no event requests available to your account.");

    $markup .= theme('table', $quick_list);
  }

  return $markup;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
