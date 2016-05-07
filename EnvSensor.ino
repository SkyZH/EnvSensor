#include <DHT.h>

#include "pm.h"
#include "data.h"

#define SERIAL_RX_BUFFER_SIZE 256
#define SERIAL_TX_BUFFER_SIZE 128

DHT dht(DHTPIN, DHTTYPE);

struct pm_storage pmStorage;

#define POWER_PIN_1 8
#define POWER_PIN_2 9

void setup()
{
  Serial.begin(9600);
  pm_initialize(&pmStorage, &Serial3);
  dht.begin();
  pinMode(POWER_PIN_1, OUTPUT);
  pinMode(POWER_PIN_2, OUTPUT);
  digitalWrite(POWER_PIN_1, HIGH);
  digitalWrite(POWER_PIN_2, HIGH);
}

int getDHTData(struct DHTData *data) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    return 0;
  }
  float hif = dht.computeHeatIndex(f, h);

  float hic = dht.computeHeatIndex(t, h, false);

  data->h = h; data->t = t; data->f = f; data->hif = hif; data->hic = hic;
  return 1;
}

int getPMData(struct PMData *data) {
  if (pmStorage.pm01 == -1 || pmStorage.pm25 == -1 || pmStorage.pm10 == -1) {
    return 0;
  }
  data->pm01 = pmStorage.pm01;
  data->pm25 = pmStorage.pm25;
  data->pm10 = pmStorage.pm10;
  pmStorage.pm01 = pmStorage.pm25 = pmStorage.pm10 = -1;
  return 1;
}

int readCommand(HardwareSerial *serial) {
  if (serial->available()) {
    return serial->read();
  }
  return 0;
}

void getHeader(struct Header* header, CMD cmd, SIZE sz) {
  header->cmd = cmd;
  header->sz = sz;
}

int writeCommand(HardwareSerial *serial, CMD cmd, SIZE sz, char* data) {
  union HeaderPack headerPack;
  getHeader(&headerPack.data, cmd, sz);

  serial->write(headerPack.pack, PACK_HEADER_LENGTH);
  serial->write(data, sz);
  serial->flush();
}

void serialEvent3() {
  while(!pm_refresh(&pmStorage));
}

union DHTPack dhtPack;
union PMPack pmPack;
struct RawPack rawPack;

void serialEvent() {
  CMD cmd = 0;
  while (cmd = readCommand(&Serial)) {
    if (cmd == DATA_CMD_TEMPERATURE) {
      if (getDHTData(&dhtPack.data)) {
        writeCommand(&Serial, cmd, PACK_DHT_LENGTH, ((char*)dhtPack.pack));
      } else {
        writeCommand(&Serial, DATA_CMD_FAILED, PACK_RAW_LENGTH, ((char*)rawPack.pack));
      }
    } else if (cmd == DATA_CMD_PM) {
      if (getPMData(&pmPack.data)) {
        writeCommand(&Serial, cmd, PACK_PM_LENGTH, ((char*)pmPack.pack));
      } else {
        writeCommand(&Serial, DATA_CMD_FAILED, PACK_RAW_LENGTH, ((char*)rawPack.pack));
      }
    } else if (cmd == DATA_CMD_DEBUG) {
      writeCommand(&Serial, DATA_CMD_DEBUG, PACK_RAW_LENGTH, ((char*)rawPack.pack));
    } else if (cmd == DATA_CMD_CLEAR) {
      writeCommand(&Serial, DATA_CMD_CLEAR, PACK_RAW_LENGTH, ((char*)rawPack.pack));
    }
  }
}

void loop() {
}

