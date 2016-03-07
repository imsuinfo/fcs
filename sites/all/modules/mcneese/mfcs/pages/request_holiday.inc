<?php

/**
 * @file
 * Defines McNeese facilities use request holiday page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request holiday main page(s).
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $year
 *   (optional) The year to load the holidays for.
 *   When NULL, the current year is used.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_holiday_0_form($form, &$form_state, $year = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  // require that year be only an integer.
  if (!is_null($year) && !cf_is_integer($year)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (is_null($year)) {
    $year = (int) date("Y");
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/holiday/year-0/' . $year);

  $page_title = "Holidays for " . $year;
  drupal_set_title($page_title);

  $user = cf_current_user();

  mfcs_include(MFCS_INCLUDE_MANAGEMENT);

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

  $class_id = '';
  $class_date = '';
  $class_title = '';
  $class_enabled = '';
  $class_operations = 'no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'ASC';
    $order_column = array(0 => strtolower($sorting['id']));
    $class_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    elseif ($order == 'title') {
      $sorting['title'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_title .= 'sorting_1 ';
    }
    elseif ($order == 'enabled') {
      $sorting['enabled'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_enabled .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
    }
  }

  $items = mfcs_get_holidays($year, NULL, NULL, NULL);

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($items === FALSE) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $rows = array();
  foreach ($items as $id => $item) {
    $row = array();

    $row['id'] = array(
      'data' => (int) $item->id,
      'class' => $class_id,
    );

    $row['date'] = array(
      'data' => check_plain(date("l, F j, Y", $item->date)),
      'class' => $class_date,
    );

    $row['title'] = array(
      'data' => check_plain($item->title),
      'class' => $class_title,
    );

    $row['enabled'] = array(
      'data' => $item->enabled ? 'Yes' : 'No',
      'class' => $class_enabled,
    );

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    $row['operations']['data'] .= '<a href="' . $base_path . 'requests/holiday/year-0/' . $year . '/view/' . $item->id . $url_arguments . '" class="operation-link operation-link-view" title="View Holiday">View</a>';
    $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/holiday/year-0/' . $year . '/edit/' . $item->id . $url_arguments . '" class="operation-link operation-link-edit" title="Edit Holiday">Edit</a>';
    $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/holiday/year-0/' . $year . '/delete/' . $item->id . $url_arguments . '" class="operation-link operation-link-delete" title="Delete Holiday">Delete</a>';

    $rows[] = $row;
  }


  $holidays_list = array();
  $holidays_list['caption'] = '' . $year . ' ' . t("Holidays");
  $holidays_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
    'title' => array('field' => 'title', 'data' => "Title", 'align' => 'center'),
    'enabled' => array('field' => 'enabled', 'data' => "Enabled", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $holidays_list['attributes'] = array();
  $holidays_list['attributes']['id'] = 'mfcs-holiday_form';
  $holidays_list['attributes']['class'] = array(
    'no_default',
    'mfcs-holiday_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $holidays_list['attributes']['summary'] = 'List of holidays.';

  $holidays_list['rows'] = $rows;
  $holidays_list['empty'] = t("This system has no holidays defined for the specified year.");

  $datatable_settings = array(
    'sort' => strtolower($sort),
    'order' => $order_column,
    #'stateSave' => TRUE,
    'lengthMenu' => array(
       5 => '5',
      10 => '10',
      20 => '20',
      40 => '40',
      80 => '80',
      -1 => '"All"',
    ),
    'processing' => FALSE,
    'serverSide' => FALSE,
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'date', 'name' => "Date", 'className' => 'column-date'),
      array('id' => 2, 'column' => 'title', 'name' => "Title", 'className' => 'column-title'),
      array('id' => 3, 'column' => 'enabled', 'name' => "Enabled", 'className' => 'column-enabled'),
      array('id' => 6, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => 0,
    'pageLength' => 40,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $form = array();

  $form['holiday_list']['#markup'] = '';
  $form['holiday_list']['#markup'] .= '<div class="holiday_links-year no-print">' . "\n";
  $form['holiday_list']['#markup'] .= '<a href="' . $base_path . 'requests/holiday/year-0/' . ($year - 1) . '" class="year-previous inline-block margin-10 float-left" title="Holidays for ' . ($year - 1) . '">Previous Year</a> ' . "\n";
  $form['holiday_list']['#markup'] .= '<a href="' . $base_path . 'requests/holiday/year-0/' . ($year + 1) . '" class="year-next inline-block margin-10 float-right" title="Holidays for ' . ($year + 1) . '">Next Year</a> ' . "\n";
  $form['holiday_list']['#markup'] .= "</div>\n";

  $form['holiday_list']['#markup'] .= mcneese_libraries_theme_table($holidays_list, $datatable_settings);

  $form['holiday_list']['#markup'] .= '<br class="no-print">' . "\n";
  $form['holiday_list']['#markup'] .= '<div class="holiday_links-operations no-print">' . "\n";
  $form['holiday_list']['#markup'] .= '<a href="' . $base_path . 'requests/holiday/year-0/' . $year . '/edit" class="operation-create inline-block margin-10 float-left">Create holiday for ' . $year . '.</a> ' . "\n";
  $form['holiday_list']['#markup'] .= "</div>\n";
  $form['holiday_list']['#markup'] .= '<br class="clear">' . "\n";

  return $form;
}

/**
 * Renders a single holiday entry.
 *
 * @param int $year
 *   The holiday year to operate on.
 * @param int $id
 *   The unique id associated with a specific holiday entry.
 */
