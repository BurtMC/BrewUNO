#include <Agitator.h>

time_t lastAgitatorStarted;
time_t lastAgitatorRest;

bool isResting = false;
bool rotationOn = false;

Agitator::Agitator(AsyncWebServer *server, ActiveStatus *activeStatus, BrewSettingsService *brewSettingsService) : _server(server),
                                                                                                           _activeStatus(activeStatus),
                                                                                                           _brewSettingsService(brewSettingsService)

{
    _server->on(START_AGITATOR_SERVIVE_PATH, HTTP_POST, std::bind(&Agitator::startAgitatorHttpService, this, std::placeholders::_1));
    _server->on(STOP_AGITATOR_SERVIVE_PATH, HTTP_POST, std::bind(&Agitator::stopAgitatorHttpService, this, std::placeholders::_1));
}

void Agitator::startAgitatorHttpService(AsyncWebServerRequest *request)
{
    TurnAgitatorOn();
    request->send(200, APPLICATION_JSON_TYPE, _activeStatus->GetJson());
}

void Agitator::stopAgitatorHttpService(AsyncWebServerRequest *request)
{
    TurnAgitatorOff();
    request->send(200, APPLICATION_JSON_TYPE, _activeStatus->GetJson());
}

void Agitator::TurnAgitatorOn()
{
    TurnAgitator(true);
    rotationOn = true;
    lastAgitatorStarted = now();
    lastAgitatorRest = 0;
    CheckRest();
}

void Agitator::TurnAgitatorOff()
{
    TurnAgitator(false);
    rotationOn = false;
    lastAgitatorStarted = 0;
    lastAgitatorRest = 0;
    CheckRest();
}

void Agitator::TurnAgitator(bool on)
{
    on = INVERT_AGITATOR ? !on : on;
    digitalWrite(AGITATOR_BUS, on ? HIGH : LOW);
    _activeStatus->AgitatorOn = INVERT_AGITATOR ? !on : on;
}

void Agitator::CheckRest()
{
    if (rotationOn && !_activeStatus->PIDActing)
    {
        time_t timeNow = now();
        if (!isResting && timeNow - lastAgitatorStarted >= _brewSettingsService->AgitatorRestInterval)
        {
            Serial.println("Agitator Rest time to sleep!");
            TurnAgitator(false);
            isResting = true;
            lastAgitatorStarted = 0;
            lastAgitatorRest = timeNow;
            Buzzer().Ring(1, 50);
        }

        if (isResting && timeNow - lastAgitatorRest >= _brewSettingsService->AgitatorRestTime)
        {
            Serial.println("Agitator Rest wake up!");
            TurnAgitator(true);
            isResting = false;
            lastAgitatorRest = 0;
            lastAgitatorStarted = timeNow;
            Buzzer().Ring(1, 50);
        }
        _activeStatus->AgitatorIsResting = isResting;
    }
    else
        _activeStatus->AgitatorIsResting = false;
}

void Agitator::antiCavitation()
{
    if (_activeStatus->ActiveStep == anticavitation)
    {
        for (byte i = 1; i < 6; i++)
        {
            TurnAgitator(true);
            delay(1500 + i * 250);
            TurnAgitator(false);
            delay(2000);
        }
        _activeStatus->ActiveStep = _activeStatus->LastActiveStep;
    }
}