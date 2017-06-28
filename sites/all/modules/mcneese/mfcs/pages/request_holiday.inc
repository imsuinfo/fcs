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
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  // require that year be only an integer.
  if (!is_null($year) && !cf_is_integer($year)) {
    drupal_not_found();
    drupal_exit();
  }

  if (is_null($year)) {
    $year = (int) date('Y');
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('holiday-0/' . $year);

  drupal_set_title('Holidays for ' . $year);

  $user = cf_current_user();

  $can_edit = mfcs_holiday_request_access('edit', $year, NULL, $user->uid);

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

  $class_id = 'column-id ';
  $class_date = 'column-date ';
  $class_title = 'column-title ';
  $class_type = 'column-type ';
  $class_enabled = 'column-enabled ';
  $class_operations = 'column-operations no-print ';

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
    elseif ($order == 'type') {
      $sorting['type'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_type .= 'sorting_1 ';
    }
    elseif ($order == 'enabled') {
      if ($can_edit) {
        $sorting['enabled'] = is_null($sort) ? 'DESC' : $sort;
        $order_column = array(4 => strtolower($sorting[$order]));
        $class_enabled .= 'sorting_1 ';
      }
    }
    else {
      $sorting['id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
    }
  }

  if ($can_edit) {
    $items = mfcs_get_holidays($year, NULL, NULL, NULL, FALSE, TRUE);
  }
  else {
    $items = mfcs_get_holidays($year, NULL, NULL, TRUE, FALSE, TRUE);
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($items === FALSE) {
    drupal_not_found();
    drupal_exit();
  }

  $rows = array();
  foreach ($items as $id => $item) {
    $row = array();

    $row['id'] = array(
      'data' => '<div class="column-item item-id item-id-0">' . ((int) $item->id) . '</div>',
      'class' => $class_id,
    );

    if ($item->date_stop == strtotime('midnight tomorrow', $item->date_start)) {
      $formatted_date = date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $item->date_start);
    }
    else {
      // the stop day is displayed as '-1 day' because it is an exclusive stop date.
      $formatted_date = date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $item->date_start);
      $formatted_date .= ' to ';
      $formatted_date .= date(MFCS_DATE_FORMAT_DATE_SQUEEZE, strtotime('-1 day', $item->date_stop));
    }

    $row['date'] = array(
      'data' => '<div class="column-item item-date_start item-date_start-0">' . check_plain($formatted_date) . '</div>',
      'class' => $class_date,
    );

    $row['title'] = array(
      'data' => '<div class="column-item item-title item-title-0">' . check_plain($item->title) . '</div>',
      'class' => $class_title,
    );

    $row['type'] = array(
      'data' => '<div class="column-item item-type item-type-0">' . check_plain($item->type_human_name) . '</div>',
      'class' => $class_title,
    );

    if ($can_edit) {
      $row['enabled'] = array(
        'data' => '<div class="column-item item-enabled item-enabled-0">' . ($item->enabled ? 'Yes' : 'No') . '</div>',
        'class' => $class_enabled,
      );
    }

    $row['operations'] = array(
      'data' => '',
      'class' => $class_operations,
    );

    $item_year = (int) $item->year;

    $row['operations']['data'] .= '<div class="column-item item-operations item-operations-view"><a href="' . $base_path . 'holiday-0/view/' . $item_year . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-view" title="View Holiday">View</a></div>';

    if ($can_edit) {
      $row['operations']['data'] .= '<div class="column-item item-operations item-operations-edit"><a href="' . $base_path . 'holiday-0/edit/' . $item_year . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-edit" title="Edit Holiday">Edit</a></div>';

      if ($year == $item_year) {
        $row['operations']['data'] .= '<div class="column-item item-operations item-operations-create"><a href="' . $base_path . 'holiday-0/create/' . $item_year . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-copy" title="Copy Holiday">Copy</a></div>';
        $row['operations']['data'] .= '<div class="column-item item-operations item-operations-delete"><a href="' . $base_path . 'holiday-0/delete/' . $item_year . '/' . $item->id . $url_arguments . '" class="operation-link operation-link-delete" title="Delete Holiday">Delete</a></div>';
      }
    }

    $rows[] = $row;
  }


  $holidays_list = array();
  $holidays_list['caption'] = '' . $year . ' ' . 'Holidays';
  $holidays_list['header'] = array(
    'id' => array('field' => 'id', 'data' => 'ID', 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => 'Date', 'align' => 'center'),
    'title' => array('field' => 'title', 'data' => 'Title', 'align' => 'center'),
    'type' => array('field' => 'type', 'data' => 'Type', 'align' => 'center'),
  );

  if ($can_edit) {
    $holidays_list['header']['enabled'] = array('field' => 'enabled', 'data' => 'Enabled', 'align' => 'center');
  }

  $holidays_list['header']['operations'] = array('data' => 'Operations', 'align' => 'center', 'class' => array('no-print'));

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
  $holidays_list['empty'] = 'This system has no holidays defined for the specified year.';

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
      array('id' => 0, 'column' => 'id', 'name' => 'ID', 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'date', 'name' => 'Date', 'className' => 'column-date'),
      array('id' => 2, 'column' => 'title', 'name' => 'Title', 'className' => 'column-title'),
      array('id' => 3, 'column' => 'type', 'name' => 'Type', 'className' => 'column-type'),
    ),
    'displayStart' => 0,
    'pageLength' => 40,
    'dom' => 'lfBrtip',
    'KeyTable' => FALSE,
    'buttons' => array(
      'colvis' => array(
        'extend' => '"colvis"',
        'text' => '"Show / Hide"',
      ),
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $holidays_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $holidays_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
    'responsive' => TRUE,
  );

  if ($can_edit) {
    $datatable_settings['columns'][] = array('id' => 4, 'column' => 'enabled', 'name' => 'Enabled', 'className' => 'column-enabled');
    $datatable_settings['columns'][] = array('id' => 5, 'column' => NULL, 'name' => 'Operations', 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE);
  }
  else {
    $datatable_settings['columns'][] = array('id' => 4, 'column' => NULL, 'name' => 'Operations', 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE);
  }

  $form = array();
  $form['holiday_list']['#markup'] = '';

  if ($can_edit) {
    $form['holiday_list']['#markup'] .= '<div class="holiday_links-create clear no-print">';
    $form['holiday_list']['#markup'] .= 'You may <a href="' . $base_path . 'holiday-0/create/' . $year . $url_arguments . '">create a new ' . $year . ' holiday</a>.<br>';
    $form['holiday_list']['#markup'] .= 'You may <a href="' . $base_path . 'holiday-0/copy/' . $year . $url_arguments . '">copy all holidays from another year</a>.<br>';
    $form['holiday_list']['#markup'] .= '</div>';
  }

  $form['holiday_list']['#markup'] .= '<div class="holiday_links-year no-print">';
  $form['holiday_list']['#markup'] .= '<a href="' . $base_path . 'holiday-0/' . ($year - 1) . $url_arguments . '" class="year-previous inline-block margin-10 float-left" title="Holidays for ' . ($year - 1) . '">Previous Year</a> ';
  $form['holiday_list']['#markup'] .= '<a href="' . $base_path . 'holiday-0/' . ($year + 1) . $url_arguments . '" class="year-next inline-block margin-10 float-right" title="Holidays for ' . ($year + 1) . '">Next Year</a> ';
  $form['holiday_list']['#markup'] .= '</div>';

  $form['holiday_list']['#markup'] .= '<br class="clear">';

  $form['holiday_list']['#markup'] .= mcneese_libraries_theme_table($holidays_list, $datatable_settings);

  $form['holiday_list']['#markup'] .= '<br class="clear">';

  return $form;
}

