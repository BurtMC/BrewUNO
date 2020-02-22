#ifndef TemperatureService_RTU_h
#define TemperatureService_RTU_h

#include <Adafruit_MAX31865.h>
#include <TimeLib.h>
#include <ESPAsyncWebServer.h>
#include <ActiveStatus.h>
#include <BrewSettingsService.h>
#include <enum.h>

#define APPLICATION_JSON_TYPE "application/json"
#define GET_SENSORS_SERVICE_PATH "/rest/getsensors"

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

class TemperatureService
{
public:
  TemperatureService(AsyncWebServer *server, FS *fs, Adafruit_MAX31865 *MAX31865, BrewSettingsService *brewSettingsService);
  ~TemperatureService();

  BrewSettingsService *_brewSettingsService;

  Temperatures GetTemperatures(int NameCounter);

  void GetTemperatureAndAdress(AsyncWebServerRequest *request);
  String Faults(Adafruit_MAX31865 *MAX31865);
  String GetSensorsJson();
  void Loop_Temp(Adafruit_MAX31865 *MAX31865);
  void InitTemps(Adafruit_MAX31865 *MAX31865);
  int DeviceCount;

private:
  Adafruit_MAX31865 *_MAX31865;
  AsyncWebServer *_server;
  FS *_fs;
  float getTemperature(int NameCounter);
};
#endif