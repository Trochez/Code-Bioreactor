#ifdef WGHT

NIL_WORKING_AREA(waThreadWeight, 16);    
NIL_THREAD(ThreadWeight, arg) {
    
    //quick init of the default parameters
    setParameter(PARAM_WAIT_TIME_PUMP_MOTOR,10);    //wait time of 10 seconds, to be changed then
    setParameter(PARAM_LVL_MAX_WATER,420);
    setParameter(PARAM_LVL_MIN_WATER,370);
    setParameter(PARAM_RELAY_PUMP,0);
    setParameter(FLAG_VECTOR,0);
    
    int leaky_wght;
    int weight=analogRead(WGHT);
    
    while(true){ 
 
      //sensor read 
      leaky_wght=weight;
      weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
      
      //implement a conversion to give the volume rather than analog read.
      setParameter(PARAM_WGHT, weight);
     
      #ifdef RELAY_PUMP
      
        if(weight>=getParameter(PARAM_LVL_MAX_WATER)){
          //set the condition to initiate pumping sequence
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_STEPPER_OFF));
          unsigned long time=now();        
          while((now()-time)<=getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
             leaky_wght=weight;
             weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
             setParameter(PARAM_WGHT, weight);
             nilThdSleepMilliseconds(1000);  
          }
          //enable pumping
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_PUMPING));
          setParameter(PARAM_RELAY_PUMP,(getParameter(PARAM_RELAY_PUMP)||8)); 
          Serial.print("param_relay");
          Serial.println(getParameter(PARAM_RELAY_PUMP));      
          
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
          setParameter(PARAM_RELAY_PUMP,(getParameter(PARAM_RELAY_PUMP)&~(8))); 
          Serial.print("param_relay");
          Serial.println(getParameter(PARAM_RELAY_PUMP));   
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_PUMPING)));
          nilThdSleepMilliseconds(1000); 
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_STEPPER_OFF)));

        }
      
      #endif
      
      nilThdSleepMilliseconds(1000);  //refresh every second
    }
}

#endif