/**
 * Renders a single holiday entry.
 *
 * @param int $year
 *   The holholiday_iday year to operate on.
 * @param int $holiday_id
 *   The unique id associated with a specific holiday entry.
 */
function mfcs_request_holiday_0_page($year, $holiday_id) {
  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($holiday_id)) {
    cf_error::invalid_integer('holiday_id');

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

  $is_power = FALSE;
  if ($is_manager || $is_administer) {
    $is_power = TRUE;
  }

  if ($is_power) {
    $holiday = mfcs_get_holiday($holiday_id, NULL, FALSE, TRUE);
  }
  else {
    $holiday = mfcs_get_holiday($holiday_id, TRUE, FALSE, TRUE);
  }

  if (!is_object($holiday)) {
    drupal_not_found();
    drupal_exit();
  }

  if ($year != $holiday->year) {
    drupal_not_found();
    drupal_exit();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($holiday->date_stop == strtotime('midnight tomorrow', $holiday->date_start)) {
    $formatted_date = '<strong>' . date(MFCS_DATE_FORMAT_DATE, $holiday->date_start) . '</strong>';
  }
  else {
    $formatted_date = '<strong>' . date(MFCS_DATE_FORMAT_DATE, $holiday->date_start) . '</strong>';
    $formatted_date .= ' to ';
    $formatted_date .= '<strong>' . date(MFCS_DATE_FORMAT_DATE, strtotime('midnight yesterday', $holiday->date_stop)) . '</strong>';
  }

  // search for and report any holiday conflicts
  $warnings = NULL;
  $conflicts = mfcs_request_holiday_0_check_for_conflicts($holiday->id);
  if (is_array($conflicts) && !empty($conflicts)) {
    $warnings = '<div class="mfcs-holiday_page-warnings">The following requests are scheduled during this holiday:<br><ul class="holiday_conflict-warning-list">';
    foreach ($conflicts as $conflict) {
      $warnings .= '<li class="holiday_conflict-warning-list-item"><a href="' . $base_path . 'requests/view-0/' . $conflict->id . $url_arguments . '">' . check_plain($conflict->title) . '</a></li>';
    }
    $warnings .= '</ul></div>';
  }
  unset($conflicts);

  $title = check_plain($holiday->title);
  $type = check_plain($holiday->type_human_name);
  $description = check_plain($holiday->description);

  mfcs_add_canonical_headers('holiday-0/view/' . $year . '/' . $holiday_id);
  drupal_set_title('' . $year . ' Holiday: ' . $holiday->title);

  $markup = '<div id="mfcs-holiday_page-view" class="mfcs-holiday_page-view">';
  $markup .= '<div class="mfcs-holiday_page-title"><strong>' . $title . '</strong> is a holiday, break, or office closure for McNeese State University on: ' . $formatted_date . '.</div>';
  $markup .= '<br>';
  $markup .= '<div class="mfcs-holiday_page-description">' . $description . '</div>';
  $markup .= '<br>';
  $markup .= '<div class="mfcs-holiday_page-type"><strong>Type</strong>: ' . $type . '</div>';

  if ($is_manager || $is_administer) {
    $markup .= '<div class="mfcs-holiday_page-status"><strong>Status:</strong> ' . ($holiday->enabled ? 'Enabled' : 'Disabled') . '<br>';

    $instance = mfcs_instance();
    $current_year = date('Y', $instance);
    if ($is_power || $year >= $current_year) {
      $markup .= '<br class="no-print">';
      $markup .= '<div class="holiday_page-operations no-print">';
      $markup .= '<div class="operation-wrapper operation-wrapper-edit"><a href="' . $base_path . 'holiday-0/edit/' . $year . '/' . $holiday_id . $url_arguments . '" class="operation-edit">Edit this holiday</a></div> ';
      $markup .= '<div class="operation-wrapper operation-wrapper-create"><a href="' . $base_path . 'holiday-0/create/' . $year . '/' . $holiday_id . $url_arguments . '" class="operation-copy">Copy this holiday</a></div> ';
      $markup .= '<div class="operation-wrapper operation-wrapper-delete"><a href="' . $base_path . 'holiday-0/delete/' . $year . '/' . $holiday_id . $url_arguments . '" class="operation-delete">Delete this holiday</a></div>';
      $markup .= '</div>';
    }

    $markup .= '<br class="clear">';
  }
  $markup .= '</div>';

  if (is_string($warnings)) {
    $markup .= $warnings;
  }

  return $markup;
}

/**
 * Provides the facilities use request holiday copy form.
 *
 * This copies all holidays from one year to another (and not individual holidays).
 * To copy individual holidays use mfcs_request_holiday_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $year
 *   The holiday year to operate on.
 *   The represents the destination year and not the source year.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see: mfcs_request_holiday_0_edit_form()
 */
function mfcs_request_holiday_0_copy_form($form, &$form_state, $year) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  mfcs_add_canonical_headers('holiday-0/copy/' . $year);
  drupal_set_title('Copy to ' . $year . ' Holidays');

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['year'] = array(
    '#id' => 'field-form-year',
    '#type' => 'value',
    '#value' => (int) $year,
  );

  $form['form']['user'] = array(
    '#id' => 'field-form-user',
    '#type' => 'value',
    '#value' => cf_current_user(),
  );

  $form['holiday'] = array(
    '#tree' => TRUE,
  );

  $form['holiday']['year'] = array(
    '#id' => 'field-holiday-year',
    '#type' => 'numberfield',
    '#title' => 'Year',
    '#required' => TRUE,
    '#description' => 'Select a year to copy all holidays of.',
  );

  $form['holiday']['include_disabled'] = array(
    '#id' => 'field-holiday-include_disabled',
    '#type' => 'checkbox',
    '#title' => 'Include Disabled',
    '#required' => FALSE,
    '#description' => 'When copying holidays, include disabled holidays.',
  );

  $form['holiday']['as_disabled'] = array(
    '#id' => 'field-holiday-as_disabled',
    '#type' => 'checkbox',
    '#title' => 'As Disabled',
    '#required' => FALSE,
    '#default_value' => TRUE,
    '#description' => 'When copying holidays, set the copied holidays to disabled.',
  );

  $form['holiday']['submit'] = array(
    '#id' => 'mfcs-holiday-copy-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('no-print'),
    ),
  );

  $form['holiday']['submit']['cancel'] = array(
    '#id' => 'mfcs-holiday-copy-submit-cancel',
    '#name' => 'mfcs-holiday-copy-submit-cancel',
    '#type' => 'submit',
    '#default_value' => 'Cancel',
  );

  $form['holiday']['submit']['submit'] = array(
    '#id' => 'mfcs-holiday-copy-submit-submit',
    '#name' => 'mfcs-holiday-copy-submit-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
  );

  return $form;
}

