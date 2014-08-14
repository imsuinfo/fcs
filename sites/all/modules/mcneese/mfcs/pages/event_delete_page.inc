<?php

/**
 * @file
 * Defines McNeese facilities use event delete page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use event delete page.
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
function mfcs_event_delete_0_form($form, &$form_state, $id) {
  if (!cf_is_integer($id)) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $form = array();

  return $form;
}

/**
 * @