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

#ifdef SERIAL
NIL_THREADS_TABLE_ENTRY(NULL, ThreadSerial, NULL, waThreadSerial, sizeof(waThreadSerial))
#endif

#if defined(THR_ETHERNET) || defined(THR_LINEAR_LOGS)
NIL_THREADS_TABLE_ENTRY(NULL, ThreadEthernet, NULL, waThreadEthernet, sizeof(waThreadEthernet))
#endif

#ifdef PH_CTRL
//NIL_THREADS_TABLE_ENTRY(NULL, ThreadPH, NULL, waThreadPH, sizeof(waThreadPH))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTap, NULL, waThreadTap, sizeof(waThreadTap))
#endif

#ifdef  GAS_CTRL 
//NIL_THREADS_TABLE_ENTRY(NULL, ThreadTap, NULL, waThreadTap, sizeof(waThreadTap))
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

#ifdef  STEPPER
NIL_THREADS_TABLE_ENTRY(NULL, ThreadStepper, NULL, waThreadStepper, sizeof(waThreadStepper))
#endif

#ifdef THR_MONITORING 
NIL_THREADS_TABLE_ENTRY(NULL, ThreadMonitoring, NULL, waThreadMonitoring, sizeof(waThreadMonitoring))
#endif

NIL_THREADS_TABLE_END()
