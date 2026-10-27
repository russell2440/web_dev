<?php

/*
 *  DB.PHP
 *  Database Settings & Connection
*/


// database settings
$server = 'localhost';
$user = 'root';
$pass = 'root';
$db = 'login';

// connect to the database
$mysqli = new mysqli($server, $user, $pass, $db);

?>