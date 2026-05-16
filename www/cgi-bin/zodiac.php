<?php

// Define the path to the JSON file, equivalent to Python's JSON_FILE constant.
define('JSON_FILE', './zodiac_list.json');

/**
 * Determines the Zodiac sign based on a given day and month.
 * This function is a direct translation of the Python get_zodiac_sign function.
 *
 * @param int $day The day of the month.
 * @param int $month The month of the year.
 * @return string The Zodiac sign.
 */
function get_zodiac_sign($day, $month) {
    switch ($month) {
        case 1: return ($day < 20) ? 'Capricorn' : 'Aquarius';
        case 2: return ($day < 19) ? 'Aquarius' : 'Pisces';
        case 3: return ($day < 21) ? 'Pisces' : 'Aries';
        case 4: return ($day < 20) ? 'Aries' : 'Taurus';
        case 5: return ($day < 21) ? 'Taurus' : 'Gemini';
        case 6: return ($day < 21) ? 'Gemini' : 'Cancer';
        case 7: return ($day < 23) ? 'Cancer' : 'Leo';
        case 8: return ($day < 23) ? 'Leo' : 'Virgo';
        case 9: return ($day < 23) ? 'Virgo' : 'Libra';
        case 10: return ($day < 23) ? 'Libra' : 'Scorpio';
        case 11: return ($day < 22) ? 'Scorpio' : 'Sagittarius';
        case 12: return ($day < 22) ? 'Sagittarius' : 'Capricorn';
        default: return ''; // Should not happen
    }
}

// --- Data Loading ---
// In Python, this was a try-except block. In PHP, we check if the file exists
// and then decode its contents. The 'true' argument to json_decode converts
// JSON objects to associative arrays, similar to Python dictionaries.
$zodiac_list = [];
if (file_exists(JSON_FILE)) {
    $json_content = file_get_contents(JSON_FILE);
    // Ensure content is not empty to avoid errors with json_decode
    if (!empty($json_content)) {
        $zodiac_list = json_decode($json_content, true);
    }
}

$confirmation_message = "";

// --- Logic to Handle Form Submissions ---
// PHP uses superglobal arrays like $_POST and $_GET to handle request data,
// which is simpler than parsing environment variables and stdin in Python's CGI model.

// Handle Deletion (via GET request)
if ($_SERVER['REQUEST_METHOD'] === 'GET' && isset($_GET['action']) && $_GET['action'] === 'delete') {
    // The null coalescing operator (??) is used for safe access, similar to dict.get() in Python.
    $index_to_delete = $_GET['index'] ?? null;
    
    // Validate the index
    if (is_numeric($index_to_delete) && isset($zodiac_list[$index_to_delete])) {
        // array_splice removes the element and re-indexes the array. It returns the removed element.
        $deleted_person_array = array_splice($zodiac_list, (int)$index_to_delete, 1);
        $deleted_person = $deleted_person_array[0]; // Get the first (and only) deleted element
        
        $first_name = htmlspecialchars($deleted_person['first_name']);
        $last_name = htmlspecialchars($deleted_person['last_name']);
        $confirmation_message = "<h2>Successfully deleted {$first_name} {$last_name}.</h2>";
        
        // Save the updated list back to the file. JSON_PRETTY_PRINT is like 'indent=4' in Python.
        file_put_contents(JSON_FILE, json_encode($zodiac_list, JSON_PRETTY_PRINT));
    } else {
        $confirmation_message = "<h2>Error: Invalid index for deletion.</h2>";
    }
}
// Handle Addition (via POST request)
elseif ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $first_name = $_POST['first_name'] ?? '';
    $last_name = $_POST['last_name'] ?? '';
    $dob_str = $_POST['dob'] ?? '';

    if (!empty($first_name) && !empty($last_name) && !empty($dob_str)) {
        // In PHP, we use DateTime::createFromFormat to parse dates.
        // It returns false on failure, which is how we check for validity.
        $dob_date = DateTime::createFromFormat('d/m/Y', $dob_str);
        
        if ($dob_date) {
            $day = (int)$dob_date->format('d');
            $month = (int)$dob_date->format('m');
            $sign = get_zodiac_sign($day, $month);
            
            $new_entry = [
                'first_name' => $first_name,
                'last_name' => $last_name,
                'sign' => $sign
            ];
            
            // Append the new entry to the array.
            $zodiac_list[] = $new_entry;
            
            file_put_contents(JSON_FILE, json_encode($zodiac_list, JSON_PRETTY_PRINT));
            
            // Using htmlspecialchars is a good practice to prevent XSS attacks.
            $fn_safe = htmlspecialchars($first_name);
            $ln_safe = htmlspecialchars($last_name);
            $sign_safe = htmlspecialchars($sign);
            $confirmation_message = "<h2>Successfully added {$fn_safe} {$ln_safe} ({$sign_safe})!</h2>";
        } else {
            $confirmation_message = "<h2>Error: Invalid date format. Please use DD/MM/YYYY.</h2>";
        }
    }
}

// --- HTML Output ---
// In PHP, we send the Content-Type header first. Then we can echo the HTML.
// The HEREDOC syntax (<<<HTML ... HTML;) is used for large blocks of text,
// similar to Python's triple-quoted strings.
header("Content-Type: text/html");

echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Zodiac List | The Good Webserv</title>
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
            <h1>Zodiac List</h1>
HTML;

echo $confirmation_message;

// Display the list
if (!empty($zodiac_list)) {
    echo '<div class="team-member" style="background: white; padding: 20px; margin-top: 20px;"><table style="width:100%; text-align: left;">';
    echo "<tr><th>First Name</th><th>Last Name</th><th>Sign</th><th>Action</th></tr>";
    
    // We loop through the array using a foreach loop, getting both index and value.
    foreach ($zodiac_list as $index => $person) {
        // Use htmlspecialchars to safely display user-provided data.
        $first_name = htmlspecialchars($person['first_name']);
        $last_name = htmlspecialchars($person['last_name']);
        $sign = htmlspecialchars($person['sign']);
        
        $delete_link = "<a href=\"/cgi-bin/zodiac.php?action=delete&index={$index}\" style=\"color: red;\">Delete</a>";
        echo "<tr><td>{$first_name}</td><td>{$last_name}</td><td>{$sign}</td><td>{$delete_link}</td></tr>";
    }
    echo "</table></div>";
} else {
    echo "<p>The list is currently empty.</p>";
}

echo <<<HTML
            <a href="/zodiac.html" class="btn" style="margin-top: 20px; display: inline-block;">Add Another Person</a>
        </div>
    </main>
    <footer>
        <p>&copy; 2026 42 / WEBSERV</p>
    </footer>
</body>
</html>
HTML;

?>