var http = require('http');
var nano = require('nano')('http://visualizer.epfl.ch');
var dbName = "bioreactor2";  // must be CREATED BEFORE and contains correct VIEW !!!!

var epochQuery=

function getCouchDBLink() {
  try {
    var couchdb=nano.db.use(dbName);
    return couchdb;
  } catch (err) {
    console.log(err.description)
    return false;
  }
}

function getLastEntryID(host, callback) {
  var bioreactor =getCouchDBLink();
  try {
      bioreactor.view('logDisplay', 'stats', {key:host}, function(err, body) {
        if (!err) {
          var lastID=-1;
          if (body.rows && body.rows[0] && body.rows[0].value && body.rows[0].value.max) {
            lastID=body.rows[0].value.max;
          }
          callback(host, lastID);
        } else {
              console.log(err.description);
          if (err && err.description) {
            if (err.description=="no_db_file") {
              console.log("ERROR: the database does not exists");
            } else  if (err.description=="missing") {
              console.log("ERROR: the view document stats is missing");
             } else  if (err.description=="XXXXXX") {
              console.log("ERROR: can not getLastEntryID: Assuming 0");
              callback(host, -1);
            } else {
              console.log("ERROR: "+err.description);
            }
          }
        }
      });
    } catch (err) {
      console.log("ERROR: "+err)
    }
}


function saveToCouchDB(entry) {
  var bioreactor =getCouchDBLink();
  bioreactor.insert( entry, function(err, body, header) {

    if (err) throw console.log(err);
    // console.log(header);
  });
}


