<?php

/**
 * @file
 * Defines McNeese Catalog hierarchies class.
 */

/**
 * @addtogroup mcneese_catalog
 * @{
 */

class mcneese_catalog_class_hierarchies extends mcneese_catalog_class_base {
  /**
   * Class constuctor.
   */
  public function __construct($catalog_id, $legacy_id) {
    parent::__construct($catalog_id, $legacy_id);

    $this->table          = 'mcneese_catalog_hierarchies';
    $this->path_name      = 'hierarchies';
    $this->cache_name     = 'hierarchies';
    $this->cache_lifetime = '+24 hours';

    if ($this->show_invisible) {
      $this->select_condition = ' where active';
    }
    else {
      $this->select_condition = ' where visible and active';
    }

    $this->list_name = 'hierarchy-list';
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
      if (property_exists($item->status, 'searchable') && !$item->status->searchable) {
        $entry->searchable = FALSE;
      }

      $entry->college_types = array();
      if (property_exists($item, 'college-types') && is_array($item->{'college-types'})) {
        $entry->college_types = $item->{'college-types'};
      }

      // @fixme: this is not json. this format needs to be determined and used instead of a string.
      $entry->options = '';
      if (property_exists($item, 'options') && is_string($item->{'options'})) {
        $entry->options = $item->{'options'};
      }

      $entry->department_types = array();
      if (property_exists($item, 'department-types') && is_array($item->{'department-types'})) {
        $entry->department_types = $item->{'department-types'};
      }

      $entry->hierarchy_type = '';
      if (property_exists($item, 'hierarchy-type') && is_string($item->{'hierarchy-type'})) {
        $entry->hierarchy_type = substr($item->{'hierarchy-type'}, 0, 128);
      }

      $this->processed[] = $entry;
      unset($entry);
    }

    $this->loaded = NULL;
    return TRUE;
  }
}

/**
 * @} End of '@addtogroup mcneese_catalog'.
 */
