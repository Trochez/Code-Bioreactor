//TODO : Clean global variables

#ifdef TRANS_PID
#include <PID_v1.h>

void pid_ctrl();
void heatingSetup();
double heatingRegInput, heatingRegOutput, heatingRegSetpoint;
//initialize PID variables

unsigned long heatingRegWindowStartTime;
//Specify the heating regulation links and initial tuning parameters //Kp=100; Ti=0.2; Td=5 are initial testing param.
//PID object definition can be found in PID library (to include for compilation).
PID heatingRegPID(&heatingRegInput, &heatingRegOutput, &heatingRegSetpoint, 7000,15,300, DIRECT);

NIL_WORKING_AREA(waThreadRelay_PID, 70);      
NIL_THREAD(ThreadRelay_PID, arg) 
{  

  heatingSetup();
  
  while(TRUE){
    pid_ctrl();
    nilThdSleepMilliseconds(500);  //refresh every 500ms
  }
}


/*Temperature PID Control addressing I2C resistors*/

void pid_ctrl()
{
  float exactPresentTime;
  heatingRegInput = getParameter(PARAM_TEMP_LIQ);
  heatingRegSetpoint = getParameter(PARAM_DESIRED_LIQUID_TEMP);
  heatingRegPID.Compute();                                   // the computation takes only 30ms!
  // turn the output pin on/off based on pid output
  exactPresentTime = millis();
  if(exactPresentTime - heatingRegWindowStartTime > HEATING_REGULATION_TIME_WINDOWS)
  { 
    //time to shift the Relay Window
    heatingRegWindowStartTime += HEATING_REGULATION_TIME_WINDOWS;
  }

  if(heatingRegOutput > exactPresentTime - heatingRegWindowStartTime) 
  {
    //set relay ON, ensure the Wire thread makes the work then !!!
    setParameter(RELAY_PID,getParameter(RELAY_PID)&(~(1<<3)));
    digitalWrite(TRANS_PID, HIGH); 
  }  
  else 
  {
    //set relay OFF, ensure the Wire thread makes the work then !!!
    setParameter(RELAY_PID,getParameter(RELAY_PID)||(1<<4));
    digitalWrite(TRANS_PID, LOW); 
  } 
}


// see the rest of oliver's code for sanity checks

void heatingSetup()
{
  //tell the PID to range between 0 and the full window size
  heatingRegPID.SetOutputLimits(0, HEATING_REGULATION_TIME_WINDOWS);          //what is heating regulation time windows ???
  //turn the PID on, cf. PID library
  heatingRegPID.SetMode(AUTOMATIC);                 
  //set PID sampling time to 10000ms                   //possibly set a timer condition with a nilsleep instead
  heatingRegPID.SetSampleTime(10000);
  heatingRegWindowStartTime = millis();
  heatingRegSetpoint = getParameter(PARAM_TEMP_MAX);
}

#endif
