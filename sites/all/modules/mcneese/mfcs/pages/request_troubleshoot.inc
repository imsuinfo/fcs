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

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $page_title = "Facilities Use Troubleshooting";
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

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_REQUESTS_REVIEW_RECHECK . ': Requests Review Recheck</h2>' . "\n";
  $markup .= '<div class="code-description">' . "\n";
  $markup .= 'When the reviewer assignments are changed, it is possible to forget to click the "Recheck Requests" button.' . "<br>\n";
  $markup .= "<br>\n";
  $markup .= 'Using this troubleshoot will force a re-check for all requests currently under review.' . "<br>\n";
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

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_MER_CACHE_SYNCHRONIZE . ': Requests Cache Synchronize</h2>' . "\n";
  $markup .= '<div class="code-description">' . "\n";
  $markup .= 'The requests cache exist to significantly increase the performance of the system when there are a large number of requests on the system. This requests cache is only loosely connected to the actual data, and therefore, the possibility exists that (due to bugs) this cache can become out of sync.' . "<br>\n";
  $markup .= "<br>\n";
  $markup .= 'Using this troubleshoot will force a re-synchronization of all cacheed request data.' . "<br>\n";
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

  $markup = '<h2 class="code-title">#' . MFCS_TROUBLESHOOT_BANNER_CACHE_SYNCHRONIZE . ': Banner Cache Synchronize</h2>' . "\n";
  $markup .= '<div class="code-description">' . "\n";
  $markup .= 'The banner data is cached locally to allow for this system to operate even when banner is unavailable. This cache is usually re-synchronized once a day.' . "<br>\n";
  $markup .= "<br>\n";
  $markup .= 'Using this troubleshoot will force a re-synchronization of all banner cache data if the system can connect to the banner database.' . "<br>\n";
  $markup .= '<strong class="code-warning warning">This may take up to 15 minutes.</strong>' . "<br>\n";
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
      $result = mfcs_sync_banner_to_local($transaction, $user);
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

    return FALSE;
  }

  return $result;
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

        $checked = mfcs_check_if_requests_are_fully_reviewed($request_ids, $classification, $step);

        if ($checked === FALSE) {
          $transaction->rollback();

          form_set_error('form', "An error occurred while trying to recheck the requests. Please contact the support staff.");
          watchdog(MFCS_WATCHDOG_ID, "An error occured while trying to recheck the requests.", array(), WATCHDOG_ERROR);

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

      $query = db_insert('mfcs_request_revisions');
      $query->fields($fields_revisions);
      $query->execute();
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
 * @} End of '@addtogroup mfcs'.
 */
