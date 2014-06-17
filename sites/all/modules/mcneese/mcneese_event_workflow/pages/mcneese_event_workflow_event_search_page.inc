<?php

/**
 * @file
 * Defines McNeese event workflow event search page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event search page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   An array containing the generated form.
 */
function mcneese_event_workflow_event_search_0_form($form, &$form_state, $page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  global $base_path;

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

  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['page_number'] = array(
    '#type' => 'value',
    '#value' => $page_number,
  );

  $form['form']['per_page'] = array(
    '#type' => 'value',
    '#value' => 20,
  );

  $form['simple'] = array(
    '#type' => 'fieldset',
    '#title' => "Simple Search",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['simple']['text'] = array(
    '#id' => 'search-simple-text',
    '#type' => 'textfield',
  );

  $form['simple']['sensitive'] = array(
    '#id' => 'search-simple-sensitive',
    '#type' => 'checkbox',
    '#title' => "Case Sensitive",
    '#default_value' => FALSE,
  );

  $form['simple']['submit'] = array(
    '#id' => 'search-simple-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-simple-submit',
        'page-' . $page_number,
      ),
    ),
  );

  $form['simple']['submit']['search'] = array(
    '#id' => 'search-simple-submit-search',
    '#type' => 'submit',
    '#value' => t("Simple Search"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-simple-submit-search',
        'page-' . $page_number,
      ),
    ),
  );

  $form['simple']['results'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  // populate the search results.
  if (isset($form_state['simple_search_results']) && is_array($form_state['simple_search_results'])) {
    $rows = array();
    if (!empty($form_state['simple_search_results'])) {
      foreach ($form_state['simple_search_results'] as $item) {
        $row = array();

        $item->id = intval($item->id);

        $can_view_event = mcneese_event_workflow_management_page_event_access('view', $item->id);
        $can_edit_event = mcneese_event_workflow_management_page_event_access('edit', $item->id);
        $can_review_event = mcneese_event_workflow_management_page_event_access('review', $item->id);
        $can_log_event = mcneese_event_workflow_management_page_event_access('log', $item->id);

        $row['id'] = '<a href="' . $base_path . 'events/view-0/' . $item->id . '">' . $item->id . '</a>';

        $location_options = event_workflow_get_event_location_list_options();
        $building_options = event_workflow_get_event_building_list_options($item->location);
        $room_options = event_workflow_get_event_room_list_options();

        $location = "";
        if (empty($item->location)) {
          $location = "Unsure";
        }
        else {
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

        $row['location'] = $location;
        $row['building'] = $building;
        $row['room'] = $room;



        foreach ($item->date as $delta => $date) {
          if (!isset($row['date'])) {
            $row['date'] = '';
          }
          else {
            $row['date'] .= '<br>';
          }

          $row['date'] .= check_plain(date("Y/m/d", $date));

          if (array_key_exists($delta, $item->time_start) && !is_null($item->time_start[$delta]) && array_key_exists($delta, $item->time_stop) && !is_null($item->time_stop[$delta])) {
            $row['date'] .= ' ' . check_plain(date("h:ia", $item->time_start[$delta]));
            $row['date'] .= ' to ' . check_plain(date("h:ia", $item->time_stop[$delta]));
          }
        }

        $date_stamp = $item->created;
        $row['created'] = check_plain(date("Y/m/d h:ia", $date_stamp));

        $row['operations'] = '';

        if ($can_view_event) {
          $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $item->id . '" class="operation-link operation-link-view">View</a> ';
        }

        if ($can_edit_event) {
          $row['operations'] .= '<a href="' . $base_path . 'events/edit-0/' . $item->id . '/request" class="operation-link operation-link-edit">Edit</a> ';
        }

        if ($can_review_event) {
          $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $item->id . '/3" class="operation-link operation-link-review">Review</a> ';
        }

        if ($can_log_event) {
          $row['operations'] .= '<a href="' . $base_path . 'events/view-0/' . $item->id . '/2" class="operation-link operation-link-log">Log</a> ';
        }

        if ($can_delete) {
          // @todo: implement this.
          #$row['operations'] .= '<a href="' . $base_path . 'events/delete-0/' . $item->id . '" class="operation-link operation-link-delete">Delete</a> ';
        }

        $rows[$item->id] = $row;
      }
    }

    $simple_table = array();
    $simple_table['caption'] = t("Simple Search Results");
    $simple_table['header'] = array(
      'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
      'location' => array('field' => 'location', 'data' => "Location", 'align' => 'center'),
      'building' => array('field' => 'building', 'data' => "Building", 'align' => 'center'),
      'room' => array('field' => 'room', 'data' => "Room", 'align' => 'center'),
      'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
      'submitted_on' => array('field' => 'submitted_on', 'data' => "Submitted On", 'sort' => 'desc', 'align' => 'center'),
      t("Operations"),
    );

    $simple_table['attributes'] = array();
    $simple_table['attributes']['id'] = 'mcneese_event_workflow-management_page-simple_table';
    $simple_table['attributes']['class'] = array ('mcneese_event_workflow-management_page-simple_table');
    $simple_table['attributes']['summary'] = 'A table containing the search results.';

    $simple_table['rows'] = $rows;
    $simple_table['empty'] = t("No event requests were found matching your search query.");

    $form['simple']['results']['table'] = array(
      '#markup' => theme('table', $simple_table),
    );
  }

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => "Advanced Search",
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );

  $form['advanced']['submit'] = array(
    '#id' => 'search-advanced-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit',
        'page-' . $page_number,
      ),
    ),
  );

  $form['advanced']['submit']['search'] = array(
    '#id' => 'search-advanced-submit-search',
    '#type' => 'submit',
    '#value' => t("Advanced Search"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit-search',
        'page-' . $page_number,
      ),
    ),
  );

  $form['advanced']['results'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  return $form;
}

/**
 * Validation for mcneese_event_workflow_event_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_search_0_form()
 */
function mcneese_event_workflow_event_search_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'search-simple-submit-search') {
    if (empty($form_state['values']['simple']['text']) && !is_numeric($form_state['values']['simple']['text'])) {
      form_set_error('simple][submit][text', "You must supply some text to search for.");
    }
  }
  elseif ($clicked_id == 'search-advanced-submit-search') {
    $form_state['#rebuild'] = TRUE;
  }
  else {
    form_set_error('', "Internal Error. An unknown submit process has been detected. Please contact the appropriate support staff.");
  }
}

/**
 * Submit for mcneese_event_workflow_event_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_search_0_form()
 */
function mcneese_event_workflow_event_search_0_form_submit($form, &$form_state) {

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'search-simple-submit-search') {
    $search = $form_state['values']['simple']['text'];
    $sensitive = (bool) $form_state['values']['simple']['sensitive'];
    $per_page = $form['form']['per_page']['#value'];
    $start = $form['form']['page_number']['#value'] * $per_page;
    $form_state['simple_search_results'] = mcneese_event_workflow_event_load_listing($search, $sensitive, $start, $per_page);
  }

  // return to the form with the search results.
  $form_state['rebuild'] = TRUE;
  $form_state['redirect'] = FALSE;
  $form_state['submitted'] = TRUE;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
