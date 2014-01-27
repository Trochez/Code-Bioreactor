var http = require('http');
var nano = require('nano')('http://localhost:5984');


var bioreactor=nano.db.use('bioreactor');

bioreactor.view('logDisplay', 'stats', {key:"172.17.0.105"}, function(err, body) {
  if (!err) {
    console.log(body.rows[0].value);
  }
});