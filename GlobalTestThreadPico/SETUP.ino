
// Main thread for event control
// no #ifdef, code always executed


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

