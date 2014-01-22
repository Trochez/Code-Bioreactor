

void printHelp(Print* output) {
  //return the menu
  output->println(F("(f)free"));
  output->println(F("(h)help"));
  output->println(F("(i)2c"));
  output->println(F("(l)og"));
  output->println(F("(o)1-wire"));
  output->println(F("(p)param"));
  output->println(F("(s)settings"));
}


static void printFreeMemory(Print* output)
{
  nilPrintUnusedStack(output);
}



void printHardCodedParameters(Print* output){
  output->println(F("Hardcoded:")); 
  output->print(F("IP:"));
  printIP(output, ip, 4, DEC);
  output->print(F("MAC:"));
  printIP(output, mac, 6, HEX);
  output->print(F("ALIX:"));
  printIP(output, (uint8_t*) alix, 4, DEC);
#ifdef RELAY_PUMP
  output->print(F("I2C relay:"));
  output->println(I2C_RELAY); 
#endif
#ifdef FLUX
  output->print(F("I2C Flux:"));
  output->println(I2C_FLUX); 
#endif
}

void printIP(Print* output, uint8_t* tab, uint8_t s, byte format){
   for(int i=0; i<s; i++){
      output->print(tab[i], format);
      output->print(' ');
   }
   output->println("");
}

