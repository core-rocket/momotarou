/*
BME280から気圧・温度を取得してシリアルで表示する．
<BME280ピン設定>
CSB : HIGH
SDO : LOW
*/
#include "mbed.h"
#include "BME280.h"

BME280 bme = BME280(PB_7, PB_6); //pin30,29 SDA,SCL

DigitalOut myled(PB_1); //pin15
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

float pre, tem;

float a =10.0;

int main(){
    wait(1.0); //気持ち
    bme.initialize();
    pc.printf("Start.\n\r");
    myled = 1;
    while(1){
pc.printf("%f", a);
        pc.printf("Pre: %f\n\r", bme.getPressure());
        pc.printf("Tem: %f\n\r", bme.getTemperature());
        wait(0.2);
    }
}