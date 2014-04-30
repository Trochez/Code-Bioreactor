#if defined(GAS_CTRL) || defined(TAP_ACID) || defined(TAP_BASE) ||defined(TAP_FOOD)
 
#define OPEN  1
#define CLOSE 0

NIL_WORKING_AREA(waThreadTap, 128);
NIL_THREAD(ThreadTap, arg) { 
  
  #ifdef TAP_FOOD
    time_t timer_food=now();
    boolean food_state=CLOSE;
  #endif
  
  #if defined(TAP_ACID) || defined(TAP_BASE)
    time_t previous_ph_adjust=now();
    boolean ph_state=CLOSE;
  #endif
  
  
  #ifdef TAP_GAS1    
    pinMode(TAP_GAS1,OUTPUT);
    unsigned long time_gas1;
    unsigned long timer_gas1=now();
    boolean state_gas1=CLOSE;
    unsigned int duty_cycle_gas1=0;  //opening set at 0% of the regulation windows initially 
    unsigned int average_flux_gas1=0;
    unsigned int average_generator_gas1=0;
    unsigned int average_counter_gas1=0;

  #endif
  
  #ifdef TAP_GAS2    
    pinMode(TAP_GAS2,OUTPUT);    
    unsigned long time_gas2;
    unsigned long timer_gas2=now();
    boolean state_gas2=CLOSE;
    unsigned int duty_cycle_gas2=0;  //opening set at 0% of the regulation windows initially 
    unsigned int average_flux_gas2=0;
    unsigned int average_generator_gas2=0;
    unsigned int average_counter_gas2=0;
    
  #endif
  
  #ifdef TAP_GAS3    
    pinMode(TAP_GAS3,OUTPUT);    
    unsigned long time_gas3;
    unsigned long timer_gas3=now();
    boolean state_gas3=CLOSE;
    unsigned int duty_cycle_gas3=0;  //opening set at 0% of the regulation windows initially 
    unsigned int average_flux_gas3=0;
    unsigned int average_generator_gas3=0;
    unsigned int average_counter_gas3=0;
    
  #endif
  
  #ifdef TAP_GAS4    
    pinMode(TAP_GAS4,OUTPUT);  
    unsigned long time_gas4;  
    unsigned long timer_gas4=now();
    boolean state_gas4=CLOSE;
    unsigned int duty_cycle_gas4=0;  //opening set at 0% of the regulation windows initially 
    unsigned int average_flux_gas4=0;
    unsigned int average_generator_gas4=0;
    unsigned int average_counter_gas4=0;
    
  #endif
    
  
  while(true){  
         
    /**************
     PH REGULATION
    ***************/

    /*PH implementation 
      1) read sensor with an Analog read or directly a getParameter if the acquisition is performed via I2C
      2) adjust according to desired PH if tap are present
      3) wait for a certain delay before to recheck the PH
    */
    
    #ifdef TAP_ACID
    
       if( (getParameter(PARAM_PH)> (getParameter(PARAM_DESIRED_PH)+PH_TOLERANCE) ) && ((now()-previous_ph_adjust)>=PH_ADJUST_DELAY) & (ph_state==CLOSE))
       {
         setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(4<<8));    //open the tap
         previous_ph_adjust=now();
         ph_state=OPEN;
       }
       else if( (previous_ph_adjust>=PH_OPENING_TIME) && (ph_state==OPEN))
       {
         setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP) & ~(4<<8)); //close the tap
         ph_state=CLOSE;
       } 
        
    #endif 
    
    
    #ifdef TAP_BASE
    
       if( (getParameter(PARAM_PH)< (getParameter(PARAM_DESIRED_PH)-PH_TOLERANCE) ) && ((now()-previous_ph_adjust)>=PH_ADJUST_DELAY) & (ph_state==CLOSE))
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

        time_gas1=(now()-timer_gas1);
        #ifdef DEBUG_GAZ        
          Serial.print("time : ");
          Serial.println(time_gas1);
          Serial.print("timer : ");
          Serial.println(timer_gas1);
          Serial.print("duty_cycle: ");
          Serial.println(duty_cycle_gas1);
          Serial.print("average_flux : ");
          Serial.println(average_flux_gas1);
        #endif
      
        // regenerate the average value and adjust to desired input, value is adjusted once every 1 time windows of 10 sec
        if(time_gas1>=FLUX_TIME_WINDOWS)
          {
            
            average_flux_gas1=(average_generator_gas1/average_counter_gas1);
            average_generator_gas1=getParameter(PARAM_FLUX_GAS1);
            average_counter_gas1=1; 
              
            if((average_flux_gas1 < (getParameter(PARAM_DESIRED_FLUX_GAS1)-FLUX_TOLERANCE)) && (duty_cycle_gas1<100))   //potential overflow problem here because of substration to zero ab initio ?
            {
              duty_cycle_gas1++;  //value limited to 100% of the time windows, possibly adjust by more than just one percente at a time
            }
              
            else if((average_flux_gas1 > (getParameter(PARAM_DESIRED_FLUX_GAS1)+FLUX_TOLERANCE)) && (duty_cycle_gas1>0))
            {
              duty_cycle_gas1--;
            }
          } 
        else
        {
          average_generator_gas1=(average_generator_gas1+getParameter(PARAM_FLUX_GAS1));
          average_counter_gas1++;
        } 
       
        /*Operate valves*/        
        if( (time_gas1 >= (FLUX_TIME_WINDOWS*duty_cycle_gas1/100)) && (state_gas1==OPEN))
        {
          #ifdef DEBUG_GAZ  
            Serial.print("close valve");
          #endif
          digitalWrite(TAP_GAS1, LOW);//close the tap using PWM port
          state_gas1=CLOSE;
          
        }
          
        else if( (time_gas1 >= FLUX_TIME_WINDOWS) && (state_gas1==CLOSE))
        {
          #ifdef DEBUG_GAZ  
            Serial.print("open valve");
          #endif
          digitalWrite(TAP_GAS1, HIGH);//open the tap using dedicated PWM port
          timer_gas1=now();
          state_gas1=OPEN; 
        }   
    
    #endif 
    
    #ifdef  TAP_GAS2
    

        time_gas2=(now()-timer_gas2);      
        // regenerate the average value and adjust to desired input, value is adjusted once every 1 time windows of 10 sec
        if(time_gas2>=FLUX_TIME_WINDOWS)
          {
            
            average_flux_gas2=(average_generator_gas2/average_counter_gas2);
            average_generator_gas2=getParameter(PARAM_FLUX_GAS2);
            average_counter_gas2=1; 
              
            if((average_flux_gas2 < (getParameter(PARAM_DESIRED_FLUX_GAS2)-FLUX_TOLERANCE)) && (duty_cycle_gas2<100))   //potential overflow problem here because of substration to zero ab initio ?
            {
              duty_cycle_gas2++;  //value limited to 100% of the time windows, possibly adjust by more than just one percente at a time
            }
              
            else if((average_flux_gas2 > (getParameter(PARAM_DESIRED_FLUX_GAS2)+FLUX_TOLERANCE)) && (duty_cycle_gas2>0))
            {
              duty_cycle_gas2--;
            }
          } 
        else
        {
          average_generator_gas2=(average_generator_gas2+getParameter(PARAM_FLUX_GAS2));
          average_counter_gas2++;
        } 
        
        /*Operate valves*/        
        if( (time_gas2 >= (FLUX_TIME_WINDOWS*duty_cycle_gas2/100)) && (state_gas2==OPEN))
        {
          digitalWrite(TAP_GAS2, LOW);//close the tap using PWM port
          state_gas2=CLOSE;
          
        }
          
        else if( (time_gas2 >= FLUX_TIME_WINDOWS) && (state_gas2==CLOSE))
        {
          digitalWrite(TAP_GAS2, HIGH);//open the tap using dedicated PWM port
          timer_gas2=now();
          state_gas2=OPEN; 
        }    

    #endif
    
    #ifdef  TAP_GAS3
    
        time_gas3=(now()-timer_gas3);      
        // regenerate the average value and adjust to desired input, value is adjusted once every 1 time windows of 10 sec
        if(time_gas3>=FLUX_TIME_WINDOWS)
          {
            
            average_flux_gas3=(average_generator_gas3/average_counter_gas3);
            average_generator_gas3=getParameter(PARAM_FLUX_GAS3);
            average_counter_gas3=1; 
              
            if((average_flux_gas3 < (getParameter(PARAM_DESIRED_FLUX_GAS3)-FLUX_TOLERANCE)) && (duty_cycle_gas3<100))   //potential overflow problem here because of substration to zero ab initio ?
            {
              duty_cycle_gas3++;  //value limited to 100% of the time windows, possibly adjust by more than just one percente at a time
            }
              
            else if((average_flux_gas3 > (getParameter(PARAM_DESIRED_FLUX_GAS3)+FLUX_TOLERANCE)) && (duty_cycle_gas3>0))
            {
              duty_cycle_gas3--;
            }
          } 
        else
        {
          average_generator_gas3=(average_generator_gas3+getParameter(PARAM_FLUX_GAS3));
          average_counter_gas3++;
        } 
        
        /*Operate valves*/        
        if( (time_gas3 >= (FLUX_TIME_WINDOWS*duty_cycle_gas3/100)) && (state_gas3==OPEN))
        {
          digitalWrite(TAP_GAS3, LOW);//close the tap using PWM port
          state_gas3=CLOSE;
          
        }
          
        else if( (time_gas3 >= FLUX_TIME_WINDOWS) && (state_gas3==CLOSE))
        {
          digitalWrite(TAP_GAS3, HIGH);//open the tap using dedicated PWM port
          timer_gas3=now();
          state_gas3=OPEN; 
        }  

    #endif
    
    #ifdef  TAP_GAS4
    
        time_gas4=(now()-timer_gas4);      
        // regenerate the average value and adjust to desired input, value is adjusted once every 1 time windows of 10 sec
        if(time_gas4>=FLUX_TIME_WINDOWS)
          {
            
            average_flux_gas4=(average_generator_gas4/average_counter_gas4);
            average_generator_gas4=getParameter(PARAM_FLUX_GAS4);
            average_counter_gas4=1; 
              
            if((average_flux_gas4 < (getParameter(PARAM_DESIRED_FLUX_GAS4)-FLUX_TOLERANCE)) && (duty_cycle_gas4<100))   //potential overflow problem here because of substration to zero ab initio ?
            {
              duty_cycle_gas4++;  //value limited to 100% of the time windows, possibly adjust by more than just one percente at a time
            }
              
            else if((average_flux_gas4 > (getParameter(PARAM_DESIRED_FLUX_GAS4)+FLUX_TOLERANCE)) && (duty_cycle_gas4>0))
            {
              duty_cycle_gas4--;
            }
          } 
        else
        {
          average_generator_gas4=(average_generator_gas4+getParameter(PARAM_FLUX_GAS4));
          average_counter_gas4++;
        } 
        
        /*Operate valves*/        
        if( (time_gas4 >= (FLUX_TIME_WINDOWS*duty_cycle_gas4/100)) && (state_gas4==OPEN))
        {
          digitalWrite(TAP_GAS4, LOW);//close the tap using PWM port
          state_gas4=CLOSE;
          
        }
          
        else if( (time_gas4 >= FLUX_TIME_WINDOWS) && (state_gas4==CLOSE))
        {
          digitalWrite(TAP_GAS4, HIGH);//open the tap using dedicated PWM port
          timer_gas4=now();
          state_gas4=OPEN; 
        }     

    #endif
    
    nilThdSleepMilliseconds(500); 
  }
  
}

#endif


