<?php

/**
 * @file
 * Defines McNeese content page functions.
 */

/**
 * @addtogroup mcneese_content
 * @{
s*/

/**
 * Provides the XML sitemap for robots to process.
 *
 * @param string|null $group
 *   A category/group to process.
 * @param int $offset
 *   A numeric id that is used in conjunction with either
 *   MCNEESE_CONTENT_SITEMAP_NODE_MAX or MCNEESE_CONTENT_SITEMAP_MEDIA_MAX
 *   to determine the sequence of links to show.
 *
 * @see: mcneese_content_sitemap_main
 * @see: mcneese_content_sitemap_group_node
 * @see: mcneese_content_sitemap_group_media
 */
function mcneese_content_sitemap_page($group = NULL, $offset = 0) {
  if (is_null($group)) {
    return mcneese_content_sitemap_main();
  }
  elseif ($group == 'node') {
    return mcneese_content_sitemap_group_node($offset);
  }
/*
  elseif ($group == 'media') {
    return mcneese_content_sitemap_group_media($offset);
  }
*/

  drupal_not_found();
  drupal_exit();
}

/**
 * Provides the XML sitemap index for robots to process.
 *
 * This function calls drupal_exit() when it is done, unless there is an
 * error. This prevents the drupal theming engine from functioning here,
 * allowing for a true XML sitemap page.
 */
function mcneese_content_sitemap_main() {
  global $base_url, $base_path;

  // get a total node count.
  try {
    $query = db_select('node', 'n');
    $query->fields('n', array('nid', 'changed'));

    $query->condition('n.status', 1);
    $query->orderBy('n.changed', 'DESC');

    $executed = $query->execute();
    $total_nodes = $executed->rowCount();
    $node_pages = (int) ($total_nodes / MCNEESE_CONTENT_SITEMAP_NODE_MAX);

    if ($total_nodes % MCNEESE_CONTENT_SITEMAP_NODE_MAX > 0) {
      $node_pages++;
    }

    $row = $executed->fetchAssoc();
    $node_last_updated = $row['changed'];
  }
  catch (Error $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }

  /*
  // get a total media (file) count.
  try {
    $query = db_select('file_managed', 'fm');
    $query->fields('fm', array('fid', 'timestamp'));

    $query->condition('fm.status', 1);
    $query->orderBy('fm.timestamp', 'DESC');

    $or = db_or();
    $or->condition('fm.uri', 'public://%', 'LIKE');
    $or->condition('fm.uri', 'dbu://%', 'LIKE');
    $query->condition($or);

    $executed = $query->execute();
    $total_media = $executed->rowCount();
    $media_pages = (int) ($total_media / MCNEESE_CONTENT_SITEMAP_MEDIA_MAX);

    if ($total_media % MCNEESE_CONTENT_SITEMAP_MEDIA_MAX > 0) {
      $media_pages++;
    }

    $row = $executed->fetchAssoc();
    $media_last_updated = $row['timestamp'];
  }
  catch (Error $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }
  */


  // make sure the browser realizes that this is not a web page and is instead an xml page.
  drupal_add_http_header('content-type', 'application/xml');
  drupal_send_headers();

  print('<?xml version="1.0" encoding="UTF-8"?>' . "\n");
  print('<sitemapindex xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' . "\n");

  $url = $base_url . '/sitemap/node/';
  for ($i = 0; $i < $node_pages; $i++) {
    print('  <sitemap>' . "\n");
    print('    <loc>' . $url . $i . '</loc>' . "\n");
    print('    <lastmod>' . date('c', $node_last_updated) . '</lastmod>' . "\n");
    print('  </sitemap>' . "\n");
  }

/*
  $url = $base_url . '/sitemap/media/';
  for ($i = 0; $i < $media_pages; $i++) {
    print('  <sitemap>' . "\n");
    print('    <loc>' . $url . $i . '</loc>' . "\n");
    print('    <lastmod>' . date('c', $media_last_updated) . '</lastmod>' . "\n");
    print('  </sitemap>' . "\n");
  }
*/

  print('</sitemapindex>' . "\n");
  drupal_exit();
}

/**
 * Provides the XML sitemap of nodes for robots to process.
 *
 * @param int $offset
 *   A numeric id that is used in conjunction with MCNEESE_CONTENT_SITEMAP_NODE_MAX
 *   to determine the sequence of links to show.
 *
 * This function calls drupal_exit() when it is done, unless there is an
 * error. This prevents the drupal theming engine from functioning here,
 * allowing for a true XML sitemap page.
 */
