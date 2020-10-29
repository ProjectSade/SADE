 #include <QTRSensors.h> // Se carga la librería del sensor utilizado
 

///// Se definen las patas del microprocesador que se van a utilizar/////
#define mi1       3   //motor izquierdo 1
#define mi2       4   //motor izquierdo 2
#define md1       8   //motor derecho 1
#define md2       7   //motor derecho 2
#define pwmd      5   //Error motor derecho
#define pwmi      6   //Error motor izquierdo
   
 
#define NUM_SENSORES  8  // Se definen el numero de sensores usados
#define NUM_SAMPLES_PER_SENSOR  1 // numero de muestras analógicas por sensor
#define PIN_EMISOR     9   // El emisor es controlado por el pin 2


#define TRIG_FRONTAL = 12; // trigger FRONTAL en pin 12
#define TRIG_IZQUIERDO = 13; // trigger IZQUIERDO en pin 13
#define TRIG_DERECHO = 14; // trigger DERECHO en pin 14
#define ECO_ FRONTAL = 9;  // echo FRONTAL en pin 9
#define ECO_IZQUIERDO = 10;  // echo IZQUIERDO en pin 10
#define ECO_DERECHO= 11;  // echo DERECHO en pin 11
#define MAX_DISTANCE 100 // Define la Maxima Distancia

QTRSensorsAnalog qtra((unsigned char[]) {7, 6, 5, 4, 3, 2,1,0},NUM_SENSORES, NUM_SAMPLES_PER_SENSOR, PIN_EMISOR);
unsigned int sensorValues[NUM_SENSORES];

/////Parametros PWM/////
int proporcional=0;
int proporcional_pasado=0;
int derivativo=0;
int integral=0;
int salida_pwm=0;
int posicion=0;
 
/////Parametros PID/////
int velocidad=80;
float KP=0.01; //reaccion rapida
float KD=0.15;
float KI=0.0001;

/////Parametros censado para la detección de la línea/////
int linea=0; //0 linea negra, 1 para linea blanca
int flanco_color=  0 ; //Reflectancia
int dentro_linea=  400 ; //Detección de linea
int ruido= 30;

////Parametros deteccion de objetos en el camino/////
int duración_frontal=0; //Duracion de la señal en el sensor frontal
int distancia_frontal=25; //Distancia de la señal en el sensor frontal
int duración_izquierda=0; //Duracion de la señal en el sensor izquierda
int distancia_izuiqerda=15;  //Distancia de la señal en el sensor izquierda
int duración_derecha=0; //Duracion de la señal en el sensor derecha
int duración_izquierda=10; //Distancia de la señal en el sensor izquierda


void setup()
{
    pinMode(mi1,OUTPUT);
    pinMode(mi2,OUTPUT);
    pinMode(pwmi,OUTPUT);
    pinMode(pwmd,OUTPUT);
    pinMode(md1,OUTPUT);
    pinMode(md2,OUTPUT);
    pinMode(TRIG_FRONTAL, OUTPUT);  // trigger como salida
    pinMode(TRIG_IZQUIERDO, OUTPUT);  
    pinMode(TRIG_DERECHO, OUTPUT);  
    pinMode(ECO_FRONTAL, INPUT);    // echo como entrada
    pinMode(ECO_IZQUIERDO, INPUT);  
    pinMode(ECO_DERECHO, INPUT);      
    Serial.begin(9600);     // inicializacion de comunicacion serial a 9600 bps
    
 delay(250);

  /////Calibracion de los sensores/////
  for (int i = 0; i < 200; i++)  // Se hace la calibracion. Esta puede tardar 15 segundos
  {
    qtra.calibrate();       // Lee 10 veces todos los sensores cada 2.5ms
  }

}
 


void loop()
{
  Deteccion_frontal();
  Deteccion_derecha();
  Deteccion_izquierda();
  
  PID(1,velocidad,KP,KI,KD); // Impacto de la de proporcional, integral y derivada
  frenos_contorno(600);
 
 Serial.println(posicion);
 delay(12);
}
 
