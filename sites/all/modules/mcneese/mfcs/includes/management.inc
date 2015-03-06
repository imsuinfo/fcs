<?php

/**
 * @file
 * Defines McNeese facilities use management functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Searches through mfcs-specific user data.
 *
 * mfcs-specific columns:
 *   'problems' is a unsigned 64-bit integer containging bitwise flags.
 *
 * @param array $conditions
 *   An array containing select conditions, such as:
 *   - 'mu.user_id': user id (uid).
 *   - 'mu.problems': problem flags.
 *   The each key may be an array such as:
 *     - array('value' => '1%', 'operator' => 'LIKE')
 *     - array('value' => array(0, 1, 3), 'operator' => 'IN')
 *     - array('value' => 1, '=>')
 *   This also supports a recursive and/or grouping in the following format:
 *   'and_{unique_id}', 'or_{unique_id}' where {unique_id} is some arbitrary
 *   string or number to ensure that the row is unique.
 * @param array $options
 *   (optional) When specified, representing an array containing:
 *     'join': When TRUE, the drupal user table is joined for the table.
 *             Only select portions of the user table is joined. This
 *             also joins custom field tables for user first and last name.
 *             This is only joined to the submitter table column.
 *     'start': The starting location.
 *     'limit': The number of rows to select, aka: length.
 *     'count': If TRUE, an unfiltered count query is processed.
 *     'results': If FALSE, the main query is not performed.
 *     'sort': An array of database sorting columns and their sort orders.
 *     'sensitive': TRUE for case sensitive and FALSE otherwise.
 *     'extra_fields': An array of extra fields that should be loaded.
 *                     The array will have a key of the table name and a value
 *                     of an array. The value array will contain an 'alias'
 *                     string, an 'on' string, a 'join' string, and a 'columns'
 *                     array.
 *     'distinct': Process the entire query as distinct.
 *     'distinct_id': Custom option to load mer.id as distinct. This overrides
 *                    'distinct' when TRUE.
 *
 * @return array|false
 *   An array of objects containing the mfcs-specific user fields.
 *
 *   FALSE is returned on error.
 *
 * @see: mfcs_request_process_query_search_array()
 */
