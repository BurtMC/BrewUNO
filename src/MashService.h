#ifndef MashService_h
#define MashService_h

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP_PLATFORM)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <AsyncArduinoJson6.h>
#include <IPAddress.h>
#include <AsyncJsonWebHandler.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include <enum.h>
#include <TemperatureService.h>
#include <ActiveStatus.h>
#include <Buzzer.h>
#if defined(PUMP_BUS)
  #include <Pump.h>
#elif defined(AGITATOR_BUS)
  #include <Agitator.h>
#endif

#define MASH_SETTINGS_FILE "/config/mashSettings.json"

class MashService
{
public:
  
    MashService(FS *fs, TemperatureService *temperatureService, 
                #if defined(PUMP_BUS)
                  Pump *pump);
                #elif defined(AGITATOR_BUS)
                  Agitator *agitator);
                #endif
    ~MashService();

  void loop(ActiveStatus *activeStatus);
  void LoadMashSettings();

private:
  FS *_fs;
  TemperatureService *_temperatureService;
  #if defined(PUMP_BUS)
    Pump *_pump;
  #elif defined(AGITATOR_BUS)
    Agitator *_agitator;
  #endif
  boolean checkTemperaturePID(ActiveStatus *activeStatus);
  void StepStarted(ActiveStatus *activeStatus, JsonArray steps, time_t timeNow);
  void BoilTime(ActiveStatus *activeStatus);
  void NextStep(ActiveStatus *activeStatus, JsonArray steps, time_t timeNow, int nextMashStep);
  String getMashName(ActiveStatus *activeStatus, JsonObject step);
};
#endif