void PID(int linea, int velocidad, float KP, float KI, float KD)
{
    
   posicion = qtra.readLine(sensorValues,QTR_EMITTERS_ON, linea, flanco_color, dentro_linea, ruido);
  //0 linea negra, 1 para linea blanca
   

  proporcional = (posicion) - 2500; // El Set point tiene el valor 2500, para así obtener el error
  integral=integral + proporcional_pasado; //Se obtiene el valor de la integral
  derivativo = (proporcional - proporcional_pasado); //Se obtiene el derivativo
  
  int Integ_pwm=integral*KI; // Se obtiene el valor de la posicion pasada para la ecuacion de PWM
  
  if(Inter_pwm>=255) Inter_pwm=255; // Control del valor positivo y negativo
  if(Integ_pwm<=-255) Inter_pwm=-255;
   
  salida_pwm = ( proporcional * KP ) + ( derivativo * KD ) + (Integ_pwm);
   
  if (  salida_pwm > velocidad )  salida_pwm = velocidad;  // Limitacion de la salida de pwm
  if ( salida_pwm < -velocidad )  salida_pwm = -velocidad;
   
  if (salida_pwm < 0)
 {
    int derecha=velocidad-salida_pwm; // PWM derecha
    int izquierda=velocidad+salida_pwm;  // PWM izquierda
    
    if(derecha>=255) derecha=255;
    if(izquierda<=0) izquierda=0;
    motores(izquierda, derecha); // LLamada a control de los motores para girar a la derecha
 }
 
 if (salida_pwm > 0)
 {
  int derecha=velocidad-salida_pwm; 
  int izquierda=velocidad+salida_pwm; 
   
  if(izquierda >= 255) izquierda=255;
  if(derecha <= 0) derecha=0;
  motores(izquierda ,derecha); // LLamada a control de los motores para girar a la izquierda
 }
 
 proporcional_pasado = proporcional; // Se guarda el valor en actual como pasado para la comparacion
}
 
 
void frenos_contorno(int flanco_comparacion)
{
    if (posicion <=10) //Si se sale por la parte derecha de la linea
    {
      while(true)
      { 
        motores(-125,60);
        qtra.read(sensorValues); //Realiza lectura en bruto de sensor
        if ( sensorValues[0]<flanco_comparacion || sensorValues[1]<flanco_comparacion || sensorValues[2]<flanco_comparacion || sensorValues[3]<flanco_comparacion || sensorValues[4]<flanco_comparacion || sensorValues[5]<flanco_comparacion || sensorValues[6]<flanco_comparacion || sensorValues[7]<flanco_comparacion)
        {
          break;
        }
         
      }
    }
 
    if (posicion >= 6990) //Si se sale por la parte izquierda de la linea
    {
      while(true)
      {
        
        motores(60,-125); //Se realiza la corrección
        qtra.read(sensorValues); // Se hacer la comprobacion hasta corregir el error
        if (sensorValues[7]<flanco_comparacion || sensorValues[6]<flanco_comparacion|| sensorValues[5]<flanco_comparacion || sensorValues[4]<flanco_comparacion || sensorValues[3]<flanco_comparacion || sensorValues[2]<flanco_comparacion || sensorValues[1]<flanco_comparacion|| sensorValues[0]<flanco_comparacion)
        {
          break;
        }
      }
  }
  
}


/////Deteccion de objetos en el camino/////
void Deteccion_frontal()
{
  
  digitalWrite(TRIG_FRONTAL, HIGH);     // generacion del pulso a enviar
  delay(1);       // al pin conectado al trigger
  digitalWrite(TRIG_FRONTAL, LOW);    // del sensor
  
  DURACION_FRONTAL = pulseIn(ECO_FRONTAL, HIGH);  // con funcion pulseIn se espera un pulso
            // alto en Echo
  DISTANCIA_FRONTAL = DURACION_FRONTAL / 58.2;    // distancia medida en centimetros

  delay(200);       // demora entre datos

  if (DISTANCIA_FRONTAL <= 120 && DISTANCIA_FRONTAL >= 0) // si distancia entre 0 y 120 cms.
     {   
        PARAR();
        delay(100);
        GIRO_DERECHA();
        delay(580);
        motores();
        delay(450);
        GIRO_IZQUIERDA();
        delay(550);
        motores();
        delay(700);
        GIRO_IZQUIERDA();
        delay(500);
        motores();
        delay(480);
        GIRO_DERECHA();
        delay(500);
    
    }
   
}