function mfcs_request_holiday_0_page($year, $id) {
  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');

    drupal_not_found();
    drupal_exit();
  }

  $user = cf_current_user();

  $is_manager = FALSE;
  if (user_access('mfcs manage', $user)) {
    $is_manager = TRUE;
  }

  $is_administer = FALSE;
  if (user_access('mfcs administer', $user)) {
    $is_administer = TRUE;
  }

  if ($is_manager || $is_administer) {
    $holiday = mfcs_get_holiday($id, NULL);
  }
  else {
    $holiday = mfcs_get_holiday($id);
  }

  if (!is_object($holiday)) {
    drupal_not_found();
    drupal_exit();
  }

  if ($year != date('Y', $holiday->date)) {
    drupal_not_found();
    drupal_exit();
  }

  $date_short = date('Y/m/d', $holiday->date);
  $date_long = date('l, F j, Y', $holiday->date);

  $title = check_plain($holiday->title);
  $description = check_plain($holiday->description);

  drupal_set_title($date_long . ": " . $title);

  $markup = '<div id="mfcs-holiday_page-view" class="mfcs-holiday_page-view">' . "\n";
  $markup .= '<div class="mfcs-holiday_page-title"><strong>' . $title . '</strong> is a holiday, break, or office closure for McNeese State University on: <strong>' . $date_long . '</strong>.</div>' . "\n";
  $markup .= "<br>\n";
  $markup .= '<div class="mfcs-holiday_page-description">' . $description . '</div>' . "\n";

  if ($is_manager || $is_administer) {
    global $base_path;
    $markup .= "<br>\n";

    $markup .= '<div class="mfcs-holiday_page-status"><strong>Status:</strong> ' . ($holiday->enabled ? 'Enabled' : 'Disabled') . "<br>\n";

    $markup .= '<br class="no-print">' . "\n";
    $markup .= '<div class="holiday_page-operations no-print">' . "\n";
    $markup .= '<a href="' . $base_path . 'requests/holiday/year-0/' . $year . '/edit/' . $holiday->id . '" class="operation-edit inline-block margin-10 float-left">Edit the holiday.</a> ' . "\n";
    $markup .= '<a href="' . $base_path . 'requests/holiday/delete-0/' . $holiday->id . '" class="operation-delete inline-block margin-10 float-right">Delete the holiday.</a>' . "\n";
    $markup .= "</div>\n";
    $markup .= '<br class="clear">' . "\n";
  }

  $markup .= '</div>' . "\n";

  return $markup;
}

