#include <SST.h>
#include <SPI.h>
#include <avr/io.h>

//SS on pin 4
#define FLASH_SSn 4 // 4 = PORTD(4)
boolean wr;
SST sst(FLASH_SSn);



#define ADDRESS_BEG   0x000000
#define ADDRESS_MAX   0x800000
// #define ADDRESS_MAX   0x800000 // http://www.sst.com/dotAsset/40498.pdf&usd=2&usg=ALhdy294tEkn4s_aKwurdSetYTt_vmXQhw
#define SECTOR_SIZE       4096 // anyway the size of the sector is also hardcoded in the library !!!!

#define LINE_SIZE 64 // should be a divider of the SECTOR_SIZE


// ======================================================================================= //

void setup()
{ 
  Serial.begin(9600);

  delay(5000);
  Serial.println("Getting memory flash ID");

  sst = SST(FLASH_SSn);
 setupMemory(sst);

  delay(2000);
  sst.printFlashID(&Serial);
delay(1000);
Serial.println("Done");

}

// ======================================================================================= //



void loop() 
{

}




void setupMemory(SST sst){
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  sst.init();
}















