#ifdef WGHT

NIL_WORKING_AREA(waThreadSensor, 70);    
NIL_THREAD(ThreadSensor, arg) 
{
    getWeight();
    nilSleepMilliseconds(500);  //refresh every 500ms
}


//add a sanity check if the value is out of range of if the weight does not change after pumping for a while (ERROR_WGHT_CTRL)
//condition would restart the mixing and disable pumping

void getWeight(char i){
  int weight= analogRead(WGHT);                       
  setParameter(PARAM_WGHT,weight);
  if(weight>=PARAM_LVL_MAX){
    //set the condition to initiate pumping sequence
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_STEPPER_OFF));
  }
  if(weight<=PARAM_LVL_MIN){
    //set the condition to disable the pumping sequence and re-anable stepper motor mixing
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_STEPPER_OFF)));
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_PUMPING)));
  }
  
}  

#endif


