#ifdef MODEL_ZIGBEE

NIL_WORKING_AREA(waThreadZigbee, 0);
NIL_THREAD(ThreadZigbee, arg) {

  Serial1.begin(9600);
  while(true) {
    while (Serial1.available()) {
      // get the new byte:
      char inChar = (char)Serial1.read(); 
      Serial.print(inChar);
      }  
    }
    nilThdSleepMilliseconds(1);
  }
}

#endif




