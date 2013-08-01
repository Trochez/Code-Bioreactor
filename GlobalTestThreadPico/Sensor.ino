
// weight cell sensor with 12V supply and analog output 0-5V
// min and max value to be defined for the thresholds
// If there is more than one device on a bus we need to specify the devices address. Otherwise we may just scan
/* pumping control, linked to weight thread */


NIL_WORKING_AREA(waThreadSensor, 70);      //check for memory allocation ?
NIL_THREAD(ThreadSensor, arg) 
{
  while(TRUE)    
  {
    getSensor();
    nilSleepMilliseconds(500);  //refresh every 500ms
  }
}


// weight cell reading
void getSensor() {
  
    for(char i=0;i<9;i++)
    {
      switch(Device[i].type)
      {
        case WGHT :
          getWeight(i);
          break;
        case PH :
          getPh(i);
          break;
        case TEMP :        //gestion du ONE_WIRE
          getTemp(i);
          break;
        case FLUX :       // case FLUX eventuellement ailleurs si trop lourd
          getFlux(i);
          break;
        case default :
          break;
      }
    }
}
 
 
void getWeight(char i){
  
  int weight= analogRead(Device[i].port);          
  setParameter(Device[i].parameter,weight);
  if(weight>getParameter(PARAM_LVL_MAX){
// test and modification of the local state vector, and global state vector if event
//    Device[i].state=;
  }
  
}  

void getPh(char i){
  
}



/*
// we allow an array of devices
// we need to specify 3 arrays
// - the buses
// - the devices
// - the target variable

// If there is more than one device on a bus we need to specify the devices address. Otherwise we may just scan
*/

void getTemp(char i){
  
  OneWire oneWire1(Device[i].port);
  DallasTemperature sensors1(&oneWire1);
  DeviceAddress oneWireAddress1;
  getTemperature(sensors1, oneWireAddress1, Device[i].port, Device[i].parameter);
  
}

void getFlux(char i){
  
}



void getTemperature(DallasTemperature sensor, DeviceAddress address, int bus, int parameter){
   sensor.begin();
  if (!sensor.getAddress(address, 0)) debugger(DEBUG_ONEWIRE_NODEVICE, bus); 
  sensor.setWaitForConversion(false); // we don't wait for conversion (otherwise may take 900mS)
  // set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensor.setResolution(address, 12);


  while (TRUE) {
    // following instruction takes 2ms
    sensor.requestTemperatures(); // Send the coThreadOneWiremmand to get temperatures
    // we should not forget that with 12 bits it takes over 600ms to get the result so in fact we
    // will get the result of the previous conversion ...
    // Following instruction takes 14ms
    setParameter(parameter,(int)(sensor.getTempC(address)*100));
  }
  
}




/*
#ifdef ONE_WIRE_BUS2
OneWire oneWire2(ONE_WIRE_BUS2);
DallasTemperature sensors2(&oneWire2);
DeviceAddress oneWireAddress2;

NIL_WORKING_AREA(waThreadOneWire2, 70);
NIL_THREAD(ThreadOneWire2, arg) {
  getTemperature(sensors2, oneWireAddress2, ONE_WIRE_BUS2, PARAM_TEMP2);
}
#endif

// function to print a device address
void printOneWireAddress(DeviceAddress deviceAddress, Print* output)
{
    if (*deviceAddress < 16) output->print("0");
    else output->print(*deviceAddress, HEX);
}

/*Does not work at the moment
void oneWireInfo(Print* output) {
  output->println("One wire");
  // Loop through each device, print out address
  for(int i=0;i<sensors1.getDeviceCount(); i++)
  {
    // Search the wire for address
    if (sensors1.getAddress(oneWireAddress, i))
    {
      output->print(i);
      output->print(": ");
      printOneWireAddress(oneWireAddress, output);
      output->println();
    }
  }
}
*/


