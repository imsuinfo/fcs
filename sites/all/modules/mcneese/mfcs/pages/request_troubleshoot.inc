<?php

/**
 * @file
 * Defines McNeese facilities use request troubleshoot page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request troubleshoot page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   (optional) The unique identifier for a request.
 * @param bool $contained
 *   (optional) When TRUE, the page is self-contained, asuch as with a PDF.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_troubleshoot_0_form($form, &$form_state, $request_id = NULL) {
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

  if (is_null($request_id)) {
    mfcs_add_canonical_headers('requests/troubleshoot');
  }
  else {
    $request = mfcs_load_request_by_id($request_id);

    if (empty($request)) {
      drupal_not_found();
      drupal_exit();
      return array();
    }

    mfcs_add_canonical_headers('requests/troubleshoot-0/' . $request_id);
  }

  $page_title = "Facilities Use Troubleshooting Tools";
  drupal_set_title($page_title);

  $module_path = drupal_get_path('module', 'mfcs');

  $form = array();

  $form['form'] = array();

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );


  // Requests Review Recheck
  $code = 'code_' . MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK;
  $form['form'][$code] = array(
    '#id' => 'field-troubleshoot-' . $code,
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
        'troubleshoot-code',
        'troubleshoot-' . $code,
      ),
    ),
  );

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK . ': Requests Review Recheck</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'When the reviewer assignments are changed, it is possible to forget to click the "Recheck Requests" button.' . "<br>";
  $markup .= "<br>";
  $markup .= 'Using this troubleshoot will force a re-check for all requests currently under review.' . "<br>";
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => t("Re-check all Requests Reviews"),
    '#attributes' => array(
      'class' => array(
        'no-print',
      ),
    ),
  );

  $markup = '<hr class="code-separator">';
  $form['form'][$code]['post_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );


  // MER Cache Synchronize
  $code = 'code_' . MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE;
  $form['form'][$code] = array(
    '#id' => 'field-troubleshoot-' . $code,
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
        'troubleshoot-code',
        'troubleshoot-' . $code,
      ),
    ),
  );

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE . ': Requests Cache Synchronize</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'The requests cache exist to significantly increase the performance of the system when there are a large number of requests on the system. This requests cache is only loosely connected to the actual data, and therefore, the possibility exists that (due to bugs) this cache can become out of sync.' . "<br>";
  $markup .= "<br>";
  $markup .= 'Using this troubleshoot will force a re-synchronization of all cached request data.' . "<br>";
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => t("Re-synchronize Requests Cache"),
    '#attributes' => array(
      'class' => array(
        'no-print',
      ),
    ),
  );

  $markup = '<hr class="code-separator">';
  $form['form'][$code]['post_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );


  // Banner Cache Synchronize
  $code = 'code_' . MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE;
  $form['form'][$code] = array(
    '#id' => 'field-troubleshoot-' . $code,
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
        'troubleshoot-code',
        'troubleshoot-' . $code,
      ),
    ),
  );

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE . ': Banner Cache Synchronize</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'The banner data is cached locally to allow for this system to operate even when banner is unavailable. This cache is usually auto-re-synchronized once a day.' . "<br>";
  $markup .= "<br>";
  $markup .= 'Using this troubleshoot will force a re-synchronization of all banner cache data (including banner role cache), if the system can connect to the banner database.' . "<br>";
  $markup .= '<strong class="code-warning warning">This may take up to 15 minutes.</strong>' . "<br>";
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => t("Re-synchronize Banner Cache"),
    '#attributes' => array(
      'class' => array(
        'no-print',
      ),
    ),
  );

  $markup = '<hr class="code-separator">';
  $form['form'][$code]['post_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );


  // Banner Role Cache Synchronize
  $code = 'code_' . MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE;
  $form['form'][$code] = array(
    '#id' => 'field-troubleshoot-' . $code,
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
        'troubleshoot-code',
        'troubleshoot-' . $code,
      ),
    ),
  );

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE . ': Banner Role Cache Synchronize</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'The banner role is cached locally in a separate manner than the other banner data. This cache is usually auto-re-synchronized once a day.' . "<br>";
  $markup .= "<br>";
  $markup .= 'Using this troubleshoot will force a re-synchronization of <strong>only</strong> the banner role cache data, if the system can connect to the banner database.' . "<br>";
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => t("Re-synchronize Banner Role Cache"),
    '#attributes' => array(
      'class' => array(
        'no-print',
      ),
    ),
  );

  $markup = '<hr class="code-separator">';
  $form['form'][$code]['post_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );


  // Banner Information
  $code = 'code_' . MFCS_TROUBLESHOOT_BANNER_INFORMATION;
  $form['form'][$code] = array(
    '#id' => 'field-troubleshoot-' . $code,
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
        'troubleshoot-code',
        'troubleshoot-' . $code,
      ),
    ),
  );

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_BANNER_INFORMATION . ': Banner Information</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'The following links provide locally stored information regarding banner data.' . "<br>";
  $markup .= "<br>";
  $markup .= 'Using this troubleshoot will force a re-synchronization of <strong>only</strong> the banner role cache data, if the system can connect to the banner database.' . "<br>";
  $markup .= '</div>';
  $markup .= '<div class="code-links">';
  $markup .= '  <ul class="code-links-list">';
  $markup .= '    <li class="code-links-list-item"><a href="' . $base_path . 'requests/troubleshoot-0/locations' . $url_arguments . '" class="code-links-list-item-link">View Banner Location Information.</a></li>';
  $markup .= '    <li class="code-links-list-item"><a href="' . $base_path . 'requests/troubleshoot-0/buildings' . $url_arguments . '" class="code-links-list-item-link">View Banner Building Information.</a></li>';
  $markup .= '    <li class="code-links-list-item"><a href="' . $base_path . 'requests/troubleshoot-0/rooms' . $url_arguments . '" class="code-links-list-item-link">View Banner Room Information.</a></li>';
  $markup .= '  </ul>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $markup = '<hr class="code-separator">';
  $form['form'][$code]['post_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );


  // Requests Review Recheck
  $code = 'code_' . MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK;
  $form['form'][$code] = array(
    '#id' => 'field-troubleshoot-' . $code,
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
        'troubleshoot-code',
        'troubleshoot-' . $code,
      ),
    ),
  );

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK . ': Requests Problem Check</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'Requests are periodically checked for problems, such as date conflicts or deleted rooms.' . "<br>";
  $markup .= "<br>";
  $markup .= 'Using this troubleshoot will perform integrity checks on requests.' . "<br>";
  $markup .= 'Problems are displaed under the <a href="' . $base_path . 'requests/problems-0/requests">Manage Request Problems</a> page.' . "<br>";
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => t("Check Requests for Problems"),
    '#attributes' => array(
      'class' => array(
        'no-print',
      ),
    ),
  );

  $markup = '<hr class="code-separator">';
  $form['form'][$code]['post_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  return $form;
}


/**
 * Validation for mfcs_request_troubleshoot_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_troubleshoot_0_form()
 */
