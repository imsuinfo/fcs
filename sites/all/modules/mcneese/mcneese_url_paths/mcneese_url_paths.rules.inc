<?php

/**
 * @file
 * Rules integration file for McNeese State University Url Paths.
 */

/**
 * @addtogroup mcneese_url_paths
 * @{
 */

/**
 * Implements hook_rules_file_info().
 */
function mcneese_url_paths_rules_file_info() {
  $items = array();
  $items[] = 'mcneese_url_paths.rules';
  
  return $items;
}

/**
 * Implements hook_rules_condition_info() on behalf of the node module.
 */
function mcneese_url_paths_rules_condition_info() {
  $items = array();

  $items['mcneese_url_paths_compare_short_strings'] = array(
    'group' => t("McNeese"),
    'label' => t("Compare short strings"),
    'base' => 'mcneese_url_paths_compare_short_strings_condition',
    'parameter' => array(
      'first_string' => array(
        'type' => 'text',
        'label' => t("First String"),
      ),
      'second_string' => array(
        'type' => 'text',
        'label' => t("Second String"),
      ),
    ),
  );

  return $items;
}

/**
 * Implements hook_rules_action_info() on behalf of the node module.
 */
function mcneese_url_paths_rules_action_info() {
  $items = array();

  $items['mcneese_url_paths_create_short_string'] = array(
    'label' => t("Create Short String"),
    'group' => t("McNeese"),
    'base' => 'mcneese_url_paths_create_short_string_action',
    'parameter' => array(
      'long_string' => array(
        'type' => 'text',
        'label' => t("Long String"),
      ),
    ),
    'provides' => array(
      'short_string' => array(
        'type' => 'text',
        'label' => t("Short String"),
      ),
    ),
  );

  $items['mcneese_url_paths_create_node_alias'] = array(
    'label' => t("Create Node Alias"),
    'group' => t("McNeese"),
    'base' => 'mcneese_url_paths_create_node_alias_action',
    'parameter' => array(
      'node_id' => array(
        'type' => 'integer',
        'label' => t("Node ID"),
      ),
      'path_alias' => array(
        'type' => 'text',
        'label' => t("Path Alias"),
      ),
    ),
  );

  $items['mcneese_url_paths_create_node_alias_workbench'] = array(
    'label' => t("Create Node Alias (workbench)"),
    'group' => t("McNeese"),
    'base' => 'mcneese_url_paths_create_node_alias_workbench_action',
    'parameter' => array(
      'node_id' => array(
        'type' => 'integer',
        'label' => t("Node ID"),
      ),
      'access_id' => array(
        'type' => 'integer',
        'label' => t("Access ID"),
      ),
      'path_alias' => array(
        'type' => 'text',
        'label' => t("Path Alias"),
      ),
      'group_representative' => array(
        'type' => 'boolean',
        'label' => t("Group Representative"),
        'optional' => TRUE,
      ),
    ),
  );

  $items['mcneese_url_paths_delete_node_alias'] = array(
    'label' => t("Delete Node Alias"),
    'group' => t("McNeese"),
    'base' => 'mcneese_url_paths_delete_node_alias_action',
    'parameter' => array(
      'node_id' => array(
        'type' => 'integer',
        'label' => t("Node ID"),
      ),
      'path_alias' => array(
        'type' => 'text',
        'label' => t("Path Alias"),
      ),
    ),
  );

  $items['mcneese_url_paths_delete_node_alias_workbench'] = array(
    'label' => t("Delete Node Alias (workbench)"),
    'group' => t("McNeese"),
    'base' => 'mcneese_url_paths_delete_node_alias_workbench_action',
    'parameter' => array(
      'node_id' => array(
        'type' => 'integer',
        'label' => t("Node ID"),
      ),
      'access_id' => array(
        'type' => 'integer',
        'label' => t("Access ID"),
      ),
      'path_alias' => array(
        'type' => 'text',
        'label' => t("Path Alias"),
      ),
      'group_representative' => array(
        'type' => 'boolean',
        'label' => t("Group Representative"),
      ),
    ),
  );

  $items['mcneese_url_paths_delete_all_node_aliases'] = array(
    'label' => t("Delete All Node Aliases"),
    'group' => t("McNeese"),
    'base' => 'mcneese_url_paths_delete_all_node_aliases_action',
    'parameter' => array(
      'node_id' => array(
        'type' => 'integer',
        'label' => t("Node ID"),
      ),
    ),
  );

  return $items;
}

/**
 * Condition: compare short versions of two strings.
 *
 * $param string $first_string
 *   First title/string to compare.
 * $param string $second_string
 *   Second title/string to compare.
 *
 * @return bool
 *   TRUE/FALSE depending on whether the short strings match.
 */
function mcneese_url_paths_compare_short_strings_condition($first_string, $second_string) {
  $short_first = mcneese_url_paths_create_short_string($first_string);
  $short_second = mcneese_url_paths_create_short_string($second_string);

  return $short_first == $short_second;
}

