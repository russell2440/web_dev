<?php
    require_once __DIR__ . '/db_connect.php';
    require_once __DIR__ . "/flash_helper.php";

    function db_table_create($mysqli) {
        // Create the players table
        $sql =
            "CREATE TABLE IF NOT EXISTS players (`id` INT NOT NULL AUTO_INCREMENT , `firstname` VARCHAR(32) NOT NULL , `lastname` VARCHAR(32) NOT NULL , PRIMARY KEY (`id`)) ENGINE = InnoDB DEFAULT CHARSET=utf8mb4;";
        /*
            "CREATE TABLE IF NOT EXISTS `if0_42358185_db_1`.`players` (`id` INT NOT NULL AUTO_INCREMENT , `firstname` VARCHAR(32) NOT NULL , `lastname` VARCHAR(32) NOT NULL , PRIMARY KEY (`id`)) ENGINE = InnoDB DEFAULT CHARSET=utf8mb4;";
         */
    
        if (!$mysqli->query($sql)) {
            flash_set('warning', "Creation of 'players' table failed with error {$mysqli->error}.");
        } else {
            flash_set('success', "Creation of 'players' table successful.");
        }
    }

    $mysqli = db_connect();

    db_table_create($mysqli);

    header("Location: ../index.php");
?>
