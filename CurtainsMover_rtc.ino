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
#define Pin4  9  // IN4

// Инициализируемся с последовательностью выводов IN1-IN3-IN2-IN4 для использования AccelStepper с 28BYJ-48
AccelStepper motor(4, Pin1, Pin3, Pin2, Pin4);

int type = 0; // опускаем или поднимаем шторы
int flagEnd = 0; // флаг показывающий подняты или опущены до конца шторы
int distance = 30000; // на сколько закручиваем шторы
int balance; // остаток непройденного расстояния

/* время утренней размотки штор */
int morning_hour = 6;
int morning_minute = 0;

/* время вечерней смотки штор */
int evening_hour = 11;
int evening_minute = 0;

int morningSeconds = morning_minute * 60 + morning_hour * 3600;
int eveningSeconds = evening_minute * 60 + evening_hour * 3600;
int currentSeconds = 0;

// инициализация всего оборудования
void setup()
{
    // Инициализация DS3231
    clock.begin();

    // время компиляции скетча как время отсчета
    clock.setDateTime(__DATE__, __TIME__);

    // получаем текущее время
    dt = clock.getDateTime();
    seconds = dt.second;
    minutes = dt.minute;
    hours = dt.hour;

    // инициализация шагового мотора
    motor.setMaxSpeed(500.0); // устанавливаем максимальную скорость вращения ротора двигателя (шагов/секунду)
    motor.setAcceleration(100.0); // устанавливаем ускорение (шагов/секунду^2)
    motor.setSpeed(100);
    pinMode (12, OUTPUT);
    pinMode (11, OUTPUT);
    pinMode (10, OUTPUT);
    pinMode (9, OUTPUT);

    motor.setCurrentPosition(0); // устанавливаем мотор шагового двигателя в нулевое положение

    // Выбираем нужный режим сна
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

// количество секунды с полуночи
int getSeconds(int new_h, int new_m, int new_s)
{
    return new_s + new_m * 60 + new_h * 3600;
}

// функция для проверки, попадает ли время в утренний интервал
bool isMorningTime(int h_, int m_, int s_)
{
    currentSeconds = getSeconds(h_, m_, s_);
    if (currentSeconds >= morningSeconds and currentSeconds <= eveningSeconds)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// функция, которая отвечает за движение мотора
int MotorMover(int h, int m, int s)
{
    /*
        Если текущая время (количество секунд с полуночи) попадает в промежуток между утренним и
        вечерним промежутками, то происходит опускание шторы. Если наступил вечер, то  происходит смотка шторы
        00:00 ---------- morning_time ------------- evening_time ---------- 23:59
    */

    // проверяем утренний и вечерний промежутки времени
    bool status = false;
    status = isMorningTime(h, m, s);
    
    // режим размотки шторы при наступлении рассвета
    if (status == true and type == 0 and flagEnd == 0)
    {
        motor.moveTo(distance);
        type = 1;
    }

    // режим смотки шторы при наступлении заката
    if (status == false and type == 1 and flagEnd == 0)
    {
        motor.moveTo(-distance);
        type = 0;
    }

    // Как только движение началось, меняем значение флага. Если движение закончилось или двигатель простаивает, то меняем значение флага на ноль
    if (motor.distanceToGo() != 0)
    {
        flagEnd = 1;
    }
    else
    {
        flagEnd = 0; // мотор докрутил штору

        motor.setCurrentPosition(0); // устанавливаем мотор шагового двигателя в нулевое положение

        // чтобы мотор не грелся в режиме ожидания снимаем напряжение с обмоток шагового двигателя
        digitalWrite (12, LOW);
        digitalWrite (11, LOW);
        digitalWrite (10, LOW);
        digitalWrite (9, LOW);
    }

    // запускаем очередной шаг мотора, ускорение определено выше
    motor.run();
    return motor.distanceToGo(); // возвращаем количество оставшихся шагов шагового двигателя
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