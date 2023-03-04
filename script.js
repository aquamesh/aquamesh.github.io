var list = document.getElementById("label1");

for (var i = 0; i < 4; i ++) {
    var b = document.createElement("button");
    var txt = "Device "
    b.innerHTML = txt.concat((i+1).toString());
    b.style.backgroundColor = "rgb(0, 179, 255)";
    b.style.color="rgb";
    b.style.position = "relative";
    // b.style.width="50%";
    b.style.height="4vh";
    b.style.width="10vw"
    list.appendChild(b);
    list.appendChild(document.createElement("br"));
    list.appendChild(document.createElement("br"));
}

var xValues = [50,60,70,80,90,100,110,120,130,140,150];
var yValues = [7,8,8,9,9,9,10,11,14,14,15];

new Chart("line", {
type: "line",
data: {
    labels: xValues,
    datasets: [{
    fill: true,
    lineTension: 0,
    backgroundColor: "rgba(0, 179, 255,1.0)",
    borderColor: "rgba(0, 179, 255,0.1)",
    data: yValues
    }]
},
options:{
    legend: {display: false},
    scales: {
    yAxes: [{ticks: {min: 6, max:16}}],
    }
}
});

