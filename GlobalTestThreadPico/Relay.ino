#ifdef  RELAY_PUMP || RELAY_PID

NIL_WORKING_AREA(waThreadRelay, 70);      
NIL_THREAD(ThreadRelay, arg) 
{
  #ifdef  RELAY_PUMP
  level_ctrl();
  #endif
  
  #ifdef  RELAY_PID
  pid_ctrl();
  #endif
    
  nilThdSleepMilliseconds(500);  //refresh every 500ms
}


void  level_ctrl(){
  //pump addressed only when the maximum tank level is reached
  if((getParameter(FLAG_VECTOR)&&FLAG_STEPPER_OFF)==FLAG_STEPPER_OFF){        
  
    //write pumping code : 1) initiate timer (the stepper thd will stop the motor !!)
    //                     2) while time is below 'PARAM_WAIT_PUMP' nilThdSleepMilliseconds(500);
    //                     3) when timer reach delay, set flag pumping ON till the level of the tank PARAM_LVL_MIN
    //                     4) set FLAG_STEPPER_OFF and FLAG_PUMPING to 0
  ;
  }
}

void  pid_ctrl(){
  
  //write code inspired from Oliver's
  ;
}

#endif
