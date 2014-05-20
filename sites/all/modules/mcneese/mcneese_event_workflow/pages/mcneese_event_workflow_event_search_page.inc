<?php

/**
 * @file
 * Defines McNeese event workflow event search page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event search page.
 *
 * @param int $page_number
 *   (optional) When there are more than a certain number of rows,
 *   this represent the page number for a given sequence of rows.
 *
 * @return array
 *   An array containing the generated form.
 */
function mcneese_event_workflow_event_search_0_page($page_number = 0) {
  if (!cf_is_integer($page_number)) {
    // because this is a url argument passed as a variable, just force a 0 for invalid paths.
    $page_number = 0;
  }

  $form = array();

  return $form;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
