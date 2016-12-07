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
 * Defines troubleshoot codes.
 */
define('MFCS_TROUBLESHOOT_NONE', 0);
define('MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK', 1);
define('MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE', 2);
define('MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE', 3);
define('MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE', 4);
define('MFCS_TROUBLESHOOT_BANNER_INFORMATION', 5);
define('MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK', 6);
define('MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK', 7);

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

  $page_title = 'Facilities Use Troubleshooting Tools';
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
  $markup .= 'When the reviewer assignments are changed, it is possible to forget to click the "Recheck Requests" button.' . '<br>';
  $markup .= '<br>';
  $markup .= 'Using this troubleshoot will force a re-check for all requests currently under review.' . '<br>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => 'Re-check all Requests Reviews',
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
  $markup .= 'The requests cache exist to significantly increase the performance of the system when there are a large number of requests on the system. This requests cache is only loosely connected to the actual data, and therefore, the possibility exists that (due to bugs) this cache can become out of sync.' . '<br>';
  $markup .= '<br>';
  $markup .= 'Using this troubleshoot will force a re-synchronization of all cached request data.' . '<br>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => 'Re-synchronize Requests Cache',
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
  $markup .= 'The banner data is cached locally to allow for this system to operate even when banner is unavailable. This cache is usually auto-re-synchronized once a day.' . '<br>';
  $markup .= '<br>';
  $markup .= 'Using this troubleshoot will force a re-synchronization of all banner cache data (including banner role cache), if the system can connect to the banner database.' . '<br>';
  $markup .= '<strong class="code-warning warning">This may take up to 15 minutes.</strong>' . '<br>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => 'Re-synchronize Banner Cache',
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
  $markup .= 'The banner role is cached locally in a separate manner than the other banner data. This cache is usually auto-re-synchronized once a day.' . '<br>';
  $markup .= '<br>';
  $markup .= 'Using this troubleshoot will force a re-synchronization of <strong>only</strong> the banner role cache data, if the system can connect to the banner database.' . '<br>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => 'Re-synchronize Banner Role Cache',
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
  $markup .= 'The following links provide locally stored information regarding banner data.' . '<br>';
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


  // Requests Problems Check
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
  $markup .= 'Requests are periodically checked for problems, such as date conflicts or deleted rooms.' . '<br>';
  $markup .= '<br>';
  $markup .= 'Using this troubleshoot will perform integrity checks on requests.' . '<br>';
  $markup .= 'Problems are displaed under the <a href="' . $base_path . 'requests/problems-0/requests">Manage Request Problems</a> page.' . '<br>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => 'Check Requests for Problems',
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


  // Users Problems Check
  $code = 'code_' . MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK;
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

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK . ': Users Problem Check</h2>';
  $markup .= '<div class="code-description">';
  $markup .= 'Users are periodically checked for problems, generally when they log into the system.' . '<br>';
  $markup .= '<br>';
  $markup .= 'Using this troubleshoot will perform integrity checks on users.' . '<br>';
  $markup .= 'Problems are displaed under the <a href="' . $base_path . 'requests/problems-0/users">Manage User Problems</a> page.' . '<br>';
  $markup .= '</div>';
  $form['form'][$code]['pre_markup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['form'][$code]['button'] = array(
    '#id' => 'troubleshoot-' . $code . '-submit',
    '#name' => 'troubleshoot-' . $code . '-submit',
    '#type' => 'submit',
    '#default_value' => 'Check Users for Problems',
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
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK . '-submit') {
    // no validation necessary
  }
  else {
    form_set_error('', 'Unknown submit process detected.');
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
  elseif ($clicked_id == 'troubleshoot-code_' . MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK . '-submit') {
    $code = MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK;
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
    $label = 'Unknown';

    if ($code == MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK) {
      $label = 'Requests review re-check';
      $result = mfcs_request_troubleshoot_0_request_review_recheck($transaction, $user);
    }
    elseif ($code == MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE) {
      $label = 'Re-synchronize requests cache';
      $result = mfcs_request_troubleshoot_0_mer_cache_synchronize($transaction, $user);
    }
    elseif ($code == MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE) {
      $label = 'Re-synchronize banner cache';
      $result = mfcs_sync_banner_to_local($transaction, $user, TRUE);
    }
    elseif ($code == MFCS_TROUBLESHOOT_BANNER_ROLE_SYNCHRONIZE) {
      $label = 'Re-synchronize banner role cache';
      $result = mfcs_sync_banner_to_local_users(NULL, NULL, TRUE);
    }
    elseif ($code == MFCS_TROUBLESHOOT_REQUESTS_PROBLEM_CHECK) {
      mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

      $label = 'Check requests for problems';
      $result = mfcs_miscellaneous_check_requests_for_problems();
    }
    elseif ($code == MFCS_TROUBLESHOOT_USERS_PROBLEM_CHECK) {
      mfcs_include(MFCS_INCLUDE_MISCELLANEOUS);

      $label = 'Check users for problems';
      $result = mfcs_miscellaneous_check_users_for_problems();
    }

    // force transaction execution.
    unset($transaction);


    $time_stop = microtime(TRUE);
    $time_elapsed = $time_stop - $time_start;
    $time_elapsed_floor = floor($time_stop - $time_start);

    $time_hours = floor($time_elapsed_floor / 3600);
    $time_minutes = floor(($time_elapsed_floor - ($time_hours * 60)) / 60);
    $time_seconds = ceil($time_elapsed_floor - ($time_minutes * 60));
    $time_microseconds = floor(($time_elapsed - $time_elapsed_floor) * 1000000);

    if ($time_hours > 0) {
      $time_text = 'in @hours hours, @minutes minutes, and @seconds seconds';
      $arguments = array('@hours' => $time_hours, '@minutes' => $time_minutes, '@seconds' => $time_seconds);
    }
    elseif ($time_minutes > 0) {
      $time_text = 'in @minutes minutes and @seconds seconds';
      $arguments = array('@minutes' => $time_minutes, '@seconds' => $time_seconds);
    }
    elseif ($time_seconds > 0) {
      $time_text = 'in @seconds seconds';
      $arguments = array('@seconds' => $time_seconds);
    }
    else {
      $time_text = 'in @microseconds microseconds';
      $arguments = array('@microseconds' => $time_microseconds);
    }

    if ($result) {
      drupal_set_message(t($label . ' completed ' . $time_text . '.', $arguments));
    }
    else {
      drupal_set_message(t($label . ' completed with errors ' . $time_text . '.', $arguments), 'warning');

      $arguments['@user'] = $user->name;
      $arguments['@user_id'] = $user->uid;
      watchdog(MFCS_WATCHDOG_ID, $label . 'started by @user (@user_id) completed with errors ' . $time_text . '.', $arguments, WATCHDOG_WARNING);
    }
  }
  catch (Error $e) {
    if (isset($transaction)) $transaction->rollback();
    cf_error::on_exception($e);
  }
  catch (Exception $e) {
    if (isset($transaction)) $transaction->rollback();
    cf_error::on_exception($e);
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
    foreach (array(MFCS_REVIEW_STEP_REVIEW, MFCS_REVIEW_STEP_FINAL_DECISION) as $step) {
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

          form_set_error('form', 'An error occurred while trying to recheck the requests. Please contact the support staff.');
          watchdog(MFCS_WATCHDOG_ID, 'An error occured while trying to recheck the requests.', array(), WATCHDOG_ERROR);

          return FALSE;
        }

        if (is_array($decisions)) {
          foreach ($decisions as $request_id => $decision) {
            if ($decision['current'] < $decision['total']) {
              continue;
            }

            $message = 'This request\'s workflow has changed as a result of a change in the reviewer management settings.';

            mfcs_workflow_next_step($request_id, $classification, $step, MFCS_REVIEW_DECISION_MANAGER_RECHECK, MFCS_REVIEW_RESTRICTIONS_NONE, $message, TRUE);
          }
        }
      }
    }
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    return FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

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
    drupal_set_message(t('A total of @total requests have been detected to be out of sync.', array('@total' => count($requests))));
    watchdog(MFCS_WATCHDOG_ID, 'The following requests have been detected to be out of sync during the re-sync process: @requests.', array('@requests' => print_r(array_keys($requests), TRUE)), WATCHDOG_NOTICE);
    foreach ($requests as $request_id => $request) {
      drupal_set_message(t('Request @request_id is out of sync and is being corrected.', array('@request_id' => $request_id)));

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
        'message' => 'Request cache has been detected to be out of sync and is now corrected.',
      );

      $request_revision = mfcs_load_request_revision_number($request_id);
      $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);

      if ($request_revision === FALSE) {
        $transaction->rollback();
        watchdog(MFCS_WATCHDOG_ID, 'Failed to load revision number for request %request_id.', array('%request_id' => $request_id), WATCHDOG_ERROR);
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
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    return FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_exception($e);

    return FALSE;
  }

  return TRUE;
}

