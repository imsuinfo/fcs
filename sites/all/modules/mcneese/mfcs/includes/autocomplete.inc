<?php

/**
 * @file
 * Defines McNeese facilities use autocomplete json functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Generates the JS array for user id autocomplete fields.
 *
 * This pulls the first 9 results that match the specified string.
 *
 * @param string $string
 *   A string to match node titles against.
 *
 * @return array
 *   An array of string matches.
 */
function mfcs_event_reviewers_0_autocomplete_user_id($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    cf_error::not_string('string');
    drupal_json_output(array());
    return;
  }

  $s = trim($string);

  $ilike = 'LIKE';
  if (db_driver() == 'pgsql') {
    $ilike = 'ILIKE';
  }

  try {
    $query = db_select('users', 'u');
    $query->innerjoin('field_data_field_user_first_name', 'fdfufn', 'u.uid = fdfufn.entity_id AND u.uid = fdfufn.entity_id AND fdfufn.delta = 0');
    $query->innerjoin('field_data_field_user_last_name', 'fdfuln', 'u.uid = fdfuln.entity_id AND u.uid = fdfuln.entity_id AND fdfufn.entity_id = fdfuln.entity_id AND fdfuln.delta = 0');

    $query->addField('u', 'uid', 'user_id');
    $query->addField('u', 'name', 'name');

    $query->addField('fdfufn', 'field_user_first_name_value', 'first_name');
    $query->addField('fdfuln', 'field_user_last_name_value', 'last_name');

    $or = db_or();
    $or->condition('u.name', '%' . db_like($s) . '%', $ilike);
    $or->condition('fdfufn.field_user_first_name_value', '%' . db_like($s) . '%', $ilike);
    $or->condition('fdfuln.field_user_last_name_value', '%' . db_like($s) . '%', $ilike);
    $query->condition($or);

    $query->condition('u.status', 0, '<>');
    $query->condition('u.uid', 1, '>');

    $query->orderBy('name');
    $query->orderBy('last_name');
    $query->orderBy('first_name');

    $query->range(0, 9);

    $results = $query->execute()->fetchAll();
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);
    drupal_json_output(array());
    return;
  }

  $matches = array();
  foreach ($results as $result) {
    $value = $result->name . ' (' . $result->last_name . ', ' . $result->first_name . ') [id: ' . $result->user_id . ']';
    $matches[$value] = check_plain($value);
  }

  drupal_json_output($matches);
}

/**
 * Generates the JS array for reviewer classification autocomplete fields.
 *
 * This pulls the first 9 results that match the specified string.
 *
 * @param string $string
 *   A string to match node titles against.
 *
 * @return array
 *   An array of string matches.
 */
function mfcs_event_reviewers_0_autocomplete_reviewer_classification($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    cf_error::not_string('string');
    drupal_json_output(array());
    return;
  }

  $s = trim($string);

  $ilike = 'LIKE';
  if (db_driver() == 'pgsql') {
    $ilike = 'ILIKE';
  }

  try {
    $query = db_select('taxonomy_term_data', 'ttd');

    $query->addField('ttd', 'tid', 'tid');
    $query->addField('ttd', 'name', 'name');

    $query->condition('ttd.vid', 17);
    $query->condition('ttd.name', '%' . db_like($s) . '%', $ilike);

    $query->orderBy('name');

    $query->range(0, 9);

    $results = $query->execute()->fetchAll();
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);
    drupal_json_output(array());
    return;
  }

  $matches = array();
  foreach ($results as $result) {
    $value = $result->name . ' [id: ' . $result->tid . ']';
    $matches[$value] = check_plain($value);
  }

  drupal_json_output($matches);
}

/**
 * Generates the JS array for event coordinator name autocomplete fields.
 *
 * This pulls the first 9 results that match the specified string.
 *
 * @param string $string
 *   A string to match node titles against.
 *
 * @return array
 *   An array of string matches.
 */
function mfcs_event_edit_0_autocomplete_event_coordinator_name($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    cf_error::not_string('string');
    drupal_json_output(array());
    return;
  }

  $s = trim($string);

  $ilike = 'LIKE';
  if (db_driver() == 'pgsql') {
    $ilike = 'ILIKE';
  }

  try {
    $query = mfcs_autocomplete_build_event_coordinator_query();

    $db_string = db_like($string);
    $or = db_or();
    $or->condition('fdfuln.field_user_last_name_value', '%' . $db_string . '%', $ilike);
    $or->condition('fdfufn.field_user_first_name_value', '%' . $db_string . '%', $ilike);
    $or->condition('u.name', '%' . $db_string . '%', $ilike);
    $or->condition('u.mail', '%' . $db_string . '%', $ilike);
    $query->condition($or);

    $query->range(0, 9);

    $results = $query->execute()->fetchAll();
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);
    drupal_json_output(array());
    return;
  }

  $matches = array();
  foreach ($results as $result) {
    $name = "";
    if (!empty($result->last_name)) {
      $name .= $result->last_name;
    }

    if (!empty($result->last_name)) {
      if (!empty($name)) {
        $name .= ", ";
      }
      $name .= $result->first_name;
    }

    $name_value = $name;

    if (!empty($name)) {
      $name = '"' . $name . '" ';
    }

    $value = $name . $result->email;
    $matches[$name_value] = check_plain($value);
  }

  drupal_json_output($matches);
}

