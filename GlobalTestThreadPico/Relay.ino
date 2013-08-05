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
    
  nilSleepMilliseconds(500);  //refresh every 500ms
}


void  level_ctrl(){
  ;
}

void  pid_ctrl(){
  ;
}

#endif


/* rewritre everything here
Must control the delay off motor shutdown by playing with the flags FLAG_STEPPER_OFF and FLAG_PUMPING


void pumping(){                       
  if((getParameter(PARAM_LVL_MAX)<getParameter(PARAM_WGHT))&                // if level max reached and FLAG_MOTOR_OFF to 0, flag set to 1
     ((getParameter(PARAM_FLAG) & FLAG_MOTOR_OFF)== TRUE))
  {  
    setParameter(PARAM_FLAG,getParameter(PARAM_FLAG)||(FLAG_MOTOR_OFF));
    int delay_counter=0;                                                    // delay_counter linked to timer, no pumping for 30 min
  }
 
  if(((getParameter(PARAM_FLAG) & FLAG_MOTOR_OFF)== TRUE))
  {  //condition compteur timer + flag motor
//    nilThdSleepMilliseconds(120000);                                      //2 min motor stop (change to 10 minutes) --> pb, no error routine served here !!!!
//    setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1)&(~(1<<3)));    //turns the 4th relay ON 
  
    if((getParameter(PARAM_WGHT)>getParameter(PARAM_LVL_MIN))&((getParameter(PARAM_FLAG)&(FLAG_PUMPING))==TRUE))  // pumping goes on as long as the
    {
      nilThdSleepMilliseconds(5000);                                        //5 sec pumping  
    }
  
    else
    {
      setParameter(PARAM_FLAG,getParameter(PARAM_FLAG)&(~(FLAG_PUMPING)));      //FLAG_MOTOR_OFF to false, all routines can start again
      setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1)||(1<<3));         //turns pump off (relay is off)
      nilThdSleepMilliseconds(120000);                                         //no control for 2min (or longer delay ???)
    }
  }
}

