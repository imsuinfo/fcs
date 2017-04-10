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
  private $processed_degree_types  = NULL;
  private $processed_program_types = NULL;

  private $table_degree_types  = NULL;
  private $table_program_types = NULL;

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

    $this->select_condition = ' where visible and active';

    $this->list_name = 'program-list';

    $this->processed_degree_types  = NULL;
    $this->processed_program_types = NULL;

    $this->table_degree_types  = 'mcneese_catalog_degree_types';
    $this->table_program_types = 'mcneese_catalog_program_types';

    $this->cache_name_degree_types  = 'degree_types';
    $this->cache_name_program_types = 'program_types';

    $this->on_conflict_fields_degree_types = array('id', 'catalog_id');
    $this->on_conflict_fields_program_types = array('id', 'catalog_id');
  }

  /**
   * Class destructor.
   */
  public function __destruct() {
    unset($this->processed_degree_types);
    unset($this->processed_program_types);

    unset($this->table_degree_types);
    unset($this->table_program_types);

    unset($this->cache_name_degree_types);
    unset($this->cache_name_program_types);

    unset($this->on_conflict_fields_degree_types);
    unset($this->on_conflict_fields_program_types);

    parent::__destruct();
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

      // ignore invisible items.
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
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

            $on_conflict_fields_conditions .= '{' . $this->table_degree_types . '}.' . $field . ' = :' . $field;
          }

          $query_string = 'insert into {' . $this->table_degree_types . '} (';
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

            $on_conflict_fields_conditions .= '{' . $this->table_program_types . '}.' . $field . ' = :' . $field;
          }

          $query_string = 'insert into {' . $this->table_program_types . '} (';
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
    db_query('delete from {' . $this->table_degree_types . '}');
    db_query('delete from {' . $this->table_program_types . '}');
    db_query('update {' . self::CACHE_TABLE . '} set updated = NULL, catalog_id = NULL where name in (:cache_name, :cache_name_degree_types, :cache_name_program_types)', array(':cache_name' => $this->cache_name, ':cache_name_degree_types' => $this->cache_name_degree_types, ':cache_name_program_types' => $this->cache_name_program_types));

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
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<li class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</div>';

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
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</div>';

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
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<span class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</div>';

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
        $code = NULL;
        if (!empty($item->code)) {
           $code = ' (' . check_plain($item->code) . ')';
        }

        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-name">' . check_plain($item->name) . $code . '</div>';

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

      // ignore invisible items.
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
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

      // ignore invisible items.
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
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
