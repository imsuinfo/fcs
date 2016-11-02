<?php

/**
 * @file
 * Defines McNeese facilities use request email log page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request e-mail log main page(s).
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_email_log_0_form($form, &$form_state, $page_number = NULL) {
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

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $no_js_fallback = TRUE;
  if (isset($conf['no_js_fallback']) && is_bool($conf['no_js_fallback'])) {
    $no_js_fallback = $conf['no_js_fallback'];
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/email_log-0/' . $page_number);

  $page_title = 'E-mail Log';
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
  $class_user_id = 'no-script-hidden ';
  $class_name = '';
  $class_date = '';
  $class_destination = '';
  $class_title = '';
  $class_code = '';
  $class_type = '';
  $class_operations = 'no-print ';


  $sorting = array();
  if (is_null($order)) {
    $sorting['id'] = 'DESC';
    $order_column = array(0 => strtolower($sorting['id']));
    $class_id .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'account id') {
      $sorting['user_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_user_id .= 'sorting_1 ';
    }
    elseif ($order == 'account') {
      $sorting['name'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_name .= 'sorting_1 ';
    }
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    elseif ($order == 'destination') {
      $sorting['destination'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_destination .= 'sorting_1 ';
    }
    elseif ($order == 'title') {
      $sorting['title'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_title .= 'sorting_1 ';
    }
    elseif ($order == 'code') {
      $sorting['code'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(6 => strtolower($sorting[$order]));
      $class_code .= 'sorting_1 ';
    }
    elseif ($order == 'type') {
      $sorting['type'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(7 => strtolower($sorting[$order]));
      $class_type .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'DESC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 20;
  $options = array(
    'sort' => $sorting,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
    'fields' => array(
      'id' => 'id',
      'destination' => 'destination',
      'title' => 'title',
      'user_id' => 'user_id',
      'date' => 'date',
      'code' => 'code',
    ),
    'associative' => FALSE,
    'distinct_id' => TRUE,
  );

  $options['extra_fields']['users'] = array(
    'alias' => 'u',
    'join' => 'inner',
    'on' => 'mle.user_id = u.uid',
    'columns' => array(
      'user_id' => 'uid',
      'name' => 'name',
    ),
  );

  $options['extra_fields2']['users'] = array(
    'alias' => 'd_u',
    'join' => 'left',
    'on' => 'mle.destination = d_u.mail',
    'columns' => array(
      'destination_user_id' => 'uid',
    ),
  );

  $options['extra_fields2']['mfcs_taxonomy_email_type'] = array(
    'alias' => 'mtet',
    'join' => 'inner',
    'on' => 'mle.type = mtet.id',
    'columns' => array(
      'type_name' => 'human_name',
    ),
  );

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $options['results'] = FALSE;
  }

  $conditions = array();

  $table_name = 'mfcs_log_emails';
  $table_alias = 'mle';
  $primary_key = 'id';

  $email_logs_returned = mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions, $options);

  $email_logs_total = 0;
  if (is_array($email_logs_returned) && array_key_exists('count', $email_logs_returned)) {
    $email_logs_total = $email_logs_returned['count'];
  }

  $email_logs = array();
  if (isset($email_logs_returned['results']) && is_array($email_logs_returned['results'])) {
    $email_logs = $email_logs_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($email_logs_total > 0 && $options['start'] >= $email_logs_total || $email_logs_total == 0 && $options['start'] > $email_logs_total) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // do not show count values when no fallback is available.
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $email_logs_total = 0;
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $may_resend_log = mfcs_page_generic_access('email_log-resend');

  $rows = array();
  foreach ($email_logs as $id => $email_log) {
    $row = array();

    $row['id'] = array(
      'data' => (int) $email_log->id,
      'class' => $class_id,
    );

    $row['account_id'] = array(
      'data' => $email_log->user_id,
      'class' => $class_user_id,
    );

    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'requests/users-0/view/' . $email_log->user_id . $url_arguments . '" class="user-link">' . check_plain($email_log->name) . '</a>' . ' <span class="user-id script-hidden">(id: ' .  $email_log->user_id . ')</span>',
      'class' => $class_name,
    );

    $row['date'] = array(
      'data' => check_plain(date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS, $email_log->date)),
      'class' => $class_date,
    );

    if (empty($email_log->destination_user_id)) {
      $row['destination'] = array(
        'data' => check_plain($email_log->destination),
        'class' => $class_destination,
      );
    }
    else {
      $row['destination'] = array(
        'data' => '<a href="' . $base_path . 'requests/users-0/view/' . $email_log->destination_user_id . $url_arguments . '" class="user-link">' . check_plain($email_log->destination) . '</a>',
        'class' => $class_destination,
      );
    }

    $row['title'] = array(
      'data' => check_plain($email_log->title),
      'class' => $class_title,
    );

    $row['code'] = array(
      'data' => 'Normal',
      'class' => $class_code,
    );

    if ($email_log->code == MFCS_EMAIL_CODE_RESEND) {
      $row['code']['data'] = 'Re-sent';
    }
    elseif ($email_log->code == MFCS_EMAIL_CODE_FORWARD) {
      $row['code']['data'] = 'Forwarded';
    }

    $row['type'] = array(
      'data' => check_plain($email_log->type_name),
      'class' => $class_type,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/email_log-0/view/' . $email_log->id . $url_arguments . '" class="operation-link operation-link-view">View</a>',
      'class' => $class_operations,
    );

    if ($may_resend_log) {
      $row['operations']['data'] .= ' <a href="' . $base_path . 'requests/email_log-0/resend/' . $email_log->id . $url_arguments . '/back_to_list" class="operation-link operation-link-resend">Re-send</a>';
    }

    $rows[] = $row;
  }


  $email_log_list = array();
  $email_log_list['caption'] = 'E-mail Log';
  $email_log_list['header'] = array(
    'id' => array('field' => 'id', 'data' => 'ID', 'align' => 'center'),
    'account_id' => array('field' => 'user_id', 'data' => 'Account ID', 'align' => 'center', 'class' => array('script-hidden')),
    'account' => array('field' => 'name', 'data' => 'Account', 'align' => 'center', 'class' => array('script-hidden')),
    'date' => array('field' => 'date', 'data' => 'Date', 'align' => 'center'),
    'destination' => array('field' => 'destination', 'data' => 'Destination', 'align' => 'center'),
    'title' => array('field' => 'title', 'data' => 'Title', 'align' => 'center'),
    'code' => array('field' => 'code', 'data' => 'Code', 'align' => 'center'),
    'type' => array('field' => 'type', 'data' => 'Type', 'align' => 'center'),
    'operations' => array('data' => 'Operations', 'align' => 'center', 'class' => array('no-print')),
  );

  $email_log_list['attributes'] = array();
  $email_log_list['attributes']['id'] = 'mfcs-email_log_form';
  $email_log_list['attributes']['class'] = array(
    'no_default',
    'mfcs-email_log_form',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $email_log_list['attributes']['summary'] = 'Log of e-mails sent out by the system.';

  $email_log_list['rows'] = $rows;
  $email_log_list['empty'] = 'This system has sent no e-mails.';

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
    'processing' => TRUE,
    'serverSide' => TRUE,
    'ajax' => array(
      'url' => '\'' . $base_path . 'requests/email_log-0/ajax' . $url_arguments . '\'',
      'type' => '\'POST\'',
    ),
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => 'ID', 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'account_id', 'name' => 'Account ID', 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 2, 'column' => 'account', 'name' => 'Account', 'className' => 'column-account', 'visible' => FALSE),
      array('id' => 3, 'column' => 'date', 'name' => 'Date', 'className' => 'column-date'),
      array('id' => 4, 'column' => 'destination', 'name' => 'Destination', 'className' => 'column-destination'),
      array('id' => 5, 'column' => 'title', 'name' => 'Title', 'className' => 'column-title'),
      array('id' => 6, 'column' => 'code', 'name' => 'Code', 'className' => 'column-code'),
      array('id' => 7, 'column' => 'code', 'name' => 'Type', 'className' => 'column-type'),
      array('id' => 7, 'column' => NULL, 'name' => 'Operations', 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter"',
      'sEmptyTable' => '"' . $email_log_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $email_log_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/email_log-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/email_log-0' . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/email_log-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/email_log-0/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($email_logs_total == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $email_logs_total) {
    $navigation_total = $email_logs_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $email_logs_total . " entries.\n";
  $navigation_bottom .= '</div>';

  if ($email_logs_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/email_log-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/email_log-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['email_log_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['email_log_list']['#markup'] .= mcneese_libraries_theme_table($email_log_list, $datatable_settings);
  #$form['email_log_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  return $form;
}

/**
 * Provides the facilities use request e-mail log view page.
 *
 * Some of the code provided here is essentially hard-coded processing of an
 * expected e-mail format that is submitted by this system. This is done to
 * save time and keep the code simpler that it otherwise would be. Changes to
 * the structure of e-mails sent by this system may potentially cause problems
 * with the e-mail parsing code provided within this function.
 *
 * @param int $id
 *   The id of the log to show.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_email_log_0_view_page($id) {
  if (!cf_is_integer($id) || $id < 0) {
    cf_error::invalid_integer('log_id');

    drupal_not_found();
    drupal_exit();
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/email_log-0/view/' . $id);

  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');

  $log = NULL;
  try {
    $query = db_select('mfcs_log_emails', 'mle');
    $query->addField('mle', 'id', 'id');
    $query->addField('mle', 'date', 'date');
    $query->addField('mle', 'destination', 'destination');
    $query->addField('mle', 'title', 'title');
    $query->addField('mle', 'headers', 'headers');
    $query->addField('mle', 'body', 'body');
    $query->addField('mle', 'code', 'code');
    $query->addField('mle', 'type', 'type');
    $query->addField('mle', 'destination_user_id', 'destination_user_id');
    $query->condition('mle.id', $id);

    $log = $query->execute()->fetchObject();
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    $log = NULL;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    $log = NULL;
  }

  // no valid object was found, so the specified id is invalid.
  if (!is_object($log)) {
    cf_error::invalid_integer('log_id');

    drupal_not_found();
    drupal_exit();
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $valid_destination = TRUE;
  if (strpos($log->destination, '@') === FALSE) {
    $valid_destination = FALSE;
  }

  $markup = '<div id="mfcs-request-email_log-0-page" class="email_log">';

  $markup .= '<div class="email_log-title-wrapper">';
  $markup .= '<h3 class="email_log-title">E-mail #' . $id . ': ' . check_plain($log->title) . '</h3>';
  $markup .= '</div>'; // email_log-title-wrapper

  $markup .= '<div class="email_log-section-top">';
  $markup .= '<div class="email_log-date-wrapper">';
  $markup .= '<div class="email_log-date"><strong>Submitted On</strong>: ' . date(MFCS_DATE_FORMAT_DATE_TIME_SECONDS_TIMEZONE, $log->date) . '.</div>';
  $markup .= '</div>'; // email_log-date-wrapper

  $markup .= '<div class="email_log-destination-wrapper">';
  if ($valid_destination) {
    $markup .= '<div class="email_log-destination"><strong>Sent To</strong>: <code>' . check_plain($log->destination) . '</code>.</div>';
  }
  else {
    $markup .= '<div class="email_log-destination"><strong>Sent To</strong>: <span class="email_log-destination-invalid">(Warning: Invalid e-mail detected)</span> <code>' . check_plain($log->destination) . '</code>.</div>';
  }
  $markup .= '</div>'; // email_log-destination-wrapper

  if ($valid_destination && !empty($log->destination_user_id) && is_numeric($log->destination_user_id)) {
    $destination_users = mfcs_load_basic_user_settings_by_user_id((int) $log->destination_user_id);

    if (!empty($destination_users)) {
      $user_name = NULL;
      if (!empty($destination_users['last_name'])) {
        $user_name .= $destination_users['last_name'];
      }

      if (!empty($destination_users['first_name'])) {
        if (!empty($user_name)) {
          $user_name .=  ', ';
        }

        $user_name .= $destination_users['first_name'];
      }

      if (empty($user_name)) {
        $user_name = $destination_users['name'];
      }

      $markup .= '<div class="email_log-destination_user-wrapper">';
      $markup .= '<div class="email_log-destination_user"><strong>Sent To User</strong>: <a href="' . $base_path . 'requests/users-0/view/' . $log->destination_user_id . $url_arguments . '" class="email_log-destination_user-link">' . check_plain($user_name) . '</a> <sup class="email_log-destination_user-user_id">[id: ' . ((int) $log->destination_user_id) . ']</sup>.</div>';
      $markup .= '</div>'; // email_log-destination_user-wrapper

      unset($user_name);
    }

    unset($destination_users);
  }

  $email_code = 'Normal';
  if ($log->code == MFCS_EMAIL_CODE_RESEND) {
    $email_code = 'Re-sent';
  }
  elseif ($log->code == MFCS_EMAIL_CODE_FORWARD) {
    $email_code = 'Forwarded';
  }

  $markup .= '<div class="email_log-code-wrapper">';
  $markup .= '<div class="email_log-code"><strong>Code</strong>: ' . $email_code . '.</div>';
  $markup .= '</div>'; // email_log-code-wrapper
  unset($email_code);

  $email_type = 'None';
  $email_type_options = mfcs_get_email_type_list_options(NULL, TRUE, TRUE);
  if (isset($log->type) && array_key_exists($log->type, $email_type_options)) {
    $email_type = $email_type_options[$log->type];
  }

  $markup .= '<div class="email_log-type-wrapper">';
  $markup .= '<div class="email_log-type"><strong>Type</strong>: ' . $email_type . '.</div>';
  $markup .= '</div>'; // email_log-type-wrapper
  unset($email_type);

  $markup .= '</div>'; // email_log-section-top


  // headers fieldset
  $fieldset = array(
    '#id' => 'email_log-' . $id . '-section-headers',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => '<h4 class="email_log-fieldset-title">E-mail Headers</h4>',
    '#parents' => array(),
    '#children' => '',
    '#attributes' => array(
      'class' => array('email_log-headers-wrapper', 'fieldset_print_as_div'),
    ),
  );

  $boundary = NULL;
  $fieldset['#children'] .= '<div class="email_log-headers"><pre>';
  $headers = unserialize($log->headers);
  if (is_array($headers)) {
    foreach ($headers as $header_name => $header) {
      $fieldset['#children'] .= '<strong>' . check_plain($header_name) . '</strong>: ' . check_plain($header) . ";\n";

      if ($header_name == 'Content-Type') {
        $matches = array();
        $matched = preg_match('@boundary="([\w|\d|_]*)"@i', $header, $matches);

        if (!empty($matches[1])) {
          $boundary = $matches[1];
        }
      }
    }
  }
  $fieldset['#children'] .= '</pre></div>';

  $state = array();
  form_process_fieldset($fieldset, $state);
  $markup .= theme('fieldset', array('element' => $fieldset));
  unset($fieldset);
  unset($state);


  // raw_body fieldset
  $fieldset = array(
    '#id' => 'email_log-' . $id . '-section-raw_body',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => '<h4 class="email_log-fieldset-title">E-mail Body (Raw)</h4>',
    '#parents' => array(),
    '#children' => '',
    '#attributes' => array(
      'class' => array('email_log-raw_body-wrapper', 'fieldset_print_as_div'),
    ),
  );

  $fieldset['#children'] .= '<div class="email_log-headers"><pre>';
  $body = unserialize($log->body);
  if (is_string($body)) {
    $fieldset['#children'] .= check_plain($body);
  }
  $fieldset['#children'] .= '</pre></div>';

  $state = array();
  form_process_fieldset($fieldset, $state);
  $markup .= theme('fieldset', array('element' => $fieldset));
  unset($fieldset);
  unset($state);

  // break the e-mail apart based on the boundary settings.
  $text_html = NULL;
  if (!is_null($boundary)) {
    // convert the final boundary into the same format as the initial boundary to make explode() operations work smoothly.
    $b = preg_replace('@^--' . $boundary . '--$@im', '', $body);

    $body_parts = explode('--' . $boundary . "\n", $b);
    array_shift($body_parts);

    foreach ($body_parts as $delta => $body_part) {
      if (empty($body_part)) continue;

      $matches = array();
      $matched = preg_match('@Content-Type: (.*)' . "\n(  .*\n)*" . '@i', $body_part, $matches);

      if ($matched) {
        $match = explode(';', $matches[1]);
        $type = strtolower($match[0]);

        $sub_boundary = NULL;
        if (!empty($match[1])) {
          $sub_boundary = preg_replace('@^  boundary="@i', '', $match[1]);
          $sub_boundary = preg_replace('@"\s*$@', '', $sub_boundary);
        }
        elseif (!empty($matches[2])) {
          $sub_boundary = preg_replace('@^  boundary="@i', '', $matches[2]);
          $sub_boundary = preg_replace('@"\s*$@', '', $sub_boundary);
        }
      }
      else {
        // skip unknown or missing types.
        continue;
      }

      $transfer_encoding = NULL;
      $matches = array();
      $matched = preg_match('@Content-Transfer-Encoding: (.*)' . "\n(  .*\n)*" . '@i', $body_part, $matches);
      if ($matched) {
        $match = explode(';', $matches[1]);
        $transfer_encoding = strtolower($match[0]);
      }

      $description = NULL;
      $matches = array();
      $matched = preg_match('@Content-Description: (.*)' . "\n(  .*\n)*" . '@i', $body_part, $matches);
      if ($matched) {
        $match = explode(';', $matches[1]);
        $description = strtolower($match[0]);
      }

      if ($type == 'multipart/related' || $type == 'multipart/alternative') {
        if ($type == 'multipart/related') {
          $bp = preg_replace('@^--' . $sub_boundary . '--$@im', '', $body_part);

          $sub_body_parts = preg_split('@^--' . $sub_boundary .'$@im', $bp);
          if (empty($sub_body_parts) && count($sub_body_parts) > 1) {
            continue;
          }

          $bp = preg_replace('@^---alt_' . $sub_boundary . '--$@im', '', $sub_body_parts[1]);

          $sub_body_parts = preg_split('@^---alt_' . $sub_boundary .'$@im', $bp);
          if (empty($sub_body_parts)) {
            continue;
          }
        }
        else {
          $bp = preg_replace('@^--' . $sub_boundary . '--$@im', '', $body_part);

          //$sub_body_parts = explode('--' . $sub_boundary . "\n", $bp);
          $sub_body_parts = preg_split('@^--' . $sub_boundary .'$@im', $bp);
          if (empty($sub_body_parts)) {
            continue;
          }
        }

        array_shift($sub_body_parts);
        if (empty($sub_body_parts)) {
          continue;
        }


        // handle #0, which is expected to be text/plain.
        $text_plain_description = NULL;
        $matches = array();
        $matched = preg_match('@Content-Description: (.*)' . "\n(  .*\n)*" . '@i', $sub_body_parts[0], $matches);
        if ($matched) {
          $match = explode(';', $matches[1]);
          $text_plain_description = strtolower($match[0]);
        }

        $encoded_text_plain = preg_replace('@Content-(.*): (.*)' . "\n(  .*\n)*" . '@i', '', $sub_body_parts[0]);
        $text_plain = quoted_printable_decode($encoded_text_plain);
        $text_plain = preg_replace("@^\n+@", '', $text_plain);
        $text_plain = preg_replace("@\n+$@", '', $text_plain);

        $fieldset = array(
          '#id' => 'email_log-' . $id . '-section-body-' . $delta . '-text_plain',
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#title' => '<h4 class="email_log-fieldset-title">' . $text_plain_description . '</h4>',
          '#parents' => array(),
          '#children' => '',
          '#attributes' => array(
            'class' => array('email_log-body-wrapper', 'fieldset_print_as_div'),
          ),
        );

        $fieldset['#children'] .= '<div class="email_log-body"><pre>';
        if (is_string($text_plain)) {
          $fieldset['#children'] .= check_plain($text_plain);
        }
        $fieldset['#children'] .= '</pre></div>';

        $state = array();
        form_process_fieldset($fieldset, $state);
        $markup .= theme('fieldset', array('element' => $fieldset));


        // handle #1, which is expected to be text/html.
        $text_html_description = NULL;
        $matches = array();
        $matched = preg_match('@Content-Description: (.*)' . "\n(  .*\n)*" . '@i', $sub_body_parts[1], $matches);
        if ($matched) {
          $match = explode(';', $matches[1]);
          $text_html_description = strtolower($match[0]);
        }

        $encoded_text_html = preg_replace('@Content-(.*): (.*)' . "\n(  .*\n)*" . '@i', '', $sub_body_parts[1]);
        $text_html = quoted_printable_decode($encoded_text_html);
        $text_html = preg_replace("@^\n+@", '', $text_html);
        $text_html = preg_replace("@\n+$@", '', $text_html);

        $fieldset = array(
          '#id' => 'email_log-' . $id . '-section-body-' . $delta . '-text_plain',
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          '#title' => '<h4 class="email_log-fieldset-title">' . $text_html_description . '</h4>',
          '#parents' => array(),
          '#children' => '',
          '#attributes' => array(
            'class' => array('email_log-body-wrapper', 'fieldset_print_as_div'),
          ),
        );

        $fieldset['#children'] .= '<div class="email_log-body">';
        if (is_string($text_html)) {
          $fieldset['#children'] .= check_markup($text_html, 'full_html');
        }
        $fieldset['#children'] .= '</div>';

        $state = array();
        form_process_fieldset($fieldset, $state);
        $markup .= theme('fieldset', array('element' => $fieldset));
      }
      elseif ($type == 'application/pgp-signature') {
        $body_part = preg_replace('@Content-(.*): (.*)' . "\n(  .*\n)*" . '@i', '', $body_part);
        $body_part = preg_replace("@^\n+@", '', $body_part);
        $body_part = preg_replace("@\n+$@", '', $body_part);

        // body fieldset
        $fieldset = array(
          '#id' => 'email_log-' . $id . '-section-body-' . $delta,
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          '#title' => '<h4 class="email_log-fieldset-title">' . $description . '</h4>',
          '#parents' => array(),
          '#children' => '',
          '#attributes' => array(
            'class' => array('email_log-body-wrapper', 'fieldset_print_as_div'),
          ),
        );

        $fieldset['#children'] .= '<div class="email_log-body"><pre>';
        if (is_string($body_part)) {
          $fieldset['#children'] .= check_plain($body_part);
        }
        $fieldset['#children'] .= '</pre></div>';

        $state = array();
        form_process_fieldset($fieldset, $state);
        $markup .= theme('fieldset', array('element' => $fieldset));
      }
    }
  }

  // Operations section, where 'resend' and 'forward' links may exist.
  $may_resend_log = mfcs_page_generic_access('email_log-resend');

  if ($may_resend_log) {
    $fieldset = array(
      '#id' => 'email_log-' . $id . '-section-operations',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => '<h4 class="email_log-fieldset-title">Operations</h4>',
      '#parents' => array(),
      '#children' => '',
      '#attributes' => array(
        'class' => array('email_log-operations-wrapper', 'no-print'),
      ),
    );

    $fieldset['#children'] .= '<div class="email_log-operations">';
    $fieldset['#children'] .= '<div class="email_log-operations-operation"><a href="' . $base_path . 'requests/email_log-0' . $url_arguments . '">Back to Log Listing</a></div>';

    if (is_string($text_html) && $valid_destination) {
      $fieldset['#children'] .= '<div class="email_log-operations-operation"><a href="' . $base_path . 'requests/email_log-0/resend/' . $id . $url_arguments . '">Re-send E-mail</a></div>';
    }
    $fieldset['#children'] .= '</div>';

    $state = array();
    form_process_fieldset($fieldset, $state);
    $markup .= theme('fieldset', array('element' => $fieldset));
  }



  $markup .= '</div>'; // #mfcs-request-email_log-0-page

  return $markup;
}

/**
 * Provides the facilities use request agreement page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $id
 *   The unique identifier for the e-mail log.
 * @param null|string $extra_arguments
 *   (optional) Additional arguments accepted by this function.
 *   Currently this only supports 'back_to_list'.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_email_log_0_resend_form($form, &$form_state, $id, $extra_arguments = NULL) {
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

  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_null($extra_arguments) && !is_string($extra_arguments)) {
    cf_error::invalid_string('extra_arguments');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  $back_to_list = FALSE;
  if (!is_null($extra_arguments)) {
    if ($extra_arguments == 'back_to_list') {
      $back_to_list = TRUE;
    }
    else {
      // only provide page when $extra_arguments is valid.
      drupal_not_found();
      drupal_exit();
      return array();
    }
  }

  $log = NULL;
  try {
    $query = db_select('mfcs_log_emails', 'mle');
    $query->addField('mle', 'id', 'id');
    $query->addField('mle', 'date', 'date');
    $query->addField('mle', 'destination', 'destination');
    $query->addField('mle', 'title', 'title');
    $query->addField('mle', 'headers', 'headers');
    $query->addField('mle', 'body', 'body');
    $query->condition('mle.id', $id);

    $log = $query->execute()->fetchObject();
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    $log = NULL;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    $log = NULL;
  }

  if (!is_object($log)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/email_log-0/resend/' . $id);

  $page_title = "Re-send E-mail";
  if (!empty($log->title)) {
    $page_title = "Re-send E-mail: " . $log->title;
  }
  drupal_set_title($page_title);

  $module_path = drupal_get_path('module', 'mfcs');

  $form = array();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  $form['form']['back_to_list'] = array(
    '#type' => 'value',
    '#value' => $back_to_list,
  );

  if (empty($form_state['values']['redirect_to'])) {
    if ($back_to_list) {
      $destination = 'requests/email_log-0' . $url_arguments;
    }
    else {
      $destination = 'requests/email_log-0/view/' . $id . $url_arguments;
    }
  }
  else {
    $destination = $form_state['values']['redirect_to'];
  }

  return confirm_form($form,
    t('Please confirm re-sending the e-mail to %destination', array('%destination' => $log->destination)),
    $destination,
    t('The e-mail <strong>%title</strong> will be sent to <strong>%destination</strong>?', array('%title' => $log->title, '%destination' => $log->destination)),
    t('Re-send E-Mail'),
    t('Cancel')
  );
}

/**
 * Validation for mfcs_request_email_log_0_resend_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_email_log_0_resend_form()
 */
