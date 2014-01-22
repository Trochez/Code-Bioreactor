/*
  This thread will take care of the logs and manage the time and its synchronisation 
  The thread write the logs at a definite fixed interval of time in the SST25VF064 chip of the main boards
  The time synchronization works through the NTP protocol and our server

	// TODO: change the time now() in global?
*/


//Log libraries TODO: should be completed and corrected, still problem with the C/C++ compiler
//#include <Log.h>

//Prototypes
void writeLog(uint8_t log_type, uint32_t* entryNb, uint32_t timestamp, uint16_t event_number, uint16_t parameter_value);
void readLastEntry(uint8_t log_type, uint8_t* result);
uint8_t readEntryN(uint8_t log_type, uint8_t* result, uint32_t entryN);
uint8_t getLogsN(uint8_t log_type, SST sst, uint8_t* result, uint32_t entryN);
uint16_t findSectorOfN(uint8_t log_type, uint32_t entryNb);
void updateEntryN(uint8_t log_type, uint32_t* entryN);
uint32_t findAddressOfEntryN(uint8_t logs_type, uint32_t entryN);
uint32_t findNextEntryN(uint8_t log_type, uint32_t entryN);
uint32_t findPreviousEntryN(uint8_t logs_type, uint32_t entryN);
uint32_t findLastEntryN(uint8_t log_type);
uint32_t getLastEntrySec();
uint32_t getLastEntryMin();

#ifdef DEBUG_ETHERNET
  void printDigits(int digits);
  void digitalClockDisplay();
#endif


#define NTP_PACKET_SIZE 48

// the different types of logs
#define COMMAND_LOGS               101
#define RRD_SEC_LOGS               115
#define ENTRY_SIZE_LINEAR_LOGS     32
#define ENTRY_SIZE_COMMAND_LOGS    12
#define NB_PARAMETERS_LINEAR_LOGS  12
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
#define ADDRESS_CMD_BEG   0x620000

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
  Function to save logs in the Flash memory. Linear logs and command logs need to be logged
  with this function.
  
  log_type:        The type of logs that should be stored (RRD_SEC_LOGS, RRD_MIN_LOGS, 
                   RRD_HOUR_LOGS, COMMAND_LOGS)
  entryNb:         Correspond to the log ID, the entry number in the memory
  timestamp:       The time when the event happend
  event_number:    If the log_type is COMMAND_LOGS, then this parameter should be set with the
                   corresponding command/event number. Should be found in the define list of
                   commands/errors
                   If the log_type is other, this parameter should be set to 0
  parameter_value: If the log_type is COMMAND_LOGS, this value add information of the event_number
                   If the log_type is other, this parameter should be set to 0
*/
void writeLog(uint8_t log_type, uint32_t * entryNb, uint32_t timestamp, uint16_t event_number, uint16_t parameter_value) {
  
  SST sst = SST(4);
  setupMemory(sst);
  
  uint16_t param = 0;
  // test if it is the begining of one sector, and erase the sector of 4094 bytes if needed
  switch(log_type)
  {
    case COMMAND_LOGS:
      if(!(*entryNb % NB_ENTRIES_CMD)) 
        sst.flashSectorErase(findSectorOfN(COMMAND_LOGS, *entryNb));
      break;
    
    case RRD_SEC_LOGS:
      if(!(*entryNb % NB_ENTRIES_SEC)) 
        sst.flashSectorErase(findSectorOfN(RRD_SEC_LOGS, *entryNb));
      break;
      

    
  }
  
  // Initialized the flash memory with the right address in the memory
  sst.flashWriteInit(findAddressOfEntryN(log_type, *entryNb));
  
  // Write the 4 bytes of the entry number
  sst.flashWriteNextInt32(*entryNb);
  
  // Write the 4 bytes of the timestamp in the memory using a mask
  sst.flashWriteNextInt32(timestamp);
  switch(log_type)
  {
    case COMMAND_LOGS:
      // write the byte of the event number
      sst.flashWriteNextInt16(event_number);
      
      // if needed the parameter value 
      if(event_number & 0x80) {
        sst.flashWriteNextInt16(parameter_value); 
      }
    break;
    
    case RRD_SEC_LOGS:
      for(int i = 0; i < NB_PARAMETERS_LINEAR_LOGS; i++) {
        param = getParameter(i);
        // write the 2 bytes of the parameters in the memory using a mask
        sst.flashWriteNextInt16(param);
      }
      break;
  }
  
  // finish the process of writing the data in memory
  sst.flashWriteFinish();
  
  //Update the value of the next event log position in the memory
  updateEntryN(log_type, entryNb);
}

