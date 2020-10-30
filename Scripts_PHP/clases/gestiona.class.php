<?php
	/**bdPDO es la clase que se encarga de la conexión con la bd y de proporcionar los metodos
	necesrios para realizar las peticiones que se requieran por esta clase. Esta interpreta las peticiones 
	y las ejecuta devoliviendo la información pertinente al programa principal
	*/
	require_once ('clases/bdPDO.class.php');
	
	class gestiona
	{
		private $ipdo;

		private $hecho = false;

		private $datos;

		function __construct()
		{
			$this->ipdo = new bdPDO;
			$this->datos = array();
		}
		
		private function Max_id($usuario,&$id)
		{
			$sql="SELECT MAX(id) FROM dispositivo.$usuario";
			$this->datos= $this->ipdo->peticion($sql);
			foreach ($this->datos as $celdas)
				{
					$id = $celdas['MAX(id)'];
				}
			$this->datos = array();
			//echo $id;
		}
		public function inicia_usuario($usuario,$sala_selecc){
			$sql = "INSERT INTO dispositivo.$usuario (id,sala,fecha)
			VALUES (null,$sala_selecc,CURRENT_TIMESTAMP)"; 
			if ($this->ipdo->peticion($sql))
			{
				echo "Se han insertado correctamente los datos." . '<br/r>';
				$this->hecho = true;
			}else
			{
				echo "Se ha producido un error, no se han insertado los datos." .'<br/r>';
			}

			return $this->hecho;			
		}

		public function actualiza_acceso($usuario,$clave,$dist_recorrida,$sala_selecc,$direccion){
			$sql = "INSERT INTO dispositivo.accesos (entrada,usuario,clave_leída,fecha,distancia,sala,direccion)
			VALUES (null,'$usuario','$clave',CURRENT_TIMESTAMP,'$dist_recorrida','$sala_selecc','$direccion')"; 
			if ($this->ipdo->peticion($sql))
			{
				echo "Se han insertado correctamente los datos." . '<br/r>';
				$this->hecho = true;
			}else
			{
				echo "Se ha producido un error, no se han insertado los datos." .'<br/r>';
			}

			return $this->hecho;			
		}
		
		
		public function obten_sala($usuario,&$sala)
		{
			$this->Max_id($usuario,$id);
			$sql = "SELECT sala FROM dispositivo.$usuario where $id = id";
			//echo $sql;
			$this->datos = $this->ipdo->peticion($sql);
			if (!empty($this->datos))
			{
				//echo "Se han obtenido correctamente los datos." . '<br/r>';
				foreach ($this->datos as $celdas)
				{
					$sala = $celdas['sala'];
				}
				//print_r($this->datos);
				echo $sala;
				//echo '<br/r>';
			}
			else
			{
				echo "Se ha producido un error, no se han obtenido los datos." .'<br/r>';
			}
			/*Resetwo de la variable datos*/
			$this->datos = array();
		}


		public function obten_distancia_sentido($clave, $sala_selecc,&$distancia,&$direccion)
		{
			$dist = 'distancia sala ' . $sala_selecc;
			$dir = 'direccion sala ' . $sala_selecc;
			$sql = "SELECT * FROM dispositivo.uid WHERE clave_tarjeta = '$clave'";
			$this->datos = $this->ipdo->peticion($sql);
			if (!empty($this->datos))
			{
				//echo "Se han obtenido correctamente los datos." . '<br/r>';
				foreach ($this->datos as $celdas)
				{
					$distancia = $celdas[$dist];
					$direccion = $celdas[$dir];
				}
				//print_r($this->datos);//funciona ver si inhabilitado devuelve sala y sentido
				print_r($distancia. " " . $direccion);//devuelve distancia y sentido
				//print_r($direccion);
			}
			else
			{
				echo "Se ha producido un error, no se han obtenido los datos." .'<br/r>';
			}
			/*Resetwo de la variable datos*/
			$this->datos = array();
		}

		public function desConectar()
		{
			$this->ipdo = null;
		}

	} //fin de la clase

?>