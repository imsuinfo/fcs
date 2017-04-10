<?php

/**
 * @file
 * Defines McNeese Catalog base class.
 */

/**
 * @addtogroup mcneese_catalog
 * @{
 */

abstract class mcneese_catalog_class_base {
  const BASE_URL     = 'catalog.mcneese.edu';
  const CACHE_TABLE  = 'mcneese_catalog_cache';
  const API_PATH     = '/widget-api/';
  const CATALOG_PATH = '/widget-api/catalog/';
  const BASE_CLASS   = 'mcneese_catalog-content';

  const AS_RAW       = 0;
  const AS_LIST      = 1;
  const AS_SPAN      = 2;
  const AS_DIVIDER   = 3;
  const AS_ACCORDIAN = 4;
  const AS_LINK      = 5;
  const AS_HEADER    = 6;

  protected $table          = NULL;
  protected $path_name      = NULL;
  protected $cache_name     = NULL;
  protected $cache_lifetime = NULL;

  protected $select_condition   = NULL;
  protected $on_conflict_fields = NULL;

  protected $list_name = NULL;

  protected $catalog_id     = NULL;
  protected $legacy_id      = NULL;
  protected $use_catalog_id = TRUE;

  protected $cached    = NULL;
  protected $loaded    = NULL;
  protected $processed = NULL;
  protected $markup    = NULL;
  protected $total     = NULL;

  protected $use_description = NULL;
  protected $use_search      = NULL;

  protected $list_as = NULL;

  /**
   * Class constuctor.
   */
  public function __construct($catalog_id, $legacy_id) {
    $this->table          = NULL;
    $this->path_name      = NULL;
    $this->cache_name     = NULL;
    $this->cache_lifetime = '+24 hours';

    $this->select_condition   = NULL;
    $this->on_conflict_fields = array('id', 'catalog_id');

    $this->list_name  = NULL;

    if ($catalog_id === FALSE || $legacy_id === FALSE) {
      $this->catalog_id = NULL;
      $this->legacy_id = NULL;
      $this->use_catalog_id = FALSE;
    }
    else {
      if (is_int($catalog_id) && is_int($legacy_id)) {
        $this->catalog_id = $catalog_id;
        $this->legacy_id = $legacy_id;
      }

      $this->use_catalog_id = TRUE;
    }

    $this->cached    = NULL;
    $this->loaded    = NULL;
    $this->processed = NULL;
    $this->markup    = NULL;
    $this->total     = NULL;

    $this->use_description = TRUE;
    $this->use_search      = FALSE;

    $this->list_as = self::AS_LIST;
  }

  /**
   * Class destructor.
   */
  public function __destruct() {
    unset($this->table);
    unset($this->path_name);
    unset($this->cache_name);
    unset($this->cache_lifetime);

    unset($this->select_condition);
    unset($this->on_conflict_fields);

    unset($this->list_name);

    unset($this->catalog_id);
    unset($this->legacy_id);
    unset($this->use_catalog_id);

    unset($this->cached);
    unset($this->loaded);
    unset($this->processed);
    unset($this->markup);
    unset($this->total);

    unset($this->use_description);
    unset($this->use_search);

    unset($this->list_as);
  }

  /**
   * Returns TRUE if this has been loaded.
   *
   * @return bool
   *   TRUE if loaded, FALSE otherwise.
   */
  public function is_loaded() {
    if (!isset($this->loaded)) {
      $this->loaded = FALSE;
    }

    return ($this->loaded === TRUE);
  }

