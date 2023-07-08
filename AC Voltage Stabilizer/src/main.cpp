#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "EmonLib.h"             // Include Emon Library
//#include <avr/wdt.h>

#define OLED_RESET 4             // Initialize OLED display
Adafruit_SSD1306 display(OLED_RESET);

EnergyMonitor emon1;             // Create an instance

// Define GPIO pin for tap changing
int over_under = 2;
int Tap1 = 3;
int Tap2 = 6;
int Tap3 = 5;
int mode = 0;


//float ac_in = 0;
float ac_involt = 0;

//int ac_out = 0;
float ac_outvolt = 0;

float input_volt = 0;
float output_volt = 0;

float ac_low = 120;
float ac_high = 275;

//this variable is for operating range of transformer
float min_range = 130;
float max_range = 270;   // Its must be 5 or 10 volt less than ac_high value to protect relay from chattering


boolean needtimer = true;  
boolean  incount = false ; 

int  count = 10 ; 
long  int  time = 0 ;
unsigned long previous=0;
unsigned long previousMillis=0;



/*
void timer(){
    unsigned long currentMillis = millis();
 
   // How much time has passed, accounting for rollover with subtraction!
   if (currentMillis - previousMillis > 5000) {
        //Sdelay = now;
        needtimer = false;
        previousMillis = currentMillis;
        Serial.println(millis() / 1000);
    }
}
*/
void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("I/P:");
  display.setCursor(50, 0);
  display.println(ac_involt);
  display.setCursor(0, 18);
  display.println("O/P:");
  display.setCursor(50, 18);
  display.println(ac_outvolt);
  display.display();
}

void  stop ( ) { 
  if(ac_involt > 120 && ac_involt <= max_range){
  time = 0 ; 
  count = 10 ; 
  incount = false ; 
  needtimer = false;
/*
  if (ac_involt >= 130 && ac_involt <= 210){
    digitalWrite(over_under, LOW);
    }else if(ac_involt > 210 && ac_involt <= max_range){
    digitalWrite(over_under, HIGH);
   }
*/

  if (ac_involt >= 180 && ac_involt < 205){
     mode = 1;
    digitalWrite(Tap1, HIGH);
    digitalWrite(Tap2, HIGH);
    digitalWrite(Tap3, HIGH);
    delay(2000);
   }
  if (ac_involt >= 150 && ac_involt < 180){
     mode = 2;
    digitalWrite(Tap1, LOW);
    digitalWrite(Tap2, HIGH);
    digitalWrite(Tap3, HIGH);
    delay(2000);
   }
  if (ac_involt >= 120 && ac_involt < 150){
     mode = 3;
    digitalWrite(Tap1, LOW);
    digitalWrite(Tap2, LOW);
    digitalWrite(Tap3, HIGH);
    delay(2000);
   }
  if (ac_involt >= 205 && ac_involt < max_range){
    mode = 4;
    digitalWrite(Tap3, LOW);
    digitalWrite(Tap1, LOW);
    digitalWrite(Tap2, LOW);
    delay(2000);
   }
  }else if(ac_involt > max_range){
    digitalWrite(over_under, HIGH);
    time = 0 ; 
    count = 10 ; 
    incount = true ; 
    needtimer = true;
    display.clearDisplay();
    //display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(44, 5);
    display.println("HIGH");
    display.display();
    delay(1000);
  }
}

void voltagedata(){
  unsigned long now = millis();
 
    if (now - previous > 2000) {
    /*
      if(input_volt <= 0.00){
        digitalWrite(over_under, LOW);
        digitalWrite(Tap1, HIGH);
        digitalWrite(Tap2, HIGH);
        digitalWrite(Tap3, HIGH);
        needtimer = true;
        time = 0 ; 
        count = 10 ;
        //mode = 1;
        incount = false;
        Serial.println("analog pin not working");
        }
  */
        Serial.print("adc volt: ");
        Serial.println(input_volt);
        Serial.print("input_volt: ");
        Serial.println(ac_involt );
        Serial.print("adc_outvolt: ");
        Serial.println(output_volt);
        Serial.print("output volt: ");
        Serial.println(ac_outvolt);
        Serial.print("mode: ");
        Serial.println(mode);
       previous = now;
       }
}
/*

void countdown() {
  unsigned long timeRemaining = millis();

  while (timeRemaining > 0 ){
    int seconds = numberofSeconds(timeRemaining);
    needtimer = false;
    timeRemaining = timeLimit - millis();
    Serial.println(seconds);
  }
}
*/

//void stablelize(){
 // if (voltage1 >= 1.0 && voltage1 <= 1.9){
  //  digitalWrite(Tap1, LOW);
 // }else{digitalWrite(Tap1, HIGH);}


//}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  emon1.voltage(1, 270.5, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.voltage1(2, 279.5, 1.7);  // Voltage: input pin, calibration, phase_shift

  pinMode(over_under, OUTPUT);digitalWrite(over_under, HIGH);
  pinMode(Tap1, OUTPUT);digitalWrite(Tap1, HIGH);
  pinMode(Tap2 , OUTPUT);digitalWrite(Tap2, HIGH);
  pinMode(Tap3, OUTPUT);digitalWrite(Tap3, HIGH);
  //pinMode(Tap5 , OUTPUT);digitalWrite(Tap5, HIGH);
  //pinMode(Tap2, OUTPUT);
  //Serial . println ( "Time: 0" ) ;
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("transformer initializing, wait for 10 sec.");
  display.display();
  delay(3000);
}

