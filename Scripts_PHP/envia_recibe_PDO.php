<?php
	/**
	programa principal recibe las peticiones de los usuarios, se encarga de formailzarlas y les devuelve los resultados
	*/
	require_once('clases/gestiona.class.php');
	/**
	Se recibiran peticiones via POST, ejemplo 1 inserta, 2 consulta, 3 ambas y dependiendo de la solicitud se hará una consulta o una inserccion,
	o dependiendo del caso las dos a la vez
	La sala en un principio la mandará la app cuando la seleccione el usuario,
	el dispositivo la leerá, este cada vez que lea una RFID insertará los datos en la tabla
	accesos y leerá de la tabla uid el giro en la próxima intersección y la distancia que 
	le queda.
	*/

	/**
	Esta parte se encarga de introducir el codigo y la sala al iniciar el acceso, para que 
	el dispositivo sepa a donde quiere ir el usuario y si debe de iniciarse el 
	*/

	$peticion = $_POST['pet']; //variable selecciona que se quiere hacer
	
	switch ($peticion){
		case 'inserta sala': //como prueba, hasta tener la app, intro la sala y el usuario
		echo "Opción 1." . "<br/r>";
		if (isset($_POST['user']) && isset($_POST['sala']))
		{
			$usuario = $_POST['user'];
			$sala_selecc = $_POST['sala'];
			$mifaz = new gestiona;
			if ($hecho=$mifaz->inicia_usuario($usuario,$sala_selecc)){
				echo "Ha ido bien";
			}
			$mifaz->desConectar();
		}else{
			echo "Error: las variables no contienen datos. ";
		}
		break;

		case 'inserta acceso':	//inserta datos itinerario del usuario
		echo "Opción 2." . "<br/r>";
		if (isset($_POST['user']) && isset($_POST['uid']) && isset($_POST['dist']) && isset($_POST['sala']) && isset($_POST['dir']))
		{
			$usuario = $_POST['user'];
			$clave = $_POST['uid'];
			$dist_recorrida = $_POST['dist'];
			$sala_selecc = $_POST['sala'];
			$direccion = $_POST['dir'];
			$mifaz = new gestiona;
			$mifaz->actualiza_acceso($usuario,$clave,$dist_recorrida,$sala_selecc,$direccion);
			$mifaz->desConectar();
		}else{
			echo "Error: las variables no contienen datos. ";
		}
		break;

		case 'distancia y sentido':	//el dispositivo obtiene estos datos para encaminarse
		//echo "Opción 3." . "<br/r>";
		if(isset($_POST['uid']) && isset($_POST['sala']))
		{
			$clave = $_POST['uid'];
			$sala_selecc = $_POST['sala']; 
			$mifaz = new gestiona;
			$distancia;
			$direccion;
			$mifaz->obten_distancia_sentido($clave,$sala_selecc,$distancia,$direccion);
			$mifaz->desConectar();
		}else{
		echo "Error: las variables no contienen datos. ";
		}
		break;

		case 'sala':
		//echo "Opción 4." Obtiene la sala a donde el usuario quiere ir
		
		if(isset($_POST['user']))
		{
			$usuario = $_POST['user'];
			$mifaz = new gestiona;
			$sala_ant;
			$sala;
			//$mifaz->Max_id($usuario);
			$mifaz->obten_sala($usuario,$sala);
			$mifaz->desconectar();
		}else{
			echo "Error: la variable no está definida. ";
		}
		break;

		default:
		echo "Algo ha fallado, revise los datos introducidos.";
	//$mifaz->inserta($usuario,$clave,$dist_recorrida,$sala_selecc); 
	}//fin case
	
	
?>