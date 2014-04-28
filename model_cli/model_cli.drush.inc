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

  //TODO Just testing. Remove these.
  drush_print("Copy from = ". $source_path);
  drush_print("Copy to = ". $destination_path);

  // Make a copy of the model module, clean up unwanted files and rename things
  // TODO Check later if it is possible to just read all filesnames, rename and replace automatically without doing the same work for each file separately below.
  drush_op('drush_copy_dir', $source_path, $destination_path);
  drush_op('drush_delete_dir', $destination_path .'/model_cli');
  drush_op('rename', drush_normalize_path($destination_path .'/model.info'), 
    drush_normalize_path($destination_path .'/'. $entity_name .'.info'));
  drush_op('rename', drush_normalize_path($destination_path .'/model.install'), 
    drush_normalize_path($destination_path .'/'. $entity_name .'.install'));
  drush_op('rename', drush_normalize_path($destination_path .'/model.module'), 
    drush_normalize_path($destination_path .'/'. $entity_name .'.module'));
  drush_op('rename', drush_normalize_path($destination_path .'/model.admin.inc'), 
    drush_normalize_path($destination_path .'/'. $entity_name .'.admin.inc'));

  drush_op('file_str_replace', drush_normalize_path($destination_path .'/'. $entity_name .'.info'), 'model', $entity_name);
  drush_op('file_str_replace', drush_normalize_path($destination_path .'/'. $entity_name .'.install'), 'model', $entity_name);
  drush_op('file_str_replace', drush_normalize_path($destination_path .'/'. $entity_name .'.module'), 'model', $entity_name);
  drush_op('file_str_replace', drush_normalize_path($destination_path .'/'. $entity_name .'.admin.inc'), 'model', $entity_name);


  //TODO Just testing. Remove this.
  drush_log("Hello world says KAKA!", 'ok');

}

/**
 * Replace strings in a file.
 */
function file_str_replace($file_path, $find, $replace) {
  $file_path = drush_normalize_path($file_path);
  $file_contents = file_get_contents($file_path);
  $file_contents = str_replace($find, $replace, $file_contents);
  //TODO do the upper case conversion here
  file_put_contents($file_path, $file_contents);
}