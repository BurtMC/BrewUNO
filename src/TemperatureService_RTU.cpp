#include <TemperatureService_RTU.h>

TemperatureService::TemperatureService(AsyncWebServer *server, FS *fs, Adafruit_MAX31865 *MAX31865, BrewSettingsService *brewSettingsService) : _server(server),
                                                                                                                                                        _fs(fs),
                                                                                                                                                        _MAX31865(MAX31865),
                                                                                                                                                        _brewSettingsService(brewSettingsService)
{
    _server->on(GET_SENSORS_SERVICE_PATH, HTTP_GET, std::bind(&TemperatureService::GetTemperatureAndAdress, this, std::placeholders::_1));
}

TemperatureService::~TemperatureService() {}

void TemperatureService::GetTemperatureAndAdress(AsyncWebServerRequest *request)
{
    request->send(200, APPLICATION_JSON_TYPE, GetSensorsJson());
}

String json = "";

String TemperatureService::GetSensorsJson()
{
    if (json == "")
        GetTemperatures(cs);
    return json;
}

Temperatures TemperatureService::GetTemperatures(int NameCounter)
{
    Temperatures temps;
    temps.Main = 0;
    temps.Sparge = 0;
    //uint16_t rtd = _MAX31865[i].readRTD();
    String _json = "{ \"sensors\": [ ";
    String addr = "";
    for (int i = 0; i < DeviceCount; i++)
    {
        float temp = _brewSettingsService->TempUnit == "C" ? _MAX31865[i].temperature(100, RREF) : _MAX31865[i].temperature(100, RREF)*9/5+32;
        _json += "{ \"address\": \"" + NameCounter + "\",\"value\": \"" + String(temp) + "\"}";
        if (i < DeviceCount - 1)
            _json += ',';
        addr = NameCounter;
        if (addr == _brewSettingsService->MainSensor)
            temps.Main = temp + _brewSettingsService->MainSensorOffset;
        if (addr == _brewSettingsService->SpargeSensor)
            temps.Sparge = temp + _brewSettingsService->SpargeSensorOffset;
        if (addr == _brewSettingsService->BoilSensor)
            temps.Boil = temp + _brewSettingsService->BoilSensorOffset;
        if (addr == _brewSettingsService->AuxOneSensor)
            temps.AuxOne = temp + _brewSettingsService->AuxSensorOneOffset;
        if (addr == _brewSettingsService->AuxTwoSensor)
            temps.AuxTwo = temp + _brewSettingsService->AuxSensorTwoOffset;
        if (addr == _brewSettingsService->AuxThreeSensor)
            temps.AuxThree = temp + _brewSettingsService->AuxSensorThreeOffset;
    }
    json = _json + "]}";
    return temps;
}

String TemperatureService::Faults(Adafruit_MAX31865 *MAX31865)
{
    // Check and print any faults
    uint8_t fault = _MAX31865[i].readFault();
    String str = "";
    if (fault) 
    {
        Serial.print("Fault 0x"); Serial.println(fault, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH) {
            str += "RTD High Threshold"; 
        }
        if (fault & MAX31865_FAULT_LOWTHRESH) {
            str += "RTD Low Threshold"; 
        }
        if (fault & MAX31865_FAULT_REFINLOW) {
            str += "REFIN- > 0.85 x Bias"; 
        }
        if (fault & MAX31865_FAULT_REFINHIGH) {
            str += "REFIN- < 0.85 x Bias - FORCE- open"; 
        }
        if (fault & MAX31865_FAULT_RTDINLOW) {
            str += "RTDIN- < 0.85 x Bias - FORCE- open"; 
        }
        if (fault & MAX31865_FAULT_OVUV) {
            str += "Under/Over voltage"; 
        }
        _MAX31865[i].clearFault();
    }
     return str;
}

void TemperatureService::Loop_Temp(Adafruit_MAX31865 *MAX31865)
{
    int cs;
    for (int i=1;i<=DeviceCount;i++)
    {
        switch (i)
        {
        case 1:
            cs= TEMPERATURE_MASH;
            break;
        case 2:
            cs= TEMPERATURE_BOIL;
            break;
        case 3:
            cs= TEMPERATURE_BOIL;
            break;
        default:
            break;
        }
        GetTemperatures(i);
    }
}

void TemperatureService::InitTemps(Adafruit_MAX31865 *MAX31865)
{
    for (int i=1;i<=DeviceCount;i++)
    {
        _MAX31865[i].begin(MAX31865_3WIRE);
    }
}