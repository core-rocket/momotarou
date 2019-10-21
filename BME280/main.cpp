/*
BME280から気圧・温度を取得してシリアルで表示する．
<BME280ピン設定>
CSB : HIGH
SDO : LOW
*/
#include "mbed.h"
#include "BME280.h"

DigitalOut myled(PB_1);
Serial pc(PA_9, PA_10, 115200); //pin 19, 20

float pre, tem;

BME280 bme = BME280(PB_7, PB_6); //pin 30, 29

int main(){
    wait(1.0); //気持ち
    bme.initialize();
    pc.printf("Start.\n\r");
    myled = 1;
    while(1){
        pc.printf("Pre: %f\n\r", bme.getPressure());
        pc.printf("Tem: %f\n\r", bme.getTemperature());
    }
}