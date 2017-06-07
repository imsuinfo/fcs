<?php

/**
 * @file
 * Defines McNeese facilities use drupal hook functions.
 *
 * Hooks thare are not called often and are very large are defined here.
 * All other hooks will continue to exist in the mfcs.module file.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Internal function for hook_permission.
 */
function mfcs_hook_permission() {
  $permissions = array();
  $permissions['mfcs administer'] = array(
    'title' => t('Administer Facilities Use Requests'),
    'description' => t('Grants permissions to administer requests and workflow.'),
  );

  $permissions['mfcs request'] = array(
    'title' => t('Request Facilities Use Requests'),
    'description' => t('Grants permissions to make requests.'),
  );

  $permissions['mfcs review'] = array(
    'title' => t('Review Facilities Use Requests'),
    'description' => t('Grants permissions to review requests.'),
  );

  $permissions['mfcs finance'] = array(
    'title' => t('Finance Facilities Use Requests'),
    'description' => t('Grants permissions to perform financial related activity with requests (a privileged subset of mfcs review).'),
  );

  $permissions['mfcs insure'] = array(
    'title' => t('Insurance Facilities Use Requests'),
    'description' => t('Grants permissions to perform insurance related activity with requests (a privileged subset of mfcs review).'),
  );

  $permissions['mfcs manage'] = array(
    'title' => t('Manage Facilities Use Requests'),
    'description' => t('Grants permissions to manage requests.'),
  );

  return $permissions;
}

/**
 * Internal function for hook_menu.
 */
function mfcs_hook_menu() {
  $module_path = drupal_get_path('module', 'mfcs');

  $items = array();
  mfcs_hook_menu_requests_basic($items, $module_path);
  mfcs_hook_menu_requests_reviewing($items, $module_path);
  mfcs_hook_menu_requests_listing($items, $module_path);

  mfcs_hook_menu_management($items, $module_path);

  mfcs_hook_menu_users($items, $module_path);

  mfcs_hook_menu_help($items, $module_path);

  return $items;
}

/**
 * hook_menu() entries related to requests.
 *
 * @param array $items
 *   An array of menu items.
 * @param string $module_path
 *   The filesystem path to the module.
 */