/**
 * Pre-Validate process for mfcs_request_holiday_0_copy_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_copy_form()
 */
function mfcs_request_holiday_0_copy_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-holiday-copy-submit-cancel') {
    $form['holiday']['year']['#required'] = FALSE;
  }
}

/**
 * Validate process for mfcs_request_holiday_0_copy_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_copy_form()
 */
function mfcs_request_holiday_0_copy_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-holiday-copy-submit-cancel') {
    return;
  }
  elseif ($clicked_id != 'mfcs-holiday-copy-submit-submit') {
    form_set_error('', 'Unknown submit process detected.');
    return;
  }

  if (isset($form_state['values']['holiday']['year']) && is_numeric($form_state['values']['holiday']['year'])) {
    $year = (int) $form_state['values']['holiday']['year'];
  }
  else {
    form_set_error('holiday][year', 'Please specify a valid year.');
    return;
  }

  $enabled = TRUE;
  if (isset($form_state['values']['holiday']['include_disabled'])) {
    if ($form_state['values']['holiday']['include_disabled'] == 1) {
      $enabled = NULL;
    }
  }

  // check to see if there are any years to copy.
  $holidays = mfcs_get_holidays($year, NULL, NULL, $enabled);
  if ($holidays === FALSE) {
    form_set_error('', 'Internal Error. An unknown error has been detected while trying to process this request. Please contact the appropriate support staff.');

    $user = cf_current_user();
    watchdog(MFCS_WATCHDOG_ID, 'Internal Error. Error while calling mfcs_get_holidays() for the user %user_name [%user_id].', array('%user_name' => $user->name, '%user_id' => $user->uid), WATCHDOG_ERROR);
  }
  elseif (empty($holidays)) {
    form_set_error('holiday][year', 'No holidays were found for the year ' . $year . '. There is nothing to copy.');
  }
}

