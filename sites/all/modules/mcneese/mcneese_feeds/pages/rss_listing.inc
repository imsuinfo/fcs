<?php

/**
 * @file
 * Defines rss feed listing functions.
 */

/**
 * @addtogroup mcneese_feeds
 * @{
 */

/**
 * Provides the main RSS feed listing page.
 */
function mcneese_feeds_rss_listing() {
  $user = cf_current_user();

  if (!is_object($user) || $user->uid < 1) {
    drupal_not_found();
    drupal_exit();
  }

  $output = '';
  $rows = array();

  $feed_url = 'rss/feed';

  // database sorting.
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  $rss_feeds = array();
  drupal_alter('mcneese_feeds_rss_listing', $rss_feeds);

  foreach ($rss_feeds as $rss_feed) {
    $row = array();

    $path = url($rss_feed['path']);

    if (isset($rss_feed['no_link']) && $rss_feed['no_link']) {
      $row[] = $rss_feed['name'] . '<br><br><strong>Path</strong>: ' . $path;
    }
    else {
      $row[] = '<a href="' . $path . '">' . $rss_feed['name'] . '</a><br><br><strong>Path</strong>: ' . $path;
    }
    $row[] = $rss_feed['description'];
    $rows[] = $row;
  }

  $header = array();
  $header[] = array(
    'data' => t("Feed"),
    'field' => 'feed_name',
    'align' => 'center',
  );
  $header[] = array(
    'data' => t("Description"),
    'field' => 'feed_description',
    'align' => 'center',
  );

  if ($order == 'feed') {
    $header[0]['sort'] = strtolower($sort);
  }
  elseif ($order == 'description') {
    $header[1]['sort'] = strtolower($sort);
  }
  else {
    $header[0]['sort'] = 'asc';
  }

  $attributes = array();
  $attributes['id'] = 'mcneese_feeds_rss_listing';
  $attributes['summary'] = t("A listing of RSS Feeds available for viewing.");
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t("RSS Feeds"), 'attributes' => $attributes, 'empty' => t("There are no RSS Feeds available for you to view.")));

  return $output;
}

/**
 * @} End of '@addtogroup mcneese_feeds'.
 */
