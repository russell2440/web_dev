<?php

function db_connect() {
    $host = 'localhost';
    $user = 'root';
    $pass = 'root';
    $db = 'records2';
    
    /*
    $host = 'sql302.infinityfree.com';
    $user = 'if0_42358185';
    $pass = 'Merlin85226';
    $db   = 'if0_42358185_db_1';
     */
    
    $mysqli = new mysqli($host, $user, $pass, $db);
    
    mysqli_report(MYSQLI_REPORT_ERROR);
    
    if ($mysqli->connect_error) {
        die("Connection failed: " . $mysqli->connect_error);
    }

    return($mysqli);
}
