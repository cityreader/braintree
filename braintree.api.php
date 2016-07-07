<?php

/**
 * @file
 * API documentation for braintree transaction.
 */

/**
 * Alter transaction sale data before Braintree transaction.
 *
 * @param $sale_data
 * @param $context
 */
function hook_braintree_transaction_sale_data_alter(&$sale_data, &$context) {
  if (isset($context['order'])) {
    $order_wrapper = entity_metadata_wrapper('commerce_order', $context['order']);

    if ($order_wrapper->field_workflow_id->value() != 'gift') {
      // Do not save to vault when using transaction for one-off parchment.
      $sale_data['options']['storeInVaultOnSuccess'] = FALSE;

      // When create a new subscription, get user email from order.
      if (isset($sale_data['customer']) && empty($sale_data['customer']['email'])) {
        $sale_data['customer']['email'] = $context['order']->mail;
      }
    }
  }
}

/**
 * Do custom logic after transaction sale succeeds.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_transaction_sale_complete($result, &$context) {
  if (isset($context['order'])) {
    $order_wrapper = entity_metadata_wrapper('commerce_order', $context['order']);

    if ($order_wrapper->field_workflow_id->value() != 'gift') {
      global $user;

      $return_customer_id = $result->transaction->customerDetails->id;

      if ($user->uid > 0) {
        $customer_id = braintree_customer_id($user);

        if (empty($customer_id) && $return_customer_id) {
          braintree_customer_id_save($user, $return_customer_id);
        }
      }
      else {
        $_SESSION['braintree_customer_id'] = $return_customer_id;
      }
    }
  }
}

/**
 * @param $customer_id
 * @param $account
 */
function hook_braintree_customer_id_alter(&$customer_id, $account) {

}
