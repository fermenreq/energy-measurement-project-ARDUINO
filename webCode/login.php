<head>
	<title>Check Login</title>
	<meta charset="UTF-8">
</head>
<body>
	
<?php
error_reporting(E_ERROR);
$host_db = "127.0.0.1";
$user_db = "prueba";
$password_db ="1234";
$db_name = "arduino";
$tbl_name = "usuario";
$conexion = mysqli_connect($host_db, $user_db, $password_db,$db_name) or die("Cannot Connect to DataBase");

if(mysqli_connect_errno()){
	exit();
}

$username = mysqli_real_escape_string($conexion,$_POST["email"]);
$password = mysqli_real_escape_string($conexion,$_POST["password"]);
$query = "SELECT * FROM usuario where USUARIO ='$username'AND CONTRASENA ='$password'";
$resultado = mysqli_query($conexion,$query);
if(mysqli_num_rows($resultado) > 0){
		header("Location: http://localhost/admin.html");
}
else{
		header("Location: http://localhost/error.html");
		echo "<p>Usuario o contraseña incorrecto</p>";
}
mysqli_close($conexion);	
?>
</body>


