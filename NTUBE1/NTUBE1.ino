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
#include <FAB_LED.h>

// Declare the LED protocol and the port
sk6812<D,2>  strip;

// How many pixels to control
const uint8_t numPixels = 1;

// How bright the LEDs will be (max 255)
const uint8_t maxBrightness = 16;

// The pixel array to display
grb  pixels[numPixels] = {};
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
 void updateColors(char r, char g, char b)
{
  for(int i = 0; i < numPixels; i++)
  {
    pixels[i].r = r;
    pixels[i].g = g;
    pixels[i].b = b;
  }
}
void setup() { 
    strip.clear(2 * numPixels);

  pinMode(APDS9960_INT, INPUT);
  pinMode(BL,OUTPUT);
  digitalWrite(BL, HIGH);
  pinMode(EN, OUTPUT);
  pinMode(POL,OUTPUT);
  digitalWrite(POL,HIGH);
  digitalWrite(EN, LOW);
    sr.setAllLow(); // set all pins LOW
  delay(500);
//  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
amPM(false);
  if ( apds.init() ) {
  } else {
amPM(true);  }
  
  // Adjust the Proximity sensor gain
  if ( !apds.setProximityGain(PGAIN_2X) ) {
amPM(true);  }
  
  // Start running the APDS-9960 proximity sensor (no interrupts)
  if ( apds.enableProximitySensor(false) ) {
  } else {
amPM(true);  }

updateColors(5,20,50);
// Display the pixels on the LED strip
strip.sendPixels(numPixels, pixels);
delay(750);
 sr.updateRegisters();

rtc.refresh();
if(rtc.hour()<18){
rtc.set();
}
  rtc.set(0, 00, 12, 6, 2, 5, 18);
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)

}





int avProx=0;
int lastreading = 0;


int mapped;
int steps=0;
int resettime=0;
void loop() {

apds.readProximity(proximity_data);
// r g b
int r,g,b;
r=0;g=0;b=0;
mapped = map(proximity_data,0,255,0,10);

if(mapped==10) {
  steps++;
resettime++;
}
else(resettime=0;)
if (steps==4) steps=0;


switch(steps){ // set led color based on the step iteration
  case 1:
  g=25*mapped;
  break;
  case 2:
  r = 25*mapped;
  break;
  case 3:
  b = 25*mapped;
  break;
  default:
  g=0;
}

updateColors(r,g,b);
// Display the pixels on the LED strip
strip.sendPixels(numPixels, pixels);

if(resettime>12){
  updateColors(255,255,0);
// Display the pixels on the LED strip
strip.sendPixels(numPixels, pixels);
delay(750);
  updateColors(0,255,255);
// Display the pixels on the LED strip
strip.sendPixels(numPixels, pixels);
delay(750);
  updateColors(255,0,255);
// Display the pixels on the LED strip
strip.sendPixels(numPixels, pixels);
}
if(resettime==24){
  resettime=0;
  steps=0;
    rtc.set(0, 00, 12, 6, 2, 5, 18);
tellTime(1);
}

if((lastreading-5)>mapped) switch(steps){
    case 1: // display time twice
    tellTime(2);
        steps=0;
    //g=0;
    break;
    case 2:
    steps=0;
   // g=0;
    nuke(2); // run through all digits rapidly
    break;
    default:
    g=0;
  }

if(mapped<6 && steps==3) { // if no hand is in front and steps has iterated to the third tell the time 
  tellTime(1);
}


lastreading=mapped;
delay(1000);


  }


int num = 0;
bool toggle = false;
void tellTime(int l) {
sr.setAllLow();
digitalWrite(EN,HIGH);  
for(int i=0; i<=l ; i++){

rtc.refresh();

writeD1(rtc.hour()%10);
writeD2((rtc.hour()/10)%10);
amPM(false);
sr.updateRegisters();
delay(1000);
sr.setAllLow();
writeD1(rtc.minute()%10);
writeD2((rtc.minute()/10)%10);
amPM(true);
sr.updateRegisters();
delay(1000);
sr.setAllLow();
}
digitalWrite(EN,LOW);
}

void nuke(int iterations){
  digitalWrite(EN,HIGH);
  for(int x=0;x<=iterations;x++){
    for(int i=0; i<10; i++){
        sr.setAllLow();
        writeD1(i);
        writeD2(9-i);
        sr.updateRegisters();
        delay(250);
    }
  }
digitalWrite(EN,LOW);
}

