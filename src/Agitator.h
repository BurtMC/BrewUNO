#ifndef Agitator_h
#define Agitator_h

#include <Arduino.h>
#include <TimeLib.h>
#include <BrewSettingsService.h>
#include <ActiveStatus.h>
#include <Buzzer.h>

#define APPLICATION_JSON_TYPE "application/json"
#define START_AGITATOR_SERVIVE_PATH "/rest/startagitator"
#define STOP_AGITATOR_SERVIVE_PATH "/rest/stopagitator"

class Agitator
{
public:
  Agitator(AsyncWebServer *server, ActiveStatus *activeStatus, BrewSettingsService *brewSettingsService);

  AsyncWebServer *_server;
  BrewSettingsService *_brewSettingsService;
  ActiveStatus *_activeStatus;

  void TurnAgitatorOn();
  void TurnAgitatorOff();
  void CheckRest();
  void antiCavitation();
  void startAgitatorHttpService(AsyncWebServerRequest *request);
  void stopAgitatorHttpService(AsyncWebServerRequest *request);
  
  void TurnAgitator(bool on);
};

#endif
