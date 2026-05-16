#!/usr/bin/python3
import os
import sys
import urllib.parse

def handle_post():
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length == 0:
        return {}
    post_data = sys.stdin.read(content_length)
    return urllib.parse.parse_qs(post_data)

def main():
    method = os.environ.get('REQUEST_METHOD', 'GET')
    
    print("Content-Type: text/html")
    print()
    
    script_name = os.environ.get('SCRIPT_NAME', '')

    print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>POST Test | The Good Webserv</title>
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
            <h1>Simple POST Handler</h1>""")
    
    if method == 'POST':
        post_data = handle_post()
        print('<div class="team-member" style="text-align: left; padding: 20px; margin-bottom: 20px;">')
        print('<h3>POST Data Received:</h3>')
        if post_data:
            for key, value in post_data.items():
                val = value[0] if value else ''
                print(f'<p><strong>{key}:</strong> {val}</p>')
        else:
            print('<p>No POST data received</p>')
        print('</div>')
    
    print(f"""
            <form action="{script_name}" method="post" class="contact-form">
                <input type="text" name="name" placeholder="Name" required>
                <input type="email" name="email" placeholder="Email" required>
                <textarea name="message" rows="4" placeholder="Message" required></textarea>
                <button type="submit" class="btn">Submit POST</button>
            </form>
        </div>
    </main>
    <footer>
        <p>&copy; 2026 42 / WEBSERV</p>
    </footer>
</body>
</html>""")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("Content-Type: text/html")
        print()
        print(f"<h1>Error</h1><p>{str(e)}</p>")