/**
 * Provides the facilities use request holiday main page(s).
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $year
 *   The holiday year to operate on.
 * @param int|null $holiday_id
 *   The id of the holiday to load.
 *   If NULL, then a new holiday is being created.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_holiday_0_edit_form($form, &$form_state, $year, $holiday_id = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_null($holiday_id) && !cf_is_integer($holiday_id)) {
    cf_error::invalid_integer('holiday_id');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $is_manager = FALSE;
  if (user_access('mfcs manage', $user)) {
    $is_manager = TRUE;
  }

  $is_administer = FALSE;
  if (user_access('mfcs administer', $user)) {
    $is_administer = TRUE;
  }

  $holiday = NULL;
  if (!is_null($holiday_id)) {
    if ($is_manager || $is_administer) {
      $holiday = mfcs_get_holiday($holiday_id, NULL);
    }
    else {
      $holiday = mfcs_get_holiday($holiday_id);
    }

    if (!is_object($holiday)) {
      drupal_not_found();
      drupal_exit();
      return array();
    }
  }

  if (is_object($holiday)) {
    mfcs_add_canonical_headers('requests/holiday/year-0/' . $year . '/edit/' . $holiday_id);

    $page_title = "Edit " . $year . " Holiday: " . $holiday->title . ' (' . date('l, F j, Y', $holiday->date) . ')';
  }
  else {
    mfcs_add_canonical_headers('requests/holiday/year-0/' . $year . '/edit');

    $page_title = "Create " . $year . " Holiday";
  }

  drupal_set_title($page_title);

  $instance = mfcs_instance();
  $year_offset = $year - ((int) date('Y', $instance));

  if ($year_offset > 0) {
    $year_offset = '+' . $year_offset;
  }
  elseif ($year_offset < 0) {
    $year_offset = '-' . abs($year_offset);
  }
  else {
    $year_offset = '0';
  }

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['holiday'] = array(
    '#id' => 'field-form-holiday',
    '#type' => 'value',
    '#value' => $holiday,
  );

  $form['form']['year'] = array(
    '#id' => 'field-form-year',
    '#type' => 'value',
    '#value' => $year,
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  $form['holiday'] = array(
    '#tree' => TRUE,
  );

  $form['holiday']['title'] = array(
    '#id' => 'field-holiday-title',
    '#type' => 'textfield',
    '#title' => "Title",
    '#required' => TRUE,
  );

  $form['holiday']['description'] = array(
    '#id' => 'field-holiday-description',
    '#type' => 'textarea',
    '#title' => "description",
  );

  $form['holiday']['date'] = array(
    '#id' => 'field-holiday-date',
    '#type' => 'date_popup',
    '#title' => "Date",
    '#date_format' => "M d",
    '#date_label_position' => 'invisible',
    '#date_year_range' => '' . $year_offset . ':' . $year_offset,
    '#datepicker_options' => array(
      'changeYear' => FALSE,
    ),
    '#expected_format' => array('date' => "M d"),
    '#required' => TRUE,
  );

  if ($is_manager || $is_administer) {
    $form['holiday']['enabled'] = array(
      '#id' => 'field-holiday-enabled',
      '#type' => 'select',
      '#title' => "Enabled",
      '#options' => array(0 => 'No', 1 => 'Yes'),
    );
  }

  if (is_object($holiday)) {
    $form['holiday']['title']['#default_value'] = $holiday->title;
    $form['holiday']['description']['#default_value'] = $holiday->description;
    $form['holiday']['date']['#default_value'] = date('Y-m-d', $holiday->date);

    if ($is_manager || $is_administer) {
      $form['holiday']['enabled']['#default_value'] = $holiday->enabled;
    }
  }

  $form['holiday']['submit'] = array(
    '#id' => 'mfcs-holiday-delete-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['holiday']['submit']['cancel'] = array(
    '#id' => 'mfcs-holiday-edit-submit-cancel',
    '#name' => 'mfcs-holiday-edit-submit-cancel',
    '#type' => 'submit',
    '#default_value' => t("Cancel"),
  );

  $form['holiday']['submit']['submit'] = array(
    '#id' => 'mfcs-holiday-edit-submit-submit',
    '#name' => 'mfcs-holiday-edit-submit-submit',
    '#type' => 'submit',
    '#default_value' => t("Submit"),
  );

  return $form;
}

/**
 * Validate process for mfcs_request_holiday_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_edit_form()
 */