  /**
   * Load and store the latest or cached data.
   *
   * @param bool $check_expire
   *   (optional) When TRUE, will check to see if cache needs to be re-built.
   * @param bool $force
   *   (optional) When TRUE, will force rebuild of the cache (if possible).
   * @param bool $all
   *   (optional) When TRUE, will attempt to load all data.
   *   When FALSE, only the requested number of rows will be loaded.
   * @param int|null $page
   *   (optional) The page number to load (example: 1->100 = page 1 and 101 -> 200 = page 2, etc..).
   *   Must not be NULL when $per_page is an integer and $all is FALSE.
   * @param int|null $per_page
   *   (optional) When an integer, defines the number of rows per page to load.
   *   When NULL, all data will be loaded via a single http request.
   *
   * @return bool|int|NULL
   *   TRUE on success.
   *   NULL is returned on error, but cached data exists and is being used.
   *   FALSE is returned on error, but no data is available to use.
   */
  public function load($check_expire = TRUE, $force = FALSE, $all = TRUE, $page = NULL, $per_page = NULL) {
    if (!is_bool($check_expire)) {
      $check_expire = FALSE;
    }

    if (!is_bool($force)) {
      $force = FALSE;
    }

    if ($force) {
      $this->clear_cache_data();
    }

    // TRUE = success, NULL = expired, FALSE = failed, -1 = overflow.
    $cache_loaded = $this->get_cache($check_expire, $page, $per_page);

    // do not attempt to load beyond what is possible.
    if (is_int($cache_loaded) || $cache_loaded < 0) {
      return FALSE;
    }

    if (is_array($this->cached) && ($cache_loaded || (is_null($cache_loaded) && !$check_expire))) {
      $this->processed = $this->cached;
      $this->cached = NULL;
      $this->loaded = TRUE;
      return TRUE;
    }
    unset($cache_loaded);


    // request new data from the remote server.
    if (!$this->request_data($per_page)) {
      if (is_array($this->cached)) {
        // provide cache as failsafe.
        $this->processed = $this->cached;
        $this->cached = NULL;
        $this->loaded = NULL;
        return NULL;
      }

      return FALSE;
    }


    // process the received data, sanitizing it and preparing it for local storage and use.
    if (!is_object($this->loaded) || !$this->process_data()) {
      if (is_array($this->cached)) {
        // provide cache as failsafe.
        $this->processed = $this->cached;
        $this->cached = NULL;
        $this->loaded = NULL;
        return NULL;
      }

      return FALSE;
    }


    // store the received data into local caches.
    if (!$this->write_cache_data()) {
      // @todo: handle error conditions.
      // this is not a critical error because the cache is not required.
    }

    // get the total amount in the table.
    $this->total = (int) db_query('select count(id) from {' . $this->table . '}')->fetchField();

    // use the cached information to load the subset of data.
    if (!$all) {
      unset($this->processed);

      $cache_loaded = $this->get_cache(FALSE, $page, $per_page);
      if (is_array($this->cached)) {
        $this->processed = $this->cached;
        $this->cached = NULL;
        $this->loaded = TRUE;
        return TRUE;
      }
      unset($cache_loaded);

      return FALSE;
    }

    $this->loaded = TRUE;

    return TRUE;
  }

  /**
   * Load and store the latest or cached data for all data.
   *
   * @param bool $check_expire
   *   (optional) When TRUE, will check to see if cache needs to be re-built.
   * @param bool $force
   *   (optional) When TRUE, will force rebuild of the cache (if possible).
   *
   * @return bool|NULL
   *   TRUE on success.
   *   NULL is returned on error, but cached data exists and is being used.
   *   FALSE is returned on error, but no data is available to use.
   *
   * @see: $this->load()
   */
  public function load_all($check_expire = TRUE, $force = FALSE) {
    return $this->load($check_expire, $force, TRUE, 100);
  }

  /**
   * Returns the processed data, if any exists.
   *
   * @return array|null
   *   An array of processed data.
   *   NULL if not defined.
   */
  public function get_processed() {
    return $this->processed;
  }

