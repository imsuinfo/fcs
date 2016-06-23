<?php

/**
 * @file
 * Provides the node handler class for McNeese url paths.
 */

/**
 * @addtogroup mcneese_url_paths
 * @{
 */
class mcneese_url_paths_node_handler {
  const FAILSAFE_GROUP_PATH = 'campus';

  private $alias_path;
  private $alias_path_alternate;
  private $alias_path_legacy;
  private $group_path;
  private $group_path_alternate;
  private $group_path_legacy;
  private $group_representative;
  private $node_id;
  private $access_id;


  /**
   * Class constructor.
   */
  public function __construct() {
    $alias_path = '';
    $group_path = '';
    $group_representative = FALSE;
    $node_id = NULL;
    $access_id = NULL;
  }

  /**
   * Class destructor.
   */
  public function __destruct() {
    unset($alias_path);
    unset($group_path);
    unset($group_representative);
    unset($node_id);
    unset($access_id);
  }

  /**
   * Set whether or not this path represents the group.
   *
   * @param boolean $group_representative
   *   A boolean value to assign as the group representative.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  public function set_group_representative($group_representative) {
    if (!is_bool($group_representative)) {
      cf_error::invalid_bool('group_representative');
      $this->group_representative = FALSE;
      return FALSE;
    }

    $this->group_representative = $group_representative;
    return TRUE;
  }

  /**
   * Set the url path.
   *
   * @param string $alias_path
   *   A non-empty url path to assign.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  public function set_alias_path($alias_path) {
    if (cf_is_empty_or_non_string('alias_path', $alias_path)) {
      $this->alias_path = '';
      return FALSE;
    }

    $this->alias_path = $this->p_sanitize_partial_path($alias_path);
    $this->alias_path_alternate = $this->p_sanitize_partial_path($alias_path, TRUE);
    $this->alias_path_legacy = $this->p_sanitize_partial_path_legacy($alias_path);
    return TRUE;
  }

  /**
   * Sets the node object
   *
   * @param int $node_id
   *   A node id of the node to assign.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  public function set_node_id($node_id) {
    if (!is_numeric($node_id)) {
      cf_error::invalid_numeric('node_id', $node_id);
      $this->node_id = NULL;
      return FALSE;
    }

    $this->node_id = $node_id;
    return TRUE;
  }

  /**
   * Set the access id.
   *
   * @param int $access_id
   *   An access_id integer for the workbench_access taxonomy.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  public function set_access_id($access_id) {
    if (!is_numeric($access_id)) {
      cf_error::invalid_numeric('access_id', $access_id);
      return FALSE;
    }

    $this->access_id = intval($access_id);

    $menus = (array) workbench_menu_get_menus(array('access_id' => $this->access_id, 'fast' => NULL), 'access_id');

    if (isset($menus[$this->access_id])) {
      $this->group_path = $this->p_sanitize_partial_path($menus[$this->access_id]->path);
      $this->group_path_alternate = $this->p_sanitize_partial_path($menus[$this->access_id]->path, TRUE);
      $this->group_path_legacy = $this->p_sanitize_partial_path_legacy($menus[$this->access_id]->path);
    }
    else {
      $this->group_path = $this->p_sanitize_partial_path($this::FAILSAFE_GROUP_PATH);
    }

    return TRUE;
  }

  /**
   * Returns the generated source path.
   *
   * @return string
   *   The generated source path.
   */
  public function get_source_path() {
    $path = '';

    if ($this->node_id > 0) {
      $path = 'node/' . $this->node_id;
    }

    return $path;
  }

  /**
   * Returns the generated destination path.
   *
   * @param int $type
   *   When set to 0, returns the normal path.
   *   When set to 1, returns the alternate path.
   *   When set to 2, returns the legacy path.
   *
   * @return string
   *   The generated destination path.
   */
  public function get_destination_path($type = 0) {
    $path = '';

    if ($this->group_representative) {
      if ($type == 0 && !empty($this->group_path)) {
        $path = $this->group_path;
      }
      else if ($type == 1 && !empty($this->group_path_alternate)) {
        $path = $this->group_path_alternate;
      }
      else if ($type == 1 && !empty($this->group_path_legacy)) {
        $path = $this->group_path_legacy;
      }
    }
    else if ($type == 0) {
      if (empty($this->group_path)) {
        $path = $this->alias_path;
      }
      else if (!empty($this->alias_path)) {
        $path = $this->group_path . '/' . $this->alias_path;
      }
    }
    else if ($type == 1) {
      if (empty($this->group_path_alternate)) {
        $path = $this->alias_path_alternate;
      }
      else if (!empty($this->alias_path_alternate)) {
        $path = $this->group_path_alternate . '/' . $this->alias_path_alternate;
      }
    }
    else if ($type == 2) {
      if (empty($this->group_path_legacy)) {
        $path = $this->alias_path_legacy;
      }
      else if (!empty($this->alias_path_legacy)) {
        $path = $this->group_path_legacy . '/' . $this->alias_path_legacy;
      }
    }

    $path = drupal_strtolower($path);

    return $path;
  }

