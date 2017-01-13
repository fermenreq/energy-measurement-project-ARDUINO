#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include <mysql.h>
#include <stdlib.h>

//Pin sensores 
int sensor1 = A5;
int sensor2 = A4;

//Consumo
int muestras = 1000;
char datos [2048];
float sensorValue_aux = 0.0;
float sensorValue_aux1 = 0.0;
float valorCorriente = 0.0;
float valorSensor = 0.0;
float voltsporUnidad = 0.004887586; //Conversión ADC 5/1023
float sensibilidad = voltsporUnidad * 10;  //Sensibilidad para sensor 20A
float sensibilidad5A = voltsporUnidad * 5.4054; //Sensibilidad para sensor 5A usada para calculo de la potencia
float tension = 230.0; //Tensión de la zona. En este caso 230V
float pot = 0.0;
int intensidadQuery = 0;

//Bluetooth
int estado = 0;
int res[2];
int * value;
#define rxPin 11
#define txPin 12

SoftwareSerial BT1(rxPin,txPin);

//Definicion Pines de los Reles
const int pinRele1 = 3;
const int pinRele2 = 2;

//Estado de los Leds
const int led1 = 13;
const int led2 = 12;

//SoftwareSerial BT1(1,0); // RX, TX
//#define BT1 Serial
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//Defincion conexion MySQL
IPAddress mysql_server(192,168,1,51);    //Ip servidor 
IPAddress ethernet_shield(192,168,1,10); //Ip arduino
char user[] = "usuario"; //Usuario Base de datos
char password[]= "1234";   //Contraseña 
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
Connector arduino_conn; //Objeto conector
int estadoConexion = 0;
void setup() {
  Serial.begin(9600);
  pinMode(pinRele1,OUTPUT);
  pinMode(pinRele2,OUTPUT);
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  BT1.begin(9600);
  lcd.begin(16,2);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  lcd.print("** BIENVENIDO **");
  digitalWrite(led1,HIGH);
  digitalWrite(led2,HIGH);
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("Cargando...");
  estadoConexion = Conexion();
  delay(2000);
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  lcd.clear();
  
}
 
void loop() {
  int * value = bluetooth();
  displayMenu(value);
}
  
 void displayMenu(int value[]){
     int enchufe1= value[0];
     int enchufe2= value[1];
     if(enchufe1==0){
      if(enchufe2==0){
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Esperando orden");
        delay(1000);
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enchufe 2 ON");
        lcd.setCursor(0,1);
        lcd.print("Leyendo datos..");
        digitalWrite(led1,HIGH);
        delay(1000);
      }
     }
     else{
      if(enchufe2==0){
        Serial.println("Enhufe1 Activado");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enchufe 1 ON");
        lcd.setCursor(0,1);
        lcd.print("Leyendo datos..");
        digitalWrite(led2,HIGH); 
        delay(1000);
      }
      else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enchufes 1-2 ON");
        lcd.setCursor(0,1);
        lcd.print("Leyendo datos..");
        digitalWrite(led1,HIGH);
        digitalWrite(led2,HIGH);
        delay(1000);
      }
     }
     if(enchufe1==1 and enchufe2==0){ //Enchufe 1 ON, Enchufe2 OFF
        int valor = 1;
        consumoIndividual(valor,estadoConexion);
     }
     if(enchufe2==1 and enchufe1==0){ //Enchufe 1 OFF, Enchufe2 ON
        int valor = 2;
        consumoIndividual(valor,estadoConexion);
     }
     if(enchufe1==1 and enchufe2==1){ //Enchufe 1 ON, Echufe 2 ON
        consumoTotal(enchufe1,enchufe2);
     }
 }
