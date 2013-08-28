#ifdef  RELAY_PUMP

NIL_WORKING_AREA(waThreadRelay, 70);      
NIL_THREAD(ThreadRelay, arg) 
{
  level_ctrl(); 
  nilThdSleepMilliseconds(500);  //refresh every 500ms
}

/* Level control addressing the pump on I2C connector */

void  level_ctrl(){
  //pump addressed only when the maximum tank level is reached
  if((getParameter(FLAG_VECTOR)&FLAG_STEPPER_OFF)==FLAG_STEPPER_OFF){  
    unsigned long timer=now();
    
    //wait for sedimentation
    while( (now() - timer) <getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
       nilThdSleepMilliseconds(500); 
    }    

    setParameter(FLAG_VECTOR,getParameter(FLAG_VECTOR)||FLAG_PUMPING);
    //turns relay ON   ------> NOT ENOUGH !!!!!!!  need to understand "Wire" Thread to implement that
    setParameter(PARAM_RELAY_PUMP,getParameter(PARAM_RELAY_PUMP)&(~(1<<3))); 
    
    while((getParameter(FLAG_VECTOR)&FLAG_PUMPING)==FLAG_PUMPING){
      nilThdSleepMilliseconds(500); 
    }
    
    //turns relay OFF  ------> NOT ENOUGH !!!!!!!  need to understand "Wire" Thread to implement that
    setParameter(PARAM_RELAY_PUMP,getParameter(PARAM_RELAY_PUMP)||(1<<3));
  }
}

#endif
