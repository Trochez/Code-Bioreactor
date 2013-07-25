// anemometer module
// measure of 4 fluxes via I2C communication

#ifdef THR_FLUX

NIL_WORKING_AREA(waThreadFlux, 70); //change memoy allocation
NIL_THREAD(ThreadFlux, arg) {
//  getFlux(parametres(gaz));
  getFlux();
}

void getFlux(){
  while (TRUE) {
    // a coder
    nilThdSleepMilliseconds(1000);
  }
}

#endif
