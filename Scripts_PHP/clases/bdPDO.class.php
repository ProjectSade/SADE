<?php

	/** Clase para usar la base de datos usando POO
	 * adaptada de https://github.com/padrecedano/PHP-PDO
	 */
	class bdPDO
	{
		private $pdo; /* Objeto PDO para la conexión a la BD*/

		private $msql; /* Objeto para SQL con la BD*/
		
		private $credenciales; /* Array con datos autenticación para la conexión a la BD*/

		private $conectado = false; /**Booleano para ber si la conexión est´establecida */

		private $parametros; /* Parametros para la consulta SQL*/

		/*Constructor de la clase*/

		function __construct()
		{
			$this->conecta();
			$this->parametros=array();
		}


		/*Función que gestiona la conexión avla BD*/
		private function conecta()
		{
			$this->credenciales = parse_ini_file(".credenciales/bd.ini.php"); /*Asigna a credenciales los datos del archivo para la autenticación*/
			$dsn = $this->credenciales["tipo_bd"] .':dbname'.$this->credenciales["bdname"].';host='.$this->credenciales["host"].'';
			$pwd = $this->credenciales["pass"];
			$usr = $this->credenciales["user"];

			/**
     *	El array $options es muy importante para tener un PDO bien configurado
     *	
     *	1. PDO::ATTR_PERSISTENT => false: sirve para usar conexiones persistentes
     *      se puede establecer a true si se quiere usar este tipo de conexión.
     *      Aunque en la práctica, el uso de conexiones persistentes podría ser problemático
     *	2. PDO::ATTR_EMULATE_PREPARES => false: Se usa para desactivar emulación de consultas preparadas 
     *      forzando el uso real de consultas preparadas. 
     *      Es muy importante establecerlo a false para prevenir Inyección SQL.
     *	3. PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION También muy importante para un correcto manejo de las excepciones. 
     *      Si no se usa bien, cuando hay algún error este se podría escribir en el log revelando datos como la contraseña !!!
     *	4. PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES 'utf8'": establece el juego de caracteres a utf8, 
     *      evitando caracteres extraños en pantalla.
     */

			$options = array
			(
			PDO::ATTR_PERSISTENT => false, 
            PDO::ATTR_EMULATE_PREPARES => false, 
            PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION, 
            PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES 'utf8'"
			);

	/*Intento de conexión entre try y catch para recoger posibles excepciones*/
			try{ 
				$this->pdo = new PDO($dsn,$usr,$pwd,$options);/*En la clase ejemplo tiene un error aqui y no instanciaba el objeto $pdo*/
				$this->conectado = true;
			}catch (PDOException $e){
	/*Archivo log para registrar excepdiones*/
				error_log($this->error = $e->getMessage(),0);
			}
	}

	/*Metodo para cerrar la conexión con la bd*/

	public function desConectar()
	{
		$this->pdo = null;
	}

/**
Las sentencias preparadas ofrecen dos grandes beneficios:

La consulta sólo necesita ser analizada (o preparada) una vez, pero puede ser ejecutada muchas veces con los mismos o diferentes parámetros. Cuando la consulta se prepara, la base de datos analizará, compilará y optimizará su plan para ejecutarla. Para consultas complejas, este proceso puede tomar suficiente tiempo como para ralentizar notablemente una aplicación si fuera necesario repetir la misma consulta muchas veces con los mismos parámetros. Mediante el empleo de una sentencia preparada, la aplicación evita repetir el ciclo de análisis/compilación/optimización. Esto significa que las sentencias preparadas utilizan menos recursos y se ejecutan más rápidamente.
Los parámetros para las sentencias preparadas no necesitan estar entrecomillados; el controlador automáticamente se encarga de esto. Si una aplicación usa exclusivamente sentencias preparadas, el desarrollador puede estar seguro de que no hay cabida para inyecciones de SQL (sin embargo, si otras partes de la consulta se construyen con datos de entrada sin escapar, aún es posible que ocurran ataques de inyecciones de SQL).
*/
	
	/* Funcion que sirve para asignar a un parametro su valors*/

	public function bind($parametro,$valor)
	{
		$this->parametros[sizeof($this->parametros)] = [":",$parametro,$valor];
		//retunr $this->parametros; /*para comprobar, tras pruebas eliminar*/
	}

	/* Crea una array asociando a cada parametor su valor*/

	public function bindParams($paramarray)
	{
		if (empty($this->parametros) && is_array($paramarray))
		{
			/*Asigna las claves del array a columnas*/
			$columnas = array_keys($paramarray);
			foreach ($columnas as $i => &$columna) {
				$this->bind($columna, $paramarray[$columna]);
			}
		}

	}

	/**
	
	*/
	public function hayConexion(){
		$conexion = $this->conectado;
		return $conexion;
	}

	private function gestionaPeticion($sql,$parametros = "")
	{	
		if(!$this->conectado) /*Comprobar si ya se esta conectado, sino se conecta*/
		{
			$this->conecta();
		}

	
		try{
			/*Preparar la consulta*/
			$this->msql = $this->pdo->prepare($sql);
			/*Asignar parametros*/
			$this->bindParams($parametros);
			/*Asignar parametros por el tipo de valor para la consulta*/
			if (!empty($this->parametros))
			{
				/**Se añade el tipo de parametro para la funcion bindValue dependiendo del valor del parametro
				foreach (expresión_array as $valor)
    			sentencias
				foreach (expresión_array as $clave => $valor)
    			sentencias
				La primera forma recorre el array dado por expresión_array. En cada iteración, el valor del elemento actual se asigna a $valor y el puntero interno del array avanza una posición (así en la próxima iteración se estará observando el siguiente elemento).
				La segunda forma además asigna la clave del elemento actual a la variable $clave en cada iteración.
				Ver:https://www.php.net/manual/es/control-structures.foreach.php
				*/
				foreach ($this->parametros as $param => $value)
				{

					if(is_int($value[1])){

						$tipo = PDO::PARAM_INT;

					}else if (is_bool($value[1])){

						$tipo = PDO::PARAM_BOOL;

					}else if (is_null($value[1])){

						$tipo = PDO::PARAM_NULL;

					}else{

						$tipo = PDO::PARAM_STR;

					}
				/**
				Vincula un valor al parámetro de sustitución con nombre o de signo de interrogación de la sentencia SQL que se utilizó para preparar la sentencia.
				Ver:https://www.php.net/manual/es/pdostatement.bindvalue.php
				*/		
				$this->msql->bindValue($value[0],$value[1],$tipo);
				}
		
			}

			$this->msql->execute();
		}catch(PDOException $e){
			error_log($this->error = $e->getMessage(). "\nSQL: ".$sql."\n",0);
		}
		/*Reset de los parametros*/
		$this->parametros = array();
	}


	public function peticion($sql,$params = null,$fetchmode = PDO::FETCH_ASSOC)
	{
		/**
		trim — Elimina espacio en blanco (u otro tipo de caracteres) del inicio y el final de la cadena
		Ver:https://www.php.net/manual/es/function.trim.php
		str_replace — Reemplaza todas las apariciones del string buscado con el string de reemplazo
		Ver:https://www.php.net/manual/es/function.str-replace.php
		*/
		$sql = trim(str_replace("\r"," ",$sql));
		$this->gestionaPeticion($sql,$params);
		/**
			explode — Divide un string en varios string
			Ver;https://www.php.net/manual/es/function.explode.php
			preg_replace — Realiza una búsqueda y sustitución de una expresión regular
			Ver:https://www.php.net/manual/es/function.preg-replace.php
			Elimina espacios en blanco, tabulaciones y saltos de linea en el string por un solo espacio en blnnco
		*/
		$sql_en_crudo = explode(" ",preg_replace("/\s+|\t+|\n+/", " ", $sql));
		/**
			Determina si el tipo de consulta INSERT, UPDATE, SELECT...
			strtolower — Convierte una cadena a minúsculas
			Ver:https://www.php.net/manual/es/function.strtolower.php
		*/
		$stmt = strtolower($sql_en_crudo[0]);

		if ($stmt === 'select' || $stmt === 'show') {
            return $this->msql->fetchAll($fetchmode);
        } elseif ($stmt === 'insert' || $stmt === 'update' || $stmt === 'delete') {
            return $this->msql->rowCount();
        } else {
            return NULL;
        }
	}

		
	}/*Fin de la clase*/


?>
