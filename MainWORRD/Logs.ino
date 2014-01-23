/*
  This thread will take care of the logs and manage the time and its synchronisation 
 The thread write the logs at a definite fixed interval of time in the SST25VF064 chip of the main boards
 The time synchronization works through the NTP protocol and our server
 
 	// TODO: change the time now() in global?
 */


//Log libraries TODO: should be completed and corrected, still problem with the C/C++ compiler
//#include <Log.h>



// the different types of logs
#define COMMAND_LOGS               101
#define RRD_SEC_LOGS               115
#define ENTRY_SIZE_LINEAR_LOGS     64
#define ENTRY_SIZE_COMMAND_LOGS    0
#define NB_PARAMETERS_LINEAR_LOGS  26
#define SIZE_TIMESTAMPS            4
#define SIZE_COUNTER_ENTRY         4

// Definition of all events to be logged
#define CARD_BOOT              0
#define PUMPING_START          1
#define PUMPING_STOP           2
#define MOTOR_START            3
#define MOTOR_STOP             4
#define NO_ANSWER_NTP_SERVER   6
#define NO_ANSWER_SERVER       7
#define SENSORS_ERROR          8
#define PUMPING_FAILURE        9
#define SET_MODE_1             16
#define SET_MODE_2             17
#define SET_MODE_3             18
#define TEMP_LIQ_FAILED        20
#define TEMP_PLATE_FAILED      21
#define TEMP_STEPPER_FAILED    22

#define WGHT_FAILURE           129
#define WGHT_BACK_TO_NORMAL    130
#define ERROR_ERASE_SECTOR     140
//When parameters are changed an event is recorded (200-225 : A-Z)
#define PARAMETER_SET          200
#define ERROR_NOT_FOUND_ENTRY_N  150

// Definition of the log sectors in the flash for the logs
#define ADDRESS_SEC_BEG   0x000000
#define ADDRESS_CMD_BEG   0x002000

#define ADDRESS_CMD_LAST  (ADDRESS_CMD_BEG + ADDRESS_CMD_SIZE - ENTRY_SIZE_COMMAND_LOGS)
#define ADDRESS_SEC_LAST  (ADDRESS_CMD_BEG - ENTRY_SIZE_LINEAR_LOGS)


#define SECTOR_SIZE       4096


#define ADDRESS_CMD_SIZE  0x030000

#define ADDRESS_SEC_SIZE  (ADDRESS_CMD_BEG  - ADDRESS_SEC_BEG)



// The number of entires by types of logs (seconds, minutes, hours, commands/events)
#define NB_ENTRIES_SEC    (ADDRESS_SEC_SIZE  / ENTRY_SIZE_LINEAR_LOGS)



#define NB_ENTRIES_CMD    (ADDRESS_CMD_SIZE  / ENTRY_SIZE_COMMAND_LOGS)


//Determine the position of the last logs in the memory for
// all type of logs (linear, RRD, commands/event)
// If we don't have enough memory in the RAM, let's use directly the function findAddress
// but it will be slower to call this function every seconds for log processes
uint32_t newEntryCmd = 0;
uint32_t newEntryRRDSec = 0;



/* 
 Function to save logs in the Flash memory.
 
 
 entryNb:         Correspond to the log ID, the entry number in the memory
 timestamp:       The time when the event happend
 event_number:    If there is a command, then this parameter should be set with the
 corresponding command/event number. Should be found in the define list of
 commands/errors
 
 */
void writeLog(uint32_t * entryNb, uint32_t timestamp, uint16_t event_number, uint16_t parameter_value) {

  Serial.println("WRITE");
  
  SST sst = SST(4);
  setupMemory(sst);

  uint16_t param = 0;
  // test if it is the begining of one sector, and erase the sector of 4094 bytes if needed

  if(!(*entryNb % NB_ENTRIES_SEC)) {
    sst.flashSectorErase(findSectorOfN(*entryNb));
  }

  // Initialized the flash memory with the right address in the memory
  sst.flashWriteInit(findAddressOfEntryN(*entryNb));

  // Write the 4 bytes of the entry number
  sst.flashWriteNextInt32(*entryNb);

  // Write the 4 bytes of the timestamp in the memory using a mask
  sst.flashWriteNextInt32(timestamp);

  for(int i = 0; i < NB_PARAMETERS_LINEAR_LOGS; i++) {
    param = getParameter(i);
    // write the 2 bytes of the parameters in the memory using a mask
    sst.flashWriteNextInt16(param);
  }
  sst.flashWriteNextInt16(event_number);
  sst.flashWriteNextInt16(parameter_value);

  // finish the process of writing the data in memory
  sst.flashWriteFinish();

  //Update the value of the next event log position in the memory
  updateEntryN(entryNb);
}

