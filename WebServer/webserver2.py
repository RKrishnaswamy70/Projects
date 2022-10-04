# Python3.7+
#
# Start this up with:
#   python3.6 webserver1.py
# Now type in the following URL in your Web browser’s address:
#   bar http://localhost:8888/hello
#
import socket

HOST, PORT = '', 8888

listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listen_socket.bind((HOST, PORT))
listen_socket.listen(1)
print(f'Serving HTTP on port {PORT} ...')
while True:
    client_connection, client_address = listen_socket.accept()
    request_data = client_connection.recv(1024)
    print(request_data.decode('utf-8'))

    http_response = b"""Content-Type: text/html

<!DOCTYPE HTML//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html lang="en" dir="ltr" >
<head>
<title>GNU cgicc v3.2.19</title>
</head>
<body>
<h1>Hello, world from GNU cgicc</h1>
</body></html>
"""
    client_connection.sendall(http_response)
    client_connection.close()

