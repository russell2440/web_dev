<?php

class tagcloud_model {
    var $mysqli;
    var $tags; // array(id, name, total)
    var $largest;

    function __construct($conn)
    {
        $this->mysqli = $conn;

        // get tags and largest value;
        $this->get_tags();
    }

    /* Utility functions
     * ============================================= */

    function get_tags() {
        $this->tags = array();
        $this->largest = 0;

        $query_string = "SELECT posts_to_tags.tag_id, tags.name, COUNT(posts_to_tags.tag_id) AS total FROM tags, posts_to_tags WHERE post_to_tags.tag_id = tags.id GROUP BY posts_to_tags.tag_id";
        $result = $this->mysqli->query($query_string);

        if ($result->num_rows > 0) {
            while ($row) {
            }
        } else {
            $this->tags = FALSE;
        }
    }


    /* Display functions
     * ============================================= */

    function get_tag_list() {
    }

    function get_tag_cloud() {
    }

}

?>
