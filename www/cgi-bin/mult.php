#!/usr/bin/php-cgi
<?php
    // --- Script Logic ---
    $result = null;
    $error_message = null;

    // Try to perform the multiplication.
    // Checks if 'a' and 'b' are set and are numeric.
    if (isset($_GET['a']) && isset($_GET['b']) && is_numeric($_GET['a']) && is_numeric($_GET['b'])) {
        $val_a = (float)$_GET['a'];
        $val_b = (float)$_GET['b'];
        $result = $val_a * $val_b;
    } else {
        // Set a generic error message if parameters are missing or not numbers.
        $error_message = "Error: Please provide two numbers. Example: <strong>?a=5&b=10</strong>";
    }

    // --- HTTP Header & Output ---
    // This line is crucial for CGI; it separates headers from the body.
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Multiplier | The Good Webserv</title>
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
<?php
if ($error_message) {
    echo "<h1>Error</h1><p>{$error_message}</p>";
} else {
    echo "<h1>Result</h1><h1 style='font-size: 5em;'>{$result}</h1>";
}
?>
        </div>
    </main>
    <footer>
        <p>&copy; 2026 42 / WEBSERV</p>
    </footer>
</body>
</html>