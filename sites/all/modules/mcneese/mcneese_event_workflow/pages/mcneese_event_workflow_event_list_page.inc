<?php

/**
 * @file
 * Defines McNeese event workflow event list page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event list page.
 *
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   An array containing the generated form.
 */
function mcneese_event_workflow_event_list_0_page($page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  global $base_path;

  $rows_per_page = 50;
  $start_range = $page_number * $rows_per_page;

  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];

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

  mcneese_event_workflow_include(4);


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

    $query->addField('mftc', 'value', 'location');
    $query->addField('mftb', 'value', 'building');
    $query->addField('mftr', 'value', 'room');

    $query->addField('mfts', 'value', 'status');
    $query->addField('mftsp', 'value', 'step');

    $query->addField('mfdd', 'value', 'date');
    $query->addField('mfdts', 'value', 'time_start');
    $query->addField('mfdtsp', 'value', 'time_stop');

    $query->fields('mer');

    $query->condition('mfts.value', 0, '>');

    $query->range($start_range, $rows_per_page);

    if (!is_null($order)) {
      if ($order == 'id') {
        $query->orderBy('mer.id', is_null($sort) ? 'DESC' : $sort);
      }
      elseif ($order == 'location') {
        $query->orderBy('location', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'building') {
        $query->orderBy('building', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'room') {
        $query->orderBy('room', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'date') {
        // @todo: find a way to sort by dates (due to multiple values, one cannot simply just sort delta = 0).
        $query->orderBy('date', is_null($sort) ? 'ASC' : $sort);
        $query->orderBy('time_start', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'status') {
        $query->orderBy('status', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'step') {
        $query->orderBy('step', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'submitted by') {
        $query->orderBy('user_id', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'submitted on') {
        $query->orderBy('created', is_null($sort) ? 'DESC' : $sort);
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

      $location_options = event_workflow_get_event_location_list_options();
      $building_options = event_workflow_get_event_building_list_options($result->location);

      $location = "";
      if (empty($result->location)) {
        $location = "Unsure";
      }
      else {
        if (isset($location_options[$result->location])) {
          $location = check_plain($location_options[$result->location]);
        }
      }

      $building = "";
      if (!empty($result->building)) {
        if (isset($building_options[$result->building])) {
          $building = check_plain($building_options[$result->building]);
        }
      }

      $row['location'] = $location;
      $row['building'] = $building;
      $row['room'] = check_plain($result->room);

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

      $status_options = event_workflow_get_event_status_list_options();

      $row['status'] = $status_options[$result->status];

      $submitter = user_load($result->user_id);

      if (is_object($submitter)) {
        $name = $submitter->name;

        if (!empty($submitter->field_user_first_name['und'][0]['value'])) {
          $name = $submitter->field_user_first_name['und'][0]['value'];

          if (!empty($submitter->field_user_last_name['und'][0]['value'])) {
            $name .= ' ' . $submitter->field_user_last_name['und'][0]['value'];
          }

          $name .= ' (' . $submitter->name . ')';
        }

        if ($administer || $manager) {
          $row['submitter_name'] = '<a href="' . $base_path . 'user/' . $submitter->uid . '">' . $name . '</a>';
        }
        else {
          $row['submitter_name'] = $name;
        }
      }
      else {
        // @todo: this is an error, report it.
        $row['submitter_name'] = '';
      }

      $date_stamp = $result->created;
      $row['created'] = check_plain(date("Y/m/d h:ia", $date_stamp));

      $row['operations'] = '';

      if ($can_view) {
        $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $event_id . '" class="operation-link operation-link-view">View</a> ';
      }

      if ($can_edit) {
        $row['operations'] .= '<a href="' . $base_path . 'events/edit-0/' . $event_id . '/request" class="operation-link operation-link-edit">Edit</a> ';
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

  $table = array();
  $table['caption'] = "Event Requests";
  $table['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
    'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
    'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
    'status' => array('field' => 'status', 'data' => "Status", 'align' => 'center'),
    'submitted_by' => array('field' => 'submitted_by', 'data' => "Submitted By", 'align' => 'center'),
    'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
    t("Operations"),
  );

  $table['attributes'] = array();
  $table['attributes']['summary'] = "A listing of event requests associated by ID.";

  $table['rows'] = $rows;
  $table['empty'] = "There are no event requests available for to your account.";

  $output .= theme('table', $table);

  return $form;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
