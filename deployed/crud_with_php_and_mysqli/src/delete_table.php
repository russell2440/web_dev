
<?php
    require_once __DIR__ . '/db_connect.php';
    require_once __DIR__ . '/db_helper.php';

    $mysqli = db_connect();

    db_table_delete($mysqli);

    header("Location: ../index.php");
?>
