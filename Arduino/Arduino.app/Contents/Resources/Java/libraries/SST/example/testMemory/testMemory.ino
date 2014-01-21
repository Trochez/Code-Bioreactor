#include <SST.h>
#include <SPI.h>
#include <avr/io.h>

char buf[16];

//SS on pin 4
#define FLASH_SSn 4 // 4 = PORTD(4)
boolean wr;
SST sst(FLASH_SSn);
// ======================================================================================= //

void setup()
{
  
  Serial.begin(38400);
  wr = false;
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  sst.init();
}

// ======================================================================================= //

void loop() 
{
  uint32_t address = 40;
  uint8_t data[] = {25, 1, 4, 18};
  uint8_t dataRead[4];
  //Write or Read in Memory
  if(wr == false){
    sst.flashWriteInit(address);
    for(int i=0; i<4; i++){
      sst.flashWriteNext(data[i]);
    }
    sst.flashWriteFinish();
    //flashWriteByte(address,data);
    wr=true;
  }
  
  
  sst.flashReadInit(address);
  //flashReadNext is much faster than changing the address every time
  for(int i=0; i<4; i++){
      dataRead[i] = sst.flashReadNext();
  }
  sst.flashReadFinish();
  
  for(int i=0; i<4; i++){
      Serial.print(dataRead[i]);
      Serial.print(' ');
    }
  Serial.println();
  delay(1000);
}
