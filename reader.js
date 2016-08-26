var fs = require('fs');

fs.readFile('main.c', function(err, data) {
  if(err) return;
  console.log(data[0]);
});
