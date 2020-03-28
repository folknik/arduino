/*
Стекч для автоматизации работы домашних рулонных штор
Использован шаговый двигатель 28BYJ-48 и платы контроля ULN2003
В качестве платы контроллера используется Arduino Nano
Для сворачивания штор по времени используются часы реального времени DS3231
*/

#include <AccelStepper.h>

// Определение пинов для управления двигателем
#define Pin1  12 // IN1
#define Pin2  11 // IN2
#define Pin3  10 // IN3
#define Pin4  9 // IN4

// Инициализируемся с последовательностью выводов IN1-IN3-IN2-IN4 для использования AccelStepper с 28BYJ-48
AccelStepper motor(4, Pin1, Pin3, Pin2, Pin4);

int xA0 = 0;
int type = 0; // тип операции - опускаем или поднимаем шторы
int flagEnd = 0; // флаг показывающий подняты или опущены до конца шторы
int distance = 15000; // на сколько закручиваем шторы
int morningLevel = 300;
int nightLevel = 600;
int balance; // остаток непройденного расстояния
int waitingTime = 60000; // время ожидания до следующего измерения фотодетектора
int startFlag = 1;

void setup()
{
    motor.setMaxSpeed(500.0);
    motor.setAcceleration(100.0);
    motor.setSpeed(100);

    pinMode (12, OUTPUT);
    pinMode (11, OUTPUT);
    pinMode (10, OUTPUT);
    pinMode (9, OUTPUT);

    balance = distance;
}

int MotorMover(int val)
{
      // режим размотки шторы при наступлении рассвета
      if (val < morningLevel and type == 0 and flagEnd == 0)
      {
          motor.moveTo(distance);
          type = 1;
          startFlag = 0;
      }

      // режим смотки шторы при наступлении заката
      if (val > nightLevel and type == 1 and flagEnd == 0)
      {
          motor.moveTo(-distance);
          type = 0;
          startFlag = 0;
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
    // считываем показания модуля фотодетектора и передаем их в функция, отвечающую за движение штор
    xA0 = 290;
    balance = MotorMover(xA0);
    if (balance == 0 and startFlag == 0)
    {
        delay(waitingTime);
    }
}