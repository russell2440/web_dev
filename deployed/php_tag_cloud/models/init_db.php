<?php
function initialize_database($mysqli) {
    // Check if the 'posts' table already exists to ensure this runs only once
    $result = $mysqli->query("SHOW TABLES LIKE 'posts'");
    
    if ($result && $result->num_rows == 0) {
        $sql_file = __DIR__ . '/../database.sql';
        
        if (file_exists($sql_file)) {
            $sql_content = file_get_contents($sql_file);
            
            // Multi-query executes multiple semicolon-separated statements
            if ($mysqli->multi_query($sql_content)) {
                do {
                    // Store and free each result set to clear out multi-query buffers
                    if ($result = $mysqli->store_result()) {
                        $result->free();
                    }
                } while ($mysqli->more_results() && $mysqli->next_result());
            } else {
                error_log("Database initialization error: " . $mysqli->error);
            }
        }
    }
}
?>
