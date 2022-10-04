# Python3.7+
#
# Start this up with command line:
#   python3.6 MatchesServer.py Matches.html
# The program will tell you what web site to connect to
# by an output line like this:
#   Open browser to web page: http://192.168.1.101:8080/local
# This is assuming that the ip-address of this machine is
# 192.168.1.101.
#
# Incidentally, you can get the computer IPV4 address by typing:
#    $hostname -I
# You can also get it as this script does by:
#    $python3.6
#    >>> import socket
#    >>> socket.gethostbyname(socket.gethostname())
#    >>> <ctrl-d>
#    $

import re
import sys
import socket
import json

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

shutdown = False

# Main program

# Check command line arguments
# if (len(sys.argv) != 2):
#    print ("Command line is: python3.6 <path-of-MatchesServer.py> <path-of-Matches.html>")
#    exit()

# This serves up the initial HTML file.
def do_GET():
    filePath = "MatchesWeb_JS_WebServer.html"

    # The with-statement will automatically
    # close the file even if errors occur.
    with open(filePath, 'r') as file:
        fileContents = file.read()
        
    # Substitute the marker $ServerURL with real value, e.g:
    #    http://192.168.1.64:8080/DontCare
    # Notice that the $ in $ServerURL is escaped since $ is a special
    # character for regexps.
    htmlText = re.sub(r"\$ServerURL", f"http://{HOST}:{PORT}/DontCare", fileContents)

    # Prepend the Content header:
    # HTTP/1.1 200 OK
    htmlText = "HTTP/1.1 200 OK\n" + htmlText

    return htmlText
# End of do_GET()


# Function to actually compute the response
def processClientRequest(requestParams, responseParams):
    matchesRemaining = requestParams['matchesRemaining']
    taken = requestParams['taken']

    matchesRemaining = int(matchesRemaining)
    taken = int(taken)

    # Below is the game logic
    if (matchesRemaining < taken):
        display = f"Sorry, cannot take {taken} from {matchesRemaining}.\n"
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

    responseParams["matchesRemaining"] = matchesRemaining
    responseParams["display"] = display

    return
# End of processClientRequest()


def do_POST(requestStr):
    # The json part of the request string is after the "{"
    idx = requestStr.find("{")
    if (idx < 0):
        idx = requestStr.find("[")
    requestStr = requestStr[idx:]
    print(requestStr)

    # Convert reqeustStr to a dictionary
    requestParams = json.loads(requestStr)

    # This function has the server logic
    responseParams = {}
    processClientRequest(requestParams, responseParams)

    # Convert responseStr to a dictionary
    responseStr = json.dumps(responseParams)

    return responseStr
# End of do_POST()


# Main program
temp = {"matchesRemaining" : 22, "display" : "Yankee Doodle went to town!"}
listenSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
listenSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listenSocket.bind((HOST, PORT))
listenSocket.listen(5)

print(f"Open browser to web page: http://{HOST}:{PORT}/local")
while True:
    clientConnection, clientAddress = listenSocket.accept()
    requestData = clientConnection.recv(1024)
    requestStr = requestData.decode("utf-8")
    if (requestStr.find("GET") == 0):
        # This is a GET request
        responseStr = do_GET()
    else:
        # This is a POST request
        responseStr = do_POST(requestStr)

    clientConnection.sendall(responseStr.encode("utf-8"))
    clientConnection.close()

listenSocket.close()

