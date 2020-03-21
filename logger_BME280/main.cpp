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
#include "BME280.h"

DigitalOut myled(PB_1); //pin15
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
BME280 bme = BME280(PB_7, PB_6); //pin30,29 SDA,SCL

SDFileSystem sd = SDFileSystem(PA_7, PA_6, PA_5, PA_4, "sd"); //MOSI,MISO,SCK,CS

int main(){
    int pre,tem;
    wait(1.0); //気持ち
    pc.printf("Start.\n\r");
    bme.initialize();
    pc.printf("BME_Init_OK.\n\r");
    /*
    while(1){
        pre = bme.getPressure();
        tem = bme.getTemperature();
        pc.printf("%d,%d\n\r", pre,tem);
        wait(1.0);
    }
    */

    while(1){
        FILE *fp = fopen("/sd/BME.csv", "a");
        //FILE *fp = fopen("/sd/test.txt", "w");
        if(fp != NULL){
            myled = 1;
            pc.printf("Writing to SDcard......\n\r");
            for(int i=0;i<30;i++){
                pre = bme.getPressure();
                tem = bme.getTemperature();
                //fprintf(fp, "%d\n", i);
                fprintf(fp,"%d,%d\n", pre,tem);
                wait(0.05);
            }
            fclose(fp);
            myled = 0;
        }
        else{
            pc.printf("Failed.\n\r");
        }
        pc.printf("End.\n\r");
    }
}