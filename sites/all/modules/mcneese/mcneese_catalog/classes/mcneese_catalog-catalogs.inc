<?php

/**
 * @file
 * Defines McNeese Catalog catalogs class.
 */

/**
 * @addtogroup mcneese_catalog
 * @{
 */

class mcneese_catalog_class_catalogs extends mcneese_catalog_class_base {
  /**
   * Class constuctor.
   */
  public function __construct($catalog_id = FALSE, $legacy_id = FALSE) {
    parent::__construct(FALSE, FALSE);

    $this->table          = 'mcneese_catalog_catalogs';
    $this->path_name      = 'catalogs';
    $this->cache_name     = 'catalogs';
    $this->cache_lifetime = '+24 hours';

    $this->select_condition = ' where not archived and published';
    $this->on_conflict_fields = array('id');

    $this->list_name = 'catalog-list';

    $this->as_link = self::AS_DIVIDER;
  }

  /**
   * Returns the name field from processed data, if any exists.
   *
   * @return array|null
   *   An array of processed data.
   *   NULL if not defined.
   */
  public function get_name() {
    if (is_null($this->processed)) {
      return FALSE;
    }

    // only a single catalog is used.
    if (!isset($this->processed[0]->name) || !is_string($this->processed[0]->name)) {
      return FALSE;
    }

    return $this->processed[0]->name;
  }

  /**
   * Returns the description field from processed data, if any exists.
   *
   * @return array|null
   *   An array of processed data.
   *   NULL if not defined.
   */
  public function get_description() {
    if (is_null($this->processed)) {
      return FALSE;
    }

    // only a single catalog is used.
    if (!isset($this->processed[0]->description) || !is_string($this->processed[0]->description)) {
      return FALSE;
    }

    return $this->processed[0]->description;
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

    $entry = NULL;

    foreach ($this->loaded->{$this->list_name} as $item) {
      // require the id.
      if (!property_exists($item, 'id') || !is_numeric($item->id)) {
        continue;
      }

      // require the legacy id.
      if (!property_exists($item, 'legacy-id') || !is_numeric($item->{'legacy-id'})) {
        continue;
      }

      // require the name string to exist and be non-empty.
      if (empty($item->name) || !is_string($item->name)) {
        continue;
      }

      // ignore archived items.
      if (property_exists($item, 'archived') && $item->archived) {
        continue;
      }

      // ignore unpublished items.
      if (property_exists($item, 'published') && !$item->published) {
        continue;
      }

      // The current should be the only entry without archived set, but just in case loop until done and use last entry found.
      $entry = new stdClass();
      $entry->id = (int) $item->id;
      $entry->legacy_id = (int) $item->{'legacy-id'};
      $this->catalog_id = $entry->id;
      $this->legacy_id = $entry->legacy_id;

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

      $entry->archived = FALSE;

      $entry->locked = TRUE;
      if (property_exists($item, 'locked') && !$item->locked) {
        $entry->locked = FALSE;
      }

      $entry->copying = FALSE;
      if (property_exists($item, 'copying') && $item->copying) {
        $entry->copying = TRUE;
      }

      $entry->published = TRUE;

      $entry->import_locked = FALSE;
      if (property_exists($item, 'import_locked') && $item->import_locked) {
        $entry->import_locked = TRUE;
      }

      if (property_exists($item->{'catalog-type'}, 'id') && is_numeric($item->{'catalog-type'}->id) && !empty($item->{'catalog-type'}->name) && is_string($item->{'catalog-type'}->name)) {
        $entry->catalog_type_id = (int) $item->{'catalog-type'}->id;
        $entry->catalog_type_name = $item->{'catalog-type'}->name;

        if (property_exists($item->{'catalog-type'}, 'organization-id') && is_numeric($item->{'catalog-type'}->{'organization-id'})) {
          $entry->catalog_type_organization_id = (int) $item->{'catalog-type'}->{'organization-id'};
        }

        if (!empty($item->{'catalog-type'}->created) && is_string($item->{'catalog-type'}->created)) {
          $sanitized_date = strtotime($item->{'catalog-type'}->created);
          $entry->catalog_type_created = date('r', $sanitized_date);
          unset($sanitized_date);
        }

        if (!empty($item->{'catalog-type'}->modified) && is_string($item->{'catalog-type'}->modified)) {
          $sanitized_date = strtotime($item->{'catalog-type'}->modified);
          $entry->catalog_type_modified = date('r', $sanitized_date);
          unset($sanitized_date);
        }

        if (property_exists($item->{'catalog-type'}, 'legacy-id') && is_numeric($item->{'catalog-type'}->{'legacy-id'})) {
          $entry->catalog_type_legacy_id = (int) $item->{'catalog-type'}->{'legacy-id'};
        }

        if (property_exists($item, 'legacy-id') && is_numeric($item->{'legacy-id'}))  {
          $entry->legacy_id = (int) $item->{'legacy-id'};
        }
      }
    }

    $this->loaded = NULL;
    $this->processed = array($entry);
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

    // only a single catalog is used.
    if (!isset($this->processed[0]) || !is_object($this->processed[0])) {
      return FALSE;
    }

    if (!is_string($markup_type)) {
      $markup_type = 'full_html';
    }

    if ($this->list_as == self::AS_LINK) {
      $this->markup = '<a href="" class="' . $this->get_class_base_name() . '-title list-as_link">' . check_plain($this->processed[0]->name);

      if ($this->use_description) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->processed[0]->description, $markup_type) . '</div>';
        $this->markup .= '</div>';
      }

      $this->markup .= '</a>';
    }
    elseif ($this->list_as == self::AS_HEADER) {
      $this->markup = '<h3 class="' . $this->get_class_base_name() . '-title list-as_header">' . check_plain($this->processed[0]->name);

      if ($this->use_description) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->processed[0]->description, $markup_type) . '</div>';
        $this->markup .= '</div>';
      }

      $this->markup .= '</h3>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup = '<div class="' . $this->get_class_base_name() . '-title list-as_divider"' . check_plain($this->processed[0]->name);

      if ($this->use_description) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->processed[0]->description, $markup_type) . '</div>';
        $this->markup .= '</div>';
      }

      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_SPAN) {
      $this->markup = '<span class="' . $this->get_class_base_name() . '-title list-as_span">' . check_plain($this->processed[0]->name);

      if ($this->use_description) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->processed[0]->description, $markup_type) . '</div>';
        $this->markup .= '</div>';
      }

      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_LIST || $this->list_as == self::AS_LIST_ITEM) {
      $this->markup = '<li class="' . $this->get_class_base_name() . '-title list-as_list">' . check_plain($this->processed[0]->name);

      if ($this->use_description) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->processed[0]->description, $markup_type) . '</div>';
      }

      $this->markup .= '</li>';
    }

    // only shows a single entry, so a pager is not needed here.
    #$this->markup .= $this->build_pager($page, $per_page);

    return TRUE;
  }
}

/**
 * @} End of '@addtogroup mcneese_catalog'.
 */
