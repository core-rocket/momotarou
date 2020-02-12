/*
SDカードに整数を書き込む．
<pin接続>
NSS: pin25 --- pin2 CD/DAT3
SCK: pin26 --- pin5 CLK
MISO:pin27 --- pin7 DAT0
MOSI:pin28 --- pin3 CMD
*/
#include "mbed.h"
#include "SDFileSystem.h"
#include "MPU9250.h"

DigitalOut myled(PB_1); //pin15
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

SDFileSystem sd = SDFileSystem(PB_5, PB_4, PB_3, PA_15, "sd"); //pin28,27,26,25

int main(){
    wait(1.0); //気持ち
    pc.printf("Start.\n\r");

    FILE *fp = fopen("/sd/test.csv", "w");
    //FILE *fp = fopen("/sd/test.txt", "w");
    if(fp != NULL){
        myled = 1;
        pc.printf("Writing to SDcard......\n\r");
        for(int i=1;i<10;i++){
            fprintf(fp, "%d\n", i);
        }
        fclose(fp);
        myled = 0;
    }
    else{
        pc.printf("Failed.\n\r");
    }
    pc.printf("End.\n\r");
}