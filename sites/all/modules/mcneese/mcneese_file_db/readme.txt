CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Contributers
 * Diff of index.php


INTRODUCTION
------------

This provides a stream wrapper for storing files in a database.
Proper Usage of this requires that the index.php of the drupal site to be
altered.


CONTRIBUTERS
------------
Kevin Day <thekevinday@gmail.com>


DIFF OF index.php
-----------------
diff --git a/index.php b/index.php
index 8b83199..602ce65 100644
--- a/index.php
+++ b/index.php
@@ -17,5 +17,54 @@
 define('DRUPAL_ROOT', getcwd());
 
 require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
-drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
-menu_execute_active_handler();
+
+$uri = request_uri();
+$arguments = explode('/', $uri);
+
+if (isset($arguments[1]) && $arguments[1] == 'f') {
+  drupal_bootstrap(DRUPAL_BOOTSTRAP_CONFIGURATION);
+
+  require_once DRUPAL_ROOT . '/includes/database/database.inc';
+  require_once DRUPAL_ROOT . '/includes/cache.inc';
+  spl_autoload_register('drupal_autoload_class');
+  spl_autoload_register('drupal_autoload_interface');
+
+  require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/mcneese_file_db.module';
+  require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/classes/mcneese_file_db_stream_wrapper.inc';
+  require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/classes/mcneese_file_db_unrestricted_stream_wrapper.inc';
+  //require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/classes/mcneese_file_db_restricted_stream_wrapper.inc';
+
+  mcneese_file_db_return_file($arguments);
+}
+else if (isset($arguments[1]) && $arguments[1] == 'files' && count($arguments) > 6 && $arguments[2] == 'styles') {
+  drupal_bootstrap(DRUPAL_BOOTSTRAP_CONFIGURATION);
+
+  require_once DRUPAL_ROOT . '/includes/database/database.inc';
+  require_once DRUPAL_ROOT . '/includes/cache.inc';
+  spl_autoload_register('drupal_autoload_class');
+  spl_autoload_register('drupal_autoload_interface');
+
+  require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/mcneese_file_db.module';
+  require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/classes/mcneese_file_db_stream_wrapper.inc';
+  require_once DRUPAL_ROOT . '/sites/all/modules/mcneese/mcneese_file_db/classes/mcneese_file_db_unrestricted_stream_wrapper.inc';
+
+  mcneese_file_db_register_stream_wrappers();
+
+  //if (($arguments[4] == mcneese_file_db_unrestricted_stream_wrapper::SCHEME || $arguments[4] == mcneese_file_db_restricted_stream_wrapper::SCHEME) && $arguments[5] == 'f') {
+  if ($arguments[4] == mcneese_file_db_unrestricted_stream_wrapper::SCHEME && $arguments[5] == 'f') {
+    $file_uri = DRUPAL_ROOT . $uri;
+
+    if (!file_exists($file_uri) || empty($arguments[8])) {
+      drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
+      mcneese_file_db_generate_image_style($arguments);
+      exit();
+    }
+  }
+
+  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
+  menu_execute_active_handler();
+}
+else {
+  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
+  menu_execute_active_handler();
+}