function mcneese_content_sitemap_group_node($offset) {
  global $base_url, $base_path;
  $rows = '';

  $offset_value = $offset * MCNEESE_CONTENT_SITEMAP_NODE_MAX;
  $url = $base_url . '/node/';

  // first load all nodes and handle errors before output is printed.
  try {
    $query = db_select('node', 'n');
    $query->fields('n', array('nid', 'changed'));

    $query->condition('n.status', 1);
    $query->orderBy('n.nid');
    $query->range($offset_value, MCNEESE_CONTENT_SITEMAP_NODE_MAX);

    $executed = $query->execute();

    if ($executed->rowCount() == 0) {
      drupal_not_found();
      drupal_exit();
    }

    $nids = array();
    foreach ($executed as $row) {
      $nids[$row->nid] = $row->changed;
    }

    if (function_exists('workbench_moderation_menu')) {
      $query = db_select('node', 'n');
      $query->innerJoin('workbench_moderation_node_history', 'wmnh', 'n.nid = wmnh.nid AND n.vid = wmnh.vid AND wmnh.published = 1');
      $query->fields('n', array('nid'));
      $query->fields('wmnh', array('stamp'));

      $query->orderBy('n.nid');
      $query->condition('n.status', 1);
      $query->condition('n.nid', array_keys($nids), 'IN');

      $executed = $query->execute();

      foreach ($executed as $row) {
        $nids[$row->nid] = $row->stamp;
      }
    }

  }
  catch (Error $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }

  // make sure the browser realizes that this is not a web page and is instead an xml page.
  drupal_add_http_header('content-type', 'application/xml');
  drupal_send_headers();

  print('<?xml version="1.0" encoding="UTF-8"?>' . "\n");
  print('<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' . "\n");
  foreach ($nids as $nid => &$changed) {
    print('  <url>' . "\n");
    print('    <loc>' . $url . $nid . '</loc>' . "\n");
    print('    <lastmod>' . date('c', $changed) . '</lastmod>' . "\n");
    print('  </url>' . "\n");
  }
  print('</urlset>' . "\n");
  drupal_exit();
}

/**
 * Provides the XML sitemap of media for robots to process.
 *
 * @param int $offset
 *   A numeric id that is used in conjunction with MCNEESE_CONTENT_SITEMAP_MEDIA_MAX
 *   to determine the sequence of links to show.
 *
 * This function calls drupal_exit() when it is done, unless there is an
 * error. This prevents the drupal theming engine from functioning here,
 * allowing for a true XML sitemap page.
 */
function mcneese_content_sitemap_group_media($offset) {
  global $base_url;
  $rows = '';

  $offset_value = $offset * MCNEESE_CONTENT_SITEMAP_MEDIA_MAX;
  $url = $base_url;

  // first load all nodes and handle errors before output is printed.
  try {
    $query = db_select('file_managed', 'fm');
    $query->fields('fm', array('uri', 'timestamp'));
    $query->condition('fm.status', 1);

    $or = db_or();
    $or->condition('fm.uri', 'public://%', 'LIKE');
    $or->condition('fm.uri', 'dbu://%', 'LIKE');
    $query->condition($or);

    $executed = $query->execute();

    if ($executed->rowCount() == 0) {
      drupal_not_found();
      drupal_exit();
    }

    $rows = '';
    $wrappers = array();
    foreach ($executed as $row) {
      $path = NULL;

      // calling file_create_url() is very slow, so instead determine external url ourself.
      $scheme = file_uri_scheme($row->uri);
      if ($scheme == 'http' || $scheme == 'https') {
        $path = $uri;
      }
      elseif (isset($wrappers[$scheme]) && is_object($wrappers[$scheme])) {
        $wrappers[$scheme]->setUri($row->uri);

        $path = $url . $wrappers[$scheme]->getExternalUrl();
      }
      else {
        if (!isset($wrappers[$scheme])) {
          $wrappers[$scheme] = file_stream_wrapper_get_instance_by_uri($row->uri);
        }
        if (is_object($wrappers[$scheme])) {
          if ($wrappers[$scheme] instanceOf mcneese_file_db_stream_wrapper) {
            $wrappers[$scheme]->set_fast_build(TRUE);
          }

          $wrappers[$scheme]->setUri($row->uri);
          $path = $url . $wrappers[$scheme]->getExternalUrl();
        }
      }

      if (!is_null($path)) {
        $rows .= '  <url>' . "\n";
        $rows .= '    <loc>' . $path . '</loc>' . "\n";
        $rows .= '    <lastmod>' . date('c', $row->timestamp) . '</lastmod>' . "\n";
        $rows .= '  </url>' . "\n";
      }
    }
  }
  catch (Error $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }

    throw $e;
  }

  // make sure the browser realizes that this is not a web page and is instead an xml page.
  drupal_add_http_header('content-type', 'application/xml');
  drupal_send_headers();

  print('<?xml version="1.0" encoding="UTF-8"?>' . "\n");
  print('<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' . "\n");
  print($rows);
  print('</urlset>' . "\n");
  drupal_exit();
}
