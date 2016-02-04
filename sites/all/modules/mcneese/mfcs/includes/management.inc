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
 * Searches through a specific table.
 *
 * Unlike the specific functions like mfcs_management_search_users(), this does
 * not perform any joining of special tables.
 *
 * @param string $table_name
 *   The name of the database table to select/search.
 * @param string $table_alias
 *   The alias of the database table to select/search.
 * @param string $primary_key
 *   This is the primary key for the table so the distinct and sorting can be
 *   properly used in the SQL Select statements. This is often 'id' or
 *   'request_id'.
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
 *     'fields: An array containing the field alias as the key with the value
 *              being the field name. This is only for fields on the table
 *              defined by table_name and table_alias. This does not join
 *              tables, to join tables instead use 'extra_fields'.
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
 *     'extra_fields2': The same as 'extra_fields', only it was added because
 *                      the current design does not support the same table
 *                      being joined multiple times. This will add the extra
 *                      table despite being added to the $already_joined list.
 *                      Use this only when necessary given that this is
 *                      essentially a workaround.
 *     'distinct': Process the entire query as distinct.
 *     'distinct_id': Custom option to load mer.id as distinct. This overrides
 *                    'distinct' when TRUE.
 *     'associative': When TRUE (default), performs a fetchAllAssoc() against the primary key.
 *                    When FALSE, performs a fetchAll().
 *
 * @return array|false
 *   An array of objects containing the mfcs-specific user fields.
 *
 *   FALSE is returned on error.
 *
 * @see: mfcs_request_process_query_search_array()
 */
