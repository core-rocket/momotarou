/*
ロードセルから荷重を取得してシリアルで表示する
*/
#include "mbed.h"
#include "Hx711.h"

Serial pc(PA_9, PA_10, 115200); //pin19,20 TX,RX
HX711 hx = HX711(PA_11, PA_12); //pin21,22 DAT/CLK

float scale = 1.0;
char times = 10;
float weight;

int main(){
    wait(0.1); //気持ち
    pc.printf("Start.\n\r");
    
    hx.tare(times);
    hx.set_scale(scale);

    while(1){
        weight = hx.get_units(times);
        pc.printf("Weight[g]: %f\n\r", weight*0.0458f);
    }
}