/**
 * Display a list of locations and their relevant data.
 *
 * This will include a list of disabled locations as well as enabled.
 *
 * @param int|null $location_id
 *   (optional) When not NULL, only load the information for the specified location.
 *   Otherwise load all locations.
 */
function mfcs_request_troubleshoot_0_locations_page($location_id = NULL) {
  if (!is_null($location_id) && !cf_is_integer($location_id)) {
    drupal_not_found();
    drupal_exit();
    return '';
  }

  global $conf;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '<div id="mfcs-troubleshoot_0-locations-page" class="information_page">';
  $markup .= '<h2 class="information_page-content_header element-invisible">Main Content</h2>';

  try {
    $query = db_select('mfcs_banner_locations', 'mbl');

    if (!is_null($location_id)) {
      $query->condition('mbl.id', $location_id);
    }

    $query->addField('mbl', 'id', 'location_id');
    $query->addField('mbl', 'machine_name', 'machine_name');
    $query->addField('mbl', 'human_name', 'human_name');
    $query->addField('mbl', 'date', 'date');
    $query->addField('mbl', 'disabled', 'disabled');

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
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Code:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->machine_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-human_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'requests/troubleshoot-0/locations/' . $entry->location_id . '/edit' . $url_arguments . '" class="information_page-entry-list-item-value-link">' . $entry->human_name . '</a></div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-date odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-disabled even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->disabled == 0 ? 'No' : 'Yes') . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '  </ul>';
      $target_markup .= '</div>';

      unset($target_markup);
    }
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    $markup .= '</div>';
    return $markup;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    $markup .= '</div>';
    return $markup;
  }

  $markup .= '<h3 id="mfcs-troubleshoot_0-locations-page-section-enabled" class="information_page-section-title information_page-section-title-enabled">' . "Enabled Locations" . '</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-enabled">';
  if (empty($markup_enabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_enabled;
  }
  $markup .= '</div>';

  $markup .= '<hr class="information_page-section-separator">';

  $markup .= '<h3 id="mfcs-troubleshoot_0-locations-page-section-enabled" class="information_page-section-title information_page-section-title-disabled">' . "Disabled / Deleted Locations" . '</h3>';
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
 * Change the name of the location.
 *
 * The banner information does not provide a human friendly name, so this provides a way to change that name.
 *
 * @param int|null $location_id
 *   (optional) When not NULL, only load the information for the specified location.
 *   Otherwise load all locations.
 */
function mfcs_request_troubleshoot_0_locations_edit_name_form($form, &$form_state, $location_id) {
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

  if (!is_null($location_id) && !cf_is_integer($location_id)) {
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

  $location_id = (int) $location_id;
  $location = mfcs_load_locations($location_id, TRUE);

  if ($location === FALSE) {
    drupal_not_found();
    drupal_exit();
  }

  mfcs_add_canonical_headers('requests/troubleshoot-0/locations/' . $location_id . '/edit');

  $page_title = 'Troubleshooting - Locations - Edit Name';
  drupal_set_title($page_title);

  $user = cf_current_user();

  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['location'] = array(
    '#type' => 'value',
    '#value' => $location,
  );

  $form['form']['location_id'] = array(
    '#type' => 'value',
    '#value' => $location_id,
  );

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '',
  );

  $form['location'] = array(
    '#tree' => TRUE,
  );

  $form['location']['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
    '#default_value' => $location->location_name,
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#id' => 'locations-edit_name-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'locations-edit_name-submit',
      ),
    ),
  );

  $form['submit']['cancel'] = array(
    '#id' => 'locations-edit_name-submit-cancel',
    '#name' => 'locations-edit_name-submit-cancel',
    '#type' => 'submit',
    '#default_value' => 'Cancel',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'request_problems-submit-cancel',
      ),
    ),
  );

  $form['submit']['submit'] = array(
    '#id' => 'locations-edit_name-submit-submit',
    '#name' => 'locations-edit_name-submit-submit',
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'request_problems-submit-submit',
      ),
    ),
  );


  $form['markup']['#markup'] .= '<div class="locations-edit_name-summary">';
  $form['markup']['#markup'] .= '  <div class="locations-edit_name-summary_row"><span class="locations-edit_name-summary_row-label label-id">Location ID:</span> <span class="locations-edit_name-summary_row-value value-id">' . $location_id . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="locations-edit_name-summary_row"><span class="locations-edit_name-summary_row-label label-machine_name">Machine Name:</span> <span class="locations-edit_name-summary_row-value value-machine_name">' . $location->machine_name . '</a></span></div>';
  $form['markup']['#markup'] .= '  <div class="locations-edit_name-summary_row"><span class="locations-edit_name-summary_row-label label-location_name">Location Name:</span> <span class="locations-edit_name-summary_row-value value-location_name">' . $location->location_name . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="locations-edit_name-summary_row"><span class="locations-edit_name-summary_row-label label-date_updated">Date Updated:</span> <span class="locations-edit_name-summary_row-value value-date_updated">' . date('l, F j, Y h:i:s a', $location->date) . '</span></div>';
  $form['markup']['#markup'] .= '  <div class="locations-edit_name-summary_row"><span class="locations-edit_name-summary_row-label label-disabled">Disabled:</span> <span class="locations-edit_name-summary_row-value value-disabled">' . (isset($location->disabled) && $location->disabled == 1 ? 'Yes' : 'No') . '</span></div>';
  $form['markup']['#markup'] .= '</div>';

  return $form;
}

