<?php

/*
	This line uses the md5 function (http://php.net/manual/en/function.md5.php). This function returns
	a 32 character string, created using the combination of the intended password ("admin") with a salt 
	(a random string of numbers, letters and special characters intended to make the password harder to 
	figure out if an unauthorized individual gains access to your database.)
*/

echo md5('admin' . 'jK7d?3');

?>