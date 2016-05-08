/*
  PBMonitorT v 1.2 example - Interrupt driven push button monitor with simple debouncing
  Triggers "an event" (calling a call back function) on release of the button
  Reports back the duration in miliseconds the button has been held down before beeing released
  so that long and short presses can be determined from the user function invoked by the button
  
  The example circuit:
   * LEDs on pins 4 and 6 to ground (+ resistors)
   * switch (normally open) from pin 3 to GND (internal pull-up configured)
   * switch (normally open) from pin 2 to Vcc with a 10K pull-down resistor

 created 08.05.2016
 by Dejan 
 */

#include "idPushButton.h"

#define BOARD_LED 13

// definition of the pins the LEDs are connected to
#define LED_R 6
#define LED_G 5

// definition of the pins the push buttons are connected to 
// can be active low (using internal puluup resistor) or high (external pulldown resistor must be installed)
#define PB1 3
#define PB2 2
#define PB3 7
#define PB4 A3

// An example callback function to be called when button1 is pressed
// Turns on and off one by one the two LEDs in sequence with 100ms delay
void FlashLeds(unsigned long n) 
{
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  int t=100;

  Serial.print(" Button held down for: ");
  Serial.print(n);
  Serial.print("ms  - ");
  Serial.print("Service 1 started.");

  for(int i=0; i<3; i++)
  {
    digitalWrite(LED_G, HIGH); 
    delay(t); Serial.print('.');
    digitalWrite(LED_R, HIGH); 
    delay(t); Serial.print('.');
    digitalWrite(LED_G, LOW); 
    delay(t); Serial.print('.');
    digitalWrite(LED_R, LOW); 
    delay(3*t); Serial.print('.');
  }  
  digitalWrite(LED_R, LOW); 
  digitalWrite(LED_G, LOW); 
  Serial.println(" service 1 ended.");
}

void FlashLeds2(unsigned long n) 
{
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  int t=100;

  Serial.print(" Button held down for: ");
  Serial.print(n);
  Serial.print("ms  - ");
  Serial.print("Service 3 started.");

  for(int i=0; i<3; i++)
  {
    digitalWrite(LED_R, HIGH); 
    delay(t); Serial.print(',');
    digitalWrite(LED_G, HIGH); 
    delay(t); Serial.print(',');
    digitalWrite(LED_R, LOW); 
    delay(t); Serial.print(',');
    digitalWrite(LED_G, LOW); 
    delay(3*t); Serial.print(',');
  }  
  digitalWrite(LED_R, LOW); 
  digitalWrite(LED_G, LOW); 
  Serial.println(" service 3 ended.");
}
// An example callback function to be called when button2 is pressed
// Turns on and off both of the LEDs with 300ms delay
void BlinkLeds(unsigned long n) 
{
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  int t=300;

  Serial.print(" Button held down for: ");
  Serial.print(n);
  Serial.print("ms  - ");
  Serial.print("Service 2 started.");

  if(n>1000) // long press
  {
    for(int i=0; i<3; i++)
    {
      int j=0;
      for( ; j<200; j+=2)
      {
        analogWrite(LED_G, j); 
        analogWrite(LED_R, j); 
        delay(3); 
      }
      Serial.print(".,");
      for( ; j>=0; j-=2)
      {
        analogWrite(LED_G, j); 
        analogWrite(LED_R, j); 
        delay(3); 
      }
      Serial.print(".,");
    }  
  }
  else
  {
    for(int i=0; i<3; i++)
    {
      digitalWrite(LED_G, HIGH); 
      digitalWrite(LED_R, HIGH); 
      delay(t); Serial.print("..");
      digitalWrite(LED_G, LOW); 
      digitalWrite(LED_R, LOW); 
      delay(t); Serial.print("..");
    }
  }  
  Serial.println(" service 2 ended.");
}

/*
// void Monitorchange1();
int cMonitor1Change=0;
PBmonitor<LOW> button1(PB2, FlashLeds, MonitorChange1, 30);
void MonitorChange1() // global user defined void f() to be used for ISR from inside the class
{
  cMonitor1Change++; // can contain other code...
  button1.change(); // must contain this call
}

// void MonitorChange2();
int cMonitor2Change=0;
PBmonitor<HIGH> button2(PB1, BlinkLeds, MonitorChange2, 50); // puldown resistor must be used
void MonitorChange2() // global user defined void f() to be used for ISR from inside the class
{
  cMonitor2Change++;
  button2.change();
}

void MonitorChange12() // global user defined void f() to be used for ISR from inside the class
{
  cMonitor1Change++; // can contain other code...
  button2.change(); // must contain this call
}

void MonitorChange21() // global user defined void f() to be used for ISR from inside the class
{
  cMonitor2Change++;
  button1.change();
}
*/

// Define the push buttons to be monitored
PUSH_BUTTON_L(button1, PB2, FlashLeds, ONRELEASE);
//SET_PB(button1, PB2, FlashLeds, MonitorChange1, LOW, 20);
PUSH_BUTTON_H(button2, PB1, BlinkLeds, ONPRESS);
//SET_PB(button2, PB1, BlinkLeds, MonitorChange2, HIGH, 100);

