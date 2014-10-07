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
  $user = cf_current_user();

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $request_classifications = mfcs_get_request_classification_list_options();
  $review_steps = mfcs_get_review_step_list_options('object');

  $markup = '<div id="mfcs-manage_reviewers">';
  $groups = array();
  foreach ($request_classifications as $classification_id => $classification_label) {
    if (!is_numeric($classification_id)) continue;

    $items = array();
    foreach ($review_steps as $review_step_id => $review_step_object) {
      if (empty($review_step_object->field_review_event_roles['und'])) {
        continue;
      }

      // these are automatically the venue coordinator, therefore skip them.
      if ($review_step_id == MFCS_REVIEW_STEP_VENUE_AVAILABLE) continue;
      if ($review_step_id == MFCS_REVIEW_STEP_MAKE_DECISIONS) continue;

      $unmatched = TRUE;
      foreach ($review_step_object->field_review_event_roles['und'] as $key => $value) {
        if ($value['tid'] == $classification_id) {
          $unmatched = FALSE;
          break;
        }
      }

      if ($unmatched) {
        continue;
      }

      $link = '<a id="mfcs-manage_reviewers-manage-' . $classification_id . '-link-' . $review_step_id .'" href="' . $base_path . 'requests/reviewers-0/list/' . $classification_id . '/' . $review_step_id . '" class="item-link mfcs-manage_reviewers-manage-link">' . $review_step_object->name . ' Step</a>';
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
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_reviewers_0_form($form, &$form_state, $classification, $review_step) {
  if (!cf_is_integer($classification)) {
    cf_error::invalid_integer('classification');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($review_step)) {
    cf_error::invalid_integer('review_step');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // these are automatically the venue coordinator, therefore skip them.
  if ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE || $review_step == MFCS_REVIEW_STEP_MAKE_DECISIONS) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/reviewers-0/list/' . $classification . '/' . $review_step);

  global $base_path;
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
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  $sorting = array();
  if (is_null($order)) {
    $sorting['reviewer_classification_name'] = 'ASC';
  }
  else {
    if ($order == 'id') {
      $sorting['id'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'reviewer classification') {
      $sorting['reviewer_classification_name'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'account') {
      $sorting['account'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'first name') {
      $sorting['first_name'] = is_null($sort) ? 'DESC' : $sort;
    }
    elseif ($order == 'last name') {
      $sorting['last_name'] = is_null($sort) ? 'DESC' : $sort;
    }
  }

  $reviewers = mfcs_get_reviewers(array('mr.request_classification' => $classification, 'mr.review_step' => $review_step, 'mr.disabled' => 0), TRUE, $sorting);

  if (!is_array($reviewers)) {
    $reviewers = array();
  }

  $rows = array();
  foreach ($reviewers as $reviewer) {
    $row = array();

    $id = $reviewer->id;
    $row['user_id'] = $id;
    $row['reviewer_classification'] = '';

    if (isset($reviewer_classifications[$reviewer->reviewer_classification])) {
      $row['reviewer_classification'] = $reviewer_classifications[$reviewer->reviewer_classification];
    }

    $user_id = $reviewer->user_id;
    $row['name'] = '<a href="' . $base_path . 'user/' . $user_id . '" class="user-link">' . check_plain($reviewer->name) . '</a>';
    $row['first_name'] = check_plain($reviewer->first_name);
    $row['last_name'] = check_plain($reviewer->last_name);

    $row['operations'] = '<a href="' . $base_path . 'requests/reviewers-0/delete/' . $id . '" class="operation-link operation-link-delete">Delete</a>';

    $rows[] = $row;
  }

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
    'account' => array('field' => 'account', 'data' => "Account", 'align' => 'center'),
    'first_name' => array('field' => 'first_name', 'data' => "First Name", 'align' => 'center'),
    'last_name' => array('field' => 'last_name', 'data' => "Last Name", 'align' => 'center'),
    t("Operations"),
  );

  $reviewers_list['attributes'] = array();
  $reviewers_list['attributes']['id'] = 'mfcs-reviewers_form-reviewers_list';
  $reviewers_list['attributes']['class'] = array ('mfcs-reviewers_form-reviewers_list');
  $reviewers_list['attributes']['summary'] = 'All users who are considered reviewers for the given request classification.';

  $reviewers_list['rows'] = $rows;
  $reviewers_list['empty'] = t("There are no users assigned to review this request classsification.");

  $form['reviewers_list']['#markup'] = theme('table', $reviewers_list);


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
      catch (Exception $ex) {
        cf_error::on_query_execution($ex);
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

      $reviewers = mfcs_get_reviewers($conditions);

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
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

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

    $failure = FALSE;
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
    catch (Exception $ex) {
      $transaction->rollback();
      cf_error::on_query_execution($ex);

      $failure = TRUE;
    }
  }
  elseif ($clicked_id == 'submit-reviewer-recheck') {
    mfcs_include(MFCS_INCLUDE_WORKFLOW);

    $classification = $form['form']['request_classification']['#value'];
    $step = $form['form']['review_step']['#value'];

    try {
      $query = db_select('mfcs_requests', 'mer');
      $query->innerJoin('mfcs_current_review', 'mcr', 'mer.id = mcr.request_id');

      $query->innerJoin('mfcs_current_request_coordinator', 'mcrc', 'mer.id = mcrc.request_id');
      $query->innerJoin('mfcs_field_request_coordinator_classification', 'mfrcc', 'mcrc.request_id = mfrcc.request_id AND mcrc.classification = mfrcc.revision AND mfrcc.delta = 0');

      $query->addField('mer', 'id', 'id');
      $query->addField('mcr', 'review', 'revision');

      $query->condition('mer.status', MFCS_REQUEST_STATUS_LOCKED);
      $query->condition('mer.step', $step);
      $query->condition('mfrcc.value', $classification);

      $request_ids = $query->execute()->fetchAllKeyed();

      if (!is_array($request_ids) || empty($request_ids)) {
        drupal_set_message("Recheck process complete.");
        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_query_execution($ex);

      return FALSE;
    }

    $checked = mfcs_check_if_requests_are_fully_reviewed($request_ids, $classification, $step);

    if ($checked === FALSE) {
      form_set_error('form', "An error occurred while trying to submit the recheck the requests. Please contact the support staff.");
      watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to submit the recheck the requests.", array(), WATCHDOG_ERROR);

      $form_state['rebuild'] = TRUE;
      $form_state['redirect'] = FALSE;
      $form_state['submitted'] = FALSE;
      return FALSE;
    }

    if (is_array($checked)) {
      foreach ($checked as $request_id => $check) {
        if ($check === FALSE) {
          continue;
        }

        $message = "This request's workflow has changed as a result of a change in the reviewer management settings.";

        mfcs_workflow_next_step($request_id, $classification, $step, MFCS_REVIEW_DECISION_MANAGER_RECHECK, $message, TRUE);
      }
    }

    drupal_set_message("Recheck process complete.");
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($form['form']['request_classification']['#value']) || empty($form['form']['review_step']['#value'])) {
      $form_state['redirect'] = 'requests/reviewers-0';
    }
    else {
      $form_state['redirect'] = 'requests/reviewers-0/list/' . $form['form']['request_classification']['#value'] . '/' . $form['form']['review_step']['#value'];
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
  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $reviewers = mfcs_get_reviewers(array('mr.id' => $id));
  $reviewer = NULL;
  if (!empty($reviewers)) {
    $reviewer = array_pop($reviewers);
  }

  if (!is_object($reviewer)) {
    drupal_not_found();
    drupal_exit();
    return array();
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
  if (isset($request_classifications[$reviewer->request_classification])) {
    $request_classification = $request_classifications[$reviewer->request_classification];
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
    '#value' => (int) $reviewer->request_classification,
  );

  $form['form']['review_step'] = array(
    '#type' => 'value',
    '#value' => (int) $reviewer->review_step,
  );


  if (empty($form_state['values']['redirect_to'])) {
    if (empty($reviewer->request_classification)) {
      $destination = 'requests/reviewers-0';
    }
    else {
      $destination = 'requests/reviewers-0/list/' . $reviewer->request_classification;
    }
  }
  else {
    $destination = $form_state['values']['redirect_to'];
  }

  return confirm_form($form,
    t("Are you sure you want to delete the account %user (%id) from being a %request_classification reviewer?", array('%user' => $user_name, '%id' => $id, '%request_classification' => $request_classification)),
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
  $reviewers = mfcs_get_reviewers(array('mr.id' => $form['form']['id']['#value']));
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

  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_reviewers');
    $query->fields(array('disabled' => 1));
    $query->condition('id', $form['form']['id']['#value']);
    $query->execute();
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_query_execution($ex);
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    if (empty($form['form']['request_classification']['#value']) || empty($form['form']['review_step']['#value'])) {
      $form_state['redirect'] = 'requests/reviewers-0';
    }
    else {
      $form_state['redirect'] = 'requests/reviewers-0/list/' . $form['form']['request_classification']['#value'] . '/' . $form['form']['review_step']['#value'];
    }
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
