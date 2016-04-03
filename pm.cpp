#include "pm.h"

int pm_transmitPM01(char *buffer){
    return ((buffer[4] << 8) + buffer[5]);
}

int pm_transmitPM25(char *buffer) {
    return ((buffer[6] << 8) + buffer[7]);
}

int pm_transmitPM10(char *buffer) {
    return ((buffer[8] << 8) + buffer[9]);
}

char pm_checkValue(char *buffer, char len) {
    int recvSum = 0;
    char i = 0;
    for(i = 0; i < len; i++) {
        recvSum = recvSum + buffer[i];
    }
    if(recvSum == ((buffer[len-2] << 8) + buffer[len - 1] + buffer[len - 2] + buffer[len - 1])) {
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
          Serial.println(storage->pm25);
        }
      }
      Serial.println("Refreshing");
    }    
}
