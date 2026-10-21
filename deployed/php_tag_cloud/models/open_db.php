<?php

$server = 'localhost';
$user = 'root';
$pass = 'root';
$db = 'tagcloud';

$mysqli = new mysqli($server, $user, $pass, $db);
if ($mysqli->connect_error) {
    die("Connection failed: " . $mysqli->connect_error);
}

// Include and run the initializer
include_once __DIR__ . '/init_db.php';
initialize_database($mysqli);

?>
