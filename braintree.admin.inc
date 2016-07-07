<?php
/**
 * @file
 * Provides Braintree admin settings pages.
 */

/**
 * Braintree admin settings form.
 */
function braintree_admin_settings($form, &$form_state) {
  $settings = braintree_settings();

  $form['environment'] = array(
    '#title' => t('Environment'),
    '#description' => t('Braintree working environment.'),
    '#type' => 'radios',
    '#options' => array(
      'sandbox' => t('Sandbox'),
      'production' => t('Production'),
    ),
    '#default_value' => $settings['environment'],
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'braintree_admin_settings_js',
      'wrapper' => 'braintree-production',
    ),
  );

  $form['braintree_debug'] = array(
    '#title' => t('Enable Braintree debug mode.'),
    '#description' => t('Debug log will be saved in your private://tmp/braintree directory.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('braintree_debug', 0),
    '#return_value' => 1,
  );

  $environment = $settings['environment'];
  if (!empty($form_state['values']['environment'])) {
    $environment = $form_state['values']['environment'];
  }

  $form['sandbox'] = array(
    '#title' => t('Sandbox environment'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );

  $form['sandbox']['merchant_id'] = array(
    '#title' => t('Merchant ID'),
    '#type' => 'textfield',
    '#default_value' => $settings['sandbox']['merchant_id'],
    '#required' => TRUE,
  );

  $form['sandbox']['public_key'] = array(
    '#title' => t('Public key'),
    '#type' => 'textfield',
    '#default_value' => $settings['sandbox']['public_key'],
    '#required' => TRUE,
  );

  $form['sandbox']['private_key'] = array(
    '#title' => t('Private key'),
    '#type' => 'textfield',
    '#default_value' => $settings['sandbox']['private_key'],
    '#required' => TRUE,
  );

  $form['sandbox']['merchant_account_id'] = array(
    '#title' => t('Default merchant account ID'),
    '#type' => 'textfield',
    '#default_value' => $settings['sandbox']['merchant_account_id'],
    '#required' => TRUE,
  );

  $form['production'] = array(
    '#title' => t('Production environment'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#id' => 'braintree-production',
    '#tree' => TRUE,
  );

  $form['production']['merchant_id'] = array(
    '#title' => t('Merchant ID'),
    '#type' => 'textfield',
    '#default_value' => $settings['production']['merchant_id'],
    '#required' => ($environment == 'production'),
  );

  $form['production']['public_key'] = array(
    '#title' => t('Public key'),
    '#type' => 'textfield',
    '#default_value' => $settings['production']['public_key'],
    '#required' => ($environment == 'production'),
  );

  $form['production']['private_key'] = array(
    '#title' => t('Private key'),
    '#type' => 'textfield',
    '#default_value' => $settings['production']['private_key'],
    '#required' => ($environment == 'production'),
  );

  $form['production']['merchant_account_id'] = array(
    '#title' => t('Default merchant account ID'),
    '#type' => 'textfield',
    '#default_value' => $settings['production']['merchant_account_id'],
    '#required' => ($environment == 'production'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['actions']['test'] = array(
    '#type' => 'submit',
    '#value' => t('Test connection'),
    '#submit' => array('braintree_test_connection_submit'),
  );

  return $form;
}

function braintree_admin_settings_js($form, $form_state) {
  return $form['production'];
}

/**
 * Validate handler for Braintree settings form.
 *
 * @todo implement test connection function
 */
function braintree_admin_settings_validate($form, &$form_state) {
  $vaules = $form_state['values'];

//  if ($vaules['environment'] == 'production') {
//    foreach (array_keys($form['production']) as $field) {
//      if (empty($vaules[$field])) {
//        form_set_error('production][' . $field, t('@field_name is required', array(
//          '@field_name' => $form['production'][$field]['#title']
//        )));
//      }
//    }
//  }

//  $merchantAccount = Braintree_MerchantAccount::find('the_merchant_account_id');
}

/**
 * Submit callback; test braintree connection.
 */
function braintree_test_connection_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  $values = $form_state['values'];

  $environment = $values['environment'];
  $settings = $values[$environment];
  $settings['environment'] = $environment;

  braintree_initialize($settings);

  try {
    $merchantAccount = Braintree_MerchantAccount::find($settings['merchant_account_id']);
    drupal_set_message(t('Braintree connection succeeded.'));
  }
  catch (Exception $e) {
    $error_message = braintree_exception_error_message($e, TRUE);
    $message = t('Braintree connection failed: @message. Please check Braintree gateway credentials.', array(
      '@message' => $error_message,
    ));

    drupal_set_message($message, 'error');
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for Braintree settings form.
 */
function braintree_admin_settings_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  $values = $form_state['values'];

  variable_set('braintree_debug', $values['braintree_debug']);
  unset($values['braintree_debug']);

  variable_set('braintree_settings', $values);
}
