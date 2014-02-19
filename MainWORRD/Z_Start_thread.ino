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

#ifdef THR_LINEAR_LOGS
NIL_THREADS_TABLE_ENTRY(NULL, ThreadLogger, NULL, waThreadLogger, sizeof(waThreadLogger))
#endif

#ifdef  STEPPER
NIL_THREADS_TABLE_ENTRY(NULL, ThreadStepper, NULL, waThreadStepper, sizeof(waThreadStepper))
#endif

#ifdef PH_CTRL
NIL_THREADS_TABLE_ENTRY(NULL, ThreadPH, NULL, waThreadPH, sizeof(waThreadPH))
#endif

#ifdef  GAS_CTRL 
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTap, NULL, waThreadTap, sizeof(waThreadTap))
#endif  

#if defined(TEMP_LIQ) || defined(TEMP_PLATE) || defined(TEMP_STEPPER)
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTemp, NULL, waThreadTemp, sizeof(waThreadTemp))                        
#endif

#ifdef TRANS_PID
NIL_THREADS_TABLE_ENTRY(NULL, ThreadRelay_PID, NULL, waThreadRelay_PID, sizeof(waThreadRelay_PID))      
#endif

#ifdef WGHT
NIL_THREADS_TABLE_ENTRY(NULL, ThreadWeight, NULL, waThreadWeight, sizeof(waThreadWeight))
#endif

#if defined(GAS_CTRL) || defined(STEPPER_CTRL) || defined(I2C_LCD) || defined(PH_CTRL)
NIL_THREADS_TABLE_ENTRY(NULL, ThreadWire, NULL, waThreadWire, sizeof(waThreadWire))
#endif

#ifdef SERIAL
NIL_THREADS_TABLE_ENTRY(NULL, ThreadSerial, NULL, waThreadSerial, sizeof(waThreadSerial))
#endif

#if defined(THR_ETHERNET)
NIL_THREADS_TABLE_ENTRY(NULL, ThreadEthernet, NULL, waThreadEthernet, sizeof(waThreadEthernet))
#endif


// This thread is mandatory and check that nothing is currently blocking the arduino
// otherwise there is an automatic RESET 
NIL_THREADS_TABLE_ENTRY(NULL, ThreadMonitoring, NULL, waThreadMonitoring, sizeof(waThreadMonitoring))

NIL_THREADS_TABLE_END()






