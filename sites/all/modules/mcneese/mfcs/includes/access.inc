<?php

/**
 * @file
 * Defines McNeese facilities use access functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Perform the access control check for the create operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_create(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if ($roles['requester']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    // create operations on a specific request is actually a clone/copy operation, which only needs view access to clone/copy.
    $access_results = mfcs_page_request_access('view', $request_ids, $user);

    if (is_array($access_results)) {
      foreach ($access_results as $id => $access_result) {
        $access[$id] = $access_result;
        $processed[$id] = $access_result;
      }
    }
    else {
      foreach ($request_ids as $id) {
        $access[$id] = FALSE;
        $processed[$id] = FALSE;
      }
    }

    return TRUE;
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the edit operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 * @param string $section
 *   Check whether or not the user is allowed to perform a certain operation
 *   for a specific section.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_edit(&$request_ids, &$processed, &$access, $roles, $user, $section) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if (!is_null($section) && !is_string($section)) {
    cf_error::invalid_string('section');
    return FALSE;
  }

  $edit_request_status = array(
    MFCS_REQUEST_STATUS_UNLOCKED,
    MFCS_REQUEST_STATUS_LOCKED,
  );


  // when quick meeting is enabled, a section is not null, then additional checks are required.
  if (!array_key_exists(NULL, $request_ids) && !is_null($section) && ($roles['manager'] || $roles['requester'])) {

    // deny access to sections that are not allowed to be edited for a quick meeting.
    $allowed_sections = array(
      'request',
      'information',
      'coordinator',
      'dates',
    );

    if (!array_key_exists($section, $allowed_sections)) {
      try {
        $query = db_select('mfcs_requests', 'mer');

        $query->innerjoin('mfcs_current_information', 'mci', 'mer.id = mci.request_id');
        $query->innerjoin('mfcs_field_information_type', 'mfit', 'mci.request_id = mfit.request_id AND mci.type = mfit.revision AND mfit.delta = 0');

        $query->addField('mer', 'id', 'id');

        $query->condition('mer.id', $request_ids, 'IN');
        $query->condition('mfit.value', MFCS_REQUEST_TYPE_QUICK_MEETING);

        $access_results = $query->execute()->fetchAllAssoc('id');

        if (is_array($access_results)) {
          foreach ($request_ids as $id) {
            if (array_key_exists($id, $access_results)) {
              $processed[$id] = FALSE;
              unset($request_ids[$id]);
            }
          }
        }
        else {
          return FALSE;
        }
      }
      catch (Exception $ex) {
        cf_error::on_exception($ex);

        return FALSE;
      }
    }
  }

  if ($roles['manager'] || $roles['requester']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
      $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mfts.value', $edit_request_status, 'IN');

      if ($roles['requester'] && !$roles['manager']) {
        $query->condition('mer.user_id', $user->uid);
      }

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return FALSE;
    }
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the view operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_view(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  $request_status = array(
    MFCS_REQUEST_STATUS_LOCKED,
    MFCS_REQUEST_STATUS_UNLOCKED,
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    MFCS_REQUEST_STATUS_CLOSED_DENIED,
    MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
    MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
    MFCS_REQUEST_STATUS_CANCELLED,
  );

  if ($roles['manager'] || $roles['requester']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
      $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');

      if ($roles['requester'] && !$roles['manager']) {
        $query->innerjoin('mfcs_field_top_step', 'mftsp', 'mct.request_id = mftsp.request_id AND mct.step = mftsp.revision AND mftsp.delta = 0');

        $or = db_or();

        $and = db_and();
        $and->condition('mer.user_id', $user->uid);
        $and->condition('mfts.value', $request_status, 'IN');
        $or->condition($and);

        // always show completed and accepted requests when there are no restrictions.
        $and = db_and();
        $and->condition('mfts.value', MFCS_REQUEST_STATUS_CLOSED_ACCEPTED);
        $and->condition('mftsp.value', MFCS_REVIEW_STEP_COMPLETED);
        $or->condition($and);

        $query->condition($or);
      }
      else {
        $query->condition('mfts.value', MFCS_REQUEST_STATUS_DELETED, '<>');
      }

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return FALSE;
    }
  }
  elseif ($roles['reviewer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    $review_access = mfcs_page_request_reviewer_access($user, $request_ids, NULL);

    if (is_array($review_access)) {
      foreach ($request_ids as $id) {
        $processed[$id] = array_key_exists($id, $review_access) && $review_access[$id];
        $access[$id] = $processed[$id];
      }
    }
    else {
      foreach ($request_ids as $id) {
        $processed[$id] = FALSE;
        $access[$id] = FALSE;
      }
    }

    return TRUE;
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the review operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_review(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if ($roles['reviewer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    $review_access = mfcs_page_request_reviewer_access($user, $request_ids, TRUE);

    if (is_array($review_access)) {
      foreach ($request_ids as $id) {
        $processed[$id] = array_key_exists($id, $review_access) && $review_access[$id];
        $access[$id] = $processed[$id];
      }
    }
    else {
      foreach ($request_ids as $id) {
        $processed[$id] = FALSE;
        $access[$id] = FALSE;
      }
    }

    return TRUE;
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the log operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_log(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if ($roles['manager']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    foreach ($request_ids as $request_id) {
      $access[$request_id] = TRUE;
      $processed[$request_id] = TRUE;
    }

    return TRUE;
  }

  if ($roles['reviewer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    $review_access = mfcs_page_request_reviewer_access($user, $request_ids, NULL);

    if (is_array($review_access)) {
      foreach ($request_ids as $id) {
        $processed[$id] = array_key_exists($id, $review_access) && $review_access[$id];
        $access[$id] = $processed[$id];
      }
    }
    else {
      foreach ($request_ids as $id) {
        $processed[$id] = FALSE;
        $access[$id] = FALSE;
      }
    }

    return TRUE;
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the cancel operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_cancel(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if ($roles['requester'] || $roles['reviewer'] || $roles['manager']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  $cancellable_status_normal = array(
    MFCS_REQUEST_STATUS_UNLOCKED,
  );

  $cancellable_status_admin = array(
    MFCS_REQUEST_STATUS_LOCKED,
    MFCS_REQUEST_STATUS_UNLOCKED,
  );

  // if the user is the venue coordinator, then allow them to cancel a request at any point in the process.
  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
    $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

    $query->innerjoin('mfcs_current_venue_coordinator', 'mcvc', 'mer.id = mcvc.request_id');
    $query->innerjoin('mfcs_field_venue_coordinator_user_id', 'mfvcui', 'mcvc.request_id = mfvcui.request_id AND mcvc.user_id = mfvcui.revision AND mfvcui.delta = 0');

    $query->addField('mer', 'id', 'id');

    $query->condition('mer.id', $request_ids, 'IN');
    $query->condition('mfts.value', $cancellable_status_admin, 'IN');
    $query->condition('mfvcui.value', $user->uid);

    $access_results = $query->execute()->fetchAllAssoc('id');

    if (is_array($access_results)) {
      $request_ids_copy = $request_ids;
      foreach ($request_ids_copy as $id => $value) {
        if (array_key_exists($id, $access_results)) {
          $processed[$id] = TRUE;
          $access[$id] = TRUE;
          unset($request_ids[$id]);
        }
      }
      unset($request_ids_copy);
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);
    return FALSE;
  }

  if ($roles['requester'] || $roles['manager']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
      $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');

      if ($roles['requester'] && !$roles['manager']) {
        $query->condition('mer.user_id', $user->uid);
        $query->condition('mfts.value', $cancellable_status_normal, 'IN');
      }
      else {
        $query->condition('mfts.value', $cancellable_status_admin, 'IN');
      }

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return FALSE;
    }

    return TRUE;
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the uncancel operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_uncancel(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }


  if ($roles['requester'] || $roles['reviewer'] || $roles['manager']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  $uncancellable_status = array(
    MFCS_REQUEST_STATUS_CANCELLED,
  );

  // if the user is the venue coordinator, then allow them to uncancel a request at any point in the process.
  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
    $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

    $query->innerjoin('mfcs_current_venue_coordinator', 'mcvc', 'mer.id = mcvc.request_id');
    $query->innerjoin('mfcs_field_venue_coordinator_user_id', 'mfvcui', 'mcvc.request_id = mfvcui.request_id AND mcvc.user_id = mfvcui.revision AND mfvcui.delta = 0');

    $query->addField('mer', 'id', 'id');

    $query->condition('mer.id', $request_ids, 'IN');
    $query->condition('mfts.value', $uncancellable_status, 'IN');
    $query->condition('mfvcui.value', $user->uid);

    $access_results = $query->execute()->fetchAllAssoc('id');

    if (is_array($access_results)) {
      $request_ids_copy = $request_ids;
      foreach ($request_ids_copy as $id => $value) {
        if (array_key_exists($id, $access_results)) {
          $processed[$id] = TRUE;
          $access[$id] = TRUE;
          unset($request_ids[$id]);
        }
      }
      unset($request_ids_copy);
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);
    return FALSE;
  }

  if ($roles['manager']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
      $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mfts.value', $uncancellable_status, 'IN');

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return FALSE;
    }

    return TRUE;
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the history operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_history(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if ($roles['manager']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
      $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mfts.value', MFCS_REQUEST_STATUS_DELETED, '<>');

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return FALSE;
    }
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * Perform the access control check for the agreement operation.
 *
 * @param array $request_ids
 *   The array of requests ids yet to be processed.
 * @param array $processed
 *   The array of already processed request ids and the access results.
 * @param array $access
 *   The static variable representing the processed array used to increase
 *   performance.
 * @param array roles
 *   An array of relevant roles associated with access control for requests.
 * @param object user
 *   The drupal user object to check access against.
 *
 * @return bool
 *   TRUE on successfull execution, FALSE otherwise.
 *   The return results do not represent the access control results.
 */
