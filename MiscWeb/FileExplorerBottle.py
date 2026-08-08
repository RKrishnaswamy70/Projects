# This is the python program to run as a File Explorer Server using Bottle.
# Run it as: python FileExplorerBottle.py
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
# To see the html text that is produced, see the data file FileExplorer.html.
# The $ServerURL marker in FileExplorer.html will be replaced by the actual
# ip-address of the server.

from Support import bottle

import sys
import re
import json
import socket

import mimetypes

from pathlib import Path


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
@app.get('/FileExplorer.bottle')
def initialHTML():
    # Return the entire file
    filePath = 'FileExplorer.html'

    try:
        # The with-statement will automatically
        # close the file even if errors occur.
        with open(filePath, 'r') as file:
            fileContents = file.read()
        
        # Substitute the marker $ServerURL with real value, e.g:
        #    http://192.168.1.64:8080/FileExplorer.bottle
        # Notice that the $ in $ServerURL is escaped since $ is a special
        # character for regexps.
        htmlText = re.sub(r"\$ServerURL", f"http://{HOST}:{PORT}/FileExplorer.bottle", fileContents)

        # Substitute the server name
        serverHostName = socket.gethostname()
        htmlText = re.sub(r"\$ServerHostName", serverHostName, htmlText)

        # Initial text is ready
        return htmlText
    except Exception as e:
        errorMsg = f"An error occurred: {e}"
        return json.dumps({"error": errorMsg}), 404


@app.get('/FileExplorer.bottle/textfile<filePath:path>')
def doGetTextFile(filePath):
    # Pick up the filename to put in the title
    fileName = Path(filePath).name

    # Just read the file using vanilla Python.
    with open(filePath, 'r') as file:
        fileContents = file.read()

    # Substitute < and > in fileContents.
    fileContents = re.sub(r"<", "&lt;", fileContents)
    fileContents = re.sub(r">", "&gt;", fileContents)

    # Now wrap it in HTML so that it shows up as vanilla text.
    # Note here we use a formatted string with string-interpolation
    # in a triple-quoted string to inline the HTML.
    #
    # Note that the braces { and } for the <style> tag are replaced
    # by {{ and }} respectively.  This is to make sure they are treated
    # as opening and closing braces, instead of string-interpolation
    # delimiters.
    htmlText = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset='UTF-8'>
    <style>
        body {{
            font-family: Arial, Helvetica, sans-serif;
            font-size: 16px; 
        }}
    </style>
    <title>{fileName}</title>
</head>
<body>
<pre>
{fileContents}
</pre>
</body>
"""
    # Return this wrapped htmlText
    return htmlText


@app.get('/FileExplorer.bottle/blobfile<filePath:path>')
def doGetBlobFile(filePath):
    filePathObj = Path(filePath)
    fileDir = filePathObj.parent
    fileName = filePathObj.name
    fileMimeType, fileEncoding = mimetypes.guess_type(filePath)

    # Even though the opened file is not used in this with-statement
    # it is useful to force the file to be closed.
    with open(filePath, 'rb') as f:
        # The download is false to show the file in the browser.
        response = bottle.static_file(fileName, 
                                      root=fileDir, 
                                      mimetype=fileMimeType, 
                                      download=False)
    # Return this response
    return response


@app.post('/FileExplorer.bottle')
def doPost():
    # The request.get_json call returns the request data
    # as a python list.  Documentation says it comes in
    # as a python dictionary, but I cannot seem to access
    # the fields using requestData.cmd.  I have to use 
    # requestData["cmd"].
    requestData = bottle.request.json
    # print(requestData)
    cmd = requestData["cmd"]
    directoryPath = requestData["path"]

    err = ""
    dirs = []
    files = []
    if (cmd == "getContents"):
        # Iterate through the directory
        try:
           directory = Path(directoryPath)
           for item in directory.iterdir():
               if item.is_file():
                   files.append(item.name)
               else:
                   dirs.append(item.name)
        except FileNotFoundError:
            err = f"Nonexistant path: '{directoryPath}'."
        except PermissionError:
            err = f"Permission error: '{directoryPath}'."
        except NotADirectoryError:
            err = f"Error: '{directoryPath}' is a file, not a directory."
        except OSError as e:
            err = f"General OS error reading: {directoryPath}"
    else:
        err = f"Unknown command: {cmd}"

    # Sort the files and directories.
    dirs.sort();
    files.sort();

    # Build the responseData 
    responseData = {"err" : err, "path" : directoryPath, "dirs" : dirs, "files" : files}
    # print(responseData)

    response = json.dumps(responseData)
    # print(response)
    return response


@app.route('/FileExplorer.bottle/upload', method='POST')
def doUpload():
    # Set response type to JSON for the JavaScript client
    bottle.response.content_type = 'application/json'

    # Retrieve the uploaded file using key 'UploadFile'
    # Use bottle.request.files to get the file.
    fileUpload = bottle.request.files.get('UploadFile')
   
    # Retrieve the target dir using key 'TargetDir'
    # Use bottle.request.forms to get the target dir.
    targetDir = bottle.request.forms.get('TargetDir')
    
    if not fileUpload:
        bottle.response.status = 400
        return {'error': 'No file part in the request.'}
    
    serverPath = targetDir + "/" + fileUpload.filename

    try:
        # Save the file to the designated directory
        fileUpload.save(serverPath, overwrite=True) 
        return {'message': f'"{fileUpload.filename}" upload ok!'}
    except Exception as e:
        bottle.response.status = 500
        return {'error': f'{fileName} upload failed: {str(e)}'}
    

@app.route('/FileExplorer.bottle/download<filePath:path>', method='GET')
def doDownload(filePath):
    filePathObj = Path(filePath)
    fileDir = filePathObj.parent
    fileName = filePathObj.name

    # This will cause the download of the file.
    return bottle.static_file(fileName, root=fileDir, download=True)


# Main program
if __name__ == '__main__':
    print(f"Connect Web Server to http://{HOST}:{PORT}/FileExplorer.bottle")
    print("If client does not connect, it may be because of a firewall")
    print("On RedHat Enterprise Linux (RHEL), run this command as superuser:")
    print("   firewall-cmd --add-port=8080/tcp")
    print("When server is stopped (on RHEL), you can run this as superuser:")
    print("   firewall-cmd --remove-port=8080/tcp")

    # Run the server.
    app.run(host=HOST, port=PORT, debug=True)

