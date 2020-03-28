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
    Serial.begin(9600);

    // Инициализация DS3231
    clock.begin();

    // устанавливаем в качестве текущее время время компиляции скетча
    clock.setDateTime(__DATE__, __TIME__);
}

void loop()
{
    dt = clock.getDateTime();

    seconds = dt.second;
    minutes = dt.minute;
    hours = dt.hour;

    // выводим время
    Serial.print("Time: ");
    Serial.print(hours); Serial.print(":");
    Serial.print(minutes); Serial.print(":");
    Serial.print(seconds); Serial.println("");

    // проверяем будильник, время срабатывания - 8:00:00
    if (hours == alarmHour && minutes == alarmMinute && seconds == alarmSecond)
    {
        Serial.println("Alarm is armed");
    }
    delay(1000);
}