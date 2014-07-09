// Nice way to make some monitoring about activity. This should be the lower priority process
// If the led is "stable" (blinks 500 times per seconds) it means there are not too
// many activities on the microcontroler


NIL_WORKING_AREA(waThreadMonitoring, 0);
NIL_THREAD(ThreadMonitoring, arg) {

  // we should not start the watchdog too quickly ...
  nilThdSleepMilliseconds(30000);

  // we activate the watchdog
  // we need to make a RESET all the time otherwise automatic reboot: wdt_reset();
  wdt_enable(WDTO_8S);




  boolean turnOn=true;
#ifdef THR_MONITORING
  pinMode(THR_MONITORING, OUTPUT);   
#endif
  while (TRUE) {
#ifdef THR_MONITORING
    if (turnOn) {
      turnOn=false;
      digitalWrite(THR_MONITORING,HIGH);
    } 
    else {
      turnOn=true;
      digitalWrite(THR_MONITORING,LOW);
    }
#endif

    nilThdSleepMilliseconds(100);

    wdt_reset();
  }
}



NIL_THREADS_TABLE_BEGIN()

#ifdef SERIAL
NIL_THREADS_TABLE_ENTRY(NULL, ThreadSerial, NULL, waThreadSerial, sizeof(waThreadSerial))
#endif


NIL_THREADS_TABLE_ENTRY(NULL, ThreadWiegand, NULL, waThreadWiegand, sizeof(waThreadWiegand))

NIL_THREADS_TABLE_ENTRY(NULL, ThreadDoorStatus, NULL, waThreadDoorStatus, sizeof(waThreadDoorStatus))

#ifdef THR_ZIGBEE
NIL_THREADS_TABLE_ENTRY(NULL, ThreadZigbee, NULL, waThreadZigbee, sizeof(waThreadZigbee))
#endif


// This thread is mandatory and check that nothing is currently blocking the arduino
// otherwise there is an automatic RESET 
NIL_THREADS_TABLE_ENTRY(NULL, ThreadMonitoring, NULL, waThreadMonitoring, sizeof(waThreadMonitoring))

NIL_THREADS_TABLE_END()











