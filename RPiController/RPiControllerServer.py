from Support import WebServerSupport
import RPiControllerLedSupport


class RPiControllerServer(WebServerSupport.WebServer):
    def __init__(self):
        # This retains state if the LED is on or off.  Initially all are off.
        self.isOn = {"A":False, "B":False, "C":False, "D":False}

        # Note: base class function must be called with the 'self'
        WebServerSupport.WebServer.__init__(self)
    # End of __init__()


    # Virtual function to be overridden in a subclass.  
    def initialHTML(self):
        return '/RPiController.html'
    # End of initialHTML()


    # Virtual function to be overridden in subclass.  Client
    # sent requestParams.  Server responds with responseParams
    def processClientRequest(self, requestParams, responseParams):
        if (requestParams['shutdown'] == True) :
            # This is the shutdown message.

            # Note: base class function must be called with the 'self'
            WebServerSupport.WebServer.delayedStop \
               (self, 'Goodbye from RPiControllerServer!')
            responseParams['screen'] = "Goodbye!"
            return

        # Not a shutdown message.  Extract value of 'led'.
        ledParam = requestParams['led']
        if (self.isOn[ledParam]):
            # Turn off the led, change the state, and set the responseText
            RPiControllerLedSupport.turnOff(ledParam)
            self.isOn[ledParam] = False
            responseParams['screen'] = f"LED-{ledParam} has been turned off."
        else:
            # Turn on the led, change the state, and set the responseText
            RPiControllerLedSupport.turnOn(ledParam)
            self.isOn[ledParam] = True
            responseParams['screen'] = f"LED-{ledParam} has been turned on."

        return
    # End of processClientRequest()

# End of class RPIControllerServer


# Main program
mainServer = RPiControllerServer()

mainServer.run()

