#!/bin/env node

module.exports = function (app) {

    app.post('/config', function (req, res) {
        res.setHeader('content-type', 'text/xml');
        res.sendFile(path.join(__dirname, '../../static', 'voice.xml'));
    });

    app.get('/call', function (req, res) {
        twilio.calls.create({
            //url: 'YOUR SERVER URL + /config', 
            //to: 'PHONE',
            //from: 'PHONE'
        }, function (err, call) {
            if (err) {
                res.json({
                    'err': err
                });
            } else {
                res.json({
                    'err': null
                });
            }
        });
    });

    app.get('/sms', function (req, res) {
        twilio.messages.create({
            //to: 'PHONE',
            //from: 'PHONE'
            body: "EMERGENCIA! A PULSEIRA OLD BIONIC FOI ATIVADA! ENVIE SOS PARA CONFIRMAR A LEITURA DESTE AVISO!"
        }, function (err, call) {
            if (err) {
                res.json({
                    'err': err
                });
            } else {
                res.json({
                    'err': null
                });
            }
        });
    });
}