#include "pm.h"

int pm_transmitPM01(char *buffer){
    return ((((int)buffer[4]) << 8) + (int)buffer[5]);
}

int pm_transmitPM25(char *buffer) {
    return ((((int)buffer[6]) << 8) + (int)buffer[7]);
}

int pm_transmitPM10(char *buffer) {
    return ((((int)buffer[8]) << 8) + (int)buffer[9]);
}

char pm_checkValue(char *buffer, char len) {
    int recvSum = 0;
    char i = 0;
    for(i = 0; i < len - 2; i++) {
        recvSum = recvSum + (int)buffer[i];
    }
    if(recvSum == ((((int)buffer[len - 2]) << 8) + (int)buffer[len - 1])) {
        return 1;
    }
    return 0;
}

void pm_initialize(struct pm_storage* storage, HardwareSerial* serial) {
    serial->begin(PM_SERIAL_BAUDRATE);
    storage->serial = serial;
    storage->pm01 = storage->pm25 = storage->pm10 = -1;
}
void pm_refresh(struct pm_storage* storage) {
    if(storage->serial->available()) {
      storage->serial->readBytes(storage->buffer, PM_BUFFER);
      if(storage->buffer[0] == 0x42 && storage->buffer[1] == 0x4d) {
        
        if(pm_checkValue(storage->buffer, PM_BUFFER)) {
          storage->pm01 = pm_transmitPM01(storage->buffer);
          storage->pm25 = pm_transmitPM25(storage->buffer);
          storage->pm10 = pm_transmitPM10(storage->buffer);
        }
      }
    }    
}
