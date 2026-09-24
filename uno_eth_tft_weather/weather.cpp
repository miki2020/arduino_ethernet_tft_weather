#include "http_stream.h"
#include "weather.h"

static const char HOST[]      PROGMEM = "api.open-meteo.com";

// keys, in the order they appear in the "current" block
static const char K_CURRENT[] PROGMEM = "\"current\":";
static const char K_TEMP[]    PROGMEM = "\"temperature_2m\":";
static const char K_HUM[]     PROGMEM = "\"relative_humidity_2m\":";
static const char K_DAY[]     PROGMEM = "\"is_day\":";
static const char K_RAIN[]    PROGMEM = "\"precipitation\":";
static const char K_CLOUD[]   PROGMEM = "\"cloud_cover\":";
static const char K_PRESS[]   PROGMEM = "\"surface_pressure\":";
static const char K_WIND[]    PROGMEM = "\"wind_speed_10m\":";
static const char K_WDIR[]    PROGMEM = "\"wind_direction_10m\":";

bool weatherFetch(const Location &l, Weather &w) {

  if (!httpOpen(HOST)) return false;

  httpSendP(PSTR("GET /v1/forecast?latitude="));
  httpSend(l.lat);
  httpSendP(PSTR("&longitude="));
  httpSend(l.lon);
  httpSendP(PSTR("&current=temperature_2m,relative_humidity_2m,is_day,precipitation,"
                 "cloud_cover,surface_pressure,wind_speed_10m,wind_direction_10m"));
  httpEnd(HOST);

  // skip "current_units", then read the values in the order they arrive
  bool ok = streamSeek(K_CURRENT)
         && streamNumber(K_TEMP,  w.temp,  sizeof(w.temp))
         && streamNumber(K_HUM,   w.hum,   sizeof(w.hum))
         && streamNumber(K_DAY,   w.isday, sizeof(w.isday))
         && streamNumber(K_RAIN,  w.rain,  sizeof(w.rain))
         && streamNumber(K_CLOUD, w.cloud, sizeof(w.cloud))
         && streamNumber(K_PRESS, w.press, sizeof(w.press))
         && streamNumber(K_WIND,  w.wind,  sizeof(w.wind))
         && streamNumber(K_WDIR,  w.wdir,  sizeof(w.wdir));

  httpClose();
  return ok;
}
