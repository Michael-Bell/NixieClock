/*
  ShiftRegister74HC595 - Library for simplified control of 74HC595 shift registers.
  Created by Timo Denk (www.timodenk.com), Nov 2014.
  Additional information is available at http://shiftregister.simsso.de/
  Released into the public domain.
*/

#include <ShiftRegister74HC595.h>
#include "uRTCLib.h"

#include <SparkFun_APDS9960.h>
#define APDS9960_INT    3 // Needs to be an interrupt pin

// uRTCLib rtc;
uRTCLib rtc(0x68, 0x57);

uint8_t proximity_data = 0;

SparkFun_APDS9960 apds = SparkFun_APDS9960();
volatile int isr_flag = 0;
/*
 * OK: Workflow for tech Demo:
 * EN Set H
 * LE LOW, BL H, POL H
 * 
 * BL - 7
 * DIN - 1
 * LE - 0
 * CLK - 10
 * POL - 9
 * EN - 8
 * 
 */

#define BL 7
#define DIN 1
#define LE 0
#define CLK 10
#define POL 9
#define EN 8
ShiftRegister74HC595 sr (4,DIN,CLK,LE); 

void writeD1(int n){
switch (n){
  case 0:
  sr.setNoUpdate(22, HIGH);
  break;
  case 1:
  sr.setNoUpdate(23, HIGH);
  break;
  case 2:
  sr.setNoUpdate(24,HIGH);
  break;
  case 3:
  sr.setNoUpdate(25,HIGH);
  break;
  case 4:
  sr.setNoUpdate(26,HIGH);
  break;
  case 5:
  sr.setNoUpdate(27,HIGH);
  break;
  case 6:
  sr.setNoUpdate(28,HIGH);
  break;
  case 7:
  sr.setNoUpdate(29,HIGH);
  break;
  case 8:
  sr.setNoUpdate(30,HIGH);
  break;
  case 9:
  sr.setNoUpdate(31,HIGH);
  break;
  default:
  sr.setNoUpdate(20, HIGH);
  break;
  }
  
}

void writeD2(int n){
switch (n){
  case 0:
  sr.setNoUpdate(10, HIGH);
  break;
  case 1:
  sr.setNoUpdate(11, HIGH);
  break;
  case 2:
  sr.setNoUpdate(13,HIGH);
  break;
  case 3:
  sr.setNoUpdate(15,HIGH);
  break;
  case 4:
  sr.setNoUpdate(17,HIGH);
  break;
  case 5:
  sr.setNoUpdate(1,HIGH);
  break;
  case 6:
  sr.setNoUpdate(3,HIGH);
  break;
  case 7:
  sr.setNoUpdate(5,HIGH);
  break;
  case 8:
  sr.setNoUpdate(7,HIGH);
  break;
  case 9:
  sr.setNoUpdate(9,HIGH);
  break;
  default:
  sr.setNoUpdate(20, HIGH);
  }
  
}

void amPM(bool t){
  if(t) sr.setNoUpdate(0,HIGH);
  else sr.setNoUpdate(0,LOW);
}
 
void setup() { 
  pinMode(APDS9960_INT, INPUT);
  pinMode(BL,OUTPUT);
  digitalWrite(BL, HIGH);
  pinMode(EN, OUTPUT);
  pinMode(POL,OUTPUT);
  digitalWrite(POL,HIGH);
  digitalWrite(EN, HIGH);
    sr.setAllLow(); // set all pins LOW
  delay(500);
//  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
if(apds.init() &&
apds.setProximityGain(PGAIN_2X) &&  
apds.enableProximitySensor(false) ) {
  amPM(true);
}
else{
  amPM(false);
}

 sr.updateRegisters();
tellTime(2);
amPM(false);
 sr.updateRegisters();


}
int avProx=0;
int lastreading = 0;
void loop() {
//apds.readProximity(proximity_data);
//if(proximity_data==255){
//  amPM(true);
//}
//else{
//  amPM(false);
//}
// sr.updateRegisters();

// if((lastreading-50)>proximity_data) tellTime(2);
//lastreading=proximity_data;
tellTime(10);
delay(10*1000);

delay(500);
  }


int num = 0;
bool toggle = false;
void tellTime(int l) {

digitalWrite(EN,HIGH);  
for(int i=0; i<=l ; i++){

rtc.refresh();

writeD1(rtc.hour()%10);
writeD2((rtc.hour()/10)%10);
//amPM(false);
sr.updateRegisters();
delay(1000);
sr.setAllLow();
writeD1(rtc.minute()%10);
writeD2((rtc.minute()/10)%10);
//amPM(true);
sr.updateRegisters();
delay(1000);
sr.setAllLow();
}
digitalWrite(EN,LOW);
}
