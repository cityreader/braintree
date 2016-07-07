<?php

/**
 * @file
 * Provide Braintree webhooks settings page.
 */

use \Braintree\WebhookNotification;
use \Braintree\WebhookTesting;

/**
 * Braintree webhooks settings form.
 */
function braintree_webhooks_admin_settings($form, &$form_state) {
  $form['token'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webhooks token'),
  );

  // Retrieve previous form values, or load from the variable
  $token_form_value = isset($form_state['values']['braintree_webhooks_token']) ? $form_state['values']['braintree_webhooks_token'] : variable_get('braintree_webhooks_token', '');

  $form['token']['braintree_webhooks_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Token'),
    '#description' => t('Access token to be added to the webhook endpoint'),
    '#default_value' => $token_form_value,
  );

  $form['token']['generate_token'] = array(
    '#type' => 'submit',
    '#value' => t('Generate new token'),
    '#submit' => array('_braintree_webhooks_admin_generate_token'),
  );

  $site_url = url(NULL, array('absolute' => TRUE));
  $form['token']['braintree_webhooks_endpoint_url'] = array(
    '#type' => 'item',
    '#title' => t('Webhook endpoint'),
    '#description' => t('This is the URL to use in the Braintree configuration'),
    '#markup' => "<span>${site_url}braintree_webhooks/$token_form_value</span>",
  );

  $form['token']['braintree_webhooks_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable debug mode'),
    '#description' => t('Log request in watchdog'),
    '#return_value' => 1,
    '#default_value' => variable_get('braintree_webhooks_debug', 0),
  );

  $form['queue'] = array(
    '#type' => 'fieldset',
    '#title' => t('Queue'),
  );

  $form['queue']['queue_item_max_failed_attempts_' . BRAINTREE_WEBHOOKS_QUEUE] = array(
    '#type' => 'textfield',
    '#title' => t('Max number of failed attempts allowed'),
    '#description' => t('(before moving to failed queue)'),
    '#default_value' => variable_get('queue_item_max_failed_attempts_' . BRAINTREE_WEBHOOKS_QUEUE, 2),
  );

  return system_settings_form($form);
}

function _braintree_webhooks_admin_generate_token($form, &$form_state) {
  // Generate a new token
  $new_token = drupal_random_key();

  // Populate the form elements with the generated values
  $form_state['input']['braintree_webhooks_token'] = $new_token;
  $form_state['values']['braintree_webhooks_token'] = $new_token;

  $form_state['rebuild'] = TRUE;
  drupal_set_message('New token generated, please click Save to save it permanently.');
}

/**
 * Braintree webhooks sample form.
 */
function braintree_webhooks_admin_sample($form, &$form_state) {
  braintree_initialize();

  $options = braintree_webhooks_kinds();

  $form['kind'] = array(
    '#type' => 'select',
    '#title' => t('Notification kind'),
    '#options' => drupal_map_assoc($options),
    '#required' => TRUE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  if (!empty($form_state['values']['kind'])) {
    $webhook_notification = braintree_webhooks_sample_notification($form_state['values']['kind']);
    $output = '<pre>';
    $output .= print_r($webhook_notification, TRUE);
    $output .= '</pre>';

    $form['sample'] = array(
      '#markup' => $output,
      '#weight' => 200,
    );
  }

  return $form;
}

/**
 * Submit handler for Braintree webhooks testing form.
 */
function braintree_webhooks_admin_sample_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Return sample notification of Braintree webhooks.
 *
 * @param $kind
 * @return \Braintree_WebhookNotification
 * @throws \Braintree_Exception_InvalidSignature
 *
 * @see https://developers.braintreepayments.com/javascript+ruby/guides/webhooks/testing-go-live
 */
function braintree_webhooks_sample_notification($kind) {
  $sample_notification = WebhookTesting::sampleNotification(
    $kind,
    "my-id"
  );

  $webhook_notification = WebhookNotification::parse(
    $sample_notification['bt_signature'],
    $sample_notification['bt_payload']
  );

  return $webhook_notification;
}

/**
 * Braintree webhooks decode form.
 */
function braintree_webhooks_admin_decode($form, &$form_state) {
  braintree_initialize();

  if (!empty($_GET['item_id'])) {
    $item_id = (int) $_GET['item_id'];
  }

  $form['item_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Queue item ID'),
    '#required' => TRUE,
    '#default_value' => ! empty($item_id) ? $item_id : '',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  if (!empty($item_id)) {
    $result = db_select('queue', 'q')
      ->fields('q', array('data'))
      ->condition('name', ['braintree_webhooks_queue', 'braintree_webhooks_queue:failed'], 'IN')
      ->condition('item_id', $item_id)
      ->execute()
      ->fetchAssoc();

    if ($result) {
      $item = $result['data'];
      $item = unserialize($item);

      try {
        $webhook_notification = WebhookNotification::parse($item['bt_signature'], $item['bt_payload']);
      }
      catch (Exception $e) {
        $error_message = $e->getMessage();
      }

      if (!empty($webhook_notification)) {
        $kind = $webhook_notification->kind;
        $subject = $webhook_notification->subject;

        // Convert UTC timezone to Melbourne timezone.
        $webhook_notification->timestamp->setTimeZone(new DateTimeZone('Australia/Melbourne'));

        $form['created'] = array(
          '#type' => 'item',
          '#title' => t('Created'),
          '#markup' => $webhook_notification->timestamp->format('Y-m-d H:i:s'),
        );

        $form['kind'] = array(
          '#type' => 'item',
          '#title' => t('Kind'),
          '#markup' => $kind,
        );

        $source = braintree_webhooks_trigger_source($kind, $subject);

        $form['source'] = array(
          '#type' => 'item',
          '#title' => t('Source'),
          '#markup' => $source,
        );

        $form['decode'] = array(
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#title' => t('Decode data'),
        );

        $output = '<pre>';
        $output .= print_r($webhook_notification, TRUE);
        $output .= '</pre>';

        $form['decode']['data'] = array(
          '#markup' => $output,
          '#weight' => 200,
        );
      }
      else {
        drupal_set_message(t('Cannot parse this webhook notification error: %error.', ['%error'=>$error_message]), 'error');
      }

    }
    else {
      drupal_set_message(t('This queue item_id does not exist.'), 'warning');
    }
  }

  return $form;
}

/**
 * Submit handler for Braintree webhooks testing form.
 */
function braintree_webhooks_admin_decode_submit($form, &$form_state) {
  $form_state['redirect'] = array(
    BRAINTREE_WEBHOOKS_DECODE_URL,
    array(
      'query' => array(
        'item_id' => $form_state['values']['item_id'],
      ),
    )
  );
}
