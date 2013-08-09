#ifdef STEPPER

NIL_WORKING_AREA(waThreadStepper, 0);
NIL_THREAD(ThreadStepper, arg) {
  byte STEPPER_TAB[]=STEPPER;
  for (byte i=0; i<sizeof(STEPPER_TAB); i++) {
    pinMode(STEPPER_TAB[i], OUTPUT);    
  }
  while (TRUE) {
    for (int i=0; i<200; i=i+20) {
      //condition on the STEPPER flag                                          CHECK IF THIS CONDITION IS AT THE RIGHT PLACE HERE !!!!!!!
      if((getParameter(FLAG_VECTOR)&&FLAG_STEPPER_OFF)==0){
        executeStep(i,true, 4, STEPPER_TAB[0], STEPPER_TAB[1]);
        executeStep(i,false, 4, STEPPER_TAB[0], STEPPER_TAB[1]);
    }
    }
  }
}

int counter=0;

void executeStep(int numberSteps, boolean forward, byte currentDelay, byte port1, byte port2) {
  while (numberSteps>0) {
    numberSteps--;
    if (forward) {
      counter++;
    }
    else { 
      counter--;
    }
    switch (counter%4) {
    case 0:
      digitalWrite(port1, LOW);
      digitalWrite(port2,LOW);
      break;
    case 1:
      digitalWrite(port1, LOW);
      digitalWrite(port2,HIGH);
      break;
    case 2:
      digitalWrite(port1, HIGH);
      digitalWrite(port2,HIGH);
      break;
    case 3:
      digitalWrite(port1, HIGH);
      digitalWrite(port2,LOW);
      break;
    }
    nilThdSleepMilliseconds(currentDelay);
  }
}

#endif
