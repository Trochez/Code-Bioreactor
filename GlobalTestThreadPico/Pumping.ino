        
//    pumping();        
        

//add control to define which of the weight sensor(s) will be addressed in the pumping control loop
/*


void pumping(){                        //add condition that the pump module is present  (if {} else{nilThdSleepMilliseconds(60000)} for instance

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
*/
