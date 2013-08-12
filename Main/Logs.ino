/*
  This thread will take care of the logs and manage the time and its synchronisation 
  The thread write the logs at a definite fixed interval of time in the SST25VF064 chip of the main boards
  The time synchronization works through the NTP protocol and our server
*/
#ifdef THR_LINEAR_LOGS

//Memory libraries
#include <SST.h>
#include <SPI.h>

//Ethernet libraries
#include <Ethernet.h>
#include <EthernetUdp.h>

// Prototypages
void setupMemory(SST sst);
void updateEntryN(uint8_t log_type);
void writeLog(uint8_t log_type,SST sst, uint32_t* addr, uint32_t timestamp, uint16_t event_number, uint16_t parameter_value);
uint8_t* readLastEntry(SST sst, uint32_t* addr, uint8_t* result);
uint8_t* readLastNParameters(SST sst, uint32_t* addr, uint8_t* result, uint8_t parameter, uint8_t n);
uint32_t* readLastTimestamp(SST sst, uint32_t* addr, uint32_t* timestamp, uint8_t n);
//uint32_t findAddress(uint8_t logs_type);
uint32_t findLastEntryN(SST sst, uint8_t logs_type);
uint32_t findAddressOfEntryN(uint8_t logs_type, uint32_t entryN);
void sendPacket(EthernetUDP& Udp, IPAddress& server, unsigned char packetBuffer[] );
boolean updateNTP(EthernetUDP& Udp, IPAddress& server, unsigned char packetBuffer[]);
unsigned long sendNTPpacket(EthernetUDP& Udp, IPAddress& address, unsigned char packetBuffer[]);

#define NTP_PACKET_SIZE (48)

// the different types of logs
#define COMMAND_LOGS               1  //
#define RRD_SEC_LOGS               2  // If Needed
#define RRD_MIN_LOGS               3  // If Needed
#define RRD_HOUR_LOGS              4  // If Needed

#define ENTRY_SIZE_LINEAR_LOGS     32
#define ENTRY_SIZE_COMMAND_LOGS    12
#define NB_PARAMETERS_LINEAR_LOGS  12
#define SIZE_TIMESTAMPS            4
#define SIZE_COUNTER_ENTRY         4

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
#
#define ERROR_ERASE_SECTOR     140
#define PARAMETER_SET          130
#define ERROR_NOT_FOUND_ENTRY_N  150
#


// Definition of the log sectors in the flash for the logs
#define ADDRESS_SEC_BEG   0x000000
#define ADDRESS_MIN_BEG   0x3F4000
#define ADDRESS_HOUR_BEG  0x5EE000
#define ADDRESS_CMD_BEG   0x620000

#define SECTOR_SIZE       4096

// Size of the log sectors in bytes
#define ADDRESS_SEC_SIZE  (ADDRESS_MIN_BEG  - ADDRESS_SEC_BEG)
#define ADDRESS_MIN_SIZE  (ADDRESS_HOUR_BEG - ADDRESS_MIN_BEG)
#define ADDRESS_HOUR_SIZE (ADDRESS_CMD_BEG  - ADDRESS_HOUR_BEG)
#define ADDRESS_CMD_SIZE  0x020000

// The number of entires by types of logs (seconds, minutes, hours, commands/events)
#define NB_ENTRIES_SEC    (ADDRESS_SEC_SIZE  / ENTRY_SIZE_LINEAR_LOGS) 
#define NB_ENTRIES_MIN    (ADDRESS_MIN_SIZE  / ENTRY_SIZE_LINEAR_LOGS)
#define NB_ENTRIES_HOUR   (ADDRESS_HOUR_SIZE / ENTRY_SIZE_LINEAR_LOGS)
#define NB_ENTRIES_CMD    (ADDRESS_CMD_SIZE  / ENTRY_SIZE_COMMAND_LOGS)

