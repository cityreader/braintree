<?php

/**
 * @file
 * Provide Braintree vault settings page.
 */

/**
 * Braintree vault settings form.
 */
function braintree_vault_admin_settings($form, &$form_state) {
  $instances = field_info_instances('user', 'user');

  $form['vault'] = array(
    '#type' => 'fieldset',
    '#title' => t('Vault settings'),
  );

  $form['vault']['braintree_use_vault'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use vault'),
    '#description' => t('Store customer and payment method in Braintree vault.'),
    '#return_value' => 1,
    '#default_value' => variable_get('braintree_use_vault', 0),
  );

  $form['vault']['braintree_vault_field'] = array(
    '#type' => 'select',
    '#title' => t('Braintree customer ID field'),
    '#description' => t('Custom field to store customer ID in Braintree vault.'),
    '#options' =>  drupal_map_assoc(array_keys($instances)),
    '#default_value' => variable_get('braintree_vault_field', ''),
    '#required' => TRUE,
  );

  $form['payment_method'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment method settings'),
  );

  $form['payment_method']['braintree_payment_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL of payment method pages'),
    '#description' => t('Custom field to store customer ID in Braintree vault.'),
    '#default_value' => braintree_payment_method_base_url(),
    '#required' => TRUE,
  );

  $form['payment_method']['braintree_use_payment_methods_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use default payment method list page'),
    '#description' => t('Provide a default payment method list page for user to manage their payment methods stored in Braintree vault. Disable this option when you implement your own payment method list page.'),
    '#return_value' => 1,
    '#default_value' => variable_get('braintree_use_payment_methods_page', 1),
  );

  $form['payment_method']['braintree_payment_methods_custom_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Path of payment methods custom page'),
    '#description' => t('Your own payment method list page.'),
    '#default_value' => variable_get('braintree_payment_methods_custom_page', 'user'),
    '#states' => array(
      'invisible' => array(
        ':input[name="braintree_use_payment_methods_page"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form = system_settings_form($form);
  $form['#submit'][] = 'braintree_vault_admin_settings_submit';
  return $form;
}

/**
 * Submit handler for Braintree vault settings form.
 */
function braintree_vault_admin_settings_submit($form, &$form_state) {
  // Rebuild menu.
  menu_rebuild();
  drupal_set_message('Menu has been rebuilt.');
}
