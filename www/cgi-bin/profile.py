#!/usr/bin/env python

import os

# The C++ server should validate the cookie and pass the username here.
username = os.environ.get('WEBSERV_USERNAME')
session_id = os.environ.get('HTTP_COOKIE', '').split('session_id=')[-1].split(';')[0]


print("Content-Type: text/html")
print() # End of headers

if username:
    print(f"<h1>Welcome back, {username}!</h1>")
    print(f"<p>This information was provided by the C++ server.</p>")
    print(f"<p>Your validated session ID is: {session_id}</p>")
else:
    print("<h1>You are not logged in.</h1>")
    print("<p>The C++ server did not validate your session.</p>")