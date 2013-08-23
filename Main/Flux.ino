#ifdef FLUX

NIL_WORKING_AREA(waThreadTap, 50);
NIL_THREAD(ThreadTap, arg) { 

  while(true){
    getFlux();
    nilThdSleepMilliseconds(200);
  }
}

void getFlux(){
  ;
}

#endif
