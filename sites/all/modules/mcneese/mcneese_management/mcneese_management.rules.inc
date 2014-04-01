<?php

/**
 * @file
 * Rules integration file for McNeese State University Url Paths.
 *
 * TODO: eventually the emergency/maintenance modes should be changed to
 * operate from completely separate database shared between multiple servers.
 * This will help facility putting multiple sites into maintenance/emergency
 * with a single click.
 *
 * TODO: Also consider adding functionality to communicate server status.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Implements hook_rules_file_info().
 */
function mcneese_management_rules_file_info() {
  $items = array();
  $items[] = 'mcneese_management.rules';

  return $items;
}

/**
 * Implements hook_rules_condition_info() on behalf of the node module.
 */
function mcneese_management_rules_condition_info() {
  $items = array();

  $items['mcneese_management_in_maintenance_mode'] = array(
    'group' => t("McNeese"),
    'label' => t("In Maintenance Mode"),
    'base' => 'mcneese_management_in_maintenance_mode_condition',
  );

  $items['mcneese_management_in_emergency_mode'] = array(
    'group' => t("McNeese"),
    'label' => t("In Emergency Mode"),
    'base' => 'mcneese_management_in_emergency_mode_condition',
  );

  return $items;
}

/**
 * Implements hook_rules_action_info() on behalf of the node module.
 */
function mcneese_management_rules_action_info() {
  $items = array();

  $items['mcneese_management_set_maintenance_mode'] = array(
    'label' => t("Set Maintenance Mode"),
    'group' => t("McNeese"),
    'base' => 'mcneese_management_set_maintenance_mode_action',
    'parameter' => array(
      'maintenance_mode' => array(
        'type' => 'boolean',
        'label' => t("Maintenance Mode"),
      ),
    ),
  );

  $items['mcneese_management_set_emergency_mode'] = array(
    'label' => t("Set Emergency Mode"),
    'group' => t("McNeese"),
    'base' => 'mcneese_management_set_emergency_mode_action',
    'parameter' => array(
      'emergency_mode' => array(
        'type' => 'boolean',
        'label' => t("Emergency Mode"),
      ),
    ),
  );

  $items['mcneese_management_get_maintenance_mode'] = array(
    'label' => t("Get Maintenance Mode"),
    'group' => t("McNeese"),
    'base' => 'mcneese_management_get_maintenance_mode_action',

    'provides' => array(
      'maintenance_mode' => array(
        'type' => 'boolean',
        'label' => t("Maintenance Mode"),
      ),
    ),
  );

  $items['mcneese_management_get_emergency_mode'] = array(
    'label' => t("Get Emergency Mode"),
    'group' => t("McNeese"),
    'base' => 'mcneese_management_get_emergency_mode_action',

    'provides' => array(
      'emergency_mode' => array(
        'type' => 'boolean',
        'label' => t("Emergency Mode"),
      ),
    ),
  );

  $items['mcneese_management_get_emergency_node'] = array(
    'label' => t("Get Emergency Node"),
    'group' => t("McNeese"),
    'base' => 'mcneese_management_get_emergency_node_action',

    'provides' => array(
      'emergency_node' => array(
        'type' => 'node',
        'label' => t("Emergency Node"),
      ),
    ),
  );

  return $items;
}


/**
 * Condition: Tests to see if the site is in maintenance mode.
 *
 * @return bool
 *   TRUE/FALSE depending on whether the site is in maintenance mode.
 */
function mcneese_management_in_maintenance_mode_condition() {
  return variable_get('maintenance_mode', FALSE);
}

/**
 * Condition: Tests to see if the site is in emergency mode.
 *
 * @return bool
 *   TRUE/FALSE depending on whether the site is in emergency mode.
 */
function mcneese_management_in_emergency_mode_condition() {
  return mcneese_management_get_emergency_node();
}

/**
 * Action: set/unset maintenance mode.
 *
 * $param boolean $maintenance_mode
 *   TRUE/FALSE to enable/disable maintenance mode.
 */
function mcneese_management_set_maintenance_mode_action($maintenance_mode) {
  if (is_bool($maintenance_mode)) {
    mcneese_management_set_emergency_mode($maintenance_mode);
  }
  else if ($emergency_mode) {
    mcneese_management_set_emergency_mode(TRUE);
  }
  else {
    mcneese_management_set_emergency_mode(FALSE);
  }

  variable_set('maintenance_mode', $maintenance_mode);

  // caches must be flushed on mode change for the change to be noticed.
  _drupal_flush_css_js();
  drupal_clear_css_cache();
  drupal_clear_js_cache();
  cache_clear_all(NULL, 'cache_block');
  cache_clear_all(NULL, 'cache_page');
  cache_clear_all(NULL, 'cache_menu');
}

/**
 * Action: set/unset emergency mode.
 *
 * $param boolean $mode
 *   TRUE/FALSE to enable/disable emergency mode.
 */
function mcneese_management_set_emergency_mode_action($emergency_mode) {
  mcneese_management_set_emergency_mode($emergency_mode);
}

/**
 * Action: Loads the maintenance mode setting.
 *
 * @return array
 *   An array containing the mode stored in the array key 'mode'.
 */
function mcneese_management_get_maintenance_mode_action() {
  return array('maintenance_mode' => variable_get('maintenance_mode', FALSE));
}

/**
 * Action: Loads the emergency mode setting.
 *
 * @return array
 *   An array containing the mode stored in the array key 'mode'.
 */
function mcneese_management_get_emergency_mode_action() {
  return array('emergency_mode' => mcneese_management_get_emergency_mode());
}

/**
 * Action: Loads the maintenance node setting.
 *
 * @return array
 *   An array containing the node stored in the array key 'node'.
 */
function mcneese_management_get_emergency_node_action() {
  return array('emergency_node' => mcneese_management_get_emergency_node());
}

/**
 * @} End of '@addtogroup mcneese_management'.
 */
