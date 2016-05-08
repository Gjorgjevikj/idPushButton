# idPushButton
Arduino Libraries and Projects
Arduino Interrupt Driven Push Button monitoring

Recently I came to a need to act on a push button that will be monitored by an external interrupt. I came to the idea of developing a simple class library that will automate the use of push button object configured to a (hardware interrupt supported) pin that will fire a user defined function once pressed (actually once released to be exact). So here it is, hopefully someone else can find a use of it (or make a comment). 

It uses EnableInterrupt.h <from https://github.com/GreyGnome/EnableInterrupt.git> to enable interrupts on all pins. Performs basic software debouncing and returns the duration the button has been pressed (when fire_on_release is configured). Supports active low (connecting the pin to GND) and active high (connecting the pin to Vcc) pushbuttons.

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
  - The pin to which the button is connected (all pins and platforms supported by EnableInterrupt library)
  - The type of the button - active high or low (will behave strange if not defined according to the way it is connected)
  - The activation type - fire on press (key-down) or release (key-up)
  - The function to be called when the button is pressed (released to be exact)
  - The function to serve as a ISR - must be void f() and must contain a call to the Change() member function of that PBMonitor class for the object (can be generated automatically if the macro definition for declaring a PBmonitor is used)
  - The minimum time pressed to be registered - to avoid bouncing (a press shorter than the specified time will be ignored)

Interrupts needs a void function (not part of the class) to act as interrupt service routine that must be defined outside the class as a wrapper to member function to be called on button change state (from the interrupt). Macros to automate Push button object instatiation with automatic interrupt service routine (global void function) definition are provided.

Finally, since everything is interrupt driven and working in the background (the loop is empty), even the use defined function to be activated when the button is pressed is called from the interrupt. These user defined functions can last and usually have a need of enabled interrupts, so a recursive call of an interrupt from another interrupt is possible (though not the same one). As a result if the button is pressed multiple times while the used defined function was executing, this button press events (of the same button) will be ignored. If you need to act on them please see the other branch of this project idPushButtonQueued. This example uses 2 interrupt driven push buttons at the same time.