function mfcs_management_search_users($conditions = array(), $options = array()) {
  if (!is_array($conditions)) {
    cf_error::invalid_array('conditions');
    return FALSE;
  }

  if (!is_array($options)) {
    cf_error::invalid_array('options');
    return FALSE;
  }

  $join = NULL;
  if (isset($options['join']) && is_bool($options['join'])) {
    $join = $options['join'];
  }

  $start = 0;
  if (isset($options['start']) && is_numeric($options['start']) && $start >= 0) {
    $start = $options['start'];
  }

  $limit = 20;
  if (isset($options['limit']) && is_numeric($options['limit']) && $limit >= 0) {
    $limit = $options['limit'];
  }

  $count = FALSE;
  if (isset($options['count']) && is_bool($options['count'])) {
    $count = (bool) $options['count'];
  }

  $sort = NULL;
  if (isset($options['sort']) && is_array($options['sort'])) {
    $sort = $options['sort'];
  }

  $results = TRUE;
  if (isset($options['results']) && is_bool($options['results'])) {
    $results = $options['results'];
  }

  $distinct = FALSE;
  if (isset($options['distinct']) && is_bool($options['distinct'])) {
    $distinct = (bool) $options['distinct'];
  }

  $distinct_id = FALSE;
  if (isset($options['distinct_id']) && is_bool($options['distinct_id'])) {
    $distinct_id = (bool) $options['distinct_id'];
    if ($distinct_id) {
      $distinct = FALSE;
    }
  }

  $return = array(
    'results' => NULL,
    'count' => NULL,
  );

  if (!$results) {
    $return['results'] = array();
  }

  $sensitive = FALSE;
  if (isset($options['sensitive']) && is_bool($options['sensitive'])) {
    $sensitive = $options['sensitive'];
  }

  $extra_fields = array();
  if (isset($options['extra_fields']) && is_array($options['extra_fields'])) {
    $extra_fields = $options['extra_fields'];
  }

  $user = cf_current_user();

  $s = 'i';
  $like = 'ILIKE';
  if ($sensitive) {
    $s = '';
    $like = 'LIKE';
  }

  $already_joined_count = 0;
  $already_joined = array();

  $already_added = array();

  $found = array();
  try {
    $outermost_and = NULL;

    $query = db_select('mfcs_users', 'mu');

    $already_joined['mfcs_users'] = 'mu';

    if ($join) {
      $query->innerJoin('users', 'u', 'mu.user_id = u.uid');
      $query->innerJoin('field_data_field_user_first_name', 'fdfufn', 'mu.user_id = fdfufn.entity_id AND fdfufn.delta = 0');
      $query->innerJoin('field_data_field_user_last_name', 'fdfuln', 'mu.user_id = fdfuln.entity_id AND fdfuln.delta = 0');

      $already_joined['users'] = 'u';
      $already_joined['field_data_field_user_first_name'] = 'fdfufn';
      $already_joined['field_data_field_user_last_name'] = 'fdfuln';
    }

    $count_query = NULL;
    if ($count) {
      $count_query = clone($query);
      $count_query->addExpression('count(mu.user_id)');
    }

    if ($distinct) {
      $query->distinct();
    }

    if ($distinct_id) {
      $query->distinct(array('field' => 'mu.user_id', 'alias' => 'user_id'));
    }
    else {
      $query->addField('mu', 'user_id', 'user_id');
    }

    $query->addField('mu', 'problems', 'problems');
    $query->addField('mu', 'date', 'date');

    if ($join) {
      $query->addField('u', 'name', 'name');
      $query->addField('u', 'mail', 'email');
      $query->addField('fdfufn', 'field_user_first_name_value', 'first_name');
      $query->addField('fdfuln', 'field_user_last_name_value', 'last_name');
    }

    // add custom fields to the select results as requested by the caller.
    if (!empty($extra_fields)) {
      foreach ($extra_fields as $extra_table_name => $extra_values) {
        if (!array_key_exists('alias', $extra_values) || !array_key_exists('join', $extra_values) || !array_key_exists('columns', $extra_values)) {
          continue;
        }

        if (array_key_exists($extra_table_name, $already_joined)) {
          continue;
        }

        if ($extra_values['join'] == 'inner') {
          $query->innerJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);

          if ($count) {
            $count_query->innerJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);
          }
        }
        elseif ($extra_values['join'] == 'left') {
          $query->leftJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);

          if ($count) {
            $count_query->leftJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);
          }
        }
        elseif ($extra_values['join'] == 'right') {
          $query->rightJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);

          if ($count) {
            $count_query->rightJoin($extra_table_name, $extra_values['alias'], $extra_values['on']);
          }
        }

        $already_joined[$extra_table_name] = $extra_values['alias'];
        foreach ($extra_values['columns'] as $column_external_name => $column_internal_name) {
          if (array_key_exists($column_external_name, $already_added)) {
            continue;
          }

          $query->addField($extra_values['alias'], $column_internal_name, $column_external_name);
          $already_added[$column_external_name] = $extra_values['alias'] . '.' . $column_internal_name;
        }
      }
    }

    if (!empty($conditions)) {
      $operand = db_and();
      $result = mfcs_request_process_query_search_array($query, $count_query, $operand, $already_joined, $already_joined_count, $already_added, $conditions, 1, $like);

      if ($result) {
        if (is_null($outermost_and)) {
          $outermost_and = db_and();
        }
        $outermost_and->condition($operand);
      }

      unset($operand);
    }

    if ($limit > 0) {
      $query->range($start, $limit);
    }

    // do not include deleted users.
    if (is_null($outermost_and)) {
      $outermost_and = db_and();
    }
    $outermost_and->condition('u.status', 0, '<>');

    $query->condition($outermost_and);
    if ($count) {
      $count_query->condition($outermost_and);
    }

    if (is_null($sort)) {
      // default to sorting/ordering by request id.
      if ($distinct || $distinct_id) {
        // when a query is distinct, the order by values must be defined in the select columns.
        $query->orderBy('user_id' , 'DESC');
      }
      else {
        $query->orderBy('mu.user_id', 'DESC');
      }
    }
    else {
      if (!empty($sort)) {
        // When selecting distinct on a specific column/alias, it must be specified in order by if order by is used.
        // And distinct field must be the first column to be ordered by.
        if ($distinct_id) {
          if (array_key_exists('user_id', $sort) && !empty($sort['user_id'])) {
            $query->orderBy('user_id' , $sort['user_id']);
          }
          else {
            $query->orderBy('user_id' , 'DESC');
          }
        }

        foreach ($sort as $sort_name => $sort_value) {
          if ($sort_name == 'user_id') {
            // when a query is distinct, the order by values must be defined in the select columns.
            if ($distinct_id) {
              continue;
            }
            elseif ($distinct) {
              $query->orderBy('user_id' , is_null($sort_value) ? 'ASC' : $sort_value);
            }
            else {
              $query->orderBy('mu.user_id', is_null($sort_value) ? 'ASC' : $sort_value);
            }
          }
          elseif (!empty($sort_name)) {
            $query->orderBy($sort_name, $sort_value);
          }
        }
      }
    }

    if ($count) {
      $return['count'] = $count_query->execute()->fetchField();
    }

    if ($results) {
      $return['results'] = $query->execute()->fetchAllAssoc('user_id');
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $return;
}

/**
 * Returns the problems taxonomy.
 *
 * @return array
 *   An array of problems by id, each with a human readable label.
 */
function mfcs_management_get_problems_taxonomy() {
  $labels = array();

  $labels[MFCS_USER_PROBLEM_SYSTEM] = "Core System Problem";
  $labels[MFCS_USER_PROBLEM_MFCS] = "Facilities Use Problem";
  $labels[MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE] = "Banner Unavailable on Create";
  $labels[MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE] = "Banner Unavailable on Login";
  $labels[MFCS_USER_PROBLEM_ACCOUNT_LOGIN_PROBLEMS_MULTIPLE] = "Multiple Login Attempt Problems";
  $labels[MFCS_USER_PROBLEM_FORM_ERROR_USER] = "Core System Form Errors";
  $labels[MFCS_USER_PROBLEM_FORM_ERROR_USER] = "Facilities Use Form Errors";

  return $labels;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
