#include "MyIoT.h"
#include <EEPROM.h>

void readObj(myIoT *obj) {
  EEPROM.begin(sizeof(myIoT));
  EEPROM.get(0, *obj);
  EEPROM.end();
}

void writeObj(myIoT *obj) {
  EEPROM.begin(sizeof(myIoT));
  EEPROM.put(0, *obj);
  EEPROM.commit();
  EEPROM.end();
}

void chkUserData(myIoT *obj, myIoT defaultObj) {
  readObj(obj);
  if (obj->tag != 0) {
    writeObj(&defaultObj);
    readObj(obj);
  }
}