// the loop routine runs over and over again forever:
void loop() {
  // print out the value you read:

  voltagedata();
  emon1.calcVI(20,100);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)  
  emon1.calcVI1(20,100);

  ac_involt = emon1.Vrms1;
  ac_outvolt = emon1.Vrms;

       

  unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= 300)
    {
      previousMillis = currentMillis;

      // read the input on analog pin 0:
        //refresh display in every 500 ms
        //DC to AC converstion input voltage
      //ac_in = (((5.45 * input_volt) / 1. 414) + 0.2) * 23.6;
      

      //DC to AC converstion output voltage
      //ac_out = (((5.46 * output_volt) / 1.414) + 0.2) * 24.8 + 4;

      //int sensorValue1 = analogRead(A2);
      //int sensorValue2 = analogRead(A0);
      //input_volt = sensorValue1 * (4.93 / 1023.0);
      //output_volt = sensorValue2 * (5.0 / 1023.0);

      //display voltage on oled display if everything okay and codition fulfilled
      if(!needtimer && !incount){
      displaydata();
      }
    }

//protect load when voltage too low
  if (ac_involt < ac_low){
    digitalWrite(over_under, HIGH);
    digitalWrite(Tap1, HIGH);
    digitalWrite(Tap2, HIGH);
    digitalWrite(Tap3, HIGH);
   
    time = 0 ; 
    count = 10 ;
    mode = 1;
    needtimer = true;
    incount = false;
    display.clearDisplay();
    //display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("LOW");
    display.setCursor(0, 18);
    display.println("I/P:");
    display.setCursor(50, 18);
    display.println(ac_involt);
    display.display();
    Serial.println("input voltage low");
    //delay(100);
  }

//protect load when voltage too high
  if((ac_involt > ac_high || ac_outvolt > ac_high)){
    digitalWrite(over_under, HIGH);
    digitalWrite(Tap1, HIGH);
    digitalWrite(Tap2, HIGH);
    digitalWrite(Tap3, HIGH);
  
    time = 0 ; 
    count = 10 ;
    mode = 1;
    needtimer = true;
    incount = false;

    display.clearDisplay();
    //display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("HIGH");
    display.setCursor(0, 18);
    display.println("I/P:");
    display.setCursor(50, 18);
    display.println(ac_involt);
    display.display();
    Serial.println("input voltage high");
    //delay(100);
  }

  if(needtimer == true && incount == false){
    //timer();
    //countdown();
      if (ac_involt >= min_range && ac_involt < max_range){
        digitalWrite(over_under, HIGH);
        digitalWrite(Tap1, HIGH);
        digitalWrite(Tap2, HIGH);
        digitalWrite(Tap3, HIGH);
        incount = true;
        //stop();

        } else if (ac_involt > max_range) {
        digitalWrite(over_under, HIGH);
        incount = false;
        display.clearDisplay();
        //display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println("HIGH");
        display.setCursor(0, 18);
        display.println("I/P:");
        display.setCursor(50, 18);
        display.println(ac_involt);
        display.display();
        }
 }

//Change Tapping of transformer according to input voltage
if (ac_involt >= 210 && ac_involt <= 280){
    mode = 4;
   }
if (ac_involt >= 180 && ac_involt < 205){
     mode = 1;
   }
if (ac_involt >= 154 && ac_involt < 170){
     mode = 2;
   }
if (ac_involt >= 130 && ac_involt < 148){
     mode = 3;
   }

   

if(!needtimer && !incount){
    //digitalWrite(over_under, HIGH);
    //stablelize();
    switch(mode)
  {
    case 0: break;
    case 1:
    digitalWrite(Tap1, HIGH);
    digitalWrite(Tap2, HIGH);
    digitalWrite(Tap3, HIGH);
    digitalWrite(over_under, LOW);
    break;

    case 2:
    digitalWrite(Tap1, LOW);
    digitalWrite(Tap2, HIGH);
    digitalWrite(Tap3, HIGH);
    digitalWrite(over_under, LOW);
    break;

    case 3:
    digitalWrite(Tap1, LOW);
    digitalWrite(Tap2, LOW);
    digitalWrite(Tap3, HIGH);
    digitalWrite(over_under, LOW);
    break;

    case 4:
    digitalWrite(Tap3, LOW);
    digitalWrite(Tap1, LOW);
    digitalWrite(Tap2, LOW);
    digitalWrite(over_under, HIGH); 
    break;
  }
}
  
//10 second delay when system boot up or also when voltage too high or too low
if  ( incount  &&  round ( ( millis ( )  -  time )  /  1000 ) )  { 

    //digitalWrite(over_under, LOW);
    time = millis ( ) ; 
    display.clearDisplay();
    //display.setTextColor(WHITE);
    display.setTextSize(3);
    if(count > 9){
      display.setCursor(50, 5);
      display.println(String ( count ));
      }else{
        display.setCursor(46, 5);
        display.println("0");
        display.setCursor(70, 5);
        display.println(String ( count ));
      }
    display.display();
    //lcd . clear ( ) ; 
    //lcd . print ( "Tempo:"  +  String ( count ) ); 
    Serial.println ( "Tempo:"  +  String ( count ) );
  
    if  ( count )  {     
      count -- ; 
    }  else  {      
      stop ( ) ;    
    }    
  }
}