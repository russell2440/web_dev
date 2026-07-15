<?php

$server = 'localhost';
$user = 'root';
$pass = 'root';
$db = 'records2';

$mysqli = new mysqli($server, $user, $pass, $db);
mysqli_report(MYSQLI_REPORT_ALL);

// Create the table
/*
CREATE TABLE `records2`.`players` (`id` INT NOT NULL AUTO_INCREMENT , `firstname` VARCHAR(32) NOT NULL , `lastname` VARCHAR(32) NOT NULL , PRIMARY KEY (`id`)) ENGINE = InnoDB;
 */

// Insert values
/*
INSERT INTO players (firstname, lastname)
VALUES 
    ('Jaxon', 'Miller'),
    ('Elena', 'Rostova'),
    ('Marcus', 'Vance'),
    ('Aria', 'Chen');
 */

// Truncate table
/*
 truncate table players;
 */


?>
