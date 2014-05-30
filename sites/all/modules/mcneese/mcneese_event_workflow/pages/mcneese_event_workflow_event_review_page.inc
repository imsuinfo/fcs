<?php

/**
 * @file
 * Defines McNeese event workflow event review page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

 /**
 * Provides the event workflow event review page.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_review_0_page() {
  $form = array();

  return $form;
}

/**
 * Provides the event workflow event review form.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_review_0_form($form, &$form_state) {
  if (!cf_is_integer($id)) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $form = array();

  return $form;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