/*
  Read the last logs in the memory
 
 result:          Array of uint8_t where the logs are stored. It should be a 64 bytes array
 for the 3 RRD logs and 12 bytes for the commands/events logs.  
 *entryN:         Pointer that gives the log ID that will correspond to the logs address to
 be read and stored in result
 
 */
void readLastEntry(byte* result) {
  readEntryN(result, findPreviousEntryN(newEntryRRDSec));
}

/*
  Read the corresponding logs in the flash memory of the entry number (ID).
 
 result:          Array of uint8_t where the logs are stored. It should be a 32 bytes array
 for the 3 RRD logs and 12 bytes for the commands/events logs.  
 *entryN:         Log ID that will correspond to the logs address to be read and stored in
 result
 
 return:          Error flag:
 0: no error occured
 ERROR_NOT_FOUND_ENTRY_N: The log ID (entryN) was not found in the
 flash memory
 */
uint8_t readEntryN(uint8_t* result, uint32_t entryN) {
  SST sst = SST(4);
  setupMemory(sst); 

  uint32_t addressOfEntryN = findAddressOfEntryN(entryN);

#ifdef DEBUG_LOGS
  Serial.print("entryN: ");
  Serial.println(entryN);
  Serial.print("addressN: ");
  Serial.println(addressOfEntryN);
#endif

  sst.flashReadInit(addressOfEntryN);

    for(int i = 0; i < ENTRY_SIZE_LINEAR_LOGS; i++) {
      result[i] = sst.flashReadNextInt8();
    }
  
  sst.flashReadFinish();
  
  if ((addressOfEntryN & 0xFF000000 >> 24 == result[0]) && (addressOfEntryN & 0x00FF0000 >> 16 == result[1]) && (addressOfEntryN & 0x0000FF00 >> 8 == result[2]) && (addressOfEntryN & 0x000000FF == result[3])) {
    Serial.println("OK");
  } else {
     Serial.println("NOT OK");
  }

   // return ERROR_NOT_FOUND_ENTRY_N;
  
}



/*
  This function returns the next log ID that should be use for the next entry in the memory
 corresponding to the log type.
 
 entryN:      The actual log ID
 
 return:      The next log ID to be used
 */
uint32_t findNextEntryN(uint32_t entryN)
{
  uint32_t lastEntry = 0;
  lastEntry = (entryN +1);
  return lastEntry;
}

/*
  This function returns the previous log ID that has been used for the previous entry in 
 the memory corresponding to the log type.
 
 entryN:      The actual log ID
 
 return:      The previous log ID stored in the memory
 */
uint32_t findPreviousEntryN(uint32_t entryN)
{
  uint32_t PreviousEntry = 0;
  if(entryN - 1 > 0){
    PreviousEntry = (entryN - 1);
  }
  else { 
    PreviousEntry = NB_ENTRIES_SEC -1;
  }
  return PreviousEntry;
}

/*
  The flash memory is implemented with sectors of a defined size.
 The function returns the sector number where the log corresponding to the ID (entryNb) 
 is stored in the flash memory
 
 entryNb:         The log ID
 
 return:          The sector number
 
 */
uint16_t findSectorOfN( uint32_t entryNb) {
  uint16_t sectorNb = 0;
  uint32_t address = findAddressOfEntryN(entryNb);
  sectorNb = address / SECTOR_SIZE;
  return sectorNb;
}

/*
  Update the value of the position where a new log should be
 stored in the flash memory. Update the log ID corresponding to the respective
 log _type
 
 entryNb:         The pointer where is stored the new log ID where should be stored the
 corresponding new log
 
 */
