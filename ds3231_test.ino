// Автор – Джон Боксэлл (John Boxall), from http://tronixstuff.com

#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

// конвертируем обычные десятичные значения в двоично-десятичный код:
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}

// конвертируем двоично-десятичный код в обычные десятичные значения: 
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

void setup(){
  Wire.begin();
  Serial.begin(9600);
  // задаем текущее время;
  // секунды, минуты, часы, день недели, дата, месяц, год:
  setDS3231time(0, 30, 23, 6, 27, 3, 20);
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
  // передаем данные о времени и дате на DS3231:
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  // выставляем регистр указателей DS3231 на регистр «00h»,
  // т.е. на регистр, в котором хранятся данные о секундах:
  Wire.write(0);
  Wire.write(decToBcd(second)); // задаем секунды
  Wire.write(decToBcd(minute)); // задаем минуты
  Wire.write(decToBcd(hour)); // задаем часы
  Wire.write(decToBcd(dayOfWeek));
  // задаем день недели (1 – это воскресенье, 7 – это суббота)
  Wire.write(decToBcd(dayOfMonth)); // задаем дату (от 1 до 31)
  Wire.write(decToBcd(month)); // задаем месяц
  Wire.write(decToBcd(year)); // задаем год (от 0 до 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  // выставляем регистр указателей DS3231 на регистр «00h»:
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // запрашиваем у DS3231 семь байтов начиная с регистра «00h»:
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime(){
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // извлекаем данные из DS3231:
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // отправляем их на монитор порта:
  Serial.print(hour, DEC);
  // при показе конвертируем байтовую переменную в десятичное число:
  Serial.print(":");
  if (minute<10){
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10){
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");  //  " День недели: "
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");  //  "Воскресенье"
    break;
  case 2:
    Serial.println("Monday");  //  "Понедельник"
    break;
  case 3:
    Serial.println("Tuesday");  //  "Вторник"
    break;
  case 4:
    Serial.println("Wednesday");  //  "Среда"
    break;
  case 5:
    Serial.println("Thursday");  //  "Четверг"
    break;
  case 6:
    Serial.println("Friday");  //  "Пятница"
    break;
  case 7:
    Serial.println("Saturday");  //  "Суббота"
    break;
  }
}
void loop(){
  displayTime();
 // показываем на мониторе порта текущие данные RTC-модуля  
  delay(1000); // секундная задержка
}