/**
 * Action: create a short version of a given string.
 *
 * $param string $long_string
 *   Long string to shorten.
 *
 * @return array
 *   An array containing the short string.
 */
function mcneese_url_paths_create_short_string_action($long_string) {
  return array('short_string' => mcneese_url_paths_create_short_string($long_string));
}

/**
 * Action: create a node path alias.
 *
 * $param numeric $node_id
 *   The unique id representing the node.
 * $param string $path_alias
 *   The url path string to create the path alias to.
 */
function mcneese_url_paths_create_node_alias_action($node_id, $path_alias) {
  $handler = new mcneese_url_paths_node_handler();

  $result = $handler->set_node_id($node_id);

  if ($result) {
    $result = $handler->set_alias_path($path_alias);
  }

  if ($result) {
    $result = $handler->create_path_alias();
  }

  if ($result === FALSE) {
    watchdog('rules', "Failed to create normal path alias for node = $node_id, access_id = %access_id, and path_alias = '%path_alias'", array('%node_id' => $node_id, '%access_id' => $access_id, '%path_alias' => $path_alias), WATCHDOG_ERROR);
  }
}

/**
 * Action: create a node path alias based on workbench menu path.
 *
 * $param numeric $node_id
 *   The unique id representing the node.
 * $param numeric $access_id
 *   The unique id representing the workbench access id.
 *   This is the unique taxonomy id.
 * $param string $path_alias
 *   The url path string to create the path alias to.
 * $param bool $group_representative
 *   (optional) The url path string to create the path alias to.
 */
function mcneese_url_paths_create_node_alias_workbench_action($node_id, $access_id, $path_alias, $group_representative = FALSE) {
  $handler = new mcneese_url_paths_node_handler();

  $result = $handler->set_group_representative($group_representative);

  if ($result) {
    $result = $handler->set_node_id($node_id);
  }

  if ($result) {
    $result = $handler->set_access_id($access_id);
  }

  if ($result) {
    $result = $handler->set_alias_path($path_alias);
  }

  if ($result) {
    $result = $handler->create_path_alias();
  }

  if ($result === FALSE) {
    watchdog('rules', "Failed to create workbench path alias for node = $node_id, access_id = %access_id, and path_alias = '%path_alias'", array('%node_id' => $node_id, '%access_id' => $access_id, '%path_alias' => $path_alias), WATCHDOG_ERROR);
  }
}

/**
 * Action: delete a node path alias.
 *
 * $param numeric $node_id
 *   The unique id representing the node.
 * $param string $path_alias
 *   The url path string to create the path alias to.
 */
function mcneese_url_paths_delete_node_alias_action($node_id, $path_alias) {
  $handler = new mcneese_url_paths_node_handler();

  $result = $handler->set_node_id($node_id);

  if ($result) {
    $result = $handler->set_alias_path($path_alias);
  }

  if ($result) {
    $result = $handler->delete_path_alias();
  }

  if ($result === FALSE) {
    watchdog('rules', "Failed to delete normal path alias for node = $node_id and path_alias = '%path_alias'", array('%node_id' => $node_id, '%path_alias' => $path_alias), WATCHDOG_ERROR);
  }
}

/**
 * Action: delete a node path alias based on workbench menu path.
 *
 * $param numeric $node_id
 *   The unique id representing the node.
 * $param numeric $access_id
 *   The unique id representing the workbench access id.
 *   This is the unique taxonomy id.
 * $param string $path_alias
 *   The url path string to create the path alias to.
 * $param bool $group_representative
 *   (optional) The url path string to create the path alias to.
 */
function mcneese_url_paths_delete_node_alias_workbench_action($node_id, $access_id, $path_alias, $group_representative = FALSE) {
  $handler = new mcneese_url_paths_node_handler();

  $result = $handler->set_group_representative($group_representative);

  if ($result) {
    $result = $handler->set_node_id($node_id);
  }

  if ($result) {
    $result = $handler->set_access_id($access_id);
  }

  if ($result) {
    $result = $handler->set_alias_path($path_alias);
  }

  if ($result) {
    $result = $handler->delete_path_alias();
  }

  if ($result === FALSE) {
    watchdog('rules', "Failed to delete workbench path alias for node = $node_id, access_id = %access_id, and path_alias = '%path_alias'", array('%node_id' => $node_id, '%access_id' => $access_id, '%path_alias' => $path_alias), WATCHDOG_ERROR);
  }
}

/**
 * Action: delete all node aliases associated with a given node id.
 *
 * $param numeric $node_id
 *   The unique id representing the node.
 */
function mcneese_url_paths_delete_all_node_aliases_action($node_id) {
  mcneese_url_paths_delete_all_node_aliases($node_id);
}

/**
 * @} End of '@addtogroup mcneese_url_paths'.
 */
