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
void updateAddrLogs(uint32_t* addr, uint8_t entrySize);
void writeLog(SST sst, uint32_t addr, uint32_t timestamp, int* data);
uint8_t* readLastEntry(SST sst, uint32_t* addr, uint8_t* result);
uint8_t* readLast(SST sst, uint32_t* addr, uint8_t* result, uint8_t parameter, uint8_t n);
uint32_t* readLastTimestamp(SST sst, uint32_t* addr, uint32_t* timestamp, uint8_t n);
uint32_t findAddress(uint8_t logs_type);
void sendPacket(EthernetUDP& Udp, IPAddress& server, unsigned char packetBuffer[] );
boolean updateNTP(EthernetUDP& Udp, IPAddress& server, unsigned char packetBuffer[] );
unsigned long sendNTPpacket(EthernetUDP& Udp, IPAddress& address, unsigned char packetBuffer[]);

//size of every new entry (4 bytes for the timestamp)
#define ENTRY_SIZE (MAX_PARAM + 4)

#define NTP_PACKET_SIZE (48)

// the different types of logs
#define LINEAR_LOGS    1
#define COMMAND_LOGS   2
#define RRD_SEC_LOGS   3  //If Needed
#define RRD_MIN_LOGS   4  //If Needed
#define RRD_HOUR_LOGS   5  //If Needed



NIL_WORKING_AREA(waThreadLinearLog, 100); //TODO : Check the actual memory requirement : 70 Bytes might be a bit short
NIL_THREAD(ThreadLinearLog, arg) {
  
  /*----------------------------------
    Memory setup
  ----------------------------------*/
   //Determine the position of the last logs in the memory for
   // all type of logs (linear, RRD, commands/event)
  uint32_t* addrLinear   = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrLinear   = findAddress(LINEAR_LOGS);
  uint32_t* addrCommand  = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrCommand  = findAddress(COMMAND_LOGS);
  uint32_t* addrRRDSec   = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrRRDSec   = findAddress(RRD_SEC_LOGS);
  uint32_t* addrRRDSMin  = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrRRDSMin  = findAddress(RRD_MIN_LOGS);
  uint32_t* addrRRDSHour = (uint32_t*)calloc(1,sizeof(uint32_t)); *addrRRDSHour = findAddress(RRD_HOUR_LOGS);
  //The memory is on PORTD4
  SST sst = SST(4);
  setupMemory(sst);
  //entry = 0;
  
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
  while(true){
    
      /****************************
      THREAD LOG & TIME : STRUCTURE    
      - Update NTP all the 3600 seconds
        Send packet
        2sec later check if answer
          answer -> update
          no answer -> log in event + try again in 3600 seconds
      - Log parameter every 1 second
      *****************************/
      
      time_now = now();
      
      if(!waitPacket && time_now - previousNTP >= 3600){
        sendPacket(Udp,alix_server, packetBuffer);
        waitPacket = true;
      } 
      // 2 seconds later we check if we have an answer from the server and update the time if possible
      else if(waitPacket && time_now - previousNTP >= 3602){
        boolean success = updateNTP(Udp,alix_server, packetBuffer);
        if(!success) {
           writeCommandLog(sst, addrCommand, NO_ANSWER_NTP_SERVER,  time_now, 0); //TODO :update the function 
        }
        previousNTP = time_now;
        waitPacket = false;
      }
      
      // This function suppose that the thread is called very regularly (at least 1 time every seconds)
      // this is the linear logs
      // 
      if(time_now - previousLog > 1) {
        writeLog(sst, addrLinear , time_now, dataToLog());
        previousLog = time_now;
      }
      
      nilThdSleepMilliseconds(500);
  }
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

// Update the value of the position where a new events should be
// logged in the memory
void updateAddrLogs(uint32_t* addr, uint8_t entrySize){
  
  *addr = *addr + sizeEntry;
  /*
  switch(log_type)
  {
    case LINEAR_LOGS:
      *addrLinear = (*addrLinear + sizeEntry);
      break;
    case COMMAND_LOGS:
      *addrCommand = (*addrCommand + sizeEntry);
      break;
    case RRD_SEC_LOGS:
      *addrRRDSec = (*addrRRDSec + sizeEntry);
      break;
    case RRD_MIN_LOGS:
      *addrRRDSMin = (*addrRRDSMin + sizeEntry);
      break;
    case RRD_HOUR_LOGS: 
      *addrRRDSHour = (*addrRRDSHour + sizeEntry);
      break;
      
     
  }
  */
}


//Write in the memory the data with a timestamp. The data has a predifined & invariable size
void writeLog(SST sst, uint32_t* addr, uint32_t timestamp, int* data, uint8_t entrySize){
     sst.flashWriteInit(*addr);
     //Write timestamp
     for(int i = 0; i < 4; i++){
        //write the 4 bytes of the timestamp in the memory using a mask
        sst.flashWriteNext((timestamp >> ((4-i-1)*8)) & 0xFF); 
     }
     for(int i = 0; i < entrySize; i++){
        sst.flashWriteNext(data[i]);
    }
    sst.flashWriteFinish();
    //Update Address of writing
    updateAddr(addr,entrySize);
}

//Read the last entry in the memory. It fills the table with all the parameters
//Have to give a sufficiently large table to the function
uint8_t* readLastEntry(SST sst, uint32_t* addr, uint8_t* result) {
    //compute the address of the last row (4 byte for the timestamp)
    uint32_t address = *addr - ENTRY_SIZE;
    sst.flashReadInit(address);
    for(int i=0; i<ENTRY_SIZE; i++){
         result[i] = sst.flashReadNext();
    }
    sst.flashReadFinish();
    return result;
}

//Read the last n parameters of the desired value
uint8_t* readLast(SST sst, uint32_t* addr, uint8_t* result, uint8_t parameter, uint8_t n){
  
    //compute the address of the last row (4 byte for the timestamp)
    uint32_t address = *addr - (MAX_PARAM - parameter - 1);
    
    for(int i=0; i<n; i++){
         sst.flashReadInit(address);
         result[i] = sst.flashReadNext();
         //TODO : is it necessary in the loop ?
         sst.flashReadFinish();
         //Update Addresss
         address = address - ENTRY_SIZE;
    }
    return result;
}

//Give the timestamp of the last n entry in the memory
uint32_t* readLastTimestamp(SST sst, uint32_t* addr, uint32_t* timestamp, uint8_t n){
    uint32_t address = *addr;
   
    for(int i=0; i<n; i++) {
        //compute the address of the last entry
        address = address - ENTRY_SIZE;
        sst.flashReadInit(address);
        uint32_t time = 0;
        for(int j=0; j<4; j++){
           //add the time corresponding to the byte (1st byte = higher)
           time = time + (sst.flashReadNext() << ((4-j-1)*8));

        }
       timestamp[i] = time;
       sst.flashReadFinish();
    }
    return timestamp;
}

//TODO: to be implemented
uint32_t findAddress(uint8_t logs_type) {
  switch(logs_type) {
    case LINEAR_LOGS:
      break;
    case COMMAND_LOGS:
      break;
    case RRD_LOGS: 
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

#endif