void Deteccion_derecha()
{
  
  digitalWrite(TRIG_DERECHA, HIGH);     // generacion del pulso a enviar
  delay(1);       // al pin conectado al trigger
  digitalWrite(TRIG_DERECHA, LOW);    // del sensor
  
  DURACION_DERECHA = pulseIn(ECO_DERECHA, HIGH);  // con funcion pulseIn se espera un pulso
            // alto en Echo
  DISTANCIA_DERECHA = DURACION_DERECHAL / 58.2;    // distancia medida en centimetros

  delay(200);       // demora entre datos

  if (DISTANCIA_DERECHA <= 25 && DISTANCIA_DERECHA >= 0) // si distancia entre 0 y 25 cms.
     {   
        PARAR();
        delay(500);
    
    }
   
}


void Deteccion_izquierda()
{
  
  digitalWrite(TRIG_IZQUIERDA, HIGH);     // generacion del pulso a enviar
  delay(1);       // al pin conectado al trigger
  digitalWrite(TRIG_IZQUIERDA, LOW);    // del sensor
  
  DURACION_IZQUIERDA = pulseIn(ECO_IZQUIERDA, HIGH);  // con la funcion pulseIn se espera un pulso
            // alto en Echo
  DISTANCIA_IZQUIERDA = DURACION_DIZQUIERDA / 58.2;    // distancia medida en centimetros

  delay(200);       // demora entre datos

  if (DISTANCIA_IZQUIERDA <= 50 && DISTANCIA_IZQUIERDA >= 0) // si distancia entre 0 y 50 cms.
     {   
        PARAR();
        delay(500);
    
    }
   
}

 
 /////Control de los motores del dispositivo EN EL SEGUIMIENTO DE LA LINEA/////

void motores(int motor_izquierdo, int motor_derecho)
{
  if ( motor_izquierdo >= 0 )  //Control motor de la izquierda
  {
    digitalWrite(mi1,LOW);
    digitalWrite(mi2,HIGH); 
    analogWrite(pwmi,motor_izquierdo); 
  }
  else
  {
    digitalWrite(mi1,HIGH); 
    digitalWrite(mi2,LOW);
    motor_izquierdo = motor_izquierdo*(-1); // Se mulltiplica por -1 para invertir el movimiento y girar
    analogWrite(pwmi,motor_izquierdo);
  }
 
  if ( motor_derecho >= 0 )  //Control motor derecho
  {
    digitalWrite(md1,LOW);
    digitalWrite(md2,HIGH);
    analogWrite(pwmd,motor_derecho);
  }
  else
  {
    digitalWrite(md1,HIGH);
    digitalWrite(md2,LOW);
    motor_derecho= motor_derecho*(-1); // Se mulltiplica por -1 para invertir el movimiento y girar
    analogWrite(pwmd,motor_derecho);
  }
}
 

/////Control de los motores del dispositivo EN LA DETECCION DE OBSTACULOS/////

void PARAR()  // Detiene los motores 
{  
  digitalWrite(mi1,LOW);
  digitalWrite(mi2,LOW); 
  digitalWrite(md1,LOW);
  digitalWrite(md2,LOW);
  analogWrite(pwmi,0); 
  analogWrite(pwmd,0);
}

void GIRO_DERECHA()   // Gira a la derecha
{   
  digitalWrite(mi1,LOW);
  digitalWrite(mi2,LOW); 
  digitalWrite(md1,HIGH);
  digitalWrite(md2,HIGH);
  analogWrite(pwmi,100); 
  analogWrite(pwmd,100);
  
}

void GIRO_IZQUIERDA()     // Gira a la izquierda
{  
  digitalWrite(md1,LOW);
  digitalWrite(md2,LOW); 
  digitalWrite(mi1,HIGH);
  digitalWrite(mi2,HIGH);
  analogWrite(pwmi,100); 
  analogWrite(pwmd,100);
}

}
