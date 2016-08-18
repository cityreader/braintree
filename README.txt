Braintree Integration
---------------------

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Recommended modules
 * Installation
 * Configuration
 * Maintainers


INTRODUCTION
------------

The Braintree module provides powerful API integration with Braintree Payment
Gateway. By leveraging the power of its sub-modules, it not only provides
SAQ-A for PCI DSS 3.0 compliance payment method, but also can transform
Drupal site to an online portal to allow users to manage their payment
methods and subscriptions.

Here are the description for each modules:

 * braintree
   The base module to provide basic Braintree API integration
 * braintree_payment
   This module provides an alternative payment form beside Drop-in UI but also
   meets SAQ-A for PCI DSS 3.0 compliance. It actually integrates with
   hostfields and paypal of braintree.js. Compared to Drop-in UI it allows
   highly customizable UI and better UX.
 * braintree_vault
   The vault module integrates with Braintree customer vault with Drupal user
   account. By linking them together payment methods stored in the vault can be
   chosen to be used in the payment method form.
 * braintree_subscription
   The subscription module gives Braintree subscription API integrations and
   enables other modules to extend subscription functions.
 * braintree_webhooks
   This is a heavy lifting module to integrate Braintree Web hooks in order to
   stimulate automation of business process.

REQUIREMENTS
------------

This module requires the following modules:

 * Static Cache (https://www.drupal.org/sandbox/cityreader/2761967)


RECOMMENDED MODULES
-------------------

 * Commerce Braintree Web (https://www.drupal.org/sandbox/cityreader/2761941):
   Provide payment method form in Commerce checkout page.


INSTALLATION
------------

 * It is recommended to enable this module by using Drush. As Drush will
   download braintree-php library automatically.

 * If braintree-php library has not been downloaded. Use `drush braintree-php`
   to download the library.

 * Enable braintree and other required sub-modules.


CONFIGURATION
-------------

 * Configure user permissions in Administration » People » Permissions:

   - Administer Braintree settings

     Change Braintree settings.

 * Customize the menu settings in Administration » Configuration and modules »
   Web services » Braintree menu.

 * Update Braintree credential.


MAINTAINERS
-----------

Current maintainers:
 * Eric Chen (eric.chenchao) - https://www.drupal.org/user/265729