function mfcs_request_holiday_0_edit_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-holiday-edit-submit-submit') {
    $date = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['holiday']['date']);
    if ($date === FALSE || date('m-d', $date) != $form_state['values']['holiday']['date']) {
      form_set_error('holiday][date', "The specified date is not valid.");
    }

    $length = strlen($form_state['values']['holiday']['title']);
    if ($length > 255) {
      form_set_error('holiday][title', "The title is " . $length . " characters long and cannot be longer than 255 characters.");
    }

    $length = strlen($form_state['values']['holiday']['description']);
    if ($length > 255) {
      form_set_error('holiday][description', "The description is " . $length . " characters long and cannot be longer than 255 characters.");
    }
  }
}

/**
 * Submit process for mfcs_request_holiday_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_edit_form()
 */
function mfcs_request_holiday_0_edit_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $holiday = NULL;
  if (isset($form['form']['holiday']['#value']->id)) {
    $holiday = $form['form']['holiday']['#value'];
  }

  $instance = mfcs_instance();
  $year_this = date('Y', $instance);

  if ($clicked_id == 'mfcs-holiday-edit-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      if (is_object($holiday)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0/' . $holiday->year . '/view/' . $holiday->id);
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0/' . $holiday->year);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $is_manager = FALSE;
  if (user_access('mfcs manage', $user)) {
    $is_manager = TRUE;
  }

  $is_administer = FALSE;
  if (user_access('mfcs administer', $user)) {
    $is_administer = TRUE;
  }

  #mfcs_include(MFCS_INCLUDE_WORKFLOW);

  $transaction = db_transaction();

  $success = TRUE;
  try {
    $date = strtotime($form['form']['year']['#value'] . '-' . $form_state['values']['holiday']['date']);
    $date = strtotime('midnight today', $date);

    $fields = array();
    $fields['title'] = $form_state['values']['holiday']['title'];
    $fields['description'] = $form_state['values']['holiday']['description'];
    $fields['date'] = $date;
    $fields['year'] = (int) $form['form']['year']['#value'];
    $fields['updated'] = mfcs_instance();

    if ($is_manager || $is_administer) {
      if (isset($form_state['values']['holiday']['enabled'])) {
        $fields['enabled'] = ($form_state['values']['holiday']['enabled'] ? 1 : 0);
      }
    }

    $id = NULL;
    if (is_object($holiday)) {
      // check to see if there are any changes made.
      $changes = array();
      foreach ($fields as $key => $value) {
        // do not check updated key because it must be different.
        if ($key == 'updated') {
          continue;
        }

        if ($value != $holiday->$key) {
          $changes[$key] = array(
            'from' => $holiday->$key,
            'to' => $value,
          );
        }
      }

      if (!empty($changes)) {
        $query = db_update('mfcs_holiday');
        $query->condition('id', $holiday->id);
        $query->fields($fields);
        $query->execute();

        $fields = array();
        $fields['holiday_id'] = $holiday->id;
        $fields['date'] = mfcs_instance();
        $fields['user_id'] = $user->uid;
        $fields['change'] = '';

        foreach ($changes as $key => $value) {
          $fields['change'] .= $key . ' has been changed from \'' . $value['from'] . '\' to \'' . $value['to'] . '\'. ';
        }

        $query = db_insert('mfcs_log_holiday');
        $query->fields($fields);
        $query->execute();
      }
    }
    else {
      $fields['created'] = $fields['updated'];

      $query = db_insert('mfcs_holiday');
      $query->fields($fields);
      $id = $query->execute();

      $fields = array();
      $fields['holiday_id'] = $id;
      $fields['date'] = mfcs_instance();
      $fields['user_id'] = $user->uid;
      $fields['change'] = 'created.';

      $query = db_insert('mfcs_log_holiday');
      $query->fields($fields);
      $query->execute();
    }

    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_exception($ex);

    $success = FALSE;
  }

  if (!$success) {
    form_set_error('form', "An error occurred while trying to edit the holiday. Please contact the support staff.");

    if (is_object($holiday)) {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to edit the holiday: @holiday_id.", array('@holiday_id' => $holiday->id), WATCHDOG_ERROR);
    }
    else {
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to create a holiday.", array(), WATCHDOG_ERROR);
    }

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (is_object($holiday)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0/' . $holiday->year . '/view/' . $holiday->id);
    }
    elseif (is_null($id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0/' . $form['form']['year']['#value']);
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0/' . $form['form']['year']['#value'] . '/view/' . $id);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request holiday main page(s).
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $year
 *   The holiday year to operate on.
 * @param int $holiday_id
 *   The id of the holiday to load.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_holiday_0_delete_form($form, &$form_state, $year, $holiday_id) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($holiday_id)) {
    cf_error::invalid_integer('holiday_id');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $is_manager = FALSE;
  if (user_access('mfcs manage', $user)) {
    $is_manager = TRUE;
  }

  $is_administer = FALSE;
  if (user_access('mfcs administer', $user)) {
    $is_administer = TRUE;
  }

  if ($is_manager || $is_administer) {
    $holiday = mfcs_get_holiday($holiday_id, NULL);
  }
  else {
    $holiday = mfcs_get_holiday($holiday_id);
  }

  if (!is_object($holiday)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if ($year != date('Y', $holiday->date)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  mfcs_add_canonical_headers('requests/holiday/delete-0/' . $holiday_id);

  $page_title = "Delete Holiday: " . check_plain($holiday->title) . ' (' . date('l, F j, Y', $holiday->date) . ')';

  drupal_set_title($page_title);

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['holiday'] = array(
    '#id' => 'field-form-holiday',
    '#type' => 'value',
    '#value' => $holiday,
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  $form['confirmation'] = array(
    '#markup' => '',
  );

  $form['confirmation']['#markup'] .= '<h3>Are you sure wou want to delete the holiday?</h3> ';
  $form['confirmation']['#markup'] .= '<div>';
  $form['confirmation']['#markup'] .= '<strong>ID</strong>: ' . $holiday->id . "<br>\n";
  $form['confirmation']['#markup'] .= '<strong>Title</strong>: ' . $holiday->title . "<br>\n";
  $form['confirmation']['#markup'] .= '<strong>Date</strong>: ' . date('l, F j, Y', $holiday->date) . "<br>\n";
  $form['confirmation']['#markup'] .= '<strong>Status</strong>: ' . ($holiday->enabled == 1 ? 'Enabled' : 'Disabled') . "<br>\n";
  $form['confirmation']['#markup'] .= '</div>';
  $form['confirmation']['#markup'] .= "<br>\n";

  $form['display']['submit'] = array(
    '#id' => 'mfcs-holiday-delete-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['display']['submit']['no'] = array(
    '#id' => 'mfcs-holiday-delete-submit-no',
    '#name' => 'mfcs-holiday-delete-submit-no',
    '#type' => 'submit',
    '#default_value' => t("No"),
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'mfcs-holiday-delete-submit-yes',
    '#name' => 'mfcs-holiday-delete-submit-yes',
    '#type' => 'submit',
    '#default_value' => t("Yes"),
  );

  return $form;
}

/**
 * Submit process for mfcs_request_holiday_0_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_delete_form()
 */
function mfcs_request_holiday_0_delete_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $holiday_id = (int) $form['form']['holiday']['#value']->id;
  $holiday_year = (int) $form['form']['holiday']['#value']->year;

  if ($clicked_id == 'mfcs-holiday-delete-submit-no') {
    if (empty($form_state['values']['redirect_to'])) {
      if (empty($holiday_id)) {
        $form_state['redirect'] = mfcs_build_redirect_array('requests');
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0/' . $holiday_year . '/view/' . $holiday_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $fields = array();
  $fields['disabled'] = 1;
  $fields['updated'] = mfcs_instance();

  $transaction = db_transaction();

  $deleted = TRUE;
  try {
    $query = db_update('mfcs_holiday');
    $query->condition('id', $holiday_id);
    $query->fields($fields);
    $query->execute();

    $fields = array();
    $fields['holiday_id'] = $holiday_id;
    $fields['date'] = mfcs_instance();
    $fields['user_id'] = $user->uid;
    $fields['change'] = 'deleted.';

    $query = db_insert('mfcs_log_holiday');
    $query->fields($fields);
    $query->execute();

    unset($transaction);
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_exception($ex);

    $deleted = FALSE;
  }

  if (!$deleted) {
    form_set_error('form', "An error occurred while trying to delete the holiday. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to delete the holiday: @holiday_id.", array('@holiday_id' => $holiday_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/holiday/year-0');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
