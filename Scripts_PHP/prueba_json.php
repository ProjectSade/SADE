<?php
$json = file_get_contents('php://input');
//$json = '{"pet":"entrada_sala","user":"prototipo","sala":2}';
//$json = '{"a":1,"b":2,"c":3,"d":4,"e":5}';
var_dump(json_decode($json));
$datos = json_decode($json);
var_dump($datos);
//print_r($datos);



echo "Petición: " . $datos->pet . " Usuario: " . $datos->user . " Sala: " . $datos->sala;


$conn = new PDO("mysql:host=localhost;dbname=dispositivo","root","");
$ins = $conn->query("INSERT INTO prototipo (id,sala,fecha) VALUES (null,$datos->sala,CURRENT_TIMESTAMP)");
echo true;
?>

