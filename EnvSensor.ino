#include <DHT.h>

#include "pm.h"
#include "data.h"


DHT dht(DHTPIN, DHTTYPE);

struct pm_storage pmStorage;

void setup()
{
  Serial.begin(115200);
  pm_initialize(&pmStorage, &Serial3);
  dht.begin();
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
  return 1;
}

int readCommand(HardwareSerial *serial) {
  if (serial->available() > 0) {
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
}

void loop() {
  union DHTPack dhtPack;
  union PMPack pmPack;
  struct RawPack rawPack;

  CMD cmd = 0;

  while (1) {
    while (!(cmd = readCommand(&Serial))) {
      pm_refresh(&pmStorage);
    }

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
    }
  }
}

