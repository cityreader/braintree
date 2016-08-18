<?php

/**
 * @file
 *   drush integration for brantree.
 */

/**
 * The Colorbox plugin URI.
 */
define('BRAINTREE_DOWNLOAD_URI', 'https://developers.braintreepayments.com/client_libraries/php/braintree-php-3.15.0.tgz');
define('COLORBOX_DOWNLOAD_PREFIX', 'colorbox-');

/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * See `drush topic docs-commands` for a list of recognized keys.
 *
 * @return array
 *   An associative array describing your command(s).
 */
function braintree_drush_command() {
  $items = array();

  // the key in the $items array is the name of the command.
  $items['braintree-php'] = array(
    'callback' => 'drush_braintree_php',
    'description' => dt('Download and install the Braintree php library.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap.
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Braintree php library. If omitted Drush will use the default location.'),
    ),
    'aliases' => array('braintreephp'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'
 *
 * @param string $section
 *   A string with the help section (prepend with 'drush:')
 *
 * @return string
 *   A string with the help text for your command.
 */
function braintree_drush_help($section) {
  switch ($section) {
    case 'drush:braintree-php':
      return dt('Download and install the Braintree php library from braintree website, default location is sites/all/libraries.');
  }
}

/**
 * Implements drush_MODULE_pre_pm_enable().
 */
function drush_braintree_pre_pm_enable() {
  $modules = drush_get_context('PM_ENABLE_MODULES');
  if (in_array('braintree', $modules) && !drush_get_option('skip')) {
    drush_braintree_plugin();
  }
}

/**
 * Command to download the Braintree php library.
 */
function drush_braintree_php() {
  $args = func_get_args();
  if (!empty($args[0])) {
    $path = $args[0];
  }
  else {
    $path = 'sites/all/libraries';
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Download the zip archive
  if ($filepath = drush_download_file(BRAINTREE_DOWNLOAD_URI)) {
    $filename = basename($filepath);
    $dirname = basename($filepath, '.tgz');

    // Remove any existing Colorbox plugin directory.
    if (is_dir($dirname) || is_dir('php')) {
      drush_delete_dir($dirname, TRUE);
      drush_delete_dir('braintree_php', TRUE);
      drush_log(dt('A existing braintree php library was deleted from @path', array('@path' => $path)), 'notice');
    }

    // Decompress the zip archive
    drush_tarball_extract($filename);

    // Change the directory name to "colorbox" if needed.
    if ($dirname != 'braintree_php') {
      drush_move_dir($dirname, 'braintree_php', TRUE);
      $dirname = 'braintree_php';
    }
  }

  if (is_dir($dirname)) {
    drush_log(dt('Braintree php library has been installed in @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to install the Braintree php library to @path', array('@path' => $path)), 'error');
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);
}
