
#if defined(GAS_CTRL) || defined(TAP_ACID_BASE) || defined(TAP_FOOD)

  
#if defined(TAP_GAS1) || defined(TAP_GAS2) || defined(TAP_GAS3) || defined(TAP_GAS4)
  
  typedef struct
  {
    unsigned long timer;
    boolean state;    
    uint8_t duty_cycle;                  
    //variables regenerating the flux average from sensor
    uint8_t      average_flux;
    unsigned int average_generator;
    uint8_t      average_counter;   
  } GAS_CONTROL, *P_GAS_CONTROL;
 
  void fluxRegulation(P_GAS_CONTROL p_gas);
  
#endif


NIL_WORKING_AREA(waThreadTap, 96);
NIL_THREAD(ThreadTap, arg) { 
  
  #define OPEN  1
  #define CLOSE 0
  
  #ifdef TAP_FOOD
    unsigned long timer_food=now();
    boolean food_state=CLOSE;
    if(getParameter(PARAM_FOOD_PERIOD) == MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_FOOD_PERIOD,10);        //one pulse every 10 sec, default config
    }
  #endif
  
  #if defined(TAP_ACID) || defined(TAP_BASE)
    unsigned long previous_ph_adjust=now();
    boolean ph_state=CLOSE;
    if(getParameter(PARAM_DESIRED_PH) == MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_DESIRED_PH,700);      //in fact corresponds to a PH of 7.0
    }
  #endif
  
  
  #ifdef TAP_GAS1    
 
    P_GAS_CONTROL  gas1;
    
    if(getParameter(PARAM_DESIRED_FLUX_GAS1) == MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS1,0);  //set by default as 0 cc/min
    }
    
    gas1->timer=now();
    gas1->state=CLOSE;
    gas1->duty_cycle=0;  //opening set at 0% of the regulation windows initially 
    gas1->average_flux=0;
    gas1->average_generator=0;
    gas1->average_counter=0;
    
  #endif
  
    #ifdef TAP_GAS2    
 
    P_GAS_CONTROL  gas2;
    
    if(getParameter(PARAM_DESIRED_FLUX_GAS2) == MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS2,0);  //set by default as 0 cc/min
    }
    
    gas2->timer=now();
    gas2->state=CLOSE;
    gas2->duty_cycle=0;  //opening set at 0% of the regulation windows initially 
    gas2->average_flux=0;
    gas2->average_generator=0;
    gas2->average_counter=0;
    
  #endif
  
  #ifdef TAP_GAS3    
 
    P_GAS_CONTROL  gas3;
    
    if(getParameter(PARAM_DESIRED_FLUX_GAS3) == MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS3,0);  //set by default as 0 cc/min
    }
    
    gas3->timer=now();
    gas3->state=CLOSE;
    gas3->duty_cycle=0;  //opening set at 0% of the regulation windows initially 
    gas3->average_flux=0;
    gas3->average_generator=0;
    gas3->average_counter=0;
    
  #endif
  
  #ifdef TAP_GAS4    
 
    P_GAS_CONTROL  gas4;
    
    if(getParameter(PARAM_DESIRED_FLUX_GAS4) == MAX_INTEGER)
    {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS4,0);  //set by default as 0 cc/min
    }
    
    gas4->timer=now();
    gas4->state=CLOSE;
    gas4->duty_cycle=0;  //opening set at 0% of the regulation windows initially 
    gas4->average_flux=0;
    gas4->average_generator=0;
    gas4->average_counter=0;
    
  #endif
    
  
  while(true){
  
        
    /***********
     FOOD SUPPLY
    ************/

    #ifdef TAP_FOOD
    
      if( ((now()-timer_food) >= FOOD_OPENING_TIME) & (food_state==OPEN))
      {
        setParameter(PARAM_RELAY_TAP,(getParameter(PARAM_RELAY_TAP)&~(8<<8))); //close the tap
        timer_food=now();
        food_state=CLOSE;
      }
    
      else if( ((now()-timer_food) >= getParameter(PARAM_FOOD_PERIOD)) & (food_state==CLOSE))
      {
        setParameter(PARAM_RELAY_TAP,(getParameter(PARAM_RELAY_TAP)|(8<<8))); //open the tap
        timer_food=now();
        food_state=OPEN;   
      }    

    #endif
        
    /**************
     PH REGULATION
    ***************/

    /*PH implementation 
      1) read sensor with an Analog read or directly a getParameter if the acquisition is performed via I2C
      2) adjust according to desired PH if tap are present
      3) wait for a certain delay before to recheck the PH
    */
    
    #ifdef TAP_ACID
    
       if( (getParameter(PARAM_PH)> (getParameter(PARAM_DESIRED_PH)+PH_TOLERANCE) ) & ((now()-previous_ph_adjust)>=PH_ADJUST_DELAY) & (ph_state==CLOSE))
       {
         setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(4<<8));    //open the tap
         previous_ph_adjust=now();
         ph_state=OPEN;
       }
       else if( (previous_ph_adjust>=PH_OPENING_TIME) & (ph_state==OPEN))
       {
         setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP) & ~(4<<8)); //close the tap
         ph_state=CLOSE;
       } 
        
    #endif 
    
    
    #ifdef TAP_BASE
    
       if( (getParameter(PARAM_PH)< (getParameter(PARAM_DESIRED_PH)-PH_TOLERANCE) ) & ((now()-previous_ph_adjust)>=PH_ADJUST_DELAY) & (ph_state==CLOSE))
       {
         setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP) | (2<<8));    //open the tap
         previous_ph_adjust=now();
         ph_state=OPEN;
       }
       else if( (previous_ph_adjust>=PH_OPENING_TIME) & (ph_state==OPEN))
       {
         setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)  & ~(2<<8)); //close the tap
         ph_state=CLOSE;
       }
        
    #endif 
    
    /***************
     GAS REGULATION
    ****************/
    
    /* gas control implementation
       1) aquire value of the gas flux over an average corresponding to the duty cycle time windows
       2) compare the averaging with the desired gas flux
       3) adapt the duty cycle to fix the average value to desired value (according to previous calibration)
       4) if the flux is null, send error message GAS_EMPTY
    */
        
    #ifdef  TAP_GAS1 
      fluxRegulation(gas1);           
    #endif
    
    
    #ifdef  TAP_GAS2
      fluxRegulation(gas2);
    #endif
    
    #ifdef  TAP_GAS3
      fluxRegulation(gas3);
    #endif
    
    #ifdef  TAP_GAS4
      fluxRegulation(gas4);
    #endif
    
    nilThdSleepMilliseconds(500); 
  }
  
}

