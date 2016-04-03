#ifndef ENVSENSOR_DATA_H
#define ENVSENSOR_DATA_H

#define DHTTYPE DHT22
#define DHTPIN 7

#define DATA_CMD_TEMPERATURE 97
#define DATA_CMD_PM 65
#define DATA_CMD_DEBUG 66
#define DATA_CMD_FAILED 98

#define PACK_HEADER_LENGTH 2
#define PACK_DHT_LENGTH 20
#define PACK_RAW_LENGTH 20
#define PACK_PM_LENGTH 12

typedef unsigned char CMD;
typedef unsigned char SIZE;

struct Header {
  CMD cmd;
  SIZE sz;
} Header;

union HeaderPack {
  unsigned char pack[PACK_HEADER_LENGTH];
  struct Header data;
} HeaderPack;

struct DHTData {
  float h, t, f, hif, hic;
} DHTData;

union DHTPack {
  unsigned char pack[PACK_DHT_LENGTH];
  struct DHTData data;
} DHTPack;

struct PMData {
  int pm01, pm25, pm10;
} PMData;

union PMPack {
  unsigned char pack[PACK_PM_LENGTH];
  struct PMData data;
} PMPack;

struct RawPack {
  unsigned char pack[PACK_RAW_LENGTH];
} RawPack;

#endif
