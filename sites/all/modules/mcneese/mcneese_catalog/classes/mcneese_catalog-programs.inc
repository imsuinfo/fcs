<?php

/**
 * @file
 * Defines McNeese Catalog programs class.
 */

/**
 * @addtogroup mcneese_catalog
 * @{
 */

class mcneese_catalog_class_programs extends mcneese_catalog_class_base {
  const BASE_URL_PROGRAM = '/preview_program.php';
  const CACHE_TABLE_DEGREE_TYPES  = 'mcneese_catalog_degree_types';
  const CACHE_TABLE_PROGRAM_TYPES  = 'mcneese_catalog_program_types';

  private $cached_degree_types  = NULL;
  private $cached_program_types = NULL;

  private $cache_name_degree_types  = NULL;
  private $cache_name_program_types = NULL;

  private $processed_degree_types  = NULL;
  private $processed_program_types = NULL;

  protected $on_conflict_fields_degree_types  = NULL;
  protected $on_conflict_fields_program_types = NULL;

  /**
   * Class constuctor.
   */
  public function __construct($catalog_id, $legacy_id) {
    parent::__construct($catalog_id, $legacy_id);

    $this->table          = 'mcneese_catalog_programs';
    $this->path_name      = 'programs';
    $this->cache_name     = 'programs';
    $this->cache_lifetime = '+24 hours';

    if ($this->show_invisible) {
      $this->select_condition = ' where active';
    }
    else {
      $this->select_condition = ' where visible and active';
    }

    $this->list_name = 'program-list';

    $this->cached_degree_types  = NULL;
    $this->cached_program_types = NULL;

    $this->cache_name_degree_types  = 'degree_types';
    $this->cache_name_program_types = 'program_types';

    $this->processed_degree_types  = NULL;
    $this->processed_program_types = NULL;

    $this->on_conflict_fields_degree_types = array('id', 'catalog_id');
    $this->on_conflict_fields_program_types = array('id', 'catalog_id');
  }

