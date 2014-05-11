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
======
## Using API's

###Vessel API
To lookup a vessel you must create a search field that passes a fully-qualified URL content request to the Vessel API.

#####Query Parameters
* key - Search Key. [mmsi, imo, id, name]
* val - Value of the key.
* format - Format of the result. [json, xml]

Examples of fully-qualified URLs:

1. http://vesselapi.parseapp.com/lookup?key=name&val=TUGELA&format=json
2. http://vesselapi.parseapp.com/lookup?key=name&val=WORLD%20SPIRIT&format=xml
3. http://vesselapi.parseapp.com/lookup?key=mmsi&val=636090647&format=json
4. http://vesselapi.parseapp.com/lookup?key=imo&val=9187863&format=xml

```
$ curl --request GET "http://vesselapi.parseapp.com/lookup?key=name&val=TUGELA&format=json"
```

###Activity API
To search for activites you must create a search field that passes a fully-qualified URL content request to the web API.

#####Query Parameters
* limit - Limit of the search results. Default is 10. Set limit to "none" to get all results. [1 to whatever number you want]
* format - Format of the result. [json, xml]
* mmsi, imo, name and id can be used to filter results. 

Examples of fully-qualified URLs:

1. http://dockmonitor.parseapp.com/activity?limit=2&format=json
2. http://dockmonitor.parseapp.com/activity?limit=none&format=json
3. http://dockmonitor.parseapp.com/activity?mmsi=636090647&limit=2&format=xml

```
$ curl --request GET "http://dockmonitor.parseapp.com/activity?limit=none&format=xml"
```
======

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
$ PATH=$PATH:$NODE_JS_HOME/bin
```

#####Running Node server
```
// Go to "pi-node" directory of the project
$ npm install

// Wait for all dependencies to be installed and then run the server
$ node server.js
```