function mfcs_request_troubleshoot_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK . '-submit') {
    // no validation necessary
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE . '-submit') {
    // no validation necessary
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE . '-submit') {
    // no validation necessary
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE . '-submit') {
    // no validation necessary
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK . '-submit') {
    // no validation necessary
  }
  else {
    form_set_error('', "Unknown submit process detected.");
  }
}

/**
 * Submit process for mfcs_request_troubleshoot_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_troubleshoot_0_form()
 */
function mfcs_request_troubleshoot_0_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $code = NULL;
  if ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK . '-submit') {
    $code = MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK;
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE . '-submit') {
    $code = MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE;
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE . '-submit') {
    $code = MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE;
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE . '-submit') {
    $code = MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE;
  }
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK . '-submit') {
    $code = MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK;
  }
  else {
    // do nothing for invalid submits.
    return;
  }

  $instance = mfcs_instance();

  $user = cf_current_user();
  $fields = array(
    'code' => $code,
    'user_id' => $user->uid,
    'date' => $instance,
  );

  $transaction = db_transaction();
  try {
    // log the action first so that if the log fails, then the operation should not be performed.
    $query = db_insert('mfcs_log_troubleshoot');
    $query->fields($fields);
    $query->execute();

    $result = FALSE;
    $time_start = microtime(TRUE);
    $label = "Unknown";

    if ($code == MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK) {
      $label = "Requests review re-check";
      $result = mfcs_request_troubleshoot_0_request_review_recheck($transaction, $user);
    }
    elseif ($code == MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE) {
      $label = "Re-synchronize requests cache";
      $result = mfcs_request_troubleshoot_0_mer_cache_synchronize($transaction, $user);
    }
    elseif ($code == MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE) {
      $label = "Re-synchronize banner cache";
      $result = mfcs_sync_banner_to_local($transaction, $user, TRUE);
    }
    elseif ($code == MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE) {
      $label = "Re-synchronize banner role cache";
      $result = mfcs_sync_banner_to_local_users(NULL, NULL, TRUE);
    }
    elseif ($code == MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK) {
      mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

      $label = "Check requests for problems";
      $result = mfcs_mischellaneous_check_requests_for_problems();
    }


    $time_stop = microtime(TRUE);
    $time_elapsed = $time_stop - $time_start;

    $time_minutes = floor($time_elapsed / 60);
    $time_seconds = ceil($time_elapsed - $time_minutes);

    if ($time_minutes < 0) {
      $time_minutes = 0;
    }

    if ($time_seconds < 0) {
      $time_seconds = 0;
    }

    if ($result) {
      drupal_set_message(t($label . " completed in @minutes minutes and @seconds seconds.", array('@minutes' => $time_minutes, '@seconds' => $time_seconds)));
    }
    else {
      drupal_set_message(t($label . " completed with errors in @minutes minutes and @seconds seconds.", array('@minutes' => $time_minutes, '@seconds' => $time_seconds)), 'warning');
      watchdog(MFCS_WATCHDOG_ID, $label . ", started by @user (@user_id) completed with errors in @minutes minutes and @seconds seconds.", array('@minutes' => $time_minutes, '@seconds' => $time_seconds, '@user' => $user->name, '@user_id' => $user->uid), WATCHDOG_WARNING);
    }
  } catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_exception($ex);
  }
}

