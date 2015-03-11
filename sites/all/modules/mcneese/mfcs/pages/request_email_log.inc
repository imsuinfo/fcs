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
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_email_log_0_form($form, &$form_state, $page_number = 0) {
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

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/email_log-0/' . $page_number);

  $page_title = "E-mail Log";
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
  $class_destination = '';
  $class_title = '';
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
    elseif ($order == 'date') {
      $sorting['date'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_date .= 'sorting_1 ';
    }
    elseif ($order == 'destination') {
      $sorting['destination'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_destination .= 'sorting_1 ';
    }
    elseif ($order == 'title') {
      $sorting['title'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_title .= 'sorting_1 ';
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
  );

  $options['extra_fields']['users'] = array(
    'alias' => 'u',
    'join' => 'left',
    'on' => 'mle.destination = u.mail',
    'columns' => array(
      'user_id' => 'uid',
    ),
  );

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
  if ($email_logs_total > 0 && $options['start'] >= $email_logs_total) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $rows = array();
  foreach ($email_logs as $id => $email_log) {
    $row = array();


    $row['id'] = array(
      'data' => (int) $email_log->id,
      'class' => $class_id,
    );

    $row['date'] = array(
      'data' => check_plain(date("Y/m/d h:i:s a", $email_log->date)),
      'class' => $class_date,
    );

    if (empty($email_log->user_id)) {
      $row['destination'] = array(
        'data' => check_plain($email_log->destination),
        'class' => $class_destination,
      );
    }
    else {
      $row['destination'] = array(
        'data' => '<a href="' . $base_path . 'user/' . $email_log->user_id . $url_arguments . '" class="user-link">' . check_plain($email_log->destination) . '</a>',
        'class' => $class_destination,
      );
    }

    $row['title'] = array(
      'data' => check_plain($email_log->title),
      'class' => $class_title,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/email_log-0/view/' . $email_log->id . $url_arguments . '" class="operation-link operation-link-view">View</a>',
      'class' => $class_operations,
    );

    $rows[] = $row;
  }


  $email_log_list = array();
  $email_log_list['caption'] = t("E-mail Log");
  $email_log_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'date' => array('field' => 'date', 'data' => "Date", 'align' => 'center'),
    'destination' => array('field' => 'destination', 'data' => "Destination", 'align' => 'center'),
    'title' => array('field' => 'title', 'data' => "Title", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
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
  $email_log_list['empty'] = t("This system has sent no e-mails.");

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
    'ajax' => "'" . $base_path . 'requests/email_log-0/ajax' . $url_arguments . "'",
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'date', 'name' => "Date", 'className' => 'column-date'),
      array('id' => 2, 'column' => 'destination', 'name' => "Destination", 'className' => 'column-destination'),
      array('id' => 3, 'column' => 'title', 'name' => "Title", 'className' => 'column-title'),
      array('id' => 4, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
    ),
    'displayStart' => $options['start'],
    'pageLength' => $rows_per_page,
    'dom' => 'lfCrtip',
    'KeyTable' => FALSE,
    'oColVis' => array(
      'buttonText' => '"Show / Hide"',
    ),
    'oLanguage' => array(
      'sSearch' => '"Filter: "',
    ),
    'searchDelay' => 1200,
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

  if ($navigation_total > $email_logs_total) {
    $navigation_total = $email_logs_total;
  }

  $navigation_bottom .= '<div class="table_listing-comment">' . "\n";
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $email_logs_total . " entries.\n";
  $navigation_bottom .= '</div>' . "\n";

  if ($email_logs_total > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/email_log-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/email_log-0/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form = array();

  $form['email_log_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['email_log_list']['#markup'] .= mcneese_libraries_theme_table($email_log_list, $datatable_settings);
  $form['email_log_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

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
 * @param int $log_id
 *   The id of the log to show.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_email_log_0_view_page($log_id) {
  if (!cf_is_integer($log_id) || $log_id < 0) {
    cf_error::invalid_integer('log_id');

    drupal_not_found();
    drupal_exit();
  }

  mfcs_add_canonical_headers('requests/email_log-0/view/' . $log_id);

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
    $query->condition('mle.id', $log_id);

    $log = $query->execute()->fetchObject();
  } catch (Exception $ex) {
    cf_error::on_exception($ex);

    $log = NULL;
  }

  $markup = '<div id="mfcs-request-email_log-0-page" class="email_log">' . "\n";

  $markup .= '<div class="email_log-title-wrapper">' . "\n";
  $markup .= '<h3 class="email_log-title">E-mail #' . $log->id . ': ' . check_plain($log->title) . '</h3>' . "\n";
  $markup .= '</div>' . "\n"; // email_log-title-wrapper

  $markup .= '<div class="email_log-section-top">';
  $markup .= '<div class="email_log-date-wrapper">' . "\n";
  $markup .= '<div class="email_log-date"><strong>Submitted On</strong>: ' . date('Y/m/d h:i:s a T', $log->date) . '.</div>' . "\n";
  $markup .= '</div>' . "\n"; // email_log-date-wrapper

  $markup .= '<div class="email_log-date-wrapper">' . "\n";
  $markup .= '<div class="email_log-date"><strong>Sent To</strong>: <code>' . check_plain($log->destination) . '</code>.</div>' . "\n";
  $markup .= '</div>' . "\n"; // email_log-date-wrapper
  $markup .= '</div>' . "\n"; // email_log-section-top


  // headers fieldset
  $fieldset = array(
    '#id' => 'email_log-' . $log->id . '-section-headers',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => '<h4 class="email_log-fieldset-title">' . "E-mail Headers" . '</h4>',
    '#parents' => array(),
    '#children' => '',
    '#attributes' => array(
      'class' => array('email_log-headers-wrapper', 'fieldset_print_as_div'),
    ),
  );

  $boundary = NULL;
  $fieldset['#children'] .= '<div class="email_log-headers"><pre>' . "\n";
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
  $fieldset['#children'] .= '</pre></div>' . "\n";

  $state = array();
  form_process_fieldset($fieldset, $state);
  $markup .= theme('fieldset', array('element' => $fieldset));


  // raw_body fieldset
  $fieldset = array(
    '#id' => 'email_log-' . $log->id . '-section-raw_body',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => '<h4 class="email_log-fieldset-title">' . "E-mail Body (Raw)" . '</h4>',
    '#parents' => array(),
    '#children' => '',
    '#attributes' => array(
      'class' => array('email_log-raw_body-wrapper', 'fieldset_print_as_div'),
    ),
  );

  $fieldset['#children'] .= '<div class="email_log-headers"><pre>' . "\n";
  $body = unserialize($log->body);
  if (is_string($body)) {
    $fieldset['#children'] .= check_plain($body) . "\n";
  }
  $fieldset['#children'] .= '</pre></div>' . "\n";

  $state = array();
  form_process_fieldset($fieldset, $state);
  $markup .= theme('fieldset', array('element' => $fieldset));

  // break the e-mail apart based on the boundary settings.
  if (!is_null($boundary)) {
    $b = preg_replace('@--' . $boundary . '--@i', '--' . $boundary, $body);

    $body_parts = explode('--' . $boundary, $b);
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

      if ($type == 'multipart/related') {
        // this assumes an expected format of: ---alt_$sub_boundary
        $bp = preg_replace('@^---alt_' . $sub_boundary . '--$@i', '', $body_part);

        $sub_body_parts = explode('---alt_' . $sub_boundary, $bp);
        if (empty($sub_body_parts)) {
          continue;
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
          '#id' => 'email_log-' . $log->id . '-section-body-' . $delta . '-text_plain',
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

        $fieldset['#children'] .= '<div class="email_log-body"><pre>' . "\n";
        if (is_string($text_plain)) {
          $fieldset['#children'] .= check_plain($text_plain) . "\n";
        }
        $fieldset['#children'] .= '</pre></div>' . "\n";

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
          '#id' => 'email_log-' . $log->id . '-section-body-' . $delta . '-text_plain',
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

        $fieldset['#children'] .= '<div class="email_log-body">' . "\n";
        if (is_string($text_html)) {
          $fieldset['#children'] .= check_markup($text_html, 'full_html') . "\n";
        }
        $fieldset['#children'] .= '</div>' . "\n";

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
          '#id' => 'email_log-' . $log->id . '-section-body-' . $delta,
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

        $fieldset['#children'] .= '<div class="email_log-body"><pre>' . "\n";
        if (is_string($body_part)) {
          $fieldset['#children'] .= check_plain($body_part) . "\n";
        }
        $fieldset['#children'] .= '</pre></div>' . "\n";

        $state = array();
        form_process_fieldset($fieldset, $state);
        $markup .= theme('fieldset', array('element' => $fieldset));
      }
    }
  }

  $markup .= '</div>' . "\n"; // #mfcs-request-email_log-0-page

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
