
// Main thread for event control
// no #ifdef, code always executed

/*-----------------------
This module should take care of several tasks:
1. Check if the state vector has changed
2. Check if the modules & ports configuration is still the same
2. Check if the module are still plugged/working
3. Change state/parameter values according to the checks above

It should be exectuted regularly every 20-30 seconds
------------------------*/

NIL_WORKING_AREA(waThreadSetup, 70);      //check for memory allocation (enough // too much ?)
NIL_THREAD(ThreadSetup, arg) 
{
  while(TRUE)
  {
    setup_ctrl();
    mode_ctrl();
    error_ctrl();
    nilThdSleepMilliseconds(2000);        //control loop performed every 2 seconds for event management (is it ok ?)
  }
}

void setup_ctrl(){}
void mode_ctrl(){}
void error_ctrl(){}

