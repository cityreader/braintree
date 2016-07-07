<?php
/**
 * @file
 * Hooks provided by the Braintree Webhooks module.
 */

use \Braintree\WebhookNotification;

/**
 * Handle Braintree Webhooks notifications as they are taken off the queue.
 *
 * @param string $kind
 *   The machine name of the webhook notification type.
 * @param array $subject
 *   An array of entities which are involved in the notification. For example:
 *   - subscription: The Braintree Subscription details
 * @param DateTime $timestamp
 *   The date and time when the event was processed by Braintree.
 */

function hook_braintree_webhooks_handle($kind, $subject, $timestamp) {
  switch ($kind) {
    case WebhookNotification::SUBSCRIPTION_CANCELED:
      break;

    case WebhookNotification::SUBSCRIPTION_CHARGED_SUCCESSFULLY:
      break;

    case WebhookNotification::SUBSCRIPTION_CHARGED_UNSUCCESSFULLY:
      break;

    case WebhookNotification::SUBSCRIPTION_EXPIRED:
      break;

    case WebhookNotification::SUBSCRIPTION_TRIAL_ENDED:
      break;

    case WebhookNotification::SUBSCRIPTION_WENT_ACTIVE:
      break;

    case WebhookNotification::SUBSCRIPTION_WENT_PAST_DUE:
      break;

    case WebhookNotification::SUB_MERCHANT_ACCOUNT_APPROVED:
      break;

    case WebhookNotification::SUB_MERCHANT_ACCOUNT_DECLINED:
      break;

    case WebhookNotification::TRANSACTION_DISBURSED:
      break;

    case WebhookNotification::DISBURSEMENT_EXCEPTION:
      break;

    case WebhookNotification::DISBURSEMENT:
      break;

    case WebhookNotification::DISPUTE_OPENED:
      break;

    case WebhookNotification::DISPUTE_LOST:
      break;

    case WebhookNotification::DISPUTE_WON:
      break;

    case WebhookNotification::PARTNER_MERCHANT_CONNECTED:
      break;

    case WebhookNotification::PARTNER_MERCHANT_DISCONNECTED:
      break;

    case WebhookNotification::PARTNER_MERCHANT_DECLINED:
      break;

    case WebhookNotification::CHECK:
      break;

    default:
      break;
  }
}
