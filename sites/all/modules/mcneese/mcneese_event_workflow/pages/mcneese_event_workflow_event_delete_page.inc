<?php

/**
 * @file
 * Defines McNeese event workflow event delete page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event delete page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $id
 *   The unique identifier for an event request.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_delete_0_page($form, &$form_state, $id) {
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
