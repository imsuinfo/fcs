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
 * Output the RSS Feed "Lagniappe".
 * Only loads "Highlight" and "Lagniappe".
 */
function mcneese_feeds_www_rss_lagniappe() {
  global $base_url;
  global $base_path;
  global $user;

  require_once(drupal_get_path('module', 'mcneese_feeds') . '/classes/rss.inc');

  $headers = array();
  $output = '';
  $rows = array();
  $categories = array();
  $feed_url = 'rss/feed/lagniappe';

  mcneese_feeds_process_if_none_match($feed_url);

  $channel = new McNeeseFeedsRssChannel();
  $channel->assign_common_required("McNeese State University - Feed - Lagniappe", $base_url, "Published lagniappe available on the McNeese State University website.");
  $channel->assign_common_optional(NULL, REQUEST_TIME, date("Y", REQUEST_TIME) . "© McNeese State University");

  // The date of the most recent item on the feed should be used here.
  $last_build_date = REQUEST_TIME;
  $original_timezone = date_default_timezone_get();

  try {
    $query = db_select('node', 'n');
    $query->innerJoin('field_data_field_order_date', 'fdfod', 'n.nid = fdfod.entity_id and fdfod.entity_type = \'node\' and fdfod.deleted = 0');
    $query->leftJoin('field_data_field_highlight', 'fdfh', 'n.nid = fdfh.entity_id and fdfh.entity_type = \'node\' and fdfh.deleted = 0');
    $query->leftJoin('field_data_field_event', 'fdfe', 'n.nid = fdfe.entity_id and fdfe.entity_type = \'node\' and fdfe.deleted = 0');
    $query->leftJoin('workbench_moderation_node_history', 'wmnh', 'n.nid = wmnh.nid and n.vid = wmnh.vid and wmnh.published = 1');

    $query->addField('n', 'nid', 'nid');
    $query->addField('n', 'title', 'title');
    $query->addField('n', 'changed', 'changed');
    $query->addField('fdfod', 'field_order_date_value', 'order_date');

    $query->condition('n.status', 1);
    $query->condition('n.type', 'system-page', '<>');

    $or = db_or();
    $or->condition('fdfh.field_highlight_value', 1);
    $or->condition('fdfe.field_event_value', 1);
    $query->condition($or);

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
