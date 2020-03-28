/*
Стекч для автоматизации работы домашних рулонных штор. Сделано с применением шагового двигателя 28BYJ-48 и платы контроля ULN2003.
В качестве платы контроллера использовалась Wemos D1 R2.
*/

#include <AccelStepper.h>

// Определение пинов для управления двигателем
#define Pin1  D2 // IN1
#define Pin2  D3 // IN2
#define Pin3  D4 // IN3
#define Pin4  D5 // IN4

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

    pinMode (D2, OUTPUT);
    pinMode (D3, OUTPUT);
    pinMode (D4, OUTPUT);
    pinMode (D5, OUTPUT);

    balance = distance;
}

int MoveMotor(int val)
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
          digitalWrite (D2, LOW);
          digitalWrite (D3, LOW);
          digitalWrite (D4, LOW);
          digitalWrite (D5, LOW);
      }

      // запускаем очередной шаг мотора
      motor.run();
      return motor.distanceToGo();
}

void loop()
{
    // считываем показания модуля фотодетектора и передаем их в функция, отвечающую за движение штор
    xA0 = analogRead(A0);
    balance = MoveMotor(xA0);
    if (balance == 0 and startFlag == 0)
    {
        delay(waitingTime);
    }
}