<?php
$host_db = "127.0.0.1";
$user_db = "prueba";
$password_db ="1234";
$db_name = "arduino";
$tbl_name = "usuario";
$conexion = mysqli_connect($host_db, $user_db, $password_db,$db_name) or die("Cannot Connect to DataBase");

try{
    $query="SELECT YEAR(CURRENT_DATE) AS ANIO, MONTH(CURRENT_DATE) AS MES, IFNULL(SUM(sensor1.INTENSIDAD),0) AS INTENSIDAD_SENSOR1, (SELECT IFNULL(SUM(sensor2.INTENSIDAD),0) 
            FROM sensor2 WHERE MONTH(sensor2.DATE) = MONTH(CURRENT_DATE) 
            AND YEAR(SENSOR2.DATE) = YEAR(CURRENT_DATE))  AS INTENSIDAD_SENSOR2 
            FROM sensor1 WHERE MONTH(sensor1.DATE) = MONTH(CURRENT_DATE) AND YEAR(SENSOR1.DATE) = YEAR(CURRENT_DATE)";

    $resultado = mysqli_query($conexion,$query);

    if(!mysqli_num_rows($resultado)){
        $error = "error, no hay datos";
        echo $error;
    }
    else{
        $array[1] = 0;
        $data = array();
        $meses = array(1 => "Enero",2 => "Febrero",3 => "Marzo",4 => "Abril",5 => "Mayo",
        6 => "Junio", 7 => "Julio", 8  => "Agosto", 9   => "Septiembre",10 => "Octubre",
        11 => "Noviembre", 12 => "Diciembre", );
        $Potencia = 0;
        while ($row= mysqli_fetch_array($resultado, 1)) {
                $Potencia+= (($row['INTENSIDAD_SENSOR1'])/1000 * 230.0) + (($row['INTENSIDAD_SENSOR2'])/1000 * 230.0);
                $Mes = $meses[$row['MES']]."/".$row["ANIO"];
                $array[1] = $Mes; 
                $data[] = array('MES' =>$array[1],'POTENCIA' =>$Potencia / 3600000);
        }       
        echo json_encode($data);
    }
}
catch(Exception $e){
    die($error);
    $error = "error";
    echo $error;
}
?>
