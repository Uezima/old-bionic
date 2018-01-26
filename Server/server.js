#!/bin/env node

require('./webservice/config/db-library-model.js')();

var app = require('./webservice/config/express.js')();

var port = 8080;

app.listen(port, function () {
    console.log('Servidor ligado na porta: ' + port);
});