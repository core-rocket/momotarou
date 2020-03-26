/*
ロードセルから荷重を取得してシリアルで表示・トワで送信・microSDカードに保存．
オンラインコンパイラでコンパイル．
*/
#include "mbed.h"
#include "Hx711.h"
#include "SDFileSystem.h"

Serial twe(PA_9, PA_10, 115200); //pin19,20 TX,RX
Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
HX711 hx = HX711(PA_11, PA_12); //pin21,22 DAT/CLK
SDFileSystem sd = SDFileSystem(PA_7, PA_6, PA_5, PA_4, "sd");

float scale = 1.0f;
float weight = 0.0f;
float n2o = 0.0f;
char times = 10;
int cnt = 0;

int main(){
    wait(0.1); //気持ち
    pc.printf("Start.\n\r");
    
    FILE *fp = fopen("/sd/log.txt", "a");
    if(fp != NULL){
        fprintf(fp, "Start.\n");
        fclose(fp);
    }
    else{
        pc.printf("Failed.\n\r");
    }
    
    hx.tare(times);
    hx.set_scale(scale);
    
    while(1){
        FILE *fp = fopen("/sd/log.txt", "a");
        int cnt = 0;
        if(fp != NULL){
            while(1){
                weight = hx.get_units(times);
                n2o = weight*0.0458f;
                
                pc.printf("Weight[g]:%f\n\r", n2o);
                twe.printf("Weight[g]:%f\r\n", n2o);
                fprintf(fp, "%f\n", n2o);
                cnt++;
                
                if(cnt == 1000){
                    fclose(fp);
                    pc.printf("Save.\n\r");
                    break;
                }
            }
        }
        else{
            pc.printf("Failed.\n\r");
        }
    }
}