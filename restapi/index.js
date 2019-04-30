const request = require("request");
const express = require("express");
const mysql = require("mysql");
const dgram = require("dgram");
const mqtt = require("mqtt");
const parser = require("body-parser");
const app = express();
const con = mysql.createConnection({
  host: "localhost",
  user: "apiuser",
  password: "apipwd",
  database: "restapi"
});
var mqttClient = mqtt.connect("mqtt://localhost");
var jsonParser = parser.json();

con.connect(function(err) {
  if (err) throw err;
  console.log("Database connected.");
});

function toggleDevice(id,status) {
  var newStatus
  if (status == 0) {newStatus = "1"}
  else if (status == 1) {newStatus = "0"};
  con.query(`UPDATE Devices SET status = ${ newStatus } WHERE DevicesID = ${ id }`, function (err,rows,fields){
  if (err) throw err;
  mqttClient.publish(`action/device/${ id }`, newStatus);
  });
}

app.post('/toggledevice', jsonParser, function (req,res) {
  con.query((`SELECT nombre,status FROM Devices WHERE DevicesID = ${ req.body.id }`), function (err,rows,fields){
  if (err) throw err;
  toggleDevice(req.body.id, rows[0].status);
  res.send(`Dispositivo ${ rows[0].nombre } actualizado con exito`);
  });
});

app.get('/sensor/:id', function (req,res) {
  con.query((`SELECT nombre,status FROM Sensores WHERE id = ${ req.params.id }`), function (err,rows,fields){
  if (err) throw err;
  res.send(rows[0]);
  });
});

app.get('/device/:id', function (req,res) {
  con.query((`SELECT nombre,status FROM Devices WHERE DevicesID = ${ req.params.id }`), function (err,rows,fields){
  if (err) throw err;
  res.send(rows[0]);
  });
});

mqttClient.on("connect", function() {
  mqttClient.subscribe("status/device/1", function() {})
})

mqttClient.on("message", function(topic,message) {
  var type = (topic.split('/'))[1];
  var id = (topic.split('/'))[2];
  console.log(message.toString());
  if (type == "device") {
    con.query((`SELECT status FROM Devices WHERE DevicesID = ${ id }`), function (err, rows, fields){
      if (err) throw err;
      if (rows[0].status != message) {
        con.query((`UPDATE Devices SET status = ${ message } WHERE DevicesID = ${ id }`), function(error) {if (err) throw err}
      )};
    });
  };
})

app.listen(3000, () => {
 console.log("El servidor está inicializado en el puerto 3000");
});