/**
 * Submit process for mfcs_request_holiday_0_copy_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_copy_form()
 */
function mfcs_request_holiday_0_copy_form_submit($form, &$form_state) {
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

  $instance = mfcs_instance();
  $year_this = date('Y', $instance);

  if ($clicked_id == 'mfcs-holiday-copy-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/' . $form['form']['year']['#value']);
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $year_current = (int) $form['form']['year']['#value'];
  $year_previous = (int) $form_state['values']['holiday']['year'];

  $enabled = TRUE;
  if ($form_state['values']['holiday']['include_disabled'] == 1) {
    $enabled = NULL;
  }

  $as_disabled = TRUE;
  if ($form_state['values']['holiday']['as_disabled'] == 0) {
    $as_disabled = FALSE;
  }

  $holidays = mfcs_get_holidays($year_previous, NULL, NULL, $enabled);

  $holidays_to_check = array();

  $transaction = db_transaction();
  $success = TRUE;
  try {
    if (!empty($holidays)) {
      foreach ($holidays as $holiday) {
        // copy the holiday.
        $current_date = '' . $year_current . date('/m/d h:i:s a', $holiday->date_start);

        $fields = array();
        $fields['year'] = $year_current;
        $fields['date_start'] = strtotime($current_date);
        $fields['date_stop'] = $fields['date_start'] + ($holiday->date_stop - $holiday->date_start);
        $fields['title'] = $holiday->title;
        $fields['type'] = $holiday->type;
        $fields['description'] = $holiday->description;
        $fields['created'] = $instance;
        $fields['updated'] = $instance;

        if ($as_disabled) {
          $fields['enabled'] = 0;
        }
        else {
          $fields['enabled'] = $holiday->enabled;
        }

        $query = db_insert('mfcs_holiday');
        $query->fields($fields);
        $id = $query->execute();

        if (is_numeric($id)) {
          $id = (int) $id;
          $holidays_to_check[$id] = $holiday->title;
        }

        // write to the log.
        $fields = array();
        $fields['holiday_id'] = $id;
        $fields['user_id'] = $user->uid;
        $fields['date'] = $instance;
        $fields['change'] = 'copied from holiday ' . $holiday->id . '.';

        $query = db_insert('mfcs_log_holiday');
        $query->fields($fields);
        $query->execute();

        unset($id);
        unset($current_date);
      }
    }
    else {
      $success = FALSE;
    }

    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }

  if (!$success) {
    form_set_error('form', 'An error occurred while trying to copy the holidays. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to copy a holiday.', array(), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/' . $form['form']['year']['#value']);
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request holiday create/edit form.
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
 * @param bool $copy
 *   If TRUE, then the specified unavailability id is "cloned/copied" instead of edited.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_holiday_0_edit_form($form, &$form_state, $year, $holiday_id = NULL, $copy = FALSE) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_bool($copy)) {
    cf_error::invalid_bool('copy');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($holiday_id) && !cf_is_integer($holiday_id)) {
    cf_error::invalid_integer('holiday_id');

    drupal_not_found();
    drupal_exit();
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
    }

    if ($year != $holiday->year) {
      drupal_not_found();
      drupal_exit();
    }
  }

  if (is_null($holiday)) {
    mfcs_add_canonical_headers('holiday-0/create/' . $year);

    $page_title = 'Create ' . $year . ' Holiday';
  }
  elseif ($copy) {
    mfcs_add_canonical_headers('holiday-0/create/' . $year . '/' . $holiday_id);

    $page_title = 'Copy ' . $year . ' Holiday: ' . $holiday->title;
  }
  else {
    mfcs_add_canonical_headers('holiday-0/edit/' . $year . '/' . $holiday_id);

    $page_title = 'Edit ' . $year . ' Holiday: ' . $holiday->title;
  }

  drupal_set_title($page_title);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $holiday_types = mfcs_get_holiday_type_list_options('select');
  $holiday_types_all = mfcs_get_holiday_type_list_options('select', TRUE);

  // a year offset is necessary to create the date field for a specific year.
  // the date field needs to be limited to a certain year and it works based off the offset to the current year.
  $instance = mfcs_instance();
  $year_offset = $year - ((int) date('Y', $instance));
  $year_offset_start = 0 + $year_offset;
  $year_offset_stop = 1 + $year_offset;

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

  $form['form']['copy'] = array(
    '#id' => 'field-form-copy',
    '#type' => 'value',
    '#value' => $copy,
  );

  $form['holiday'] = array(
    '#tree' => TRUE,
  );

  $form['holiday']['title'] = array(
    '#id' => 'field-holiday-title',
    '#type' => 'textfield',
    '#title' => 'Title',
    '#required' => TRUE,
  );

  $form['holiday']['type'] = array(
    '#id' => 'field-holiday-type',
    '#type' => 'select',
    '#title' => 'Type',
    '#options' => $holiday_types,
    '#required' => TRUE,
    '#default_value' => MFCS_HOLIDAY_TYPE_HOLIDAY,
  );

  $form['holiday']['description'] = array(
    '#id' => 'field-holiday-description',
    '#type' => 'textarea',
    '#title' => 'Description',
  );

  $form['holiday']['date_start'] = array(
    '#id' => 'section-holiday-date_start',
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => 'Start Date',
    '#description' => 'The start day is inclusive.',
  );

  $form['holiday']['date_start']['day'] = array(
    '#id' => 'field-holiday-date_start-day',
    '#type' => 'date_popup',
    '#title' => 'Start Date',
    '#date_format' => 'M d, Y',
    '#date_label_position' => 'invisible',
    '#date_year_range' => '' . $year_offset_start . ':' . $year_offset_start,
    '#datepicker_options' => array(
      'changeYear' => FALSE,
    ),
    '#expected_format' => array('date' => 'M d, Y'),
    '#required' => TRUE,
  );

  $form['holiday']['date_stop'] = array(
    '#id' => 'section-holiday-date_stop',
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => 'Stop Date',
    '#description' => 'The stop day is exclusive, meaning that the date specified here is <strong>not</strong> a specified holiday.',
  );

  $form['holiday']['date_stop']['day'] = array(
    '#id' => 'field-holiday-date_stop-day',
    '#type' => 'date_popup',
    '#title' => 'Stop Date',
    '#date_format' => 'M d, Y',
    '#date_label_position' => 'invisible',
    '#date_year_range' => '' . $year_offset_start . ':' . $year_offset_stop,
    '#datepicker_options' => array(
      'changeYear' => TRUE,
    ),
    '#expected_format' => array('date' => 'M d, Y'),
    '#required' => FALSE,
  );

  if ($is_manager || $is_administer) {
    $form['holiday']['enabled'] = array(
      '#id' => 'field-holiday-enabled',
      '#type' => 'select',
      '#title' => 'Enabled',
      '#options' => array(0 => 'No', 1 => 'Yes'),
    );
  }

  if (is_object($holiday)) {
    $form['holiday']['title']['#default_value'] = $holiday->title;
    $form['holiday']['type']['#default_value'] = $holiday->type;
    $form['holiday']['description']['#default_value'] = $holiday->description;
    $form['holiday']['date_start']['day']['#default_value'] = date('Y-m-d', $holiday->date_start);
    $form['holiday']['date_stop']['day']['#default_value'] = date('Y-m-d', $holiday->date_stop);

    if ($is_manager || $is_administer) {
      $form['holiday']['enabled']['#default_value'] = $holiday->enabled;
    }

    if ($holiday->type == MFCS_HOLIDAY_TYPE_NONE) {
      $form['holiday']['type']['#options'] = $holiday_types_all;

      if (!($is_manager || $is_administer)) {
        $form['holiday']['type']['#readonly'] = TRUE;
        $form['holiday']['type']['#disabled'] = TRUE;
      }
    }
  }

  $form['holiday']['help'] = array(
    '#id' => 'section-help',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => 'Help',
  );

  $form['holiday']['help']['inclusiveness'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );

  $form['holiday']['help']['inclusiveness']['#markup'] .= '<div class="inclusiveness">';
  $form['holiday']['help']['inclusiveness']['#markup'] .= '<h3 class="inclusiveness-title">Inclusiveness and Exclusiveness</h3>';
  $form['holiday']['help']['inclusiveness']['#markup'] .= '<div class="inclusiveness-text">';
  $form['holiday']['help']['inclusiveness']['#markup'] .= 'The start day is <em>inclusive</em>, but the stop day is <em>exclusive</em>.<br>';
  $form['holiday']['help']['inclusiveness']['#markup'] .= 'What this means is that if you say that start day is July 29th and the stop day is July 30th, then the holiday ends on the second before July 30th begins (aka: ends on 11:59:59pm).<br>';
  $form['holiday']['help']['inclusiveness']['#markup'] .= 'If you want to ensure that both July 29th and July 30th are included, then the stop day must be July 31st.<br>';
  $form['holiday']['help']['inclusiveness']['#markup'] .= '</div>';
  $form['holiday']['help']['inclusiveness']['#markup'] .= '</div>';

  $form['holiday']['submit'] = array(
    '#id' => 'mfcs-holiday-edit-submit',
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
    '#default_value' => 'Cancel',
  );

  $form['holiday']['submit']['submit'] = array(
    '#id' => 'mfcs-holiday-edit-submit-submit',
    '#name' => 'mfcs-holiday-edit-submit-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
  );

  return $form;
}

