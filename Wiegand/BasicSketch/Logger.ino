#include <EXROM.h>

/*
EEPROM is devided in 2 parts
 
 0->511 : allowed IDs
 
 8 bits for allowed door
 24 bits for the ID
 
 512 -> 1023 : logs
 
 
 // epoch can be retried using: now()
 
 */

/*
  The ID contains
 8 bits with the doors that can be open
 24 bits with the key value
 
 If no doors are allowed: delete the entry
 */
boolean pushID ( uint32_t id ) {

  return true;
}

void getLogAfter(uint32_t time, Print* output) {
  // need to check if there is any new record
  // send the log record
  // NEED TO TAKE CARE THAT THERE ARE NEVER 2 LOGS AT THE SAME TIME

  // just an example
  printLogLine(14, output);

}

void printLogLine(int line, Print* output) {
  byte tmpByte;
  int start=line*8;
  int end=line*8+8;
  byte checkDigit=0;
  for (int i=start; i<end; i++) {
    EXROM.read(i, &tmpByte);
    printByte(tmpByte, output);
    checkDigit^=tmpByte;
  }
  printByte(checkDigit, output);
  output->println("");
}

void printByte(byte value, Print* output) {
  if (value<16) {
   output->print(0, HEX);
  }
  output->print(value, HEX);
}

void initLogger() {
  // we should get the last log event written
  // setTime(lastFoundEpoch);
  // lastFoundEpoch must be smaller than 0xFFFFFFFF
  // we need to get the last position of the log in which the last epoch was found
}

void printIDs(Print* output) {
  uint32_t currentID;
  output->println("List of allowed badges / doors");
  for (int i=0; i<511; i=i+4) {
    EXROM.read(i, &currentID);
    byte door=currentID>>24;
    uint32_t id=currentID & 0x00FFFFFF;
    output->print(i/4+1);
    output->print(". ");
    output->print(door, BIN);
    output->print(" - ");
    output->println(id);
  }
}

void printLogs(Print* output) {
  output->println("List of last events");
  uint32_t epoch;
  uint32_t currentID;
  for (int i=512; i<1023; i=i+8) {
    EXROM.read(i, &epoch);
    EXROM.read(i+4, &currentID);
    byte door=currentID>>24;
    uint32_t id=currentID & 0x00FFFFFF;
    output->print(i/8-63);
    output->print(". ");
    output->print(epoch);
    output->print(" - ");
    output->print(door, BIN);
    output->print(" - ");
    output->println(id);
  }
}





