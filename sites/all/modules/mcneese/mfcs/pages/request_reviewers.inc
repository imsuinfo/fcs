<?php

/**
 * @file
 * Defines McNeese facilities use request reviewer page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request reviewer page.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_reviewers_0_page() {
  mfcs_add_canonical_headers('requests/reviewers-0');

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $page_title = "Manage Reviewers";
  drupal_set_title($page_title);

  $user = cf_current_user();

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_classifications = mfcs_get_request_classification_list_options();
  $review_steps = mfcs_get_review_step_list_options('roles');

  $markup = '<div id="mfcs-manage_reviewers">';
  $groups = array();
  foreach ($request_classifications as $classification_id => $classification_label) {
    if (!is_numeric($classification_id)) continue;

    $items = array();
    foreach ($review_steps as $review_step_id => $review_step) {
      if (empty($review_step['roles'])) {
        continue;
      }

      // this only applies to two different steps.
      if ($review_step_id != MFCS_REVIEW_STEP_REVIEW && $review_step_id != MFCS_REVIEW_STEP_REQUIREMENTS) continue;

      $unmatched = TRUE;
      foreach ($review_step['roles'] as $role) {
        if ($role == $classification_id) {
          $unmatched = FALSE;
          break;
        }
      }

      if ($unmatched) {
        continue;
      }

      $link = '<a id="mfcs-manage_reviewers-manage-' . $classification_id . '-link-' . $review_step_id .'" href="' . $base_path . 'requests/reviewers-0/list/' . $classification_id . '/' . $review_step_id . $url_arguments . '" class="item-link mfcs-manage_reviewers-manage-link">' . $review_step['name'] . ' Step</a>';
      $items[] = array(
        'data' => $link,
        'class' => array(
        ),
      );
    }

    $item_list = theme('item_list', array('items' => $items, 'type' => 'ul'));

    $markup .= '<div id="mfcs-manage_reviewers-manage-' . $classification_id . '" class="mfcs-manage_reviewers-manage">';
    $markup .= '<h3 class="mfcs-manage_reviewers-manage-header">' . "Manage " . $classification_label . " Reviewers" . '</h3>';
    $markup .= $item_list;
    $markup .= '</div>';
  }

  $markup .= '</div>';

  return $markup;
}

/**
 * Provides the facilities use request reviewer page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $classification
 *   Number representing the request classification.
 *   This is not the reviewer classification.
 * @param int $review_step
 *   Number representing the request review step.
 * @param int|null $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_reviewers_0_form($form, &$form_state, $classification, $review_step, $page_number = NULL) {
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

  if (!cf_is_integer($classification) || $classification < 0) {
    cf_error::invalid_integer('classification');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($review_step) || $review_step < 0) {
    cf_error::invalid_integer('review_step');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (is_string($review_step)) {
    $review_step = (int) $review_step;
  }

  $auto_page_number = FALSE;
  if (is_null($page_number)) {
    $auto_page_number = TRUE;
    $page_number = 0;
  }

  if (is_string($page_number)) {
    $page_number = (int) $page_number;
  }

  if ($page_number < 0 || !is_int($page_number)) {
    $page_number = 0;
  }

  // only the specified steps can be used.
  if ($review_step !== MFCS_REVIEW_STEP_REVIEW && $review_step !== MFCS_REVIEW_STEP_REQUIREMENTS) {
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

  if (!cf_is_integer($page_number) || $page_number < 0) {
    $page_number = 0;
  }

  mfcs_add_canonical_headers('requests/reviewers-0/list/' . $classification . '/' . $review_step . '/' . $page_number);

  $user = cf_current_user();

  $form = array();

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_classifications = mfcs_get_request_classification_list_options();
  $review_steps = mfcs_get_review_step_list_options();
  $reviewer_classifications = mfcs_get_reviewer_classification_list_options();
  $reviewer_classifications_list = mfcs_get_reviewer_classification_list_options('select');

  $page_title = "Reviewers for Step: " . $review_steps[$review_step];
  drupal_set_title($page_title);

  if (!array_key_exists($classification, $request_classifications)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!array_key_exists($review_step, $review_steps)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }


  // load sorting information
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  $order_column = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  $class_id = '';
  $class_classification = '';
  $class_account_id = 'no-script-hidden ';
  $class_account = '';
  $class_first_name = '';
  $class_last_name = '';
  $class_operations = 'no-print ';

  $sorting = array();
  if (is_null($order)) {
    $sorting['reviewer_classification_name'] = 'ASC';
    $order_column = array(1 => strtolower($sorting['reviewer_classification_name']));
    $class_classification .= 'sorting_1 ';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(0 => strtolower($sorting[$order]));
      $class_id .= 'sorting_1 ';
    }
    elseif ($order == 'reviewer classification') {
      $sorting['reviewer_classification_name'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = array(1 => strtolower($sorting[$order]));
      $class_classification .= 'sorting_1 ';
    }
    elseif ($order == 'account id') {
      $sorting['user_id'] = is_null($sort) ? 'DESC' : $sort;
      $order_column = array(2 => strtolower($sorting[$order]));
      $class_account_id .= 'sorting_1 ';
    }
    elseif ($order == 'account') {
      $sorting['name'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = array(3 => strtolower($sorting[$order]));
      $class_account .= 'sorting_1 ';
    }
    elseif ($order == 'first name') {
      $sorting['first_name'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = array(4 => strtolower($sorting[$order]));
      $class_first_name .= 'sorting_1 ';
    }
    elseif ($order == 'last name') {
      $sorting['last_name'] = is_null($sort) ? 'ASC' : $sort;
      $order_column = array(5 => strtolower($sorting[$order]));
      $class_last_name .= 'sorting_1 ';
    }
    else {
      $sorting['id'] = 'ASC';
      $order_column = array(0 => strtolower($sorting['id']));
      $class_id .= 'sorting_1 ';
    }
  }

  $rows_per_page = 20;
  $options = array(
    'sort' => $sorting,
    'join' => TRUE,
    'start' => $page_number * $rows_per_page,
    'limit' => $rows_per_page,
    'count' => TRUE,
  );

  // do not load listing on first pass (improves performance at cost of no support for no-js.).
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $options['results'] = FALSE;
  }

  $reviewers_returned = mfcs_get_reviewers(array('mr.request_classification' => $classification, 'mr.review_step' => $review_step, 'mr.disabled' => 0), $options);
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }

  // do not provide pages when the page number would bring the user beyond the available items.
  if ($reviewers_returned['count'] > 0 && $options['start'] >= $reviewers_returned['count'] || $reviewers_returned['count'] == 0 && $options['start'] > $reviewers_returned['count']) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // do not show count values when no fallback is available.
  if ($no_js_fallback === FALSE && $auto_page_number) {
    $count = 0;
  }

  // Results is needed and therefore must not be FALSE, unsetting it will default to TRUE.
  unset($options['results']);

  $rows = array();
  foreach ($reviewers as $reviewer) {
    $row = array();

    $row['id'] = array(
      'data' => $reviewer->id,
      'class' => $class_id,
    );

    $row['reviewer_classification'] = array(
      'data' => '',
      'class' => $class_classification,
    );

    if (is_string($reviewer_classification = $reviewer->reviewer_classification)) {
      $reviewer_classification = (int) $reviewer->reviewer_classification;
    }
    else {
      $reviewer_classification = $reviewer->reviewer_classification;
    }

    if (isset($reviewer_classifications[$reviewer_classification])) {
      $row['reviewer_classification']['data'] = $reviewer_classifications[$reviewer_classification];
    }

    $row['account_id'] = array(
      'data' => $reviewer->user_id,
      'class' => $class_account_id,
    );

    $row['account'] = array(
      'data' => '<a href="' . $base_path . 'requests/users-0/view/' . $reviewer->user_id . $url_arguments . '" class="user-link">' . check_plain($reviewer->name) . '</a>' . ' <span class="user-id script-hidden">(id: ' .  $reviewer->user_id . ')</span>',
      'class' => $class_account,
    );

    $row['first_name'] = array(
      'data' => check_plain($reviewer->first_name),
      'class' => $class_first_name,
    );

    $row['last_name'] = array(
      'data' => check_plain($reviewer->last_name),
      'class' => $class_last_name,
    );

    $row['operations'] = array(
      'data' => '<a href="' . $base_path . 'requests/reviewers-0/delete/' . $reviewer->id . $url_arguments . '" class="operation-link operation-link-delete">Delete</a>',
      'class' => $class_operations,
    );

    $rows[] = $row;
  }
  unset($reviewer_classification);

  $form['form']['request_classification'] = array(
    '#type' => 'value',
    '#value' => (int) $classification,
  );

  $form['form']['review_step'] = array(
    '#type' => 'value',
    '#value' => (int) $review_step,
  );

  $reviewers_list = array();
  $reviewers_list['caption'] = t("Reviewers Assigned to %request_classification for %step", array('%request_classification' => $request_classifications[$classification], '%step' => $review_steps[$review_step]));
  $reviewers_list['header'] = array(
    'id' => array('field' => 'id', 'data' => "ID", 'align' => 'center'),
    'reviewer_classification' => array('field' => 'reviewer_classification', 'data' => "Reviewer Classification", 'align' => 'center', 'sort' => 'asc'),
    'account_id' => array('field' => 'account_id', 'data' => "Account ID", 'align' => 'center', 'class' => array('no-script-hidden')),
    'account' => array('field' => 'account', 'data' => "Account", 'align' => 'center'),
    'first_name' => array('field' => 'first_name', 'data' => "First Name", 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => "Last Name", 'align' => 'center'),
    'operations' => array('data' => "Operations", 'align' => 'center', 'class' => array('no-print')),
  );

  $reviewers_list['attributes'] = array();
  $reviewers_list['attributes']['id'] = 'mfcs-reviewers_form-reviewers_list';
  $reviewers_list['attributes']['class'] = array(
    'no_default',
    'mfcs-reviewers_form-reviewers_list',
    'display',
    'hover',
    'order-column',
    'stripe',
    'cell-border',
  );
  $reviewers_list['attributes']['summary'] = 'All users who are considered reviewers for the given request classification.';

  $reviewers_list['rows'] = $rows;
  $reviewers_list['empty'] = t("There are no users assigned to review this request classsification.");

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
      'url' => "'" . $base_path . 'requests/reviewers-0/ajax/list/' . $classification . '/' . $review_step . $url_arguments . "'",
      'type' => "'POST'",
    ),
    'deferRender' => TRUE,
    'columns' => array(
      array('id' => 0, 'column' => 'id', 'name' => "ID", 'className' => 'column-id', 'sanitize' => FALSE),
      array('id' => 1, 'column' => 'review_classification', 'name' => "Review Classification", 'className' => 'column-review_classification'),
      array('id' => 2, 'column' => 'account_id', 'name' => "Account ID", 'className' => 'column-account_id', 'sanitize' => FALSE, 'visible' => FALSE),
      array('id' => 3, 'column' => 'account', 'name' => "Account", 'className' => 'column-account'),
      array('id' => 4, 'column' => 'first_name', 'name' => "First Name", 'className' => 'column-first_name'),
      array('id' => 5, 'column' => 'last_name', 'name' => "Last Name", 'className' => 'column-last_name'),
      array('id' => 6, 'column' => NULL, 'name' => "Operations", 'className' => 'column-operations no-print', 'searchable' => FALSE, 'orderable' => FALSE),
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
      'sEmptyTable' => '"' . $reviewers_list['empty'] . '"',
    ),
    'language' => array(
      'emptyTable' => '"' . $reviewers_list['empty'] . '"',
    ),
    'searchDelay' => 1200,
    'pagingType' => 'input',
  );

  $navigation_top = '';
  $navigation_bottom = '';
  if ($page_number > 0) {
    if ($page_number == 1) {
      $navigation_top .= '<a href="' . $base_path . 'requests/reviewers-0/list/' . $classification . "/" . $review_step . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/reviewers-0/list/' . $classification . "/" . $review_step . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
    else {
      $previous_page = $page_number - 1;
      $navigation_top .= '<a href="' . $base_path . 'requests/reviewers-0/list/' . $classification . "/" . $review_step . '/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
      $navigation_bottom .= '<a href="' . $base_path . 'requests/reviewers-0/list/' . $classification . "/" . $review_step . '/' . $previous_page . $url_arguments . '" class="table_listing-choice table_listing-choice-previous no-print" title="Previous Page">Prev</a>';
    }
  }

  $navigation_start = $options['start'] + 1;
  $navigation_total = $options['start'] + $rows_per_page;

  if ($navigation_total == 0) {
    $navigation_start = 0;
  }

  if ($navigation_total > $reviewers_returned['count']) {
    $navigation_total = $reviewers_returned['count'];
  }

  $navigation_bottom .= '<div class="table_listing-comment">';
  $navigation_bottom .= "Showing " . $navigation_start . " to " . $navigation_total . " of " . $reviewers_returned['count'] . " entries.";
  $navigation_bottom .= '</div>';

  if ($reviewers_returned['count'] > ($options['start'] + $rows_per_page)) {
    $next_page = $page_number + 1;
    $navigation_top .= '<a href="' . $base_path . 'requests/reviewers-0/list/' . $classification . "/" . $review_step . '/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
    $navigation_bottom .= '<a href="' . $base_path . 'requests/reviewers-0/list/' . $classification . "/" . $review_step . '/' . $next_page . $url_arguments . '" class="table_listing-choice table_listing-choice-next no-print" title="Next Page">Next</a>';
  }

  $form['reviewers_list']['#markup'] = '<div class="table_listing-navigator table_listing-navigator-top no-print">' . $navigation_top . '</div>';
  $form['reviewers_list']['#markup'] .= mcneese_libraries_theme_table($reviewers_list, $datatable_settings);
  #$form['reviewers_list']['#markup'] .= '<div class="table_listing-navigator table_listing-navigator-bottom">' . $navigation_bottom . '</div>';

  $form['reviewer'] = array(
    '#id' => 'reviewers-reviewer',
    '#type' => 'fieldset',
    '#title' => 'Add Reviewer',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['reviewer']['user_id'] = array(
    '#id' => 'reviewers-reviewer-user_id',
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => 'User',
    '#size' => 96,
    '#autocomplete_path' => 'requests/reviewers-0/autocomplete/user_id',
    '#required' => TRUE,
  );

  $form['reviewer']['reviewer_classification'] = array(
    '#id' => 'reviewers-reviewer-reviewer_classification',
    '#type' => 'select',
    '#default_value' => '',
    '#options' => $reviewer_classifications_list,
    '#title' => 'Classification',
    '#required' => TRUE,

    #'#type' => 'textfield',
    #'#size' => 96,
    #'#autocomplete_path' => 'requests/reviewers-0/autocomplete/reviewer_classification',
  );

  $form['reviewer']['add'] = array(
    '#id' => 'submit-reviewer-add',
    '#type' => 'submit',
    '#default_value' => t("Add Reviewer"),
  );

  $form['reviewer']['recheck'] = array(
    '#id' => 'submit-reviewer-recheck',
    '#type' => 'submit',
    '#default_value' => t("Recheck Requests"),
  );

  return $form;
}

/**
 * Pre-Validation for mfcs_request_reviewers_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reviewers_0_form()
 */
