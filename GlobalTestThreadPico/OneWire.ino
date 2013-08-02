
// we allow an array of devices
// we need to specify 3 arrays
// - the buses
// - the devices
// - the target variable

// If there is more than one device on a bus we need to specify the devices address. Otherwise we may just scan
#ifdef ONE_WIRE_BUS1 || ONE_WIRE_BUS2

#ifdef ONE_WIRE_BUS1
//OneWire oneWire;
//oneWire = oneWire(ONE_WIRE_BUS1);
OneWire oneWire1(ONE_WIRE_BUS1);
DallasTemperature sensors1(&oneWire1);
DeviceAddress oneWireAddress1;

NIL_WORKING_AREA(waThreadOneWire1, 70);
NIL_THREAD(ThreadOneWire1, arg) {
  getTemperature(sensors1, oneWireAddress1, ONE_WIRE_BUS1, PARAM_TEMP1);
}
#endif

#ifdef ONE_WIRE_BUS2
OneWire oneWire2(ONE_WIRE_BUS2);
DallasTemperature sensors2(&oneWire2);
DeviceAddress oneWireAddress2;

NIL_WORKING_AREA(waThreadOneWire2, 70);
NIL_THREAD(ThreadOneWire2, arg) {
  getTemperature(sensors2, oneWireAddress2, ONE_WIRE_BUS2, PARAM_TEMP2);
}
#endif


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
    nilThdSleepMilliseconds(1000);
  }
  
}


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
#endif