NIL_WORKING_AREA(waThreadLinearLog, 100); //TODO : Check the actual memory requirement : 70 Bytes might be a bit short
NIL_THREAD(ThreadLinearLog, arg) {
    //The memory is on PORTD4
  SST sst = SST(4);
  setupMemory(sst); 
  //entry = 0;
  /*----------------------------------
    Memory setup
  ----------------------------------*/
  
   //Determine the position of the last logs in the memory for
   // all type of logs (linear, RRD, commands/event)
   // If we don't have enough memory in the RAM, let's use directly the function findAddress
   // but it will be slower to call this function every seconds for log processes
  //uint32_t* addrCommand  = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrCommand  = findAddress(COMMAND_LOGS);
  //uint32_t* addrRRDSec   = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrRRDSec   = findAddress(RRD_SEC_LOGS);
  //uint32_t* addrRRDSMin  = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrRRDSMin  = findAddress(RRD_MIN_LOGS);
  //uint32_t* addrRRDSHour = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrRRDSHour = findAddress(RRD_HOUR_LOGS);
  uint32_t* newEntryCmd      = (uint32_t*)calloc(1,sizeof(uint32_t)); *newEntryCmd  = findLastEntryN(sst, COMMAND_LOGS);
  uint32_t* newEntryRRDSec   = (uint32_t*)calloc(1,sizeof(uint32_t)); *newEntryRRDSec   = findLastEntryN(sst, RRD_SEC_LOGS);
  uint32_t* newEntryRRDSMin  = (uint32_t*)calloc(1,sizeof(uint32_t)); *newEntryRRDSMin  = findLastEntryN(sst, RRD_MIN_LOGS);
  uint32_t* newEntryRRDSHour = (uint32_t*)calloc(1,sizeof(uint32_t)); *newEntryRRDSHour = findLastEntryN(sst, RRD_HOUR_LOGS);
  
  

  
  /*----------------------------------
    ethernet & NTP Setup
  ----------------------------------*/
  
  //const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
  unsigned char packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

  // Boolean variable to test a t the first place several times the actuall
  // time of the arduino. Count 5 times the arduino time before synchronization
  time_t time_now = 0;
  time_t previousNTP = 0;
  time_t previousLog = 0;
  boolean waitPacket = false;
  // A UDP instance to let us send and receive packets over UDP
  EthernetUDP Udp;
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
    
  /*--------------------------------
    Thread Main Loop
  --------------------------------*/
  while(true) {
      Serial.println("test!");
      /****************************
      THREAD LOG & TIME : STRUCTURE    
      - Update NTP all days
      Send packet
        2sec later check if answer
          answer -> update
          no answer -> log in event + try again in 3600 seconds
      - Log parameter every 1 second
      *****************************/
      time_now = now();
      
      if(!waitPacket && time_now - previousNTP >= 24*60*60) {
        sendPacket(Udp,alix_server, packetBuffer);
        waitPacket = true;
      } 
      // 2 seconds later we check if we have an answer from the server and update the time if possible
      else if(waitPacket && time_now - previousNTP >= 3602) {
        boolean success = updateNTP(Udp,alix_server, packetBuffer);
        if(!success) {
          Serial.println("Fail NTP update");  // A virer
           writeLog(COMMAND_LOGS, sst, newEntryCmd, time_now, NO_ANSWER_NTP_SERVER, 0); //TODO :update the function 
        }
        previousNTP = time_now;
        waitPacket = false;
      }
      
      // This function suppose that the thread is called very regularly (at least 1 time every seconds)
      // this is the linear logs
      // 
      if(time_now - previousLog > 1) {
        writeLog(RRD_SEC_LOGS, sst, newEntryRRDSec , time_now, 0, 0);
        Serial.println("Log");  // A Virer
        previousLog = time_now;
      }
      
      nilThdSleepMilliseconds(500);
  }
}

