<?php
    // This file should be copied into the Document Root directory
    // for Apache Tomcat (aka httpd).  This is the directory
    // /var/www/html

    ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);

    function doGET() {
        $initialHTML = file_get_contents("MatchesWeb_JS_WebServer.html");

        if (isset($_SERVER['SERVER_ADDR'])) {
            // Get the LAN ip address from the server
            $host = $_SERVER['SERVER_ADDR'];
        } else {
            // This does not always work to get the LAN ip address 
            // like 192.168.1.101. It may return a loopback address
            // like 127.0.0.1
            $host = gethostbyname(gethostname());
        }

        $port = 80; // Standard port for web server
        
        // Substitute the marker $ServerURL with real value, e.g:
        //    http://192.168.1.64:80/MatchesWeb.php
        // Notice that the single-quotes for the search-string prevents
        // interpreting $ServerURL as a variable name
        $initialHTML = str_replace('$ServerURL', "http://$host:$port/MatchesWeb.php", $initialHTML);
        echo $initialHTML;
    }


    function doPOST() {
        // Get the raw POST data
        $json_data = file_get_contents('php://input');

        // Decode the JSON data into an associative array
        $data = json_decode($json_data, true);

        $display = "";
        $taken = $data["taken"];
        $matchesRemaining = $data["matchesRemaining"];

        // Below is the game logic
        if ($matchesRemaining < $taken) {
            $display = "Sorry, cannot take $taken from $matchesRemaining!\n";
            // matchesRemaining is unchanged
        } else if ($matchesRemaining == $taken) {
            // User won.
            $display = "Congratulations! You win!!\n";
            $matchesRemaining = 0;
        } else {
            // Decrement matches remaining.
            $matchesRemaining -= $taken;
            // User play is remainder modulo 5 if possible.
            $newTaken = $matchesRemaining % 5;
            if ($newTaken == 0) {
                // User made winning move.  Take lowest possible, hoping for user mistake!
                $newTaken = 1;
            }

            $matchesRemaining -= $newTaken;
            $display = "You took $taken.  I take $newTaken leaving $matchesRemaining.\n";
            if ($matchesRemaining == 0) {
                    // Server won.
                    $display .= "I win!\n";
            }
        }

        $response = array("display"=>"$display", "matchesRemaining"=>$matchesRemaining);
        echo json_encode($response);
    }

    if ($_SERVER['REQUEST_METHOD'] === 'GET') {
        doGET();
    } else {
        doPOST();
    }
?>

