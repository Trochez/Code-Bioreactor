// Nice way to make some monitoring about activity. This should be the lower priority process
// If the led is "stable" (blinks 500 times per seconds) it means there are not too
// many activities on the microcontroler
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
    nilThdSleepMilliseconds(1);
  }
}


NIL_THREADS_TABLE_BEGIN()

#ifdef PH_CTRL

NIL_THREADS_TABLE_ENTRY(NULL, ThreadPH, NULL, waThreadPH, sizeof(waThreadPH))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTap, NULL, waThreadTap, sizeof(waThreadTap))

#endif


#ifdef  GAS_CTRL 

NIL_THREADS_TABLE_ENTRY(NULL, ThreadFlux, NULL, waThreadFlux, sizeof(waThreadFlux))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTap, NULL, waThreadTap, sizeof(waThreadTap))

#endif  


#ifdef  TEMP_CTRL 

//NIL_THREADS_TABLE_ENTRY(NULL, ThreadRelay_PID, NULL, waThreadRelay_PID, sizeof(waThreadRelay_PID))  TODO: Enlever comment
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTemp, NULL, waThreadTemp, sizeof(waThreadTemp))                

#endif


#ifdef  STEPPER_CTRL

NIL_THREADS_TABLE_ENTRY(NULL, ThreadStepper, NULL, waThreadStepper, sizeof(waThreadStepper)
NIL_THREADS_TABLE_ENTRY(NULL, ThreadWeight, NULL, waThreadWeight, sizeof(waThreadWeight))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadRelay, NULL, waThreadRelay, sizeof(waThreadRelay))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTap, NULL, waThreadTap, sizeof(waThreadTap))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadTemp, NULL, waThreadTemp, sizeof(waThreadTemp))

#endif

#ifdef THR_LINEAR_LOGS
//NIL_THREADS_TABLE_ENTRY(NULL, ThreadLinearLog, NULL, waThreadLinearLog, sizeof(waThreadLinearLog))
#endif

#ifdef THR_ETHERNET
//NIL_THREADS_TABLE_ENTRY(NULL, ThreadEthernet, NULL, waThreadEthernet, sizeof(waThreadEthernet))
#endif

NIL_THREADS_TABLE_ENTRY(NULL, ThreadSerial, NULL, waThreadSerial, sizeof(waThreadSerial))
NIL_THREADS_TABLE_ENTRY(NULL, ThreadWire, NULL, waThreadWire, sizeof(waThreadWire))

#ifdef THR_MONITORING 
NIL_THREADS_TABLE_ENTRY(NULL, ThreadMonitoring, NULL, waThreadMonitoring, sizeof(waThreadMonitoring))
#endif

NIL_THREADS_TABLE_END()
