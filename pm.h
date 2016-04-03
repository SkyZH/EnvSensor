char pm_checkValue(char *buffer, char len);
int pm_transmitPM01(char *buffer);
int pm_transmitPM25(char *buffer);
int pm_transmitPM10(char *buffer);
void pm_initialize(struct pm_storage* storage, HardwareSerial* serial);
void pm_refresh(struct pm_storage* storage);

#define PM_BUFFER 32

struct pm_storage {
  int pm01, pm25, pm10;
  HardwareSerial* serial;
  char buffer[PM_BUFFER];
} pm_storage;