  /**
   * Returns the node id.
   *
   * @return int
   *   The node id.
   */
  public function get_node_id() {
    return $this->node_id;
  }

  /**
   * Returns the access id.
   *
   * @return int
   *   The access id.
   */
  public function get_access_id() {
    return $this->access_id;
  }

  /**
   * Returns the group representative..
   *
   * @return bool
   *   The group representative.
   */
  public function get_group_representative() {
    return $this->group_representative;
  }

  /**
   * Deletes an existing url path.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  public function delete_path_alias() {
    $source = $this->get_source_path();
    $destination_0 = $this->get_destination_path(0);
    $destination_1 = $this->get_destination_path(1);
    $destination_2 = $this->get_destination_path(2);

    if (empty($source) || empty($destination_0)) {
      return FALSE;
    }

    $transaction = db_transaction();

    $deleted = $this->p_delete_path_alias($source, $destination_0, $transaction);

    if ($deleted && !empty($destination_1)) {
      $deleted = $this->p_delete_path_alias($source, $destination_1, $transaction);
    }

    if ($deleted && !empty($destination_2)) {
      $deleted = $this->p_delete_path_alias($source, $destination_2, $transaction);
    }

    return $deleted;
  }

  /**
   * Creates a url path.
   *
   * If an old path exists, then it gets deleted.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  public function create_path_alias() {
    $source = $this->get_source_path();
    $destination_0 = $this->get_destination_path(0);
    $destination_1 = $this->get_destination_path(1);
    $destination_2 = $this->get_destination_path(2);

    if (empty($source) || empty($destination_0)) {
      return FALSE;
    }

    // if the exact same alias already exists, do nothing.
    $exists_0 = $this->p_alias_exists($source, $destination_0);
    $exists_1 = $this->p_alias_exists($source, $destination_1);

    if ($exists_0 && ($exists_1 || $destination_0 == $destination_1)) {
      return TRUE;
    }

    $transaction = db_transaction();

    if (!$this->p_delete_path_alias($source, $destination_0, $transaction)) {
      return FALSE;
    }

    if (!empty($destination_1)) {
      if (!$this->p_delete_path_alias($source, $destination_1, $transaction)) {
        return FALSE;
      }
    }

    if (!empty($destination_2)) {
      if (!$this->p_delete_path_alias($source, $destination_2, $transaction)) {
        return FALSE;
      }
    }

    $created = $this->p_create_path_alias($source, $destination_0, $transaction);

    if ($created && !empty($destination_1) && $destination_0 != $destination_1) {
      $this->p_create_path_alias($source, $destination_1, $transaction);
    }

    return $created;
  }

  /**
   * Sanitize a url path piece.
   *
   * Partial paths do not have '/' in them.
   *
   * @param string $path
   *   A partial url path.
   *   All '/' will be removed/converted.
   *
   * @param bool $alternate
   *   When TRUE generates the alternate path.
   *   WHen FALSE generates the normal path.
   *
   * @return string
   *   A sanitized partial path.
   */
  private function p_sanitize_partial_path($path, $alternate = FALSE) {
    $sanitized = preg_replace('/^(\s*\/)+/i', '', $path);

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/\s*$/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/(`|~|!|@|#|\$|\||\^|%|\&|\*|\(|\)|\+|\\\\|=|{|}|[|]|:|;|\'|"|,|\?|<|>)/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      if ($alternate) {
        $sanitized = preg_replace('/\s/i', '-', $sanitized);
      }
      else {
        $sanitized = preg_replace('/\s/i', '_', $sanitized);
      }
    }

    if (is_string($sanitized)) {
      if ($alternate) {
        $sanitized = preg_replace('/_/i', '-', $sanitized);
      }
    }

    if (is_string($sanitized)) {
      if ($alternate) {
        $sanitized = preg_replace('/_-/i', '-', $sanitized);

        if (is_string($sanitized)) {
          $sanitized = preg_replace('/-_/i', '-', $sanitized);
        }
      }
      else {
        $sanitized = preg_replace('/_-/i', '_', $sanitized);

        if (is_string($sanitized)) {
          $sanitized = preg_replace('/-_/i', '_', $sanitized);
        }
      }
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/_+/i', '_', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/-+/i', '-', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/^(_|-)+/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/(_|-)+$/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      return $sanitized;
    }

    return '';
  }

  /**
   * Sanitize a url path piece using the legacy method.
   *
   * Partial paths do not have '/' in them.
   *
   * @param string $path
   *   A partial url path.
   *   All '/' will be removed/converted.
   *
   * @return string
   *   A sanitized partial path.
   */
  private function p_sanitize_partial_path_legacy($path) {
    $sanitized = preg_replace('/^(\s*\/)+/i', '', $path);

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/\s*$/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/(`|~|!|@|#|\$|\||\^|%|\&|\*|\(|\)|\+|\\\\|=|{|}|[|]|:|;|\'|"|,|\?|<|>|\s)/i', '_', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/_+/i', '_', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/-+/i', '-', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/^(_|-)+/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      $sanitized = preg_replace('/(_|-)+$/i', '', $sanitized);
    }

    if (is_string($sanitized)) {
      return $sanitized;
    }

    return '';
  }

