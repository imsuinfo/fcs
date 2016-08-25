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

  if ($roles['requester'] || $roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    // create operations on a specific request is actually a clone/copy operation, which only needs view access to clone/copy.
    return mfcs_page_request_access_operation_view($request_ids, $processed, $access, $roles, $user);
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

  if ($roles['requester'] || $roles['reviewer'] || $roles['manager']  || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      // load type field when section is not NULL so that it can be tested for quick meeting status.
      if (!is_null($section)) {
        $query->addField('mer', 'type', 'type');
      }

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', $edit_request_status, 'IN');

      if (!$roles['manager'] && !$roles['administer']) {
        if ($roles['requester']) {
          if ($roles['reviewer']) {
            $or = db_or();
            $or->condition('mer.venue_coordinator', $user->uid);
            $or->condition('mer.user_id', $user->uid);
            $query->condition($or);
          }
          else {
            $query->condition('mer.user_id', $user->uid);
          }
        }
        elseif ($roles['reviewer']) {
          $query->condition('mer.venue_coordinator', $user->uid);
        }
      }

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        if (is_null($section)) {
          foreach ($request_ids as $id) {
            $processed[$id] = array_key_exists($id, $access_results);
            $access[$id] = $processed[$id];
          }
        }
        else {
          // deny access to sections that are not allowed to be edited for a quick meeting.
          $allowed_sections_quick_meeting = array(
            'request',
            'information',
            'coordinator',
            'dates',
          );

          $section_restricted_quick_meeting = !(array_key_exists($section, $allowed_sections_quick_meeting));
          foreach ($request_ids as $id) {
            $processed[$id] = array_key_exists($id, $access_results);
            $access[$id] = $processed[$id];

            if ($processed[$id] && $access_results[$id]->type === MFCS_REQUEST_TYPE_QUICK_MEETING) {
              if ($section_restricted_quick_meeting) {
                $access[$id] = FALSE;
              }
            }
          }
        }

        return TRUE;
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
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
  );

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
    }
    else {
      foreach ($request_ids as $request_id) {
        $access[$request_id] = TRUE;
        $processed[$request_id] = TRUE;
      }
    }

    return TRUE;
  }

  if (!$roles['reviewer'] && $roles['requester']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');

      if ($roles['requester'] && !$roles['manager'] && !$roles['administer']) {
        $or = db_or();

        $and = db_and();
        $and->condition('mer.user_id', $user->uid);
        $and->condition('mer.status', $request_status, 'IN');
        $or->condition($and);

        // always show completed and accepted requests when there are no restrictions.
        $and = db_and();
        $and->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_ACCEPTED);
        $and->condition('mer.step', MFCS_REVIEW_STEP_COMPLETED);
        $or->condition($and);

        $query->condition($or);
      }
      else {
        $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');
      }

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        $request_ids_copy = $request_ids;
        foreach ($request_ids_copy as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];

          if ($processed[$id] === TRUE) {
            unset($request_ids[$id]);
          }
        }
        unset($request_ids_copy);
      }

      // looks like all of the requests have been processed, so return.
      if (empty($request_ids)) {
        return TRUE;
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
  }

  if ($roles['reviewer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    $review_access = mfcs_page_request_reviewer_access($user, $request_ids, MFCS_REVIEW_RESTRICTION_VIEW);

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

  if ($roles['reviewer'] || $roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    $review_access = mfcs_page_request_reviewer_access($user, $request_ids, MFCS_REVIEW_RESTRICTION_REVIEW);

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
 * Perform the access control check for the comment operation.
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
function mfcs_page_request_access_operation_comment(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if ($roles['requester'] || $roles['reviewer'] || $roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    // managers and administers should always be able to comment.
    if ($roles['manager'] || $roles['administer']) {
      foreach ($request_ids as $id) {
        $processed[$id] = TRUE;
        $access[$id] = TRUE;
      }
      return TRUE;
    }
    elseif ($roles['reviewer'] || $roles['requester']) {
      $review_access = mfcs_page_request_reviewer_access($user, $request_ids, MFCS_REVIEW_RESTRICTION_COMMENT);

      if (is_array($review_access)) {
        foreach ($request_ids as $id) {
          if (array_key_exists($id, $processed)) {
            continue;
          }

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

  if ($roles['manager'] || $roles['administer']) {
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

  if ($roles['requester']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    // access is dependent on the requester, so load all requester ids for all requests.
    $requesters = mfcs_load_requesters_of_requests($request_ids);

    foreach ($request_ids as $request_id) {
      if (isset($requesters[$request_id]) && $requesters[$request_id] == $user->uid) {
        $access[$request_id] = TRUE;
        $processed[$request_id] = TRUE;
      }
      else {
        $access[$request_id] = FALSE;

        // If the requester also has reviewer role, make sure the unapproved requests are also checked by not marking the request as processed.
        if (!$roles['reviewer']) {
          $processed[$request_id] = FALSE;
        }
      }
    }

    if (!$roles['reviewer']) {
      return TRUE;
    }
  }

  if ($roles['reviewer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    $review_access = mfcs_page_request_reviewer_access($user, $request_ids, MFCS_REVIEW_RESTRICTION_NONE);

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

  if ($roles['requester'] || $roles['reviewer'] || $roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  $not_cancellable_status = array(
    MFCS_REQUEST_STATUS_DELETED,
    MFCS_REQUEST_STATUS_CANCELLED,
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
    MFCS_REQUEST_STATUS_CLOSED_DENIED,
    MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE,
  );

  // always grant managers and administers cancel access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');

      $nand = db_and();
      $nand->not();
      $nand->condition('mer.status', $not_cancellable_status, 'IN');
      $query->condition($nand);

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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

    // a manager has more access than a venue coordinator (and requester), so if they cannot access it, then no one can.
    // improve performance by returning instead of doing more database calls.
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

  // allow the venue coordinators to cancel a request at any point in the process.
  $proxies = mfcs_get_proxy_venue_coordinator($user->uid);
  $venue_coordinator_ids = array();
  if (!empty($proxies)) {
    $venue_coordinator_ids = $proxies;
  }
  $venue_coordinator_ids[$user->uid] = $user->uid;

  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->addField('mer', 'id', 'id');

    $query->condition('mer.id', $request_ids, 'IN');
    $query->condition('mer.venue_coordinator', $venue_coordinator_ids, 'IN');

    $nand = db_and();
    $nand->not();
    $nand->condition('mer.status', $not_cancellable_status, 'IN');
    $query->condition($nand);

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

      // looks like all of the requests have been processed, so return.
      if (empty($request_ids)) {
        return TRUE;
      }
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

  if ($roles['requester']) {
    $cancellable_status = array(
      MFCS_REQUEST_STATUS_UNLOCKED,
      MFCS_REQUEST_STATUS_LOCKED,
      MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK,
      MFCS_REQUEST_STATUS_CLOSED_ACCEPTED,
    );

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.user_id', $user->uid);
      $query->condition('mer.status', $cancellable_status, 'IN');

      $access_results = $query->execute()->fetchAllAssoc('id');

      if (is_array($access_results)) {
        foreach ($request_ids as $id) {
          $processed[$id] = array_key_exists($id, $access_results);
          $access[$id] = $processed[$id];
        }

        return TRUE;
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
 * Perform the access control check for the cancelling operation.
 *
 * Do not confuse this with the cancel/uncancel functionality.
 * This operates only on published requests and only for requests whose individual dates may be cancelled or uncancelled.
 * It does not cancel or uncancel a request as a whole, but instead operates on inidivudal dates.
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
function mfcs_page_request_access_operation_cancelling(&$request_ids, &$processed, &$access, $roles, $user) {
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

  global $conf;


  // support manually disabling of accounts based on specific roles.
  $disable_cancelling_for = array();
  if (isset($conf['disable_cancelling_for']) && is_array($conf['disable_cancelling_for'])) {
    $disable_cancelling_for = $conf['disable_cancelling_for'];
  }

  if (!isset($disable_cancelling_for['reviewer']) || !is_bool($disable_cancelling_for['reviewer'])) {
    $disable_cancelling_for['reviewer'] = FALSE;
  }

  if (!isset($disable_cancelling_for['requester']) || !is_bool($disable_cancelling_for['requester'])) {
    $disable_cancelling_for['requester'] = FALSE;
  }


  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }
  elseif ($roles['reviewer']) {
    if (array_key_exists(NULL, $request_ids)) {
      if ($disable_cancelling_for['reviewer']) {
        // also check for requester access.
        if (!$roles['requester'] || $disable_cancelling_for['requester']) {
          $access[NULL] = FALSE;
          $processed[NULL] = FALSE;
          return FALSE;
        }
      }
      else {
        $access[NULL] = TRUE;
        $processed[NULL] = TRUE;
        return TRUE;
      }
    }
    elseif ($disable_cancelling_for['reviewer']) {
      // also check for requester access.
      if (!$roles['requester'] || $disable_cancelling_for['requester']) {
        $access[NULL] = FALSE;
        $processed[NULL] = FALSE;
        return FALSE;
      }
    }
  }
  elseif ($roles['requester']) {
    if (array_key_exists(NULL, $request_ids)) {
      if ($disable_cancelling_for['requester']) {
        $access[NULL] = FALSE;
        $processed[NULL] = FALSE;
        return FALSE;
      }
      else {
        $access[NULL] = TRUE;
        $processed[NULL] = TRUE;
        return TRUE;
      }
    }
    elseif ($disable_cancelling_for['requester']) {
      $access[$request_id] = FALSE;
      $processed[$request_id] = FALSE;
      return FALSE;
    }
  }
  else {
    // deny access for all other accounts.
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = FALSE;
      $processed[NULL] = FALSE;
      return FALSE;
    }
    else {
      $access[$request_id] = FALSE;
      $processed[$request_id] = FALSE;
      return FALSE;
    }
  }

  // allow the venue coordinators to cancel a request at any point in the process.
  $proxies = mfcs_get_proxy_venue_coordinator($user->uid);
  $venue_coordinator_ids = array();
  if (!empty($proxies)) {
    $venue_coordinator_ids = $proxies;
  }
  $venue_coordinator_ids[$user->uid] = $user->uid;

  $instance = mfcs_instance();
  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->addField('mer', 'id', 'id');

    $query->condition('mer.id', $request_ids, 'IN');

    if (!$roles['manager'] || !$roles['administer']) {
      if ($roles['requester'] && $roles['reviewer']) {
        $or = db_or();
        $or->condition('mer.user_id', $user->uid);
        $or->condition('mer.venue_coordinator', $venue_coordinator_ids, 'IN');
        $query->condition($or);
        unset($or);
      }
      elseif ($roles['reviewer']) {
        $query->condition('mer.venue_coordinator', $venue_coordinator_ids, 'IN');
      }
      elseif ($roles['requester']) {
        $query->condition('mer.user_id', $user->uid);
      }
    }

    $query->condition('mer.status', MFCS_REQUEST_STATUS_CLOSED_ACCEPTED);
    $query->condition('mer.step', MFCS_REVIEW_STEP_COMPLETED);

    // make sure the request has at least 1 date in the future.
    $subquery = db_select('mfcs_current', 'mc');
    $subquery->innerJoin('mfcs_field_dates_date', 'mfdd', 'mc.request_id = mfdd.request_id and mc.dates_date = mfdd.revision');
    $subquery->leftJoin('mfcs_field_dates_time_start', 'mfdts', 'mfdd.request_id = mfdts.request_id and mfdd.revision = mfdts.revision and mfdd.delta = mfdts.delta');

    $subquery->addField('mc', 'request_id', 'request_id');

    $subquery->condition('mc.request_id', $request_ids, 'IN');
    $subquery->condition('mfdd.value', $instance, '>');

    $subor = db_or();
    $subor->isNull('mfdts.value');
    $subor->condition('mfdts.value', $instance, '>');
    $subquery->condition($subor);

    $query->condition('mer.id', $subquery, 'IN');

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

      // looks like all of the requests have been processed, so return.
      if (empty($request_ids)) {
        return TRUE;
      }
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
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
  );

  // always grant managers and administers uncancel access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', $uncancellable_status, 'IN');

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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

    // a manager has more access than a venue coordinator (and requester), so if they cannot access it, then no one can.
    // improve performance by returning instead of doing more database calls.
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

  $proxies = mfcs_get_proxy_venue_coordinator($user->uid);
  $venue_coordinator_ids = array();
  if (!empty($proxies)) {
    $venue_coordinator_ids = $proxies;
  }
  $venue_coordinator_ids[$user->uid] = $user->uid;

  // allow venue coordinators to uncancel a request at any point in the process.
  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->addField('mer', 'id', 'id');

    $query->condition('mer.id', $request_ids, 'IN');
    $query->condition('mer.status', $uncancellable_status, 'IN');
    $query->condition('mer.venue_coordinator', $venue_coordinator_ids, 'IN');

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

      // looks like all of the requests have been processed, so return.
      if (empty($request_ids)) {
        return TRUE;
      }
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
 * Perform the access control check for the reassign coordinator operation.
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
function mfcs_page_request_access_operation_reassign_coordinator(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  // always grant managers and administers re-assign access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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
 * Perform the access control check for the reassign requester operation.
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
function mfcs_page_request_access_operation_reassign_requester(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  // always grant managers and administers re-assign access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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
 * Perform the access control check for the manage operation.
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
function mfcs_page_request_access_operation_manage(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  // always grant managers and administers manage access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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
 * Perform the access control check for the override operation.
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
function mfcs_page_request_access_operation_override(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  // always grant managers and administers override access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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
 * Perform the access control check for the move operation.
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
function mfcs_page_request_access_operation_move(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }
  }

  // always grant managers and administers override access.
  if ($roles['manager'] || $roles['administer']) {
    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');

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

        // looks like all of the requests have been processed, so return.
        if (empty($request_ids)) {
          return TRUE;
        }
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

  if ($roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');

      if (!$roles['administer']) {
        $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED, '<>');
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
    catch (Error $e) {
      cf_error::on_exception($e);

      return FALSE;
    }
    catch (Exception $e) {
      cf_error::on_exception($e);

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
    MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED,
  );

  $agreement_request_step = array(
    MFCS_REVIEW_STEP_COMPLETED,
  );

  if ($roles['manager'] || $roles['reviewer'] || $roles['administer']) {
    $agreement_request_step = array(
      MFCS_REVIEW_STEP_COMPLETED,
      MFCS_REVIEW_STEP_REVIEW,
      MFCS_REVIEW_STEP_FINAL_DECISION,
    );
  }

  if ($roles['requester'] || $roles['reviewer'] || $roles['manager'] || $roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = TRUE;
      $processed[NULL] = TRUE;
      return TRUE;
    }

    try {
      $query = db_select('mfcs_requests', 'mer');

      $query->addField('mer', 'id', 'id');

      $query->condition('mer.id', $request_ids, 'IN');
      $query->condition('mer.status', $agreement_request_status, 'IN');
      $query->condition('mer.step', $agreement_request_step, 'IN');

      // the submitter should be the only non-manager/non-reviewer/non-administer that can access the agreement.
      if (!$roles['manager'] && !$roles['reviewer'] && !$roles['administer']) {
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
    catch (Error $e) {
      cf_error::on_exception($e);

      return FALSE;
    }
    catch (Exception $e) {
      cf_error::on_exception($e);

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
 * Perform the access control check for the delete operation.
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
function mfcs_page_request_access_operation_delete(&$request_ids, &$processed, &$access, $roles, $user) {
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

  if (!$roles['administer']) {
    if (array_key_exists(NULL, $request_ids)) {
      $access[NULL] = FALSE;
      $processed[NULL] = FALSE;
      return FALSE;
    }

    foreach ($request_ids as $request_id) {
      $access[$request_id] = FALSE;
      $processed[$request_id] = FALSE;
    }

    return $request_ids;
  }

  // deny delete access to already deleted requests.
  try {
    $query = db_select('mfcs_requests', 'mer');

    $query->addField('mer', 'id', 'id');

    $query->condition('mer.id', $request_ids, 'IN');
    $query->condition('mer.status', MFCS_REQUEST_STATUS_DELETED);

    $access_results = $query->execute()->fetchAllAssoc('id');

    if (is_array($access_results)) {
      $request_ids_copy = $request_ids;
      foreach ($request_ids_copy as $id => $value) {
        if (array_key_exists($id, $access_results)) {
          $processed[$id] = FALSE;
          $access[$id] = FALSE;
          unset($request_ids[$id]);
        }
        else {
          $processed[$id] = TRUE;
          $access[$id] = TRUE;
          unset($request_ids[$id]);
        }
      }
      unset($request_ids_copy);

      // looks like all of the requests have been processed, so return.
      if (empty($request_ids)) {
        return TRUE;
      }
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
