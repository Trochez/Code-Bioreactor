
#if defined(TAP_GAS1_2) || defined(TAP_GAS3_4) || defined(TAP_ACID_BASE) || defined(TAP_FOOD)



NIL_WORKING_AREA(waThreadTap, 50);
NIL_THREAD(ThreadTap, arg) { 
  
  #define OPEN  1
  #define CLOSE 0
  
  #ifdef TAP_FOOD
    unsigned long previous_time_food=now();
    boolean food_state=CLOSE;
    setParameter(FOOD_PERIOD,10);        //one pulse every 10 sec, default config
  #endif
  
  #if defined(TAP_ACID) | defined(TAP_BASE)
    unsigned long previous_ph_check=now();
  
  #endif
//  unsigned long previous_time_ph=now();
//  unsigned long previous_time_gas=now();
  
  while(true){
  
    
/***********
 FOOD SUPPLY
************/


    #ifdef TAP_FOOD
    
      if( ((now()-previous_time_food) >= OPENING_TIME) & (food_state==OPEN))
      {
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)&~(8<<8)); //open the tap
        food_state=CLOSE;
      }
    
      else if( ((now()-previous_time_food) >= getParameter(FOOD_PERIOD)) & (food_state==CLOSE))
      {
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<8)); //close the tap
        previous_time_food=now();
        food_state=OPEN;   
      }
      

    #endif
    
    /*PH implementation 
      1) read sensor with an Analog read or directly a getParameter if the acquisition is performed via I2C
      2) adjust according to desired PH if tap are present
      3) wait for a certain delay before to recheck the PH
    */
    
    /* add here the additionnal conditions (desired PH, opening time for the TAP, delay before next adjestement) */
    
  /*  
    #ifdef TAP_ACID
    
       if( (getParameter(PARAM_PH)>getParameter(PARAM_DESIRED_PH)) & ((now()-previous-ph-check)>=PH_ADJUST_DELAY))
       {
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<4)); //open the tap
       }
       
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)&~(8<<4)); //close the tap
        
        
    #endif 
    
    #ifdef TAP_BASE
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<2)); //open the tap
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)&~(8<<2)); //close the tap
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
    
    nilThdSleepMilliseconds(200); 
  }
  
}

#endif
