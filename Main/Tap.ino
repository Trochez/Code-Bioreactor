
#if defined(TAP_GAS1_2) || defined(TAP_GAS3_4) || defined(TAP_ACID_BASE) || defined(TAP_FOOD)



NIL_WORKING_AREA(waThreadTap, 50);
NIL_THREAD(ThreadTap, arg) { 
  
  #define OPEN  1
  #define CLOSE 0
  
  #ifdef TAP_FOOD
    unsigned long previous_time_food=now();
    boolean food_state=CLOSE;
    setParameter(PARAM_FOOD_PERIOD,10);        //one pulse every 10 sec, default config
  #endif
  
  #if defined(TAP_ACID) | defined(TAP_BASE)
    unsigned long previous_ph_adjust=now();
    boolean ph_state=CLOSE;
    setParameter(PARAM_DESIRED_PH,700);      //in fact corresponds to 7.0
  #endif
//  unsigned long previous_time_ph=now();
//  unsigned long previous_time_gas=now();
  
  while(true){
  
    
/***********
 FOOD SUPPLY
************/

    #ifdef TAP_FOOD
    
      if( ((now()-previous_time_food) >= FOOD_OPENING_TIME) & (food_state==OPEN))
      {
        setParameter(PARAM_RELAY_TAP,(getParameter(PARAM_RELAY_TAP)&~(8<<8))); //close the tap
        food_state=CLOSE;
      }
    
      else if( ((now()-previous_time_food) >= getParameter(PARAM_FOOD_PERIOD)) & (food_state==CLOSE))
      {
        setParameter(PARAM_RELAY_TAP,(getParameter(PARAM_RELAY_TAP)|(8<<8))); //open the tap
        previous_time_food=now();
        food_state=OPEN;   
      }    

    #endif
    
    /*PH implementation 
      1) read sensor with an Analog read or directly a getParameter if the acquisition is performed via I2C
      2) adjust according to desired PH if tap are present
      3) wait for a certain delay before to recheck the PH
    */
    
    /* add here the additionnal conditions (desired PH, opening time for the TAP, delay before next adjustment) */
    /*  
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
    */
    
    #ifdef  TAP_GAS1
    #endif
    
    #ifdef  TAP_GAS2
    #endif
    
    #ifdef  TAP_GAS3
    #endif
    
    #ifdef  TAP_GAS4
    #endif
    
    nilThdSleepMilliseconds(500); 
  }
  
}

#endif
