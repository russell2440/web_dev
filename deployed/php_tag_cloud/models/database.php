<?php

class database {
    private $mysqli;
    private $server = 'localhost';
    private $user = 'root';
    private $pass = 'root';
    private $db = 'tagcloud';
    private $sql_file;

    public function __construct($sql_file = null) {
        $this->sql_file = $sql_file;
        $this->mysqli = new mysqli($this->server, $this->user, $this->pass, $this->db);
        
        if ($this->mysqli->connect_error) {
            die("Connection failed: " . $this->mysqli->connect_error);
        }

        $this->initialize_database();
    }

    private function initialize_database() {
        if (!$this->sql_file) {
            return;
        }

        $result = $this->mysqli->query("SHOW TABLES LIKE 'posts'");
        
        if ($result && $result->num_rows == 0) {
            if (file_exists($this->sql_file)) {
                $sql_content = file_get_contents($this->sql_file);
                
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
