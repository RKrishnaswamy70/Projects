# This is the python program to run as a Matches Server using Bottle.
# Run it as: python MatchesBottle.py
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
# To see the html text that is produced, see the data file MatchesWeb_JS_WebServer.html.
# The $HostAndPort marker in Matches.html will be replaced by the actual
# ip-address of the server.

from Support import bottle

import re
import json
import socket


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
@app.get('/MatchesWeb.bottle')
def initialHTML():
    # Return the entire file
    filePath = 'MatchesWeb_JS_WebServer.html'

    try:
        # The with-statement will automatically
        # close the file even if errors occur.
        with open(filePath, 'r') as file:
            fileContents = file.read()
        
        # Substitute the marker $ServerURL with real value, e.g:
        #    http://192.168.1.64:8080/MatchesWeb.bottle
        # Notice that the $ in $ServerURL is escaped since $ is a special
        # character for regexps.
        htmlText = re.sub(r"\$ServerURL", f"http://{HOST}:{PORT}/MatchesWeb.bottle", fileContents)
        return htmlText
    except Exception as e:
        errorMsg = f"An error occurred: {e}"
        return json.dumps({"error": errorMsg}), 404


@app.post('/MatchesWeb.bottle')
def doPost():
    # The request.get_json call returns the request data
    # as a python list.  Documentation says it comes in
    # as a python dictionary, but I cannot seem to access
    # the fields using data.matchesRemaining.  I have to
    # use data["matchesRemaining"]
    data = bottle.request.json
    print(data)
    matchesRemaining = data["matchesRemaining"]
    taken = data["taken"]

    matchesRemaining = int(matchesRemaining)
    taken = int(taken)

    # Below is the game logic
    if (matchesRemaining < taken):
        display = f"Sorry, cannot take {taken} from {matchesRemaining}\n"
        # matchesRemaining is unchanged
    elif (matchesRemaining == taken):
        # User won.
        display = "Congratulations! You win!!\n"
        matchesRemaining = 0
    else:
        # Decrement matches remaining.
        matchesRemaining -= taken
        # User play is remainder modulo 5 if possible.
        newTaken = matchesRemaining % 5
        if (newTaken == 0):
            # User made winning move.  Take lowest possible, hoping for user mistake!
            newTaken = 1

        matchesRemaining -= newTaken
        display = f"You took {taken}.  I take {newTaken} leaving {matchesRemaining}.\n"
        if (matchesRemaining == 0):
                # Server won.
                display += "I win!\n"

    responseData = {"display" : display, "matchesRemaining" : matchesRemaining}
    # print(responseData)

    response = json.dumps(responseData)
    # print(response)
    return response


if __name__ == '__main__':
    # Run the server
    print(f"Connect Web Server to http://{HOST}:{PORT}/MatchesWeb.bottle")
    app.run(host=HOST, port=PORT, debug=True)

