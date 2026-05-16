#!/usr/bin/env python3
import os
import sys
import html

UPLOAD_DIR = '../uploads/'

def send_response(status="200 OK", content_type="text/html", body=""):
    sys.stdout.write(f"Status: {status}\r\n")
    sys.stdout.write(f"Content-Type: {content_type}\r\n")
    sys.stdout.write(f"Content-Length: {len(body)}\r\n")
    sys.stdout.write("\r\n")
    sys.stdout.write(body)

def generate_page(title, content):
    return f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title} | The Good Webserv</title>
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
            <h1>{title}</h1>
            <p>{content}</p>
        </div>
    </main>
    <footer>
        <p>&copy; 2026 42 / WEBSERV</p>
    </footer>
</body>
</html>
"""

def parse_multipart_form_data():
    try:
        content_type = os.environ['CONTENT_TYPE']
        content_length = int(os.environ['CONTENT_LENGTH'])
        boundary = '--' + content_type.split('boundary=')[1]
    except (KeyError, IndexError, ValueError):
        return None, "Missing or invalid headers."

    body_bytes = sys.stdin.buffer.read(content_length)
    parts = body_bytes.split(boundary.encode())
    form_data = {}

    for part in parts:
        if b'Content-Disposition' not in part:
            continue
        try:
            headers_raw, content = part.split(b'\r\n\r\n', 1)
            content = content.rstrip(b'\r\n')
            
            headers = headers_raw.decode().split('\r\n')
            disposition = next((h for h in headers if 'Content-Disposition' in h), None)
            
            if disposition:
                field_name, filename = "", ""
                disp_parts = disposition.split(';')
                for item in disp_parts:
                    item = item.strip()
                    if item.startswith('name='):
                        field_name = item.split('=')[1].strip('"')
                    elif item.startswith('filename='):
                        filename = item.split('=')[1].strip('"')
                
                if field_name:
                    if filename:
                        form_data[field_name] = (filename, content)
                    else:
                        form_data[field_name] = content.decode()
        except Exception:
            continue
            
    return form_data, None

def main():
    if os.environ.get("REQUEST_METHOD") != "POST":
        send_response("405 Method Not Allowed", body=generate_page("Error", "405 Method Not Allowed"))
        return

    try:
        os.makedirs(UPLOAD_DIR, exist_ok=True)
    except OSError as e:
        send_response("500 Internal Server Error", body=generate_page("Error", f"Could not create upload directory: {html.escape(str(e))}"))
        return

    form, error = parse_multipart_form_data()

    if error:
        send_response("400 Bad Request", body=generate_page("Error", f"Could not parse form data: {html.escape(error)}"))
        return
        
    if 'filename' in form and isinstance(form['filename'], tuple):
        original_filename, file_bytes = form['filename']
        if original_filename:
            safe_filename = os.path.basename(original_filename)
            filepath = os.path.join(UPLOAD_DIR, safe_filename)
            try:
                with open(filepath, 'wb') as f:
                    f.write(file_bytes)
                message = f'The file "{html.escape(safe_filename)}" was uploaded successfully.'
                send_response("200 OK", body=message)
            except IOError as e:
                message = f'Could not save file. Error: {html.escape(str(e))}'
                send_response("500 Internal Server Error", body=generate_page("Error", message))
        else:
            send_response("400 Bad Request", body=generate_page("Error", 'No file was selected.'))
    else:
        send_response("400 Bad Request", body=generate_page("Error", 'Missing "filename" field.'))

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        body = generate_page("Internal Server Error", f"An unexpected error occurred: {html.escape(str(e))}")
        send_response("500 Internal Server Error", body=body)