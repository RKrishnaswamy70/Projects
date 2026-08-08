							RPiController ReadMe

Synopsis
========
RPiController is an example of controlling the Raspberry Pi from a web browser.

Architecture
============

There will be two processes:
    - A Server Process.  This is Python process running the program 
      RPiControllerServer.py.  This will run on your Raspberry Pi
    - A Client Process.  This is your web browser.  You should run
      this on your laptop/desktop.
	
How to run it
=============

    1. First make sure your Raspberry Pi is connected to WiFi, and is
       on the same network as your laptop/desktop.
    1. Now open a terminal window and cd to the RPiController directory.
    2. Now startup the Server Process from the terminal window.
       Enter the command:
          python3 RPiControllerServer.py
       The process will startup and print a line specifying a URL.
       This is the URL that your browser on your laptop/desktop will
       connect to.
    3. Now startup the Client Process on your laptop, a web browser.
       Enter that URL.
	
The displayed web page will on your laptop will be in communcation
with the Server Process on your Raspberry Pi.

You can now press the buttons to turn on or off the LEDs.

Press Shutdown to shut the server down.

Files
=====
    - RPiController.html: The html file showing the buttons.  It uses
	  html and JavaScript.
	- RPiControllerLedSupport.py: Modify this file to turn LEDs on or off.
	  You will need to implement the functions turnOn(led) and turnOff(led).
	- RPiControllerServer.py: This is the main RPiControllerServer program.
	  It imports RPiControllerLedSupport to control the LEDs.  It also
	  imports WebServerSupport which implements the basic communication
	  mechanism with the browser.
	- WebServerSupport.py: This is the python program to implement basic
	  communication with the web server.

The actual technology that is used in the python programs and the html is
something you will study later as you study more in Computer Science.

For now, you can just use it to implement your part, the electronic control
of the LEDs through the Raspberry Pi.

Enjoy!





	
