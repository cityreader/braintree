<?php

/**
 * @file
 * API documentation for braintree vault customer and payment methods.
 */

/**
 * Alter customer data before saving into Braintree vault.
 *
 * @param $customer_data
 * @param $context
 */
function hook_braintree_vault_customer_data_alter(&$customer_data, &$context) {
  if (isset($context['order'])) {
    $order_wrapper = entity_metadata_wrapper('commerce_order', $context['order']);

    if ($order_wrapper->field_workflow_id->value() != 'gift') {
      // When create a new subscription, get user email from order.
      if (empty($customer_data['email'])) {
        $customer_data['email'] = mymodule_order_get_email($context['order']);
      }
    }
  }
}

/**
 * Do custom logic after a customer is saved into Braintree vault.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_vault_customer_saved($result, &$context) {
  if (isset($context['order'])) {
    $order_wrapper = entity_metadata_wrapper('commerce_order', $context['order']);

    if ($order_wrapper->field_workflow_id->value() != 'gift') {
      global $user;
      $return_customer_id = $result->customer->id;

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
 * Alter payment method data before saving to Braintree vault.
 *
 * @param $payment_method
 * @param $context
 */
function hook_braintree_vault_payment_method_alter(&$payment_method, &$context) {

}

/**
 * Do custom logic after inserting a new payment method.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_vault_payment_method_saved($result, &$context) {

}

/**
 * Do custom logic after deleting a payment method.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_vault_payment_method_delete($result, &$context) {

}

/**
 * Do custom logic after changing a default payment method.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_vault_payment_method_make_default($result, &$context) {

}
