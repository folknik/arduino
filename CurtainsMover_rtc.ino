#include <Wire.h>
#include <DS3231.h>
#include <AccelStepper.h>
#include "GyverWDT.h"
#include <avr/sleep.h>


// инициализация переменных для RTC clock
DS3231 clock;
RTCDateTime dt;
int seconds;
int minutes;
int hours;

// Определение пинов для управления двигателем мотора
#define Pin1  12 // IN1
#define Pin2  11 // IN2
#define Pin3  10 // IN3
#define Pin4  9 // IN4

// Инициализируемся с последовательностью выводов IN1-IN3-IN2-IN4 для использования AccelStepper с 28BYJ-48
AccelStepper motor(4, Pin1, Pin3, Pin2, Pin4);

int type = 0; // опускаем или поднимаем шторы, если заливаем скетч днем, то ставим 1
int flagEnd = 0; // флаг показывающий подняты или опущены до конца шторы
int distance = 30000; // на сколько закручиваем шторы
int balance; // остаток непройденного расстояния

/* интервал минут и секунд ~5мин*/
int minGap = 4;
int secGap = 59;

/* время утренней размотки штор */
int morning_hour = 11;
int morning_minute = 50;

/* время вечерней смотки штор */
int evening_hour = 12;
int evening_minute = 05;

// инициализация всего оборудования
void setup()
{
    // Инициализация DS3231
    clock.begin();
    // время компиляции скетча как время отсчета
    clock.setDateTime(__DATE__, __TIME__);
    
    // инициализация шагового мотора
    motor.setMaxSpeed(500.0);
    motor.setAcceleration(100.0);
    motor.setSpeed(100);
    pinMode (12, OUTPUT);
    pinMode (11, OUTPUT);
    pinMode (10, OUTPUT);
    pinMode (9, OUTPUT);
    // balance = distance;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Выбираем нужный режим сна
}

// функция для проверки, попадает ли время в утренний интервал
bool CheckMorningTime(int h_, int m_, int s_)
{
    if (h_ == morning_hour and m_ - morning_minute >= 0 and m_ - morning_minute <= minGap  and s_ <= secGap)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// функция для проверки, попадает ли время в вечерний интервал
bool CheckEveningTime(int h_, int m_, int s_)
{
    if (h_ == evening_hour and m_ - evening_minute >= 0 and m_ - evening_minute <= minGap  and s_ <= secGap)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// функция, которая вращает мотор
int MotorMover(int h, int m, int s)
{
    // проверяем утренний и вечерний промежутки времени
    bool resMorning = false;
    bool resEvening = false;
    resMorning = CheckMorningTime(h, m, s);
    resEvening = CheckEveningTime(h, m, s);
    
    // режим размотки шторы при наступлении рассвета
    if (resMorning and type == 0 and flagEnd == 0)
    {
        motor.moveTo(distance);
        type = 1;
        // startFlag = 0;
    }

    // режим смотки шторы при наступлении заката
    if (resEvening and type == 1 and flagEnd == 0)
    {
        motor.moveTo(-distance);
        type = 0;
        // startFlag = 0;
    }

    // если движение началось, но еще не закончилось, то меняем значение флага, чтобы режимы не повторялись
    if (motor.distanceToGo() != 0)
    {
        flagEnd = 1;
    }
    else
    {
        // согнализируем о том, что мотор докрутил шторы
        flagEnd = 0;
        // чтобы мотор не грелся в режиме ожидания, подаем на все обмотки нулевой сигнал
        digitalWrite (12, LOW);
        digitalWrite (11, LOW);
        digitalWrite (10, LOW);
        digitalWrite (9, LOW);
    }

    // запускаем очередной шаг мотора
    motor.run();
    return motor.distanceToGo();
}

void loop()
{
    // получаем текущее время
    dt = clock.getDateTime();
    seconds = dt.second;
    minutes = dt.minute;
    hours = dt.hour;

    // передаем время в MotorMover
    balance = MotorMover(hours, minutes, seconds);

    /* если distanceToGо = 0, то можем поспать */
    if (balance == 0)
    {
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~8c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~16c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~24c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~32c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~40c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~48c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024); // Режим прерываний, +8с, таймаут ~56c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
        Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_512);  // Режим прерываний, +4с, таймаут ~60c
        sleep_enable();                                      // Разрешаем сон
        sleep_cpu();                                         // Уходим в сон
    }
}


/* Прерывание watchdog , в нем мы просыпаемся */
ISR(WATCHDOG)
{
    sleep_disable();     // Запрещаем сон
    Watchdog.disable();  // Выключаем watchdog
}
