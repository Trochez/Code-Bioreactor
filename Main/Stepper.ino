/* Thread Stepper Motor
This is the thread controlling the motor. It should have a high priority 
as it is called very often and is short. It controls the sequence with the pin PWM and IO
of the port.
The sequence for turning the motor turn is :
RED-GREEN-BLUE-BLACK => win turn clockwise (top view) where :
RED = {PWM=LOW, IO=LOW}
BLUE = {PWM=LOW, IO=HIGH}
BLACK = {PWM=HIGH, IO=HIGH}
RED = {PWM=HIGH, IO=LOW}
*/

//We define here the number of step executed during every call to the thread
#ifdef STEPPER
#define NB_STEP_CALL 60




NIL_WORKING_AREA(waThreadStepper, 0);
NIL_THREAD(ThreadStepper, arg) {
  byte STEPPER_TAB[]=STEPPER;
  boolean forward = true;
  uint8_t count = 0;
  for (byte i=0; i<sizeof(STEPPER_TAB); i++) {
    pinMode(STEPPER_TAB[i], OUTPUT);    
  }
  while (1) {
     executeStep(NB_STEP_CALL, forward, STEPPER_TAB[1],STEPPER_TAB[0]);
     //The final delay has to be decided 
     if(!(count % 40)) {
       forward = !forward;
     }
     count = (count + 1) % 40;
     nilThdSleepMilliseconds(10);
  }
}

void executeStep(int numberSteps, boolean forward, byte port1, byte port2) {
  int counter=1000;
  while (numberSteps>0) {
    numberSteps--;
    if (forward) {
      counter++;
    }
    else { 
      counter--;
    }
    
    switch (counter % 4) {
      case 0:
        //This is RED
        digitalWrite(port1, LOW);
        digitalWrite(port2,LOW);
        break;
      case 2:
        //This is BLUE
        digitalWrite(port1, LOW);
        digitalWrite(port2,HIGH);
        break;
      case 3:
      //This is Black
        digitalWrite(port1, HIGH);
        digitalWrite(port2,HIGH);
        break;
      case 1:
      //This is Green
        digitalWrite(port1, HIGH);
        digitalWrite(port2,LOW);
        break;
    }
    delay(5);
  } 

}

#endif

