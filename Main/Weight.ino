#ifdef WGHT

NIL_WORKING_AREA(waThreadWeight, 32);    
NIL_THREAD(ThreadWeight, arg) {
    
    //quick init of the default parameters
    if(getParameter(PARAM_WAIT_TIME_PUMP_MOTOR) != MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_WAIT_TIME_PUMP_MOTOR,10);    //wait time of 10 seconds, to be changed then
    }  

    if(getParameter(PARAM_LVL_MAX_WATER) != MAX_INTEGER)
    {    
      setAndSaveParameter(PARAM_LVL_MAX_WATER,420);
    }
      
    if(getParameter(PARAM_LVL_MIN_WATER) != MAX_INTEGER)
    {    
      setAndSaveParameter(PARAM_LVL_MIN_WATER,370); 
    }  
      
    if(getParameter(PARAM_RELAY_PUMP) != MAX_INTEGER)  
    {
      setAndSaveParameter(PARAM_RELAY_PUMP,0);
    }
      
    if(getParameter(FLAG_VECTOR) != MAX_INTEGER)
    {
      setAndSaveParameter(FLAG_VECTOR,0);
    }
    
    int leaky_wght;
    int weight = analogRead(WGHT);
    
    while(true){ 
 
      //sensor read 
      leaky_wght=weight;
      weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
      
      //implement a conversion to give the volume rather than analog read.
      setParameter(PARAM_WGHT, weight);
     
      #ifdef RELAY_PUMP
      
        if(weight>=getParameter(PARAM_LVL_MAX_WATER)){
          //set the condition to initiate pumping sequence
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) | FLAG_STEPPER_OFF));
          writeLog(COMMAND_LOGS, &newEntryCmd, now(), MOTOR_STOP, 0);
          unsigned long time=now();        
          while((now()-time)<=getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
             leaky_wght=weight;
             weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
             setParameter(PARAM_WGHT, weight);
             nilThdSleepMilliseconds(1000);  
          }
          //enable pumping
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) | FLAG_PUMPING));
          setParameter(PARAM_RELAY_PUMP,(getParameter(PARAM_RELAY_PUMP)|8));
          writeLog(COMMAND_LOGS, &newEntryCmd, now(), PUMPING_START, 0);
          
        //add a sanity check here if the value is out of range of if the weight does not change after pumping for a while (ERROR_WGHT_CTRL)
        //condition would restart the mixing and disable pumping
          /*
          while(weight>=getParameter(PARAM_LVL_MIN_WATER)){
             weight = analogRead(WGHT);
             setParameter(PARAM_WGHT, weight);
             nilThdSleepMilliseconds(1000);  
          }  */
        }
        
        if( (weight<=getParameter(PARAM_LVL_MIN_WATER)) & ((getParameter(FLAG_VECTOR)&(FLAG_STEPPER_OFF))==(FLAG_STEPPER_OFF))){
          //set the condition to disable the pumping sequence and re-anable stepper motor mixing
          setParameter(PARAM_RELAY_PUMP, (getParameter(PARAM_RELAY_PUMP)&~8));
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) & ~(FLAG_PUMPING)));
          writeLog(COMMAND_LOGS, &newEntryCmd, now(), PUMPING_STOP, 0);
          nilThdSleepMilliseconds(1000); 
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) & ~(FLAG_STEPPER_OFF)));
          writeLog(COMMAND_LOGS, &newEntryCmd, now(), MOTOR_START, 0);

        }
      
      #endif
      
      nilThdSleepMilliseconds(1000);  //refresh every second
    }
}

#endif
