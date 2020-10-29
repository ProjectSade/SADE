/*Proyecto Fin Grado de Ricardo Mateos Robleda y Jaime Román Vico                     */
/*Sketch Arduino que lee una clave RFID la envia a un servidor y recibe instrucciones */
/*del servidor para saber hacia donde debe girar en las intersecciones                */
/*Se usa un nucleo para comunicacion y el otro para sensores*/
/*Autor: Ricardo Mateos Robleda, alumno de la ETSISI UPM*/


/*Librerias necesariaas*/
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <FreeRTOS.h>



/*Clase que gestiona la conexión wifi, el envío y recepción de mensajes con el servidor*/
class wifi{
  private:
  /*constantes*/
  const char*  ssid = "nombre_red";
  const char* password = "contraseña";
  /*variables*/
  String url;
  
  
  public:
  /*constructor*/
  wifi(String url)
  {
    this->url = url;
  }
  /*Metodo que gestiona la conexion a la red wifi*/
  void conectar() {
  Serial.begin(115200);
  WiFi.begin(ssid,password);  
  Serial.println("Conectndo...");
  while (WiFi.status() !=  WL_CONNECTED){
    delay(500); //Espera medio segundo
    Serial.print(".");
  }
  Serial.print("Conectado con exito, mi IP es:");
  Serial.println(WiFi.localIP());
}

/*Metodo que se encarga del envio y la recepcion de datos*/
bool insertar(String datos) {
bool exito = false;
if (WiFi.status() ==  WL_CONNECTED) {
//Variable para comunicacion  
HTTPClient client;     
//String con los datos a enviar
client.begin(url);
client.addHeader("Content-Type","application/x-www-form-urlencoded");

//Respuesta servidor
int respuesta = client.POST(datos);

if (respuesta==200)
{
    exito = true;
}else{
  Serial.println("Error en el envio de datos, código: " + respuesta );
}
client.end();

}else
{
  Serial.println("Fallo en la conexión wifi.");
}
return exito;
}//Fin insertar

/*Metodo que gestiona las peticiones a la base de datos*/
String consultar(String datos) {
  String respuesta;
  if (WiFi.status()==WL_CONNECTED) {
    HTTPClient client; // var para comuicación
    client.begin(url);
    client.addHeader("Content-Type","application/x-www-form-urlencoded"); //configuración mensaje
    //envio y recepción
    int cod_respuesta = client.POST(datos);
    //comienza el procesado de la respuesta
    if(cod_respuesta==200){
        respuesta = client.getString();
    } else
    {
        Serial.println("Error en el envio de POST, codigo: " + cod_respuesta);
    }
    //fin de la conexión
    client.end();
  }//fin 1 if
  else{
      Serial.println("Fallo la conexión WIFI.");
  }
  return respuesta;
}//fin consultar



}; /*Fin clase wifi*/

/**Clase que gestiona la lectura de las claves de tarjets RFID
 * Devuelve la clave en formato String hexadecimal
*/

class uid{
  private: 
  MFRC522 lector;
  public:
  /*Constructor de la clase*/
  uid(MFRC522 lector ){
    this->lector = lector;
    init();
  }

/*Inicia los dispositivos necesarios*/
void init() {
  Serial.begin(115200);
  SPI.begin();
  lector.PCD_Init();
}

/*Transforma la clave a formato String*/
String keyToString(byte *buffer, byte bufferSize){
	String dato;
  for (byte i = 0; i < bufferSize; i++ ){
      if (buffer[i] < 0x10){
        dato = dato + " 0";
      }
      else {
        dato = dato + " ";
      }
      dato = dato + String(buffer[i],HEX);
  }
  dato.trim();
  return dato;
  }

/*Lee la clave RFID cuando detecta una tarjeta*/
String lee(){
  String clave;
  if (lector.PICC_IsNewCardPresent()){
    if (lector.PICC_ReadCardSerial()){
      clave = keyToString(lector.uid.uidByte,lector.uid.size);
      lector.PICC_HaltA();
    }
  }
  delay(1000);
  return clave;
}
}; /* Fin clase*/

/*Main*/

/*Pinout comunicacion SPI*/
const int SS_PIN = 21;
const int RST_PIN = 22;

/*variables clase uid*/
MFRC522 lector(SS_PIN,RST_PIN);
uid clave(lector);
String la_clave = "Vacia";

/*variables uso clase wifi*/
String url = "http://192.168.1.9/htdocs/envia_recibe_PDO.php";
String pedir_dist_sent = " ";
String usuario = "prototipo";
String sala = "En inicio";
String respuesta = "En blanco";
float dist = 0; /*Variable para que el contador simule distancia recorrida*/
float dist_sala = 0; /*Variable para saber la distancia a la sala*/
int sent; /*variable que recibe el sentido del giro*/
bool iniciado = false; /*variable para detectar si se ha iniciado */
bool nueva_clave = false; /*para saber si es la misma tarjeta*/
bool insertado = false; /*Controla si los datos se han insertado*/
bool llegado = false; /*variable para detectar si se ha llegado*/
wifi conn(url); /*Paso de la url para el envio y recepcion datos*/

/*variables mutex y multitarea*/
SemaphoreHandle_t lee_clave,actualiza;
TaskHandle_t Tarea_RFID, Tarea_WIFI;

/*Simula el decremento de la distancioa a una velocidad de 5m/s */
void simula_velocidad(float &dist){
      dist = dist + 0.30;   
}

//Obtiene la sala a la que dirigirse
bool inicio(){
  String peticion ="pet=sala&user=" + usuario;
  bool inicio = false;
  if (inicio==false)
   {   
      sala=conn.consultar(peticion);
   }
   if (!sala.isEmpty()){
     inicio = true;
     llegado = false;
   }  
  return inicio;
}

