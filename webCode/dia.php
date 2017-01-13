<?php
$host_db = "127.0.0.1";
$user_db = "prueba";
$password_db ="1234";
$db_name = "arduino";
$tbl_name = "usuario";
$conexion = mysqli_connect($host_db, $user_db, $password_db,$db_name) or die("Cannot Connect to DataBase");

if(mysqli_connect_errno()){
	exit();
}
date_default_timezone_set('Europe/Madrid');
$dia = date('j'); //Eliminar -1 para dia actual
$array[0] = $dia;
$array[1] = 0;

try {
	//$query ="SELECT SUM(POTENCIA) as SENSOR1, SUM(POTENCIA1) AS SENSOR2 FROM sensores WHERE DAY(DATE)=".$dia;
	$query = "SELECT IFNULL(SUM(sensor1.INTENSIDAD),0)  AS INTENSIDAD_SENSOR1, (SELECT IFNULL(SUM(sensor2.INTENSIDAD),0) 
		FROM  sensor2 WHERE DATE(sensor2.DATE) = CURRENT_DATE) 
		AS INTENSIDAD_SENSOR2  FROM  sensor1 WHERE DATE(sensor1.DATE) = CURRENT_DATE";
	$resultado = mysqli_query($conexion,$query);

	if(!mysqli_num_rows($resultado)){
		printf ("Tabla Vacía");
		mysqli_close($conexion);			
	}
	else{		
		$data=array();
		$Potencia = 0;
			while ($row= mysqli_fetch_array($resultado, 1)) {
				$Potencia+= (($row['INTENSIDAD_SENSOR1']/1000) *230.0) + (($row['INTENSIDAD_SENSOR2']/1000) * 230.0);
			}
		$array[1]= $Potencia/3600000; //kW
		$array[0]=($array[0]);
		$data[] = array('DIA' => "Día:".($array[0]),'POTENCIA' =>($array[1]));
		echo json_encode($data);
	}
}
catch(Exception $e){
	die($error);
	$error = "error";
	echo $error;
}
?>
