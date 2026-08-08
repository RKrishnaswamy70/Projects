from Support import WebServerSupport


class MatchesServer(WebServerSupport.WebServer):
    def __init__(self):
        # Any member variables could be initialized here.  For instance
        #   self.isOn = {"A":False, "B":False, "C":False, "D":False}

        # Note: base class function must be called with the 'self'
        WebServerSupport.WebServer.__init__(self)
    # End of __init__()


    # Virtual function to be overridden in a subclass.  
    def initialHTML(self):
        return 'MatchesWeb_JS_WebServer.html'
    # End of initialHTML()


    # Virtual function to be overridden in subclass.  Client
    # sent requestParams.  Server responds with responseParams
    def processClientRequest(self, requestParams, responseParams):
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

        # MatchesServer does not support a shutdown-server message.
        # If it did, this is how we could shut it down.
        #
        # if (requestParams['shutdown'] == True) :
        #    # This is the shutdown message.
        #
        #    # Note: base class function must be called with the 'self'
        #    WebServerSupport.WebServer.delayedStop \
        #       (self, 'Goodbye from RPiControllerServer!')
        #    responseParams['screen'] = "Goodbye!"
        #    return

        responseParams["matchesRemaining"] = matchesRemaining
        responseParams["display"] = display

        return
    # End of processClientRequest()

# End of class MatchesServer


# Main program
mainServer = MatchesServer()

mainServer.run()

