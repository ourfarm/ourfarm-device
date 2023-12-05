#ifndef MYIOT_H
#define MYIOT_H

typedef struct {
  char tag;
  char ssid[20];
  char pass[20];
  char sw1;
} myIoT;

void readObj(myIoT *obj);
void writeObj(myIoT *obj);
// defaultObj & 다시 보기
void chkUserData(myIoT *obj, myIoT defaultObj);

#endif // MYIOT_H