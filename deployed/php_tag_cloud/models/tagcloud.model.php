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
        if ($this->tags == FALSE) {
            return 'No tags to display';
        } else {
            $list = '';

            // display the tags in a table
            $list .= "<table border='1' cellpadding='10'>";
            $list .= "<tr><th>ID</th><th>Tag Name</th><th>Count</th><th>Weight</th></tr>";

            foreach ($this->tags as $tag) {
                $list .= "<tr>";
                $list .= "<td>" . $tag['id'] . "</td>";
                $list .= "<td>" . $tag['name'] . "</td>";
                $list .= "<td>" . $tag['total'] . "</td>";
                $weight = round(($tag['total']/$this->largest) * 10);
                $list .= "<td>" . $weight . "</td>";
                $list .= "</tr>";
            }

            $list .= '</table>';

            return $list;
        }
    }

    function get_tag_cloud() {
        //return '';
        if ($this->tags == FALSE) {
            return 'No tags to display';
        } else {
            // Create unordered list
            $list = '';

            $list .= '<ul class="tagcloud">';

            foreach ($this->tags as $tag) {
                // create list item
                $weight = round(($tag['total']/$this->largest) * 10);
                $list .= "<li><a class='tag".$weight."' href='tags.php?id=". $tag['id']. "'>";
                $list .= $tag['name'] . "<a></li>";
                $list .= "\n";
            }

            $list .= '</ul>';

            return $list;
        }
    }

}

?>
