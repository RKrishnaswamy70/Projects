<?php
    // This file should be copied into the Document Root directory
    // for Apache Tomcat (aka httpd).  This is the directory
    // /var/www/html

    ini_set('log_errors', 'On');
    ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    ini_set('error_log', '/var/log/httpd/php_errors.log');
    error_reporting(E_ALL);


    function debugTrace(string $str) {
        $log = "/var/log/httpd/php_trace.log";
        file_put_contents($log, $str, FILE_APPEND);
    }


    function initialHTML() {
        $htmlText = file_get_contents("FileExplorer.html");

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
        //    http://192.168.1.64:80/FileExplorer.php
        // Notice that the single-quotes for the search-string prevents
        // interpreting $ServerURL as a variable name
        $htmlText = str_replace('$ServerURL', "http://$host:$port/FileExplorer.php", $htmlText);

        // Substitute the server name
        $serverHostName = gethostname();
        $htmlText = str_replace('$ServerHostName',$serverHostName, $htmlText);

        echo $htmlText;
    }


    function getTextFile(string $path) {
	// Check if the file exists before reading
	if (file_exists($path)) {
	    $content = file_get_contents($path);
	} else {
	    echo "Error: File does not exist.";
            return;
	}

        // Replace '<' with '&lt;' and '>' with '&gt;'
        $content = str_replace('<', '&lt;', $content);
        $content = str_replace('>', '&gt;', $content);

        // Get the filename
        $fileName = basename($path);

        // debugTrace("---> getTextFile() is complete \n");

        echo
<<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset='UTF-8'>
        <style>
             body {
                font-family: Arial, Helvetica, sans-serif;
                font-size: 16px;
             }
        </style>
    <title>$fileName</title>
</head>
<body>
<pre>
$content
</pre>
</body>
HTML;
    }


    // This function gets the blob-file (binary large object file).
    // Files such as .pdf and .jpeg are blob-files.
    function getBlobFile(string $path) {
        if (!file_exists($path)) {
            http_response_code(404);
            echo "Error file not found: $path";
            return;
        }

        // 1. Clear any previous output buffers to avoid file corruption
        while (ob_get_level()) {
            ob_end_clean();
        }

        // 2a. Set headers to tell the receiver this is raw PDF binary data
        header('Content-Type: ' . mime_content_type($path));
        header('Content-Length: ' . filesize($path));
        // 2b. Optional but recommended headers for browser caching and stability
        header('Content-Transfer-Encoding: binary');
        header('Accept-Ranges: bytes');
        header('Expires: 0');
        header('Cache-Control: private, must-revalidate, post-check=0, pre-check=0');
        header('Pragma: public');

        // 3a. Set this option
        // Option A: Open directly inline in the browser/viewer
        header('Content-Disposition: inline; filename="' . basename($path) . '"');
    
        // 3b. Don't set this option
        // Option B: Force the browser to download the file directly
        // header('Content-Disposition: attachment; filename="' . basename($path) . '"');

        // 4. Stream the raw binary content of the file
        // Note that with the readfile() function we do not use 'echo'.
        readfile($path);

        // debugTrace("---> getBlobFile() is complete \n");

        // 5. Terminate instantly so no trailing whitespace/html is appended
        exit;
    }


    function downloadFile(string $path) {
        if (!file_exists($path)) {
            http_response_code(404);
            echo "Error file not found: $path";
            return;
        }

        // 1. Clear any previous output buffers to avoid file corruption
        while (ob_get_level()) {
            ob_end_clean();
        }

        // 2. Set headers to tell the receiver this is raw PDF binary data
        header('Content-Type: application/octet-stream');
        header('Content-Disposition: attachment; filename="' . basename($path) . '"');
        header('Content-Length: ' . filesize($path));

        // 3. Stream the raw binary content of the file
        // Note that with the readfile() function we do not use 'echo'.
        readfile($path);

        // 4. Terminate instantly so no trailing whitespace/html is appended
        exit;
    }


    function doGET() {
        $requestURI = $_SERVER['REQUEST_URI'];

        // This is for tracing.  It comes into the file $log.
        // debugTrace("---> 1: $requestURI\n");

        // Separate out the $requestURI into its parts, namely
        // the path (like /FileExplorer.php/home/Family and the parameters
        // if any separated by '?'.
        $uriParts = explode('?', $requestURI, 2);
        $path = $uriParts[0]; 

        // debugTrace("---> 2: $path\n");

        $expectedRoot = '/FileExplorer.php';
        $expectedRootLen = strlen($expectedRoot);

        // The $path will either be the $expectedRoot, or else
        // a path whose prefix is the $expectedRoot.  So chop
        // off the expectedRoot.
        $path = substr($path, $expectedRootLen);
        if ($path === "") {
            // The entire path was the $expectedRoot.  Simply
            // return the initial HTML.
            // debugTrace("---> 3: initialHTML()\n");
            initialHTML();
            return;
        }

        // debugTrace("---> 4: $path\n");

        // Now that $expectedRoot is cut off from $path, the
        // rest is one of:
        //   - "/textfile<filepath>" to display a text file
        //   - "/blobfile<filepath>" to display a blobfile 
        //     such as an image or pdf file.
        //   - "/download<filepath>" to download a file.
        // The keywords "textfile", "blobfile" and "download"
        // are from index 1 (after the leading /) and upto the
        // next / which is at the start of the file or dirpath.
        //
        // $pathStartIndex is the position of the / after index 1.
        // We call:
        //   strpos(subject-str, search-str, offset)
        // on subject-str $path, search-str "/" and offset 1
        $pathStartIndex = strpos($path, "/", 1);
        $keyword = substr($path, 1, $pathStartIndex - 1);
        $path = substr($path, $pathStartIndex);

        // debugTrace("---> 5: $keyword, $path\n");

        // The path is a file.  Use the file as directed by 'keyword'.
        if ($keyword == 'textfile') {
            getTextFile($path);
        } else if ($keyword == 'blobfile') {
            getBlobFile($path);
        } else if ($keyword == 'download') {
            downloadFile($path);
        } else if ($keyword == '') {
            // The incoming path was: /FileExplorer.php/
            // Deliver the inital HTML
            initialHTML();
        } else {
            http_response_code(404);
            echo "Expecting 'textfile' or 'blobfile' or 'download' instead of: $path";
        }
    }


    function doPOST() {
        // Get the raw POST data
        $json_data = file_get_contents('php://input');

        // Decode the JSON data into an associative array
        $request = json_decode($json_data, true);

        $cmd = $request["cmd"];
        $path = $request["path"];

        // Now get contents of the directory indicated by $path
        $err = "";
        $dirs = [];
        $files = [];
        if ($cmd === "getContents") {
            // Get contents of the "path" setting
            try {
                $dirIter = new DirectoryIterator($path);
                foreach ($dirIter as $dirItem) {
                    if ($dirItem->isDot()) {
                        // Skip . and ..
                        continue;
                    }

                    if ($dirItem->isDir()) {
                        $dirs[] = $dirItem->getFilename();
                    } else {
                        $files[] = $dirItem->getFilename();
                    }
                }
            } catch (Exception $ex) {
                $err = $ex->getMessage();
            }
        }

        // Sort the files and directories.
        sort($dirs);
        sort($files);

        // Note the $path must be in quotes.
        $response = array("err"=>"$err", "path"=>"$path", "dirs"=>$dirs, "files"=>$files);
        echo json_encode($response);
    }


    // This is the main program
    if ($_SERVER['REQUEST_METHOD'] === 'GET') {
        doGET();
    } else {
        doPOST();
    }

// There should be no text after the ending tag!  Otherwise
// the getBlobFile() function will append junk at the end of
// the blob (binary large object).
?>
