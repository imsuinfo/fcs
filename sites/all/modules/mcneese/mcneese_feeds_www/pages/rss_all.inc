<?php

/**
 * @file
 * Defines rss feed listing all functions.
 *
 */

/**
 * @addtogroup mcneese_feeds_www
 * @{
 */

/**
 * Output the RSS Feed "All".
 */
function mcneese_feeds_www_rss_all($type = NULL) {
  if (!is_null($type)) {
    if (!is_string($type) || empty($type)) {
      $type = NULL;
    }
  }

  $types = array();
  if (is_string($type)) {
    $types = explode(MCNEESE_FEEDS_GROUPING, $type);

    // the database stores types as all lower case, so force all passed types to be lower case to allow for case-insensitive urls.
    foreach ($types as &$type) {
      $type = strtolower($type);
    }
  }

  global $base_url;
  global $base_path;
  global $user;
  global $conf;

  require_once(drupal_get_path('module', 'mcneese_feeds') . '/classes/rss.inc');

  $headers = array();
  $output = '';
  $rows = array();
  $categories = array();
  $feed_url = 'rss/feed/all';

  if (!is_null($type)) {
    $feed_url = 'rss/feed/all/' . $type;
  }

  $blacklist_types = array();
  if (isset($conf['feed_all_blacklist']['types']) && is_array($conf['feed_all_blacklist']['types'])) {
    $blacklist_types = $conf['feed_all_blacklist']['types'];
  }

  $blacklist_nodes = array();
  if (isset($conf['feed_all_blacklist']['nodes'][NULL]) && is_array($conf['feed_all_blacklist']['nodes'][NULL])) {
    $blacklist_nodes = $conf['feed_all_blacklist']['nodes'][NULL];
  }

  if (!is_null($type) && in_array($type, $blacklist_types)) {
    drupal_not_found();
    drupal_exit();
  }

  mcneese_feeds_process_if_none_match($feed_url);

  $channel = new McNeeseFeedsRssChannel();
  $channel->assign_common_required("McNeese State University - Feed", $base_url, "Published content available on the McNeese State University website.");
  $channel->assign_common_optional(NULL, REQUEST_TIME, date("Y", REQUEST_TIME) . "© McNeese State University");

  // The date of the most recent item on the feed should be used here.
  $last_build_date = REQUEST_TIME;
  $original_timezone = date_default_timezone_get();

  try {
    $query = db_select('node', 'n');
    $query->innerJoin('field_data_field_order_date', 'fdfod', 'n.nid = fdfod.entity_id and fdfod.entity_type = \'node\' and fdfod.deleted = 0');
    $query->leftJoin('workbench_moderation_node_history', 'wmnh', 'n.nid = wmnh.nid and n.vid = wmnh.vid and wmnh.published = 1');

    $query->addField('n', 'nid', 'nid');
    $query->addField('n', 'title', 'title');
    $query->addField('n', 'changed', 'changed');
    $query->addField('fdfod', 'field_order_date_value', 'order_date');

    $query->condition('n.status', 1);
    $query->condition('n.type', 'system-page', '<>');

    if (isset($conf['feed_all_blacklist']['nodes']) && is_array($conf['feed_all_blacklist']['nodes'])) {
      foreach ($types as $t) {
        if (isset($conf['feed_all_blacklist']['nodes'][$t]) && is_array($conf['feed_all_blacklist']['nodes'][$t])) {
          $blacklist_nodes = array_merge($blacklist_nodes, $conf['feed_all_blacklist']['nodes'][$t]);
        }
      }
      unset($t);
    }

    if (!empty($blacklist_nodes)) {
      $query->condition('n.nid', $blacklist_nodes, 'NOT IN');
    }

    if (is_null($type)) {
      if (!empty($blacklist_types)) {
        $query->condition('n.type', $blacklist_types, 'NOT IN');
      }
    }
    else {
      if (!empty($blacklist_types)) {
        $types_clone = $types;
        foreach ($types_clone as $k => $t) {
          if (in_array($t, $blacklist_types)) {
            unset($types[$k]);
          }
        }
        unset($types_clone);
        unset($t);
        unset($k);
      }

      $query->condition('n.type', $types, 'IN');

      $query_types = db_select('node_type', 'nt');
      $query_types->addField('nt', 'name', 'name');
      $query_types->condition('nt.type', $types, 'IN');

      $results_types = $query_types->execute()->fetchAll();

      if (empty($results_types)) {
        drupal_not_found();
        drupal_exit();
      }

      $results_label = NULL;
      foreach ($results_types as $results_type) {
        if (is_null($results_label)) {
          $results_label = $results_type->name;
          continue;
        }

        $results_label .= ', ' . $results_type->name;
      }

      $channel->set_title("McNeese State University - Feed - " . $results_label);
      $channel->set_description("Published content available on the McNeese State University website that is categorized as: " . $results_label);

      unset($results_type);
      unset($results_types);
      unset($results_label);
      unset($query_type);
    }

    $query->orderBy('fdfod.field_order_date_value', 'desc');
    //$query->orderBy('wmnh.stamp', 'desc');
    $query->orderBy('n.changed', 'desc');

    $query->range(0, MCNEESE_FEEDS_WWW_FEED_LIMIT);

    $results = $query->execute()->fetchAllAssoc('nid');

    if (!empty($results)) {
      $node_ids = array_keys($results);
      $nodes = node_load_multiple($node_ids);
      $nodes_full = node_view_multiple($nodes, 'full');
      $nodes_teaser = node_view_multiple($nodes, 'teaser');

      foreach ($results as $nid => $result) {
        $item = new McNeeseFeedsRssItem();

        if (!isset($nodes_full['nodes'][$nid])) {
          continue;
        }

        if (isset($nodes_full['nodes'][$nid]['body'][0]['#markup'])) {
          $nodes_full['nodes'][$nid]['body'][0]['#markup'] = preg_replace('/&nbsp;/i', ' ', $nodes_full['nodes'][$nid]['body'][0]['#markup']);
          $nodes_full['nodes'][$nid]['body'][0]['#markup'] = preg_replace('/\s+/m', ' ', $nodes_full['nodes'][$nid]['body'][0]['#markup']);
        }

        if (isset($nodes_teaser['nodes'][$nid]['body'][0]['#markup'])) {
          $nodes_teaser['nodes'][$nid]['body'][0]['#markup'] = preg_replace('/&nbsp;/i', ' ', $nodes_teaser['nodes'][$nid]['body'][0]['#markup']);
          $nodes_teaser['nodes'][$nid]['body'][0]['#markup'] = preg_replace('/\s+/m', ' ', $nodes_teaser['nodes'][$nid]['body'][0]['#markup']);
        }

        // this function has a major performance impact.
        $body = drupal_render($nodes_full['nodes'][$nid]);
        $body = mcneese_feeds_www_strip_invisible_node_header_on_h1($body);

        // the teaser/description, however, only needs to be built from the body and should have less of a performance impact.
        if (empty($nodes_teaser['nodes'][$nid]['body'][0]['#markup'])) {
          // not all nodes have a body markup field, in which case, re-using body may be necessary.
          if (isset($nodes_full['nodes'][$nid]['body'][0]['#markup'])) {
            $teaser = check_markup($nodes_full['nodes'][$nid]['body'][0]['#markup'], 'text_only');
          }
          else {
            $teaser = check_markup($body, 'text_only');
          }
        }
        else {
          $teaser = check_markup($nodes_teaser['nodes'][$nid]['body'][0]['#markup'], 'text_only');
        }

        $teaser = strip_tags($teaser);
        $teaser = preg_replace('/\s+/', ' ', $teaser);
        $teaser = text_summary($teaser, NULL, MCNEESE_FEEDS_WWW_FEED_TEASER_LIMIT);

        $date_changed = (int) $result->changed;

        date_default_timezone_set('UTC');
        $date_published = (int) strtotime($result->order_date);
        date_default_timezone_set($original_timezone);

        $item->assign_common_required($result->title, $base_url . '/node/' . $nid, $teaser);
        $item->assign_common_optional($body, $date_published, date("Y", $date_changed) . "© McNeese State University");
        $item->assign_optional('node:' . $nid, FALSE);

        $query_workbench = db_select('workbench_access_node', 'wan');
        $query_workbench->addField('wan', 'access_id', 'access_id');
        $query_workbench->condition('wan.nid', $nid);
        $query_workbench->condition('wan.access_scheme', 'taxonomy');

        $results_workbench = $query_workbench->execute()->fetchAllAssoc('access_id');
        if (!empty($results_workbench)) {
          $tids = array();
          foreach ($results_workbench as $workbench_key => $workbench_value) {
            if (array_key_exists($workbench_value->access_id, $categories)) {
              $item->append_category($categories[$workbench_value->access_id]);
              continue;
            }

            $tids[] = $workbench_value->access_id;
          }

          if (!empty($tids)) {
            $query_taxonomy = db_select('taxonomy_term_data', 'ttd');
            $query_taxonomy->addField('ttd', 'tid', 'tid');
            $query_taxonomy->addField('ttd', 'name', 'name');
            $query_taxonomy->condition('ttd.tid', $tids, 'IN');
            $query_taxonomy->condition('ttd.vid', MCNEESE_FEEDS_WWW_VOCABULARY_GROUP);

            $results_taxonomy = $query_taxonomy->execute()->fetchAllAssoc('tid');
            if (!empty($results_taxonomy)) {
              foreach ($results_taxonomy as $taxonomy) {
                $item->append_category($taxonomy->name);
                $categories[$taxonomy->tid] = $taxonomy->name;
              }
            }
          }
        }

        $channel->append_item($item);
      }
    }
  }
  catch (Error $e) {
    cf_error::on_exception($e);

    watchdog('mcneese_feeds_www', "Failed to generate RSS Feed Channel", array(), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }
  catch (Exception $e) {
    cf_error::on_exception($e);

    watchdog('mcneese_feeds_www', "Failed to generate RSS Feed Channel", array(), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  $channel->assign_required($base_url . '/');
  $channel->assign_optional('en-US', $base_url . '/' . $feed_url, TRUE, $last_build_date, array('url' => $base_url . '/sites/all/themes/mcneese/images/logo-blue.png', 'title' => "McNeese State University", 'link' => $base_url . '/'));

  $output = $channel->get_feed();

  if ($output === FALSE) {
    watchdog('mcneese_feeds_www', "Failed to generate RSS Feed Channel", array(), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
  }

  // save results as a string and perform checksum against results to make checksum specific to results and not the generated page.
  // making checksums specific to the generated page would result in different checksums for the same data due to the presence of timestamps.
  $results_string = print_r($results, TRUE);

  $checksum = mcneese_feeds_save_cache($feed_url, $results_string);

  mcneese_feeds_send_output($feed_url, $output, $checksum);
}

/**
 * @} End of '@addtogroup mcneese_feeds_www'.
 */