/*
  Read the last logs in the memory of a corresponding log types (seconds, commands/event, etc) 
  
  log_type:        The type of logs that should be stored (RRD_SEC_LOGS, RRD_MIN_LOGS, 
                   RRD_HOUR_LOGS, COMMAND_LOGS)
  result:          Array of uint8_t where the logs are stored. It should be a 32 bytes array
                   for the 3 RRD logs and 12 bytes for the commands/events logs.  
  *entryN:         Pointer that gives the log ID that will correspond to the logs address to
                   be read and stored in result
  
*/
void readLastEntry(uint8_t log_type, uint8_t* result) {
  
  SST sst = SST(4);
  setupMemory(sst); 
  
  uint32_t address = NULL;
  
  //compute the address of the last row (4 byte for the timestamp)
  switch(log_type)
  {
    case COMMAND_LOGS:
      address = findAddressOfEntryN(log_type, findPreviousEntryN(log_type, newEntryCmd));
      break;
    case RRD_SEC_LOGS: 
      address = findAddressOfEntryN(log_type, findPreviousEntryN(log_type, newEntryRRDSec));
      break;
  } 
  
  // Initializate the memory for the reading
  sst.flashReadInit(address);
  
  // write the time stamp and the data  
  switch(log_type)
  {
    case COMMAND_LOGS:
      for(int i = 0; i < ENTRY_SIZE_COMMAND_LOGS + 4; i++) 
        result[i] = sst.flashReadNextInt8();
      break;
    
    case RRD_SEC_LOGS: 
      for(int i = 0; i < ENTRY_SIZE_LINEAR_LOGS + 4; i++) 
        result[i] = sst.flashReadNextInt8();
      break;
  }
  
  sst.flashReadFinish();
}

/*
  Read the corresponding logs in the flash memory of the entry number (ID).

  log_type:        The type of logs that should be stored (RRD_SEC_LOGS, RRD_MIN_LOGS, 
                   RRD_HOUR_LOGS, COMMAND_LOGS)
  result:          Array of uint8_t where the logs are stored. It should be a 32 bytes array
                   for the 3 RRD logs and 12 bytes for the commands/events logs.  
  *entryN:         Log ID that will correspond to the logs address to be read and stored in
                   result
  
  return:          Error flag:
                   0: no error occured
                   ERROR_NOT_FOUND_ENTRY_N: The log ID (entryN) was not found in the
                   flash memory
*/
uint8_t readEntryN(uint8_t log_type, uint8_t* result, uint32_t entryN)
{
  SST sst = SST(4);
  setupMemory(sst); 
  
  uint32_t temp = 0;
  uint32_t addressOfEntryN = 0;
  switch(log_type)
  {
    case COMMAND_LOGS:
      addressOfEntryN = findAddressOfEntryN(log_type, entryN);
      break;
    case RRD_SEC_LOGS: 
      addressOfEntryN = findAddressOfEntryN(log_type, entryN);
      break;
  }
  sst.flashReadInit(addressOfEntryN);
  temp = sst.flashReadNextInt32();
  if(temp == entryN) {
    return getLogsN(log_type, sst, result, entryN); }
  else {
    sst.flashReadFinish();
    return ERROR_NOT_FOUND_ENTRY_N;
  }
}

/*
  Function that write the log entry in the variable result

  log_type:        The type of logs (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS)
  sst:             The object where is defined the operations to manipulate 
                   the flash memory
  result:          Array of uint8_t where the logs are stored. It should be a 32 bytes array
                   for the 3 RRD logs and 12 bytes for the commands/events logs.  
  entryN:          The log ID to be written in the variable result
  
  return:          Error flags:
                   0: no error occured
                   // Need to implement tests for error detection
  
*/
uint8_t getLogsN(uint8_t log_type, SST sst, uint8_t* result, uint32_t entryN)
{
  uint8_t index = 0;
  uint8_t restOfByte = 0;
  result[index] = (entryN >> 24) & 0xFF;index++;
  result[index] = (entryN >> 16) & 0xFF; index++;
  result[index] = (entryN >> 8) & 0xFF; index++;
  result[index] = (entryN) & 0xFF; index++;
  switch(log_type)
  {
    case RRD_SEC_LOGS:
      restOfByte = ENTRY_SIZE_LINEAR_LOGS;
      break;
    case COMMAND_LOGS:
      restOfByte = ENTRY_SIZE_COMMAND_LOGS;
      break;
  }
  
  for(index; index < restOfByte;index++) 
    result[index] = sst.flashReadNextInt8();
  sst.flashReadFinish();
  return 0;
}

