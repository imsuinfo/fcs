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
 * Internal function for hook_permission.
 */
function mfcs_hook_menu() {
  $items = array();

  $items['requests'] = array(
    'title' => "Requests Dashboard",
    'description' => "Requests Dashboard",
    'page callback' => 'mfcs_management_page',
    'file' => 'management_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_management_page_access',
    'tab_parent' => '',
    'tab_root' => '',
  );

  $items['requests/create-0'] = array(
    'title' => "New Request",
    'description' => "New Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_create_0_form'),
    'file' => 'request_create_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_create_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/create-0/autocomplete/request_coordinator/email'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_email',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/create-0/autocomplete/request_coordinator/name'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_name',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('create'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/create-0/autocomplete/request_coordinator/phone'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_phone',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
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
    'file' => 'request_edit_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_edit_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit', 2, NULL, 3),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/edit-0/autocomplete/request_coordinator/email'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_email',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/edit-0/autocomplete/request_coordinator/name'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_name',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/edit-0/autocomplete/request_coordinator/phone'] = array(
    'title' => "",
    'page callback' => 'mfcs_edit_0_autocomplete_request_coordinator_phone',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('edit'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/reviewers-0'] = array(
    'title' => "Manage Reviewers",
    'description' => "Review Request",
    'page callback' => 'mfcs_request_reviewers_0_page',
    'file' => 'request_reviewers_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_reviewers_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/delete/%'] = array(
    'title' => "Delete Reviewer",
    'description' => "Delete Reviewer",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_reviewers_0_delete_form', 3),
    'file' => 'request_reviewers_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'tab_parent' => 'requests/reviewers-0',
    'tab_root' => 'requests/reviewers-0',
  );

  $items['requests/reviewers-0/autocomplete/user_id'] = array(
    'title' => "",
    'page callback' => 'mfcs_request_reviewers_0_autocomplete_user_id',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/reviewers-0/autocomplete/reviewer_classification'] = array(
    'title' => "",
    'page callback' => 'mfcs_request_reviewers_0_autocomplete_reviewer_classification',
    'file' => 'autocomplete.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/includes',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('reviewers'),
    'type' => MENU_CALLBACK,
  );

  $items['requests/review-0'] = array(
    'title' => "Review Requests",
    'description' => "Review Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_review_0_page'),
    'file' => 'request_review_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_review_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('review'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/delete-0/%'] = array(
    'title' => "Delete Request",
    'description' => "Delete Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_delete_0_form', 2),
    'file' => 'request_delete_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_cancel_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_cancel_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('uncancel', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/list-0'] = array(
    'title' => "List Requests",
    'description' => "List Requests",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_list_0_page'),
    'file' => 'request_list_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/calendar-0/month'] = array(
    'title' => "Request Calendar",
    'description' => "Request Calendar - Month",
    'page callback' => 'mfcs_request_calendar_month_0_page',
    'page arguments' => array(),
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_calendar_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_ical_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/ical-0/month'] = array(
    'title' => "Request iCal",
    'description' => "Request iCal - Month",
    'page callback' => 'mfcs_request_ical_month_0_page',
    'page arguments' => array(NULL, NULL),
    'file' => 'request_ical_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_ical_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_ical_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_ical_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('list'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/list-0/%'] = array(
    'title' => "List Requests",
    'description' => "List Requests",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_list_0_page', 2),
    'file' => 'request_list_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_search_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('search'),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  $items['requests/view-0/%'] = array(
    'title' => "View Request",
    'description' => "View Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_view_0_page', 2),
    'file' => 'request_view_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['requests/view-0/%/%'] = array(
    'title' => "View Request",
    'description' => "View Request",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_view_0_page', 2, 3),
    'file' => 'request_view_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
  );

  $items['requests/pdf-0/%/%'] = array(
    'title' => "Request PDF",
    'description' => "Download Request PDF",
    'page callback' => 'mfcs_request_pdf_0_page',
    'page arguments' => array(2, 3),
    'file' => 'request_view_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('view', 2),
    'tab_parent' => 'requests/list-0',
    'tab_root' => 'requests/list-0',
    'type' => MENU_CALLBACK,
  );

  $items['requests/history-0/%'] = array(
    'title' => "Request History",
    'description' => "Request History",
    'page callback' => 'drupal_get_form',
    'page arguments' => array('mfcs_request_history_0_form', 2),
    'file' => 'request_history_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_agreement_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
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
    'file' => 'request_agreement_page.inc',
    'file path' => drupal_get_path('module', 'mfcs') . '/pages',
    'access callback' => 'mfcs_page_request_access',
    'access arguments' => array('agreement', 2),
    'tab_parent' => 'requests',
    'tab_root' => 'requests',
  );

  return $items;
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

  return $items;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
