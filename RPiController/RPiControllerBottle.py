# This is the python program to run as a RPiController Server using Bottle.
# Run it as: python RPiControllerBottle.py
# It will tell you the URL to connect to on a client machine (could be
# the same machine as the server machine).
#
# If the client machine is not the server machine, you will need to enable
# the server machine port for queries.
#   1. On the server machine, as super-user, do the command:
#         netstat -a | grep :8080
#      There should be no output.  The port 8080 should be unused.
#   2. Then do
#         firewall-cmd --add-port=8080/tcp
#   3. Later you will need to undo the port addition:
#         firewall-cmd --remove-port=8080/tcp
#
# To see the html text that is produced, see the data file RPiController.html.
# The $HostAndPort marker in RPiController.html will be replaced by the actual
# ip-address of the server.

from Support import bottle

import os
import re
import json
import socket

import RPiControllerLedSupport

PORT = 8080

# We would prefer to do this, but it does not always work:
#   HOST = socket.gethostbyname(socket.gethostname())
# It sometimes returns a loopback address like 127.0.0.1
# instead of a LAN address like 192.168.1.174. So we do
# this more complext way using SOC_DGRAM
def getLANIpAddr():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8",80))
    lanIpAddr = s.getsockname()[0]
    s.close()
    return lanIpAddr

HOST = getLANIpAddr()

app = bottle.Bottle()

# Example of how to define a route that passes a parameter
# @app.get('/user/<user_id>')
# def get_user(user_id):
@app.get('/RPiController.bottle')
def initialHTML():
    # Return the entire file
    filePath = 'RPiController.html'

    try:
        # The with-statement will automatically
        # close the file even if errors occur.
        with open(filePath, 'r') as file:
            fileContents = file.read()
        
        # Substitute the marker $ServerURL with real value, e.g:
        #    http://192.168.1.64:8080/RPiController.bottle
        # Notice that the $ in $ServerURL is escaped since $ is a special
        # character for regexps.
        htmlText = re.sub(r"\$ServerURL", f"http://{HOST}:{PORT}/RPiController.bottle", fileContents)
        return htmlText
    except Exception as e:
        errorMsg = f"An error occurred: {e}"
        return json.dumps({"error": errorMsg}), 404


# This global state indicates whether the four leds are on or off.
# Initially all four are off.
globalLedIsOn = {"A":False, "B":False, "C":False, "D":False}

@app.post('/RPiController.bottle')
def doPost():
    # The request.get_json call returns the request data
    # as a python list.  Documentation says it comes in
    # as a python dictionary, but I cannot seem to access
    # the fields using data.led.  I have to use data["led"].
    requestParams = bottle.request.json
    print(requestParams)

    if (requestParams['shutdown'] == True) :
        print('Goodbye from RPiControllerBottle!')
        os._exit(0)

    # Not a shutdown message.  Extract value of 'led'.
    ledParam = requestParams['led']
    responseParams = {}
    if (globalLedIsOn[ledParam]):
        # Turn off the led, change the state, and set the responseText
        RPiControllerLedSupport.turnOff(ledParam)
        globalLedIsOn[ledParam] = False
        responseParams['screen'] = f"LED-{ledParam} has been turned off."
    else:
        # Turn on the led, change the state, and set the responseText
        RPiControllerLedSupport.turnOn(ledParam)
        globalLedIsOn[ledParam] = True
        responseParams['screen'] = f"LED-{ledParam} has been turned on."

    response = json.dumps(responseParams)
    return response


if __name__ == '__main__':
    # Run the server
    print(f"Connect Web Server to http://{HOST}:{PORT}/RPiController.bottle")
    app.run(host=HOST, port=PORT, debug=True)

