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
 * Output the RSS Feed "Group".
 *
 * @param string|integer $group
 *   The taxonomy term group by the group name or by term id. Using the term
 *   name is more convenient, but if there are conflicts, then grab them by id.
 *   Terms that have the same name will all be loaded as if they were a single
 *   term. Multiple terms may be specified by providing '+' signs to separate.
 */
function mcneese_feeds_www_rss_group($group) {
  if (!is_string($group) || empty($group)) {
    cf_error::invalid_string('group');
    drupal_not_found();
    drupal_exit();
  }

  global $conf;

  $blacklist_groups = array();
  if (isset($conf['feed_groups_blacklist']['groups']) && is_array($conf['feed_groups_blacklist']['groups'])) {
    $blacklist_groups = $conf['feed_groups_blacklist']['groups'];
  }

  $blacklist_nodes = array();
  if (isset($conf['feed_groups_blacklist']['nodes'][NULL]) && is_array($conf['feed_groups_blacklist']['nodes'][NULL])) {
    $blacklist_nodes = $conf['feed_groups_blacklist']['nodes'][NULL];
  }

  $group_ids = array();
  $group_names = array();

  $numbers_only_1 = str_replace(MCNEESE_FEEDS_GROUPING, '', $group);
  $numbers_only_2 = "" . (int) $numbers_only_1;
  if (strcmp($numbers_only_1, $numbers_only_2) == 0) {
    $group_ids = explode(MCNEESE_FEEDS_GROUPING, $group);

    foreach ($group_ids as &$group_id) {
      $group_id = (int) $group_id;
    }
  }
  else {
    $group_names = explode(MCNEESE_FEEDS_GROUPING, $group);
  }

  if (empty($group_names) && empty($group_ids)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_url;
  global $base_path;
  global $user;

  require_once(drupal_get_path('module', 'mcneese_feeds') . '/classes/rss.inc');

  $headers = array();
  $output = '';
  $rows = array();
  $categories = array();
  $feed_url = 'rss/feed/group/' . $group;

  mcneese_feeds_process_if_none_match($feed_url);

  $channel = new McNeeseFeedsRssChannel();
  $channel->assign_common_required("McNeese State University - Category Feed", $base_url, "Published content available on the McNeese State University website associated with the specified category.");
  $channel->assign_common_optional(NULL, REQUEST_TIME, date("Y", REQUEST_TIME) . "© McNeese State University");

  // The date of the most recent item on the feed should be used here.
  $last_build_date = REQUEST_TIME;
  $original_timezone = date_default_timezone_get();

  try {
    $query_taxonomy = db_select('taxonomy_term_data', 'ttd');
    $query_taxonomy->addField('ttd', 'tid', 'tid');
    $query_taxonomy->addField('ttd', 'name', 'name');

    if (empty($group_ids)) {
      $or = db_or();
      foreach ($group_names as $group_name) {
        $or->condition('ttd.name', db_like($group_name), 'ILIKE');
      }

      $query_taxonomy->condition($or);
      unset($or);
    }
    else {
      $query_taxonomy->condition('ttd.tid', $group_ids, 'IN');
    }

    $query_taxonomy->condition('ttd.vid', MCNEESE_FEEDS_WWW_VOCABULARY_GROUP);

    $results_taxonomy = $query_taxonomy->execute()->fetchAllAssoc('tid');
    if (empty($results_taxonomy)) {
      drupal_not_found();
      drupal_exit();
    }

    $results_label = NULL;
    foreach ($results_taxonomy as $taxonomy) {
      if (array_key_exists($taxonomy->tid, $blacklist_groups)) {
        continue;
      }

      if (isset($conf['feed_groups_blacklist']['nodes'][$taxonomy->tid]) && is_array($conf['feed_groups_blacklist']['nodes'][$taxonomy->tid])) {
        $blacklist_nodes = array_merge($blacklist_nodes, $conf['feed_groups_blacklist']['nodes'][$taxonomy->tid]);
      }

      $categories[$taxonomy->tid] = $taxonomy->name;

      if (is_null($results_label)) {
        $results_label = $taxonomy->name;
        continue;
      }

      $results_label .= ', ' . $taxonomy->name;
    }

    if (empty($categories)) {
      drupal_not_found();
      drupal_exit();
    }

    $exception_case_handle_police_complaints = (bool) array_key_exists(MCNEESE_FEEDS_WWW_VOCABULARY_GROUP_POLICE, $categories);

    $channel->set_title("McNeese State University - Category Feed - " . $results_label);
    $channel->set_description("Published content available on the McNeese State University website that is categorized as: " . $results_label);

    $query = db_select('node', 'n');
    $query->innerJoin('field_data_field_order_date', 'fdfod', 'n.nid = fdfod.entity_id and fdfod.entity_type = \'node\' and fdfod.deleted = 0');

    if ($exception_case_handle_police_complaints) {
      $query->leftJoin('field_data_field_group', 'fdfg', 'n.nid = fdfg.entity_id and fdfg.entity_type = \'node\' and fdfg.deleted = 0');
    }
    else {
      $query->innerJoin('field_data_field_group', 'fdfg', 'n.nid = fdfg.entity_id and fdfg.entity_type = \'node\' and fdfg.deleted = 0');
    }

    $query->leftJoin('workbench_moderation_node_history', 'wmnh', 'n.nid = wmnh.nid and n.vid = wmnh.vid and wmnh.published = 1');

    $query->addField('n', 'nid', 'nid');
    $query->addField('n', 'title', 'title');
    $query->addField('n', 'changed', 'changed');
    $query->addField('fdfod', 'field_order_date_value', 'order_date');

    $query->condition('n.status', 1);
    $query->condition('n.type', 'system-page', '<>');

    if (!empty($blacklist_nodes)) {
      $query->condition('n.nid', $blacklist_nodes, 'NOT IN');
    }

    if ($exception_case_handle_police_complaints) {
      $or = db_or();
      $or->condition('n.type', MCNEESE_FEEDS_WWW_NODE_TYPE_POLICE);
      $or->condition('fdfg.field_group_tid', array_keys($categories), 'IN');

      $query->condition($or);
      unset($or);
    }
    else {
      $query->condition('fdfg.field_group_tid', array_keys($categories), 'IN');
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
        $item->assign_common_optional($body, $date_published, date("Y", $date_changed) . "© McNeese State University", $categories);
        $item->assign_optional('node:' . $nid, FALSE);

        $channel->append_item($item);
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);

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