  /**
   * Class destructor.
   */
  public function __destruct() {
    unset($this->cached_degree_types);
    unset($this->cached_program_types);

    unset($this->cache_name_degree_types);
    unset($this->cache_name_program_types);

    unset($this->processed_degree_types);
    unset($this->processed_program_types);

    unset($this->on_conflict_fields_degree_types);
    unset($this->on_conflict_fields_program_types);

    parent::__destruct();
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

    $this->get_cache_degree_types($check_expire, $page, $per_page);
    $this->get_cache_program_types($check_expire, $page, $per_page);

    if (is_array($this->cached) && ($cache_loaded || (is_null($cache_loaded) && !$check_expire))) {
      $this->processed = $this->cached;
      $this->processed_degree_types = $this->cached_degree_types;
      $this->processed_program_types = $this->cached_program_types;
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
        $this->processed_degree_types = $this->cached_degree_types;
        $this->processed_program_types = $this->cached_program_types;
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
        $this->processed_degree_types = $this->cached_degree_types;
        $this->processed_program_types = $this->cached_program_types;
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
        $this->processed_degree_types = $this->cached_degree_types;
        $this->processed_program_types = $this->cached_program_types;
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
   * Assign show visibility setting.
   *
   * @param bool $show_invisible
   *   Toggle loading of content with visible flag set to TRUE or FALSE.
   */
  public function set_show_invisible($show_invisible) {
    parent::set_show_invisible($show_invisible);

    if ($this->show_invisible) {
      $this->select_condition = ' where active';
    }
    else {
      $this->select_condition = ' where visible and active';
    }
  }

  /**
   * Returns the processed degree type data, if any exists.
   *
   * @return array|null
   *   An array of processed data.
   *   NULL if not defined.
   */
  public function get_processed_degree_types() {
    return $this->processed_degree_types;
  }

  /**
   * Returns the processed program type data, if any exists.
   *
   * @return array|null
   *   An array of processed data.
   *   NULL if not defined.
   */
  public function get_processed_program_types() {
    return $this->processed_program_types;
  }

  /**
   * Removes all data from the cache.
   *
   * Be sure to run this inside of a transaction.
   *
   * @return bool|null
   *   TRUE on success, FALSE otherwise.
   */
  public function clear_cache_data() {
    if (!is_string($this->table) || empty($this->table)) {
      return FALSE;
    }

    if (!is_string($this->cache_name) || empty($this->cache_name)) {
      return FALSE;
    }

    db_query('delete from {' . $this->table . '}');
    db_query('delete from {' . self::CACHE_TABLE_DEGREE_TYPES . '}');
    db_query('delete from {' . self::CACHE_TABLE_PROGRAM_TYPES . '}');
    db_query('update {' . self::CACHE_TABLE . '} set updated = NULL, catalog_id = NULL, legacy_id = NULL where name in (:cache_name, :cache_name_degree_types, :cache_name_program_types)', array(':cache_name' => $this->cache_name, ':cache_name_degree_types' => $this->cache_name_degree_types, ':cache_name_program_types' => $this->cache_name_program_types));

    return TRUE;
  }

  /**
   * Loads the an entire degree type item from the currently cached catalog.
   *
   * This does not check cache and will load nothing database cache is not populated.
   *
   * @param int $id
   *   The item id.
   * @param int $as_legacy_id
   *   (optional) When TRUE, the provided id is the legacy id.
   *   When FALSE, it is the normal id.
   *
   * @return array|bool
   *   A description string on success.
   *   FALSE is returned on error.
   */
  public function get_item_degree_type($id, $as_legacy_id = FALSE) {
    if (!is_int($this->catalog_id) || empty($this->table)) {
      return FALSE;
    }

    if (!is_int($id) || !is_bool($as_legacy_id)) {
      return FALSE;
    }

    try {
      $query = db_select(self::CACHE_TABLE_DEGREE_TYPES, 'mct');
      $query->fields('mct');

      if ($as_legacy_id) {
        $query->condition('mct.legacy_id', $id);
      }
      else {
        $query->condition('mct.id', $id);
      }

      $result = $query->execute()->fetchAll();
      if (is_array($result)) {
        if (empty($result)) {
          return array();
        }

        return array_pop($result);
      }
    }
    catch (Exception $e) {
      // @todo
    }
    catch (Error $e) {
      // @todo
    }

    return FALSE;
  }

  /**
   * Loads the an entire program type item from the currently cached catalog.
   *
   * This does not check cache and will load nothing database cache is not populated.
   *
   * @param int $id
   *   The item id.
   * @param int $as_legacy_id
   *   (optional) When TRUE, the provided id is the legacy id.
   *   When FALSE, it is the normal id.
   *
   * @return array|bool
   *   A description string on success.
   *   FALSE is returned on error.
   */
  public function get_item_program_type($id, $as_legacy_id = FALSE) {
    if (!is_int($this->catalog_id) || empty($this->table)) {
      return FALSE;
    }

    if (!is_int($id) || !is_bool($as_legacy_id)) {
      return FALSE;
    }

    try {
      $query = db_select(self::CACHE_TABLE_PROGRAM_TYPES, 'mct');
      $query->fields('mct');

      if ($as_legacy_id) {
        $query->condition('mct.legacy_id', $id);
      }
      else {
        $query->condition('mct.id', $id);
      }

      $result = $query->execute()->fetchAll();
      if (is_array($result)) {
        if (empty($result)) {
          return array();
        }

        return array_pop($result);
      }
    }
    catch (Exception $e) {
      // @todo
    }
    catch (Error $e) {
      // @todo
    }

    return FALSE;
  }

  /**
   * Returns the cached table, specific to degree types.
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
  protected function get_cache_degree_types($check_expire = FALSE, $page = NULL, $per_page = NULL) {
    $expired = FALSE;
    if ($check_expire) {
      try {
        $last_updated = db_query('select updated from {' . self::CACHE_TABLE . '} where name = :cache_name', array(':cache_name' => $this->cache_name_degree_types))->fetchField();

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
    $query_string = 'select * from {' . self::CACHE_TABLE_DEGREE_TYPES . '}';
    if (!is_null($this->select_condition)) {
      $query_string .= $this->select_condition;
    }

    try {
      if (is_int($per_page) && is_int($page)) {
        $query_string .= ' limit :limit offset :offset';
        $arguments[':limit'] = $per_page;
        $arguments[':offset'] = $per_page * $page;

        if (is_null($this->total)) {
          $this->total = (int) db_query('select count(id) from {' . self::CACHE_TABLE_DEGREE_TYPES . '}')->fetchField();
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
      $this->cached_degree_types = $items;
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Returns the cached table, specific to program types.
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
  protected function get_cache_program_types($check_expire = FALSE, $page = NULL, $per_page = NULL) {
    $expired = FALSE;
    if ($check_expire) {
      try {
        $last_updated = db_query('select updated from {' . self::CACHE_TABLE . '} where name = :cache_name', array(':cache_name' => $this->cache_name_program_types))->fetchField();

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
    $query_string = 'select * from {' . self::CACHE_TABLE_PROGRAM_TYPES . '}';
    if (!is_null($this->select_condition)) {
      $query_string .= $this->select_condition;
    }

    try {
      if (is_int($per_page) && is_int($page)) {
        $query_string .= ' limit :limit offset :offset';
        $arguments[':limit'] = $per_page;
        $arguments[':offset'] = $per_page * $page;

        if (is_null($this->total)) {
          $this->total = (int) db_query('select count(id) from {' . self::CACHE_TABLE_PROGRAM_TYPES . '}')->fetchField();
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
      $this->cached_program_types = $items;
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Process the loaded data.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function process_data() {
    if (empty($this->loaded->{$this->list_name})) {
      $this->loaded = NULL;
      $this->processed = array();
      return TRUE;
    }

    $this->processed = array();
    $this->degree_types = array();
    $this->program_types = array();

    foreach ($this->loaded->{$this->list_name} as $item) {
      // require the id.
      if (!property_exists($item, 'id') || !is_numeric($item->id)) {
        continue;
      }

      // require the catalog id.
      if (!property_exists($item, 'catalog-id') || !is_numeric($item->{'catalog-id'})) {
        continue;
      }

      // require the legacy id.
      if (!property_exists($item, 'legacy-id') || !is_numeric($item->{'legacy-id'}))  {
        continue;
      }

      // require the name string to exist and be non-empty.
      if (empty($item->name) || !is_string($item->name)) {
        continue;
      }

      // require the url to exist and be non-empty.
      if (empty($item->url) || !is_string($item->url)) {
        continue;
      }

      // the status object must exists
      if (!property_exists($item, 'status') || !is_object($item->status)) {
        continue;
      }

      // ignore inactive items.
      if (property_exists($item->status, 'active') && !$item->status->active) {
        continue;
      }

      $entry = new stdClass();
      $entry->id = (int) $item->id;
      $entry->catalog_id = $item->{'catalog-id'};
      $entry->legacy_id = (int) $item->{'legacy-id'};

      $entry->name = $item->name;
      $entry->url = substr($item->url, 0, 256);

      $entry->description = '';
      if (property_exists($item, 'description') && is_string($item->description)) {
        $entry->description = $this->sanitize_markup($item->description, array('hr'));
        $entry->description = $this->cleanup_description($entry->description);
      }

      if (!empty($item->created) && is_string($item->created)) {
        $sanitized_date = strtotime($item->created);
        $entry->created = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      if (!empty($item->modified) && is_string($item->modified)) {
        $sanitized_date = strtotime($item->modified);
        $entry->modified = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      $entry->active = TRUE;

      $entry->visible = TRUE;
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
        $entry->visible = FALSE;
      }

      $entry->searchable = TRUE;
      if (property_exists($item, 'searchable') && !$item->searchable) {
        $entry->searchable = FALSE;
      }

      $entry->code = '';
      if (property_exists($item, 'code') && is_string($item->code)) {
        $entry->code = $item->code;
      }

      // the degree types are not a distinct group, but are stored within programs.
      // filter out these for separate storage in a more efficient manner.
      if (property_exists($item, 'degree_types') && is_array($item->{'degree_types'})) {
        $entry->degree_types = $this->process_data_degree_types($item->{'degree_types'});
      }

      // the program types are not a distinct group, but are stored within programs.
      // filter out these for separate storage in a more efficient manner.
      if (property_exists($item, 'program_types') && is_array($item->{'program_types'})) {
        $entry->program_types = $this->process_data_program_types($item->{'program_types'});
      }

      $this->processed[] = $entry;
      unset($entry);
    }

    $this->loaded = NULL;
    return TRUE;
  }

  /**
   * Renders the markup for degree types.
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
  protected function render_markup_degree_types($page = NULL, $per_page = NULL) {
    if (is_null($this->processed)) {
      return FALSE;
    }

    $markup = '';

    // @todo

    return TRUE;
  }

  /**
   * Renders the markup for program types.
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
  protected function render_markup_program_types($page = NULL, $per_page = NULL) {
    if (is_null($this->processed)) {
      return FALSE;
    }

    $markup = '';

    // @todo

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


      // cache the degree types.
      if (is_array($this->processed_degree_types)) {
        foreach ($this->processed_degree_types as $item) {
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
            $arguments[':' . $key_name] = $item->{$key_name};

            $name_string .= ', ' . $key_name;
            $placeholder_string .= ', :' . $key_name;
          }
          unset($item_array);

          $on_conflict_field_names = implode(', ', $this->on_conflict_fields_degree_types);

          $on_conflict_fields_conditions = NULL;
          foreach ($this->on_conflict_fields_degree_types as $field) {
            if (!is_null($on_conflict_fields_conditions)) {
              $on_conflict_fields_conditions .= ' and ';
            }

            $on_conflict_fields_conditions .= '{' . self::CACHE_TABLE_DEGREE_TYPES . '}.' . $field . ' = :' . $field;
          }

          $query_string = 'insert into {' . self::CACHE_TABLE_DEGREE_TYPES . '} (';
          $query_string .= $name_string;
          $query_string .= ') values (';
          $query_string .= $placeholder_string;
          $query_string .= ') on conflict (' . $on_conflict_field_names . ') do update set (' . $name_string . ') = (' . $placeholder_string . ') ' . ' where ' . $on_conflict_fields_conditions;

          db_query($query_string, $arguments);

          if (is_int($item->catalog_id) && is_int($item->legacy_id)) {
            db_query('update {' . self::CACHE_TABLE . '} set updated = :updated, catalog_id = :catalog_id, legacy_id = :legacy_id where name = :cache_name', array(':updated' => REQUEST_TIME, ':cache_name' => $this->cache_name_degree_types, ':catalog_id' => $this->catalog_id, ':legacy_id' => $this->legacy_id));
          }
          else {
            db_query('update {' . self::CACHE_TABLE . '} set updated = :updated where name = :cache_name', array(':updated' => REQUEST_TIME, ':cache_name' => $this->cache_name_degree_types));
          }
        }
      }


      // cache the program types.
      if (is_array($this->processed_program_types)) {
        foreach ($this->processed_program_types as $item) {
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
            $arguments[':' . $key_name] = $item->{$key_name};

            $name_string .= ', ' . $key_name;
            $placeholder_string .= ', :' . $key_name;
          }
          unset($item_array);

          $on_conflict_field_names = implode(', ', $this->on_conflict_fields_program_types);

          $on_conflict_fields_conditions = NULL;
          foreach ($this->on_conflict_fields_program_types as $field) {
            if (!is_null($on_conflict_fields_conditions)) {
              $on_conflict_fields_conditions .= ' and ';
            }

            $on_conflict_fields_conditions .= '{' . self::CACHE_TABLE_PROGRAM_TYPES . '}.' . $field . ' = :' . $field;
          }

          $query_string = 'insert into {' . self::CACHE_TABLE_PROGRAM_TYPES . '} (';
          $query_string .= $name_string;
          $query_string .= ') values (';
          $query_string .= $placeholder_string;
          $query_string .= ') on conflict (' . $on_conflict_field_names . ') do update set (' . $name_string . ') = (' . $placeholder_string . ') ' . ' where ' . $on_conflict_fields_conditions;

          db_query($query_string, $arguments);

          if (is_int($item->catalog_id) && is_int($item->legacy_id)) {
            db_query('update {' . self::CACHE_TABLE . '} set updated = :updated, catalog_id = :catalog_id, legacy_id = :legacy_id where name = :cache_name', array(':updated' => REQUEST_TIME, ':cache_name' => $this->cache_name_program_types, ':catalog_id' => $this->catalog_id, ':legacy_id' => $this->legacy_id));
          }
          else {
            db_query('update {' . self::CACHE_TABLE . '} set updated = :updated where name = :cache_name', array(':updated' => REQUEST_TIME, ':cache_name' => $this->cache_name_program_types));
          }
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
   * Renders the markup.
   *
   * @param int|null $page
   *   (optional) If NULL, then no pager is used.
   *   If an integer, then this represents the particular page in use for generating a pager.
   * @param int|null $per_page
   *   (optional) $page is defined and is an integer, then this represents the number of items per page.
   * @param string $markup_type
   *   (optional) The markup type filter to use.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function render_markup($page = NULL, $per_page = NULL, $markup_type = 'full_html') {
    if (is_null($this->processed)) {
      return FALSE;
    }

    if (!is_string($markup_type)) {
      $markup_type = 'full_html';
    }

    $markup = '';
    if ($this->list_as == self::AS_LIST) {
      $this->markup .= '<ul class="' . $this->get_class_base_name() . '-list list-as_list">';
      foreach ($this->processed as $item) {
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<li class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_PROGRAM . '?catoid=' . ((int) $this->legacy_id) . '&poid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, $markup_type) . '</div>';
        }
        $this->markup .= '</li>';
      }
      $this->markup .= '</ul>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_divider">';
      foreach ($this->processed as $item) {
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_PROGRAM . '?catoid=' . ((int) $this->legacy_id) . '&poid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, $markup_type) . '</div>';
        }
        $this->markup .= '</div>';
      }
      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_span">';
      foreach ($this->processed as $item) {
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<span class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_PROGRAM . '?catoid=' . ((int) $this->legacy_id) . '&poid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, $markup_type) . '</div>';
        }
        $this->markup .= '</span>';
      }
      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_ACCORDIAN) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_accordian">';
      foreach ($this->processed as $item) {
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_PROGRAM . '?catoid=' . ((int) $this->legacy_id) . '&poid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, $markup_type) . '</div>';
        }

        $this->markup .= '</div>';
      }
      $this->markup .= '</div>';
    }

    $this->markup .= $this->build_pager($page, $per_page);

    return TRUE;
  }

  /**
   * Processes degree-type specific data.
   *
   * @param array $degree_types
   *   An array of degree types as decoded by json_decode().
   *
   * @return string
   *   A json encode string representing the specific set of degree types after being processed.
   */
  private function process_data_degree_types($degree_types) {
    if (!is_array($this->processed_degree_types)) {
      $this->processed_degree_types = array();
    }

    if (empty($degree_types)) {
      return json_encode(array());
    }

    $current_degree_types = array();
    foreach ($degree_types as $item) {
      // require the id.
      if (!property_exists($item, 'id') || !is_numeric($item->id)) {
        continue;
      }

      // require the catalog id.
      if (!property_exists($item, 'catalog-id') || !is_numeric($item->{'catalog-id'})) {
        continue;
      }

      // require the legacy id.
      if (!property_exists($item, 'legacy-id') || !is_numeric($item->{'legacy-id'}))  {
        continue;
      }

      // require the order id.
      if (!property_exists($item, 'order') || !is_numeric($item->{'order'}))  {
        continue;
      }

      // require the name string to exist and be non-empty.
      if (empty($item->name) || !is_string($item->name)) {
        continue;
      }

      // the status object must exists
      if (!property_exists($item, 'status') || !is_object($item->status)) {
        continue;
      }

      // ignore inactive items.
      if (property_exists($item->status, 'active') && !$item->status->active) {
        continue;
      }

      $entry = new stdClass();
      $entry->id = (int) $item->id;
      $entry->catalog_id = $item->{'catalog-id'};
      $entry->legacy_id = (int) $item->{'legacy-id'};
      $entry->order_id = (int) $item->{'order'};

      $entry->name = $item->name;

      $entry->description = '';
      if (property_exists($item, 'description') && is_string($item->description)) {
        $entry->description = $this->sanitize_markup($item->description, array('hr'));
        $entry->description = $this->cleanup_description($entry->description);
      }

      if (!empty($item->created) && is_string($item->created)) {
        $sanitized_date = strtotime($item->created);
        $entry->created = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      if (!empty($item->modified) && is_string($item->modified)) {
        $sanitized_date = strtotime($item->modified);
        $entry->modified = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      $entry->active = TRUE;

      $entry->visible = TRUE;
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
        $entry->visible = FALSE;
      }

      $entry->searchable = TRUE;
      if (property_exists($item, 'searchable') && !$item->searchable) {
        $entry->searchable = FALSE;
      }

      // @fixme: how does this differ from visible?
      $entry->display = TRUE;
      if (property_exists($item, 'display') && !$item->display) {
        $entry->display = FALSE;
      }

      $entry->category = '';
      if (property_exists($item, 'category') && is_string($item->category)) {
        $entry->category = substr($item->category, 0, 64);
      }

      $this->processed_degree_types[] = $entry;
      $current_degree_types[] = $entry;
      unset($entry);
    }

    return json_encode($current_degree_types);
  }

  /**
   * Processes program-type specific data.
   *
   * @param array $program_types
   *   An array of program types as decoded by json_decode().
   *
   * @return string
   *   A json encode string representing the specific set of program types after being processed.
   */
  private function process_data_program_types($program_types) {
    if (!is_array($this->processed_program_types)) {
      $this->processed_program_types = array();
    }

    if (empty($program_types)) {
      return json_encode(array());
    }

    $current_program_types = array();
    foreach ($program_types as $item) {
      // require the id.
      if (!property_exists($item, 'id') || !is_numeric($item->id)) {
        continue;
      }

      // require the catalog id.
      if (!property_exists($item, 'catalog-id') || !is_numeric($item->{'catalog-id'})) {
        continue;
      }

      // require the legacy id.
      if (!property_exists($item, 'legacy-id') || !is_numeric($item->{'legacy-id'}))  {
        continue;
      }

      // require the order id.
      if (!property_exists($item, 'order') || !is_numeric($item->{'order'}))  {
        continue;
      }

      // require the name string to exist and be non-empty.
      if (empty($item->name) || !is_string($item->name)) {
        continue;
      }

      // the status object must exists
      if (!property_exists($item, 'status') || !is_object($item->status)) {
        continue;
      }

      // ignore inactive items.
      if (property_exists($item->status, 'active') && !$item->status->active) {
        continue;
      }

      $entry = new stdClass();
      $entry->id = (int) $item->id;
      $entry->catalog_id = $item->{'catalog-id'};
      $entry->legacy_id = (int) $item->{'legacy-id'};
      $entry->order_id = (int) $item->{'order'};

      $entry->name = $item->name;

      $entry->description = '';
      if (property_exists($item, 'description') && is_string($item->description)) {
        $entry->description = $this->sanitize_markup($item->description, array('hr'));
        $entry->description = $this->cleanup_description($entry->description);
      }

      if (!empty($item->created) && is_string($item->created)) {
        $sanitized_date = strtotime($item->created);
        $entry->created = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      if (!empty($item->modified) && is_string($item->modified)) {
        $sanitized_date = strtotime($item->modified);
        $entry->modified = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      $entry->active = TRUE;

      $entry->visible = TRUE;
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
        $entry->visible = FALSE;
      }

      $entry->searchable = TRUE;
      if (property_exists($item, 'searchable') && !$item->searchable) {
        $entry->searchable = FALSE;
      }

      // @fixme: how does this differ from visible?
      $entry->display = TRUE;
      if (property_exists($item, 'display') && !$item->display) {
        $entry->display = FALSE;
      }

      $entry->category = '';
      if (property_exists($item, 'category') && is_string($item->category)) {
        $entry->category = substr($item->category, 0, 64);
      }

      $this->processed_program_types[] = $entry;
      $current_program_types[] = $entry;
      unset($entry);
    }

    return json_encode($current_program_types);
  }
}

/**
 * @} End of '@addtogroup mcneese_catalog'.
 */