function mfcs_hook_menu_requests_basic(&$items, $module_path) {
  $items['requests'] = array(
    'title' => 'Dashboard',
    'description' => 'View your dashboard',
    'page callback' => 'mfcs_dashboard_page',
    'file' => 'dashboard.inc',
    'file path' => $module_path . '/pages',
    'tab_parent' => '',
    'tab_root' => '',
    'access callback' => 'mfcs_page_dashboard_access', // prevent drupals access callback from being called.
  );

  $items['requests/create-0'] = array(
    'title' => 'Create Request',
    'description' => 'Create Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_create_0_form'),
    'file' => 'request_create.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/create-0/%'] = array(
    'title' => 'Create Request',
    'description' => 'Create Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_create_0_form', 2),
    'file' => 'request_create.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/create-0/autocomplete/request_coordinator/email'] = array(
    'title' => '',
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_email',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/create-0/autocomplete/request_coordinator/name'] = array(
    'title' => '',
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_name',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/create-0/autocomplete/request_coordinator/phone'] = array(
    'title' => '',
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_phone',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  // this is provided as a way to have the breadcrumb path simulate the 'view' despite not being in the url path.
  $items['requests/edit-0/%'] = array(
    'title' => 'View Request',
    'description' => 'Edit Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_edit_0_form', 2),
    'file' => 'request_edit.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/edit-0/%/%'] = array(
    'title' => 'Edit Request',
    'description' => 'Edit Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_edit_0_form', 2, 3),
    'file' => 'request_edit.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit', 2, NULL, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/edit-0/autocomplete/request_coordinator/email'] = array(
    'title' => '',
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_email',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/edit-0/autocomplete/request_coordinator/name'] = array(
    'title' => '',
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_name',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/edit-0/autocomplete/request_coordinator/phone'] = array(
    'title' => '',
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_phone',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/delete-0/%'] = array(
    'title' => 'Delete Request',
    'description' => 'Delete Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_delete_0_form', 2),
    'file' => 'request_delete.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('delete', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/cancel-0/%'] = array(
    'title' => 'Cancel Request',
    'description' => 'Cancel Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_cancel_0_form', 2),
    'file' => 'request_cancel.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('cancel', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/uncancel-0/%'] = array(
    'title' => 'Uncancel Request',
    'description' => 'Uncancel Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_uncancel_0_form', 2),
    'file' => 'request_cancel.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('uncancel', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/cancelling-0/%'] = array(
    'title' => 'Cancel or Uncancel Individual Dates',
    'description' => 'Cancel or Uncancel Individual Dates',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_cancelling_0_form', 2),
    'file' => 'request_cancel.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('cancelling', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/manage-0/%'] = array(
    'title' => 'Override Request',
    'description' => 'Override Request',
    'page callback' => 'mfcs_request_manage_0_page',
    'page arguments' => array(2),
    'file' => 'request_manage.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('manage', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/manage-0/override/%'] = array(
    'title' => 'Override Request',
    'description' => 'Override Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_manage_0_override_form', 3),
    'file' => 'request_override.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('override', 3),
    'tab_parent' => 'requests/manage-0',
    'tab_root' => 'requests/manage-0',
  );

  $items['requests/manage-0/reassign/coordinator/%'] = array(
    'title' => 'Re-Assign Request Coordinator',
    'description' => 'Re-Assign Request Coordinator',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_manage_0_reassign_coordinator_form', 4),
    'file' => 'request_reassign.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reassign-coordinator', 4),
    'tab_parent' => 'requests/manage-0',
    'tab_root' => 'requests/manage-0',
  );

  $items['requests/manage-0/reassign/requester/%'] = array(
    'title' => 'Re-Assign Request Requester',
    'description' => 'Re-Assign Request Requester',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_manage_0_reassign_requester_form', 4),
    'file' => 'request_reassign.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reassign-requester', 4),
    'tab_parent' => 'requests/manage-0',
    'tab_root' => 'requests/manage-0',
  );

  $items['requests/manage-0/reassign/requester/autocomplete/user_id'] = array(
    'title' => '',
    'page callback' => 'mfcs_autocomplete_user_id',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reassign-requester'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/manage-0',
    'tab_root' => 'requests/manage-0',
  );

  $items['requests/manage-0/locks/%'] = array(
    'title' => 'Request Locks',
    'description' => 'Request Locks',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_manage_0_locks_form', 3),
    'file' => 'request_locks.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('manage', 3),
    'tab_parent' => 'requests/manage-0',
    'tab_root' => 'requests/manage-0',
  );

  $items['requests/manage-0/move/%'] = array(
    'title' => 'Move Request',
    'description' => 'Move Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_manage_0_move_form', 3),
    'file' => 'request_move.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('move', 3),
    'tab_parent' => 'requests/manage-0',
    'tab_root' => 'requests/manage-0',
  );

  $items['requests/view-0/%'] = array(
    'title' => 'View Request',
    'description' => 'View Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_view_0_form', 2),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['requests/view-0/%/%'] = array(
    'title' => 'View Request',
    'description' => 'View Request',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_view_0_form', 2, 3),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['pdf-0/%'] = array(
    'title' => 'Request PDF',
    'description' => 'Download Request PDF',
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(1),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 1),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['pdf-0/%/%'] = array(
    'title' => 'Request PDF',
    'description' => 'Download Request PDF',
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(1, 2),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 1),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['pdf-0/%/%/%'] = array(
    'title' => 'Request PDF',
    'description' => 'Download Request PDF',
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(1, 2, 3),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 1),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/print-0/%'] = array(
    'title' => 'Printable Request',
    'description' => 'Self-contained/Printable Request',
    'page callback' => 'mfcs_request_print_0_page',
    'page arguments' => array(2),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/print-0/%/%'] = array(
    'title' => 'Printable Request',
    'description' => 'Self-contained/Printable Request',
    'page callback' => 'mfcs_request_print_0_page',
    'page arguments' => array(2, 3),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/print-0/%/%/%'] = array(
    'title' => 'Printable Request',
    'description' => 'Self-contained/Printable Request',
    'page callback' => 'mfcs_request_print_0_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/ical/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'Perform iCalendar synchronization operations.',
    'page callback' => 'mfcs_request_synchronize_0_ical_page',
    'page arguments' => array(3),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array('' . MFCS_SYNCHRONIZE_TYPE_ICAL, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/ical/%/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'Perform iCalendar synchronization operations.',
    'page callback' => 'mfcs_request_synchronize_0_ical_page',
    'page arguments' => array(3, 4),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array('' . MFCS_SYNCHRONIZE_TYPE_ICAL, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/ical/%/%/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'Perform iCalendar synchronization operations.',
    'page callback' => 'mfcs_request_synchronize_0_ical_page',
    'page arguments' => array(3, 4, 5),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array('' . MFCS_SYNCHRONIZE_TYPE_ICAL, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/ical/%/%/%/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'Perform iCalendar synchronization operations.',
    'page callback' => 'mfcs_request_synchronize_0_ical_page',
    'page arguments' => array(3, 4, 5, 6),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array('' . MFCS_SYNCHRONIZE_TYPE_ICAL, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );
}

/**
 * hook_menu() entries related to requests reviewing.
 *
 * @param array $items
 *   An array of menu items.
 * @param string $module_path
 *   The filesystem path to the module.
 */
function mfcs_hook_menu_requests_reviewing(&$items, $module_path) {
  $items['requests/review-0'] = array(
    'title' => 'Review',
    'description' => 'View a listing of requests that you are allowed to review.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_review_0_page'),
    'file' => 'request_review.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('review'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/review-0/%'] = array(
    'title' => 'Review',
    'description' => 'Review Requests',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_review_0_page', 2),
    'file' => 'request_review.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('review'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/review-0/ajax'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_review_0',
    'page arguments' => array(),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('review'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/review-0',
    'tab_root' => 'requests/review-0',
  );

  $items['requests/history-0/%'] = array(
    'title' => 'Request History',
    'description' => 'Request History',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_history_0_form', 2),
    'file' => 'request_history.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('history', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/agreement-0/%'] = array(
    'title' => 'Facilities Use Agreement',
    'description' => 'Facilities Use Agreement',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_agreement_0_form', 2),
    'file' => 'request_agreement.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('agreement', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/agreement-0/%/pdf'] = array(
    'title' => 'Facilities Use Agreement',
    'description' => 'Facilities Use Agreement',
    'page callback' => 'mfcs_request_agreement_0_pdf',
    'page arguments' => array(2),
    'file' => 'request_agreement.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('agreement', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );
}

/**
 * hook_menu() entries related to request listing.
 *
 * @param array $items
 *   An array of menu items.
 * @param string $module_path
 *   The filesystem path to the module.
 */
function mfcs_hook_menu_requests_listing(&$items, $module_path) {
  $items['requests/list-0'] = array(
    'title' => 'Requests',
    'description' => 'View all requests on the system.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_list_0_form'),
    'file' => 'request_list.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/list-0/%'] = array(
    'title' => 'List Requests',
    'description' => 'List Requests',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_list_0_form', 2),
    'file' => 'request_list.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/list-0/ajax'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_list_0',
    'page arguments' => array(),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['requests/list-0/ajax/%'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_list_0',
    'page arguments' => array(3),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['requests/calendar-0/month'] = array(
    'title' => 'Request Calendar',
    'description' => 'Request Calendar - Month',
    'page callback' => 'mfcs_request_calendar_month_0_page',
    'page arguments' => array(),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/month/%'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Month',
    'page callback' => 'mfcs_request_calendar_month_0_page',
    'page arguments' => array(3),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/month/%/%'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Month',
    'page callback' => 'mfcs_request_calendar_month_0_page',
    'page arguments' => array(3, 4),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/day'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Day',
    'page callback' => 'mfcs_request_calendar_day_0_page',
    'page arguments' => array(),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/day/%'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Day',
    'page callback' => 'mfcs_request_calendar_day_0_page',
    'page arguments' => array(3),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/day/%/%'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Day',
    'page callback' => 'mfcs_request_calendar_day_0_page',
    'page arguments' => array(3, 4),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/day/%/%/%'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Day',
    'page callback' => 'mfcs_request_calendar_day_0_page',
    'page arguments' => array(3, 4, 5),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/day/%/%/%/%'] = array(
    'title' => 'Request Calendar',
    'description' => 'Requests Calendar - Day',
    'page callback' => 'mfcs_request_calendar_day_0_page',
    'page arguments' => array(3, 4, 5, 6),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/%'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Request iCalendar',
    'page callback' => 'mfcs_request_ical_request_0_page',
    'page arguments' => array(2),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/year'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Request iCalendar - Year',
    'page callback' => 'mfcs_request_ical_year_0_page',
    'page arguments' => array(NULL, NULL),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/year/%'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Requests iCalendar - Year',
    'page callback' => 'mfcs_request_ical_year_0_page',
    'page arguments' => array(3, 4),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/month'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Request iCalendar - Month',
    'page callback' => 'mfcs_request_ical_month_0_page',
    'page arguments' => array(NULL, NULL),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/month/%/%'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Requests iCalendar - Month',
    'page callback' => 'mfcs_request_ical_month_0_page',
    'page arguments' => array(3, 4),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/day'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Requests iCalendar - Day',
    'page callback' => 'mfcs_request_ical_day_0_page',
    'page arguments' => array(),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/day/%/%/%'] = array(
    'title' => 'Request iCalendar',
    'description' => 'Requests iCalendar - Day',
    'page callback' => 'mfcs_request_ical_day_0_page',
    'page arguments' => array(3, 4, 5),
    'file' => 'request_ical.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/search-0'] = array(
    'title' => 'Search Requests',
    'description' => 'Search Requests',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_search_0_form'),
    'file' => 'request_search.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('search'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  /*
  // @fixme: the function callbacks specified here are not implemented.
  $items['requests/search-0/ajax/simple/%'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_search_simple_0',
    'page arguments' => array(4),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('search'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/search-0',
    'tab_root' => 'requests/search-0',
  );

  $items['requests/search-0/ajax/advanced/%'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_search_simple_0',
    'page arguments' => array(4),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('search'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/search-0',
    'tab_root' => 'requests/search-0',
  );
  */

  $items['pdf-0/calendar/%/%'] = array(
    'title' => 'PDF of Monthly Calendar',
    'description' => 'PDF of monthly calendar.',
    'page callback' => 'mfcs_request_calendar_month_0_page_pdf',
    'page arguments' => array(2, 3),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['pdf-0/calendar/%/%/%/%'] = array(
    'title' => 'PDF of Daily Calendar',
    'description' => 'PDF of daily calendar.',
    'page callback' => 'mfcs_request_calendar_day_0_page_pdf',
    'page arguments' => array(2, 3, 4, 5),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['pdf-0/statistics/%/%'] = array(
    'title' => 'PDF of Monthly Calendar',
    'description' => 'PDF of monthly calendar.',
    'page callback' => 'mfcs_request_calendar_month_0_page_pdf',
    'page arguments' => array(2, 3),
    'file' => 'request_calendar.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['pdf-0/statistics/%'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page_pdf',
    'page arguments' => array(2),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['pdf-0/statistics/%/%'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page_pdf',
    'page arguments' => array(2, 3),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['pdf-0/statistics/%/%/%'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page_pdf',
    'page arguments' => array(2, 3, 4),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );
}

/**
 * hook_menu() entries related to management.
 *
 * @param array $items
 *   An array of menu items.
 * @param string $module_path
 *   The filesystem path to the module.
 */
function mfcs_hook_menu_management(&$items, $module_path) {
  $items['requests/management'] = array(
    'title' => 'Management',
    'description' => 'Access management options.',
    'page callback' => 'mfcs_management_page',
    'file' => 'management.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_management_page_access',
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0'] = array(
    'title' => 'Proxies',
    'description' => 'Manage who is able to proxy as venue coordinators.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_proxy_0_form'),
    'file' => 'request_proxy.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0/%'] = array(
    'title' => 'Manage Proxies',
    'description' => 'List users who are able to proxy as a venue coordinator.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_proxy_0_form', 2),
    'file' => 'request_proxy.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0/ajax'] = array(
    'title' => 'Manage Proxies',
    'description' => 'List users who are able to proxy as a venue coordinator.',
    'page callback' => 'mfcs_ajax_proxy_0_list',
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0/delete/%'] = array(
    'title' => 'Delete Proxy',
    'description' => 'Delete a proxy',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_proxy_0_delete_form', 3),
    'file' => 'request_proxy.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests/proxy-0',
    'tab_root' => 'requests/proxy-0',
  );

  $items['requests/proxy-0/autocomplete/user_id'] = array(
    'title' => '',
    'page callback' => 'mfcs_autocomplete_user_id',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/reviewers-0'] = array(
    'title' => 'Reviewers',
    'description' => 'Manage what reviewers are allowed to review.',
    'page callback' => 'mfcs_request_reviewers_0_page',
    'file' => 'request_reviewers.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/reviewers-0/list/%/%'] = array(
    'title' => 'List Reviewers',
    'description' => 'List Reviewers',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_reviewers_0_form', 3, 4),
    'file' => 'request_reviewers.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/list/%/%/%'] = array(
    'title' => 'List Reviewers',
    'description' => 'List Reviewers',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_reviewers_0_form', 3, 4, 5),
    'file' => 'request_reviewers.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/ajax/list/%/%'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_reviewers_0_list',
    'page arguments' => array(4, 5),
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/delete/%'] = array(
    'title' => 'Delete Reviewer',
    'description' => 'Delete Reviewer',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_reviewers_0_delete_form', 3),
    'file' => 'request_reviewers.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/autocomplete/user_id'] = array(
    'title' => '',
    'page callback' => 'mfcs_autocomplete_user_id',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/reviewers-0/autocomplete/reviewer_classification'] = array(
    'title' => '',
    'page callback' => 'mfcs_request_reviewers_0_autocomplete_reviewer_classification',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/statistics-0'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page',
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/statistics-0/%'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page',
    'page arguments' => array(2),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/statistics-0/%/%'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page',
    'page arguments' => array(2, 3),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/statistics-0/%/%/%'] = array(
    'title' => 'Request Statistics',
    'description' => 'Request Statistics',
    'page callback' => 'mfcs_request_statistics_0_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0'] = array(
    'title' => 'E-mail Logs',
    'description' => 'E-mail Logs',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_form'),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0/ajax'] = array(
    'title' => 'E-mail Logs',
    'description' => 'E-mail Logs',
    'page callback' => 'mfcs_ajax_email_log_0_list',
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0/%'] = array(
    'title' => 'E-mail Logs',
    'description' => 'E-mail Logs',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_form', 2),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0/view/%'] = array(
    'title' => 'E-mail Logs',
    'description' => 'E-mail Logs',
    'page callback' => 'mfcs_request_email_log_0_view_page',
    'page arguments' => array(3),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests/email_log-0',
    'tab_root' => 'requests/email_log-0',
  );

  $items['requests/email_log-0/resend/%'] = array(
    'title' => 'E-mail Logs',
    'description' => 'E-mail Logs',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_resend_form', 3),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('email_log-resend'),
    'tab_parent' => 'requests/email_log-0',
    'tab_root' => 'requests/email_log-0',
  );

  $items['requests/email_log-0/resend/%/%'] = array(
    'title' => 'E-mail Logs',
    'description' => 'E-mail Logs',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_resend_form', 3, 4),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('email_log-resend'),
    'tab_parent' => 'requests/email_log-0',
    'tab_root' => 'requests/email_log-0',
  );

  $items['troubleshoot-0'] = array(
    'title' => 'Troubleshooting',
    'description' => 'Troubleshoot problems on the system using pre-built tools.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_troubleshoot_0_form'),
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['troubleshoot-0/locations'] = array(
    'title' => 'Troubleshooting - Locations',
    'description' => 'A listing of locations available on the system with details.',
    'page callback' => 'mfcs_request_troubleshoot_0_locations_page',
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['troubleshoot-0/locations/%'] = array(
    'title' => 'Troubleshooting - Locations',
    'description' => 'A listing of locations available on the system with details.',
    'page callback' => 'mfcs_request_troubleshoot_0_locations_page',
    'page arguments' => array(2),
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['troubleshoot-0/locations/%/edit'] = array(
    'title' => 'Troubleshooting - Edit Location Name',
    'description' => 'A listing of locations available on the system with details.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_troubleshoot_0_locations_edit_name_form', 2),
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['troubleshoot-0/buildings'] = array(
    'title' => 'Troubleshooting - Buildings',
    'description' => 'A listing of buildings available on the system with details.',
    'page callback' => 'mfcs_request_troubleshoot_0_buildings_page',
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['troubleshoot-0/buildings/%'] = array(
    'title' => 'Troubleshooting - Buildings',
    'description' => 'A listing of buildings available on the system with details.',
    'page callback' => 'mfcs_request_troubleshoot_0_buildings_page',
    'page arguments' => array(2),
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['troubleshoot-0/rooms'] = array(
    'title' => 'Troubleshooting - Rooms',
    'description' => 'A listing of rooms available on the system with details.',
    'page callback' => 'mfcs_request_troubleshoot_0_rooms_page',
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['troubleshoot-0/rooms/%'] = array(
    'title' => 'Troubleshooting - Rooms',
    'description' => 'A listing of rooms available on the system with details.',
    'page callback' => 'mfcs_request_troubleshoot_0_rooms_page',
    'page arguments' => array(2),
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'troubleshoot-0',
    'tab_root' => 'troubleshoot-0',
  );

  $items['problems-0'] = array(
    'title' => 'Manage Problems',
    'description' => 'Manage problems and troubleshoot detected by the system.',
    'page callback' => 'mfcs_management_problems_page',
    'page arguments' => array(),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['problems-0/users'] = array(
    'title' => 'User Problems',
    'description' => 'View a list of users with problems that the system has detected.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_users_form'),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0',
    'tab_root' => 'problems-0',
  );

  $items['problems-0/users/ajax'] = array(
    'title' => 'User Problems',
    'description' => 'View a list of users with problems that the system has detected.',
    'page callback' => 'mfcs_ajax_problems_0_users_list',
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0',
    'tab_root' => 'problems-0',
  );

  $items['problems-0/users/%'] = array(
    'title' => 'User Problems',
    'description' => 'View a list of users with problems that the system has detected.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_users_form', 2),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0',
    'tab_root' => 'problems-0',
  );

  $items['problems-0/users/user/%'] = array(
    'title' => 'User Problems',
    'description' => 'Manage problems detected by the system that are specific to a single user.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_user_form', 3),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0/users',
    'tab_root' => 'problems-0/users',
  );

  $items['problems-0/users/resynchronize/%'] = array(
    'title' => 'User Problems - Re-sync Account',
    'description' => 'Attempt to re-synchronize user account to banner database.',
    'page callback' => 'mfcs_request_problems_0_users_resynchronize',
    'page arguments' => array(3),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0/users',
    'tab_root' => 'problems-0/users',
  );

  $items['problems-0/users/disabled_in_use/%'] = array(
    'title' => 'User Problems - Disabled Account in Use',
    'description' => 'Display open requests that are associated with a given disabled user.',
    'page callback' => 'mfcs_request_problems_0_users_disabled_in_use',
    'page arguments' => array(3),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0/users',
    'tab_root' => 'problems-0/users',
  );

  $items['problems-0/users/disabled_in_use/%/%'] = array(
    'title' => 'User Problems - Disabled Account in Use',
    'description' => 'Display open requests that are associated with a given disabled user.',
    'page callback' => 'mfcs_request_problems_0_users_disabled_in_use',
    'page arguments' => array(3, 4),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0/users',
    'tab_root' => 'problems-0/users',
  );

  $items['problems-0/users/disabled_in_use/ajax/%'] = array(
    'title' => '',
    'page callback' => 'mfcs_ajax_problems_0_disabled_in_use',
    'page arguments' => array(4),
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'problems-0/users',
    'tab_root' => 'problems-0/users',
  );

  $items['problems-0/requests'] = array(
    'title' => 'Request Problems',
    'description' => 'View a list of requests with problems that the system has detected.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_requests_form'),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0',
    'tab_root' => 'problems-0',
  );

  $items['problems-0/requests/ajax'] = array(
    'title' => 'Request Problems',
    'description' => 'View a list of requests with problems that the system has detected.',
    'page callback' => 'mfcs_ajax_problems_0_requests_list',
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0',
    'tab_root' => 'problems-0',
  );

  $items['problems-0/requests/ajax/%'] = array(
    'title' => 'Request Problems',
    'description' => 'View a list of requests with problems that the system has detected.',
    'page callback' => 'mfcs_ajax_problems_0_requests_list',
    'page arguments' => array(3),
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0',
    'tab_root' => 'problems-0',
  );

  $items['problems-0/requests/request/%'] = array(
    'title' => 'Request Problems',
    'description' => 'Manage problems detected by the system that are specific to a single request.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_request_form', 3),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0/requests',
    'tab_root' => 'problems-0/requests',
  );

  $items['problems-0/requests/request/%/recheck'] = array(
    'title' => 'Request Problems',
    'description' => 'Re-check a single problem for a single request.',
    'page callback' => 'mfcs_request_problems_0_request_recheck',
    'page arguments' => array(3),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'problems-0/requests',
    'tab_root' => 'problems-0/requests',
  );

  $items['requests/synchronize-0'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'mfcs_request_synchronize_0_page',
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/ajax'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'mfcs_ajax_synchronize_0_form',
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/synchronize-0/create'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_synchronize_0_create_form'),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/create/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_synchronize_0_create_form', 3),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/edit/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_synchronize_0_create_form', 3, FALSE),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/delete/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_synchronize_0_delete_form', 3),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/%'] = array(
    'title' => 'Synchronize Requests',
    'description' => 'The synchronize ',
    'page callback' => 'mfcs_request_synchronize_0_page',
    'page arguments' => array(2),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['holiday-0'] = array(
    'title' => 'Holidays',
    'description' => 'View or manage holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_form'),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('manage'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/%'] = array(
    'title' => 'Holidays',
    'description' => 'Holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_form', 1),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('manage', 1),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/copy/%'] = array(
    'title' => 'Copy Holidays',
    'description' => 'Holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_copy_form', 2),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('create', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/create/%'] = array(
    'title' => 'Create Holidays',
    'description' => 'Holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_edit_form', 2),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('create', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/create/%/%'] = array(
    'title' => 'Create Holidays',
    'description' => 'Holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_edit_form', 2, 3, TRUE),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('create', 2, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/edit/%/%'] = array(
    'title' => 'Edit Holidays',
    'description' => 'Holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_edit_form', 2, 3),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('edit', 2, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/delete/%/%'] = array(
    'title' => 'Delete Holidays',
    'description' => 'Holidays.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_delete_form', 2, 3),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('delete', 2, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['holiday-0/view/%/%'] = array(
    'title' => 'Holidays',
    'description' => 'Holidays.',
    'page callback' => 'mfcs_request_holiday_0_page',
    'page arguments' => array(2, 3),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array('view', 2, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0'] = array(
    'title' => 'Room Unavailabilities',
    'description' => 'View or manage when specific rooms are unavailable for reservation.',
    'page callback' => 'mfcs_request_unavailability_0_rooms_page',
    'page arguments' => array(),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('manage'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/all'] = array(
    'title' => 'Room Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_0_room_page',
    'page arguments' => array(NULL),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('manage'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/%/all'] = array(
    'title' => 'Room Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_0_room_page',
    'page arguments' => array(NULL, 1),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('manage', 1),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/%'] = array(
    'title' => 'Room Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_0_room_page',
    'page arguments' => array(1),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('manage', NULL, 1),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/%/%'] = array(
    'title' => 'Room Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_0_room_page',
    'page arguments' => array(2, 1),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('manage', 1, 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/copy/%/%'] = array(
    'title' => 'Copy Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_unavailability_0_copy_form', 2, 3),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('create', 2, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/create/%/%'] = array(
    'title' => 'Create Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_unavailability_0_edit_form', 2, 3),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('create', 2, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/create/%/%/%'] = array(
    'title' => 'Edit Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_unavailability_0_edit_form', 2, 3, 4, TRUE),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('edit', 2, 3, 4),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/view/%'] = array(
    'title' => 'View Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_redirect_page',
    'page arguments' => array('view', 2),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => TRUE,
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/view/%/%/%'] = array(
    'title' => 'View Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_0_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('view', 3, 4),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/edit/%'] = array(
    'title' => 'Edit Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_redirect_page',
    'page arguments' => array('edit', 2),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => TRUE,
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/edit/%/%/%'] = array(
    'title' => 'Edit Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_unavailability_0_edit_form', 2, 3, 4),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('edit', 2, 3, 4),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/delete/%'] = array(
    'title' => 'Delete Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'mfcs_request_unavailability_redirect_page',
    'page arguments' => array('delete', 2),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => TRUE,
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['unavailable-0/delete/%/%/%'] = array(
    'title' => 'Delete Unavailabilities',
    'description' => 'Unavailabilities.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_unavailability_0_delete_form', 2, 3, 4),
    'file' => 'request_unavailability.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_unavailability_request_access',
    'access arguments' => array('delete', 2, 3, 4, NULL, FALSE),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );
}

/**
 * hook_menu() entries related to users.
 *
 * @param array $items
 *   An array of menu items.
 * @param string $module_path
 *   The filesystem path to the module.
 */
function mfcs_hook_menu_users(&$items, $module_path) {
  $items['users-0/list'] = array(
    'title' => 'Users',
    'description' => 'View or manage users on the system.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_users_0_form'),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/list/%'] = array(
    'title' => 'Manage Users',
    'description' => 'Manage Users',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_users_0_form', 2),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/list/ajax'] = array(
    'title' => 'Manage Users',
    'description' => 'Manage Users',
    'page callback' => 'mfcs_ajax_users_0_list',
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/list/ajax/%'] = array(
    'title' => 'Manage Users',
    'description' => 'Manage Users',
    'page callback' => 'mfcs_ajax_users_0_list',
    'page arguments' => array(3),
    'file' => 'ajax-management.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/add'] = array(
    'title' => 'Add User',
    'description' => 'Add User',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_users_0_add_form'),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-add'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/view/%'] = array(
    'title' => 'View User',
    'description' => 'View User',
    'page callback' => 'mfcs_request_users_0_view_page',
    'page arguments' => array(2),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-view', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/edit/%'] = array(
    'title' => 'Edit User',
    'description' => 'Edit User',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_users_0_edit_form', 2),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-edit', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/check/%'] = array(
    'title' => 'Check User for Problems',
    'description' => 'Check User for Problems',
    'page callback' => 'mfcs_request_users_0_check_page',
    'page arguments' => array(2),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-check'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['users-0/customize/%'] = array(
    'title' => 'Customize User',
    'description' => 'Customize User Experience',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_users_0_customize_form', 2),
    'file' => 'request_users.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('users-customize', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );
}

/**
 * hook_menu() entries related to help.
 *
 * @param array $items
 *   An array of menu items.
 * @param string $module_path
 *   The filesystem path to the module.
 */
function mfcs_hook_menu_help(&$items, $module_path) {
  $items['help-0'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(1),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/%/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(1, 2),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/%/%/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(1, 2, 3),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/presentation/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page_presentation',
    'page arguments' => array(2),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/presentation/%/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page_presentation',
    'page arguments' => array(2, 3),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/presentation/%/%/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page_presentation',
    'page arguments' => array(2, 3, 4),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['help-0/presentation/%/%/%/%'] = array(
    'title' => 'Help',
    'description' => 'Online Documentation',
    'page callback' => 'mfcs_help_0_page_presentation',
    'page arguments' => array(2, 3, 4, 5),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  // @todo: provide custom pdf downloading for help pages
  /*
  $items['pdf-0/help'] = array(
    'title' => 'Help PDF',
    'description' => 'Download Help PDF',
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['pdf-0/help/%'] = array(
    'title' => 'Help PDF',
    'description' => 'Download Help PDF',
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(2),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['pdf-0/help/%/%'] = array(
    'title' => 'Help PDF',
    'description' => 'Download Help PDF',
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(2, 3),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['pdf-0/help/%/%/%'] = array(
    'title' => 'Help PDF',
    'description' => 'Download Help PDF',
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_generic_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );
  */
}

/**
 * Internal function for hook_cronapi.
 */
function mfcs_hook_cronapi() {
  $items = array();

  $items['mfcs_oracle_recache'] = array(
    'title' => t('Recreate Oracle Cache'),
    'callback' => 'mfcs_oracle_recache_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_insurance_not_provided_alert'] = array(
    'title' => t('Insurance Not Provided Alert'),
    'callback' => 'mfcs_insurance_not_provided_alert_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_unreviewed_request_alert'] = array(
    'title' => t('Unreviewed Request Alert'),
    'callback' => 'mfcs_unreviewed_request_alert_alert_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_autocancel_old_requests'] = array(
    'title' => t('Auto-Cancel Old Reqests'),
    'callback' => 'mfcs_autocancel_old_requests_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_auto_block_accounts'] = array(
    'title' => t('Auto-Block Accounts'),
    'callback' => 'mfcs_auto_block_accounts_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_check_for_problems_requests'] = array(
    'title' => t('Check for Problems - Requests'),
    'callback' => 'mfcs_check_for_problems_requests_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  return $items;
}

/**
 * Internal function for hook_user_load().
 */
function mfcs_hook_user_load($users) {
  $results = db_query('SELECT user_id, banner_id, first_name, last_name, phone_number, ldap_duplicate, primary_account, calendar, calendar_holidays, calendar_unavailabilities, filter_by_classification, filter_by_type, filter_by_building, problems, updated, updated_problems FROM {mfcs_users} WHERE user_id IN (:uids)', array(':uids' => array_keys($users)));

  $found = array();
  foreach ($results as $result) {
    $found[$result->user_id] = $result->user_id;
    $users[$result->user_id]->mfcs = array();
    $users[$result->user_id]->mfcs['banner_id'] = is_numeric($result->banner_id) ? (int) $result->banner_id : NULL;
    $users[$result->user_id]->mfcs['first_name'] = $result->first_name;
    $users[$result->user_id]->mfcs['last_name'] = $result->last_name;
    $users[$result->user_id]->mfcs['phone_number'] = $result->phone_number;
    $users[$result->user_id]->mfcs['ldap_duplicate'] = ($result->ldap_duplicate == TRUE) ? TRUE : FALSE;
    $users[$result->user_id]->mfcs['primary_account'] = ($result->primary_account == TRUE) ? TRUE : FALSE;
    $users[$result->user_id]->mfcs['calendar'] = (int) $result->calendar;
    $users[$result->user_id]->mfcs['calendar_holidays'] = ($result->calendar_holidays == TRUE) ? TRUE : FALSE;
    $users[$result->user_id]->mfcs['calendar_unavailabilities'] = ($result->calendar_unavailabilities == TRUE) ? TRUE : FALSE;
    $users[$result->user_id]->mfcs['filter_by_classification'] = array();
    $users[$result->user_id]->mfcs['filter_by_type'] = array();
    $users[$result->user_id]->mfcs['filter_by_building'] = array();
    $users[$result->user_id]->mfcs['problems'] = (int) $result->problems;
    $users[$result->user_id]->mfcs['updated'] = (int) $result->updated;
    $users[$result->user_id]->mfcs['updated_problems'] = (int) $result->updated_problems;

    if (!empty($result->filter_by_classification)) {
      $users[$result->user_id]->mfcs['filter_by_classification'] = array();

      $filters = preg_replace('/(^{|}$)/i', '', $result->filter_by_classification);
      $filters = explode(',', $filters);


      if (is_array($filters) && !empty($filters)) {
        foreach ($filters as $filter) {
          if (is_numeric($filter)) {
            $users[$result->user_id]->mfcs['filter_by_classification'][(int) $filter] = (int) $filter;
          }
        }
        unset($filter);
      }
      unset($filters);
    }

    if (!empty($result->filter_by_type)) {
      $users[$result->user_id]->mfcs['filter_by_type'] = array();

      $filters = preg_replace('/(^{|}$)/i', '', $result->filter_by_type);
      $filters = explode(',', $filters);

      if (is_array($filters) && !empty($filters)) {
        foreach ($filters as $filter) {
          if (is_numeric($filter)) {
            $users[$result->user_id]->mfcs['filter_by_type'][(int) $filter] = (int) $filter;
          }
        }
        unset($filter);
      }
      unset($filters);
    }

    if (!empty($result->filter_by_building)) {
      $users[$result->user_id]->mfcs['filter_by_building'] = array();

      $filters = preg_replace('/(^{|}$)/i', '', $result->filter_by_building);
      $filters = explode(',', $filters);

      if (is_array($filters) && !empty($filters)) {
        foreach ($filters as $filter) {
          if (is_numeric($filter)) {
            $users[$result->user_id]->mfcs['filter_by_building'][(int) $filter] = (int) $filter;
          }
        }
        unset($filter);
      }
      unset($filters);
    }
  }

  foreach (array_keys($users) as $user_id) {
    if (array_key_exists($user_id, $found)) continue;

    $user = new stdClass();
    $user->mfcs = array();
    $user->mfcs['banner_id'] = NULL;
    $user->mfcs['first_name'] = NULL;
    $user->mfcs['last_name'] = NULL;
    $user->mfcs['phone_number'] = NULL;
    $user->mfcs['ldap_duplicate'] = FALSE;
    $user->mfcs['primary_account'] = TRUE;
    $user->mfcs['calendar'] = MFCS_USER_CALENDAR_NONE;
    $user->mfcs['calendar_holidays'] = TRUE;
    $user->mfcs['calendar_unavailabilities'] = TRUE;
    $user->mfcs['filter_by_classification'] = array();
    $user->mfcs['filter_by_type'] = array();
    $user->mfcs['filter_by_building'] = array();
    $user->mfcs['problems'] = MFCS_USER_PROBLEM_NONE;
    $user->mfcs['updated'] = isset($users[$user_id]->created) ? (int) $users[$user_id]->created : mfcs_instance();
    $user->mfcs['updated_problems'] = $user->mfcs['updated'];
  }
}

/**
 * Internal function for hook_user_login().
 */
function mfcs_hook_user_login(&$edit, $user, $category) {
  if (!is_object($user)) {
    return;
  }

  $roles = (array) $user->roles;
  $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
  if (array_key_exists($bannerless_exception_role_id->rid, $roles)) {
    // do not sync to banner for bannerless-exception.
    return;
  }

  if (!isset($user->mfcs['banner_id']) || !is_numeric($user->mfcs['banner_id'])) {
    $ldap_information = mfcs_get_ldap_information($user);
    if (is_array($ldap_information)) {
      $information = array(
        'banner_id' => $ldap_information['banner_id'],
        'first_name' => $ldap_information['first_name'],
        'last_name' => $ldap_information['last_name'],
        'ldap_duplicate' => $ldap_information['ldap_duplicate'],
      );
      mfcs_save_user($user->uid, $information);
      unset($information);

      // reload settings to ensure the user object now has the correct ldap information.
      $user = user_load($user->uid, TRUE);
    }
  }

  // if the user does not have a valid banner id, then there is nothing to synchronize to.
  if (empty($user->mfcs['banner_id']) || !is_int($user->mfcs['banner_id'])) {
    return;
  }

  mfcs_synchronize_user_settings($user, array(), TRUE, TRUE, TRUE);


  // trigger a save update if the users roles should be changed according to mfcs_hook_process_user_banner_roles().
  $settings = array(
    'banner_id' => NULL,
    'primary' => (int) $user->uid,
    'status' => (int) $user->status,
  );

  if (isset($user->mfcs['banner_id']) && is_numeric($user->mfcs['banner_id'])) {
    $settings['banner_id'] = (int) $user->mfcs['banner_id'];
  }

  $problems = MFCS_USER_PROBLEM_NONE;
  if (isset($user->mfcs['problems'])) {
    $problems = $user->mfcs['problems'];
  }
  $problems_original = $problems;

  $changes = mfcs_hook_process_user_banner_roles($user, $problems, $settings);
  unset($settings);

  // save problems in the problems database.
  $reset_user = FALSE;
  if ($problems != $problems_original) {
    mfcs_save_user($user->uid, array('problems' => $problems));
    $reset_user = TRUE;
  }

  if (isset($changes['roles']) && is_array($changes['roles'])) {
    $user_roles = array_keys($user->roles);
    $changes_roles = array_keys($changes['roles']);

    if (count($user_roles) != count($changes_roles)) {
      user_save($user);

      // user_save() already resets the user information.
      $reset_user = FALSE;
    }
    else {
      $difference = array_diff($user_roles, $changes_roles);
      if (!empty($difference)) {
        user_save($user);

        // user_save() already resets the user information.
        $reset_user = FALSE;
      }
      else {
        $difference = array_diff($changes_roles, $user_roles);
        if (!empty($difference)) {
          user_save($user);

          // user_save() already resets the user information.
          $reset_user = FALSE;
        }
      }
    }
  }

  // reload user settings, populating it with the latest information.
  if ($reset_user) {
    $user = user_load($user->uid, TRUE);
  }
}

/**
 * Internal function for hook_user_insert().
 */
function mfcs_hook_user_insert(&$edit, $account, $category) {
  // $account must be an object.
  if (!is_object($account)) {
    return;
  }

  // only operate on the account-specific categories.
  if ($category != 'account') {
    return;
  }

  // attempt to load banner information.
  if (isset($account->uid)) {
    $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
    if (!isset($account->roles) || !is_array($account->roles) || !array_key_exists($bannerless_exception_role_id->rid, $account->roles)) {
      $ldap_information = mfcs_get_ldap_information($account);
      if (is_array($ldap_information)) {
        $edit['mail'] = $ldap_information['email'];

        $information = array(
          'banner_id' => $ldap_information['banner_id'],
          'first_name' => $ldap_information['first_name'],
          'last_name' => $ldap_information['last_name'],
          'ldap_duplicate' => $ldap_information['ldap_duplicate'],
        );
        mfcs_save_user($account->uid, $information);
        unset($information);

        // save the updated settings.
        if (!isset($account->mfcs)) {
          $account->mfcs = array();
          $account->mfcs['banner_id'] = NULL;
          $account->mfcs['first_name'] = NULL;
          $account->mfcs['last_name'] = NULL;
          $account->mfcs['phone_number'] = NULL;
          $account->mfcs['ldap_duplicate'] = FALSE;
          $account->mfcs['primary_account'] = TRUE;
          $account->mfcs['problems'] = MFCS_USER_PROBLEM_NONE;
          $account->mfcs['updated'] = isset($account->created) ? (int) $account->created : mfcs_instance();
          $account->mfcs['updated_problems'] = $user->mfcs['updated'];
        }

        $account->mfcs['banner_id'] = $ldap_information['banner_id'];
        $account->mfcs['first_name'] = $ldap_information['first_name'];
        $account->mfcs['last_name'] = $ldap_information['last_name'];
        $account->mfcs['ldap_duplicate'] = $ldap_information['ldap_duplicate'];
      }
      unset($ldap_information);
    }
  }

  // if the user does not have a valid banner id, then there is nothing to synchronize to.
  if (empty($account->mfcs['banner_id']) || !is_int($account->mfcs['banner_id'])) {

    // create the initial/default user settings.
    if (isset($account->uid)) {
      if (!isset($account->mfcs)) {
        $account->mfcs = array();
        $account->mfcs['banner_id'] = NULL;
        $account->mfcs['first_name'] = NULL;
        $account->mfcs['last_name'] = NULL;
        $account->mfcs['phone_number'] = NULL;
        $account->mfcs['ldap_duplicate'] = FALSE;
        $account->mfcs['primary_account'] = TRUE;
        $account->mfcs['problems'] = MFCS_USER_PROBLEM_NONE;
        $account->mfcs['updated'] = isset($account->created) ? (int) $account->created : mfcs_instance();
        $account->mfcs['updated_problems'] = $user->mfcs['updated'];
      }

      mfcs_save_user($account->uid, $account->mfcs);
    }

    return;
  }

  $roles = (array) $account->roles;
  $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
  if (array_key_exists($bannerless_exception_role_id->rid, $roles)) {
    // create the initial/default user settings.
    if (isset($account->uid) && isset($account->mfcs)) {
      mfcs_save_user($account->uid, $account->mfcs);
    }

    // do not sync to banner for bannerless-exception.
    return;
  }

  // Some changes usually happen during the user presave hook, but the uid does not yet exist for new accounts.
  // Now that the user id should be defined, perform those operations.
  if (isset($account->uid)) {
    $result = mfcs_sync_banner_to_local_users($account->uid, $account->mfcs['banner_id']);
    if ($result === FALSE) {
      $problems = MFCS_USER_PROBLEM_NONE;
      if (isset($account->mfcs['problems']) && is_numeric($account->mfcs['problems'])) {
        $problems = (int) $account->mfcs['problems'];
      }
      $problems_original = $problems;

      $problems = $problems | MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE;
      if ($problems != $problems_original) {
        $account->mfcs['problems'] = $problems;
      }

      mfcs_save_user($account->uid, $account->mfcs);
    }
  }
}

/**
 * Internal function for hook_user_presave().
 */
function mfcs_hook_user_presave(&$edit, $account, $category) {
  // only operate on the account-specific categories.
  if ($category != 'account') {
    return;
  }

  // $account must be an object.
  if (!is_object($account)) {
    return;
  }

  // store the timestamp in which the account was last updated.
  $edit['mfcs']['updated'] = mfcs_instance();


  // bannerless exception can be present on the original account or added via the edit.
  // drupal sometimes sets values to 0 to designate that they are to be removed.
  $bannerless_exception = FALSE;
  $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
  if (isset($account->roles) && is_array($account->roles)) {
    if (array_key_exists($bannerless_exception_role_id->rid, $account->roles)) {
      // drupal inconsistently stores the roles as either a string of text or a number with 0 representing role not available.
      // detect this before assigning bannerless exception based on array key alone.
      if (is_numeric($account->roles[$bannerless_exception_role_id->rid]) && $account->roles[$bannerless_exception_role_id->rid] != 0) {
        $bannerless_exception = TRUE;
      }
      else {
        $bannerless_exception = TRUE;
      }
    }
  }

  if (isset($edit['roles']) && is_array($edit['roles']) && array_key_exists($bannerless_exception_role_id->rid, $edit['roles'])) {
    if ($edit['roles'][$bannerless_exception_role_id->rid] == 0) {
      $bannerless_exception = FALSE;
    }
    else {
      $bannerless_exception = TRUE;
    }
  }


  // enforce banner information if the account is not a bannerless exception.
  if (isset($account->uid) && isset($account->mfcs)) {
    if (!$bannerless_exception) {
      $ldap_information = mfcs_get_ldap_information($account);
      if (is_array($ldap_information) && !empty($ldap_information)) {
        if (isset($edit['mail'])) {
          if ($edit['mail'] != $ldap_information['email']) {
            $edit['mail'] = $ldap_information['email'];
          }
        }
        elseif (isset($account->mail)) {
          if ($account->mail != $ldap_information['email']) {
            $edit['mail'] = $ldap_information['email'];
          }
        }

        if (isset($edit['mfcs']['banner_id'])) {
          if ($edit['mfcs']['banner_id'] != $ldap_information['banner_id']) {
            $edit['mfcs']['banner_id'] = $ldap_information['banner_id'];
          }
        }
        elseif (isset($account->mfcs) && array_key_exists('banner_id', $account->mfcs)) {
          if ($account->mfcs['banner_id'] != $ldap_information['banner_id']) {
            $edit['mfcs']['banner_id'] = $ldap_information['banner_id'];
          }
        }

        if (isset($edit['mfcs']['first_name'])) {
          if ($edit['mfcs']['first_name'] != $ldap_information['first_name']) {
            $edit['mfcs']['first_name'] = $ldap_information['first_name'];
          }
        }
        elseif (isset($account->mfcs) && array_key_exists('first_name', $account->mfcs)) {
          if ($account->mfcs['first_name'] != $ldap_information['first_name']) {
            $edit['mfcs']['first_name'] = $ldap_information['first_name'];
          }
        }

        if (isset($edit['mfcs']['last_name'])) {
          if ($edit['mfcs']['last_name'] != $ldap_information['last_name']) {
            $edit['mfcs']['last_name'] = $ldap_information['last_name'];
          }
        }
        elseif (isset($account->mfcs) && array_key_exists('last_name', $account->mfcs)) {
          if ($account->mfcs['last_name'] != $ldap_information['last_name']) {
            $edit['mfcs']['last_name'] = $ldap_information['last_name'];
          }
        }

        if (isset($edit['mfcs']['ldap_duplicate'])) {
          if ($edit['mfcs']['ldap_duplicate'] != $ldap_information['ldap_duplicate']) {
            $edit['mfcs']['ldap_duplicate'] = $ldap_information['ldap_duplicate'];
          }
        }
        elseif (isset($account->mfcs) && array_key_exists('ldap_duplicate', $account->mfcs)) {
          if ($account->mfcs['ldap_duplicate'] != $ldap_information['ldap_duplicate']) {
            $edit['mfcs']['ldap_duplicate'] = $ldap_information['ldap_duplicate'];
          }
        }
      }
      unset($ldap_information);
    }
  }

  // re-synchronize account information following change.
  if (!$bannerless_exception) {
    if (isset($edit['mfcs']) && array_key_exists('banner_id', $edit['mfcs'])) {
      if (!empty($edit['mfcs']['banner_id'])) {
        $result = mfcs_sync_banner_to_local_users($account->uid, $edit['mfcs']['banner_id']);
      }
    }
    elseif (isset($account->mfcs) && array_key_exists('banner_id', $account->mfcs)) {
      if (!empty($account->mfcs['banner_id'])) {
        $result = mfcs_sync_banner_to_local_users($account->uid, $account->mfcs['banner_id']);
      }
    }
  }

  $problems = MFCS_USER_PROBLEM_NONE;
  if (isset($account->mfcs['problems']) && is_numeric($account->mfcs['problems'])) {
    $problems = (int) $account->mfcs['problems'];
  }
  $problems_original = $problems;

  $settings = array();

  if (array_key_exists('mfcs', $edit) && is_array($edit['mfcs'])) {
    $settings['mfcs'] = $edit['mfcs'];
  }

  if (array_key_exists('roles', $edit) && is_array($edit['roles'])) {
    $settings['roles'] = $edit['roles'];
  }

  $changes = NULL;
  if (!empty($settings) && !$bannerless_exception) {
    $changes = mfcs_hook_process_user_banner_roles($account, $problems, $settings);

    if ($problems != $problems_original) {
      $edit['mfcs']['problems'] = $problems;
    }
  }
  unset($settings);

  if (is_array($changes)) {
    if (isset($changes['roles']) && is_array($changes['roles'])) {
      $edit['roles'] = array();
      foreach ($changes['roles'] as $role_id => $role_name) {
        $edit['roles'][$role_id] = (int) $role_id;
      }
    }

    if (array_key_exists('status', $changes)) {
      $edit['status'] = $changes['status'];
    }

    // if status has changed, and the account is disabled, then unset all roles.
    if (isset($edit['roles'])) {
      if (isset($edit['status']) && isset($account->status)) {
        if ($account->status != $edit['status']) {
          if (array_key_exists('status', $edit)) {
            if ($edit['status'] == 0) {
              foreach ($edit['roles'] as &$role_value) {
                $role_value = 0;
              }
            }
          }
          elseif ($account->status == 0) {
            foreach ($edit['roles'] as &$role_value) {
              $role_value = 0;
            }
          }
        }
      }
      elseif ($account->status == 0) {
        foreach ($edit['roles'] as &$role_value) {
          $role_value = 0;
        }
      }
    }
  }

  // pass along the changes to the problems, but do not perform any update actions just yet.
  if (isset($account->uid)) {
    if ($problems != $problems_original || (isset($edit['mfcs']) && count($edit['mfcs']) > 1)) {
      $edit['mfcs']['problems'] = $problems;
    }
  }
}

/**
 * Internal function for hook_user_update().
 */
function mfcs_hook_user_update(&$edit, $account, $category) {
  // $account must be an object.
  if (!is_object($account)) {
    return;
  }

  // only operate on the account-specific categories.
  if ($category != 'account') {
    return;
  }

  if (!isset($account->uid)) {
    return;
  }

  // if the status has changed, perform a user problem check.
  if (array_key_exists('status', $edit) && !isset($edit['original']->status) && $edit['status'] != $edit['original']->status) {
    $disabled_with_active = mfcs_disabled_venue_coordinators_with_active_content($account->uid);

    $problems = MFCS_USER_PROBLEM_NONE;
    if (isset($edit['original']->mfcs['problems']) && is_numeric($edit['original']->mfcs['problems'])) {
      $problems = (int) $edit['original']->mfcs['problems'];
    }

    if (isset($edit['mfcs']['problems']) && is_numeric($edit['mfcs']['problems'])) {
      $problems = (int) $edit['mfcs']['problems'];
    }
    $problems_original = $problems;

    if ($disabled_with_active) {
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) == 0) {
        $problems = ($problems | MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE);

        if ($problems != $problems_original) {
          $edit['mfcs']['problems'] = $problems;
        }
      }
    }
    else {
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) != 0) {
        $problems -= MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE;

        if ($problems != $problems_original) {
          $edit['mfcs']['problems'] = $problems;
        }
      }
    }

    // the code below cannot be performed due to security restrictions on the tables (which prevent update operations).
    // walk through each request whose request id is associated with the given user and update the name if the name settings have changed.
    /*
    $request_ids = mfcs_find_requests_with_requester_id($account->uid, TRUE);
    if (!empty($request_ids)) {
      $old_name = mfcs_generate_form_user_name($edit['original']);
      $new_name = mfcs_generate_form_user_name($account);

      $transaction = db_transaction();
      try {
        $query = db_update('mfcs_field_request_coordinator_name');
        $query->fields(array('value' => $new_name));

        $query->condition('value', $old_name);
        $query->condition('delta', 0);

        $or = db_or();
        foreach ($request_ids as $request_id => $revision_ids) {
          foreach ($revision_ids as $revision_id) {
            $and = db_and();
            $and->condition('request_id', $request_id);
            $and->condition('revision', $revision_id);
            $or->condition($and);
          }
        }
        unset($request_id);
        unset($revision_id);
        unset($revision_ids);

        $query->condition($or);
        $results = $query->execute();

        unset($transaction);
      }
      catch (Error $e) {
        $transaction->rollback();
        cf_error::on_exception($e);
      }
      catch (Exception $e) {
        $transaction->rollback();
        cf_error::on_exception($e);
      }
    }
    */
  }

  // save operations are always recorded because the 'updated' field must accurately reflect every update.
  if (!empty($edit['mfcs'])) {
    mfcs_save_user($account->uid, $edit['mfcs']);
  }
}

/**
 * Internal function for hook_user_cancel().
 */
function mfcs_hook_user_cancel($account) {
  mfcs_save_user($account->uid, array('updated' => mfcs_instance()));
}

/**
 * Pre-process the user login form for validation.
 */
function mfcs_hook_user_login_name_validate($form, $form_state) {
  if (!empty($form_state['values']['name'])) {
    try {
      $query = db_select('users', 'u');
      $query->condition('u.name', db_like($form_state['values']['name']), 'ilike');
      $query->addField('u', 'status', 'status');
      $query->addField('u', 'uid', 'user_id');


      $user = $query->execute()->fetchObject();
      if (is_object($user)) {
        if ($user->status == 0) {
          mfcs_synchronize_user_settings((int) $user->user_id);
        }
      }

      unset($user);
      unset($query);
    }
    catch (Error $e) {
      cf_error::on_query_execution($e);
    }
    catch (Exception $e) {
      cf_error::on_query_execution($e);
    }
  }
}

/**
 * Given a user object, the user settings will be updated.
 *
 * Process bannerless exception before calling this and if bannerless exception is enabled, then do not call this.
 *
 * @param object $user
 *   The user object to process.
 * @param int $problems
 *   An integer representing existing problems.
 *   This will be altered so that the caller can discover and handle problems encountered during the execution of this function.
 * @param array $changes
 *   An array of changes, as passed by mfcs_hook_user_presave().
 *   Leave empty to assign no changes to the user object.
 *
 * @return bool|array
 *   An array containing the changed fields, to be merged with a function such as mfcs_hook_user_presave().
 *   TRUE is returned when banner roles are not to be changed.
 *   FALSE is returned on error.
 *
 * @see: mfcs_hook_user_presave()
 * @see: mfcs_hook_user_login()
 */
function mfcs_hook_process_user_banner_roles($user, &$problems, $changes = array()) {
  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  if (!cf_is_integer($problems)) {
    cf_error::invalid_integer('problems');
    return FALSE;
  }

  if (!is_array($changes)) {
    cf_error::invalid_array('changes');
    return FALSE;
  }

  // prevent multiple executions per instance using static variables.
  if (isset($user->uid)) {
    $already_executed = &drupal_static(__FUNCTION__, NULL);
    if (is_array($already_executed) && array_key_exists($user->uid, $already_executed)) {
      return $already_executed[$user->uid];
    }
  }

  $mfcs_user_settings = array(
    'banner_id' => NULL,
    'primary_account' => TRUE,
    'ldap_duplicate' => FALSE,
    'first_name' => NULL,
    'last_name' => NULL,
    'phone_number' => NULL,
    'updated' => mfcs_instance(),
  );

  // load defaults assigned to the user object.
  if (isset($user->mfcs) && is_array($user->mfcs)) {
    if (array_key_exists('banner_id', $user->mfcs) && is_numeric($user->mfcs['banner_id'])) {
      $mfcs_user_settings['banner_id'] = (int) $user->mfcs['banner_id'];
    }

    if (array_key_exists('primary_account', $user->mfcs) && is_bool($user->mfcs['primary_account'])) {
      $mfcs_user_settings['primary_account'] = $user->mfcs['primary_account'];
    }

    if (array_key_exists('ldap_duplicate', $user->mfcs) && is_bool($user->mfcs['ldap_duplicate'])) {
      $mfcs_user_settings['ldap_duplicate'] = $user->mfcs['ldap_duplicate'];
    }

    if (array_key_exists('first_name', $user->mfcs) && is_string($user->mfcs['first_name'])) {
      $mfcs_user_settings['first_name'] = $user->mfcs['first_name'];
    }

    if (array_key_exists('last_name', $user->mfcs) && is_string($user->mfcs['last_name'])) {
      $mfcs_user_settings['last_name'] = $user->mfcs['last_name'];
    }

    if (array_key_exists('phone_number', $user->mfcs) && is_string($user->mfcs['phone_number'])) {
      $phone_number = $user->mfcs['phone_number'];
    }
  }

  // use any available changes in place of default.
  if (isset($changes['mfcs']) && array_key_exists('mfcs', $changes)) {
    if (array_key_exists('banner_id', $changes['mfcs']) && is_numeric($changes['mfcs']['banner_id'])) {
      $mfcs_user_settings['banner_id'] = (int) $changes['mfcs']['banner_id'];
    }

    if (array_key_exists('primary_account', $changes['mfcs']) && is_bool($changes['mfcs']['primary_account'])) {
      $mfcs_user_settings['primary_account'] = $changes['mfcs']['primary_account'];
    }

    if (array_key_exists('ldap_duplicate', $changes['mfcs']) && is_bool($changes['mfcs']['ldap_duplicate'])) {
      $mfcs_user_settings['ldap_duplicate'] = $changes['mfcs']['ldap_duplicate'];
    }

    if (array_key_exists('first_name', $changes['mfcs']) && is_string($changes['mfcs']['first_name'])) {
      $mfcs_user_settings['first_name'] = $changes['mfcs']['first_name'];
    }

    if (array_key_exists('last_name', $changes['mfcs']) && is_string($changes['mfcs']['last_name'])) {
      $mfcs_user_settings['last_name'] = $changes['mfcs']['last_name'];
    }

    if (array_key_exists('phone_number', $changes['mfcs']) && is_string($changes['mfcs']['phone_number'])) {
      $phone_number = $changes['mfcs']['phone_number'];
    }
  }


  $primary_account_id = NULL;
  if (isset($user->uid)) {
    // uid is not present for new accounts, only assign if it exists.
    $primary_account_id = (int) $user->uid;
  }

  if (!is_null($mfcs_user_settings['banner_id']) && $mfcs_user_settings['primary_account'] === FALSE) {
    $associated_users = mfcs_users_load_banner_id_to_user_id($mfcs_user_settings['banner_id']);
    if (isset($associated_users['banner'][$mfcs_user_settings['banner_id']]['primary']['user_id'])) {
      $primary_account_id = (int) $associated_users['banner'][$mfcs_user_settings['banner_id']]['primary']['user_id'];
    }
    unset($associated_users);
  }

  // warning: these are assumed to be defined (on a new install, they must be manually created with these exact names).
  $requester_role = user_role_load_by_name('requester');
  $reviewer_role = user_role_load_by_name('reviewer');
  $financer_role = user_role_load_by_name('financer');
  $insurer_role = user_role_load_by_name('insurer');
  $manager_role = user_role_load_by_name('manager');
  $administrator_role = user_role_load_by_name('administrator');
  $bannerless_exception_role = user_role_load_by_name('bannerless exception');
  $reviewer_exception_role = user_role_load_by_name('reviewer exception');

  $role_mappings = mfcs_get_drupal_to_banner_role_mappings('banner_name');

  $user_roles = array();
  if (isset($user->roles) && is_array($user->roles)) {
    $user_roles = $user->roles;
  }

  if (array_key_exists('roles', $changes)) {
    if (is_array($changes['roles'])) {
      $user_roles = $changes['roles'];
    }
  }

  // force array to be populated with role ids and not role names.
  $processed_roles = array();
  foreach ($user_roles as $role_id => $role_name) {
    // drupal's edit will set role_name to 0 for removing roles under certain conditions (while for others will use a role name to simply specify that the roles are assigned).
    if (is_numeric($role_name) && $role_name == 0) {
      continue;
    }

    $processed_roles[(int) $role_id] = (int) $role_id;
  }
  unset($role_id);
  unset($role_name);

  // handle exceptional cases.
  if (isset($bannerless_exception_role->rid) && array_key_exists($bannerless_exception_role->rid, $processed_roles)) {
    // if the user is defined as being an exception to operate as if it is bannerless, then do so.
    if (isset($user->uid)) {
      $already_executed[$user->uid] = TRUE;
    }
    return TRUE;
  }
  unset($bannerless_exception_role);

  if (isset($administrator_role->rid) && array_key_exists($administrator_role->rid, $processed_roles)) {
    // administrators are not auto-managed.
    if (isset($user->uid)) {
      $already_executed[$user->uid] = TRUE;
    }
    return TRUE;
  }
  unset($administrator_role);

  $is_manager = array_key_exists($manager_role->rid, $processed_roles);

  // remove roles associated with banner from the user roles for non-managers and non-administers.
  if (!$is_manager) {
    foreach ($role_mappings as $mapping) {
      if (array_key_exists($mapping->role_id, $processed_roles)) {
        unset($processed_roles[$mapping->role_id]);
      }
    }
    unset($mapping);
  }

  // There is little to do when the user has no banner id (this is not an error).
  if (is_null($mfcs_user_settings['banner_id'])) {
    // remove all roles that require banner access.
    unset($processed_roles[$reviewer_role->rid]);
    unset($processed_roles[$requester_role->rid]);
    unset($processed_roles[$financer_role->rid]);
    unset($processed_roles[$insurer_role->rid]);

    $results = array();
    $results['roles'] = $processed_roles;

    if (isset($user->uid)) {
      $already_executed[$user->uid] = $results;
    }

    return $results;
  }

  // validate banner id, if invalid, then strip all user roles.
  $banner_information = mfcs_load_request_user_by_banner_id($mfcs_user_settings['banner_id']);
  if (is_bool($banner_information) || empty($banner_information['roles'])) {
    $user_status = NULL;
    if (isset($user->status)) {
      $user_status = (int) $user->status;
    }

    if (array_key_exists('status', $changes) && is_numeric($changes['status'])) {
      $user_status = (int) $changes['status'];
    }

    if ($user_status === 0) {
      // disabled accounts should not report invalid banner ids.
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID) != 0) {
        $problems = $problems - MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID;
      }

      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_NOT_IN_LDAP) != 0) {
        $problems = $problems - MFCS_USER_PROBLEM_ACCOUNT_BANNER_NOT_IN_LDAP;
      }

      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES) != 0) {
        $problems = $problems - MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES;
      }
    }
    else {
      if (is_bool($banner_information)) {
        if (($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID) == 0) {
          $problems = $problems | MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID;
        }
      }
      elseif (empty($banner_information['roles'])) {
        if (($problems & MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES) == 0) {
          $problems = $problems | MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES;
        }
      }
    }

    if (!$is_manager) {
      // remove all roles that require banner access.
      unset($processed_roles[$reviewer_role->rid]);
      unset($processed_roles[$requester_role->rid]);
      unset($processed_roles[$financer_role->rid]);
      unset($processed_roles[$insurer_role->rid]);

      $results = array();
      $results['roles'] = $processed_roles;

      if (isset($user->uid)) {
        $already_executed[$user->uid] = $results;
      }

      return $results;
    }
  }
  else {
    if (($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID) != 0) {
      $problems = $problems - MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID;
    }

    if (($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_NOT_IN_LDAP) != 0) {
      $problems = $problems - MFCS_USER_PROBLEM_ACCOUNT_BANNER_NOT_IN_LDAP;
    }

    if (($problems & MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES) != 0) {
      $problems = $problems - MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES;
    }
  }
  unset($banner_information);

  // load banner role information associated with the primary user.
  $user_banner = array();
  if ($mfcs_user_settings['primary_account']) {
    if (isset($user->uid)) {
      $user_banner = mfcs_load_request_user_by_user_id($user->uid);
      if ($user_banner == FALSE) {
        $user_banner = array();
      }
    }
  }
  elseif (!is_null($primary_account_id)) {
    $user_banner = mfcs_load_request_user_by_user_id($primary_account_id);
    if ($user_banner == FALSE) {
      $user_banner = array();
    }
  }

  // Assign requester role for all users who have valid banner accounts (administrators and bannerless exception should never reach this point).
  if (!array_key_exists($requester_role->rid, $processed_roles) && !empty($user_banner)) {
    $processed_roles[$requester_role->rid] = $requester_role->rid;
  }

  // managers are auto-granted roles even if the roles are not supplied according to their banner access.
  if ($is_manager) {
    foreach ($role_mappings as $mapping) {
      $processed_roles[$mapping->role_id] = $mapping->role_id;
    }
    unset($mapping);
  }

  // determine if the user is a reviewer exception
  $reviewer_exception = FALSE;
  if (array_key_exists($reviewer_exception_role->rid, $processed_roles)) {
    $reviewer_exception = TRUE;
  }

  // if the (non-admin) user is a venue or room coordinator, then add/remove reviewer role as necessary.
  if (empty($user_banner['role'])) {
    // if the user has no banner roles, is not a manager, and is not a reviewer exception, then do not allow the user to have reviewer, financer, insurer, or requester roles (bannerless exception and administrators cannot reach this point).
    if (!$is_manager && !$reviewer_exception) {
      unset($processed_roles[$reviewer_role->rid]);
      unset($processed_roles[$requester_role->rid]);
      unset($processed_roles[$financer_role->rid]);
      unset($processed_roles[$insurer_role->rid]);
    }
  }
  else {
    foreach ($user_banner['role'] as $role) {
      if (isset($role_mappings[$role])) {
        $processed_roles[$role_mappings[$role]->role_id] = $role_mappings[$role]->role_id;
      }
    }
    unset($role);
  }

  // if the user can proxy as another account, then they need the reviewer role (but make no changes if the user is a manager or is a reviewer exception).
  if (!$is_manager && !$reviewer_exception) {
    $can_review = FALSE;
    $proxy_accounts = mfcs_get_proxy_venue_coordinators(array('mpvc.proxy_id' => $primary_account_id, 'mpvc.disabled' => 0));
    if (!empty($proxy_accounts['results'])) {
      $processed_roles[$reviewer_role->rid] = $reviewer_role->rid;
      $can_review = TRUE;
    }

    // Alter reviewer role settings for users without 'manager' and 'reviewer exception' roles.
    $reviewers = mfcs_get_reviewers(array('mr.user_id' => $primary_account_id, 'mr.disabled' => 0));
    if (!empty($reviewers['results'])) {
      $can_review = TRUE;
    }

    $all_coordinators = mfcs_load_room_coordinators(NULL, TRUE);
    if (array_key_exists($primary_account_id, $all_coordinators)) {
      if (array_key_exists($reviewer_role->rid, $processed_roles)) {
        $can_review = TRUE;
      }
    }

    if ($can_review) {
      $processed_roles[$reviewer_role->rid] = $reviewer_role->rid;

      // make sure that if the reviewer can now review, to get rid of the reviewer not allowed problem flag.
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED) != 0) {
        $problems -= MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED;
      }
    }
    else {
      if (array_key_exists($reviewer_role->rid, $processed_roles)) {
        unset($processed_roles[$reviewer_role->rid]);
        unset($processed_roles[$financer_role->rid]);
        unset($processed_roles[$insurer_role->rid]);

        // the function arguments are trying to assign 'proxy' role to a user who has no venue or room to review.
        // do not allow this and provide a watchdog warning for reference.
        watchdog(MFCS_WATCHDOG_ID, 'The user \'%user_name\' (%user_id, %primary_id) is not being granted reviewer access as requested because the user is not assigned as either a venue or room coordinator in the banner database and is not a venue coordinator proxy.', array('%user_name' => isset($user->name) ? $user->name : 'undefined', '%user_id' => isset($user->uid) ? $user->uid : 'undefined', '%primary_id' => $primary_account_id), WATCHDOG_WARNING);

        if (($problems & MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED) == 0) {
          $problems = $problems | MFCS_USER_PROBLEM_ACCOUNT_REVIEWER_NOT_ALLOWED;
        }
      }
    }
  }

  $results = array();

  // auto-lock or auto-unlock accounts without any banner roles (or other banner id related problems), but only when banner can be refreshed.
  if (($problems & MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE) == 0 && ($problems & MFCS_USER_PROBLEM_ACCOUNT_SYNC_BANNER_UNAVAILABLE) == 0) {
    if (($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_INVALID) == 0 && ($problems & MFCS_USER_PROBLEM_ACCOUNT_BANNER_NOT_IN_LDAP) == 0 && ($problems & MFCS_USER_PROBLEM_ACCOUNT_NO_BANNER_ROLES) == 0) {
      // If the user has banner roles, then make sure MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS is no longer set.
      if (!empty($banner_roles) && ($problems & MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS) != 0) {
        $problems -= MFCS_USER_PROBLEM_ACCOUNT_NO_ACCESS;
      }

      if ($user->status == 0) {
        if (!empty($banner_roles)) {
          // auto-unlock account.
          $results['status'] = 1;

          watchdog(MFCS_WATCHDOG_ID, 'The user \'%user_name\' (%user_id, %primary_id) has had the account enabled/unlocked due to regaining sufficient banner roles.', array('%user_name' => $user->name, '%user_id' => isset($user->uid) ? $user->uid : 'undefined', '%primary_id' => $primary_account_id), WATCHDOG_NOTICE);
        }
      }
      else {
        if (empty($banner_roles)) {
          // auto-lock account (currently disabled).
          #$results['status'] = 0;

          #watchdog(MFCS_WATCHDOG_ID, 'The user \'%user_name\' (%user_id, %primary_id) has had the account disabled/locked due to insufficient banner roles.', array('%user_name' => $user->name, '%user_id' => isset($user->uid) ? $user->uid : 'undefined', '%primary_id' => $primary_account_id), WATCHDOG_NOTICE);
        }
      }
    }
    else {
      if ($user->status == 1 && empty($banner_roles)) {
        // auto-lock account (currently disabled).
        #$results['status'] = 0;

        #watchdog(MFCS_WATCHDOG_ID, 'The user \'%user_name\' (%user_id, %primary_id) has had the account disabled/locked due to insufficient banner roles.', array('%user_name' => $user->name, '%user_id' => isset($user->uid) ? $user->uid : 'undefined', '%primary_id' => $primary_account_id), WATCHDOG_NOTICE);
      }
    }
  }

  $results['roles'] = $processed_roles;

  if (isset($user->uid)) {
    $already_executed[$user->uid] = $results;
  }

  return $results;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
