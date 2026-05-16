#!/usr/bin/env python

import os
import sys
import html

# A modern, dependency-free CGI script template
#This test file was made by another student

def main():
    """
    Main function to handle the CGI request.
    """
    # --- 1. Print HTTP Headers ---
    # The Content-Type header is required. The blank line after the
    # headers tells the server where the body begins.
    # Using \r\n (CRLF) is the proper HTTP line ending.
    sys.stdout.write("Content-Type: text/html\r\n")
    sys.stdout.write("\r\n")

    # --- 2. Generate the HTML content ---
    body = "<html>\n"
    body += "<head><title>Modern CGI Example</title></head>\n"
    body += "<body>\n"
    body += "<h1>Welcome to a Modern CGI Script!</h1>\n"
    
    # --- 3. Display CGI Environment Variables ---
    body += "<h2>Environment Variables:</h2>\n"
    body += "<table border='1' cellpadding='5'>\n"
    body += "<tr><th>Variable</th><th>Value</th></tr>\n"
    for key, value in sorted(os.environ.items()):
        # Use html.escape to prevent HTML injection from variable values
        safe_key = html.escape(key)
        safe_value = html.escape(value)
        body += f"<tr><td>{safe_key}</td><td>{safe_value}</td></tr>\n"
    body += "</table>\n"

    body += "</body>\n"
    body += "</html>\n"

    # --- 4. Write the content to standard output ---
    sys.stdout.write(body)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        # If an error occurs, send a plain text error message and a 500 status
        # This makes debugging much easier.
        sys.stdout.write("Content-Type: text/plain\r\n")
        sys.stdout.write("Status: 500 Internal Server Error\r\n")
        sys.stdout.write("\r\n")
        sys.stdout.write("An internal error occurred in the Python script.\n")
        sys.stdout.write(f"Error: {e}\n")
        sys.exit(1) # Exit with a non-zero status to signal failure