#!/usr/bin/env python

print("Content-Type: text/html")
# Setting Max-Age to 0 tells the browser to immediately delete the cookie
print("Set-Cookie: session_id=deleted; HttpOnly; Max-Age=0; Path=/")
print() # End of headers

print("You have been successfully logged out.")