//pumping();        
//add control to define which of the weight sensor(s) will be addressed in the pumping control loop

NIL_WORKING_AREA(waThreadI2C, 70);      //check for memory allocation ?
NIL_THREAD(ThreadI2C, arg) 
{
  while(TRUE)    
  {
    controlI2C();
    nilSleepMilliseconds(500);  //refresh every 500ms
  }
}



void controlI2C() {
  
    for(char i=0;i<9;i++)
    {
      switch(Device[i].type)
      {
        case RELAY :
          //additional test on the device port which must be either dof the I2C ports
          //if weight sensor present {pumping();}                      //do nothing most of the time, would we like to add a timer condition here or not ?
          //if temperature sensor present {pidTemperature();}
        case FLUX  :      
          //possibly put a special thread for flux control
          break;
        case default :
          break;
      }
    }
}



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

