var http = require('http');

var options = {
  host: '172.17.0.105',
  port: 80,
  path: '/s'
};


tmp = []  

var req = http.request(options, function(res) {
  res.setEncoding('utf8');
  res.on('data', function(chunk) {
  tmp.push(chunk)
  });
  res.on('end', function (e) {
  var body = tmp.join('');
  console.log(body);
  });
});

req.on('error', function(e) {
  console.log('problem with request: ' + e.message);
});

// write data to request body
req.write('data\n');
req.end();

console.log(req.e.message);

callback = function(response) {
  var str = '';

  //another chunk of data has been recieved, so append it to `str`
  response.on('data', function (chunk) {
    str += chunk;
  });

  //the whole response has been recieved, so we just print it out here
  response.on('end', function () {
    console.log(str);
  });
}

http.request(options, callback).end();

