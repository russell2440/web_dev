<?php
// Ensure session is running before using flash messages
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

/**
 * Set or display flash messages.
 *
 * @param string|null $name    The key name for the message (e.g., 'success', 'error').
 * @param string|null $message The message text to set. Leave empty to display messages.
 * @param string      $class   CSS class modifier for styling (default: 'info').
 */
function flash($name = null, $message = null, $class = 'info') {
    // SCENARIO 1: Setting a message
    if (!empty($name) && !empty($message)) {
        // Clear any old message under this key first
        if (!empty($_SESSION['flash'][$name])) {
            unset($_SESSION['flash'][$name]);
        }
        $_SESSION['flash'][$name] = [
            'message' => $message,
            'class'   => $class
        ];
        return;
    }

    // SCENARIO 2: Displaying a specific message by name
    if (!empty($name) && empty($message)) {
        if (!empty($_SESSION['flash'][$name])) {
            $msg = $_SESSION['flash'][$name];
            unset($_SESSION['flash'][$name]); // Remove after displaying
            
            echo '<div class="alert alert-' . htmlspecialchars($msg['class']) . '">' 
               . htmlspecialchars($msg['message']) 
               . '</div>';
        }
        return;
    }

    // SCENARIO 3: Displaying ALL pending flash messages (if no $name provided)
    if (empty($name) && !empty($_SESSION['flash'])) {
        foreach ($_SESSION['flash'] as $key => $msg) {
            echo '<div class="alert alert-' . htmlspecialchars($msg['class']) . '">' 
               . htmlspecialchars($msg['message']) 
               . '</div>';
            unset($_SESSION['flash'][$key]);
        }
    }
}
