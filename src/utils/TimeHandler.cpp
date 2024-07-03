#include "TimeHandler.hpp"

extern struct tm timeinfo;

void updateTime() {
  configTime(0, 0, "pool.ntp.org");
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org"); // Setting Danish timezone

  Serial.print(F("Waiting for NTP time sync"));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }
  //gmtime_r(&nowSecs, &timeinfo);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("\nNTP sync done");
}

int getCurrentHour() {
  return timeinfo.tm_hour;
}