function mfcs_request_email_log_0_resend_form_validate($form, &$form_state) {
  // nothing to validate.
}

/**
 * Submit process for mfcs_request_email_log_0_resend_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_email_log_0_resend_form()
 */
function mfcs_request_email_log_0_resend_form_submit($form, &$form_state) {
  $log = NULL;
  try {
    $query = db_select('mfcs_log_emails', 'mle');
    $query->addField('mle', 'id', 'id');
    $query->addField('mle', 'date', 'date');
    $query->addField('mle', 'destination', 'destination');
    $query->addField('mle', 'title', 'title');
    $query->addField('mle', 'headers', 'headers');
    $query->addField('mle', 'body', 'body');
    $query->addField('mle', 'type', 'type');
    $query->condition('mle.id', $form['form']['id']['#value']);

    $log = $query->execute()->fetchObject();
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    $log = NULL;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    $log = NULL;
  }

  if (!is_object($log)) {
    drupal_set_message(t('Failed to load log with id of @id.', array('@id' => $form['form']['id']['#value'])), 'error');

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // it is possible for e-mails to have no destination in the database.
  // this is incorrect, but there was some bug in the past that saved the entries.
  // prevent sending of e-mails with no valid destination address.
  if (strpos($log->destination, '@') === FALSE) {
    drupal_set_message(t('Canno re-send e-mail [id: @id] because no valid destination address exists.', array('@id' => $form['form']['id']['#value'])), 'error');
    watchdog(MFCS_WATCHDOG_ID, 'Cannot re-send e-mail [id: @id] because no valid destination address exists.', array('@id' => $form['form']['id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  $destinations = array($log->destination => $log->destination);
  $body = unserialize($log->body);

  // for testing, add/remove email addresses
  global $conf;

  $testing = isset($conf['fcs_testing']) && $conf['fcs_testing'];
  $debugging_email = isset($conf['fcs_testing-email_to-debug']) && $conf['fcs_testing-email_to-debug'];

  // the e-mail whitelist supercedes the "remove email" addresses.
  $email_whitelist = array();
  if (isset($conf['fcs_testing-email_to-whitelist']) && is_array($conf['fcs_testing-email_to-whitelist'])) {
    $email_whitelist = $conf['fcs_testing-email_to-whitelist'];
  }

  // when testing, prepend the testing prefix to e-mails to distinguish between production and non-production e-mails.
  if ($testing) {
    if (isset($conf['fcs_testing-email_to-prefix']) && is_string($conf['fcs_testing-email_to-prefix'])) {
      $regex_safe_search = preg_quote($conf['fcs_testing-email_to-prefix'], '/');
      if (preg_match('/^' . $regex_safe_search . '/i', $log->title) == 0) {
        $log->title = $conf['fcs_testing-email_to-prefix'] . $log->title;
      }
    }
  }

  if ($testing && $debugging_email) {
    $debug_accounts = array();

    if (isset($conf['fcs_testing-email_to-add']) && is_array($conf['fcs_testing-email_to-add'])) {
      foreach ($conf['fcs_testing-email_to-add'] as $add_email) {
        $destinations[$add_email] = $add_email;
        $debug_accounts[$add_email] = $add_email;
      }
    }

    if (isset($conf['fcs_testing-email_to-remove']) && is_array($conf['fcs_testing-email_to-remove'])) {
      foreach ($conf['fcs_testing-email_to-remove'] as $remove_email) {
        // the e-mail whitelist supercedes the "remove email" addresses
        if (!in_array($remove_email, $email_whitelist)) {
          unset($destinations[$remove_email]);
          unset($debug_accounts[$remove_email]);
        }
      }
    }

    foreach ($destinations as $to) {
      if (!in_array($to, $email_whitelist) && !in_array($to, $debug_accounts)) {
        drupal_set_message(t('Warning: not sending mail to @to while testing the system.', array('@to' => $to)), 'warning');
        #watchdog('debug', "Warning: not sending mail to @to while testing the system.", array('@to' => $to), WATCHDOG_WARNING);
        unset($destinations[$to]);
      }
    }
  }

  if (empty($destinations)) {
    drupal_set_message('There is no e-mail to re-send to. Perhaps e-mail debugging is enabled and no accounts are setup to receive e-mails.', 'warning');

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // bypass the drupal send functionality and directly send the e-mail.
  // this is done because the e-mail must be resent exactly as-is.
  $headers = unserialize($log->headers);

  $mimeheaders = array();
  foreach ($headers as $name => $value) {
    $mimeheaders[] = $name . ': ' . mime_header_encode($value);
  }

  // For headers, PHP's API suggests that we use CRLF normally,
  // but some MTAs incorrectly replace LF with CRLF. See #234403.
  $mail_headers = implode("\n", $mimeheaders);

  // Prepare mail commands.
  $mail_subject = mime_header_encode(substr($log->title, 0, MFCS_EMAIL_MAX_SUBJECT_LENGTH));

  $user = cf_current_user();
  $user_id = 0;
  if (isset($user->uid)) {
    $user_id = $user->uid;
  }

  $success = TRUE;
  $instance = mfcs_instance();
  foreach ($destinations as $destination) {
    // log the e-mail first so that if there is a problem writing the log, no e-mail will be sent.
    // this prevents errors in writing the log from hiding that an e-mail was in fact sent.
    $fields = array(
      'destination' => $destination,
      'title' => $log->title,
      'headers' => $log->headers,
      'body' => $log->body,
      'code' => MFCS_EMAIL_CODE_RESEND,
      'user_id' => $user_id,
      'date' => $instance,
      'type' => $log->type,
    );

    $destination_user = mfcs_get_user_by_email($destination);
    if (is_object($destination_user)) {
      $fields['destination_user_id'] = $destination_user->uid;
    }
    unset($destination_user);

    $transaction = db_transaction();

    try {
      $query = db_insert('mfcs_log_emails');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_exception($e);

      $success = FALSE;

      // do not send e-mail because of log saving problems.
      continue;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_exception($e);

      $success = FALSE;

      // do not send e-mail because of log saving problems.
      continue;
    }

    if (isset($mimeheaders['Return-Path']) && !ini_get('safe_mode')) {
      // On most non-Windows systems, the "-f" option to the sendmail command
      // is used to set the Return-Path. There is no space between -f and
      // the value of the return path.
      $mail_result = @mail(
        substr($destination, 0, MFCS_EMAIL_MAX_DESTINATION_LENGTH),
        $mail_subject,
        $body,
        $mail_headers,
        '-f' . $mimeheaders['Return-Path']
      );
    }
    else {
      // The optional $additional_parameters argument to mail() is not
      // allowed if safe_mode is enabled. Passing any value throws a PHP
      // warning and makes mail() return FALSE.
      $mail_result = @mail(
        substr($destination, 0, MFCS_EMAIL_MAX_DESTINATION_LENGTH),
        $mail_subject,
        $body,
        $mail_headers
      );
    }
  }

  if ($success) {
    drupal_set_message(t('E-mail has been re-sent to the following addresses: @emails', array('@emails' => implode(', ', $destinations))));
  }
  else {
    drupal_set_message(t('Failed to re-send e-mail to the following addresses: @emails', array('@emails' => implode(', ', $destinations))), 'error');

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;

    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $back_to_list = (bool) $form['form']['back_to_list']['#value'];

    if ($back_to_list) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/email_log-0');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/email_log-0/view/' . $form['form']['id']['#value']);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
