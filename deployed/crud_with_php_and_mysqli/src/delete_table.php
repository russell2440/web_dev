
<?php
    require_once __DIR__ . '/db_connect.php';
    require_once __DIR__ . "/flash_helper.php";

    function db_table_delete($mysqli) {
        // Delete the players table
        $sql = "DROP TABLE IF EXISTS players;";
    
        if (!$mysqli->query($sql)) {
            flash_set('warning', "Deletion of 'players' table failed with error {$mysqli->error}.");
        } else {
            flash_set('success', "Deletion of 'players' table successful.");
        }
    }

    $mysqli = db_connect();

    db_table_delete($mysqli);

    header("Location: ../index.php");
?>