/**
 * Generates the JS array for event coordinator email autocomplete fields.
 *
 * This pulls the first 9 results that match the specified string.
 *
 * @param string $string
 *   A string to match node titles against.
 *
 * @return array
 *   An array of string matches.
 */
function mfcs_event_edit_0_autocomplete_event_coordinator_email($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    cf_error::not_string('string');
    drupal_json_output(array());
    return;
  }

  $s = trim($string);

  $ilike = 'LIKE';
  if (db_driver() == 'pgsql') {
    $ilike = 'ILIKE';
  }

  try {
    $query = mfcs_autocomplete_build_event_coordinator_query();

    $db_string = db_like($string);
    $or = db_or();
    $or->condition('fdfuln.field_user_last_name_value', '%' . $db_string . '%', $ilike);
    $or->condition('fdfufn.field_user_first_name_value', '%' . $db_string . '%', $ilike);
    $or->condition('u.name', '%' . $db_string . '%', $ilike);
    $or->condition('u.mail', '%' . $db_string . '%', $ilike);
    $query->condition($or);

    $query->range(0, 9);

    $results = $query->execute()->fetchAll();
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);
    drupal_json_output(array());
    return;
  }

  $matches = array();
  foreach ($results as $result) {
    $name = "";
    if (!empty($result->last_name)) {
      $name .= $result->last_name;
    }

    if (!empty($result->last_name)) {
      if (!empty($name)) {
        $name .= ", ";
      }
      $name .= $result->first_name;
    }

    if (!empty($name)) {
      $name = '"' . $name . '" ';
    }

    $value = $name . $result->email;
    $matches[$result->email] = check_plain($value);
  }

  drupal_json_output($matches);
}

/**
 * Generates the JS array for event coordinator email autocomplete fields.
 *
 * This pulls the first 9 results that match the specified string.
 *
 * @param string $string
 *   A string to match node titles against.
 *
 * @return array
 *   An array of string matches.
 */
function mfcs_event_edit_0_autocomplete_event_coordinator_phone($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    cf_error::not_string('string');
    drupal_json_output(array());
    return;
  }

  $s = trim($string);

  $ilike = 'LIKE';
  if (db_driver() == 'pgsql') {
    $ilike = 'ILIKE';
  }

  try {
    $query = mfcs_autocomplete_build_event_coordinator_query();

    $query->innerjoin('field_data_field_user_phone_number', 'fdfupn', 'u.uid = fdfupn.entity_id AND fdfupn.delta = 0');
    $query->addField('fdfupn', 'field_user_phone_number_value', 'phone_number');

    $db_string = db_like($string);
    $or = db_or();
    $or->condition('fdfuln.field_user_last_name_value', '%' . $db_string . '%', $ilike);
    $or->condition('fdfufn.field_user_first_name_value', '%' . $db_string . '%', $ilike);
    $or->condition('u.name', '%' . $db_string . '%', $ilike);
    $or->condition('u.mail', '%' . $db_string . '%', $ilike);
    $query->condition($or);

    $query->range(0, 9);

    $results = $query->execute()->fetchAll();
  }
  catch (Exception $e) {
    cf_error::on_query_execution($e);
    drupal_json_output(array());
    return;
  }

  $matches = array();
  foreach ($results as $result) {
    if (empty($result->phone_number)) {
      continue;
    }

    $phone = (string) $result->phone_number;

    $name = "";
    if (!empty($result->last_name)) {
      $name .= $result->last_name;
    }

    if (!empty($result->last_name)) {
      if (!empty($name)) {
        $name .= ", ";
      }
      $name .= $result->first_name;
    }

    if (!empty($name)) {
      $name = '"' . $name . '" ';
    }

    $value = $name . $result->email;
    $matches[$phone] = check_plain($value);
  }

  drupal_json_output($matches);
}

/**
 * Builds the Event Coordinator query.
 *
 * This should be called inside of a try {} catch {}.
 *
 * @return object|false
 *   The build query object or FALSE on error;
 */
function mfcs_autocomplete_build_event_coordinator_query() {
  $query = db_select('users', 'u');
  $query->innerjoin('field_data_field_user_first_name', 'fdfufn', 'u.uid = fdfufn.entity_id AND u.uid = fdfufn.entity_id AND fdfufn.delta = 0');
  $query->innerjoin('field_data_field_user_last_name', 'fdfuln', 'u.uid = fdfuln.entity_id AND u.uid = fdfuln.entity_id AND fdfufn.entity_id = fdfuln.entity_id AND fdfuln.delta = 0');

  $query->addField('u', 'uid', 'user_id');
  $query->addField('u', 'name', 'name');
  $query->addField('u', 'mail', 'email');

  $query->addField('fdfufn', 'field_user_first_name_value', 'first_name');
  $query->addField('fdfuln', 'field_user_last_name_value', 'last_name');

  $query->condition('u.status', 0, '<>');
  $query->condition('u.uid', 1, '>');

  $query->o