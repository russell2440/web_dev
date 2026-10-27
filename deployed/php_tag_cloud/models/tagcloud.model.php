<?php

// Tag Cloud model class
//=================================================

class tagcloud_model {
    private $db;
    var $tags; // array(id, name, total)
    var $largest;

    // Constructor
    //=================================================

    function __construct(database $db)
    {
        $this->db = $db;

        // get tags and largest value;
        $this->get_tags();
    }

    // Utility functions
    //=================================================

    function get_tags() {
        $this->tags = array();
        $this->largest = 0;

        $query_string = "
            SELECT posts_to_tags.tag_id, tags.name, COUNT(posts_to_tags.tag_id) AS total
            FROM tags
            JOIN posts_to_tags ON posts_to_tags.tag_id = tags.id
            GROUP BY posts_to_tags.tag_id, tags.name";

        // Ask the generic database object to execute and fetch the rows
        $rows = $this->db->fetch_all($query_string);

        if ($rows !== false) {
            foreach ($rows as $row) {
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

    // Display functions
    //=================================================

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
        if ($this->tags == FALSE) {
            return 'No tags to display';
        } else {
            // Create unordered list
            $list = '';

            $list .= '<ul class="tagcloud">';

            foreach ($this->tags as $tag) {
                // create list item
                $weight = round(($tag['total']/$this->largest) * 10);
                $list .= "<li><a href='tags.php?id=". $tag['id']. "' class='tag".$weight."'>";
                $list .= $tag['name'] . "</a></li>";
            }

            $list .= '</ul>';

            return $list;
        }
    }

}

?>
