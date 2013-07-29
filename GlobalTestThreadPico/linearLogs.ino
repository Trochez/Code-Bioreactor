#include <SST.h>
#include <SPI.h>

//size of every new entry (4 bytes for the timestamp)
#define ENTRY_SIZE (MAX_PARAM+4)
SST sst = SST(4);
int entry = 0;

NIL_WORKING_AREA(waThreadLog, 70);
NIL_THREAD(ThreadLog, arg) {
  
  //The address of the actual address for new entry in the memory
  uint32_t entry = 0;

  setupMemory();
  //entry = 0;
  
  //time NTP
  int start = now();
  
  while(true){
      int time = now();
      //This function suppose that the thread is called very regularly
      //We should find a better approach : maybe using interruptions ?
      if(start - time >0){
         writeLog(time, getParametersTable());
         entry = updateEntry();
      }
      start = time;
      nilThdSleepMilliseconds(200);
  }
}

void setupMemory(){
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  sst.init();
}


//Update the value of the actual entry
int updateEntry(){
  return (entry + ENTRY_SIZE);
}

int getEntry(){
   return entry; 
}

//Write in the memory the data with a timestamp. The data has a predifined & invariable size
void writeLog(uint32_t timestamp, int* data){
     sst.flashWriteInit(getEntry());
     //Write timestamp
     for(int i=0; i<4; i++){
        //write the 4 bytes of the timestamp in the memory using a mask
        sst.flashWriteNext((timestamp >> ((4-i-1)*8)) & 0xFF); 
     }
     for(int i=0; i<MAX_PARAM; i++){
        sst.flashWriteNext(data[i]);
    }
    sst.flashWriteFinish();
}

//Read the last entry in the memory. It fills the table with all the parameters
//Have to give a sufficiently large table to the function
uint8_t* readLastEntry(uint8_t* result) {
    uint32_t address = getEntry();
    //compute the address of the last row (4 byte for the timestamp)
    address = address - ENTRY_SIZE;
    sst.flashReadInit(address);
    for(int i=0; i<ENTRY_SIZE; i++){
         result[i] = sst.flashReadNext();
    }
    sst.flashReadFinish();
    return result;
}

//Read the last n parameters of the desired value
uint8_t* readLast(uint8_t* result, uint8_t parameter, uint8_t n){
  
    uint32_t address = getEntry();
    //compute the address of the last row (4 byte for the timestamp)
    address = address - (MAX_PARAM- parameter-1);
    
    for(int i=0; i<n; i++){
         sst.flashReadInit(address);
         result[i] = sst.flashReadNext();
         //TODO : is it necessary in the loop ?
         sst.flashReadFinish();
         //Update Addresss
         address = address- ENTRY_SIZE;
    }
    return result;
}

//Give the timestamp of the last n entry in the memory
uint32_t* readLastTimestamp(uint32_t* timestamp, uint8_t n){
    uint32_t address = getEntry();
   
    for(int i=0; i<n; i++){
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