/*
  This function returns the next log ID that should be use for the next entry in the memory
  corresponding to the log type.
  
  log_type:    The type of logs (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS)
  entryN:      The actual log ID
  
  return:      The next log ID to be used
*/
uint32_t findNextEntryN(uint8_t log_type, uint32_t entryN)
{
  uint32_t lastEntry = 0;
  lastEntry = (entryN +1);
  /*switch(log_type) 
  {
    case COMMAND_LOGS:
      lastEntry = (entryN + 1);
      break;
    case RRD_SEC_LOGS: 
      lastEntry = (entryN + 1);
      break;
  }*/
  return lastEntry;
}

/*
  This function returns the previous log ID that has been used for the previous entry in 
  the memory corresponding to the log type.
  
  log_type:    The type of logs (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS)
  entryN:      The actual log ID
  
  return:      The previous log ID stored in the memory
*/
uint32_t findPreviousEntryN(uint8_t logs_type, uint32_t entryN)
{
  uint32_t PreviousEntry = 0;
  if(entryN - 1 > 0){
    PreviousEntry = (entryN - 1);
  }
  else { 
    switch(logs_type) 
    {
      case COMMAND_LOGS:
        PreviousEntry = NB_ENTRIES_CMD-1;
        break;
      case RRD_SEC_LOGS: 
        PreviousEntry = NB_ENTRIES_SEC -1;
        break;
    }
  }
  return PreviousEntry;
}

/*
  The flash memory is implemented with sectors of a defined size.
  The function returns the sector number where the log corresponding to the ID (entryNb) 
  is stored in the flash memory
  
  log_type:        The type of log (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS) 
  entryNb:         The log ID
  
  return:          The sector number
  
*/
uint16_t findSectorOfN(uint8_t log_type, uint32_t entryNb) {
  uint16_t sectorNb = 0;
  uint32_t address = findAddressOfEntryN(log_type, entryNb);
  sectorNb = address / SECTOR_SIZE;
  /*switch(log_type) 
  {
    case COMMAND_LOGS:
      sectorNb = (ADDRESS_CMD_BEG / SECTOR_SIZE) + ((entryNb % NB_ENTRIES_CMD) - (entryNb % SECTOR_SIZE)) / SECTOR_SIZE;
      break;
    case RRD_SEC_LOGS: 
      sectorNb = (ADDRESS_SEC_BEG / SECTOR_SIZE) + ((entryNb % NB_ENTRIES_SEC) - (entryNb % SECTOR_SIZE)) / SECTOR_SIZE;;
      break;
  }*/
  return sectorNb;
}

/*
  Update the value of the position where a new log should be
  stored in the flash memory. Update the log ID corresponding to the respective
  log _type
  
  log_type:        The type of logs (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS) 
  entryNb:         The pointer where is stored the new log ID where should be stored the
                   corresponding new log
  
*/
void updateEntryN(uint8_t log_type, uint32_t * entryNb) {
   /*switch(log_type) 
  {
    case COMMAND_LOGS:
      *entryNb = (*entryNb + 1) % NB_ENTRIES_CMD;
      break;
    case RRD_SEC_LOGS: 
      *entryNb = (*entryNb + 1) % NB_ENTRIES_SEC;
      break;
  }*/
  *entryNb = (*entryNb + 1);
}

/*
  Function that return the corresponding address in the memory of one log ID corresponding
  to the right log type in memory (seconds, minutes, hours, commands/event)
  
  log_type:    The type of logs (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS) 
  entryNb:     The log ID
  
  return:      The address of the first byte where are stored the log corresponding to
               the log ID (entryN)
*/
uint32_t findAddressOfEntryN(uint8_t logs_type, uint32_t entryN)
{
  uint32_t address = 0;
  switch(logs_type) 
  {
    case COMMAND_LOGS:
      address = ((entryN % NB_ENTRIES_CMD) * ENTRY_SIZE_COMMAND_LOGS) % ADDRESS_CMD_SIZE + ADDRESS_CMD_BEG;
      break;
    case RRD_SEC_LOGS:
      address = ((entryN % NB_ENTRIES_SEC) * ENTRY_SIZE_LINEAR_LOGS) % ADDRESS_SEC_SIZE + ADDRESS_SEC_BEG;
      break;
  }
  return address;
}

/*
  Function that return the last log ID stored in the memory corresponding
  to the right log type in memory (seconds, minutes, hours, commands/event)
 
  log_type:    The type of logs (RRD_SEC_LOGS, RRD_MIN_LOGS, RRD_HOUR_LOGS, COMMAND_LOGS)
  
  return:      The last log ID stored in the memory corresponding to a log type
*/

