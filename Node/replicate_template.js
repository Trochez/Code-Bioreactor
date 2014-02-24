
var http = require('http');
var nano = require('nano')('http://localhost:5984');

nano.db.replicate('http://visualizer.epfl.ch/bioreactortemplate', 'bioreactortemplate',
                  { create_target:true }, function(err, body) {
    if (!err)
      console.log(body);
});