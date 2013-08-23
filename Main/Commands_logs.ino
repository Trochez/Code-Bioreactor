//Whenever a action occurs in the Bioreactor, a log should be written.
#include <SST.h>
#include <SPI.h>

// Definition of all events to be logged

#define PUMPING_START          1
#define PUMPING_STOP           2
#define MOTOR_START            3
#define MOTOR_STOP             4
#define NO_ANSWER_NTP_SERVER   6
#define NO_ANSWER_SERVER       7
#define SENSORS_ERROR          8
#define SET_MODE_1             16
#define SET_MODE_2             17
#define SET_MODE_3             18
#define PARAMETER_SET          130

#define LOGS_ENTRY_SIZE        7

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
  
 
  // Initialized the flash memory with the rigth address in the memory
  sst.flashWriteInit(*addr);
  
  // Write the timestamp
  for(int i = 0; i < 4; i++) {
    //write the 4 bytes of the timestamp in the memory using a mask
    sst.flashWriteNext((timestamp >> ((4 - i - 1) * 8)) & 0xFF); 
  }
  
  // write the byte of the event number
  sst.flashWriteNext(event_number);
  
  // if needed the parameter value 
  if(event_number & 0x80) {
    for(int i = 0; i < 2; i++) {
      // write the 2 bytes of the timestamp in the memory using a mask
      sst.flashWriteNext((parameter_value >> ((2 - i - 1) * 8)) & 0xFF); 
    }
  }
  
  // finish the process of writing the data in memory
  sst.flashWriteFinish();
  
  //Update the value of the next event log position in the memory
  updateAddrLogs(addr, 4 + 3);
  
}

// Update the value of the position where a new events should be
// logged in the memory
void updateAddrLogs(uint32_t* addr, uint8_t entrySize){
   *addr = (*addr + entrySize);
}
