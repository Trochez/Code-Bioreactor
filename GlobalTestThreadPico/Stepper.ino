// Stepper motor thread, highest priority module


NIL_WORKING_AREA(waThreadStepper, 0);        //memory allocation ??
NIL_THREAD(ThreadStepper, arg) {
  
    int stepper= findStepper();
    pinMode(Device[stepper].io(),OUTPUT); 
    pinMode(Device[stepper].pwm(),OUTPUT); 
    
  while (TRUE /* set condition here on the presence or not of a motor not to recheck the position of the motor each time*/) {
    for (int i=0; i<200; i=i+20) {
      executeStep(i,true, 4, Device[stepper].pwm(), Device[stepper].io());
      executeStep(i,false, 4, Device[stepper].pwm(), Device[stepper].io());
    }
  }
}


//find if a Stepper motor is connected
unint8_t findStepper() {
    for(char i=0;i<MAX_DEVICES;i++)
    {
      switch(Device[i].type)
      {
        case STEPPER :
          return i;
        default :
          break;
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



