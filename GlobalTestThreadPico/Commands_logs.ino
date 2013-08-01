//Whenever a action occurs in the Bioreactor, a log should be written.
#include <SST.h>
#include <SPI.h>

#define PARAMETER_SIZE 32

// Definition of all events to be logged
#define PUMPING_START
#define PUMPING_STOP
#define MOTOR_START
#define MOTOR_STOP
#define PARAMETER_SET
#define SENSORS_ERROR
#define SET_MODE

/* 
  Function to save the events in the Flash memory
  
  event:  The event number to be logged
  value:  The value of the changes made if needed
*/
/*
void writeCommandLog(uint8_t event, uint16_t value) {
  
  // take the time, need to be synchronized with the NTP server
  int time = now();
  
  // Initialized the flash memory
  sst.flashWriteInit();
  
  // Write the timestamp
  for(int i=0; i<4; i++){
    //write the 4 bytes of the timestamp in the memory using a mask
    sst.flashWriteNext((timestamp >> ((4-i-1)*8)) & 0xFF); 
  }
  
  // Write the eventcode and if needed, the parameter/value  
  // TODO: Modify the writing function to addapted
  
  
  sst.flashWriteFinish();
}*/
