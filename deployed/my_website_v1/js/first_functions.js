function sniffer(message){
    console.log("sniffer caught event: " + message);
}

document.getElementById("ht").onkeypress =function(){
    sniffer('html onkeypress event');
}
document.body.onload=function(){
    sniffer('body onload event');
}
document.body.onresize =function(){
    sniffer('body onresize event');
}
document.getElementById("input").onchange=function(){
    alert('input onchange event');
    sniffer('input onchange event');
}
document.getElementById("butt").onclick=function(){
    sniffer('butt onclick event');
}
document.getElementById("first_p").onclick=function(){
    sniffer('first_p onclick event');
}
document.getElementById("second_p").onmouseover=function(){
    sniffer('second_p onmouseover event');
}
