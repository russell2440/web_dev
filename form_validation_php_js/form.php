<!DOCTYPE html>
<html>
    <head>
        <title>Form Validation Example</title>
        <meta tbd>        
        <script type="text/javascript" src="xjquery.js"></script>
        <script type="text/javascript" src="xvalidate.js"></script>

        <script type="text/javascript">
            $(document).ready(function(){
                $.validator.addMethod( "phoneUS", function( phone_number, element ) {
                    phone_number = phone_number.replace( /\s+/g, "" );
	                return this.optional( element ) || phone_number.length > 9 && phone_number.match( /^(\+?1-?)?(\([2-9]([02-9]\d|1[02-9])\)|[2-9]([02-9]\d|1[02-9]))-?[2-9]\d{2}-?\d{4}$/ );
                    }, "Please specify a valid phone number." );

                $("#form").validate();
            });
        </script>
<!--
/*
 */
-->
    </head>

    <style type="text/css">
        body {font-family: Arial, Helvetica, sans-serif; font-size: 12px;}
        fieldset { border: 0;}
        label { display: block; float: left; width: 180px; clear: both; margin-top: 10px;}
        label em { display: block; float: right; padding-right: 8px; color: red;}
        textarea, input {float: left; width: 220px; padding: 2px;}
        textarea { height: 180px;}
        #submit { margin-left: 180px; clear: both; width: 100px;}
        label.error {float: left; width: 200px; color: red; clear: none; padding-left: 10px; font-size: 11px;}
        .required_msg { float: left; padding-left: 180px; clear: both; color: red;}
    </style>
    
    <body>
        <form action="" method="post" id="form">
            <fieldset>
                <label for="name">Name: <em>*</em></label>
                <input type="text" name="name" id="name" class="required" value="<?php echo $form['name']; ?>"> <?php echo $error['name'] ?>

                <label for="phone">Phone (000-000-0000): <em>*</em></label>
                <input type="text" name="phone" id="phone" class="required phoneUS" value="<?php echo $form['phone']; ?>"> <?php echo $error['phone'] ?>

                <label for="fax">Fax (000-000-0000): </label>
                <input type="text" name="fax" id="fax" value="<?php echo $form['fax']; ?>">

                <label for="email">Email: <em>*</em></label>
                <input type="text" name="email" id="email" class="required email" value="<?php echo $form['email']; ?>"> <?php echo $error['email'] ?>

                <label for="comments">Comments: </label>
                <textarea name="comments" id="comments"><?php echo $form['email']; ?></textarea>

                <p class="required_msg">* required fields</p>

                <input type="submit" name="submit" id="submit">

            </fieldset>
        </form>
    </body>
</html>