#if defined(TAP_GAS1) || defined(TAP_GAS2) || defined(TAP_GAS3) || defined(TAP_GAS4)

void fluxRegulation(P_GAS_CONTROL p_gas)
{
  unsigned long time=now()-p_gas->timer;
  // regenerate the average value and adjust to desired input, value is adjusted once every 1 time windows of 10 sec
  if(time>=FLUX_TIME_WINDOWS)
    {
      p_gas->average_flux=(p_gas->average_generator/p_gas->average_counter);
      p_gas->average_generator=getParameter(PARAM_FLUX_GAS1);
      p_gas->average_counter=1;
        
      if((p_gas->average_flux < (getParameter(PARAM_DESIRED_FLUX_GAS1)-FLUX_TOLERANCE)) & (p_gas->duty_cycle<100))   //potential overflow problem here because of substration to zero ab initio ?
      {
       p_gas->duty_cycle++;  //value limited to 100% of the time windows, possibly adjust by more than just one percente at a time
      }
        
      else if((p_gas->average_flux > (getParameter(PARAM_DESIRED_FLUX_GAS1)+FLUX_TOLERANCE)) & (p_gas->duty_cycle>0))
      {
        p_gas->duty_cycle--;
      }
    } 
  else
  {
    p_gas->average_generator=(p_gas->average_generator+getParameter(PARAM_FLUX_GAS1));
    p_gas->average_counter++;
  } 
  
  /*Operate valves*/
  
  if( time >= ((FLUX_TIME_WINDOWS*p_gas->duty_cycle)/100) & (p_gas->state==OPEN))
  {
    digitalWrite(TAP_GAS1, LOW);//close the tap using PWM port
    p_gas->state=CLOSE;
  }
    
  else if( (time >= FLUX_TIME_WINDOWS) & (p_gas->state==CLOSE))
  {
    digitalWrite(TAP_GAS1, HIGH);//open the tap using dedicated PWM port
    p_gas->timer=now();
    p_gas->state=OPEN; 
  }   
}

#endif

  
#endif