function mfcs_request_reviewers_0_form_pre_validate(&$form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-reviewer-add') {
  }
  else {
    $form['reviewer']['user_id']['#required'] = FALSE;
    $form['reviewer']['reviewer_classification']['#required'] = FALSE;
  }
}

/**
 * Validation for mfcs_request_reviewers_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reviewers_0_form()
 */
function mfcs_request_reviewers_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'submit-reviewer-add') {
    $user_id = NULL;
    if (cf_is_integer($form_state['values']['reviewer']['user_id'])) {
      $user_id = $form_state['values']['reviewer']['user_id'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['reviewer']['user_id'], $matches);

      if ($matched && !empty($matches[1])) {
        $user_id = $matches[1];
      }
    }

    if (!is_null($user_id)) {
      $user = user_load($user_id);

      if (!is_object($user)) {
        form_set_error('reviewer][user_id', "An invalid User has been specified. Please select a valid User.");
      }
    }

    $reviewer_classification = NULL;
    if (cf_is_integer($form_state['values']['reviewer']['reviewer_classification'])) {
      $reviewer_classification = $form_state['values']['reviewer']['reviewer_classification'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['reviewer']['reviewer_classification'], $matches);

      if ($matched && !empty($matches[1])) {
        $reviewer_classification = $matches[1];
      }
    }

    if (!is_null($reviewer_classification)) {
      try {
        $query = db_select('taxonomy_term_data', 'ttd');
        $query->fields('ttd');

        $query->condition('ttd.vid', 17);
        $query->condition('ttd.tid', $reviewer_classification);

        $query->countQuery();

        $result = $query->execute()->fetchField();
        if ($result == 0) {
          $reviewer_classification = NULL;
          form_set_error('reviewer][reviewer_classification', "An invalid Reviewer Classification has been specified. Please select a valid Reviewer Classification.");
        }
      }
      catch (Error $e) {
        cf_error::on_query_execution($e);
      }
      catch (Exception $e) {
        cf_error::on_query_execution($e);
      }
    }

    if (!is_null($user_id) && !is_null($reviewer_classification)) {
      $conditions = array(
        'mr.request_classification' => $form['form']['request_classification']['#value'],
        'mr.review_step' => $form['form']['review_step']['#value'],
        'mr.reviewer_classification' => $reviewer_classification,
        'mr.user_id' => $user_id,
        'mr.disabled' => 0,
      );

      $reviewers_returned = mfcs_get_reviewers($conditions);
      $reviewers = array();
      if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
        $reviewers = $reviewers_returned['results'];
      }
      unset($reviewers_returned);

      if (count($reviewers) > 0) {
        form_set_error('reviewer][user_id', "The specified user already exists with the specified reviewer classification.");
      }
    }
  }
  elseif ($clicked_id == 'submit-reviewer-recheck') {
  }
  else {
    form_set_error('', "Unknown submit process detected.");
  }
}