void consumoIndividual(int enchufe, int conectado){ 
  //Enchufe 1 con sensor modelo 5A
   if(enchufe==1){
    //Consulta base de datos Planta1
   lcd.setCursor(0,1);
   lcd.print("Leyendo datos...");
   for(int i=0; i<muestras; i++){
    sensorValue_aux = sensibilidad5A * (analogRead(sensor1) - 510); 
    if(sensorValue_aux < 0 ) sensorValue_aux = - sensorValue_aux;  //Rectificacion de la componente - AC
     valorSensor = valorSensor + sensorValue_aux / float(muestras); //Voltaje promedio de AC rectificada
    }
    valorCorriente = 1.1107 * valorSensor; 
    if(valorCorriente <= 0.05){ //+-0.05 aprox. valor experimental
      valorCorriente = 0.000;
    }
    Serial.println(valorCorriente);
    lcd.clear();
    lcd.setCursor(0,0);
    //Tratamos error
    valorSensor = 0;
    Serial.println(valorCorriente);
    lcd.clear();
    lcd.setCursor(0,0);
    pot = valorCorriente * tension;
    intensidadQuery = 0 + valorCorriente * 1000;
    if(conectado == 1){
        if(pot != 0.00 || intensidadQuery != 0){
          lcd.print("Guardando..."); 
          char * Query = "INSERT INTO sensor1 (INTENSIDAD,DATE,TIME) VALUE (%d,CURRENT_DATE,CURRENT_TIME)";  
          sprintf(datos,Query,intensidadQuery);
          Serial.println("Datos");
          Serial.println(datos);
          arduino_conn.cmd_query("use arduino");//Usamos tabla arduino
          arduino_conn.cmd_query(datos);
          lcd.clear();
        }
    }
    lcd.print("Corriente:");
    lcd.print(valorCorriente,3);
    lcd.print("A");
    //Cambiamos de cursor
    lcd.setCursor(0,2);
    lcd.print("Consumo:");
    lcd.print(pot);
    lcd.print("W");
    lcd.setCursor(0,3);
    valorSensor = 0;
		valorCorriente = 0;
    delay(2500);
   }
   else{
    //Enchufe 2 con sensor modelo 20
   lcd.setCursor(0,1);
   lcd.print("Leyendo datos...");
    for(int i=0; i<muestras; i++){ //Sensor2 Rele 2
      sensorValue_aux = sensibilidad * (analogRead(sensor2) - 510); 
      if(sensorValue_aux < 0 ) sensorValue_aux = - sensorValue_aux;
      valorSensor = valorSensor + sensorValue_aux / float(muestras);
    }
    valorCorriente = 1.1107 * valorSensor;
    if(valorCorriente <= 0.06){  //+-0.06 aprox. valor experimental
      valorCorriente = 0.000;
    }
    lcd.clear();
    lcd.setCursor(0,0);
    pot = valorCorriente * tension;
    intensidadQuery = valorCorriente * 1000;
    if(conectado ==1){
        if(pot != 0.00 || intensidadQuery != 0){
        lcd.print("Guardando..."); 
        char * Query = "INSERT INTO sensor2 (INTENSIDAD,DATE,TIME) VALUE (%d,CURRENT_DATE,CURRENT_TIME)";  
        sprintf(datos,Query,intensidadQuery);
          //Usamos tabla arduino 
        arduino_conn.cmd_query("use arduino");
        arduino_conn.cmd_query(datos);
        Serial.println("Inserting data...");
        Serial.println(datos);
        lcd.clear();
        }
    }
    lcd.print("Corriente:");
    lcd.print(valorCorriente,3);
    lcd.print("A");
    //Cambiamos de cursor
    lcd.setCursor(0,2);
    lcd.print("Consumo:");
    lcd.print(pot);
    lcd.print("W");
    lcd.setCursor(0,3);
    valorCorriente = 0;
    valorSensor = 0;
    delay(2500);
    lcd.clear();
    lcd.print("Guardando....");
    delay(2500);
   }
}
int * bluetooth(){

 if(BT1.available()){            
        estado = BT1.read();
 }
 if(estado == '1'){     
       digitalWrite(pinRele1,HIGH);
       res[0] = 1;
 }
 if(estado == '2' ){                
       digitalWrite(pinRele1,LOW);
       res[0] = 0;
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Enchufe 1 OFF");
       digitalWrite(led1,LOW);
       delay(1000);
       lcd.clear();
 }
 if(estado == '3'){                
       digitalWrite(pinRele2,HIGH);
       res[1]=1;
 }
 if(estado == '4'){              
       digitalWrite(pinRele2,LOW);
       res[1]= 0;
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Enchufe 2 OFF");
       digitalWrite(led2,LOW);
       delay(1000);
       lcd.clear();
 }
 return res;
 }
//Calcula potencia total de los 2 enchufes
void consumoTotal( int enchufe1, int enchufe2){
   //Consumo Enchufe1  y Enchufe2
   if(enchufe1 == 1 and enchufe2 == 1){ 
    for(int i=0; i<muestras; i++){
      sensorValue_aux = (sensibilidad5A * (analogRead(sensor1) - 510)) + (sensibilidad * (analogRead(sensor2) - 510));
      if(sensorValue_aux < 0 ) sensorValue_aux = - sensorValue_aux;
        valorSensor = valorSensor + sensorValue_aux  / double(muestras);
      }
    valorCorriente = 1.1107 * valorSensor;
    if(valorCorriente <= 0.08){
      valorCorriente = 0.00;
    }
    valorSensor = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    int pot = valorCorriente * tension;
    lcd.print("Corriente:");
    lcd.print(valorCorriente,3);
    lcd.print("A");
    //Cambiamos de cursor
    lcd.setCursor(0,2);
    lcd.print("Consumo:");
    lcd.print(pot);
    lcd.print("Watts");
    lcd.setCursor(0,3);
    delay(5000);
   }
}
int Conexion(){
  Ethernet.begin(mac_addr, ethernet_shield);
  int res = 0;
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,2);
  lcd.print("Conectando...");
  if(arduino_conn.mysql_connect(mysql_server,3306,user,password)){
    res= 1;
    lcd.clear();
    lcd.print("Conectado.");
    delay(1000);
  }
  else{
    res = 0;
    lcd.clear();
    lcd.print("Sin conexion");
    delay(1000);
  }
  return res;
}