/**
 * Process the request review recheck.
 *
 * @param object $transaction
 *   The transaction object.
 * @param object|null $user
 *   (optional) The user object or NULL for the current user.
 */
function mfcs_request_troubleshoot_0_request_review_recheck($transaction, $user = NULL) {
  if (!is_object($transaction)) {
    cf_error::invalid_object('transaction');
    return FALSE;
  }

  if (is_null($user)) {
    $user = cd_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_WORKFLOW);

  try {
    foreach (array(MFCS_REVIEW_STEP_REVIEW, MFCS_REVIEW_STEP_REQUIREMENTS) as $step) {
      foreach (array(MFCS_REQUEST_CLASSIFICATION_STUDENT, MFCS_REQUEST_CLASSIFICATION_CAMPS, MFCS_REQUEST_CLASSIFICATION_FACULTY, MFCS_REQUEST_CLASSIFICATION_EXTERNAL) as $classification) {
        $query = db_select('mfcs_requests', 'mer');

        $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');

        $query->addField('mer', 'id', 'id');
        $query->addField('mer', 'step', 'step');
        $query->addField('mer', 'classification', 'classification');
        $query->addField('mc', 'review_review', 'revision');

        $query->condition('mer.status', MFCS_REQUEST_STATUS_LOCKED);
        $query->condition('mer.step', $step);
        $query->condition('mer.classification', $classification);

        $request_ids = $query->execute()->fetchAllKeyed();

        if (!is_array($request_ids) || empty($request_ids)) {
          continue;
        }

        $decisions = mfcs_get_requests_current_review_decisions($request_ids, $classification, $step);

        if ($decisions === FALSE) {
          $transaction->rollback();

          form_set_error('form', "An error occurred while trying to recheck the requests. Please contact the support staff.");
          watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to recheck the requests.", array(), WATCHDOG_ERROR);

          return FALSE;
        }

        if (is_array($decisions)) {
          foreach ($decisions as $request_id => $decision) {
            if ($decision['current'] < $decision['total']) {
              continue;
            }

            $message = "This request's workflow has changed as a result of a change in the reviewer management settings.";

            mfcs_workflow_next_step($request_id, $classification, $step, MFCS_REVIEW_DECISION_MANAGER_RECHECK, $message, TRUE);
          }
        }
      }
    }
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_exception($ex);

    return FALSE;
  }

  return TRUE;
}

