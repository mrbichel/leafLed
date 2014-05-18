LeafLED
=====================================
Software to control LPD8806 based adressable LED Strips over network from a syphon video input.
The application handles simple mapping as well.

Developed by [Johan Bichel Lindegaard](http://johan.cc) for [Obscura](http://obscura.dk) 


Dependencies
------------
ofxOsc
ofxXmlSettings
ofxUI
ofxSyphon
[ofxLEDs](https://github.com/fishkingsin/ofxLEDs)
[ofxLPD8806](https://github.com/fishkingsin/ofxLPD8806)

Getting started
---------------
You can download the release client and server from releases. 

Follow this guide for installing openFrameworks on the pi http://www.openframeworks.cc/setup/raspberrypi/Raspberry-Pi-Getting-Started.html

As default the clients are listening to commands from the hostname leafMaster.local. Set your hostname to this. 


Upgrading the client
--------------------
You can use fabric and the included fabfile to perform operations on a large number of clients.
Open up the file and add in hostnames for your clients in the two groups named leader and all.
Make sure fabric is installed.

### Compiling the client
If you have downloaded the client binary you don't need to compile, just put it in 'ledClient/bin/data' and then run deploy.
Compile will copy new src to the client compile it and then copy the binary back to 'ledClient/bin/data'.

    fab compile

### Deploying to all units
	
	fab deploy_all

### Deploying to a specific unit
	
	fab deploy --host [USER]@[HOSTNAME].local


Hardware
--------
The client units are build around a raspberry pi and a network switch.


Licence
-------
The code in this repository is available under the [MIT License](https://secure.wikimedia.org/wikipedia/en/wiki/Mit_license).

Copyright (c) 2013-2014 [Johan Bichel Lindegaard](http://johan.cc)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.