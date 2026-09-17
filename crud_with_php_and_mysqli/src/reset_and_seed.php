<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require_once 'connect_db.php';
require_once 'flash_helper.php';
require_once 'ai_helper.php';

// 1. Truncate table
$mysqli->query("TRUNCATE TABLE players");

// 2. Fetch array dynamically via AI helper function
$players = generate_dynamic_players();

// 3. Insert records
$stmt = $mysqli->prepare("INSERT INTO players (firstname, lastname) VALUES (?, ?)");
foreach ($players as $player) {
    if (isset($player[0], $player[1])) {
        $stmt->bind_param("ss", $player[0], $player[1]);
        $stmt->execute();
    }
}

$stmt->close();
$mysqli->close();

$totalInserted = count($players);

// Set a success message before redirecting
flash_set('success', "Database successfully reset and seeded with {$totalInserted} dynamic players!");

// Redirect straight to view page
header("Location: view_paginated.php");
exit();
