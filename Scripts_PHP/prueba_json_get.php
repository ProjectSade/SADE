<?php
$conn = new PDO("mysql:host=localhost;dbname=dispositivo","root","");
$res = $conn->query("SELECT Usuario,Fecha,Estancia,Movimiento,Distancia FROM usuarios WHERE id = (SELECT MAX(id) FROM usuarios)");
$resul = array();

foreach ($res as $row)
{
	array_push($resul,array(
		'Usuario' => $row['Usuario'],
		'Fecha' => $row['Fecha'],
		'Estancia' => $row['Estancia'],
		'Movimiento' => $row['Movimiento'],
		'Distancia' => $row['Distancia'],
	));
}

echo utf8_encode(json_encode($resul));

?>