/*
ロードセルから荷重を取得してシリアルで表示する
*/
#include "mbed.h"
#include "Hx711.h"

DigitalOut myled(PB_1); //pin15
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

float scale = 1.0;
char times = 10;
float weight;

HX711 hx = HX711(PA_11, PA_12); //pin21,22 DAT/CLK

int main(){
    wait(1.0); //気持ち
    pc.printf("Start.\n\r");
    
    hx.tare(times);
    hx.set_scale(scale);
    
    myled = 1;
    while(1){
        weight = hx.get_units(times);
        pc.printf("Weight: %f\n\r", weight/100.0); //とりあえず100で割る[g]でいい感じの値が出る．
    }
}