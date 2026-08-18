<?php
    require_once('connect_db.php');

    if (isset($_GET['id']) && is_numeric($_GET['id'])) {
        $id = $_GET['id'];
        if($stmt = $mysqli->prepare('DELETE FROM players WHERE id = ? LIMIT 1')) {
            $stmt->bind_param('i', $id);
            $stmt->execute();
            $stmt->close();
        } else {
            echo "Error: could not prepare SQL statement.";
        }
        $mysqli->close();
    }

    header("Location: view.php");
?>
