#!/usr/bin/python
import os
import sys

# --- Script Logic ---
result = None
error_message = None
params = {}

# Get the query string from the environment variable.
query_string = os.environ.get('QUERY_STRING')

# Parse the query string into a dictionary.
if query_string:
    pairs = query_string.split('&')
    for pair in pairs:
        if '=' in pair:
            key, value = pair.split('=', 1)
            params[key] = value

# Try to perform the addition.
try:
    val_a = int(params['a'])
    val_b = int(params['b'])
    result = val_a + val_b
except (KeyError, ValueError):
    error_message = "Error: Please provide two numbers. Example: <strong>?a=5&b=10</strong>"

# --- HTTP Header & Output ---
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")

print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Adder | The Good Webserv</title>
    <link rel="stylesheet" href="/static/style.css">
</head>
<body>
    <header>
        <nav>
            <a href="/index.html" class="nav-brand">WebServ</a>
            <ul>
                <li><a href="/index.html">Home</a></li>
                <li><a href="/about.html">About</a></li>
                <li><a href="/contact.html">Contact</a></li>
                <li class="dropdown">
                    <a href="javascript:void(0)" class="dropbtn">CGI Tests</a>
                    <div class="dropdown-content">
                        <a href="/upload.html">Upload</a>
                        <a href="/zodiac.html">Zodiac</a>
                        <a href="/cgi-bin/simple_post.py">POST</a>
                        <a href="/cgi-bin/pet_gallery.py">Pet Gallery</a>
                        <a href="/cgi-bin/loop.py">Loop</a>
                        <a href="/cgi-bin/error.py">Error</a>
                        <a href="/cookie_test.html">Cookie Test</a>
                    </div>
                </li>
            </ul>
        </nav>
    </header>
    <main>
        <div class="card">
""")

if error_message:
    print(f"<h1>Error</h1><p>{error_message}</p>")
else:
    print(f"<h1>Result</h1><h1 style='font-size: 5em;'>{result}</h1>")

print("""
        </div>
    </main>
    <footer>
        <p>&copy; 2026 42 / WEBSERV</p>
    </footer>
</body>
</html>
""")