/**
 * Pre-Validate process for mfcs_request_holiday_0_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_holiday_0_edit_form()
 */
function mfcs_request_holiday_0_edit_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'mfcs-holiday-edit-submit-cancel') {
    $form['holiday']['title']['#required'] = FALSE;
    $form['holiday']['type']['#required'] = FALSE;
    $form['holiday']['date_start']['#required'] = FALSE;
  }
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
    // when the field is 'required', but is empty, for some reason an array is provided instead of a string.
    if (is_array($form_state['values']['holiday']['date_start']['day'])) {
      $form_state['values']['holiday']['date_start']['day'] = NULL;
    }

    $date_start = strtotime($form_state['values']['holiday']['date_start']['day']);
    $date_start = strtotime('midnight today', $date_start);

    if ($date_start === FALSE || date('Y-m-d', $date_start) != $form_state['values']['holiday']['date_start']['day']) {
      form_set_error('holiday][date_start][day', 'The specified start date day is not valid.');
    }
    elseif ((int) date('Y', $date_start) != $form['form']['year']['#value']) {
      form_set_error('holiday][date_start][day', 'The specified start date time must be set to the year ' . (int) $form['form']['year']['#value'] . '.');
    }

    if (isset($form_state['values']['holiday']['date_stop']['day'])) {
      $date_stop = strtotime($form_state['values']['holiday']['date_stop']['day']);

      if ($date_stop === FALSE || date('Y-m-d', $date_stop) != $form_state['values']['holiday']['date_stop']['day']) {
        form_set_error('holiday][date_stop][day', 'The specified stop date day is not valid.');
      }

      if ($date_stop == $date_start) {
          form_set_error('holiday][date_stop][day', 'The specified stop date day cannot be the same as the start date.');
      }

      if ($date_stop < $date_start) {
        form_set_error('holiday][date_stop][day', 'The specified stop date day cannot be before the start date.');
      }
    }

    $length = strlen($form_state['values']['holiday']['title']);
    if ($length > 255) {
      form_set_error('holiday][title', 'The title is ' . $length . ' characters long and cannot be longer than 255 characters.');
    }

    $length = strlen($form_state['values']['holiday']['description']);
    if ($length > 255) {
      form_set_error('holiday][description', 'The description is ' . $length . ' characters long and cannot be longer than 255 characters.');
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

  $holiday_id = NULL;
  $holiday = NULL;
  if (isset($form['form']['holiday']['#value']->id)) {
    $holiday = $form['form']['holiday']['#value'];
    $holiday_id = (int) $form['form']['holiday']['#value']->id;
  }

  $instance = mfcs_instance();
  $year_this = date('Y', $instance);

  if ($clicked_id == 'mfcs-holiday-edit-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      if (is_object($holiday)) {
        $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/view/' . $holiday->year . '/' . $holiday->id);
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/' . $form['form']['year']['#value']);
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
    $fields = array();
    $fields['title'] = $form_state['values']['holiday']['title'];
    $fields['type'] = $form_state['values']['holiday']['type'];
    $fields['description'] = $form_state['values']['holiday']['description'];
    $fields['year'] = (int) $form['form']['year']['#value'];
    $fields['updated'] = mfcs_instance();

    $fields['date_start'] = strtotime($form_state['values']['holiday']['date_start']['day']);
    $fields['date_start'] = strtotime('midnight today', $fields['date_start']);

    if (isset($form_state['values']['holiday']['date_stop']['day']) && strlen($form_state['values']['holiday']['date_stop']['day']) > 0) {
      $fields['date_stop'] = strtotime($form_state['values']['holiday']['date_stop']['day']);
      $fields['date_stop'] = strtotime('midnight today', $fields['date_stop']);
    }
    else {
      // when date stop is not specified, then the event is an all day event.
      // setting the stop date to 'tomorrow' logically produces an all day event.
      $fields['date_stop'] = strtotime('midnight tomorrow', $fields['date_start']);
    }

    if ($is_manager || $is_administer) {
      if (isset($form_state['values']['holiday']['enabled'])) {
        $fields['enabled'] = ($form_state['values']['holiday']['enabled'] ? 1 : 0);
      }
    }

    if (is_object($holiday)) {
      if ($form['form']['copy']['#value']) {
        $fields['created'] = $fields['updated'];

        $query = db_insert('mfcs_holiday');
        $query->fields($fields);
        $id = $query->execute();

        if (is_numeric($id)) {
          $holiday_id = (int) $id;
        }

        $change = 'copied from holiday ' . $holiday->id . '.';
      }
      else {
        // check to see if there are any changes made.
        $changes = array();
        foreach ($fields as $key => $value) {
          // do not check updated key because it must be different.
          if ($key == 'updated') {
            continue;
          }

          if ($value != $holiday->{$key}) {
            $changes[$key] = array(
              'from' => $holiday->{$key},
              'to' => $value,
            );
          }
        }

        $change = '';
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
            $change .= $key . ' has been changed from \'' . $value['from'] . '\' to \'' . $value['to'] . '\'. ';
          }

          trim($change);
        }
      }
    }
    else {
      $fields['created'] = $fields['updated'];

      $query = db_insert('mfcs_holiday');
      $query->fields($fields);
      $id = $query->execute();

      if (is_numeric($id)) {
        $holiday_id = (int) $id;
      }

      $change = 'created.';
    }

    if (!is_null($change)) {
      $fields = array();
      $fields['holiday_id'] = $holiday_id;
      $fields['date'] = mfcs_instance();
      $fields['user_id'] = $user->uid;
      $fields['change'] = $change;

      $query = db_insert('mfcs_log_holiday');
      $query->fields($fields);
      $query->execute();
    }

    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $success = FALSE;
  }

  if (!$success) {
    form_set_error('form', 'An error occurred while trying to edit the holiday. Please contact the support staff.');

    if (null($holiday)) {
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to create a holiday.', array(), WATCHDOG_ERROR);
    }
    elseif ($form['form']['copy']['#value']) {
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to copy the holiday: @holiday_id.', array('@holiday_id' => $holiday->id), WATCHDOG_ERROR);
    }
    else {
      watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to edit the holiday: @holiday_id.', array('@holiday_id' => $holiday->id), WATCHDOG_ERROR);
    }

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (is_numeric($holiday_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/view/' . $form['form']['year']['#value'] . '/' . $holiday_id);
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/' . $form['form']['year']['#value']);
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
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($year)) {
    cf_error::invalid_integer('year');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($holiday_id)) {
    cf_error::invalid_integer('holiday_id');

    drupal_not_found();
    drupal_exit();
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
  }

  if ($year != $holiday->year) {
    drupal_not_found();
    drupal_exit();
  }

  mfcs_add_canonical_headers('holiday-0/delete/' . $year . '/' . $holiday_id);

  drupal_set_title('Delete ' . $year . ' Holiday: ' . $holiday->title);

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

  $form['confirmation'] = array(
    '#markup' => '',
  );

  $form['confirmation']['#markup'] .= '<h3>Are you sure you want to delete the holiday?</h3> ';
  $form['confirmation']['#markup'] .= '<div>';
  $form['confirmation']['#markup'] .= '<strong>ID</strong>: ' . $holiday->id . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Title</strong>: ' . $holiday->title . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Year</strong>: ' . $holiday->year . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Date Start</strong>: ' . date(MFCS_DATE_FORMAT_DATE, $holiday->date_start) . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Date Stop</strong>: ' . date(MFCS_DATE_FORMAT_DATE, $holiday->date_stop) . '<br>';
  $form['confirmation']['#markup'] .= '<strong>Status</strong>: ' . ($holiday->enabled == 1 ? 'Enabled' : 'Disabled') . '<br>';
  $form['confirmation']['#markup'] .= '</div>';
  $form['confirmation']['#markup'] .= '<br>';

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
    '#default_value' => 'No',
  );

  $form['display']['submit']['yes'] = array(
    '#id' => 'mfcs-holiday-delete-submit-yes',
    '#name' => 'mfcs-holiday-delete-submit-yes',
    '#type' => 'submit',
    '#default_value' => 'Yes',
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
        $year = (int) $form['form']['year']['#value'];

        $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/' . $year);
      }
      else {
        $form_state['redirect'] = mfcs_build_redirect_array('holiday-0/view/' . $holiday_year . '/' . $holiday_id);
      }
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $fields = array();
  $fields['deleted'] = 1;
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
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $deleted = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    $deleted = FALSE;
  }

  if (!$deleted) {
    form_set_error('form', 'An error occurred while trying to delete the holiday. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to delete the holiday: @holiday_id.', array('@holiday_id' => $holiday_id), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('holiday-0');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Find any requests that conflict with the specified holiday.
 *
 * @param int $holiday_id
 *   The id of the holiday.
 *
 * @return array|FALSE
 *   An array of requests whose dates conflict with the specified holiday.
 *   FALSE is returned on error.
 */
function mfcs_request_holiday_0_check_for_conflicts($holiday_id) {
  if (!cf_is_integer($holiday_id)) {
    cf_error::invalid_integer('holiday_id');
    return FALSE;
  }

  try {
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_dates_date', 'mfdd', 'mc.request_id = mfdd.request_id AND mc.dates_date = mfdd.revision');
    $query->innerJoin('mfcs_holiday', 'mh', 'mfdd.value >= mh.date_start and mfdd.value < mh.date_stop');

    $query->addField('mer', 'id', 'id');
    $query->addField('mer', 'title', 'title');
    $query->addField('mer', 'location', 'location_id');
    $query->addField('mer', 'building', 'building_id');
    $query->addField('mer', 'room', 'room_id');

    $query->addField('mfdd', 'delta', 'delta');
    $query->addField('mfdd', 'value', 'date');

    $query->condition('mh.id', $holiday_id);

    $not_these_status = array(
      MFCS_REQUEST_STATUS_DELETED,
      MFCS_REQUEST_STATUS_CLOSED_DENIED,
      MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
      MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
      MFCS_REQUEST_STATUS_CANCELLED,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
    );

    $query->condition('mer.status', $not_these_status, 'NOT IN');
    $query->condition('mh.enabled', 1);
    $query->condition('mh.deleted', 0);

    $results = $query->execute()->fetchAll();
    if (!is_array($results)) {
      return FALSE;
    }
  }
  catch (Error $e) {
    cf_error::on_exception($e);
    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);
    return FALSE;
  }

  return $results;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
