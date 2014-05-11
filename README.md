Dock monitor system 
=======

This project includes four sub projects.

1. API - Vessel API to get info about vessels
2. Pi-Node - Node server that takes picture of vessel and stores the data to database
3. Pi-C - Manages communication with arduino via XBee
4. Arduino - Handles sensor logic and communicates with PI via XBee
5. Web - Interface to check out dock activity. Also has an api to get the dock activity.


## Installing Project

```
$ git clone git://github.com/aaronsmooth/ShipSensing.git
```

## Building Project


###Pi-Node

#####Install and setup Node
```
// Update package manager
$ sudo apt-get upgrade && sudo apt-get update

// Install Node
$ wget http://nodejs.org/dist/v0.10.2/node-v0.10.2-linux-arm-pi.tar.gz
$ tar -xvzf node-v0.10.2-linux-arm-pi.tar.gz

// Set NODE_JS_HOME variable to the directory where you un-tarred Node, and 
// add the bin dir to your PATH
$ NODE_JS_HOME=/home/pi/node-v0.10.2-linux-arm-pi 
PATH=$PATH:$NODE_JS_HOME/bin
```

#####Running Node server
```
// Go to "pi-node" directory of the project
$ npm install

// Wait for all dependencies to be installed and then run the server
$ node server.js
```
