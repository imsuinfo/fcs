<?php

/**
 * @file
 * Defines McNeese facilities use logic defragment functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

define('MFCS_DEFRAG_USER_STRATEGY_NONE', 0);

// randomization, first 100 reserved, blacklist and whitelist supported.
define('MFCS_DEFRAG_USER_STRATEGY_1', 1);

// randomization with disabled accounts the higher numbers, first 100 reserved, blacklist and whitelist supported.
define('MFCS_DEFRAG_USER_STRATEGY_2', 2);

/**
 * Performs defragmentation against user ids.
 *
 * @param int $strategy
 *   (optional) The strategy integer.
 * @param array $dont_defrag
 *   (optional) An array of user ids whose ids should not be changed during the operations.
 *   This overrides the $defrag user ids.
 * @param array $defrag
 *   (optional) An array of user ids to defrag.
 *   When an empty array, all user ids found within the database are loaded.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_defragment_defrag_user_ids($strategy = MFCS_DEFRAG_USER_STRATEGY_1, $dont_defrag = array(), $defrag = array()) {
  if (!is_int($strategy)) {
    cf_error::invalid_integer('strategy');
    return FALSE;
  }

  if (!is_array($dont_defrag)) {
    cf_error::invalid_integer('dont_defrag');
    return FALSE;
  }

  if (!is_array($defrag)) {
    cf_error::invalid_integer('defrag');
    return FALSE;
  }

  // require appropriate access to call this function.
  if (!mfcs_defragment_access()) {
    return FALSE;
  }

  global $user;

  $editing_user_id = NULL;

  $instance = mfcs_instance();

  $all_users = array();
  $total_users = 0;
  try {
    $total_users = (int) db_query('select count(uid) from users')->fetchField();
    if ($total_users < 2) {
      // < 2 is used because the two accounts (anonymous and administrator) are required by drupal.
      return FALSE;
    }

    $query = db_select('users', 'u');
    $query->addField('u', 'uid', 'user_id');
    $query->addField('u', 'status', 'status');

    if (!empty($defrag)) {
      $query->condition('u.uid', $defrag, 'IN');
    }

    if (!empty($dont_defrag)) {
      $query->condition('u.uid', $dont_defrag, 'NOT IN');
    }

    $results = $query->execute()->fetchAll();
    if (empty($results)) {
      return FALSE;
    }

    foreach ($results as $result) {
      $all_users[(int) $result->user_id] = array(
        'user_id' => (int) $result->user_id,
        'status' => (int) $result->status,
      );
    }
    unset($result);
    unset($results);

    ksort($all_users);
  }
  catch (Error $e) {
    cf_error::on_query_execution($e);

    return FALSE;
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);

    return FALSE;
  }

  $success = FALSE;
  $new_user_ids = array();
  if ($strategy === MFCS_DEFRAG_USER_STRATEGY_1 || $strategy === MFCS_DEFRAG_USER_STRATEGY_2) {
    // This strategy is as follows:
    // First 0->99 integers are reserved for system user.
    // All disabled/locked/deleted accounts will be moved to the end of the know id list (giving them large keys).
    // - End of the known id list is not the maximum integer but is instead derived from the total number of user accounts.
    // All other non-system/non-reserved accounts will be randomly placed in the lower id list.
    $highest_integer = $total_users;

    // Define the hardcoded/known system user accounts.
    $system_accounts = array(
      0 => 0, // anonymous
      1 => 1, // administrator
    );

    foreach ($system_accounts as $system_account) {
      unset($all_users[$system_account]);
    }

    if ($strategy === MFCS_DEFRAG_USER_STRATEGY_1) {
      $shuffled_users = $all_users;
      if (!shuffle($shuffled_users)) {
        // @todo: report error about shuffle failing.
        return FALSE;
      }
    }
    elseif ($strategy === MFCS_DEFRAG_USER_STRATEGY_2) {
      $shuffled_users_enabled = array();
      $shuffled_users_disabled = array();

      foreach ($all_users as $user_columns) {
        if ($user_columns['status'] == 0) {
          $shuffled_users_disabled[$user_columns['user_id']] = $user_columns;
        }
        else {
          $shuffled_users_enabled[$user_columns['user_id']] = $user_columns;
        }
      }
      unset($user_columns);

      if (!shuffle($shuffled_users_enabled)) {
        // @todo: report error about shuffle failing.
        return FALSE;
      }

      if (!shuffle($shuffled_users_disabled)) {
        // @todo: report error about shuffle failing.
        return FALSE;
      }

      $shuffled_users = array_merge($shuffled_users_enabled, $shuffled_users_disabled);
      unset($shuffled_users_enabled);
      unset($shuffled_users_disabled);
    }

    $count = 100;
    $unused = $highest_integer;
    foreach ($shuffled_users as $user_columns) {
      if ($user_columns['user_id'] == $count) {
        $new_user_ids[$user_columns['user_id']] = $user_columns['user_id'];
        $count++;
        continue;
      }

      if (array_key_exists($count, $all_users)) {
        $shuffled_users[$count] = $unused;
        $unused = $user_columns['user_id'];
      }

      $new_user_ids[$count] = $user_columns['user_id'];
      $count++;
    }
    unset($user_columns);
    unset($shuffled_users);

    ksort($new_user_ids);
  }
  unset($all_users);

  // re-organize the keys so that they may be updated one at a time without causing conflicts.
  ksort($new_user_ids);
  $new_user_ids = array_reverse($new_user_ids, TRUE);

  // the array key is the new id and the array value is the old id.
  $transaction = db_transaction();
  try {
    // completely lock the table to ensure nothing changes during operation (lock gets released on transaction commit).
    $locked = db_query('lock table {users} in access exclusive mode');
    if ($locked === FALSE) {
      // @todo provide an error message.
      return FALSE;
    }
    unset($locked);

    // Instead of performing a more intelligent heuristic for updating user accounts without user id conflicts, just make two updates per user id.
    // The biggest flaw in this simple design is that the available user ids must be ((total users) * 2) + reserved users + 1) available space.
    $max_user_id = db_query('select max(uid) from {users}')->fetchField();
    if ($max_user_id === FALSE) {
      $transaction->rollback();
      // @todo: provide an error message.
      return FALSE;
    }
    $max_user_id = ((int) $max_user_id) + 1;

    // pass 1: move user ids to (desired id + max id).
    foreach ($new_user_ids as $new_id => $old_id) {
      $query = db_update('users');
      $query->fields(array('uid' => $max_user_id + $new_id));
      $query->condition('uid', $old_id);
      $query->execute();

      // make sure that the current user id points to the correct (updated) user id instead of the old one.
      if (is_null($editing_user_id) && $old_id == $user->uid) {
        $editing_user_id = $new_id;
      }
    }
    unset($new_id);
    unset($old_id);

    if (is_null($editing_user_id)) {
      $editing_user_id = $user->uid;
    }

    // pass 2: move user ids from (desired id + max id) to (desired id).
    foreach ($new_user_ids as $new_id => $old_id) {
      $query = db_update('users');
      $query->fields(array('uid' => $new_id));
      $query->condition('uid', $max_user_id + $new_id);
      $query->execute();
    }
    unset($new_id);
    unset($old_id);
    unset($max_user_id);

    $log_fields = array(
      'from_ids' => '{' . implode(', ', $new_user_ids) . '}',
      'to_ids' => '{' . implode(', ', array_keys($new_user_ids)) . '}',
      'user_id' => $editing_user_id,
      'updated' => $instance,
    );

    $query = db_insert('mfcs_log_user_defragment');
    $query->fields($log_fields);
    $query->execute();

    unset($log_fields);
    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    return FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    return FALSE;
  }

  return $success;
}

/**
 * Revert a defragmentation operation based on a specific log entry.
 *
 * @param int $log_entry_id
 *   ID of the log entry to revert the user id defrag changes.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_defragment_defrag_user_ids_revert($log_entry_id) {
  if (!is_int($log_entry_id)) {
    cf_error::invalid_integer('log_entry_id');
    return FALSE;
  }

  // require appropriate access to call this function.
  if (!mfcs_defragment_access()) {
    return FALSE;
  }

  global $user;

  $editing_user_id = NULL;

  $instance = mfcs_instance();

  $transaction = db_transaction();
  try {
    $query = db_select('mfcs_log_user_defragment', 'mlud');
    $query->addField('mlud', 'from_ids', 'from_ids');
    $query->addField('mlud', 'to_ids', 'to_ids');
    $query->condition('mlud.id', $log_entry_id);

    $entry = $query->execute()->fetchAssoc();
    if (!is_array($entry)) {
      return FALSE;
    }
    unset($query);

    $entry['to_ids'] = str_replace('{', '', $entry['to_ids']);
    $entry['to_ids'] = str_replace('}', '', $entry['to_ids']);
    $entry['from_ids'] = str_replace('{', '', $entry['from_ids']);
    $entry['from_ids'] = str_replace('}', '', $entry['from_ids']);

    $to_ids = explode(',', $entry['to_ids']);
    $from_ids = explode(',', $entry['from_ids']);

    // completely lock the table to ensure nothing changes during operation (lock gets released on transaction commit).
    $locked = db_query('lock table {users} in access exclusive mode');
    if ($locked === FALSE) {
      // @todo provide an error message.
      return FALSE;
    }
    unset($locked);

    // Instead of performing a more intelligent heuristic for updating user accounts without user id conflicts, just make two updates per user id.
    // The biggest flaw in this simple design is that the available user ids must be ((total users) * 2) + reserved users + 1) available space.
    $max_user_id = db_query('select max(uid) from {users}')->fetchField();
    if ($max_user_id === FALSE) {
      $transaction->rollback();
      // @todo: provide an error message.
      return FALSE;
    }
    $max_user_id = ((int) $max_user_id) + 1;

    // pass 1: move user ids to (desired id + max id).
    foreach ($to_ids as $row_number => $to_id) {
      $query = db_update('users');
      $query->fields(array('uid' => $max_user_id + $from_ids[$row_number]));
      $query->condition('uid', $to_id);
      $query->execute();

      // make sure that the current user id points to the correct (updated) user id instead of the old one.
      if (is_null($editing_user_id) && $to_id == $user->uid) {
        $editing_user_id = $from_ids[$row_number];
      }
    }
    unset($row_number);
    unset($to_id);
    unset($to_ids);

    if (is_null($editing_user_id)) {
      $editing_user_id = $user->uid;
    }

    // pass 2: move user ids from (desired id + max id) to (desired id).
    foreach ($from_ids as $from_id) {
      $query = db_update('users');
      $query->fields(array('uid' => $from_id));
      $query->condition('uid', $max_user_id + $from_id);
      $query->execute();
    }
    unset($new_id);
    unset($old_id);
    unset($from_ids);
    unset($max_user_id);

    $log_fields = array(
      'from_ids' => '{' . $entry['to_ids'] . '}',
      'to_ids' => '{' . $entry['from_ids'] . '}',
      'user_id' => $editing_user_id,
      'updated' => $instance,
    );

    $query = db_insert('mfcs_log_user_defragment');
    $query->fields($log_fields);
    $query->execute();

    unset($log_fields);
    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    return FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    return FALSE;
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
