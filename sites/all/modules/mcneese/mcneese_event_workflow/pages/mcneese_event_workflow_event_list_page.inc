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


  try {
    $query = db_select('mew_event_requests', 'mer');

    // so long as the fields being pulled here have only a single delta, this specific implementation should be fine.
    // having multiple values will change the behavior.
    // for now, default to only allowing delta = 0.
    $query->innerjoin('mew_current_top', 'mct', 'mer.id = mct.event_id');
    $query->innerjoin('mew_current_coordinator', 'mcc', 'mer.id = mcc.event_id AND mct.event_id = mcc.event_id');

    $query->innerjoin('mew_field_top_status', 'mfts', 'mct.event_id = mfts.event_id AND mcc.event_id = mfts.event_id AND mct.status = mfts.revision AND mfts.delta = 0');
    $query->innerjoin('mew_field_top_state', 'mftse', 'mct.event_id = mftse.event_id AND mcc.event_id = mftse.event_id AND mct.state = mftse.revision AND mftse.delta = 0');
    $query->innerjoin('mew_field_top_location', 'mftc', 'mct.event_id = mftc.event_id AND mcc.event_id = mftc.event_id AND mct.location = mftc.revision AND mftc.delta = 0');
    $query->innerjoin('mew_field_top_building', 'mftb', 'mct.event_id = mftb.event_id AND mcc.event_id = mftb.event_id AND mct.building = mftb.revision AND mftb.delta = 0');
    $query->innerjoin('mew_field_top_room', 'mftr', 'mct.event_id = mftr.event_id AND mcc.event_id = mftr.event_id AND mct.room = mftr.revision AND mftr.delta = 0');

    $query->innerjoin('mew_field_coordinator_classification', 'mfcc', 'mct.event_id = mfcc.event_id AND mcc.event_id = mftc.event_id AND mcc.classification = mfcc.revision AND mfcc.delta = 0');
    $query->innerjoin('mew_field_coordinator_name', 'mfcn', 'mct.event_id = mfcn.event_id AND mcc.event_id = mfcn.event_id AND mcc.name = mfcn.revision AND mfcn.delta = 0');

    $query->addField('mftc', 'value', 'top_location');
    $query->addField('mftb', 'value', 'top_building');
    $query->addField('mftr', 'value', 'top_room');

    $query->addField('mfcc', 'value', 'coordinator_classification');
    $query->addField('mfcn', 'value', 'coordinator_name');

    $query->fields('mer');

    $query->condition('mfts.value', 0, '>');

    $query->range($start_range, $rows_per_page);

    if (!is_null($order)) {
      if ($order == 'id') {
        $query->orderBy('mer.id', is_null($sort) ? 'DESC' : $sort);
      }
      elseif ($order == 'coordinator') {
        $query->orderBy('coordinator_name', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'location') {
        $query->orderBy('top_location', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'classification') {
        $query->orderBy('coordinator_classification', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'building') {
        $query->orderBy('top_building', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'room') {
        $query->orderBy('top_room', is_null($sort) ? 'ASC' : $sort);
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
      // load all events created by the requestor who is neither an admin nor a manager.
      if (!$administer && !$manager) {
        $query->condition('mer.user_id', $user->uid);
      }
    }
    else {
      // failsafe, should never happen.
      drupal_access_denied();
      drupal_exit();
    }

    $executed = $query->execute();
    $results = $executed->fetchAll();


    // @todo: make sure a sort is applied during the database call.
    $rows = array();
    foreach ($results as $result) {
      $row = array();

      $id = intval($result->id);
      $row['id'] = '<a href="' . $base_path . 'events/view-0/' . $id . '">' . $id . '</a>';

      $row['coordinator_name'] = check_plain($result->coordinator_name);
      $row['location'] = check_plain($result->top_location);
      $row['building'] = check_plain($result->top_building);
      $row['room'] = check_plain($result->top_room);

      $classification = $result->coordinator_classification;
      $classification_term = taxonomy_term_load($classification);

      if (is_object($classification_term)) {
        $row['coordinator_classification'] = check_plain($classification_term->name);
      }
      else {
        // @todo: this is an error, report it.
        $row['coordinator_classification'] = '';
      }

      $submitter = user_load($result->user_id);

      if (is_object($submitter)) {
        if ($administer || $manager) {
          $row['submitter_name'] = '<a href="' . $base_path . 'user/' . $submitter->uid . '">' . $submitter->name . '</a>';
        }
        else {
          $row['submitter_name'] = $submitter->name;
        }
      }
      else {
        // @todo: this is an error, report it.
        $row['submitter_name'] = '';
      }

      $date_stamp = $result->created;
      $row['created'] = check_plain(date("Y/m/d h:ia", $date_stamp));

      $rows[] = $row;
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
    'coordinator' => array('field' => 'coordinator', 'data' => "Coordinator", 'align' => 'center'),
    'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
    'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
    'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
    'classification' => array('field' => 'classification', 'data' => "Classification", 'align' => 'center'),
    'submitted_by' => array('field' => 'submitted_by', 'data' => "Submitted By", 'align' => 'center'),
    'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
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
