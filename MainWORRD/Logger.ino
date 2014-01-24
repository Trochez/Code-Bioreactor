/*
  This thread will take care of the logs and manage the time and its synchronisation 
 The thread write the logs at a definite fixed interval of time in the SST25VF064 chip of the main boards
 The time synchronization works through the NTP protocol and our server
 
 	// TODO: change the time now() in global?
 */


//Log libraries TODO: should be completed and corrected, still problem with the C/C++ compiler
//#include <Log.h>



// the different types of logs


#define ENTRY_SIZE_LINEAR_LOGS     64
#define NB_PARAMETERS_LINEAR_LOGS  26
#define SIZE_TIMESTAMPS            4
#define SIZE_COUNTER_ENTRY         4


// Definition of the log sectors in the flash for the logs
#define ADDRESS_BEG   0x000000
#define ADDRESS_MAX   0x002000

#define ADDRESS_LAST  (ADDRESS_MAX - ENTRY_SIZE_LINEAR_LOGS)

#define SECTOR_SIZE       4096
#define NB_ENTRIES_PER_SECTOR    (SECTOR_SIZE  / ENTRY_SIZE_LINEAR_LOGS)
#define ADDRESS_SIZE  (ADDRESS_MAX  - ADDRESS_BEG)


// The number of entires by types of logs (seconds, minutes, hours, commands/events)
#define MAX_NB_ENTRIES    (ADDRESS_SIZE  / ENTRY_SIZE_LINEAR_LOGS)


//Determine the position of the last logs in the memory for
// all type of logs (linear, RRD, commands/event)
// If we don't have enough memory in the RAM, let's use directly the function findAddress
// but it will be slower to call this function every seconds for log processes
uint32_t nextEntryID = 0;



/* 
 Function to save logs in the Flash memory.
 
 event_number:    If there is a command, then this parameter should be set with the
 corresponding command/event number. Should be found in the define list of
 commands/errors
 
 */
void writeLog() {
  writeLog(0,0);
}

void writeLog(uint16_t event_number, uint16_t parameter_value) {

  SST sst = SST(4);
  setupMemory(sst);

  uint16_t param = 0;
  // test if it is the begining of one sector, and erase the sector of 4096 bytes if needed

  if(!(nextEntryID % NB_ENTRIES_PER_SECTOR)) {
    sst.flashSectorErase(findSectorOfN());
    Serial.print("erase: ");
    Serial.println(findSectorOfN());
   }

  // Initialized the flash memory with the right address in the memory
  sst.flashWriteInit(findAddressOfEntryN(nextEntryID));
  // Write the 4 bytes of the entry number
  sst.flashWriteNextInt32(nextEntryID);
  Serial.print("nextEntryID: ");
  Serial.println(nextEntryID);
   Serial.print("nextAddress: ");
  Serial.println(findAddressOfEntryN(nextEntryID));
  // Write the 4 bytes of the timestamp in the memory using a mask
  sst.flashWriteNextInt32(now());

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
  nextEntryID++;
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

  if (!(((entryN & 0xFF000000) >> 24 == result[0]) && ((entryN & 0x00FF0000) >> 16 == result[1]) && ((entryN & 0x0000FF00) >> 8 == result[2]) && ((entryN & 0x000000FF) == result[3]))) {
    for (byte i=0; i<ENTRY_SIZE_LINEAR_LOGS; i++) {
      result[i]=255; 
    }
  }
}


/*
  The flash memory is implemented with sectors of a defined size.
 The function returns the sector number where the log corresponding to the ID (entryNb) 
 is stored in the flash memory
 entryNb:         The log ID
 return:          The sector number
 */
uint16_t findSectorOfN( ) {
  uint16_t sectorNb = 0;
  uint32_t address = findAddressOfEntryN(nextEntryID);
  sectorNb = address / SECTOR_SIZE;
  return sectorNb;
}


/*
  Function that return the corresponding address in the memory of one log ID
 
 entryNb:     The log ID 
 return:      The address of the first byte where are stored the log corresponding to
 the log ID (entryN)
 */
uint32_t findAddressOfEntryN(uint32_t entryN)
{
  uint32_t address = ((entryN % MAX_NB_ENTRIES) * ENTRY_SIZE_LINEAR_LOGS) % ADDRESS_SIZE + ADDRESS_BEG;
  return address;
}

/*
  Function that return the last log ID stored in the memory 
 return:      The last log ID stored in the memory corresponding to a log type
 */
void recoverLastEntryN() 
{
  SST sst = SST(4);
  setupMemory(sst); 

  uint32_t ID_temp = 0;
  uint32_t Time_temp = 0;
  uint32_t addressEntryN = ADDRESS_BEG;

  boolean found = false;
#ifdef DEBUG_LOGS
  Serial.print("First record address: ");
  Serial.println(ADDRESS_BEG);
  Serial.print("Max record address: ");
  Serial.println(ADDRESS_LAST);
#endif

  while(addressEntryN<ADDRESS_LAST) 
  {
    sst.flashReadInit(addressEntryN);    
    ID_temp = sst.flashReadNextInt32();  
    Time_temp = sst.flashReadNextInt32();  
    sst.flashReadFinish();          
#ifdef DEBUG_LOGS    
    Serial.print("ID_temp1: ");
    Serial.println(ID_temp);
    Serial.print("nextEntryID: ");
    Serial.println(nextEntryID);
#endif
    // Test if first memory slot contains any information
    if(ID_temp == 0xFFFFFFFF || ID_temp < nextEntryID)
    {
      break;
    }
    addressEntryN += ENTRY_SIZE_LINEAR_LOGS;
    nextEntryID = ID_temp+1;
    setTime(Time_temp);

#ifdef DEBUG_LOGS
    Serial.print("current nextEntryID:"); 
    Serial.println(nextEntryID);
#endif
  }
#ifdef DEBUG_LOGS
  Serial.print("Final nextEntryID:"); 
  Serial.println(nextEntryID);
#endif
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
  printLogN(output, nextEntryID-1);
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

NIL_WORKING_AREA(waThreadLogger, 200);
NIL_THREAD(ThreadLogger, arg) {

  nilThdSleepMilliseconds(1000);
  recoverLastEntryN();
  while(TRUE) {
    writeLog();
    nilThdSleepMilliseconds(500);
  }
}