/**
 * Synchronize the mer cache.
 *
 * @param object $transaction
 *   The transaction object.
 * @param object|null $user
 *   (optional) The user object or NULL for the current user.
 */
function mfcs_request_troubleshoot_0_mer_cache_synchronize($transaction, $user = NULL) {
  if (!is_object($transaction)) {
    cf_error::invalid_object('transaction');
    return FALSE;
  }

  if (is_null($user)) {
    $user = cd_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  $instance = mfcs_instance();

  try {
    $requests = array();

    // updated
    // @todo: append current request revision number to the mfcs_current table and use that value.


    // status
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_status', 'mfts', 'mc.request_id = mfts.request_id AND mc.top_status = mfts.revision AND mfts.delta = 0');
    $query->where('NOT mer.status = mfts.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mfts', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['status'] = $result->value;
    }


    // step
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_step', 'mftsp', 'mc.request_id = mftsp.request_id AND mc.top_step = mftsp.revision AND mftsp.delta = 0');
    $query->where('NOT mer.step = mftsp.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mftsp', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['step'] = $result->value;
    }


    // type
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_information_type', 'mfit', 'mc.request_id = mfit.request_id AND mc.information_type = mfit.revision AND mfit.delta = 0');
    $query->where('NOT mer.type = mfit.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mfit', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['type'] = $result->value;
    }


    // classification
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_request_coordinator_classification', 'mfrcc', 'mc.request_id = mfrcc.request_id AND mc.request_coordinator_classification = mfrcc.revision AND mfrcc.delta = 0');
    $query->where('NOT mer.classification = mfrcc.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mfrcc', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['classification'] = $result->value;
    }


    // location
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_location', 'mftl', 'mc.request_id = mftl.request_id AND mc.top_location = mftl.revision AND mftl.delta = 0');
    $query->where('NOT mer.location = mftl.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mftl', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['location'] = $result->value;
    }


    // building
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_building', 'mftb', 'mc.request_id = mftb.request_id AND mc.top_building = mftb.revision AND mftb.delta = 0');
    $query->where('NOT mer.building = mftb.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mftb', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['building'] = $result->value;
    }


    // room
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_top_room', 'mftr', 'mc.request_id = mftr.request_id AND mc.top_room = mftr.revision AND mftr.delta = 0');
    $query->where('NOT mer.room = mftr.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mftr', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['room'] = $result->value;
    }


    // venue_coordinator
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_venue_coordinator_user_id', 'mfvcui', 'mc.request_id = mfvcui.request_id AND mc.venue_coordinator_user_id = mfvcui.revision AND mfvcui.delta = 0');
    $query->where('NOT mer.venue_coordinator = mfvcui.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mfvcui', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['venue_coordinator'] = $result->value;
    }


    // title
    $query = db_select('mfcs_requests', 'mer');
    $query->innerJoin('mfcs_current', 'mc', 'mer.id = mc.request_id');
    $query->innerJoin('mfcs_field_information_title', 'mfit', 'mc.request_id = mfit.request_id AND mc.information_title = mfit.revision AND mfit.delta = 0');
    $query->where('NOT mer.title = mfit.value');
    $query->addField('mer', 'id', 'request_id');
    $query->addField('mfit', 'value', 'value');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      if (!array_key_exists($result->request_id, $requests)) {
        $requests[$result->request_id] = array();
      }

      $requests[$result->request_id]['title'] = $result->value;
    }


    // fix all out of sync requests.
    drupal_set_message(t("A total of @total requests have been detected to be out of sync.", array('@total' => count($requests))));
    watchdog(MFCS_WATCHDOG_ID, "The following requests have been detected to be out of sync during the re-sync process: @requests.", array('@requests' => print_r(array_keys($requests), TRUE)), WATCHDOG_NOTICE);
    foreach ($requests as $request_id => $request) {
      drupal_set_message(t("Request @request_id is out of sync and is being corrected.", array('@request_id' => $request_id)));

      // make sure the updated date is the same as the new revision date.
      $request['updated'] = $instance;

      $query = db_update('mfcs_requests');
      $query->fields($request);
      $query->condition('id', $request_id);
      $query->execute();

      // The request is being changed so create a new revision.
      $fields_revisions = array(
        'request_id' => $request_id,
        'user_id' => $user->uid,
        'date' => $instance,
        'message' => "Request cache has been detected to be out of sync and is now corrected.",
      );

      $request_revision = mfcs_load_request_revision_number($request_id);
      $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

      if ($request_revision === FALSE) {
        $transaction->rollback();
        watchdog(MFCS_WATCHDOG_ID, "Failed to load revision number for request %request_id.", array('%request_id' => $request_id), WATCHDOG_ERROR);
        return FALSE;
      }
      elseif (is_null($request_revision)) {
        $request_revision_next = 0;
      }
      else {
        $request_revision_next = $request_revision + 1;
      }

      $fields_revisions['revision'] = $request_revision_next;

      $signature = mfcs_gpg_sign($fields_revisions['message']);
      if ($signature !== FALSE) {
        $fields_revisions['signature'] = $signature;
      }

      if (!$processed_mfcs_request_revisions) {
        $query = db_insert('mfcs_request_revisions');
        $query->fields($fields_revisions);
        $query->execute();

        $processed_mfcs_request_revisions = TRUE;
      }
    }
  }
  catch (Exception $ex) {
    $transaction->rollback();
    cf_error::on_exception($ex);

    return FALSE;
  }

  return TRUE;
}

