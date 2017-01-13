<?php
$host_db = "127.0.0.1";
$user_db = "prueba";
$password_db ="1234";
$db_name = "arduino";
$tbl_name = "usuario";
$data=array();
$conexion = mysqli_connect($host_db,$user_db, $password_db,$db_name) or die("Cannot Connect to DataBase");

try{
	//$query = "SELECT INTENSIDAD, POTENCIA FROM sensores ORDER BY TIME DESC LIMIT 1";
	$query = "SELECT  SENSOR1.INTENSIDAD AS INTENSIDAD from SENSOR1 ORDER BY SENSOR1.TIME DESC limit 1";
	$resultado = mysqli_query($conexion,$query);

	if(mysqli_num_rows($resultado) == 0){
			$error = "error";
			echo $error;	
	}
	else{
		while ($row= mysqli_fetch_array($resultado, 1)) {
			$Intensidad = $row['INTENSIDAD'];
			$Potencia  = ($Intensidad / 1000) * 230.0;
		}
		$data[] = array('POTENCIA' =>($Potencia),'INTENSIDAD' => ($Intensidad));
		echo json_encode($data);	
		}
	}
catch(Exception $e){
	die($error);
	$error = "error";
	echo $error;
}
?>

