#include <Arduino.h>           /* Libreris para usar la funcionalidad de arduino */
#include <SPI.h>               /* Libreria para configurar el bus SPI */
#include <MFRC522.h>           /* Libreria para usar el RFID creda por Balboa */
//#include <FreeRTOS.h>          /* Librerir para usar variables semaforo */


const int SS_PIN = 21;
const int RST_PIN = 22;
MFRC522 lector(SS_PIN,RST_PIN);
byte obt_clave = 0;           /*inicia puntero a 0  */
byte tam_clave = 0;           /*inicia puntero a 0 */
byte *ptam_clave = &tam_clave;
byte *pobt_clave = &obt_clave;
//byte *pclave_leida = pobt_clave;
bool leido = false;



 /*Función que muestra la clave en el monitor*/
  void printArray(byte *buffer,byte bufferSize){  
  for (byte i = 0; i < bufferSize; i++){
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
  }
  Serial.println();
}


void setup(){
  lector.PCD_Init();
  SPI.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Se inicia la lectura de las tarjetas. ");
  }

 

void loop () {
   /*Se espera la lectura de una tarjeta RFID*/
    if (lector.PICC_IsNewCardPresent()){
    if (lector.PICC_ReadCardSerial()) {
      Serial.print(F("Card_UID: "));
      printArray(lector.uid.uidByte ,lector.uid.size);
      Serial.println();
      leido = true;
      pobt_clave = lector.uid.uidByte;
      *ptam_clave = lector.uid.size;
      Serial.println("¿Se ha leido la tarjeta?: " + leido);
      Serial.print(F("Card_UID: "));
      printArray(pobt_clave ,*ptam_clave);
      Serial.println();
      lector.PICC_HaltA();
    }
  }
  delay(250);  
  } // Fin del loop


