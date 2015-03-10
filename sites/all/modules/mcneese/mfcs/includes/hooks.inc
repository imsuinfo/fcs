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
    'access callback' => TRUE, // prevent drupals access callback from being called.
  );

  $items['requests/create-0'] = array(
    'title' => "New Request",
    'description' => "New Request",
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
    'title' => "New Request",
    'description' => "New Request",
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
    'title' => "Cancel Request",
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

  return $items;
}

/**
 * Internal function for hook_user_login().
 */
function mfcs_hook_user_login(&$edit, $account, $category) {
  mfcs_synchronize_user_settings();
}

/**
 * Internal function for hook_user_insert().
 */
function mfcs_hook_user_insert(&$edit, $account, $category) {
  $result = mfcs_sync_banner_to_local_users($account->uid);

  // account synchronization failed, so assign the appropriate problem flag.
  if ($result === FALSE) {
    // if the user does not have a valid banner id, then there is nothing to syncrhonize to.
    // do not flag this as a problem.
    $user = user_load($account->uid);
    if (is_object($user) && empty($user->field_user_banner_id['und'][0]['value'])) {
      return;
    }

    $mfcs_users = mfcs_load_users($account->uid);

    $problems = MFCS_USER_PROBLEM_NONE + MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE;
    if (isset($mfcs_users[$account->uid])) {
      $problems = $mfcs_users[$account->uid]->problems;
      $problems += MFCS_USER_PROBLEM_ACCOUNT_CREATE_BANNER_UNAVAILABLE;
    }

    mfcs_save_user($account->uid, array('problems' => $problems));
  }
}

/**
 * @} End of '@addtogroup mfcs'.
 */
