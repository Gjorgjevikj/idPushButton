/*
  FILE:     idPushButton.h
  VERSION:  0.3
  PURPOSE:  Interrupt driven library for push buttons on Arduino.
  LICENCE:  Apache v2 ()

  idPushButton v 0.3 - Interrupt driven push button monitor library with simple debouncing. 
  Triggers a call to a user defined call back function on press or release (configurable) 
  of the button. When reacting on release, reports back the duration in milliseconds the 
  button has been held down before releasing so that long and short presses can be determined 
  inside the user function invoked by the button.

  Active on low and active on high push buttons are supported. If active on low (when pushed connects the pinPB to GND) 
  internal pullup resistor is configured, so there is no need for installing a pullup resistor in the circuitry
  BUT if active on high pushbutton is used a pulldown resistor MUST be installed in the circuitry. The push button 
  objects act independently and can innterupt the service of others push buttons, but the own one (to avoid recursion)

  Parameters that are specified in definition of the PBMonitor object:
  - The pinPB to which the button is connected 
  - The type of the button - active high or low (will behave strange if not defined according to the way it is connected)
  - The function to be called when the button is pressed (released to be exact)
  - The function to serve as a ISR - must be void f() and must contain a call to the change() member function of that 
    PBMonitor class for the object (can be generated automatically if the macro definition for declaring a PBmonitor is used)
  - The minimum time pressed to be registered - to avoid bouncing (a press shorter than the specified time will be ignored) 
 
 created 08.05.2016
 by Dejan Gjorgjevikj
 
 */ 

#ifndef idPushButton_H__
#define idPushButton_H__

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define IDPUSHBUTTON_VERSION "0.2" 

//#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
// from https://github.com/GreyGnome/EnableInterrupt.git


// Macros to automate Push button object instatiation with interrupt service routine global function definition 
// Interrupts needs a void function (not part of the class) to act as interrupt service routine that must be defined outside the class 
// as a wrapper to member function to be called on button change state (from the interrupt)
#define SET_PB(PBUTTON, PIN_BUT, FP_CALLB, FP_ISR, TYPE, POR, T_IGNORE) PBmonitor PBUTTON(PIN_BUT, FP_CALLB, PBUTTON##_ISR, TYPE, POR, T_IGNORE); \
void PBUTTON##_ISR() { PBUTTON.change(); }
#define PUSH_BUTTON_L(PBUTTON, PIN_BUT, FP_CALLB, POR) PBmonitor PBUTTON(PIN_BUT, FP_CALLB, PBUTTON##_ISR, LOW, POR); \
void PBUTTON##_ISR() { PBUTTON.change(); }
#define PUSH_BUTTON_H(PBUTTON, PIN_BUT, FP_CALLB, POR) PBmonitor PBUTTON(PIN_BUT, FP_CALLB, PBUTTON##_ISR, HIGH, POR); \
void PBUTTON##_ISR() { PBUTTON.change(); }

typedef void (*ISR)(); // pointer to void function (to act as interrupt service routine)
typedef void (*PBcallback) (unsigned long); // pointer to void function taking one int (should be unsigned long) 
// to be called from the push button monitor object when the button wil be released passing the number od miliseconds the button was held down

#define ONRELEASE false
#define ONPRESS   true

class PBmonitor
{
  public:
    PBmonitor(uint8_t pinPBNo, PBcallback f, ISR isrv, bool act = LOW, bool fon = ONRELEASE, unsigned long t_i = 20) : 
      pinPB(pinPBNo), callback(f), isr(isrv), debounceDelay(t_i), elapsedMils(0) 
    { 
      bp.type = act;
      bp.firesOn = fon;
      bp.inCallback = false;
      bp.prevState = !bp.type;
      bp.monitoring = false;
    }
    ~PBmonitor() { stopMonitoring(); }
    void startMonitoring() 
    { 
      digitalWrite(pinPB, !bp.type); 
      pinMode(pinPB, bp.type ? INPUT : INPUT_PULLUP); 
      bp.prevState = digitalRead(pinPB);
      elapsedMils=0;
      bp.monitoring=true;
      uint8_t oldSREG = SREG; // Save the status
      noInterrupts();
      // attachInterrupt(digitalPinToInterrupt(pinPB), isr, CHANGE); // set interrupt on change
      enableInterrupt( pinPB, isr, CHANGE);
      SREG = oldSREG;
    }

    void stopMonitoring() 
    { 
      // detachInterrupt(digitalPinToInterrupt(pinPB)); 
      disableInterrupt(pinPB);
      bp.monitoring=false; 
    }
        
    void change() 
    { 
      bool pushRegistered=false;
      uint8_t oldSREG = SREG; // Save the status
      interrupts();
      unsigned long now=millis();
      SREG = oldSREG;
      bool currentState = digitalRead(pinPB);
      
      if(bp.prevState == !bp.type && currentState == bp.type)
      {
        elapsedMils = now; // just store the time when pushed down
        pushRegistered = bp.firesOn;
      }
      else if(bp.prevState == bp.type && currentState == !bp.type) // released 
      {
        pushRegistered = !bp.firesOn;
      } //  else ... no change so do nothing
      
      bp.prevState=currentState;
      if(pushRegistered && (bp.firesOn || now - elapsedMils > debounceDelay) && !bp.inCallback) // was pressed long enought and not servicing previous press
      {
        bp.inCallback=true;
        uint8_t oldSREG = SREG; // Save the status
        interrupts();
        callback(now - elapsedMils);
        SREG = oldSREG;
        bp.inCallback=false;
      }
    }

    // if not used this functions can be commented out to save space
    bool type() const { return bp.type; }
    bool isMonitoring() const { return bp.monitoring; } // is monitored = ISR installed
    void setUBdelay(unsigned long t) { debounceDelay = t; }
    unsigned long getUBdelay(void) const { return debounceDelay; }
    bool isInCallback() const { return bp.inCallback; }
    void setCallback(PBcallback f) { callback=f; }
    PBcallback getCallback() const { return callback; }
    bool getFiresOn() const { return bp.firesOn; }
    void setFiresOn(bool fon) { bp.firesOn = fon; }

  protected:
    uint8_t pinPB; // pinPB at which change of level is monitored
    PBcallback callback; // pointer to function to be called when button is pressed
    ISR isr; // pointer to void f() function to serve as interrupt service routine - must be defined on a global scope
    unsigned long elapsedMils; // elapsed millis since the last call to ISR
    unsigned long debounceDelay; // time to be ignorred - changes that appear @ t < debounceDelay will be ignored
    struct bitPack // saves space packing all bool data memebers in single bute
    {
      uint8_t type:1; // type of the button: active on high or active on low
      uint8_t firesOn:1; // when to react (call the callbak) on press (true) or on release (false)
      uint8_t inCallback:1; // true while executing the callback function, will not re-enter the function from the same interrupt (isr disabled)
      uint8_t prevState:1; // previous state of the pinPB (checked in the ISR)
      uint8_t monitoring:1; // is active and monitoring the push button
    } bp;
};


#endif //idPushButton_H__

