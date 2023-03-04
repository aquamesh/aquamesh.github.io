const xyValues = [
    {x:50, y:7},
    {x:60, y:8},
    {x:70, y:8},
    {x:80, y:9},
    {x:90, y:9},
    {x:100, y:9},
    {x:110, y:10},
    {x:120, y:11},
    {x:130, y:14},
    {x:140, y:14},
    {x:150, y:15}
];

var x = new Chart("scatter", {
type: "scatter",
data: {
    datasets: [{
    pointRadius: 4,
    pointBackgroundColor: "rgb(0,0,255)",
    data: xyValues
}]
},
options: {
    legend: {display: false},
    scales: {
    xAxes: [{ticks: {min: 40, max:160}}],
    yAxes: [{ticks: {min: 6, max:16}}],
    }
}
});

var xValues = [50,60,70,80,90,100,110,120,130,140,150];
var yValues = [7,8,8,9,9,9,10,11,14,14,15];

new Chart("line", {
type: "line",
data: {
    labels: xValues,
    datasets: [{
    fill: true,
    lineTension: 0,
    backgroundColor: "rgba(0,0,255,1.0)",
    borderColor: "rgba(0,0,255,0.1)",
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
