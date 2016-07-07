(function($) {

  Drupal.behaviors.braintreePaymentMethodChange = {
    attach: function(context, settings) {
      var $paymentMethod = $('input[name="commerce_payment[payment_method]"], input[name="payment_method"]', context);

      if ($paymentMethod.length > 0) {
        var formId = settings.braintree.formId;
        var $form = $('#' + formId);

        $paymentMethod.bind('change', function(event) {
          // Show spinning icon.
          $('.checkout-processing', this.$form).removeClass('element-invisible');

          // Clean up process.
          Drupal.myBraintree && Drupal.myBraintree.cleanUp();

          // Destroy Braintree integration.
          Drupal.myBraintreeIntegration && Drupal.myBraintreeIntegration.teardown($.proxy(Drupal.myBraintree.teardown, Drupal.myBraintree));
        });
      }
    }
  }

  Drupal.behaviors.braintreePaymentForm = {
    attach: function(context, settings) {
      // Only continue when first page load or context has id payment-details.
      if (context != document && $(context).attr('id') != 'payment-details') {
        return;
      }

      var $body = $('body');

      var bootstrapBraitnree = function() {
        if (typeof settings.braintree != 'undefined' && typeof braintree != 'undefined') {
          var $form = $('#' + settings.braintree.formId);
          Drupal.myBraintree = new Drupal.braintree($form, settings.braintree);

          // Give other modules a chance to override functions.
          setTimeout(function () {
            Drupal.myBraintree.bootstrap();
            // Hide spinning icon.
            $('.checkout-processing', this.$form).addClass('element-invisible');
          }, 20);

          $body.trigger('drupal.braintree.bootstrap');
        }
      }

      var destroyBraintree = function() {
        Drupal.myBraintreeIntegration && Drupal.myBraintreeIntegration.teardown($.proxy(Drupal.myBraintree.teardown, Drupal.myBraintree));
      }

      var $usingExistingPaymentMethod = $('input[name$="[braintree][using_existing_payment_method]"]');
      if ($usingExistingPaymentMethod.length > 0) {

        var _class = 'using-existing-payment-method';

        var text1 = Drupal.t('Add new payment method');
        var text2 = Drupal.t('Use saved payment method');
        var $toggleLinkWrapper = $('<div class="braintree-payment-toggle-link"><a href="#" class="arrow-right arrow-before"></a></div>');
        var $toggleLink = $toggleLinkWrapper.find('a');

        if ($usingExistingPaymentMethod.val() == 1) {
          $body.addClass(_class);
          $toggleLink.text(text1);
        }
        else {
          $toggleLink.text(text2);
          bootstrapBraitnree();
        }

        $toggleLink.click(function() {
          if ($body.hasClass(_class)) {
            $body.removeClass(_class);
            $toggleLink.text(text2);

            $usingExistingPaymentMethod.val(0);

            bootstrapBraitnree();
          }
          else {
            $body.addClass(_class);
            $toggleLink.text(text1);

            $usingExistingPaymentMethod.val(1);

            destroyBraintree();
          }

          return false;
        });

        var $existingPaymentMethod = $('div[class$="-payment-details-braintree-payment-method-token"]');
        $existingPaymentMethod.after($toggleLinkWrapper);

      }
      else {
        bootstrapBraitnree();
      }

      // Braintree hijacks all submit buttons for this form. Tear down
      // Braintree integration to continue.
      $('.checkout-cancel, .checkout-back, #edit-prev', context).click(destroyBraintree);

    },

    detach: function (context, settings) {
      //if ($('#payment-details', context).length > 0) {
      //  Drupal.myBraintreeIntegration && Drupal.myBraintreeIntegration.teardown($.proxy(Drupal.myBraintree.teardown, Drupal.myBraintree));
      //}
    }
  }

  Drupal.braintree = function($form, settings) {
    this.settings = settings;
    this.$form = $form;
    this.formId = this.$form.attr('id');
    this.$submit = this.$form.find('#edit-continue, #edit-submit, #edit-next');  // don't add all submits. Some go back, we only want to disable forwards.

    return this;
  }

  Drupal.braintree.prototype.bootstrap = function() {
    var options = this.getOptions(this.settings.integration);

    braintree.setup(this.settings.clientToken, this.settings.integration, options);

    if (this.settings.integration == 'paypal') {
      this.bootstrapPaypal();
    }
  }

  Drupal.braintree.prototype.bootstrapPaypal = function() {
    this.$submit.attr('disabled', 'disabled');
  }

  Drupal.braintree.prototype.resetSubmitBtn = function() {
    $('.checkout-processing', this.$form).addClass('element-invisible');

    // Remove disbabled attribute added by commerce_checkout.js
    this.$submit.next('.checkout-continue').removeAttr('disabled');

    // Reset submit button.
    if (Drupal.behaviors.hideSubmitBlockit) {
      $.event.trigger('clientsideValidationFormHasErrors', this.$form);
    }
  }

  Drupal.braintree.prototype.jsValidateErrorHandler = function(response) {
    var message = this.errorMsg(response);
    this.showError(message);
  }

  Drupal.braintree.prototype.errorMsg = function(response) {
    var message;

    switch (response.message) {
      case 'User did not enter a payment method':
        message = Drupal.t('Please enter your credit card details.');
        break;

      case 'Some payment method input fields are invalid.':
        var fieldName = '';
        var fields = [];
        var invalidFields = this.$form.find('.braintree-hosted-fields-invalid');

        function getFieldName(id) {
          return id.replace('-', ' ');
        }

        if (invalidFields.length > 0) {
          invalidFields.each(function(index) {
            var id = $(this).attr('id');

            fields.push(Drupal.t(getFieldName(id)));
          });

          if (fields.length > 1) {
            var last = fields.pop()
            fieldName = fields.join(', ');
            fieldName += ' and ' + Drupal.t(last);
            message = Drupal.t('The @field you entered are invalid', {'@field': fieldName});
          }
          else {
            fieldName = fields.pop();
            message = Drupal.t('The @field you entered is invalid', {'@field': fieldName});
          }

        }
        else {
          message = Drupal.t('The payment details you entered are invalid');
        }

        message += Drupal.t(', please check your details and try again.');

        break;

      default:
        message = response.message;
    }

    return message;
  }

  Drupal.braintree.prototype.showError = function(message) {
    this.resetSubmitBtn();

    if (Drupal.myClientsideValidation) {
      if (typeof this.validator == 'undefined') {
        this.validator = Drupal.myClientsideValidation.validators[this.formId];
      }

      var errors = {
        'braintree[errors]' : message
      };

      this.validator.showErrors(errors);
    }
  }

  /**
   * Dismiss error messages generated by clientside_validation module.
   *
   * @see Drupal.clientsideValidation.prototype.bindRules()
   */
  Drupal.braintree.prototype.dismissErrors = function() {
    // We can only hide this box.
    $(".clientside-error").hide();
  }

  /**
   * Clean up process triggered after changing payment method.
   */
  Drupal.braintree.prototype.cleanUp = function() {
    this.dismissErrors();
    // Remove hidden input op generated by hide_submit module.
    // If op value is submitted, $form_state['trigger_element'] will be
    // replaced.
    this.$submit.next('input[type=hidden][name=op]').remove();
  }

  Drupal.braintree.prototype.getOptions = function(integration) {
    var self = this;

    var options = {
      onReady: $.proxy(self.onReady, self),
      onError: $.proxy(self.onError, self)
    }

    var getCustomOptions = function() {
      options.id = self.formId;
      options.hostedFields = {};

      // Set up hosted fields selector
      options.hostedFields = $.extend(options.hostedFields, self.settings.hostedFields);
      options.hostedFields.styles = self.settings.fieldsStyles;
      options.hostedFields.onFieldEvent = self.onFieldEvent;

      return options;
    }

    var getPayPalOptions = function() {
      options.container = self.settings.paypalContainer;
      options.onPaymentMethodReceived = $.proxy(self.onPaymentMethodReceived, self);

      if (typeof self.settings.displayName != 'undefined') {
        options.displayName = self.settings.displayName;
      }

      if (typeof self.settings.singleUse != 'undefined' && self.settings.singleUse) {
        options.singleUse = self.settings.singleUse;
        options.amount = self.settings.amount;
        options.currency = self.settings.currency;
      }

      return options;
    }

    if (integration == 'paypal') {
      options = getPayPalOptions();
    }
    else {
      options = getCustomOptions();
    }

    // Add fraud protection - ZVT-25
    // For CC transactions, and Paypal non-recurring vault transactions
    // see https://developers.braintreepayments.com/guides/paypal/vault/javascript/v2#collecting-device-data
    // Just CC for now (we don't use any Paypal non-recurring vault transxns?)
    if (integration == 'custom') {
      options.dataCollector = self.settings.dataCollector;
    }
    else {
      // Not sure if have to delete both of these or just from options?
      delete options.dataCollector;
      delete self.settings.dataCollector;
    }

    return options;
  }

  // Global event callback

  Drupal.braintree.prototype.onReady = function (braintreeInstance) {
    Drupal.myBraintreeIntegration = braintreeInstance;

    // Add fraud protection - ZVT-25
    if (this.settings.integration == 'custom') {
      var form = document.getElementById(this.formId);
      var deviceDataInput = form['device_data'];
      if (deviceDataInput == null) {
        deviceDataInput = document.createElement('input');
        deviceDataInput.name = 'device_data';
        deviceDataInput.type = 'hidden';
        form.appendChild(deviceDataInput);
      }

      deviceDataInput.value = braintreeInstance.deviceData;
    }
  }

  Drupal.braintree.prototype.onError = function (response) {
    if (response.type == 'VALIDATION') {
      this.jsValidateErrorHandler(response);
    }
    else {
      !console && console.log('Other error', arguments);
    }
  }

  // Hosted Fields event callback

  /**
   * You can subscribe to events using the onFieldEvent callback. This
   * allows you to hook into focus, blur, and fieldStateChange.
   *
   * @param event
   *
   * @see https://developers.braintreepayments.com/javascript+php/guides/hosted-fields/events
   */
  Drupal.braintree.prototype.onFieldEvent = function (event) {
    //if (event.type === "focus") {
    //  // Handle focus
    //} else if (event.type === "blur") {
    //  // Handle blur
    //} else if (event.type === "fieldStateChange") {
    //  // Handle a change in validation or card type
    //  console.log(event.isValid); // true|false
    //  if (event.card) {
    //    console.log(event.card.type);
    //    // visa|master-card|american-express|diners-club|discover|jcb|unionpay|maestro
    //  }
    //}
  }

  // PayPal event callback

  /**
   * A successful completion of the PayPal flow by your customer will result in
   * certain information being returned to you, depending on the client
   * options you have set.
   *
   * @param response
   *
   * @see https://developers.braintreepayments.com/javascript+php/guides/paypal/client-side
   */
  Drupal.braintree.prototype.onPaymentMethodReceived = function (obj) {
    var self = this;

    this.$submit.removeAttr('disabled');

    var $nonce_el = $('input[name=payment_method_nonce]');
    if ($nonce_el.length > 1) {
      var nonce = $($nonce_el[0]).val();
      $nonce_el[1].value = nonce;
    }

    // Bind cancel button to restore PayPal form.
    $('#bt-pp-cancel').one('click', function () {
      self.$submit.attr('disabled', 'disabled');
    });
  }

  /**
   * Braintree integration teardown callback.
   */
  Drupal.braintree.prototype.teardown = function () {
    delete Drupal.myBraintreeIntegration;

    // Reset submit button.
    this.$submit.removeAttr('disabled');

    // Remove device_data if added
    var form = document.getElementById(this.formId);
    var deviceDataInput = form['device_data'];
    if (typeof deviceDataInput !== 'undefined') {
      form.removeChild(deviceDataInput);
    }
  }

})(jQuery);
