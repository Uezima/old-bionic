#!/bin/env node

module.exports = function () {

    var accountSid = process.env.TWILIO_ACCOUNT_SID;
    var authToken = process.env.TWILIO_AUTH_TOKEN;

    //Twilio
    twilio = require('twilio')(accountSid, authToken);

    path = require('path');

}
