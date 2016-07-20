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
 */
function mfcs_management_page() {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  mfcs_add_canonical_headers('requests/management');

  $requester = user_access('mfcs request', $user);
  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $administer = user_access('mfcs administer', $user);

  $items_1 = array();
  $items_2 = array();

  $items_array = &$items_1;
  $items_array_current = 0;

  if ($manager || $administer) {
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/reviewers-0' . $url_arguments . '" class="item-link">Manage Reviewers</a></h4>';
    $help = '<div class="item-description inline-block">Manage what users are allowed to approve/deny requests by request type.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($reviewer || $manager || $administer) {
    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    $today = strtotime('midnight today');
    $year = date('Y', $today);
    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/statistics-0/' . $year . $url_arguments . '" class="item-link">Request Statistics</a></h4>';
    $help = '<div class="item-description inline-block">View statistics for requests on this system that you have access to view.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($manager || $administer) {
    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/proxy-0' . $url_arguments . '" class="item-link">Manage Proxies</a></h4>';
    $help = '<div class="item-description inline-block">Manage which venue coordinators a given user is allowed to proxy as.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($manager || $administer) {
    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/email_log-0' . $url_arguments . '" class="item-link">E-mail Logs</a></h4>';
    $help = '<div class="item-description inline-block">View the e-mails sent by this system.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );

    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/users-0/list' . $url_arguments . '" class="item-link">Manage Users</a></h4>';
    $help = '<div class="item-description inline-block">View, add, delete, and otherwise manage users.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );

    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/synchronize-0' . $url_arguments . '" class="item-link">Manage Synchronizations</a></h4>';
    $help = '<div class="item-description inline-block">View and manage list of links for synchronizing, such as iCal (calendar) synchronization.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );

    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/problems-0' . $url_arguments . '" class="item-link">Manage Problems</a></h4>';
    $help = '<div class="item-description inline-block">Manage and troubleshoot problems that have been detected by the system.</div>';
    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($requester || $reviewer || $manager || $administer) {
    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    if ($reviewer || $manager || $administer) {
      $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/holiday-0' . $url_arguments . '" class="item-link">Manage Holidays</a></h4>';
      $help = '<div class="item-description inline-block">View and manage the list of holidays.</div>';
    }
    else {
      $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/holiday-0' . $url_arguments . '" class="item-link">View Holidays</a></h4>';
      $help = '<div class="item-description inline-block">View the list of holidays.</div>';
    }

    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  if ($requester || $reviewer || $manager || $administer) {
    $c_1 = count($items_1);
    $c_2 = count($items_2);
    if ($c_1 == $c_2) {
      $items_array = &$items_1;
    }
    elseif ($c_1 > $c_2) {
      $items_array = &$items_2;
    }
    else {
      $items_array = &$items_1;
    }

    if ($reviewer || $manager || $administer) {
      $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/unavailable-0' . $url_arguments . '" class="item-link">Room Unavailability</a></h4>';
      $help = '<div class="item-description inline-block">Manage unavailability dates for specific rooms.</div>';
    }
    else {
      $link = '<h4 class="item-link-wrapper inline-block"><a href="' . $base_path . 'requests/unavailable-0' . $url_arguments . '" class="item-link">Room Unavailability</a></h4>';
      $help = '<div class="item-description inline-block">View unavailability dates for specific rooms.</div>';
    }

    $items_array[] = array(
      'data' => $link . '<br>' . $help,
      'class' => array(
      ),
    );
  }

  $markup = '<div id="mfcs-management_page-choices" class="mfcs-management_page-choices">';

  if (!empty($items_1)) {
    $item_list_1 = theme('item_list', array('items' => $items_1, 'type' => 'ul'));
    $markup .= '<div class="mfcs-management_page-choices-list_1">' . $item_list_1 . '</div>';
  }

  if (!empty($items_2)) {
    $item_list_2 = theme('item_list', array('items' => $items_2, 'type' => 'ul'));
    $markup .= '<div class="mfcs-management_page-choices-list_2">' . $item_list_2 . '</div>';
  }

  $markup .= '</div>';

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
