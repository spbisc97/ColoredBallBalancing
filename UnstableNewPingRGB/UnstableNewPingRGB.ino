  

/*
 * il codice funzionerà solo se viene aggiunta la funzione GetIntegrale() alla libreria pid_v1
 * che ritorna il valore della componente integrale
 */

#include<NewPing.h>   //libreria Ultrasonic sensor 
#include<Servo.h>
#include<PID_v1.h>

#define BAUDRATE 230400
#define MAX_DISTANCE 50
#define horizon 1540
#define holdUS 5000

#define servoPin 6  
#define trigPin  4
#define echoPin  5

#define kpPin A0
#define kdPin A1
#define kiPin A2

#define RPIN 9
#define GPIN 10
#define BPIN 11


#define positionPin A3
#define MaxOutput 300    //ogni USstep è 0.135° -> 300 USstep = 40.5°
#define MinOutput -300

float Kp = 10.5;                                                    // Proportional Gain Iniziale
float Ki = 0.02;                                                     // Integral Gain Iniziale
float Kd = 5.2;  // Derivative Gain Iniziale

int red,blue,green;

float duration, cm; //prec=1;

unsigned long int serialtime=0;

double Setpoint, Input, Output, ServoOutput;  


NewPing sonar(trigPin, echoPin, MAX_DISTANCE);

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);           //PID object
                                                                   
                                                                     
                                                                     
Servo myServo;                                                       //Initialize Servo.


void setup() {
  millis();

                                                  
  Serial.begin(BAUDRATE);  //Connessione Seriale
  myServo.attach(servoPin,833,2166);                                   //Attach Servo   //il servo che usiamo ha un range di 270° e 500-2500 Usec impulse
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT );

  pinMode(RPIN,OUTPUT);
  pinMode(GPIN,OUTPUT);
  pinMode(BPIN,OUTPUT);
                                      
  myPID.SetMode(AUTOMATIC);                                           //Attiva il collegamento tra oggetto myPID e Output
  myPID.SetOutputLimits(MinOutput,MaxOutput);                                    //Set Output limits 

  myServo.writeMicroseconds(horizon);
  Serial.println("Progetto Sviluppato da:");
  Serial.println("        Simone Rotondi");
  Serial.println("        Francesco Riccioni:");
  Serial.println("        Matteo Matera");
  Serial.println("        Leonardo Pio Lo Porto");

  LedPosition(20);

  delay(holdUS);
}

void loop()
{
  delay(10);//10 millis

  Setpoint=knobLinMapper(positionPin) ;
  //Setpoint =15;
  LedPosition(Setpoint);
  
  Input = (double)sonar.ping_median(2)/US_ROUNDTRIP_CM; 
  //if (Input==0){Input=prec;}  
  //prec=Input;
                                 
  Kp=knobExpMapper(kpPin);
  Kd=knobExpMapper(kdPin);
  Ki=knobExpMapper(kiPin);
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.Compute();
  
  //scegliamo di utilizzare l'output in microsecondi poichè il metodo Write() accetta solo interi, limitando gli step possibili
  //il servo funziona tra i 500 e i 2500 microsecondi, un range di 2000 per 270° ovvero 0.135° per UsecStep,
  //o 7.407 UsecStep per grado
  
  myServo.writeMicroseconds(horizon+Output);   
  if(millis()>serialtime){
    SerialSend();
    serialtime=serialtime+100;
  }
  
}
      
void SerialSend()
{

  // Serial.print(red);Serial.print(",");Serial.print(green);Serial.print(",");Serial.print(blue);Serial.print(",");

  //commenta le informazioni che non ti servono
  //Serial.print(Input);Serial.print(" ");//per il plotter
  Serial.print("{");
  //Serial.print("\"Millis\":");Serial.print(millis()); Serial.print(",");
  Serial.print("\"Setpoint\":");Serial.print(Setpoint); Serial.print(",");
  Serial.print("\"Input\":");Serial.print(Input); Serial.print(",");
  Serial.print("\"Output\":");Serial.print(Output); Serial.print(",");
  //Serial.print("\"AngleOutput\":");Serial.print(map(horizon+Output,833,2166,0,180)); Serial.print(",");
  //Serial.print("\"ServoAngle\":");Serial.print(myServo.read()); Serial.print(",");
  
  
  Serial.print("\"kp\":");Serial.print(myPID.GetKp());Serial.print(",");
  Serial.print("\"ki\":");Serial.print(myPID.GetKi());Serial.print(",");
  Serial.print("\"kd\":");Serial.print(myPID.GetKd());Serial.print(",");  
  Serial.print("\"IntegralPart\":");Serial.print(myPID.GetIntegrale());Serial.print("}");
  Serial.println();
  
}

float knobExpMapper(int knob){
  //Ci vogliono circa 100 microsecondi ( 0.0001 s ) per leggere un ingresso analogico
  int val=analogRead(knob); //legge i valori 
  float uscita=pow(3,(double)val/400) - 1;  // divide i valori per 300 in modo da avere un range da 0 a 41 
                                    // lega i valori ad una funzione esponenziale  3^(val/300) -1
  return uscita;   //ritorna il valore
}
int knobLinMapper(int knob){
  //Ci vogliono circa 100 microsecondi ( 0.0001 s ) per leggere un ingresso analogico
  int val=analogRead(knob)-1;    //legge i valori 
  return map((double)val,0,1023,10,21);     //val/102.3 +10;   //ritorna il valore mappato tra 10 e 20 
}

void LedPosition(int pos){
  if(pos>15){
    red=map(pos,15,20,255,0);
    green=map(pos,15,20,0,200);
    blue=0;
   }else{
    red=map(pos,10,15,0,255);
    green=0;
    blue=map(pos,10,15,255,0);
   }

   analogWrite(RPIN,255-red);
   analogWrite(GPIN,255-green);
   analogWrite(BPIN,255-blue);
   //myLED.setColor(red,green,blue);
  
}
