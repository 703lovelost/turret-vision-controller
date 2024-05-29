#include <FIFO.h>
#include <GStypes.h>
#include <GyverPlanner.h>
#include <GyverPlanner2.h>
#include <GyverStepper.h>
#include <GyverStepper2.h>
#include <StepperCore.h>
#include <stdint.h>
//список команд
/*
 * 1.вращение двигателя 1 и 2 в градусах
 * 2.изменение ускорения при повороте для 1 и 2 двигателей в градусах
 * 3.изменение лимита скорости поворота для 1 и 2 двигателей в градусах 
*/
//разбайтовка команд
/*
 * 1: 1byte[command number] + 1byte[motor number] + 4byte[32 bit signed int | degrees] 
 * 2: 1byte[command number] + 1byte[motor number] + 4byte[32 bit float | acceleration]
 * 3: 1byte[command number] + 1byte[motor number] + 4byte[32 bit float | max speed]
*/


#define rotate 1          //номер 1 команды
#define changeaccel 2     //номер 2 команды
#define changelimit 3     //номер 3 команды

#define motor1 1          //номер мотора в основании
#define motor2 2          //номер мотора на башне

#define steppin1 3
#define dirpin1 2
#define steppin2 6
#define dirpin2 5
#define rotsteps 800
GStepper< STEPPER2WIRE> stepper1(rotsteps,steppin1,dirpin1); 
GStepper< STEPPER2WIRE> stepper2(rotsteps,steppin2,dirpin2); 
void setup() {
  // put your setup code here, to run once:
  pinMode(steppin1,OUTPUT);
  pinMode(dirpin1,OUTPUT);
  pinMode(steppin2,OUTPUT);
  pinMode(dirpin2,OUTPUT);
  stepper1.setRunMode(FOLLOW_POS);
  stepper2.setRunMode(FOLLOW_POS);
  Serial.begin(9600);
  Serial.setTimeout(5);
  Serial.print(1);
}
float readfloat(byte *buf){
  return *((float*)buf);
}

int32_t readint(byte *buf){
  return *((int32_t*)buf);
}
void serialwait(){
  while(!Serial.available()){}    
}
byte comnum;
byte motnum;
byte buf[4];
void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    comnum = Serial.read();
    Serial.print(comnum);         //отладочный вывод номера команды 
    switch(comnum){
      case rotate:
        serialwait();
        motnum = Serial.read(); 
        Serial.print(motnum);     //отладочный вывод номера двигателя
        for(int i = 0; i < 4; i++){ serialwait(); buf[i] = Serial.read(); }
        Serial.print(readint(buf)); //отладочный вывод градусов поворота (int32)
        switch(motnum){
          case motor1:
            stepper1.setTargetDeg(readint(buf),RELATIVE);
            while(stepper1.tick()) {} //программа не продолжит выполнение до момента окончания поворота двигателя
            break;
          case motor2:
            stepper2.setTargetDeg(readint(buf),RELATIVE);
            while(stepper2.tick()) {} //программа не продолжит выполнение до момента окончания поворота двигателя
            break;
        }
        break;
      case changeaccel:
        serialwait();
        motnum = Serial.read();
        Serial.print(motnum);         //отладочный вывод номера двигателя
        for(int i = 0; i < 4; i++) {serialwait(); buf[i] = Serial.read(); }
        Serial.print(readfloat(buf)); //отладочный вывод нового ускорения в градусах (float)
        switch(motnum){
          case motor1:
            stepper1.setAccelerationDeg(readfloat(buf));
            break;
          case motor2:
            stepper2.setAccelerationDeg(readfloat(buf));
            break;
        }
        break;
      case changelimit:
        serialwait();
        motnum = Serial.read();
        Serial.print(motnum);        //отладочный вывод номера двигателя
        for(int i = 0; i < 4; i++) {serialwait(); buf[i] = Serial.read(); }
        Serial.print(readfloat(buf));//отладочный вывод нового лимита скорости в градусах (float)
        switch(motnum){
          case motor1:
            stepper1.setMaxSpeedDeg(readfloat(buf));
            break;
          case motor2:
            stepper2.setMaxSpeedDeg(readfloat(buf));
            break;
        }
        break;
    }
    Serial.print(1);
  }
}