/**
 * Display a list of locations and their relevant data.
 *
 * This will include a list of disabled locations as well as enabled.
 */
function mfcs_request_troubleshoot_0_locations_page() {
  $markup = '<div id="mfcs-troubleshoot_0-locations-page" class="information_page">';
  $markup .= '<h2 class="information_page-content_header element-invisible">Main Content</h2>';

  try {
    $query = db_select('mfcs_banner_locations', 'mbl');
    $query->innerJoin('field_data_field_column_venue_category', 'fdfcvc', 'mbl.machine_name = fdfcvc.field_column_venue_category_value');
    $query->innerJoin('taxonomy_term_data', 'ttd', 'fdfcvc.entity_id = ttd.tid');

    $query->addField('mbl', 'id', 'location_id');
    $query->addField('mbl', 'machine_name', 'machine_name');
    $query->addField('mbl', 'human_name', 'human_name');
    $query->addField('ttd', 'tid', 'term_id');
    $query->addField('mbl', 'date', 'date');
    $query->addField('mbl', 'disabled', 'disabled');

    $query->condition('ttd.vid', MFCS_VID_REQUEST_LOCATION);

    $query->orderBy('mbl.disabled');
    $query->orderBy('mbl.machine_name');

    $entries = $query->execute()->fetchAllAssoc('location_id');

    $is_even = TRUE;
    $markup_enabled = '';
    $markup_disabled = '';
    foreach ($entries as $entry) {
      $target_markup = &$markup_enabled;
      if ($entry->disabled == 1) {
        $target_markup = &$markup_disabled;
      }

      $is_even = !$is_even;
      if ($is_even) {
        $even_odd_css = 'even';
      }
      else {
        $even_odd_css = 'odd';
      }

      $target_markup .= '<div id="mfcs-troubleshoot_0-locations-page-entry-' . $entry->location_id . '" class="information_page-entry ' . $even_odd_css . '">';
      $target_markup .= '  <h4 class="information_page-entry-title">' . $entry->human_name . '</h4>';
      $target_markup .= '  <ul class="information_page-entry-list">';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-machine_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-human_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->human_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-term_id even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Term ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->term_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-date odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-disabled even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->disabled == 0 ? "No" : "Yes") . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '  </ul>';
      $target_markup .= '</div>';

      unset($target_markup);
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);

    $markup .= '</div>';
    return $markup;
  }

  $markup .= '<h3 id="mfcs-troubleshoot_0-locations-page-section-enabled" class="information_page-section-title">' . "Enabled Locations" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-enabled">';
  if (empty($markup_enabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_enabled;
  }
  $markup .= '</div>';

  $markup .= '<hr class="information_page-section-separator">';

  $markup .= '<h3 id="mfcs-troubleshoot_0-locations-page-section-enabled" class="information_page-section-title">' . "Disabled / Deleted Locations" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-disabled">';
  if (empty($markup_disabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_disabled;
  }
  $markup .= '</div>';

  $markup .= '</div>';
  return $markup;
}

/**
 * Display a list of buildings and their relevant data.
 *
 * This will include a list of disabled buildings as well as enabled.
 */
function mfcs_request_troubleshoot_0_buildings_page() {
  $markup = '<div id="mfcs-troubleshoot_0-buildings-page" class="information_page">';
  $markup .= '<h2 class="information_page-content_header element-invisible">Main Content</h2>';

  try {
    $query = db_select('mfcs_banner_buildings', 'mbb');
    $query->innerJoin('mfcs_banner_building_to_location', 'mbbl', 'mbb.id = mbbl.building');
    $query->innerJoin('mfcs_banner_locations', 'mbl', 'mbbl.location = mbl.id');

    $query->addField('mbb', 'id', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_machine_name');
    $query->addField('mbb', 'human_name', 'building_human_name');
    $query->addField('mbb', 'date', 'building_date');
    $query->addField('mbb', 'disabled', 'building_disabled');
    $query->addField('mbbl', 'date', 'building_to_location_date');
    $query->addField('mbbl', 'disabled', 'building_to_location_disabled');
    $query->addField('mbl', 'id', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_machine_name');
    $query->addField('mbl', 'human_name', 'location_human_name');
    $query->addField('mbl', 'date', 'location_date');
    $query->addField('mbl', 'disabled', 'location_disabled');

    $query->orderBy('mbb.disabled');
    $query->orderBy('mbb.machine_name');

    $entries = $query->execute()->fetchAllAssoc('building_id');

    $is_even = TRUE;
    $markup_enabled = '';
    $markup_disabled = '';
    foreach ($entries as $entry) {
      $target_markup = &$markup_enabled;
      if ($entry->building_disabled == 1) {
        $target_markup = &$markup_disabled;
      }

      $is_even = !$is_even;
      if ($is_even) {
        $even_odd_css = 'even';
      }
      else {
        $even_odd_css = 'odd';
      }

      $target_markup .= '<div id="mfcs-troubleshoot_0-buildings-page-entry-' . $entry->building_id . '" class="information_page-entry ' . $even_odd_css . '">';
      $target_markup .= '  <h4 class="information_page-entry-title">' . $entry->building_human_name . '</h4>';
      $target_markup .= '  <ul class="information_page-entry-list">';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_machine_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_human_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_human_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_date even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->building_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_disabled odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->building_disabled == 0 ? "No" : "Yes") . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_id even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_machine_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_human_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_human_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_date odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->location_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_disabled even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->location_disabled == 0 ? "No" : "Yes") . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '  </ul>';
      $target_markup .= '</div>';

      unset($target_markup);
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);

    $markup .= '</div>';
    return $markup;
  }

  $markup .= '<h3 id="mfcs-troubleshoot_0-buildings-page-section-enabled" class="information_page-section-title">' . "Enabled Buildings" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-enabled">';
  if (empty($markup_enabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_enabled;
  }
  $markup .= '</div>';

  $markup .= '<hr class="information_page-section-separator">';

  $markup .= '<h3 id="mfcs-troubleshoot_0-buildings-page-section-enabled" class="information_page-section-title">' . "Disabled / Deleted Buildings" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-disabled">';
  if (empty($markup_disabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_disabled;
  }
  $markup .= '</div>';

  $markup .= '</div>';
  return $markup;
}

/**
 * Display a list of buildings and their relevant data.
 *
 * This will include a list of disabled buildings as well as enabled.
 */
function mfcs_request_troubleshoot_0_rooms_page() {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '<div id="mfcs-troubleshoot_0-rooms-page" class="information_page">';
  $markup .= '<h2 class="information_page-content_header element-invisible">Main Content</h2>';

  try {
    $query = db_select('mfcs_banner_rooms', 'mbr');
    $query->innerJoin('mfcs_banner_buildings', 'mbb', 'mbr.building = mbb.id');
    $query->innerJoin('mfcs_banner_building_to_location', 'mbbl', 'mbr.building = mbbl.building');
    $query->innerJoin('mfcs_banner_locations', 'mbl', 'mbbl.location = mbl.id');
    $query->leftJoin('users', 'u', 'mbr.coordinator_id = u.uid');

    $query->addField('mbr', 'id', 'room_id');
    $query->addField('mbr', 'machine_name', 'room_machine_name');
    $query->addField('mbr', 'human_name', 'room_human_name');
    $query->addField('mbr', 'date', 'room_date');
    $query->addField('mbr', 'disabled', 'room_disabled');
    $query->addField('mbr', 'coordinator_id', 'room_coordinator_id');
    $query->addField('mbr', 'capacity_normal', 'room_capacity_normal');
    $query->addField('mbr', 'capacity_max', 'room_capacity_max');
    $query->addField('mbbl', 'date', 'room_to_location_date');
    $query->addField('mbbl', 'disabled', 'room_to_location_disabled');
    $query->addField('mbl', 'id', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_machine_name');
    $query->addField('mbl', 'human_name', 'location_human_name');
    $query->addField('mbl', 'date', 'location_date');
    $query->addField('mbl', 'disabled', 'location_disabled');
    $query->addField('mbb', 'id', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_machine_name');
    $query->addField('mbb', 'human_name', 'building_human_name');
    $query->addField('mbb', 'date', 'building_date');
    $query->addField('mbb', 'disabled', 'building_disabled');
    $query->addField('u', 'name', 'room_coordinator_name');

    $query->orderBy('mbr.disabled');
    $query->orderBy('mbr.machine_name');

    $entries = $query->execute()->fetchAllAssoc('room_id');

    $is_even = TRUE;
    $markup_enabled = '';
    $markup_disabled = '';
    foreach ($entries as $entry) {
      $target_markup = &$markup_enabled;
      if ($entry->room_disabled == 1) {
        $target_markup = &$markup_disabled;
      }

      $is_even = !$is_even;
      if ($is_even) {
        $even_odd_css = 'even';
      }
      else {
        $even_odd_css = 'odd';
      }

      $target_markup .= '<div id="mfcs-troubleshoot_0-rooms-page-entry-' . $entry->room_id . '" class="information_page-entry ' . $even_odd_css . '">';
      $target_markup .= '  <h4 class="information_page-entry-title">' . $entry->room_human_name . '</h4>';
      $target_markup .= '  <ul class="information_page-entry-list">';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_machine_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_human_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_human_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_date even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->room_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_disabled odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->room_disabled == 0 ? "No" : "Yes") . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_coordinator even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Coordinator:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'user/' . $entry->room_coordinator_id . $url_arguments . '" title="View User Profile">' . $entry->room_coordinator_name . '</a> [id: ' . $entry->room_coordinator_id . ']' . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_capacity_normal odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Capacity Normal:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_capacity_normal . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_capacity_max even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Capacity Max:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_capacity_max . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_machine_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_human_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_human_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_date even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->building_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_disabled odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->building_disabled == 0 ? "No" : "Yes") . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_id even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_machine_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_human_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_human_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_date odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->location_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_disabled even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->location_disabled == 0 ? "No" : "Yes") . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '  </ul>';
      $target_markup .= '</div>';

      unset($target_markup);
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);

    $markup .= '</div>';
    return $markup;
  }

  $markup .= '<h3 id="mfcs-troubleshoot_0-rooms-page-section-enabled" class="information_page-section-title">' . "Enabled Rooms" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-enabled">';
  if (empty($markup_enabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_enabled;
  }
  $markup .= '</div>';

  $markup .= '<hr class="information_page-section-separator">';

  $markup .= '<h3 id="mfcs-troubleshoot_0-rooms-page-section-enabled" class="information_page-section-title">' . "Disabled / Deleted Rooms" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-disabled">';
  if (empty($markup_disabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_disabled;
  }
  $markup .= '</div>';

  $markup .= '</div>';
  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
