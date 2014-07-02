#include <EXROM.h>

/*
EEPROM is devided in 2 parts
 
 0->511 : allowed IDs
 
 8 bits for allowed door
 24 bits for the ID
 
 512 -> 1023 : logs
 
 
 */

void initLogger() {


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




