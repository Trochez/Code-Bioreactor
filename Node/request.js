var http = require('http');

var options = {
  host: '172.17.0.105',
  port: 80,
  path: '/m10'
};


tmp = []  

var req = http.request(options, function(res) {
  res.setEncoding('utf8');
    res.on('data', function(chunk) {
      console.log(chunk);
      tmp.push(chunk)
    });
    res.on('end', function (e) {
    var body = tmp.join('');
    // console.log(body);
  });
});

req.on('error', function(e) {
  console.log('problem with request: ' + e.message);
});

// write data to request body
req.write('data\n');
req.end();