function mfcs_management_search_table($table_name, $table_alias, $primary_key, $conditions = array(), $options = array()) {
  if (!is_string($table_name) || empty($table_name)) {
    cf_error::invalid_string('table_name');
    return FALSE;
  }

  if (!is_string($table_alias) || empty($table_alias)) {
    cf_error::invalid_string('table_alias');
    return FALSE;
  }

  if (!is_string($primary_key) || empty($primary_key)) {
    cf_error::invalid_string('primary_key');
    return FALSE;
  }

  if (!is_array($conditions)) {
    cf_error::invalid_array('conditions');
    return FALSE;
  }

  if (!is_array($options)) {
    cf_error::invalid_array('options');
    return FALSE;
  }

  $fields = array();
  if (isset($options['fields']) && is_array($options['fields'])) {
    $fields = $options['fields'];
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

  $associative = TRUE;
  if (isset($options['associative']) && is_bool($options['associative'])) {
    $associative = (bool) $options['associative'];
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

  $extra_fields2 = array();
  if (isset($options['extra_fields2']) && is_array($options['extra_fields2'])) {
    $extra_fields2 = $options['extra_fields2'];
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

    $query = db_select($table_name, $table_alias);

    $already_joined[$table_alias] = $table_name;

    $count_query = NULL;
    if ($count) {
      $count_query = clone($query);

      if ($distinct_id) {
        $count_query->addExpression('count(distinct ' . $table_alias . '.' . $primary_key . ')');
      }
      else {
        $count_query->addExpression('count(' . $table_alias . '.' . $primary_key . ')');
      }
    }

    if ($distinct) {
      $query->distinct();
    }

    if ($distinct_id) {
      $query->distinct(array('field' => $table_alias . '.' . $primary_key, 'alias' => $primary_key));
    }
    else {
      $query->addField($table_alias, $primary_key, $primary_key);
    }

    $already_added[$primary_key] = $table_alias . '.' . $primary_key;

    if (!empty($fields)) {
      foreach ($fields as $field_alias => $field) {
        if (!is_string($field) || empty($field)) {
          continue;
        }

        if (array_key_exists($field_alias, $already_added)) {
          continue;
        }

        $query->addField($table_alias, $field, $field_alias);
        $already_added[$field_alias] = $table_alias . '.' . $field;
      }
      unset($field);
    }

    // add custom fields to the select results as requested by the caller.
    if (!empty($extra_fields)) {
      foreach ($extra_fields as $extra_table_name => $extra_values) {
        if (!array_key_exists('alias', $extra_values) || !array_key_exists('join', $extra_values) || !array_key_exists('columns', $extra_values)) {
          continue;
        }

        if (array_key_exists($extra_values['alias'], $already_joined)) {
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

        $already_joined[$extra_values['alias']] = $extra_table_name;
        foreach ($extra_values['columns'] as $column_external_name => $column_internal_name) {
          if (array_key_exists($column_external_name, $already_added)) {
            continue;
          }

          $query->addField($extra_values['alias'], $column_internal_name, $column_external_name);
          $already_added[$column_external_name] = $extra_values['alias'] . '.' . $column_internal_name;
        }
      }
    }

    // perform additional joins.
    if (!empty($extra_fields2)) {
      foreach ($extra_fields2 as $extra_table_name => $extra_values) {
        if (!array_key_exists('alias', $extra_values) || !array_key_exists('join', $extra_values) || !array_key_exists('columns', $extra_values)) {
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

        $already_joined[$extra_values['alias']] = $extra_table_name;
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

    if (!is_null($outermost_and)) {
      $query->condition($outermost_and);

      if ($count) {
        $count_query->condition($outermost_and);
      }
    }

    if (is_null($sort)) {
      // default to sorting/ordering by the primary key.
      if ($distinct || $distinct_id) {
        // when a query is distinct, the order by values must be defined in the select columns.
        $query->orderBy($primary_key , 'DESC');
      }
      else {
        $query->orderBy($table_alias . '.' . $primary_key, 'DESC');
      }
    }
    else {
      if (!empty($sort)) {
        // When selecting distinct on a specific column/alias, it must be specified in order by if order by is used.
        // And distinct field must be the first column to be ordered by.
        if ($distinct_id) {
          if (array_key_exists($primary_key, $sort) && !empty($sort[$primary_key])) {
            $query->orderBy($primary_key, $sort[$primary_key]);
          }
          else {
            $query->orderBy($primary_key, 'DESC');
          }
        }

        foreach ($sort as $sort_name => $sort_value) {
          if ($sort_name == $primary_key) {
            // when a query is distinct, the order by values must be defined in the select columns.
            if ($distinct_id) {
              continue;
            }
            elseif ($distinct) {
              $query->orderBy($primary_key, is_null($sort_value) ? 'ASC' : $sort_value);
            }
            else {
              $query->orderBy($table_alias . '.' . $primary_key, is_null($sort_value) ? 'ASC' : $sort_value);
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
      if ($associative) {
        $return['results'] = $query->execute()->fetchAllAssoc($primary_key);
      }
      else {
        $return['results'] = $query->execute()->fetchAll();
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }

  return $return;
}

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
 *     'include_disabled': When TRUE will not limit the results to only enabled accounts.
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

  $enabled_accounts_only = TRUE;
  if (isset($options['include_disabled']) && is_bool($options['include_disabled'])) {
    $enabled_accounts_only = !($options['include_disabled']);
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

      $already_joined['u'] = 'users';
      $already_joined['fdfufn'] = 'field_data_field_user_first_name';
      $already_joined['fdfuln'] = 'field_data_field_user_last_name';
    }

    $count_query = NULL;
    if ($count) {
      $count_query = clone($query);

      if ($distinct_id) {
        $count_query->addExpression('count(distinct mu.user_id)');
      }
      else {
        $count_query->addExpression('count(mu.user_id)');
      }
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

    $already_added['user_id'] = 'mu.user_id';
    $already_added['problems'] = 'mu.problems';
    $already_added['date'] = 'mu.date';

    if ($join) {
      $query->addField('u', 'name', 'name');
      $query->addField('u', 'mail', 'email');
      $query->addField('fdfufn', 'field_user_first_name_value', 'first_name');
      $query->addField('fdfuln', 'field_user_last_name_value', 'last_name');

      $already_added['name'] = 'u.name';
      $already_added['email'] = 'u.mail';
      $already_added['first_name'] = 'fdfufn.first_name';
      $already_added['last_name'] = 'fdfuln.last_name';
    }

    // add custom fields to the select results as requested by the caller.
    if (!empty($extra_fields)) {
      foreach ($extra_fields as $extra_table_name => $extra_values) {
        if (!array_key_exists('alias', $extra_values) || !array_key_exists('join', $extra_values) || !array_key_exists('columns', $extra_values)) {
          continue;
        }

        if (array_key_exists($extra_values['alias'], $already_joined)) {
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

        $already_joined[$extra_values['alias']] = $extra_table_name;
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

    if ($enabled_accounts_only) {
      $outermost_and->condition('u.status', 0, '<>');
    }

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
  $labels[MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED] = "Unable to Assign/Unassign Reviewer Role";
  $labels[MFCS_USER_PROBLEM_ACCOUNT_DUPLICATES] = "Old/Duplicate Account";
  $labels[MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE] = "Disabled Account in Use";
  $labels[MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID] = "Invalid Banner ID";
  $labels[MFCS_USER_PROBLEM_FORM_ERROR_USER] = "Core System Form Errors";
  $labels[MFCS_USER_PROBLEM_FORM_ERROR_USER] = "Facilities Use Form Errors";

  return $labels;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
