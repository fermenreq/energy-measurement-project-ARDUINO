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
$hora = date('G'); //Eliminar -1 para dia actual
$array[0] = $hora;
$array[1] = 0;
try {
	//$query = "SELECT SUM(POTENCIA) as SENSOR1, SUM(POTENCIA1) AS SENSOR2 FROM sensores WHERE DATE = CURRENT_DATE AND HOUR(TIME) =".$hora;
	$query = "SELECT IFNULL(SUM(sensor1.INTENSIDAD),0)  AS INTENSIDAD_SENSOR1, (SELECT IFNULL(SUM(sensor2.INTENSIDAD),0)
			 FROM  sensor2 WHERE HOUR(sensor2.TIME) =".$hora.") AS INTENSIDAD_SENSOR2  FROM  sensor1 WHERE HOUR(sensor1.TIME)=".$hora;
	$resultado = mysqli_query($conexion,$query);

	if(!mysqli_num_rows($resultado)){
		$html.='<h2>No existen datos</h2>';
		mysqli_close($conexion);			
	}
	else{		
		$data=array();
		$Potencia = 0;
			while ($row= mysqli_fetch_array($resultado, 1)) {
				$potenciaSensor1 = $row['INTENSIDAD_SENSOR1'] * 230.0;
				$potenciaSensor2 = $row['INTENSIDAD_SENSOR2'] * 230.0;
				$Potencia += $potenciaSensor1/1000 + $potenciaSensor2/1000;	
			}
		$array[1]= ($Potencia/3600000);
		$data[] = array('HORA' => ($array[0]).":00 horas",'POTENCIA' =>($array[1]));
		echo json_encode($data);
	}
}
catch(Exception $e){
	die($error);
	$error = "error";
	echo $error;
}
?>
