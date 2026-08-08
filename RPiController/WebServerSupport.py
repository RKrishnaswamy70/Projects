# Import libraries
import re
import time
import socket
import http.server
import socketserver
import threading
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


# Create a request-handler subclass.
class RequestHandler(http.server.SimpleHTTPRequestHandler):
    # This serves up the initial HTML file.
    def do_GET(self):
        # One way would be to do this:
        #   self.path = self.webServer.initialHTML()
        #   return http.server.SimpleHTTPRequestHandler.do_GET(self)
        # But that does not give the opportunity to substitute the
        # server-url string in the html with the true IPv4 address.
        #
        # So we do this more complicated thing.

        # Need to call initialHTML() from WebServer subclass.
        filePath = "./" + self.webServer.initialHTML()

        # The with-statement will automatically
        # close the file even if errors occur.
        with open(filePath, 'r') as file:
            fileContents = file.read()
        
        # Substitute the marker $ServerURL with real value, e.g:
        #    http://192.168.1.64:8080/DontCare
        # Notice that the $ in $ServerURL is escaped since $ is a special
        # character for regexps.
        htmlText = re.sub(r"\$ServerURL", f"http://{HOST}:{PORT}/DontCare", fileContents)

        self.send_response(200, "Success")
        self.send_header("Content-type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(htmlText.encode('utf-8'))
        
        # Should not do this.  An AI query seemed to recommend it,
        # but it does not seem necessary.
        # return http.server.SimpleHTTPRequestHandler.do_GET(self)
    # End of do_GET()


    # This responds to the XMLHTTPRequest POST message.
    def do_POST(self):
        # 1. Get the Content-Length header
        contentLength = int(self.headers['Content-Length'])
        
        # 2. Read from rfile
        requestParamsBytes = self.rfile.read(contentLength)
        
        # 3. Decode to Json string
        requestParamsJsonStr = requestParamsBytes.decode('utf-8')

        # 4. Convert it to a dictionary
        requestParams = json.loads(requestParamsJsonStr)

        # Call to the virtual webServer.processClientRequest() to process
        # the client request
        responseParams = {}
        self.webServer.processClientRequest(requestParams, responseParams)

        # Convert responseParams into a string
        responseParamsJsonStr = json.dumps(responseParams)

        self.send_response(200, "Success")
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(responseParamsJsonStr.encode('utf-8'))
    # End of do_POST()

# End of class RequestHandler


# This is the main class.
class WebServer:
    def __init__(self):
        # Defining PORT number.  Does not really need to be a member of this
        # class, but may be useful later on.
        self.PORT = PORT

        # Creating handler, and save this WebServer as an instance
        # member.  This is needed so that RequestHandler.do_POST()
        # can call WebServer.processClientRequest(). 
        self.handler = RequestHandler
        self.handler.webServer = self

        # This classwide setting in socketserver.TCPServer avoids
        # Errno 98 about address reuse when you restart the server
        # Set allow_reuse_address on the class
        socketserver.TCPServer.allow_reuse_address = True

        # The TCPServer member is theServer
        # The empty parameter allows it to connect at all available IPv4 interfaces.
        # Other values that would work are:
        #   - HOST (which is the IP address of the server)
        #   - "localhost" - only when browser and server are on the same computer.
        self.theServer = socketserver.TCPServer(("", self.PORT), self.handler)

        # Print connect message
        print(f"Connect browser to URL http://{HOST}:{PORT}/")
    # End of __init__()


    # Simply call theServer.serve_forever()
    def run(self):
        try:
            self.theServer.serve_forever()
        except KeyboardInterrupt:
            pass

        self.theServer.server_close()
    # End of run()


    # Virtual function to be overridden in a subclass.  Gets
    # the initial HTML file.  The overriding function can
    # return something like '/Foo.html'
    def initialHTML(self):
        # This default will return the directory in which
        # the server runs.
        return '/'


    # Virtual function to be overridden in subclass.  Client
    # sent requestParams.  Server responds with responseParams
    # This is the default implementation.
    def processClientRequest(self, requestParams, responseParams):
        # requestParams and responseParams are both dictionaries.
        # Default behaviour is to just copy the requestParams
        # into the responseParams.
        responseParams = requestParams

        #  Print the requestParams
        print("Request params", requestParams)

        #  Print the responseParams
        print("Response params", responseParams)
    # End of processClientRequest()


    # To stop the server.
    def delayedStop(self, message):
        # This is the shutdown message.  We need an asynchronous thread
        # to kill the server, that should wait some time so that the
        # server can do what it is currently doing.  So we give it a
        # few seconds.  The AsyncKillServer is a function that waits
        # and then shuts it down.  That allows the current thread in the
        # server to complete its response.
        def AsyncKillServer():
            # Delay, then shutdown.
            time.sleep(1);
            # The 'theServer' variable is a socketserver.TCPServer
            # and is initialized in __init__().  Note that Python does
            # not require a forward declaration to resolve the name statically.
            # The name must be defined when it is called.
            self.theServer.shutdown()

        print(message)
        thrd = threading.Thread(target=AsyncKillServer, args=())
        thrd.start()
    # End of stop()
        
#End of class WebServer




