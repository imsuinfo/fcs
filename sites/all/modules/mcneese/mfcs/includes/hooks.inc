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
    'title' => t("Administer Facilities Use Requests"),
    'description' => t("Grants permissions to administer requests and workflow."),
  );

  $permissions['mfcs request'] = array(
    'title' => t("Request Facilities Use Requests"),
    'description' => t("Grants permissions to make requests."),
  );

  $permissions['mfcs review'] = array(
    'title' => t("Review Facilities Use Requests"),
    'description' => t("Grants permissions to review requests."),
  );

  $permissions['mfcs manage'] = array(
    'title' => t("Manage Facilities Use Requests"),
    'description' => t("Grants permissions to manage requests."),
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
    'title' => "Requests Dashboard",
    'description' => "Requests Dashboard",
    'page callback' => 'mfcs_dashboard_page',
    'file' => 'dashboard.inc',
    'file path' => $module_path . '/pages',
    'tab_parent' => '',
    'tab_root' => '',
    'access callback' => 'mfcs_page_dashboard_access', // prevent drupals access callback from being called.
  );

  $items['requests/create-0'] = array(
    'title' => "Create Request",
    'description' => "Create Request",
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
    'title' => "Create Request",
    'description' => "Create Request",
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
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_email',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/create-0/autocomplete/request_coordinator/name'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_name',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/create-0/autocomplete/request_coordinator/phone'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_phone',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  // this is provided as a way to have the breadcrumb path simulate the 'view' despite not being in the url path.
  $items['requests/edit-0/%'] = array(
    'title' => "View Request",
    'description' => "Edit Request",
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
    'title' => "Edit Request",
    'description' => "Edit Request",
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
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_email',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/edit-0/autocomplete/request_coordinator/name'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_name',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/edit-0/autocomplete/request_coordinator/phone'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_phone',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/delete-0/%'] = array(
    'title' => "Delete Request",
    'description' => "Delete Request",
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
    'title' => "Cancel Request",
    'description' => "Cancel Request",
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
    'title' => "Uncancel Request",
    'description' => "Uncancel Request",
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
    'title' => "Cancel or Uncancel Individual Dates",
    'description' => "Cancel or Uncancel Individual Dates",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_cancelling_0_form', 2),
    'file' => 'request_cancel.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('cancelling', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/reassign-0/%'] = array(
    'title' => "Re-Assign Request",
    'description' => "Re-Assign Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_reassign_0_form', 2),
    'file' => 'request_reassign.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reassign', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/view-0/%'] = array(
    'title' => "View Request",
    'description' => "View Request",
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
    'title' => "View Request",
    'description' => "View Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_view_0_form', 2, 3),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['requests/pdf-0/%'] = array(
    'title' => "Request PDF",
    'description' => "Download Request PDF",
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(2),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/pdf-0/%/%'] = array(
    'title' => "Request PDF",
    'description' => "Download Request PDF",
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(2, 3),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/pdf-0/%/%/%'] = array(
    'title' => "Request PDF",
    'description' => "Download Request PDF",
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'request_view.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/print-0/%'] = array(
    'title' => "Printable Request",
    'description' => "Self-contained/Printable Request",
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
    'title' => "Printable Request",
    'description' => "Self-contained/Printable Request",
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
    'title' => "Printable Request",
    'description' => "Self-contained/Printable Request",
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
    'title' => "Synchronize Requests",
    'description' => "Perform iCal synchronization operations.",
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
    'title' => "Synchronize Requests",
    'description' => "Perform iCal synchronization operations.",
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
    'title' => "Review Requests",
    'description' => "Review Request",
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
    'title' => "Review Requests",
    'description' => "Review Request",
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
    'title' => "",
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
    'title' => "Request History",
    'description' => "Request History",
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
    'title' => "Facilities Use Agreement",
    'description' => "Facilities Use Agreement",
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
    'title' => "Facilities Use Agreement",
    'description' => "Facilities Use Agreement",
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
    'title' => "List Requests",
    'description' => "List Requests",
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
    'title' => "List Requests",
    'description' => "List Requests",
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
    'title' => "",
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
    'title' => "",
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
    'title' => "Request Calendar",
    'description' => "Request Calendar - Month",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Month",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Month",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Day",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Day",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Day",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Day",
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
    'title' => "Request Calendar",
    'description' => "Requests Calendar - Day",
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
    'title' => "Request iCal",
    'description' => "Request iCal",
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
    'title' => "Request iCal",
    'description' => "Request iCal - Year",
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
    'title' => "Request iCal",
    'description' => "Requests iCal - Year",
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
    'title' => "Request iCal",
    'description' => "Request iCal - Month",
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
    'title' => "Request iCal",
    'description' => "Requests iCal - Month",
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
    'title' => "Request iCal",
    'description' => "Requests iCal - Day",
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
    'title' => "Request iCal",
    'description' => "Requests iCal - Day",
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
    'title' => "Search Requests",
    'description' => "Search Requests",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_search_0_form'),
    'file' => 'request_search.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('search'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/search-0/ajax/simple/%'] = array(
    'title' => "",
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
    'title' => "",
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
    'title' => "Requests Management",
    'description' => "Requests Management",
    'page callback' => 'mfcs_management_page',
    'file' => 'management.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_management_page_access',
    'tab_parent' => '',
    'tab_root' => '',
  );

  $items['requests/proxy-0'] = array(
    'title' => "Manage Proxies",
    'description' => "List users who are able to proxy as a venue coordinator.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_proxy_0_form'),
    'file' => 'request_proxy.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0/%'] = array(
    'title' => "Manage Proxies",
    'description' => "List users who are able to proxy as a venue coordinator.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_proxy_0_form', 2),
    'file' => 'request_proxy.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0/ajax'] = array(
    'title' => "Manage Proxies",
    'description' => "List users who are able to proxy as a venue coordinator.",
    'page callback' => 'mfcs_ajax_proxy_0_list',
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/proxy-0/delete/%'] = array(
    'title' => "Delete Proxy",
    'description' => "Delete a proxy",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_proxy_0_delete_form', 3),
    'file' => 'request_proxy.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('proxy'),
    'tab_parent' => 'requests/proxy-0',
    'tab_root' => 'requests/proxy-0',
  );

  $items['requests/proxy-0/autocomplete/user_id'] = array(
    'title' => "",
    'page callback' => 'mfcs_autocomplete_user_id',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/reviewers-0'] = array(
    'title' => "Manage Reviewers",
    'description' => "Review Request",
    'page callback' => 'mfcs_request_reviewers_0_page',
    'file' => 'request_reviewers.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/reviewers-0/list/%/%'] = array(
    'title' => "List Reviewers",
    'description' => "List Reviewers",
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
    'title' => "List Reviewers",
    'description' => "List Reviewers",
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
    'title' => "",
    'page callback' => 'mfcs_ajax_reviewers_0_list',
    'page arguments' => array(4, 5),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/delete/%'] = array(
    'title' => "Delete Reviewer",
    'description' => "Delete Reviewer",
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
    'title' => "",
    'page callback' => 'mfcs_autocomplete_user_id',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/reviewers-0/autocomplete/reviewer_classification'] = array(
    'title' => "",
    'page callback' => 'mfcs_request_reviewers_0_autocomplete_reviewer_classification',
    'file' => 'autocomplete.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/statistics-0'] = array(
    'title' => "Request Statistics",
    'description' => "Request Statistics",
    'page callback' => 'mfcs_request_statistics_0_page',
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/statistics-0/%'] = array(
    'title' => "Request Statistics",
    'description' => "Request Statistics",
    'page callback' => 'mfcs_request_statistics_0_page',
    'page arguments' => array(2),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/statistics-0/%/%'] = array(
    'title' => "Request Statistics",
    'description' => "Request Statistics",
    'page callback' => 'mfcs_request_statistics_0_page',
    'page arguments' => array(2, 3),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/statistics-0/%/%/%'] = array(
    'title' => "Request Statistics",
    'description' => "Request Statistics",
    'page callback' => 'mfcs_request_statistics_0_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'request_statistics.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('statistics'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0'] = array(
    'title' => "E-mail Logs",
    'description' => "E-mail Logs",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_form'),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0/ajax'] = array(
    'title' => "E-mail Logs",
    'description' => "E-mail Logs",
    'page callback' => 'mfcs_ajax_email_log_0_list',
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0/%'] = array(
    'title' => "E-mail Logs",
    'description' => "E-mail Logs",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_form', 2),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/email_log-0/view/%'] = array(
    'title' => "E-mail Logs",
    'description' => "E-mail Logs",
    'page callback' => 'mfcs_request_email_log_0_view_page',
    'page arguments' => array(3),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('email_log'),
    'tab_parent' => 'requests/email_log-0',
    'tab_root' => 'requests/email_log-0',
  );

  $items['requests/email_log-0/resend/%'] = array(
    'title' => "E-mail Logs",
    'description' => "E-mail Logs",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_resend_form', 3),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('email_log-resend'),
    'tab_parent' => 'requests/email_log-0',
    'tab_root' => 'requests/email_log-0',
  );

  $items['requests/email_log-0/resend/%/%'] = array(
    'title' => "E-mail Logs",
    'description' => "E-mail Logs",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_email_log_0_resend_form', 3, 4),
    'file' => 'request_email_log.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('email_log-resend'),
    'tab_parent' => 'requests/email_log-0',
    'tab_root' => 'requests/email_log-0',
  );

  $items['requests/troubleshoot-0'] = array(
    'title' => "Troubleshooting Tools",
    'description' => "Troubleshoot problems on the system using pre-built tools.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_troubleshoot_0_form'),
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/troubleshoot-0/locations'] = array(
    'title' => "Troubleshooting Tools - Locations",
    'description' => "A listing of locations available on the system with details.",
    'page callback' => 'mfcs_request_troubleshoot_0_locations_page',
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'requests/troubleshoot-0',
    'tab_root' => 'requests/troubleshoot-0',
  );

  $items['requests/troubleshoot-0/buildings'] = array(
    'title' => "Troubleshooting Tools - Buildings",
    'description' => "A listing of buildings available on the system with details.",
    'page callback' => 'mfcs_request_troubleshoot_0_buildings_page',
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'requests/troubleshoot-0',
    'tab_root' => 'requests/troubleshoot-0',
  );

  $items['requests/troubleshoot-0/rooms'] = array(
    'title' => "Troubleshooting Tools - Rooms",
    'description' => "A listing of rooms available on the system with details.",
    'page callback' => 'mfcs_request_troubleshoot_0_rooms_page',
    'file' => 'request_troubleshoot.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('troubleshoot'),
    'tab_parent' => 'requests/troubleshoot-0',
    'tab_root' => 'requests/troubleshoot-0',
  );

  $items['requests/problems-0'] = array(
    'title' => "Manage Problems",
    'description' => "Manage problems detected by the system.",
    'page callback' => 'mfcs_management_problems_page',
    'page arguments' => array(),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/problems-0/users'] = array(
    'title' => "User Problems",
    'description' => "View a list of users with problems that the system has detected.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_users_form'),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0',
    'tab_root' => 'requests/problems-0',
  );

  $items['requests/problems-0/users/ajax'] = array(
    'title' => "User Problems",
    'description' => "View a list of users with problems that the system has detected.",
    'page callback' => 'mfcs_ajax_problems_0_users_list',
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0',
    'tab_root' => 'requests/problems-0',
  );

  $items['requests/problems-0/users/%'] = array(
    'title' => "User Problems",
    'description' => "View a list of users with problems that the system has detected.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_users_form', 3),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0',
    'tab_root' => 'requests/problems-0',
  );

  $items['requests/problems-0/users/user/%'] = array(
    'title' => "User Problems",
    'description' => "Manage problems detected by the system that are specific to a single user.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_user_form', 4),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0/users',
    'tab_root' => 'requests/problems-0/users',
  );

  $items['requests/problems-0/users/resynchronize/%'] = array(
    'title' => "User Problems - Re-sync Account",
    'description' => "Attempt to re-synchronize user account to banner database.",
    'page callback' => 'mfcs_request_problems_0_users_resynchronize',
    'page arguments' => array(4),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0/users',
    'tab_root' => 'requests/problems-0/users',
  );

  $items['requests/problems-0/users/disabled_in_use/%'] = array(
    'title' => "User Problems - Disabled Account in Use",
    'description' => "Display open requests that are associated with a given disabled user.",
    'page callback' => 'mfcs_request_problems_0_users_disabled_in_use',
    'page arguments' => array(4),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0/users',
    'tab_root' => 'requests/problems-0/users',
  );

  $items['requests/problems-0/users/disabled_in_use/%/%'] = array(
    'title' => "User Problems - Disabled Account in Use",
    'description' => "Display open requests that are associated with a given disabled user.",
    'page callback' => 'mfcs_request_problems_0_users_disabled_in_use',
    'page arguments' => array(4, 5),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0/users',
    'tab_root' => 'requests/problems-0/users',
  );

  $items['requests/problems-0/users/disabled_in_use/ajax/%'] = array(
    'title' => "",
    'page callback' => 'mfcs_ajax_problems_0_disabled_in_use',
    'page arguments' => array(5),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'type' => MENU_CALLBACK,
    'tab_parent' => 'requests/problems-0/users',
    'tab_root' => 'requests/problems-0/users',
  );

  $items['requests/problems-0/requests'] = array(
    'title' => "Request Problems",
    'description' => "View a list of requests with problems that the system has detected.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_requests_form'),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0',
    'tab_root' => 'requests/problems-0',
  );

  $items['requests/problems-0/requests/ajax'] = array(
    'title' => "Request Problems",
    'description' => "View a list of requests with problems that the system has detected.",
    'page callback' => 'mfcs_ajax_problems_0_requests_list',
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0',
    'tab_root' => 'requests/problems-0',
  );

  $items['requests/problems-0/requests/ajax/%'] = array(
    'title' => "Request Problems",
    'description' => "View a list of requests with problems that the system has detected.",
    'page callback' => 'mfcs_ajax_problems_0_requests_list',
    'page arguments' => array(4),
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0',
    'tab_root' => 'requests/problems-0',
  );

  $items['requests/problems-0/requests/request/%'] = array(
    'title' => "Request Problems",
    'description' => "Manage problems detected by the system that are specific to a single request.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_problems_0_request_form', 4),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0/requests',
    'tab_root' => 'requests/problems-0/requests',
  );

  $items['requests/problems-0/requests/request/%/recheck'] = array(
    'title' => "Request Problems",
    'description' => "Re-check a single problem for a single request.",
    'page callback' => 'mfcs_request_problems_0_request_recheck',
    'page arguments' => array(4),
    'file' => 'request_problems.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('problems'),
    'tab_parent' => 'requests/problems-0/requests',
    'tab_root' => 'requests/problems-0/requests',
  );

  $items['requests/synchronize-0'] = array(
    'title' => "Synchronize Requests",
    'description' => "The synchronize ",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_synchronize_0_form'),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/synchronize-0/ajax'] = array(
    'title' => "Synchronize Requests",
    'description' => "The synchronize ",
    'page callback' => 'mfcs_ajax_synchronize_0_form',
    'file' => 'ajax.inc',
    'file path' => $module_path . '/includes',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/synchronize-0/%'] = array(
    'title' => "Synchronize Requests",
    'description' => "The synchronize ",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_synchronize_0_form', 2),
    'file' => 'request_synchronize.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_synchronize_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/holiday/year-0'] = array(
    'title' => "Holidays",
    'description' => "Holidays.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_form'),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/holiday/year-0/%'] = array(
    'title' => "Holidays",
    'description' => "Holidays.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_form', 3),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/holiday/year-0/%/edit'] = array(
    'title' => "Create Holidays",
    'description' => "Holidays.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_edit_form', 3),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/holiday/year-0/%/edit/%'] = array(
    'title' => "Edit Holidays",
    'description' => "Holidays.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_edit_form', 3, 5),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array(),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/holiday/year-0/%/delete/%'] = array(
    'title' => "Delete Holidays",
    'description' => "Holidays.",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_holiday_0_delete_form', 3, 5),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array(5, NULL, FALSE),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/holiday/year-0/%/view/%'] = array(
    'title' => "Holidays",
    'description' => "Holidays.",
    'page callback' => 'mfcs_request_holiday_0_page',
    'page arguments' => array(3, 5),
    'file' => 'request_holiday.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_holiday_request_access',
    'access arguments' => array(5, NULL, NULL),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
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
  $items['requests/help-0'] = array(
    'title' => "Help",
    'description' => "Online Documentation",
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/help-0/%'] = array(
    'title' => "Help",
    'description' => "Online Documentation",
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(2),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/help-0/%/%'] = array(
    'title' => "Help",
    'description' => "Online Documentation",
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(2, 3),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/help-0/%/%/%'] = array(
    'title' => "Help",
    'description' => "Online Documentation",
    'page callback' => 'mfcs_help_0_page',
    'page arguments' => array(2, 3, 4),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  // @todo: provide custom pdf downloading for help pages
  /*
  $items['requests/pdf-0/help'] = array(
    'title' => "Help PDF",
    'description' => "Download Help PDF",
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/pdf-0/help/%'] = array(
    'title' => "Help PDF",
    'description' => "Download Help PDF",
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(3),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/pdf-0/help/%/%'] = array(
    'title' => "Help PDF",
    'description' => "Download Help PDF",
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(3, 4),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('help'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
    'type' => MENU_CALLBACK,
  );

  $items['requests/pdf-0/help/%/%/%'] = array(
    'title' => "Help PDF",
    'description' => "Download Help PDF",
    'page callback' => 'mfcs_help_0_pdf_page',
    'page arguments' => array(3, 4, 5),
    'file' => 'help.inc',
    'file path' => $module_path . '/pages',
    'access callback' => 'mfcs_page_request_access',
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
    'title' => t("Recreate Oracle Cache"),
    'callback' => 'mfcs_oracle_recache_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_insurance_not_provided_alert'] = array(
    'title' => t("Insurance Not Provided Alert"),
    'callback' => 'mfcs_insurance_not_provided_alert_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_autocancel_old_requests'] = array(
    'title' => t("Auto-Cancel Old Reqests"),
    'callback' => 'mfcs_autocancel_old_requests_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_auto_block_accounts'] = array(
    'title' => t("Auto-Block Accounts"),
    'callback' => 'mfcs_auto_block_accounts_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  $items['mfcs_check_for_problems_requests'] = array(
    'title' => t("Check for Problems - Requests"),
    'callback' => 'mfcs_check_for_problems_requests_cron',
    'callback arguments' => array(),
    'enabled' => FALSE,
  );

  return $items;
}

/**
 * Internal function for hook_user_login().
 */
function mfcs_hook_user_login(&$edit, $user, $category) {
  if (!is_object($user)) {
    return;
  }

  // if the user does not have a valid banner id, then there is nothing to synchronize to.
  if (empty($user->field_user_banner_id['und'][0]['value']) || !is_numeric($user->field_user_banner_id['und'][0]['value'])) {
    return;
  }

  $roles = (array) $user->roles;
  $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
  if (array_key_exists($bannerless_exception_role_id->rid, $roles)) {
    // do not sync to banner for bannerless-exception.
    return;
  }
  mfcs_synchronize_user_settings();


  // trigger a save update if the users roles should be changed according to mfcs_process_user_banner_roles().
  $settings = array(
    'banner_id' => NULL,
    'primary' => (int) $user->uid,
    'status' => (int) $user->status,
  );

  if (isset($user->field_user_banner_id['und'][0]) && array_key_exists('value', $user->field_user_banner_id['und'][0])) {
    if (is_numeric($user->field_user_banner_id['und'][0]['value'])) {
      $settings['banner_id'] = (int) $user->field_user_banner_id['und'][0]['value'];
    }
  }

  $problems = MFCS_USER_PROBLEM_NONE;
  $changes = mfcs_process_user_banner_roles($user, $problems, $settings);

  $changes_roles = array();
  if (isset($changes['roles']) && is_array($changes['roles'])) {
    $changes_roles = array_keys($changes['roles']);
  }

  if (is_array($changes)) {
    if (count($user->roles) != count($changes_roles)) {
      user_save($user);
    }
    else {
      $difference = array_diff($user->roles, $changes_roles);
      if (!empty($difference)) {
        user_save($user);
      }
      else {
        $difference = array_diff($changes_roles, $user->roles);
        if (!empty($difference)) {
          user_save($user);
        }
      }
    }
  }
}

/**
 * Internal function for hook_user_insert().
 */
function mfcs_hook_user_insert(&$edit, $account, $category) {
  // if the user does not have a valid banner id, then there is nothing to synchronize to.
  if (empty($account->field_user_banner_id['und'][0]['value']) || !is_numeric($account->field_user_banner_id['und'][0]['value'])) {
    return;
  }

  $roles = (array) $account->roles;
  $bannerless_exception_role_id = user_role_load_by_name('bannerless exception');
  if (array_key_exists($bannerless_exception_role_id->rid, $roles)) {
    // do not sync to banner for bannerless-exception.
    return;
  }

  $result = mfcs_sync_banner_to_local_users($account->uid, $account->field_user_banner_id['und'][0]['value']);

  // account synchronization failed, so assign the appropriate problem flag.
  if ($result === FALSE) {
    $mfcs_users = mfcs_load_users($account->uid);

    $problems = MFCS_USER_PROBLEM_NONE + MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE;
    if (isset($mfcs_users[$account->uid])) {
      $problems = $mfcs_users[$account->uid]->problems;
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE) == 0) {
        $problems += MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE;
      }
    }

    mfcs_save_user($account->uid, array('problems' => $problems));
  }
}

/**
 * Internal function for hook_user_presave().
 */
function mfcs_hook_user_presave(&$edit, $account, $category) {
  if (!is_object($account) || !property_exists($account, 'uid')) {
    // for new accounts, the object will not be defined and therefore no user id exist during the user presave function.
    // nothing can be done yet.
    return;
  }

  if (isset($edit['field_user_primary_account']['und'][0]) && isset($account->field_user_primary_account['und'][0])) {
    if (!empty(array_diff($edit['field_user_primary_account']['und'][0], $account->field_user_primary_account['und'][0]))) {
      // re-synchronize account information following change.
      $result = mfcs_sync_banner_to_local_users($account->uid, $account->field_user_banner_id['und'][0]['value']);
    }
  }

  $roles = array();
  foreach ($account->roles as $role_id => $role_value) {
    $roles[$role_id] = $role_id;
  }
  unset($role_id);
  unset($role_value);

  if (isset($edit['roles']) && is_array($edit['roles'])) {
    foreach ($edit['roles'] as $role_id => $role_value) {
      if ($role_value == 0) {
        unset($roles[$role_id]);
      }
      else {
        $roles[$role_id] = $role_id;
      }
    }
    unset($role_id);
    unset($role_value);
  }

  $bannerless_exception = user_role_load_by_name('bannerless exception');
  if (array_key_exists($bannerless_exception->rid, $roles)) {
    // do not sync to banner for bannerless-exception.
    return;
  }

  $mfcs_users = mfcs_load_users($account->uid, FALSE, ($account->status == 0));
  $problems_original = MFCS_USER_PROBLEM_NONE;
  if (isset($mfcs_users[$account->uid])) {
    $problems_original = $mfcs_users[$account->uid]->problems;
  }
  unset($mfcs_users);
  $problems = $problems_original;

  $settings = array(
    'banner_id' => NULL,
    'primary' => $account->uid,
    'roles' => array(),
  );

  if (array_key_exists('status', $edit)) {
    $settings['status'] = (int) $edit['status'];
  }

  if (!isset($edit['field_user_banner_id']['und']) && isset($account->field_user_banner_id['und'][0]) && array_key_exists('value', $account->field_user_banner_id['und'][0])) {
    if (is_numeric($account->field_user_banner_id['und'][0]['value'])) {
      $settings['banner_id'] = (int) $account->field_user_banner_id['und'][0]['value'];
    }
  }

  foreach ($roles as $role_id) {
    $loaded_role = user_role_load($role_id);
    if (is_object($loaded_role)) {
      $settings['roles'][$role_id] = $loaded_role->name;
    }
  }

  if (isset($edit['field_user_banner_id']['und'][0]) && array_key_exists('value', $edit['field_user_banner_id']['und'][0])) {
    $settings['banner_id'] = $edit['field_user_banner_id']['und'][0]['value'];
    if (is_null($settings['banner_id']) || (is_string($settings['banner_id']) && strlen($settings['banner_id']) == 0)) {
      $settings['banner_id'] = NULL;
    }
    elseif (!is_int($settings['banner_id'])) {
      $settings['banner_id'] = (int) $settings['banner_id'];
    }
  }

  if (is_null($settings['banner_id'])) {
    // do not perform any banner-specific changes when banner id is not specified.
    return;
  }

  if (isset($edit['field_user_primary_account']['und'][0]) && array_key_exists('value', $edit['field_user_primary_account']['und'][0])) {
    $settings['primary'] = $edit['field_user_primary_account']['und'][0]['value'];
    if (empty($settings['primary'])) {
      unset($settings['primary']);
    }
    elseif (!is_int($settings['banner_id'])) {
      $settings['primary'] = (int) $settings['banner_id'];
    }
  }

  $changes = mfcs_process_user_banner_roles($account, $problems, $settings);
  if (is_array($changes)) {
    if (isset($changes['roles']) && is_array($changes['roles'])) {
      $edit['roles'] = array();
      foreach ($changes['roles'] as $role_id => $role_name) {
        $edit['roles'][$role_id] = (int) $role_id;
      }
    }

    $edit['field_user_event_roles'] = $changes['field_user_event_roles'];
    if (array_key_exists('status', $changes)) {
      $edit['status'] = $changes['status'];
    }


    // if status has changed, and the account is disabled, then unset all roles.
    if (isset($edit['status'])) {
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


  if ($problems != $problems_original) {
    mfcs_save_user($account->uid, array('problems' => $problems));
  }
}

/**
 * Internal function for hook_user_update().
 */
function mfcs_hook_user_update(&$edit, $account, $category) {
  if (!array_key_exists('status', $edit) || !isset($edit['original']->status) || !isset($account->uid)) {
    return;
  }

  // if the status has changed, perform a user problem check.
  if ($edit['status'] != $edit['original']->status) {
    $disabled_with_active = mfcs_disabled_users_with_active_content($account->uid);

    $mfcs_users = mfcs_load_users($account->uid);

    $problems = MFCS_USER_PROBLEM_NONE;
    if (is_array($mfcs_users) && array_key_exists($account->uid, $mfcs_users)) {
      $problems = $mfcs_users[$account->uid]->problems;
    }

    if ($disabled_with_active) {
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) == 0) {
        $problems = ($problems | MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE);
        mfcs_save_user($account->uid, array('problems' => $problems));
      }
    }
    else {
      if (($problems & MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE) != 0) {
        $problems -= MFCS_USER_PROBLEM_ACCOUNT_DISABLED_IN_USE;
        mfcs_save_user($account->uid, array('problems' => $problems));
      }
    }
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
