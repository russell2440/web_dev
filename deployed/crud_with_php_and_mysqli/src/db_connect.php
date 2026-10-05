<?php

function db_connect() {
    // Check if the host contains 'localhost' or an IP like 127.0.0.1 / ::1
    $is_local = in_array($_SERVER['HTTP_HOST'], ['localhost', '127.0.0.1', 'localhost:8888']) || stristr($_SERVER['HTTP_HOST'], 'localhost');
    
    if ($is_local) {
        // MAMP Configuration
        $host = 'localhost';
        $user = 'root';
        $pass = 'root';
        $db = 'records2';
    } else {
        // InfinityFree Configuration
        $host = 'sql302.infinityfree.com';
        $user = 'if0_42358185';
        $pass = 'Merlin85226';
        $db   = 'if0_42358185_db_1';
    }

    $mysqli = new mysqli($host, $user, $pass, $db);

    mysqli_report(MYSQLI_REPORT_ERROR);

    if ($mysqli->connect_error) {
        die("Connection failed: " . $mysqli->connect_error);
    }

    return($mysqli);
}
