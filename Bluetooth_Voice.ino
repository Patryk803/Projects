#include <SoftwareSerial.h>
#include "SimpleDHT.h"
#define pinDHT11 2
SimpleDHT11 dht11;
#define photoresistor (A0)
#define Relay 7
double light;
#include "IRremote.h"
int receiver = 3; // Signal Pin of IR receiver to Arduino Digital Pin 3
int relay = 7;
int buzzer = 4;
int motion = 5;

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

SoftwareSerial BT(0, 1); // TX, RX
String state; // string to store incoming message from bluetooth
int reading; // variable to store either 1 or 0 from button "on" and "off"


void setup() {
  BT.begin(9600); //bluetooth serial communication will hapen on 
  Serial.begin(9600); // serial communication to check the data
  pinMode(Relay, OUTPUT); // Relay Switch
  digitalWrite(Relay, HIGH);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH); // Turn off Buzzer
  pinMode(motion, INPUT);
  digitalWrite(motion, LOW);
}

void loop() {
  motion_buzzer();
  ir_receiver();
  bluetooth();
  buttons();
  boardsensors();
}
void motion_buzzer(){
  if(digitalRead(motion) == HIGH){
  digitalWrite(buzzer, LOW);
  digitalWrite(Relay, LOW);
  //int motionValue = analogRead(motion);
  //Serial.println(motionValue);
  //Serial.println("!ALARM!");
  delay(100);
  digitalWrite(buzzer, HIGH);
  digitalWrite(Relay, HIGH);
}
}
void ir_receiver(){
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR();
    irrecv.resume(); // receive the next value
  }  
}/* --(end main loop )-- */

/*-----( Function )-----*/
void translateIR() // takes action based on IR code received

// describing Remote IR codes 

{
  switch(results.value)
  {
  case 0xFF906F: Serial.println("Turn On");
  digitalWrite(relay, LOW); //Turn on Relay/Turn on lamp
  break;
  case 0xFFA857: Serial.println("Turn Off");
  digitalWrite(relay, HIGH);
  break;
  default: 
    Serial.print("Other button, Hexadecimal value: ");
    Serial.println(results.value, HEX);
  }// End Case
  delay(500); // Do not get immediate repeat
}
void bluetooth(){
  while (BT.available()){ //Check if there is an available byte
    delay(10); //Delay to make it stable
    char c = BT.read(); //Conduct a serial read
    state += c; //build string - either "on" or "off"
  }
if(state.length() > 0){
  Serial.println(state);

  if(state == "on"){
    digitalWrite(Relay, LOW);
  }
  else if(state == "off"){
    digitalWrite(Relay, HIGH);
  }
  state= ""; //Reset the variable
}
}
void buttons(){
  if(Serial.available() > 0){
    reading = Serial.read();
  }

  if (reading == '0'){
    digitalWrite(Relay, HIGH); // Turn light off
    //Serial.println("Light: OFF");
    reading = 0;
  }
  else if (reading == '1'){
    digitalWrite(Relay, LOW);
    //Serial.println("Light: ON");
    reading = 0;
  }
}

void boardsensors(){
  byte temperature = 0;
  byte humidity = 0;
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL))
  {
    return;
  }
  light = analogRead(photoresistor);
  light = 1023 - light; // to change reverse the values
  light = light/1023*100; // To get % of the light intensity
  Serial.print((int)temperature);
  Serial.print(" C");
  Serial.print("|");
  Serial.print((int)humidity);
  Serial.print(" %");
  Serial.print("|");
  Serial.print((int)light);
  Serial.print(" %");

  delay(3000);
}