  /**
   * Loads the renderred cached or processed data into HTML.
   *
   * The cache must be loaded or the content must already be processed.
   *
   * @param int|null $page
   *   (optional) If NULL, then no pager is used.
   *   If an integer, then this represents the particular page in use for generating a pager.
   * @param int|null $per_page
   *   (optional) $page is defined and is an integer, then this represents the number of items per page.
   * @param bool $wrapped
   *   (optional) When TRUE, content will be wrapped in a div tag with appropriate class names.
   *   When FALSE, the no wrapping div tag will be used.
   *
   * @return string
   *   The processed HTML.
   *   An empty string is returned on error.
   */
  public function get_markup($page = NULL, $per_page = NULL, $wrapped = TRUE) {
    if (is_null($this->markup)) {
      $this->render_markup($page, $per_page);
    }

    if (!is_string($this->markup)) {
      return '';
    }

    if ($wrapped) {
      return '<div class="' . self::BASE_CLASS . ' ' . self::BASE_CLASS . '-' . cf_theme_safe_css($this->list_name) . '">' . $this->markup . '</div>';
    }

    return $this->markup;
  }

  /**
   * Get the catalog ID.
   *
   * @param bool $allow_cache
   *   (optional) When TRUE catalog id is loaded from cache if not already assigned to this class.
   *   When FALSE, cache is not used.
   *
   * @return int|bool
   *   Catalog ID is returned on success.
   *   FALSE is returned on error.
   */
  public function get_catalog_id($allow_cache = TRUE) {
    if (is_int($this->catalog_id)) {
      return $this->catalog_id;
    }

    if ($allow_cache) {
      $this->get_catalog_ids_from_cache();

      if (is_int($this->catalog_id)) {
        return $this->catalog_id;
      }
    }

    return FALSE;
  }

  /**
   * Get the catalog legacy ID.
   *
   * @param bool $allow_cache
   *   (optional) When TRUE catalog id is loaded from cache if not already assigned to this class.
   *   When FALSE, cache is not used.
   *
   * @return int|bool
   *   Catalog Legacy ID is returned on success.
   *   FALSE is returned on error.
   */
  public function get_legacy_id($allow_cache = TRUE) {
    if (is_int($this->legacy_id)) {
      return $this->legacy_id;
    }

    if ($allow_cache) {
      $this->get_catalog_ids_from_cache();

      if (is_int($this->legacy_id)) {
        return $this->legacy_id;
      }
    }

    return FALSE;
  }

  /**
   * Assign the use description boolean.
   *
   * @param bool $use_description
   *   Whether or not descriptions should be used when rendering the markup.
   */
  public function set_use_description($use_description) {
    if (!is_bool($use_description)) {
      return;
    }

    $this->use_description = $use_description;
  }

  /**
   * Gets the use description boolean.
   *
   * @return bool $use_description
   *   Whether or not descriptions should be used when rendering the markup.
   */
  public function get_use_description() {
    if (!is_bool($this->use_description)) {
      $this->use_description = TRUE;
    }

    return $this->use_description;
  }

  /**
   * Assign the use search boolean.
   *
   * @param bool $use_search
   *   Whether or not a search should be provided when rendering the markup.
   */
  public function set_use_search($use_search) {
    if (!is_bool($use_search)) {
      return;
    }

    $this->use_search = $use_search;
  }

  /**
   * Gets the use search boolean.
   *
   * @return bool
   *   Whether or not a search should be provided when rendering the markup.
   */
  public function get_use_search() {
    if (!is_bool($this->use_search)) {
      $this->use_search = TRUE;
    }

    return $this->use_search;
  }

  /**
   * Assign the list as integer.
   *
   * Not all "as" types apply to all sub-classes.
   * See each sub-class's render_markup() implementation.
   *
   * @param int $list_as
   *   The format in which list items are displayed as when rendering the markup.
   */
  public function set_list_as($list_as) {
    if (!is_int($list_as)) {
      return;
    }

    switch ($list_as) {
      case self::AS_RAW:
      case self::AS_LIST:
      case self::AS_SPAN:
      case self::AS_DIVIDER:
      case self::AS_ACCORDIAN:
      case self::AS_LINK:
      case self::AS_HEADER:
        $this->list_as = $list_as;
        break;
    }
  }

