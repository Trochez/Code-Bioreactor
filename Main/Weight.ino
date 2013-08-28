#ifdef WGHT

NIL_WORKING_AREA(waThreadWeight, 16);    
NIL_THREAD(ThreadWeight, arg) {
    uint16_t weight = 0;
    while(true){ 
      
      //sensor read
      weight = analogRead(WGHT);
      setParameter(PARAM_WGHT, weight);
     
      #ifdef RELAY_PUMP
      
        if(weight>=getParameter(PARAM_LVL_MAX_WATER)){
          //set the condition to initiate pumping sequence
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_STEPPER_OFF));
          unsigned long time=now();        
          while((now()-time)<=getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
             weight = analogRead(WGHT);
             setParameter(PARAM_WGHT, weight);
             nilThdSleepMilliseconds(1000);  
          }
          //enable pumping
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_PUMPING));
          setParameter(PARAM_RELAY_PUMP,getParameter(PARAM_RELAY_PUMP)&(~(1<<3))); //to discuss if we keep like that or not
          
        //add a sanity check here if the value is out of range of if the weight does not change after pumping for a while (ERROR_WGHT_CTRL)
        //condition would restart the mixing and disable pumping
          /*
          while(weight>=getParameter(PARAM_LVL_MIN_WATER)){
             weight = analogRead(WGHT);
             setParameter(PARAM_WGHT, weight);
             nilThdSleepMilliseconds(1000);  
          }  */
        }
        
        if(weight<=getParameter(PARAM_LVL_MIN_WATER)){
          //set the condition to disable the pumping sequence and re-anable stepper motor mixing
          setParameter(PARAM_RELAY_PUMP,getParameter(PARAM_RELAY_PUMP)||(1<<3));    
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_STEPPER_OFF)));
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_PUMPING)));
        }
      
      #endif
      
      nilThdSleepMilliseconds(1000);  //refresh every second
    }
}

#endif
