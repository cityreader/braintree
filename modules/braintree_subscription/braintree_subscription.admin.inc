<?php

/**
 * @file
 * Braintree subscription admin pages.
 */


function braintree_subscription_admin_settings($form, &$form_state) {
  $entity_info = entity_get_info('commerce_product');

  $form['braintree_use_subscription'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Braintree subscription'),
    '#description' => t('Enable Braintree subscription integration.'),
    '#return_value' => 1,
    '#default_value' => variable_get('braintree_use_subscription', 0),
  );

  $form['subscription'] = array(
    '#type' => 'fieldset',
    '#title' => t('Subscription settings'),
    '#id' => 'braintree-subscription-settings',
  );

  $product_type_options = drupal_map_assoc(array_keys($entity_info['bundles']));
  $default_product_type = variable_get('braintree_subscription_product_type', '');
  if (empty($default_product_type)) {
    $default_product_type = reset($product_type_options);
  }

  $form['subscription']['braintree_subscription_product_type'] = array(
    '#type' => 'select',
    '#title' => t('Product type'),
    '#description' => t('Custom field to store customer ID in Braintree vault.'),
    '#options' => $product_type_options,
    '#default_value' => $default_product_type,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'braintree_subscription_product_type_js',
      'wrapper' => 'braintree-subscription-settings',
      'method' => 'replace',
    ),
  );

  $product_type = $default_product_type;
  if (!empty($form_state['values']['braintree_subscription_product_type'])) {
    $product_type = $form_state['values']['braintree_subscription_product_type'];
  }

  $form['subscription']['braintree_subscription_field'] = array(
    '#type' => 'select',
    '#title' => t('Field to indicate subscription'),
    '#description' => t('Custom field to store customer ID in Braintree vault.'),
    '#options' => array(),
    '#default_value' => variable_get('braintree_subscription_field', ''),
    '#required' => TRUE,
  );

  if (!empty($product_type)) {
    $instances = field_info_instances('commerce_product', $product_type);
    $form['subscription']['braintree_subscription_field']['#options'] = drupal_map_assoc(array_keys($instances));
  }

  return system_settings_form($form);
}