  /**
   * Gets the list as integer.
   *
   * @return bool
   *   The format in which list items are displayed as when rendering the markup.
   */
  public function get_list_as() {
    if (!is_int($this->list_as)) {
      $this->list_as = self::AS_SPAN;
    }

    return $this->list_as;
  }

  /**
   * Ensure that the markup is corrected and made more accessible.
   *
   * The caller should still call the appropraite check_markup() on output of markup fields.
   *
   * This uses the 'page' content type settings.
   *
   * @todo: provide accessibility alerts.
   * @todo: future versions might auto-fix known common accessibililty problems.
   *
   * @param string $markup
   *   The HTML markup to sanitize.
   * @param array $disallow_tags
   *   (optional) An array of tags to explicitly remove in addition to the normal.
   *
   * @return string
   *   Sanitized markup.
   *   An empty string is returned on error.
   */
  protected function sanitize_markup($markup, $disallow_tags = array()) {
    if (!is_string($markup)) {
      cf_error::invalid_string('markup');
      return '';
    }

    if (!is_array($disallow_tags)) {
      $disallow_tags = array();
    }

    $deny_abused_codes_value = variable_get('mcneese_content_formats_deny_abused_codes_page', FALSE);
    $denied_tags_string = variable_get('mcneese_content_formats_denied_tags_page', 'script noscript');
    $denied_tags = explode(' ', $denied_tags_string);

    $sanitized = $markup;

    if ($deny_abused_codes_value) {
      $abused_codes = array();
      $abused_codes['&#34;'] = '"';
      $abused_codes['&#38;'] = '&amp;';
      $abused_codes['&#39;'] = '\'';
      $abused_codes['&#60;'] = '&lt;';
      $abused_codes['&#62;'] = '&gt;';
      $abused_codes['&quot;'] = '"';
      $abused_codes['&ndash;'] = '-';
      $abused_codes['&mdash;'] = '-';
      $abused_codes['&lsquo;'] = '\'';
      $abused_codes['&rsquo;'] = '\'';
      $abused_codes['&ldquo;'] = '"';
      $abused_codes['&rdquo;'] = '"';

      mcneese_content_formats_deny_abused_codes($abused_codes, $sanitized);
      unset($abused_codes);
    }
    unset($deny_abused_codes_value);

    $operations = array();
    $operations['fix_links'] = variable_get('mcneese_content_formats_fix_links_page', FALSE);
    $operations['fix_markup'] = variable_get('mcneese_content_formats_fix_markup_page', FALSE);
    $operations['denied_tags'] = $denied_tags;
    $operations['strip_p_tags'] = TRUE;
    unset($denied_tags);

    foreach ($disallow_tags as $disallow_tag) {
      $operations['denied_tags'][] = $disallow_tag;
    }

    $markup_format = 'full_html';
    mcneese_content_formats_alter_dom($sanitized, $operations, $markup_format);

    return $sanitized;
  }

  /**
   * Remove problematic tags and characters with the description field.
   *
   * This should be run after calling sanitize_markup() on the description field.
   *
   * @param string $markup
   *   The HTML markup to sanitize.
   *
   * @return string
   *   Sanitized markup.
   *   An empty string is returned on error.
   *
   * @see: sanitize_markup()
   */
  protected function cleanup_description($markup, $disallow_tags = array()) {
    if (!is_string($markup)) {
      cf_error::invalid_string('markup');
      return '';
    }

    // remove all line endings.
    $sanitized = preg_replace("/\r/i", '', $markup);
    $sanitized = preg_replace("/\n/i", '', $sanitized);

    // Do not allow large numbers of redundant breaks.
    $sanitized = preg_replace('/(<br><br>)+/i', '<br><br>', $sanitized);
    $sanitized = preg_replace('/(<br><br><br>)+/i', '<br><br>', $sanitized);

    return $sanitized;
  }

