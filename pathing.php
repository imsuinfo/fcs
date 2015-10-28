<?php

define('DRUPAL_ROOT', getcwd());
require_once 'includes/bootstrap.inc';
require_once 'includes/database/database.inc';

drupal_bootstrap(DRUPAL_BOOTSTRAP_CONFIGURATION);


/**
 * Gets the current timestamp of the connection.
 *
 * If the server did not supply a timestamp, one will be generated.
 * This utilizes a static variable, so it will be queried only once.
 *
 * @param bool $reset
 *   (optional) If TRUE, then the instance timestamp will be re-generated.
 *
 * @return int
 *   Unix timestamp for the current time.
 */
function pathing_get_instance($reset = FALSE) {
  static $instance = NULL;

  if (!is_null($instance) && $reset === FALSE) {
    return $instance;
  }

  if ($reset === TRUE) {
    $instance = strtotime('now');
    return $instance;
  }

  if (array_key_exists('REQUEST_TIME', $_SERVER)) {
    $instance = (int) $_SERVER['REQUEST_TIME'];
  }

  if (empty($instance)) {
    $instance = strtotime('now');
  }

  return $instance;
}

/**
 * Exits with 404.
 */
function pathing_not_found() {
  $instance = pathing_get_instance();

  $headers = array();
  $headers['status'] = array('value' => '404 Not Found', 'status_code' => 404);
  $headers['Content-Type'] = array('value' => 'text/html');
  $headers['Date'] = array('value' => gmdate(DATE_RFC1123, $instance));

  unrestricted_send_headers($headers);

  # load the not found page, if it exists.
  $file = getcwd() . '/not_found/index.html';
  if (file_exists($file)) {
    print(file_get_contents($file));
  }
  else {
    print("<h1>Not Found (404)</h1>\n");
  }

  exit();
}

/**
 * Parses the request uri.
 *
 * @param int $uri_unshift
 *   (optional) Number representing how many arguments to unshift from the uri.
 *   The default is 1 because the initial / is considered an argument.
 *   A website sitting at: www.example.com/mysite will need an unshift value of
 *   2.
 *
 * @return array
 *   An array containing the uri which each key representing an argument
 *   between the / in the uri.
 */
function pathing_get_uri($uri_unshift = 1) {
  if (empty($_SERVER['REQUEST_URI'])) {
    pathing_not_found();
  }

  $uri_parts = explode('/', $_SERVER['REQUEST_URI']);
  if (!is_array($uri_parts)) {
    pathing_not_found();
  }

  for ($i = 0; $i < $uri_unshift; $i++) {
    array_shift($uri_parts);
  }
  unset($i);

  $uri = array();
  foreach ($uri_parts as $uri_part) {
    $uri[] = $uri_part;
  }

  return $uri;
}

/**
 * Main Function
 */
function pathing_main() {
  $uri = pathing_get_uri();

  // currently operate on /node/ urls only at this time.
  if (count($uri) == 2 && ($uri[0] == 'node')) {
    $path = implode($uri, '/');
    unset($uri);
    try {
      $results = db_query('select ua.alias from {url_alias} ua where ua.source = :source order by ua.pid', array(':source' => $path))->fetchAll();

      if (empty($results[0]->alias)) {
        unset($results);
        drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
        menu_execute_active_handler();
      }
      else {
        global $base_path;
        header('Location: ' . $base_path . $results[0]->alias, TRUE, 301);
      }
    }
    catch (Exception $ex) {
      // if something goes wrong, simply jump into the normal drupal load functions.
      drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
      menu_execute_active_handler();
    }
  }
  else {
    unset($uri);
    drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
    menu_execute_active_handler();
  }
}

pathing_main();
