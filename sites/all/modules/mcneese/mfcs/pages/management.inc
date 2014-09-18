<?php

/**
 * @file
 * Defines McNeese facilities use management page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request manage page.
 *
 * @return string
 *   The HTML output representing the page.
 *
 * @see mfcs_blocks_at_path()
 */
function mfcs_management_page() {
  global $base_path;
  $user = cf_current_user();

  mfcs_add_canonical_headers('requests/management');

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);

  $items_1 = array();
  $items_2 = array();
  $items_3 = array();
  $items_4 = array();

  $items_array = &$items_1;
  $items_array_current = 1;

  if ($manager) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/reviewers-0" class="item-link">Manage Reviewers</a></h4>';
    $help = '<div class="item-description inline-block">Manage what users are allowed to approve/deny requests by request type.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if (count($items_array) > 0) {
    if ($items_array_current == 1) {
      $items_array = &$items_2;
      $items_array_current++;
    }
  }

  if ($manager || $reviewer) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/statistics-0" class="item-link">Request Statistics</a></h4>';
    $help = '<div class="item-description inline-block">View statistics for requests on this system that you have access to view.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if (count($items_array) > 0) {
    if ($items_array_current == 1) {
      $items_array = &$items_3;
      $items_array_current++;
    }
  }

  $item_list_1 = theme('item_list', array('items' => $items_1, 'type' => 'ul'));
  $item_list_2 = theme('item_list', array('items' => $items_2, 'type' => 'ul'));

  $markup = '<div id="mfcs-management_page-choices" class="mfcs-management_page-choices">';

  if (!empty($items_1)) {
    $markup .= '<div class="mfcs-management_page-choices-list_1">' . $item_list_1 . '</div>';
  }

  if (!empty($items_2)) {
    $markup .= '<div class="mfcs-management_page-choices-list_2">' . $item_list_2 . '</div>';
  }

  $markup .= '</div>';

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */