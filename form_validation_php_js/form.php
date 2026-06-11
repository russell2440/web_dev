<!DOCTYPE html>
<html>
    <head>
        <title>Form Validation Example</title>
        
        <script type="text/javascript" src="jquery.js"></script>
        <script type="text/javascript" src="validate.js"></script>
        <script type="text/javascript"></script>
    </head>

    <style type="text/css">
        body {}
        fieldset {}
        label {}
        label em {}
        textarea, input {}
        textarea {}
        #submit {}
        label.error {}
        .required_msg {}
    </style>
    
    <body>
        <form>
            <fieldset>
                <label for="name">Name: <em>*</em></label>
                <input type="text" name="name" id="name">

                <label for="phone">Phone (000-000-0000): </label>
                <input type="text" name="phone" id="phone">

                <label for="fax">Fax (000-000-0000): </label>
                <input type="text" name="fax" id="fax">

                <label for="email">Email: <em>*</em></label>
                <input type="text" name="email" id="email">

                <label for="comments">Comments: </label>
                <textarea name="comments" id="comments"></textarea>

                <p class="required_msg">* required fields</p>

                <input type="submit" name="submit" id="submit">

            </fieldset>
        </form>
    </body>
</html>
