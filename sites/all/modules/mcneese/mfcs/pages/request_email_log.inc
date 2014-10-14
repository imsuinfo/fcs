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
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/email_log-0');

  global $base_path;
  static $mfcs_current;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }


  $form = array();
  $form['output'] = array('#markup' => '');

  $output = &$form['output']['#markup'];

  $rows_per_page = 25;
  $start_range = $page_number * $rows_per_page;

  $columns = array(
    'id' => "ID",
    'date' => "Date",
    'destination' => "Destination",
    'Title' => "Title",
    'operations' => "Operations",
  );

  // load sorting information
  $sorting = array();
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
    $sorting['order'] = $order;
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
    else {
      $sorting['sort'] = $sort;
    }
  }

  $logs = array();
  try {
    $query = db_select('mfcs_log_emails', 'mle');
    $query->addField('mle', 'id', 'id');
    $query->addField('mle', 'date', 'date');
    $query->addField('mle', 'destination', 'destination');
    $query->addField('mle', 'title', 'title');
    $query->range($start_range, $rows_per_page + 1);

    if (is_null($order)) {
      $query->orderBy('id', 'DESC');
    }
    else {
      if ($order == 'id') {
        $query->orderBy('id', is_null($sort) ? 'DESC' : $sort);
      }
      elseif ($order == 'date') {
        $query->orderBy('date', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'destination') {
        $query->orderBy('destination', is_null($sort) ? 'ASC' : $sort);
      }
      elseif ($order == 'title') {
        $query->orderBy('title', is_null($sort) ? 'ASC' : $sort);
      }
    }

    $logs = $query->execute()->fetchAll();
  } catch (Exception $ex) {
    cf_error::on_exception($ex);
  }

  $table = array();
  foreach ($columns as $column_name => $column_title) {
    $column_class = array();

    if ($column_name == 'operations') {
      $column_class[] = 'no-print';

      $table['header'][$column_name] = array(
        'data' => $column_title,
        'align' => 'center',
        'class' => $column_class,
      );
    }
    else {
      $table['header'][$column_name] = array(
        'data' => $column_title,
        'align' => 'center',
        'class' => $column_class,
      );

      $table['header'][$column_name]['field'] = $column_name;
    }
  }

  if (is_null($order) && isset($table['header']['id'])) {
    $table['header']['id']['sort'] = 'DESC';
  }

  $table['attributes'] = array();
  $table['attributes']['summary'] = "A listing of e-mails sent by this system.";
  $table['attributes']['class'] = array('mfcs-table-email_logs');
  $table['rows'] = array();
  $table['empty'] = t("No e-mails were found.");
  $table['caption'] = "E-mail Logs";

  $total_results = 0;

  if (!empty($logs)) {
    $total_results = count($logs);

    // the select query loads rows_per_page+1 to determine if the 'next' link should appear.
    // do not process this extra row if it exists.
    if ($total_results > $rows_per_page) {
      array_pop($logs);
    }

    foreach ($logs as $log) {
      $row = array();

      $row['id'] = array(
        'data' => $log->id,
        'class' => 'column-id',
      );

      $row['date'] = array(
        'data' => date("Y/m/d h:i:s a", $log->date),
        'class' => 'column-date',
      );

      $row['destination'] = array(
        'data' => check_plain($log->destination),
        'class' => 'column-destination',
      );

      $row['title'] = array(
        'data' => check_plain($log->title),
        'class' => 'column-title',
      );

      $row['operations'] = array(
        'data' => '<a href="' . $base_path . 'requests/email_log-0/view/' . $log->id . $url_arguments . '">View</a>',
        'class' => 'column-operations no-print',
      );

      $table['rows'][] = $row;
    }
  }


  $navigation = "";
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation .= '<a href="' . $base_path . 'requests/email_log-0' . $url_arguments . '" class="request-email_log-page-choice request-email_log-page-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation .= '<a href="' . $base_path . 'requests/email_log-0/' . $previous_page  . $url_arguments. '" class="request-email_log-page-choice request-email_log-page-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  if ($total_results > $rows_per_page) {
    $next_page = $page_number + 1;
    $navigation .= '<a href="' . $base_path . 'requests/email_log-0/' . $next_page . $url_arguments . '" class="request-email_log-page-choice request-email_log-page-next no-print" title="Next Page">Next</a>';
  }

  $output = '';
  $output .= '<div class="request-email_log-page-navigator request-email_log-page-navigator-top no-print">' . $navigation . '</div>';
  $output .= theme('table', $table);
  $output .= '<div class="request-email_log-page-navigator request-email_log-page-navigator-bottom no-print">' . $navigation . '</div>';

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
  if (!cf_is_integer($log_id)) {
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

        $text = quoted_printable_decode($body_part);

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
        if (is_string($text)) {
          $fieldset['#children'] .= check_plain($text) . "\n";
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