/* 
  Function to save the events in the Flash memory
  
  sst:             The object where is defined the operations to manipulate 
                   the flash memory
  addr:            The location in the memory where the command logs should be writen
  timesamp:        The time when the event happend 
  parameter_value: The value of the parameter (used when a user changes the value of
                   on of the 26 variables)
*/
void writeLog(uint8_t log_type, SST sst, uint32_t* entryNb, uint32_t timestamp, uint16_t event_number, uint16_t parameter_value) {
  uint16_t param = 0;
  
  // test if it is the begining of one sector, and erase the sector of 4094 bytes if needed
  if(!(*entryNb % SECTOR_SIZE))
    sst.flashSectorErase(findSectorOfN(log_type, *entryNb));
      
  // Initialized the flash memory with the rigth address in the memory
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
  Read the last entry in the memory. It fills the table with all the parameters
  Have to give a sufficiently large table to the function
  
  log_types:
  sst:
  addr:
  result:
  
  return:
*/
void readLastEntry(uint8_t log_type, SST sst, uint8_t* result, uint32_t* entryN) {
  uint32_t address = NULL;
  
  //compute the address of the last row (4 byte for the timestamp)
  switch(log_type)
  {
    case COMMAND_LOGS:
      address = findAddressOfEntryN(log_type ,findPreviousEntryN(log_type, *entryN));
      break;
    case RRD_SEC_LOGS: 
      address = findAddressOfEntryN(log_type ,findPreviousEntryN(log_type, *entryN));
      break;
    case RRD_MIN_LOGS:
      address = findAddressOfEntryN(log_type ,findPreviousEntryN(log_type, *entryN));
      break;
    case RRD_HOUR_LOGS:
      address = findAddressOfEntryN(log_type ,findPreviousEntryN(log_type, *entryN));
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

uint8_t readEntryN(uint8_t log_type, SST sst, uint8_t* result, uint32_t entryN)
{
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
    case RRD_MIN_LOGS:
      addressOfEntryN = findAddressOfEntryN(log_type, entryN);
      break;
    case RRD_HOUR_LOGS:
      addressOfEntryN = findAddressOfEntryN(log_type, entryN);
      break;
  }
  
  sst.flashReadInit(addressOfEntryN);
  temp = sst.flashReadNextInt32();
  if(temp == entryN)
    return getLogsN(log_type, sst, entryN, result);;
  else
    return ERROR_NOT_FOUND_ENTRY_N;
}

// Can implement return error message
uint8_t getLogsN(uint8_t log_type, SST sst, uint32_t entryN, uint8_t* result)
{
  uint8_t index = 0;
  uint8_t restOfByte = 0;
  result[index] = entryN >> 24;
  result[++index] = entryN >> 16;
  result[++index] = entryN >> 8;
  result[++index] = entryN;
  
  switch(log_type)
  {
    case RRD_SEC_LOGS: case RRD_MIN_LOGS: case RRD_HOUR_LOGS:
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

uint16_t findSectorOfN(uint8_t log_type, uint32_t entryNb) {
  uint16_t sectorNb = 0;
  switch(log_type) 
  {
    case COMMAND_LOGS:
      sectorNb = (ADDRESS_CMD_BEG / SECTOR_SIZE) + ((entryNb % NB_ENTRIES_CMD) - (entryNb % SECTOR_SIZE)) / SECTOR_SIZE;
      break;
    case RRD_SEC_LOGS: 
      sectorNb = (ADDRESS_SEC_BEG / SECTOR_SIZE) + ((entryNb % NB_ENTRIES_SEC) - (entryNb % SECTOR_SIZE)) / SECTOR_SIZE;;
      break;
    case RRD_MIN_LOGS: 
      sectorNb = (ADDRESS_MIN_BEG / SECTOR_SIZE) + ((entryNb % NB_ENTRIES_MIN) - (entryNb % SECTOR_SIZE)) / SECTOR_SIZE;;
      break;
    case RRD_HOUR_LOGS: 
      sectorNb = (ADDRESS_HOUR_BEG / SECTOR_SIZE) + ((entryNb % NB_ENTRIES_HOUR) - (entryNb % SECTOR_SIZE)) / SECTOR_SIZE;;
      break;
  }
  return sectorNb;
}

// Update the value of the position where a new events should be
// logged in the memory
void updateEntryN(uint8_t log_type, uint32_t* entryN) {
   switch(log_type) 
  {
    case COMMAND_LOGS:
      *entryN = (*entryN + 1) % NB_ENTRIES_CMD;
      break;
    case RRD_SEC_LOGS: 
      *entryN = (*entryN + 1) % NB_ENTRIES_SEC;
      break;
    case RRD_MIN_LOGS: 
      *entryN = (*entryN + 1) % NB_ENTRIES_MIN;
      break;
    case RRD_HOUR_LOGS: 
      *entryN = (*entryN + 1) % NB_ENTRIES_HOUR;
      break;
  }
}

/*
  Function that return the corresponding address in the memory of one entry number corresponding
  to the right sector of memory (seconds, minutes, hours, commands/event)
  
  logs_type:
  entryN:
  
  return:
*/
uint32_t findAddressOfEntryN(uint8_t logs_type, uint32_t entryN)
{
  uint32_t address = 0;
  switch(logs_type) 
  {
    case COMMAND_LOGS:
      address = (entryN % ADDRESS_CMD_SIZE) * ENTRY_SIZE_COMMAND_LOGS + ADDRESS_CMD_BEG;
      break;
    case RRD_SEC_LOGS:
      address = (entryN % ADDRESS_SEC_SIZE) * ENTRY_SIZE_LINEAR_LOGS + ADDRESS_SEC_BEG;
      break;
    case RRD_MIN_LOGS:
      address = (entryN % ADDRESS_MIN_SIZE) * ENTRY_SIZE_LINEAR_LOGS + ADDRESS_MIN_BEG;
      break;
    case RRD_HOUR_LOGS:
      address = (entryN % ADDRESS_HOUR_SIZE) * ENTRY_SIZE_LINEAR_LOGS + ADDRESS_HOUR_BEG;
      break;
  }
  return address;
}

uint32_t findLastEntryN(SST sst, uint8_t log_type) {
  uint32_t lastEntry = 0;
  uint32_t temp = 0;
  uint32_t addressEntryN = 0;
  boolean found = true;
  switch(log_type) 
  {
    case COMMAND_LOGS:
      addressEntryN = ADDRESS_CMD_BEG;
      break;
    case RRD_SEC_LOGS:
      addressEntryN = ADDRESS_SEC_BEG;
      break;
    case RRD_MIN_LOGS: 
      addressEntryN = ADDRESS_MIN_BEG;
      break;
    case RRD_HOUR_LOGS: 
      addressEntryN = ADDRESS_HOUR_BEG;
      break;
  }
  while(found) {
    sst.flashReadInit(addressEntryN);    
    temp = sst.flashReadNextInt32();
    if(temp == 0xFFFFFFFF || temp != lastEntry + 1) {// A Vérifier
      found = true;
    }
    sst.flashReadFinish();
    lastEntry = temp;
    addressEntryN = findAddressOfEntryN(log_type, findNextEntryN(log_type, lastEntry));
  }
  return lastEntry;
}

uint32_t findNextEntryN(uint8_t logs_type, uint32_t entryN)
{
  uint32_t lastEntry = 0;
  switch(logs_type) 
  {
    case COMMAND_LOGS:
      lastEntry = (entryN + 1) % NB_ENTRIES_CMD;
      break;
    case RRD_SEC_LOGS: 
      lastEntry = (entryN + 1) % NB_ENTRIES_SEC;
      break;
    case RRD_MIN_LOGS: 
      lastEntry = (entryN + 1) % NB_ENTRIES_MIN;
      break;
    case RRD_HOUR_LOGS: 
      lastEntry = (entryN + 1) % NB_ENTRIES_HOUR;
      break;
  return 0;
  }
}

uint32_t findPreviousEntryN(uint8_t logs_type, uint32_t entryN)
{
  uint32_t PreviousEntry = 0;
  switch(logs_type) 
  {
    case COMMAND_LOGS:
      PreviousEntry = (entryN - 1 + NB_ENTRIES_CMD) % NB_ENTRIES_CMD;
      break;
    case RRD_SEC_LOGS: 
      PreviousEntry = (entryN - 1 + NB_ENTRIES_SEC) % NB_ENTRIES_SEC;
      break;
    case RRD_MIN_LOGS: 
      PreviousEntry = (entryN - 1 + NB_ENTRIES_MIN) % NB_ENTRIES_MIN;
      break;
    case RRD_HOUR_LOGS: 
      PreviousEntry = (entryN - 1 + NB_ENTRIES_HOUR) % NB_ENTRIES_HOUR;
      break;
  }
  return 0;
}

/*-----------------------
  NTP related functions
-----------------------*/
void sendPacket(EthernetUDP& Udp, IPAddress& server, unsigned char packetBuffer[] ){
   sendNTPpacket(Udp, server, packetBuffer); // send an NTP packet to a time server 
}

boolean updateNTP(EthernetUDP& Udp, IPAddress& server, unsigned char packetBuffer[] ){
   if ( Udp.parsePacket() ) {  
       // We've received a packet, read the data from it
      Udp.read(packetBuffer, (size_t) NTP_PACKET_SIZE);  // read the packet into the buffer
  
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
      
      setTime(epoch);
      return true;
   } else {
      //TODO: What should we do here ? New update 
      return false;
   }
   
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(EthernetUDP& Udp, IPAddress& address, unsigned char packetBuffer[])
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
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
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

#endif
