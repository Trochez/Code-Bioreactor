#include <Wiegand.h>
WIEGAND wg;


NIL_WORKING_AREA(waThreadWiegand, 256);
NIL_THREAD(ThreadWiegand, arg) {


  // we should here create all the Wiegand listener for the 5 ports
  // using a loop ???????

  wg.begin();


  while(true) {
    // should check if there is one of the door that received a Weigand info using a loop
    // if yes: check if allowed or not
    // log the event (allowed / not allowed ?)
    if(wg.available())
    {
      Serial.print("Wiegand HEX = ");
      Serial.print(wg.getCode(),HEX);
      Serial.print(", DECIMAL = ");
      Serial.print(wg.getCode());
      Serial.print(", Type W");
      Serial.println(wg.getWiegandType());    
    }



    nilThdSleepMilliseconds(5);
  }

}









