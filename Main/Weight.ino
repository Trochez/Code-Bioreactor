#ifdef WGHT

NIL_WORKING_AREA(waThreadWeight, 16);    
NIL_THREAD(ThreadWeight, arg) {
    uint16_t weight = 0;
    while(true){ 
      
      //sensor read
      weight = analogRead(WGHT);
      setParameter(PARAM_WGHT, weight);
      
      //add a sanity check if the value is out of range of if the weight does not change after pumping for a while (ERROR_WGHT_CTRL)
      //condition would restart the mixing and disable pumping

      if(weight>=PARAM_LVL_MAX_WATER){
        //set the condition to initiate pumping sequence
        setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_STEPPER_OFF));
        time=now();        
        while((now()-time)<=getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
           weight = analogRead(WGHT);
           setParameter(PARAM_WGHT, weight);
           nilThdSleepMilliseconds(1000);  
        }
        setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_PUMPING));
        
      }
      
      if(weight<=PARAM_LVL_MIN_WATER){
        //set the condition to disable the pumping sequence and re-anable stepper motor mixing
        setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_STEPPER_OFF)));
        setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_PUMPING)));
      }
     
      
      nilThdSleepMilliseconds(1000);  //refresh every second
    }
}

#endif
