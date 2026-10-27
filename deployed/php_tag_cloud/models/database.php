<?php

class database {
    private $mysqli;
    private $server = 'localhost';
    private $user = 'root';
    private $pass = 'root';
    private $db = 'tagcloud';

    public function __construct() {
        $this->mysqli = new mysqli($this->server, $this->user, $this->pass, $this->db);
        
        if ($this->mysqli->connect_error) {
            die("Connection failed: " . $this->mysqli->connect_error);
        }

        $this->initialize_database();
    }

    private function initialize_database() {
        $result = $this->mysqli->query("SHOW TABLES LIKE 'posts'");
        
        if ($result && $result->num_rows == 0) {
            $sql_file = __DIR__ . '/../database.sql';
            if (!file_exists($sql_file)) {
                $sql_file = __DIR__ . '/database.sql';
            }
            
            if (file_exists($sql_file)) {
                $sql_content = file_get_contents($sql_file);
                
                if ($this->mysqli->multi_query($sql_content)) {
                    do {
                        if ($result = $this->mysqli->store_result()) {
                            $result->free();
                        }
                    } while ($this->mysqli->more_results() && $this->mysqli->next_result());
                } else {
                    error_log("Database initialization error: " . $this->mysqli->error);
                }
            }
        }
    }

    // Generic helper method: executes any SQL string and returns rows as objects
    public function fetch_all($sql) {
        $result = $this->mysqli->query($sql);

        if ($result && $result->num_rows > 0) {
            $rows = [];
            while ($row = $result->fetch_object()) {
                $rows[] = $row;
            }
            $result->free();
            return $rows;
        }

        return false;
    }

    public function __destruct() {
        if ($this->mysqli) {
            $this->mysqli->close();
        }
    }
}

?>
