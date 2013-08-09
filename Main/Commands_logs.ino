//Whenever a action occurs in the Bioreactor, a log should be written.
#include <SST.h>
#include <SPI.h>

#define PARAMETER_SIZE 32

// Definition of all events to be logged
#define PUMPING_START    1
#define PUMPING_STOP     2
#define MOTOR_START      3
#define MOTOR_STOP       4
//#define PARAMETER_SET
#define SENSORS_ERROR    5
#define SET_MODE_1       16
#define SET_MODE_2       17
#define SET_MODE_3       18


#define LOGS_ENTRY_SIZE  8


/* 
  Function to save the events in the Flash memory
  
  sst:             The object where is defined the operations to manipulate 
                   the flash memory
  addr:            The location in the memory where the command logs should be writen
  timesamp:        The time when the event happend 
  parameter_value: The value of the parameter (used when a user changes the value of
                   on of the 26 variables)
  
*/
void writeCommandLog(SST sst, uint32_t* addr, uint32_t timestamp, uint8_t event_number, uint16_t parameter_value) {
  
  if(parameter_value == 0)
  
  //TODO CHECK event_number
  //compute the address of the last row (4 byte for the timestamp)
  uint32_t address = *addr - (LOGS_ENTRY_SIZE - event_number - 1);
  
  // Initialized the flash memory with the rigth address in the memory
  sst.flashWriteInit(*addr);
  
  // Write the timestamp
  for(int i=0; i<4; i++){
    //write the 4 bytes of the timestamp in the memory using a mask
    sst.flashWriteNext((timestamp >> ((4-i-1)*8)) & 0xFF); 
  }
  
  // Write the eventcode and if needed, the parameter/value  
  // TODO: Modify the writing function to addapted
  
  
  sst.flashWriteFinish();
  
  
  //Update the value of the actual entry
  updateAddrLogs(addr);
  
}

// Update the value of the position where a new events should be
// logged in the memory
void updateAddrLogs(uint32_t* addr){
   *addr = (*addr + LOGS_ENTRY_SIZE);
}