void updateEntryN(uint32_t * entryNb) {
  *entryNb = (*entryNb + 1);
}

/*
  Function that return the corresponding address in the memory of one log ID
 
 
 entryNb:     The log ID
 
 return:      The address of the first byte where are stored the log corresponding to
 the log ID (entryN)
 */
uint32_t findAddressOfEntryN(uint32_t entryN)
{
  uint32_t address = 0;
  address = ((entryN % NB_ENTRIES_SEC) * ENTRY_SIZE_LINEAR_LOGS) % ADDRESS_SEC_SIZE + ADDRESS_SEC_BEG;
  return address;
}

/*
  Function that return the last log ID stored in the memory 
 return:      The last log ID stored in the memory corresponding to a log type
 */

uint32_t findLastEntryN() 
{
  SST sst = SST(4);
  setupMemory(sst); 

  uint32_t lastEntry = 0;
  uint32_t ID_temp = 0;
  uint32_t addressEntryN = 0;
  uint32_t addressLastEntryN = 0;
  boolean found = false;
  addressEntryN = ADDRESS_SEC_BEG;
  addressLastEntryN = ADDRESS_SEC_LAST;
#ifdef DEBUG_LOGS
  Serial.print("addressEntryN: ");
  Serial.println(ADDRESS_SEC_BEG);
  Serial.print("addressLastEntryN: ");
  Serial.println(ADDRESS_SEC_LAST);
#endif

  while(!found) 
  {
    sst.flashReadInit(addressEntryN);    
    ID_temp = sst.flashReadNextInt32();  
    sst.flashReadFinish();          
#ifdef DEBUG_LOGS    
    Serial.print("ID_temp1: ");
    Serial.println(ID_temp);
    Serial.print("lastEntry: ");
    Serial.println(lastEntry);
#endif
    // Test if first memory slot contains any information
    if(ID_temp == 0xFFFFFFFF)
    {
      sst.flashReadInit(addressLastEntryN);    
      ID_temp = sst.flashReadNextInt32();
      sst.flashReadFinish();
#ifdef DEBUG_LOGS    
      Serial.print("ID_temp2: ");
      Serial.println(ID_temp);
#endif

      // Test if the last slot of the memory contains any information
      if(ID_temp == 0xFFFFFFFF && lastEntry == 0)
      {
        // This is the initialisation step, the first entry should be 0
        lastEntry = 0;
        found = true;
        continue;
      }
      else if (lastEntry != 0 && ID_temp != lastEntry + 1)
      {
        found = true;
        continue;
      }

    }
    else if(lastEntry != 0 && ID_temp != lastEntry + 1)
    {
      found = true;
      continue;
    }
#ifdef DEBUG_LOGS
    Serial.print("findNextEntryN :"); 
    Serial.println(findNextEntryN(ID_temp));
#endif
    addressEntryN = findAddressOfEntryN(findNextEntryN(ID_temp));
    lastEntry = ID_temp;
#ifdef DEBUG_LOGS
    Serial.print("address Entry N :"); 
    Serial.println(addressEntryN);
#endif
  }
#ifdef DEBUG_LOGS
  Serial.print("Last Entry 2:"); 
  Serial.println(lastEntry);
#endif
  return lastEntry;
}

uint32_t getLastEntrySec() {
  return findPreviousEntryN(newEntryRRDSec);
}




/*--------------------------
 Memory related functions
 ---------------------------*/
//Setup the memory for future use
//Need to be used only onced at startup
void setupMemory(SST sst){
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  sst.init();
}

void printLastLog(Print* output) {
  byte record[ENTRY_SIZE_LINEAR_LOGS];
  readLastEntry(record);
  printTab(output, record);
}

void printLogN(Print* output, uint32_t entryN) {
  uint8_t record[ENTRY_SIZE_LINEAR_LOGS];
  readEntryN(record, entryN);
  printTab(output, record);
}

void printTab(Print* output, uint8_t* tab) {
  for(int i=0; i<ENTRY_SIZE_LINEAR_LOGS; i++){
    output->print(tab[i], HEX);
    output->print(' ');
  }
  output->println();
} 




