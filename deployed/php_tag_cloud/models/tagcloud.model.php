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

        echo '<pre>';
        print_r($this->tags);
        echo '</pre>';
        exit;
    }

    /* Utility functions
     * ============================================= */

    function get_tags() {
        $this->tags = array();
        $this->largest = 0;

        /*
        $query_string = "SELECT posts_to_tags.tag_id, tags.name, COUNT(posts_to_tags.tag_id) AS total FROM tags, posts_to_tags WHERE post_to_tags.tag_id = tags.id GROUP BY posts_to_tags.tag_id";
         */
        $query_string = "
            SELECT posts_to_tags.tag_id, tags.name, COUNT(posts_to_tags.tag_id) AS total
            FROM tags
            JOIN posts_to_tags ON posts_to_tags.tag_id = tags.id
            GROUP BY posts_to_tags.tag_id, tags.name";
        $result = $this->mysqli->query($query_string);

        if ($result->num_rows > 0) {
            while ($row = $result->fetch_object()) {
                // check for largest
                if ($row->total > $this->largest) {
                    $this->largest = $row->total;
                }

                // add tag to array
                $this->tags[] = array(
                    'id' => $row->tag_id,
                    'name' => $row->name,
                    'total' => $row->total);
            }
            
            // sort tags
            usort($this->tags, array('tagcloud_model', 'compare_names'));
        } else {
            $this->tags = FALSE;
        }
    }

    function compare_names($a, $b) {
        return strcmp($a['name'], $b['name']);
    }

    /* Display functions
     * ============================================= */

    function get_tag_list() {
    }

    function get_tag_cloud() {
    }

}

?>