  /**
   * Returns the cached table
   *
   * @param bool $check_expire
   *  (optional) When TRUE, will check to see if cache needs to be re-built.
   * @param int|null $page
   *   (optional) The page number to load (example: 1->100 = page 1 and 101 -> 200 = page 2, etc..).
   *   Must not be NULL when $per_page is an integer.
   * @param int|null $per_page
   *   (optional) When an integer, defines the number of rows per page to load.
   *   When NULL, all data will be loaded via a single http request.
   *
   * @return bool|null
   *   TRUE is returned on successful cache load.
   *   NULL is returned when cache is expired.
   *   FALSE is returned on error.
   */
  protected function get_cache($check_expire = FALSE, $page = NULL, $per_page = NULL) {
    $expired = FALSE;
    if ($check_expire) {
      try {
        $last_updated = db_query('select updated from {' . self::CACHE_TABLE . '} where name = :cache_name', array(':cache_name' => $this->cache_name))->fetchField();

        if (is_null($last_updated)) {
          $expired = TRUE;
        }
        elseif (REQUEST_TIME >= strtotime($this->cache_lifetime, $last_updated)) {
          $expired = TRUE;
        }
        unset($last_updated);
      }
      catch (Exception $e) {
        // @todo
      }
      catch (Error $e) {
        // @todo
      }
    }

    // if a category id is supplied, then assume it is the correct version.
    if (is_int($this->catalog_id)) {
      $catalog_id = $this->catalog_id;
      $legacy_id = $this->legacy_id;
      $this->get_catalog_ids_from_cache();

      // if the supplied categeory id is not the same as the one stored in the cache, then the cache is out of date.
      if ($catalog_id != $this->catalog_id) {
        $expired = TRUE;
      }

      $this->catalog_id = $catalog_id;
      $this->legacy_id = $legacy_id;

      unset($catalog_id);
      unset($legacy_id);
    }

    if ($expired) {
      return NULL;
    }

    if (!is_int($per_page) || $per_page < 1) {
      $per_page = NULL;
    }

    if (!is_int($page) || $page < 0) {
      $per_page = NULL;
    }

    $arguments = array();
    $query_string = 'select * from {' . $this->table . '}';
    if (!is_null($this->select_condition)) {
      $query_string .= $this->select_condition;
    }

    try {
      if (is_int($per_page) && is_int($page)) {
        $query_string .= ' limit :limit offset :offset';
        $arguments[':limit'] = $per_page;
        $arguments[':offset'] = $per_page * $page;

        if (is_null($this->total)) {
          $this->total = (int) db_query('select count(id) from {' . $this->table . '}')->fetchField();
        }

        // do not attempt to select more than is available.
        if ($page * $per_page > $this->total) {
          return -1;
        }
      }

      $items = db_query($query_string, $arguments)->fetchAll();
    }
    catch (Exception $e) {
      cf_error::on_exception($e);
      return FALSE;
    }
    catch (Error $e) {
      cf_error::on_exception($e);
      return FALSE;
    }

    if (is_array($items) && !empty($items)) {
      $this->cached = $items;
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Load data from remote server.
   *
   * This loads all data so that it may be cached locally.
   *
   * @param int $per_page
   *   (optional) The number of rows per page loaded per request until all data is loaded.
   */
  protected function request_data($per_page = 100) {
    if ($this->use_catalog_id) {
      if (!is_int($this->catalog_id)) {
        $this->get_catalog_ids_from_cache();
        if (!is_int($this->catalog_id)) {
          return FALSE;
        }
      }

      $path = 'http://' . self::BASE_URL . self::CATALOG_PATH . $this->catalog_id . '/' . $this->path_name;
    }
    else {
      $path = 'http://' . self::BASE_URL . self::API_PATH . $this->path_name;
    }

    if (!is_int($per_page) || $per_page < 0) {
      $per_page = 100;
    }

    $result = drupal_http_request($path . '?page-size=' . $per_page . '&page=1');

    if (!isset($result->code) && $result->code != 200) {
      // @todo: report problem to watchdog().
      return FALSE;
    }

    $json_data = json_decode($result->data);
    if (!is_object($json_data) || empty($json_data->{$this->list_name})) {
      // @todo: ideally, load the cached result on http errors (therefore keep a more long term cache backup for http failures and replace it only on success).
      // for now just return FALSE.
      return FALSE;
    }

    $this->loaded = $json_data;

    if (!isset($json_data->count) || !is_numeric($json_data->count)) {
      // @todo: watchdog should throw a warning.
      return TRUE;
    }

    $current = count($json_data->{$this->list_name});
    $total = (int) $json_data->count;
    $page = 2;

    while ($current < $total) {
      $result = drupal_http_request($path . '?page-size=' . $per_page . '&page=' . $page);

      if (!is_object($json_data) || empty($json_data->{$this->list_name})) {
        // @todo: provide watchdog error.
        return FALSE;
      }

      $json_data = json_decode($result->data);
      if (is_array($json_data)) {
        // @todo: provide watchdog error.
        return FALSE;
      }

      foreach ($json_data->{$this->list_name} as $data) {
        $this->loaded->{$this->list_name}[] = $data;
      }
      unset($data);

      $current += count($json_data->{$this->list_name});
      $page++;
    }

    return TRUE;
  }

  /**
   * Write to the database cache.
   *
   * @return bool|null
   *   TRUE on success.
   *   NULL if nothing to do.
   *   FALSE otherwise.
   */
  protected function write_cache_data() {
    if (!is_array($this->processed) || empty($this->processed)) {
      return NULL;
    }

    // cache the data.
    $transaction = db_transaction();
    try {
      $this->clear_cache_data();

      foreach ($this->processed as $item) {
        $arguments = array();
        $arguments[':id'] = $item->id;

        $name_string = NULL;
        $placeholder_string = NULL;
        $update_string = NULL;

        $item_array = (array) $item;
        unset($item_array['id']);

        $name_string = 'id';
        $placeholder_string = ':id';

        foreach (array_keys($item_array) as $key_name) {
          if (is_null($item->{$key_name})) {
            continue;
          }

          if (is_bool($item->{$key_name})) {
            $name_string .= ', ' . $key_name;

            if ($item->{$key_name}) {
              $placeholder_string .= ', true';
            }
            else {
              $placeholder_string .= ', false';
            }

            continue;
          }

          if (is_array($item->{$key_name})) {
            $arguments[':' . $key_name] = json_encode($item->{$key_name});

            $name_string .= ', ' . $key_name;
            $placeholder_string .= ', :' . $key_name;

            continue;
          }

          $arguments[':' . $key_name] = $item->{$key_name};

          $name_string .= ', ' . $key_name;
          $placeholder_string .= ', :' . $key_name;
        }
        unset($item_array);

        $on_conflict_field_names = implode(', ', $this->on_conflict_fields);

        $on_conflict_fields_conditions = NULL;
        foreach ($this->on_conflict_fields as $field) {
          if (!is_null($on_conflict_fields_conditions)) {
            $on_conflict_fields_conditions .= ' and ';
          }

          $on_conflict_fields_conditions .= '{' . $this->table . '}.' . $field . ' = :' . $field;
        }

        $query_string = 'insert into {' . $this->table . '} (';
        $query_string .= $name_string;
        $query_string .= ') values (';
        $query_string .= $placeholder_string;
        $query_string .= ') on conflict (' . $on_conflict_field_names . ') do update set (' . $name_string . ') = (' . $placeholder_string . ') ' . ' where ' . $on_conflict_fields_conditions;

        db_query($query_string, $arguments);

        if (is_int($this->catalog_id) && is_int($this->legacy_id)) {
          db_query('update {' . self::CACHE_TABLE . '} set updated = :updated, catalog_id = :catalog_id, legacy_id = :legacy_id where name = :cache_name', array(':updated' => REQUEST_TIME, ':cache_name' => $this->cache_name, ':catalog_id' => $this->catalog_id, ':legacy_id' => $this->legacy_id));
        }
        else {
          db_query('update {' . self::CACHE_TABLE . '} set updated = :updated where name = :cache_name', array(':updated' => REQUEST_TIME, ':cache_name' => $this->cache_name));
        }
      }
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_exception($e);
      throw $e;
      return FALSE;
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_exception($e);
      throw $e;
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Removes all data from the cache.
   *
   * Be sure to run this inside of a transaction.
   *
   * @return bool|null
   *   TRUE on success, FALSE otherwise.
   */
  protected function clear_cache_data() {
    if (!is_string($this->table) || empty($this->table)) {
      return FALSE;
    }

    if (!is_string($this->cache_name) || empty($this->cache_name)) {
      return FALSE;
    }

    db_query('delete from {' . $this->table . '}');
    db_query('update {' . self::CACHE_TABLE . '} set updated = NULL, catalog_id = NULL where name = :cache_name', array(':cache_name' => $this->cache_name));

    return TRUE;
  }

  /**
   * Loads the catalog data from the cache.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function get_catalog_ids_from_cache() {
    try {
      $this->catalog_id = NULL;
      $this->legacy_id = NULL;

      $ids = db_query('select catalog_id, legacy_id from {' . self::CACHE_TABLE . '} where name = :cache_name', array(':cache_name' => $this->cache_name))->fetchAll();

      if (is_array($ids) && isset($ids[0]->catalog_id) && isset($ids[0]->legacy_id) && is_numeric($ids[0]->catalog_id) && is_numeric($ids[0]->legacy_id)) {
        $this->catalog_id = (int) $ids[0]->catalog_id;
        $this->legacy_id = (int) $ids[0]->legacy_id;
      }
    }
    catch (Exception $e) {
      // @todo
      return FALSE;
    }
    catch (Error $e) {
      // @todo
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Returns the base css name for this class.
   *
   * @return string
   *   A string containing the base css name.
   */
  protected function get_class_base_name() {
    return self::BASE_CLASS . '-' . cf_theme_safe_css($this->list_name);
  }

  /**
   * Render a custom pager via the pagerer module.
   *
   * @param int $page
   *   The particular page in use for generating a pager.
   * @param int $per_page
   *   This represents the number of items per page.
   *
   * @return string
   *   The renderred markup.
   *
   * @see: pager_default_initialize()
   */
  protected function build_pager($page, $per_page) {
    if (!is_int($page) || !is_int($per_page)) {
      return '';
    }

    if ($page < 0 || $per_page < 0 || $this->total < 1) {
      return '';
    }

    $page = pager_default_initialize($this->total, $per_page); // @fixme: $page parameters probably should be $pager_id or something.

    return theme('pager');
  }

  /**
   * Process the loaded data.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function process_data() {
    return FALSE;
  }

  /**
   * Renders the markup.
   *
   * @param int|null $page
   *   (optional) If NULL, then no pager is used.
   *   If an integer, then this represents the particular page in use for generating a pager.
   * @param int|null $per_page
   *   (optional) $page is defined and is an integer, then this represents the number of items per page.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function render_markup($page = NULL, $per_page = NULL) {
    if (is_null($this->processed)) {
      return FALSE;
    }

    $markup = '';
    if ($this->list_as == self::AS_LIST) {
      $this->markup .= '<ul class="' . $this->get_class_base_name() . '-list list-as_list">';
      foreach ($this->processed as $item) {
        $this->markup .= '<li class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . '</div>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }

        $this->markup .= '</li>';
      }
      $this->markup .= '</ul>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_divider">';
      foreach ($this->processed as $item) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . '</div>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }

        $this->markup .= '</div>';
      }
      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_span">';
      foreach ($this->processed as $item) {
        $this->markup .= '<span class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . '</div>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }

        $this->markup .= '</span>';
      }
      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_ACCORDIAN) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_accordian">';
      foreach ($this->processed as $item) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . '</div>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }

        $this->markup .= '</div>';
      }
      $this->markup .= '</div>';
    }

    $this->markup .= $this->build_pager($page, $per_page);

    return TRUE;
  }
}


/**
 * @} End of '@addtogroup mcneese_catalog'.
 */