function mfcs_page_request_access_operation_agreement(&$request_ids, &$processed, &$access, $roles, $user) {
  if (!is_array($request_ids)) {
    cf_error::invalid_array('request_ids');
    return FALSE;
  }

  if (!is_array($processed)) {
    cf_error::invalid_array('processed');
    return FALSE;
  }

  if (!is_array($access)) {
    cf_error::invalid_array('access');
    return FALSE;
  }

  if (!is_array($roles)) {
    cf_error::invalid_array('roles');
    return FALSE;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  $agreement_request_status = array(
    MFCS_REQUEST_STATUS_LOCKED,
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    MFCS_REQUEST_STATUS_CLOSED_DENIED,
    MFCS_REQUEST_STATUS_CANCELLED,
  );

  $agreement_request_step = array(
    MFCS_REVIEW_STEP_COMPLETED,
  );

  if ($roles['manager'] || $roles['reviewer']) {
    $agreement_request_step = array(
      MFCS_REVIEW_STEP_COMPLETED,
      MFCS_REVIEW_STEP_MAKE_DECISIONS,
      MFCS_REVIEW_STEP_REVIEW,
      MFCS_REVIEW_STEP_REQUIREMENTS,
    );
  }

  if ($roles['requester'] || $roles['reviewer'] || $roles['manager']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->innerjoin('mfcs_current_top', 'mct', 'mer.id = mct.request_id');
      $query->innerjoin('mfcs_field_top_status', 'mfts', 'mct.request_id = mfts.request_id AND mct.status = mfts.revision AND mfts.delta = 0');
      $query->innerjoin('mfcs_field_top_step', 'mftsp', 'mct.request_id = mftsp.request_id AND mct.step = mftsp.revision AND mftsp.delta = 0');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mfts.value', $agreement_request_status, 'IN');
      $query->condition('mftsp.value', $agreement_request_step, 'IN');

      // the submitter should be the only non-manager/non-reviewer that can access the agreement.
      if (!$roles['manager'] && !$roles['reviewer']) {
        $query->condition('mer.user_id', $user->uid);
      }

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
      }
    }
    catch (Exception $ex) {
      cf_error::on_exception($ex);

      return FALSE;
    }
  }

  if (array_key_exists(NULL, $request_ids)) {
    $access[NULL] = FALSE;
    $processed[NULL] = FALSE;
    return TRUE;
  }

  foreach ($request_ids as $request_id) {
    $access[$request_id] = FALSE;
    $processed[$request_id] = FALSE;
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