  /**
   * Deletes an existing url path.
   *
   * This is the internal function not to be directly called.
   *
   * @param string $source
   *   The source path string.
   * @param $string $destination
   *   The destination path string.
   * @param object $transaction
   *   The database transaction object.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  private function p_delete_path_alias($source, $destination, &$transaction) {
    try {
      // only delete the known destination path if one exists.
      // do not delete using the source path because multiple source paths may exist.
      $query = db_delete('url_alias');
      $query->condition('source', $source);


      // the 'alias'-'node_id' path might also exist due to path conflict avoidance.
      // delete this as well.
      if ($this->node_id > 0) {
        $or = db_or();
        $or->condition('alias', $destination);
        $or->condition('alias', $destination . '-' . $this->node_id);
        $query->condition($or);
      }
      else {
        $query->condition('alias', $destination);
      }

      $query->execute();
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
      return FALSE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Creates an existing url path.
   *
   * This is the internal function not to be directly called.
   *
   * @param string $source
   *   The source path string.
   * @param $string $destination
   *   The destination path string.
   * @param object $transaction
   *   The database transaction object.
   *
   * @return bool
   *   Returns TRUE on success, FALSE on failure.
   */
  private function p_create_path_alias($source, $destination, &$transaction) {
    $record = array();
    $record['source'] = $source;
    $record['alias'] = $destination;
    $record['language'] = LANGUAGE_NONE;

    try {
      // check to see if the given alias exists, and if so generate another.
      if ($this->node_id > 0) {
        $query = db_select('url_alias', 'ua');

        $query->fields('ua');
        $query->condition('alias', $destination);
        $query->condition('source', $source, '<>');
        $results = $query->execute();

        if ($results->rowCount() > 0) {
          $record['alias'] .= '-' . $this->node_id;
        }
      }

      $query = db_insert('url_alias');
      $query->fields($record);
      $query->execute();
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
      return FALSE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Checks to see if the exact source and destination already exist.
   *
   * @param string $source
   *   The source path string.
   * @param $string $destination
   *   The destination path string.
   *
   * @return bool
   *   Returns TRUE on exists, FALSE otherwise.
   */
  private function p_alias_exists($source, $destination) {
    try {
      if ($this->node_id > 0) {
        $query = db_select('url_alias', 'ua');

        $query->fields('ua');
        $query->condition('alias', $destination);
        $query->condition('source', $source);
        $results = $query->execute();

        if ($results->rowCount() > 0) {
          return TRUE;
        }
      }
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
      return FALSE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_query_execution($e);
      return FALSE;
    }

    return FALSE;
  }
}

/**
 * @} End of '@addtogroup mcneese_url_paths'.
 */
