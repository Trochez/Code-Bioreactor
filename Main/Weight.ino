#ifdef WGHT

//TO BE DONE
//implement a conversion to give the volume rather than analog read.
//hard coded safety value, TO BE CHANGED ONCE THE SENSOR IS CALIBRATED and conversion performed automatically !!!!!!!!!
#define MAX_ABSOLUTE_WGHT          600

NIL_WORKING_AREA(waThreadWeight, 64);    
NIL_THREAD(ThreadWeight, arg) {
  
    int leaky_wght;
    int weight = analogRead(WGHT);
    
    while(true){ 
      
      //sensor read 
      leaky_wght=weight;
      weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
      setParameter(PARAM_WGHT, weight);      
     
      #ifdef RELAY_PUMP
      
        if(weight>=getParameter(PARAM_LVL_MAX_WATER)){
          //set the condition to initiate pumping sequence
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) | FLAG_STEPPER_OFF));
          
          while(!sendError(MOTOR_STOP, 0)){
            nilThdSleepMilliseconds(220); 
          }
          
          unsigned long time=now();        
          while((now()-time)<=getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
             leaky_wght=weight;
             weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
             setParameter(PARAM_WGHT, weight);
             nilThdSleepMilliseconds(500);  
             
          /*********
          sanity check for sensor failure
          **********/
          boolean weight_failure=false;
          
          while(weight>MAX_ABSOLUTE_WGHT)
          {
            
            if(weight_failure==false){
              if(sendError(WGHT_FAILURE, weight)){
                weight_failure=true;   
              }
            }
            
            leaky_wght=weight;
            weight = 0.9*leaky_wght+0.1*analogRead(WGHT);
            setParameter(PARAM_WGHT, weight);
            
            if((weight <= MAX_ABSOLUTE_WGHT)) {
              while(!sendError(WGHT_BACK_TO_NORMAL, weight)); 
              break;
            }
            nilThdSleepMilliseconds(500); 
          }    
             
        }
          
        //enable pumping
        setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) | FLAG_PUMPING));
        setParameter(PARAM_RELAY_PUMP,(getParameter(PARAM_RELAY_PUMP)|8));
        
        while(!sendError(PUMPING_START, 0)){
          nilThdSleepMilliseconds(220); 
        }
          
          
          
        //add a sanity check here if the value is out of range or if the weight does not change after pumping for a while (ERROR_WGHT_CTRL)
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
 
          while(!sendError(PUMPING_STOP, 0)){
            nilThdSleepMilliseconds(220); 
          }
          
          nilThdSleepMilliseconds(1000); 
          
          setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) & ~(FLAG_STEPPER_OFF)));
          
          while(!sendError(MOTOR_START, 0)){
            nilThdSleepMilliseconds(220); 
          }

        }
      
      #endif
      
      nilThdSleepMilliseconds(500);  //refresh every second
    }
}

#endif