/**
 * Submit process for mfcs_request_reviewers_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reviewers_0_form()
 */
function mfcs_request_reviewers_0_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $failure = FALSE;
  if ($clicked_id == 'submit-reviewer-add') {
    $values = array();
    $values['request_classification'] = $form['form']['request_classification']['#value'];
    $values['review_step'] = $form['form']['review_step']['#value'];

    if (cf_is_integer($form_state['values']['reviewer']['user_id'])) {
      $values['user_id'] = $form_state['values']['reviewer']['user_id'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['reviewer']['user_id'], $matches);

      if ($matched && !empty($matches[1])) {
        $values['user_id'] = $matches[1];
      }
    }

    if (cf_is_integer($form_state['values']['reviewer']['reviewer_classification'])) {
      $values['reviewer_classification'] = $form_state['values']['reviewer']['reviewer_classification'];
    }
    else {
      $matches = array();
      $matched = preg_match('@\[id:\s*(\d+)\].*@', $form_state['values']['reviewer']['reviewer_classification'], $matches);

      if ($matched && !empty($matches[1])) {
        $values['reviewer_classification'] = $matches[1];
      }
    }

    $transaction = db_transaction();
    try {
      $query = db_select('mfcs_reviewers', 'mr');
      $query->addField('mr', 'id', 'id');
      $query->condition('mr.user_id', $values['user_id']);
      $query->condition('mr.request_classification', $values['request_classification']);
      $query->condition('mr.reviewer_classification', $values['reviewer_classification']);
      $query->condition('mr.review_step', $values['review_step']);
      $query->condition('mr.disabled', 1);

      $result = $query->execute()->fetchField();

      if ($result > 0) {
        $query = db_update('mfcs_reviewers');
        $query->fields(array('disabled' => 0));
        $query->condition('id', $result);
        $query->execute();
      }
      else {
        $query = db_insert('mfcs_reviewers');
        $query->fields($values);
        $query->execute();
      }
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      $failure = TRUE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      $failure = TRUE;
    }
  }
  elseif ($clicked_id == 'submit-reviewer-recheck') {
    mfcs_include(MFCS_INCLUDE_WORKFLOW);

    $classification = $form['form']['request_classification']['#value'];
    $step = $form['form']['review_step']['#value'];

    if (is_string($classification)) {
      $classification = (int) $classification;
    }

    if (is_string($step)) {
      $step = (int) $step;
    }

    $transaction = db_transaction();
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');

      $query->addField('mer', 'id', 'id');
      $query->addField('mc', 'review_review', 'revision');

      $query->condition('mer.status', MFCS_REQUEST_STATUS_LOCKED);
      $query->condition('mer.step', $step);
      $query->condition('mer.classification', $classification);

      $request_ids = $query->execute()->fetchAllKeyed();

      if (!is_array($request_ids) || empty($request_ids)) {
        drupal_set_message("Recheck process complete.");
        return TRUE;
      }
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      $failure = TRUE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);

      $failure = TRUE;
    }

    $decisions = FALSE;
    if (!$failure) {
      $decisions = mfcs_get_requests_current_review_decisions($request_ids, $classification, $step);
    }

    if ($decisions === FALSE) {
      form_set_error('form', "An error occurred while trying to recheck the requests. Please contact the support staff.");
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to recheck the requests.", array(), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;
      return FALSE;
    }

    if (is_array($decisions)) {
      foreach ($decisions as $request_id => $decision) {
        if ($decision['current'] < $decision['total']) {
          continue;
        }

        $message = "This request's workflow has changed as a result of a change in the reviewer management settings.";

        mfcs_workflow_next_step($request_id, $classification, $step, MFCS_REVIEW_DECISION_MANAGER_RECHECK, MFCS_REVIEW_RESTRICTIONS_NONE, $message, TRUE);
      }
    }

    drupal_set_message("Recheck process complete.");
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($form['form']['request_classification']['#value']) || empty($form['form']['review_step']['#value'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/reviewers-0');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/reviewers-0/list/' . $form['form']['request_classification']['#value'] . '/' . $form['form']['review_step']['#value']);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the facilities use request reviewer delete page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $id
 *   Number representing the reviewer id.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_reviewers_0_delete_form($form, &$form_state, $id) {
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

  if (!cf_is_integer($id) || $id < 0) {
    cf_error::invalid_integer('id');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $reviewers_returned = mfcs_get_reviewers(array('mr.id' => $id));
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  $reviewer = NULL;
  if (!empty($reviewers)) {
    $reviewer = array_pop($reviewers);
  }

  if (!is_object($reviewer)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  mfcs_add_canonical_headers('requests/reviewers-0/delete/' . $id);

  $user = user_load($reviewer->user_id);

  $user_name = '';
  if (is_object($user)) {
    $user_name = $user->name;
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_classifications = mfcs_get_request_classification_list_options();
  $request_classification = '';
  if (is_string($reviewer->request_classification)) {
    $reviewer_request_classification = (int) $reviewer->request_classification;
  }
  else {
    $reviewer_request_classification = $reviewer->request_classification;
  }

  if (is_string($reviewer->review_step)) {
    $reviewer_step = (int) $reviewer->review_step;
  }
  else {
    $reviewer_step = $reviewer->review_step;
  }

  $request_classification_name = "";
  if (isset($request_classifications[$reviewer_request_classification])) {
    $request_classification_name = $request_classifications[$reviewer_request_classification];
  }

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  $form['form']['request_classification'] = array(
    '#type' => 'value',
    '#value' => $reviewer_request_classification,
  );

  $form['form']['review_step'] = array(
    '#type' => 'value',
    '#value' => $reviewer_step,
  );


  if (empty($form_state['values']['redirect_to'])) {
    if (empty($reviewer_request_classification)) {
      $destination = 'requests/reviewers-0' . $url_arguments;
    }
    else {
      $destination = 'requests/reviewers-0/list/' . $reviewer_request_classification . $url_arguments;
    }
  }
  else {
    $destination = $form_state['values']['redirect_to'];
  }

  return confirm_form($form,
    t("Are you sure you want to delete the account %user (%id) from being a %request_classification_name reviewer?", array('%user' => $user_name, '%id' => $reviewer->user_id, '%request_classification_name' => $request_classification_name)),
    $destination,
    t("This action cannot be undone."),
    t("Delete"),
    t("Cancel")
  );
}

/**
 * Validation for mfcs_request_reviewers_0_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reviewers_0_delete_form()
 */
function mfcs_request_reviewers_0_delete_form_validate($form, &$form_state) {
  $reviewers_returned = mfcs_get_reviewers(array('mr.id' => $form['form']['id']['#value'], 'mr.disabled' => 0));
  $reviewers = array();
  if (isset($reviewers_returned['results']) && is_array($reviewers_returned['results'])) {
    $reviewers = $reviewers_returned['results'];
  }
  unset($reviewers_returned);

  $reviewer = NULL;
  if (!empty($reviewers)) {
    $reviewer = array_pop($reviewers);
  }

  if (!is_object($reviewer)) {
    form_set_error('', "An invalid reviewer id has been specified.");
  }
}

/**
 * Submit process for mfcs_request_reviewers_0_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_reviewers_0_delete_form()
 */
function mfcs_request_reviewers_0_delete_form_submit($form, &$form_state) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_reviewers');
    $query->fields(array('disabled' => 1));
    $query->condition('id', $form['form']['id']['#value']);
    $query->execute();
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    form_set_error('form', "An error occurred while trying to delete the reviewer. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to delete the reviewer '%reviewer_id'.", array('%reviewer_id' => $form['form']['id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    form_set_error('form', "An error occurred while trying to delete the reviewer. Please contact the support staff.");
    watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to delete the reviewer '%reviewer_id'.", array('%reviewer_id' => $form['form']['id']['#value']), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($form['form']['request_classification']['#value']) || empty($form['form']['review_step']['#value'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/reviewers-0');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/reviewers-0/list/' . $form['form']['request_classification']['#value'] . '/' . $form['form']['review_step']['#value']);
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
