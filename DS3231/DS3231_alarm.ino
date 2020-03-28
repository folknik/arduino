#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

int seconds;
int minutes;
int hours;

// определяем время срабатывания будильника
int alarmSecond = 0;
int alarmMinute = 0;
int alarmHour = 8;

void setup()
{
    // Инициализация DS3231
    clock.begin();

    // устанавливаем в качестве текущего время время компиляции скетча
    clock.setDateTime(__DATE__, __TIME__);
}

void loop()
{
    // получаем текущее время
    dt = clock.getDateTime();
    seconds = dt.second;
    minutes = dt.minute;
    hours = dt.hour;

    // проверяем будильник, время срабатывания - 8:00:00
    if (hours == alarmHour && minutes == alarmMinute && seconds == alarmSecond)
    {
        // TO DO
    }
    delay(1000);
}