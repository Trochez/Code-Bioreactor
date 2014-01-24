// Nice way to make some monitoring about activity. This should be the lower priority process
// If the led is "stable" (blinks 500 times per seconds) it means there are not too
// many activities on the microcontroler

#ifdef THR_MONITORING
  NIL_WORKING_AREA(waThreadMonitoring, 0);
  NIL_THREAD(ThreadMonitoring, arg) {
    boolean turnOn=true;
    pinMode(THR_MONITORING, OUTPUT);   
    while (TRUE) {
      if (turnOn) {
        turnOn=false;
        digitalWrite(THR_MONITORING,HIGH);
      } 
      else {
        turnOn=true;
        digitalWrite(THR_MONITORING,LOW);
      }
      nilThdSleepMilliseconds(500);
    }
  }
#endif


NIL_THREADS_TABLE_BEGIN()


NIL_THREADS_TABLE_ENTRY(NULL, ThreadLogger, NULL, waThreadLogger, sizeof(waThreadLogger))

#ifdef SERIAL
NIL_THREADS_TABLE_ENTRY(NULL, ThreadSerial, NULL, waThreadSerial, sizeof(waThreadSerial))
#endif




#if defined(TEMP_LIQ) || defined(TEMP_PLATE) || defined(TEMP_STEPPER)
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTemp, NULL, waThreadTemp, sizeof(waThreadTemp))                        
#endif


#ifdef THR_MONITORING 
NIL_THREADS_TABLE_ENTRY(NULL, ThreadMonitoring, NULL, waThreadMonitoring, sizeof(waThreadMonitoring))
#endif

NIL_THREADS_TABLE_END()