//Transforma el array en 2 valores y los asigna a dos variables
void procesa_respuesta(String respuesta, float &dist_sala, int &sent){
  String dis;
  String sen;
  dis = respuesta[0];
  sen = respuesta[2];
  dist_sala = dis.toFloat();
  sent = sen.toInt();
}


void loop_Tarea_RFID(void *parameter){
  //lo que se ejecuta antes del for se hace secuencial
  for(;;)
  {
    if(iniciado){
    xSemaphoreTake(lee_clave, portMAX_DELAY); //entra en la RC
    la_clave=clave.lee(); 
    Serial.println("La clave es: " + la_clave);  
    xSemaphoreGive(lee_clave); //sale de la RC
    }
    else{
      iniciado=inicio();
    }
    if (dist_sala>0 && !llegado){//inicia silulacion velocidad
        xSemaphoreTake(actualiza,portMAX_DELAY);
        simula_velocidad(dist);
        xSemaphoreGive(actualiza);
    }
    if(dist>dist_sala && llegado){
        xSemaphoreTake(actualiza,portMAX_DELAY);
        Serial.println("estado de la variable ha llegado: " + (String)llegado);
        dist = 0;
        sala = "En blanco";
        dist_sala = 0;
        iniciado=inicio(); /*vuelve a iniciar la lectura de otra sala*/  
        xSemaphoreGive(actualiza);      
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); //Espera 2000 tics o 2 segundos
  }
}

void loop_Tarea_WIFI(void *parameter){
//lo que se ejecuta antes del for se hace secuencial
String clave;
for(;;){
  Serial.println("Soy la tarea wifi ejecutandome en el nücleo: " + (String)xPortGetCoreID());
  xSemaphoreTake(lee_clave, portMAX_DELAY); //entra en la RC
  if (!la_clave.isEmpty()){
      String pide_dist_sent ="pet=distancia y sentido&uid=" + la_clave + "&sala=" + sala;
      Serial.println(pide_dist_sent);
      respuesta = conn.consultar(pide_dist_sent);
      procesa_respuesta(respuesta,dist_sala,sent);
      /*trazas para el seguimiento de la ejecución*/
      Serial.println("Distancia a nueva intersección: " + (String)dist_sala);
      Serial.println("Sentido a tomar en la próxima intersección: " + (String)sent);
      clave = la_clave;
      Serial.println("Soy la copia de la clave: " + clave);
  }
  xSemaphoreGive(lee_clave); //sale de la RC*/
  xSemaphoreTake(actualiza,portMAX_DELAY);
  if (dist<dist_sala){
    String inserta ="pet=inserta acceso&user=" + usuario + "&uid=" + clave + "&dist=" + dist + "&sala=" + sala + "&dir=" + sent;
    Serial.println("Inserta: " + inserta);
    insertado = conn.insertar(inserta);
    Serial.println("Respuesta a inserta: " + (String)insertado);  
  }  
  if (dist>dist_sala && !llegado){ /*Inserta parar en la base de datos*/
    String inserta ="pet=inserta acceso&user=" + usuario + "&uid=" + clave + "&dist=" + dist + "&sala=" + sala + "&dir=" + 0;
    insertado = conn.insertar(inserta);
    llegado = true; /*Indica que se ha llegado*/
  }    
  xSemaphoreGive(actualiza);
  
  vTaskDelay(pdMS_TO_TICKS(1000)); //Espera 2000 tics o 2 segundos

}//fin for

}//fin loop

/*Inicio y configuracion*/
void setup() {
  Serial.begin(115200);
  delay(1000); //Da un segundo para que se inicie el monitor serie
  conn.conectar();
  delay(1000); //Da un segundo para que se inicie la conexión wifi
  iniciado = inicio();
  delay(1000); //Da un segundo para que se inicie el resto de tareas
    
  lee_clave = xSemaphoreCreateMutex(); /*inicio variable mutex*/  
  if (lee_clave == NULL){
    Serial.println("No se ha creado la variable de condicion para clave.");
  }
  actualiza = xSemaphoreCreateMutex();
  if (actualiza == NULL){
    Serial.println("No se ha creado la variable de condicion para insetar.");
  }

  //parametros tarea 1
 xTaskCreatePinnedToCore(  //Es indistinto ponerlo antes o despues del inicio del monitor serie
  loop_Tarea_RFID, //Funcion o preocedimiento a ejecutar
  "Tarea_RFID", //Nombre de la tarea
  3000,   //Tam. de la pila
  NULL,  //parametros a pasar a la funcion o procedimiento
  1, //Prioridad de ejecucion
  &Tarea_RFID, //Direccion de memoria de la tarea variable TaskHndle_t
  1 );//nucleo donde se quiere ejecutar la tarea

  
 //parametros tarea 2  
xTaskCreatePinnedToCore(  //Es indistinto ponerlo antes o despues del inicio del monitor serie
  loop_Tarea_WIFI, //Funcion o preocedimiento a ejecutar
  "Tarea_WIFI", //Nombre de la tarea
  3000,   //Tam. de la pila
  NULL,  //parametros a pasar a la funcion o procedimiento
  1, //Prioridad de ejecucion
  &Tarea_WIFI, //Direccion de memoria de la tarea variable TaskHndle_t
  0 );//nucleo donde se quiere ejecutar la tarea
 } // Fin setup 





void loop() {
 /*proceso ocioso para mostrar que se usan ambos nucleos*/
  Serial.println("Soy el bucle principal y uso el nucleo: " + String(xPortGetCoreID()));
  delay(1500);
}
  