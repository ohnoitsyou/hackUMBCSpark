var express = require('express')
  , hbs = require('express-hbs')
  , http = require('http')
  , app = express()
  , server = http.createServer(app)
  , spark = require('spark')
  ;

var knownDevices = {};

spark.login({accessToken: '<insert your access token here>'}).then(
  function(token) {
    console.log('Login success');
  },
  function(err) {
    console.log('Login failure');
  }
);
server.listen(3000);

app.engine('hbs', hbs.express3({
  partialsDir : __dirname + '/views/partials',
  contentHelperName : 'content',
}));

app.set('view engine', 'hbs');
app.set('views', __dirname + '/views');
app.use(express.static(__dirname + '/static'));

function refreshDevices() {
  var devicesPr = spark.listDevices();
  devicesPr.then(
    function(devices) {
      devices.forEach(function(device) {
        knownDevices[device.attributes.name] = device;
        console.log("Found connected device: " + device.attributes.name);
      });
    },
    function(err) {
      console.log('Device list call failed');
    }
  );
}

refreshDevices();
setInterval(refreshDevices,100000);

app.get('/', function(req,res) {
  var links = "";
  var keys = Object.keys(knownDevices);
  keys.forEach(function(key) {
    links += "<a href=\""+ key + "\">"+key+"</a></ br>";
  });
  res.send(links);
});

app.get('/:device', function(req,res) {
  var device = req.params.device;
  if(knownDevices[device]) {
  var curDev = knownDevices[device];
  }
  res.send('hi');
});

app.get('/:device/setColors/:r/:g/:b', function(req,res) {
  var rgbString = req.params.r + ":" + req.params.g + ":" + req.params.b;
  var device = req.params.device;
  if(knownDevices[device]) {
    var curDev = knownDevices[device];
    curDev.getVariable('role', function(err, data) {
      if(data.result == "lights") {
        if(knownDevices[device]) {
          var curDev = knownDevices[device];
          curDev.callFunction('setColors',rgbString, function(err, data) {
            if(err) {
              console.log('An error occurred', err);
            } else {
              console.log('Command successful', data);
              res.send('Success');
            }
          });
        } else {
          console.log('Unknown endpoint');
          res.send('Unknown endpoint');
        }
      } else {
        console.log('End point not of appropriate type');
        res.send('Endpoint not of appropriate type');
      }
    });
  }
});

/*
app.get('/:device/setRoutine/:routine', function(req,res) {
  var device  = req.params.device;
  var routine = req.params.routine;
  // Is it a known device ?
  if(isKnownDevice(device)) {
    var curDevice = knownDevices[device];
    curDev.getVariable('role', function(err, data) {
      // if this method is appropriate for the endpoint
      if(data.result == "lights") {
        curDev.callFunction("setRoutine", routine, function(err, data) {
          err ? res.send("An error occurred", err) :
                res.send("Command successful", data);
        });
      } else {
        console.log("Endpoint is not of appropriate type");
        res.send("Endpoint is not of appropriate type");
      }
    });
});
*/

app.get('/:device/setRoutine/:routine', function(req, res) {
  var device  = req.params.device;
  var routine = req.params.routine;
  if(isKnownDevice(device)) {
    var curDevice = knownDevices[device];
    var role = getDeviceRole(device);
    if(verifyRole(role, "lights")) {
      var result = sendCommand(device, "setRoutine", routine);
      console.log(result);
      res.send(result);
    } else {
      console.log("Endpoint is not of appropriate type");
      res.send("Endpoint is not of appropriate type");
    }
  } else {
    console.log("Not a valid endpoint");
    res.send("Not a valid endpoint");
  }
});

app.get('/:device/off', function(req,res) {
  var device = req.params.device;
  if(knownDevices[device]) {
    var curDev = knownDevices[device];
    curDev.getVariable('role', function(err, data) {
      if(data.result == "lights") {
        curDev.callFunction('setRoutine','1', function(err, data) {
          if(err) {
            console.log('An error occurred', err);
          } else {
            console.log('command Successful', data);
            res.send('Success');
          }
        });
      } else {
        console.log('Endpoint not of appropriate type');
        res.send('Endpoint not of appropriate type');
      }
    });
  }
});


function isKnownDevice(device) {
  if(knownDevices[device]) {
    return true;
  } else {
    return false;
  }
}

function verifyRole(device, role) {
  console.log(device);
  return true;
}

function getDeviceRole(device) {
  knownDevices[device].getVariable('role', function(err, data) {
    if(err) {
      return err
    } else {
      return data;
    }
  });
}

function sendCommand(device, command, args) {
  knownDevices[device].callFunction(command, args, function(err, data) {
    if(err) {
      return err;
    } else {
       return data;
    }
  });
}
/*
app.get('/setColors/:r/:g/:b', function(req,res) {
  var rgbString = req.params.r + ":" + req.params.g + ":" + req.params.b;
  var devicesPr = spark.listDevices();
  devicesPr.then(
    function(devices) {
      devices[0].callFunction('setColors',rgbString, function(err, data) {
        if(err) {
          console.log('an error occurred:', err);
        } else {
          console.log('function successful: ', data);
        }
      });
    },
    function(err) {
      console.log('list devices call failed: ', err);
    }
  );
  res.send('foo');
});
*/
