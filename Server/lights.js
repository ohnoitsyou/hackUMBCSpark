var express = require('express')
  , http = require('http')
  , app = express()
  , server = http.createServer(app)
  , spark = require('spark')
  , q = require('q')
  , config = require('config')
  , knownDevices = {}
  ; 

server.listen(3000);

// At the moment most commands just send a success or failure back to
//   the client.
// I want to expand it to have more of a UI.
// However, that's work for another version.


// Login to spark cloud
spark.login({accessToken: config.get('accessToken')}).then(
  function(token) {
    console.log('Login success');
  },
  function(error) {
    console.log('Login failure');
  }
);

app.get('/', function(req, res) {
  res.send('home');
});

app.get('/:device', function(req, res) {
  var device = req.params.device;
  if(device != 'favicon.ico') {
    isKnownDevice(device)
      .then(function(result) {
          console.log('Final result',result);
          res.send('Command Success');
        },function(result) {
          console.log('Final result',result);
          res.send('Command Failure');
      })
      .catch(function(error) {
        console.log('Caught error:',error);
        res.send('Unhandled error:',error);
      });
  } else {
    console.log('Ignoring device:', device);
  }
});

app.get('/:device/off', function(req, res) {
var device = req.params.device;
  if(device != 'favicon.ico') {
    isKnownDevice(device)
      .then(getDeviceRole)
      .then(function(role) {
        return verifyRole(role, 'lights');
      }).then(function() {
        return sendCommand(device, 'setRoutine',0);
      }).then(function(result) {
          console.log('Final result',result);
          res.send('Command Success');
        },function(result) {
          console.log('Final result',result);
          res.send('Command Failure');
      })  
      .catch(function(error) {
        console.log('Caught error:',error);
        res.send('Unhandled error:',error);
      }); 
  } else {
    console.log('Ignoring device:', device);
  }
});

app.get('/:device/r/:routine', function(req, res) {
  var device = req.params.device;
  var routine = req.params.routine;
  if(device != 'favicon.ico') {
    isKnownDevice(device)
      .then(getDeviceRole)
      .then(function(role) {
        return verifyRole(role, 'lights');
      }).then(function() {
        return sendCommand(device, 'setRoutine',routine);
      }).then(function(result) {
          console.log('Final result',result);
          res.send('Command Success');
        },function(result) {
          console.log('Final result',result);
          res.send('Command Failure');
      })  
      .catch(function(error) {
        console.log('Caught error:',error);
        res.send('Unhandled error:',error);
      }); 
  } else {
    console.log('Ignoring device:', device);
  }
});

app.get('/:device/c/:r/:g/:b',function(req, res) {
  var device = req.params.device;
  // I should add some verification of nubmers here.
  var rgbColor = req.params.r + ':' + req.params.g + ':' + req.params.b;
  // I really don't know why this request is coming through.
  // I'm just excluding it for now.
  if(device != 'favicon.ico') {
    isKnownDevice(device)
      .then(getDeviceRole)
      .then(function(role) {
        return verifyRole(role, 'lights');
      }).then(function() {
        return sendCommand(device, 'setColor',rgbColor);
      }).then(function(result) {
          console.log('Final result',result);
          res.send('Command Success');
        },function(result) {
          console.log('Final result',result);
          res.send('Command Failure');
      })  
      .catch(function(error) {
        console.log('Caught error:',error);
        res.send('Unhandled error:',error);
      }); 
  } else {
    console.log('Ignoring device:', device);
  }
});


// Define helper functions

/**
 *  getDevices
 *  This method gets a list of known devices from the Spark Cloud
 *    and caches their information so that we don't have to keep
 *    going back out and getting it.
 *  Only devices that are 'online' are saved
 */
function getDevices() {
  spark.listDevices().then(
    function(devices) {
      devices.forEach(function(device) {
        if(device.connected) {
          knownDevices[device.name] = device;
          console.log('found connected device:', device.name);
        }
      });
    },
    function(result) {
      console.log('getDevices: failure:', result);
    }
  );
}

/**
 * isKnownDevice
 * This method takes a device name, like one passed in from the URL
 *   and checks to see if we have it in the knownDevices list
 * If it is known, then we can call functions on it. 
 */
function isKnownDevice(deviceName) {
  var deferred = q.defer();  
  if(knownDevices[deviceName]) {
    console.log('I know about this device');
    deferred.resolve(deviceName);
  } else {
    console.log('I don\'t know about this device');
    deferred.reject('Unknown device');
  }
  return deferred.promise;
}

/**
 * getDeviceRole
 * This function retrieves the role that the spark thinks it is.
 * At the moment, this is set in firmware, and can't be changed
 *   at runtime. I don't see a need to be able to.
 */
function getDeviceRole(deviceName) {
  var deferred = q.defer();
  knownDevices[deviceName].getVariable('role', function(err, data) {
    if(err) {
      console.log('Error getting role', err);
      deferred.reject('Error getting role');
    } else {
      console.log('Success getting role');
      deferred.resolve(data.result);
    }
  });
  return deferred.promise;
}

/**
 * verifyRole
 * This function just compares the role you got back from getDeviceRole
 *   against (at the moment) a role provided
 * I plan to expand this function so that 'role' is an array. i.e. one
 *   server method can be accepted by multiple 'classes' of core.
 */

function verifyRole(deviceRole, role) {
  var deferred = q.defer();
  if(deviceRole == role) {
    console.log('Role verified');
    deferred.resolve('true');
  } else { 
    console.log('Role not verified');
    deferred.reject('false');
  }
  return deferred.promise;
}

/**
 * sendCommand
 * This function does the hard work of actually sending the command to
 *   the core.
 */
function sendCommand(deviceName, command, args) {
  var deferred = q.defer();
  knownDevices[deviceName].callFunction(command, args, function(err, data) {
    if(err) {
      console.log('Command failure');
      deferred.reject('Command failure');
    } else {
      console.log('Command success');
      deferred.resolve('Command success');
    }
  });
  return deferred.promise;
}

// Do the initial retrival of devices and set a refresh every 100 seconds
// This could probably be longer, you're not gonna be adding and removing
//   devices all the time but they might come and go as far as
//   connectedness goes
getDevices();
setInterval(getDevices,100000);
