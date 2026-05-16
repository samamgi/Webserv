#!/usr/bin/env python3

import os
import sys
from urllib.parse import parse_qs

session_id = os.environ.get('WEBSERV_SESSION_ID')

query = os.environ.get('QUERY_STRING', '')
params = parse_qs(query)
if not params:
    content_length = int(os.environ.get('CONTENT_LENGTH', 0) or 0)
    if content_length > 0:
        body = sys.stdin.read(content_length)
        params = parse_qs(body)

username = params.get('username', ['guest'])[0]

# Print the HTTP headers
print("Content-Type: text/html")

# Set the cookie if the server provided a session ID
if session_id:
    print(f"Set-Cookie: session_id={session_id}; HttpOnly; Max-Age=3600; Path=/")
    print() # End of headers
    print(f"Login successful for {username}! Your session is now active.")
else:
    print() # End of headers
    print("<h1>Login Failed</h1>")
    print("<p>Server did not provide a session ID.</p>")