PUSH_BUTTON_L(button3, PB3, FlashLeds2, ONPRESS);
PUSH_BUTTON_H(button4, PB4, FlashLeds2, ONRELEASE);



int cnt=0;

void setup() {
  // put your setup code here, to run once:

//  FlashLeds(3);
///////////////////
  Serial.begin(115200);
  delay(100);

  Serial.println("Push button from interrupt example ...");
  Serial.println("Buttons:");
  Serial.print("Button #1 @pin2");
  Serial.print(button1.type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button1.isMonitoring()?"  monitored":"   idle");
  Serial.print("Button #2 @pin3");
  Serial.print(button2.type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button2.isMonitoring()?"  monitored":"   idle");
  Serial.print(button3.type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button3.isMonitoring()?"  monitored":"   idle");

  Serial.println("Start monitoring...");

// Mandatory - initiate monitoring
  button1.startMonitoring();
  button2.startMonitoring();
  button3.startMonitoring();
  button4.startMonitoring();
 
  Serial.print("Button #1 @pin2");
  Serial.print(button1.type()?"(active HIGH)":"(active  LOW)");
  Serial.print(button1.isMonitoring()?"  monitored ":"   idle ");
  Serial.print((long)button1.getCallback(), HEX);
  Serial.println();
  Serial.print("Button #2 @pin3");
  Serial.print(button2.type()?"(active HIGH)":"(active  LOW)");
  Serial.print(button2.isMonitoring()?"  monitored ":"   idle ");
  Serial.print((long)button2.getCallback(), HEX);
  Serial.println();
  Serial.print("Button #3 @pin7");
  Serial.print(button3.type()?"(active HIGH)":"(active  LOW)");
  Serial.print(button3.isMonitoring()?"  monitored ":"   idle ");
  Serial.print((long)button3.getCallback(), HEX);
  Serial.println();

  Serial.println("Push button ready ...");
  pinMode(BOARD_LED, OUTPUT);


  Serial.print("sizeof(uint8_t)=");
  Serial.println(sizeof(uint8_t));
  Serial.print("sizeof(PBcallback)=");
  Serial.println(sizeof(PBcallback));
  Serial.print("sizeof(unsigned long)=");
  Serial.println(sizeof(unsigned long));
//  Serial.print("sizeof(bitPack)=");
//  Serial.println(sizeof(PBmonitor<LOW>::bitPack));


  Serial.print(sizeof(button1));
  Serial.print("  ");
  Serial.println(sizeof(button2));
  Serial.println();

} // end setup

void loop() {
  // put your main code here, to run repeatedly:
  // Nothing really happens here - EVERYTHING is interrupt driven
  // you can put any other code to monitor other sensor / perform other tasks

  // Just monitoring the current state od the pins 2 and 3 (push button 1 & 2 are connected to)
  // the functions will fire up on button presses and will print on the serial 

  cnt++;
  Serial.print(cnt);
  Serial.print(". digitalRead(PB1)=");
  Serial.print(digitalRead(PB1));
  Serial.print("  ");
  Serial.print("digitalRead(PB2)=");
  Serial.print(digitalRead(PB2));
  Serial.print("  ");
  Serial.print("digitalRead(PB3)=");
  Serial.print(digitalRead(PB3));
  Serial.print(button1.isInCallback()?" 1-inCB":"       ");
  Serial.print(button2.isInCallback()?" 2-inCB":"       ");
  Serial.print(button3.isInCallback()?" 3-inCB":"       ");
  Serial.println();
/*
  if(cnt==10)
  {
    button1.stopMonitoring();
    Serial.print("Button #1 @pin2");
    Serial.print(button1.type()?"(active HIGH)":"(active  LOW)");
    Serial.print(button1.isMonitoring()?"  monitored ":"   idle ");
    Serial.print((long)button1.getCallback(), HEX);
    Serial.print(button1.isInCallback()?"  inCB ":" !inCB ");
    Serial.println();
  }    

  if(cnt==20)
  {
    button1.stopMonitoring();
    button2.stopMonitoring();
    PBcallback t=button2.getCallback();
    button2.setCallback(button1.getCallback());
    button1.setCallback(t);
    button1.startMonitoring();
    button2.startMonitoring();
    Serial.print("Button #1 @pin2");
    Serial.print(button1.type()?"(active HIGH)":"(active  LOW)");
    Serial.print(button1.isMonitoring()?"  monitored ":"   idle ");
    Serial.print((long)button1.getCallback(), HEX);
    Serial.print(button1.isInCallback()?"  inCB ":" !inCB ");
    Serial.println();
    Serial.print("Button #2 @pin3");
    Serial.print(button2.type()?"(active HIGH)":"(active  LOW)");
    Serial.print(button2.isMonitoring()?"  monitored ":"   idle ");
    Serial.print((long)button2.getCallback(), HEX);
    Serial.print(button2.isInCallback()?"  inCB ":" !inCB ");
    Serial.println();
  }
 */

  digitalWrite(BOARD_LED, HIGH); 
  delay(50);
  digitalWrite(BOARD_LED, LOW); 
  delay(50);
  digitalWrite(BOARD_LED, HIGH); 
  delay(50);
  digitalWrite(BOARD_LED, LOW); 
  delay(50);
    
  delay(800);
}

