<?php
/**
 * @file
 * Contains functions only needed for drush integration.
 */

/**
 * Implementation of hook_drush_command().
 */
function model_cli_drush_command() {
  $items = array();

  //TODO add more arguments and required/optional settings.
  $items['model-entity-create'] = array(
    'description' => 'Create a custom entity based on code in the model module.',
    'arguments' => array(
      'entity_name'         => 'A name for your custom entity.',
    ),
    'options' => array(
      'entity_name'         => array(
        'description' => 'A name for your custom entity.',
        'example-value' => 'school,vehicle'
        ),
    ),
    'examples' => array(
      'drush model-entity-create school' => 'Creates a custom entity by the name School',
    ),
    'aliases' => array('mec'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  return $items;
}

/**
 * Create a new custom module with all $entity_name code based on the model module
 */
function drush_model_cli_model_entity_create($entity_name = NULL) {
  $source_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drupal_get_path('module', 'model'));
  $destination_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT'). '/sites/all/modules/' .$entity_name);

  // Make a copy of the model module, clean up unwanted files and rename things
  drush_op('drush_copy_dir', $source_path, $destination_path);
  drush_op('drush_delete_dir', $destination_path .'/model_cli');
  // See - http://api.drush.org/api/drush/includes%21filesystem.inc/function/drush_scan_directory/6.x
  $files = drush_scan_directory(
    $destination_path, //$path
    '/model/', //$patern
    array('.', '..', 'CVS'), //$nomask (default = array('.', '..', 'CVS'))
    0, //$callback (default = 0)
    TRUE, //$recurse_max_depth (default = TRUE)
    'basename', //$key (default = 'filename')
    0, //$min_depth (default = 0)
    FALSE, //$include_dot_files (default = FALSE)
    0 //$depth (default = 0)
  );
  foreach ($files as $key => $file) {
    drush_op('file_rename_and_replace', $file, $entity_name);
  }

  drush_log("A new module '$entity_name' created based on model entity", 'ok');
  drush_print("You may now DISABLE the 'model_cli' module");
  drush_print("You may now ENABLE the '$entity_name' module and make changes to it as desired");
}

/**
 * Rename the file and replace upper and lower case strings
 */
function file_rename_and_replace($file, $entity_name) {
  $filepath_old = $file->filename;
  $filename_old = $file->basename;
  $filename_new = str_replace('model', $entity_name, $filename_old);
  $filepath_new = dirname($filepath_old)."/".$filename_new;

  drush_op('rename', $filepath_old, $filepath_new);
  $file_contents = file_get_contents($filepath_new);
  $file_contents = str_replace('model', $entity_name, $file_contents);
  $file_contents = str_replace('Model', ucfirst($entity_name), $file_contents);
  file_put_contents($filepath_new, $file_contents);
}