/**
 * Validation for mfcs_request_troubleshoot_0_locations_edit_name_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_troubleshoot_0_locations_edit_name_form()
 */
function mfcs_request_troubleshoot_0_locations_edit_name_form_validate($form, &$form_state) {
}

/**
 * Submit for mfcs_request_troubleshoot_0_locations_edit_name_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_troubleshoot_0_locations_edit_name_form()
 */
function mfcs_request_troubleshoot_0_locations_edit_name_form_submit($form, &$form_state) {
  global $mfcs_determined;
  $user = cf_current_user();

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'locations-edit_name-submit-cancel') {
    if (empty($form_state['values']['redirect_to'])) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/troubleshoot-0/locations');
    }
    else {
      $form_state['redirect'] = $form_state['values']['redirect_to'];
    }

    return;
  }

  $instance = mfcs_instance();

  $failure = FALSE;
  $transaction = db_transaction();
  try {
    $query = db_update('mfcs_banner_locations');
    $query->fields(array('human_name' => $form_state['values']['location']['name'], 'date' => $instance));
    $query->condition('id', $form['form']['location_id']['#value']);
    $query->execute();
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

  if ($failure) {
    form_set_error('form', 'An error occurred while trying to save the location name. Please contact the support staff.');
    watchdog(MFCS_WATCHDOG_ID, 'An error occurred while trying to save the location name.', array(), WATCHDOG_ERROR);

    $form_state['rebuild'] = TRUE;
    $form_state['redirect'] = FALSE;
    $form_state['submitted'] = FALSE;
    return FALSE;
  }

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = mfcs_build_redirect_array('requests/troubleshoot-0/locations');
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Display a list of buildings and their relevant data.
 *
 * This will include a list of disabled buildings as well as enabled.
 *
 * @param int|null $building_id
 *   (optional) When not NULL, only load the information for the specified building.
 *   Otherwise load all buildings.
 */
function mfcs_request_troubleshoot_0_buildings_page($building_id = NULL) {
  if (!is_null($building_id) && !cf_is_integer($building_id)) {
    drupal_not_found();
    drupal_exit();
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '<div id="mfcs-troubleshoot_0-buildings-page" class="information_page">';
  $markup .= '<h2 class="information_page-content_header element-invisible">Main Content</h2>';

  try {
    $query = db_select('mfcs_banner_buildings', 'mbb');
    $query->innerJoin('mfcs_banner_building_to_location', 'mbbl', 'mbb.id = mbbl.building');
    $query->innerJoin('mfcs_banner_locations', 'mbl', 'mbbl.location = mbl.id');

    if (!is_null($building_id)) {
      $query->condition('mbb.id', $building_id);
    }

    $query->addField('mbb', 'id', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_code');
    $query->addField('mbb', 'human_name', 'building_name');
    $query->addField('mbb', 'date', 'building_date');
    $query->addField('mbb', 'disabled', 'building_disabled');
    $query->addField('mbbl', 'date', 'building_to_location_date');
    $query->addField('mbbl', 'disabled', 'building_to_location_disabled');
    $query->addField('mbl', 'id', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_code');
    $query->addField('mbl', 'human_name', 'location_name');
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
      $target_markup .= '  <h4 class="information_page-entry-title">' . $entry->building_name . '</h4>';
      $target_markup .= '  <ul class="information_page-entry-list">';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_code even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Code:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_code . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_name . '</div>';
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
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'requests/troubleshoot-0/locations/' . $entry->location_id . $url_arguments . '" class="no_print_links_as_link">' . $entry->location_id . '</a></div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_code odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Code:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_code . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_date odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->location_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_disabled even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->location_disabled == 0 ? 'No' : 'Yes') . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '  </ul>';
      $target_markup .= '</div>';

      unset($target_markup);
    }
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    $markup .= '</div>';
    return $markup;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    $markup .= '</div>';
    return $markup;
  }

  $markup .= '<h3 id="mfcs-troubleshoot_0-buildings-page-section-enabled" class="information_page-section-title information_page-section-title-enabled">Enabled Buildings</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-enabled">';
  if (empty($markup_enabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_enabled;
  }
  $markup .= '</div>';

  $markup .= '<hr class="information_page-section-separator">';

  $markup .= '<h3 id="mfcs-troubleshoot_0-buildings-page-section-disabled" class="information_page-section-title information_page-section-title-disabled">Disabled / Deleted Buildings</h3>';
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
 *
 * @param int|null $room_id
 *   (optional) When not NULL, only load the information for the specified room.
 *   Otherwise load all rooms.
 */
function mfcs_request_troubleshoot_0_rooms_page($room_id = NULL) {
  if (!is_null($room_id) && !cf_is_integer($room_id)) {
    drupal_not_found();
    drupal_exit();
    return '';
  }

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
    $query->leftJoin('users', 'u', 'mbr.room_coordinator_id = u.uid');
    $query->leftJoin('users', 'u2', 'mbr.venue_coordinator_id = u2.uid');

    if (!is_null($room_id)) {
      $query->condition('mbr.id', $room_id);
    }

    $query->addField('mbr', 'id', 'room_id');
    $query->addField('mbr', 'machine_name', 'room_number');
    $query->addField('mbr', 'human_name', 'room_name');
    $query->addField('mbr', 'date', 'room_date');
    $query->addField('mbr', 'disabled', 'room_disabled');
    $query->addField('mbr', 'room_coordinator_id', 'room_coordinator_id');
    $query->addField('mbr', 'venue_coordinator_id', 'venue_coordinator_id');
    $query->addField('mbr', 'capacity_normal', 'room_capacity_normal');
    $query->addField('mbr', 'capacity_max', 'room_capacity_max');
    $query->addField('mbbl', 'date', 'room_to_location_date');
    $query->addField('mbbl', 'disabled', 'room_to_location_disabled');
    $query->addField('mbl', 'id', 'location_id');
    $query->addField('mbl', 'machine_name', 'location_code');
    $query->addField('mbl', 'human_name', 'location_name');
    $query->addField('mbl', 'date', 'location_date');
    $query->addField('mbl', 'disabled', 'location_disabled');
    $query->addField('mbb', 'id', 'building_id');
    $query->addField('mbb', 'machine_name', 'building_code');
    $query->addField('mbb', 'human_name', 'building_name');
    $query->addField('mbb', 'date', 'building_date');
    $query->addField('mbb', 'disabled', 'building_disabled');
    $query->addField('u', 'name', 'room_coordinator_name');
    $query->addField('u2', 'name', 'venue_coordinator_name');

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
      $target_markup .= '  <h4 class="information_page-entry-title">' . $entry->room_name . '</h4>';
      $target_markup .= '  <ul class="information_page-entry-list">';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_id . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_number even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Number:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_number . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_date even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->room_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_disabled odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->room_disabled == 0 ? 'No' : 'Yes') . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_coordinator even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Coordinator:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'users-0/view/' . $entry->room_coordinator_id . $url_arguments . '" title="View User Profile" class="no_print_links_as_link">' . $entry->room_coordinator_name . '</a> [id: ' . $entry->room_coordinator_id . ']' . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-venue_coordinator odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Venue Coordinator:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'users-0/view/' . $entry->venue_coordinator_id . $url_arguments . '" title="View User Profile" class="no_print_links_as_link">' . $entry->venue_coordinator_name . '</a> [id: ' . $entry->venue_coordinator_id . ']' . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_capacity_normal even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Capacity Normal:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_capacity_normal . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-room_capacity_max odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Room Capacity Max:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->room_capacity_max . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_id even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'requests/troubleshoot-0/buildings/' . $entry->building_id . $url_arguments . '" class="no_print_links_as_link">' . $entry->building_id . '</a></div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_code odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Code:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_code . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_name even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->building_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_date odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->building_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-building_disabled even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Building Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->building_disabled == 0 ? 'No' : 'Yes') . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_id odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location ID:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value"><a href="' . $base_path . 'requests/troubleshoot-0/locations/' . $entry->location_id . $url_arguments . '" class="no_print_links_as_link">' . $entry->location_id . '</a></div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_code even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Machine Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_code . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_name odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Human Name:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . $entry->location_name . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_date even">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Last Updated:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . date('l, F j Y h:i:s a', $entry->location_date) . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '    <li class="information_page-entry-list-item information_page-entry-list-item-location_disabled odd">';
      $target_markup .= '      <div class="information_page-entry-list-item-name">Location Disabled / Deleted:</div> ';
      $target_markup .= '      <div class="information_page-entry-list-item-value">' . ($entry->location_disabled == 0 ? 'No' : 'Yes') . '</div>';
      $target_markup .= '    </li>';
      $target_markup .= '  </ul>';
      $target_markup .= '</div>';

      unset($target_markup);
    }
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    $markup .= '</div>';
    return $markup;
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    $markup .= '</div>';
    return $markup;
  }

  $markup .= '<h3 id="mfcs-troubleshoot_0-rooms-page-section-enabled" class="information_page-section-title information_page-section-title-enabled">"Enabled Rooms</h3>';
  $markup .= '<div class="information_page-section-content information_page-section-content-enabled">';
  if (empty($markup_enabled)) {
    $markup .= 'There are no entries in this section.';
  }
  else {
    $markup .= $markup_enabled;
  }
  $markup .= '</div>';

  $markup .= '<hr class="information_page-section-separator">';

  $markup .= '<h3 id="mfcs-troubleshoot_0-rooms-page-section-disabled" class="information_page-section-title information_page-section-title-disabled">Disabled / Deleted Rooms</h3>';
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
