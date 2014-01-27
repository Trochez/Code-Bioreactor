var http = require('http');
var nano = require('nano')('http://localhost:5984');


var arduinoIPs=["172.17.0.105"]

setInterval(function() {
  for (var i=0; i<arduinoIPs.length; i++) {
    var ip=arduinoIPs[i];

    // we need to search for the last entryID for this specific IP address
    getLastEntryID(ip, function(ip, lastID) {
      fetchLog(ip, lastID+1);
    })
  }

},5000)






function fetchLog(host, lastID) {
  var tmp = [];
  var options={
    host: host,
    path: "/m"+lastID
  };

console.log(options);

  var req = http.request(options, function(res) {
    res.setEncoding('utf8');
    res.on('data', function(chunk) {
      tmp.push(chunk);
    });
    res.on('end', function (e) {
       var body = tmp.join('');
       parseResult(body, options);
    });
  });

  req.on('error', function(e) {
    console.log('problem with request: ' + e.message);
  });

  // write data to request body
  req.end();
}


function parseResult(result, options) {
  var lines=result.split(/[\r\n]+/);
  for (var i=0; i<lines.length; i++) {
    var line=lines[i];
    var entry={};
    if (line.length>0) {
      if (line.length!=134) {
          console.log("Not correct length: "+line);
      } else {
        if (checkDigit(line)) {
          entry.id=parseInt("0x"+line.substring(0,8));
          entry.epoch=parseInt("0x"+line.substring(8,16));
          for (var j=0; j<26; j++) {
            entry[String.fromCharCode(65+j)]=convertSignedIntHexa(line.substring(16+(j*4),20+(j*4)));
          }
          entry.event=parseInt("0x"+line.substring(120,124));
          entry.eventParameter=parseInt("0x"+line.substring(124,128));
          entry.mac=line.substring(128,132);
          entry.host=options.host;
          saveToCouchDB(entry);
        } else {
          console.log("Check digit error: "+line);
        }
      }
    }
  }

  function checkDigit(line) {
    var checkDigit=0;
    for (var i=0; i<line.length; i=i+2) {
      checkDigit^=parseInt("0x"+line[i]+line[i+1]);
    }
    if (checkDigit==0) return true;
    return false;
  }

  function convertSignedIntHexa(hexa) {
    var value=parseInt("0x"+hexa);
    if (value>32767) {
      return (65536-value)*-1;
    } 
    return value;
  }
}

function getCouchDBLink() {
  nano.db.create('bioreactor');
  return nano.db.use('bioreactor');   
}

function getLastEntryID(host, callback) {
  var bioreactor =getCouchDBLink();    
  bioreactor.view('logDisplay', 'stats', {key:"172.17.0.105"}, function(err, body) {
    if (!err) {
      callback(host, body.rows[0].value.max);
    } else {
      console.log("ERROR: can not getLastEntryID from: "+host);
    }
  });
}


function saveToCouchDB(entry) {
  var bioreactor =getCouchDBLink();     
  bioreactor.insert( entry, function(err, body, header) {
    if (err) throw console.log(err);
    // console.log(header);
  });
}


