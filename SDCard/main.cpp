/*
SDカードに文字列を書き込む．
<pin接続>
NSS: pin25 --- pin2 CD/DAT3
SCK: pin26 --- pin5 CLK
MISO:pin27 --- pin7 DAT0
MOSI:pin28 --- pin3 CMD
*/
#include "mbed.h"
#include "SDFileSystem.h"

DigitalOut myled(PB_1);
Serial pc(PA_9, PA_10, 115200); //pin 19, 20

SDFileSystem sd = SDFileSystem(PB_5, PB_4, PB_3, PA_15, "sd"); //pin 28, 27, 26, 25

int main(){
    wait(1.0); //気持ち
    pc.printf("Start.\n\r");
    myled = 1;

    FILE *fp = fopen("/sd/test.csv", "w");
    //FILE *fp = fopen("/sd/test.txt", "w");
    if(fp != NULL){
        pc.printf("Writing to SDcard......\n\r");
        for(int i=1;i<10;i++){
            fprintf(fp, "%d\n", i);
        }
        fclose(fp);
    }
    else{
        pc.printf("Failed.\n\r");
    }
    pc.printf("End.\n\r");
}