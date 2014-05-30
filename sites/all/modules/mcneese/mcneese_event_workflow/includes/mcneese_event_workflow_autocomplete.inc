<?php

/**
 * @file
 * Defines McNeese event workflow autocomplete json functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Generates the JS array for user id autocomplete fields.
 *
 * This pulls the first 9 results that match the specified string.
 *
 * @param string $string
 *   A string to match node titles against.
 * @return array
 *   An array of string matches.
 */
function mcneese_event_workflow_event_reviewers_0_autocomplete_user_id($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    if (class_exists('cf_error')) {
      cf_error::not_string('string');
    }

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

    $query->orderBy('name');
    $query->orderBy('last_name');
    $query->orderBy('first_name');

    $query->range(0, 9);

    $executed = $query->execute();
    $results = $executed->fetchAll();
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
 * @return array
 *   An array of string matches.
 */
function mcneese_event_workflow_event_reviewers_0_autocomplete_reviewer_classification($string) {
  if (function_exists('devel_menu')) {
    $GLOBALS['devel_shutdown'] = FALSE;
  }

  if (!is_string($string)) {
    if (class_exists('cf_error')) {
      cf_error::not_string('string');
    }

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

    $executed = $query->execute();
    $results = $executed->fetchAll();
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
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
