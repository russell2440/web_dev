
function validate_form() {
    var first_name_elem = document.getElementById("first_name_id");
    var first_name_value = first_name_elem.value;

    console.log("checking name :" + first_name_value);
    if (first_name_value != 'Russ') {
        p_elem = document.getElementById("p_id");
        p_elem.style.color = "red";
        p_elem.innerHTML = "<h2>Put in my name!</h2>";

        first_name_elem.select();
        first_name_elem.focus();

        return false;
    }

    return true;
}

document.getElementById("submit_id").onclick =function() {
    var do_submit = validate_form();

    if (false == do_submit)
    {
        return false;
    }

    return true;
}