uint32_t findLastEntryN(uint8_t log_type) 
{
  SST sst = SST(4);
  setupMemory(sst); 
  
  uint32_t lastEntry = 0;
  uint32_t ID_temp = 0;
  uint32_t addressEntryN = 0;
  uint32_t addressLastEntryN = 0;
  boolean found = false;
  switch(log_type) 
  {
    case COMMAND_LOGS:
      addressEntryN = ADDRESS_CMD_BEG;
      addressLastEntryN = ADDRESS_CMD_LAST;
			#ifdef DEBUG_LOGS
      	Serial.print("addressEntryN: ");Serial.println(ADDRESS_CMD_BEG);
      	Serial.print("addressLastEntryN: ");Serial.println(ADDRESS_CMD_LAST);
			#endif
      break;
    case RRD_SEC_LOGS:
      addressEntryN = ADDRESS_SEC_BEG;
      addressLastEntryN = ADDRESS_SEC_LAST;
			#ifdef DEBUG_LOGS
      	Serial.print("addressEntryN: ");Serial.println(ADDRESS_SEC_BEG);
      	Serial.print("addressLastEntryN: ");Serial.println(ADDRESS_SEC_LAST);
			#endif
      break;
  }
  while(!found) 
  {
    sst.flashReadInit(addressEntryN);    
    ID_temp = sst.flashReadNextInt32();  
    sst.flashReadFinish();          
    #ifdef DEBUG_LOGS    
      Serial.print("ID_temp1: ");Serial.println(ID_temp);
      Serial.print("lastEntry: ");Serial.println(lastEntry);
    #endif
    // Test if first memory slot contains any information
    if(ID_temp == 0xFFFFFFFF)
    {
      sst.flashReadInit(addressLastEntryN);    
      ID_temp = sst.flashReadNextInt32();
      sst.flashReadFinish();
      #ifdef DEBUG_LOGS    
        Serial.print("ID_temp2: ");Serial.println(ID_temp);
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
    	Serial.print("findNextEntryN :"); Serial.println(findNextEntryN(log_type, ID_temp));
    #endif
    addressEntryN = findAddressOfEntryN(log_type, findNextEntryN(log_type, ID_temp));
    lastEntry = ID_temp;
    #ifdef DEBUG_LOGS
    	Serial.print("address Entry N :"); Serial.println(addressEntryN);
    #endif
  }
  #ifdef DEBUG_LOGS
     Serial.print("Last Entry 2:"); Serial.println(lastEntry);
  #endif
  return lastEntry;
}

uint32_t getLastEntrySec() {return findPreviousEntryN(RRD_SEC_LOGS, newEntryRRDSec);}
uint32_t getLastEntryCmd() {return findPreviousEntryN(COMMAND_LOGS, newEntryCmd);}

/*-----------------------
  NTP related functions
-----------------------*/

boolean updateNTP(EthernetUDP Udp, byte* packetBuffer ){
   if ( Udp.parsePacket() ) {
     
     #ifdef DEBUG_ETHERNET
       Serial.println("receive");
     #endif
     
       // We've received a packet, read the data from it
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word( packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word( packetBuffer[42],  packetBuffer[43]);  
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;  
      
      // now convert NTP time into everyday time:
      const unsigned long seventyYears = 2208988800UL;     
      // subtract seventy years, the hour in Colombia: -5 from GMT
      unsigned long epoch = secsSince1900 - seventyYears - 5 * 3600; 
      
      #ifdef DEBUG_ETHERNET 
        Serial.println(epoch);
      #endif
      
      setTime(epoch);
      return true;
   } else {
      #ifdef DEBUG_ETHERNET
        Serial.println("no packet received");
      #endif
      //TODO: What should we do here ? New update 
      return false;
   }
   
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(EthernetUDP * Udp, IPAddress address, byte * packetBuffer)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:         
  (*Udp).beginPacket(address, 123); //NTP requests are to port 123
  (*Udp).write(packetBuffer, NTP_PACKET_SIZE);
  (*Udp).endPacket();
  
  // TODO: implement error detection
  return 0;
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


#ifdef DEBUG_ETHERNET
  // function to print formated digit
  void printDigits(int digits){
    // utility for digital clock display: prints preceding colon and leading 0
    Serial.print(":");
    if(digits < 10)
      Serial.print('0');
    Serial.print(digits);
  }
  
  // print the actuall time
  void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(month());
    Serial.print(" ");
    Serial.print(year()); 
    Serial.println(); 
  }
#endif
