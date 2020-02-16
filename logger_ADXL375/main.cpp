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
#include "ADXL375_i2c.h"

DigitalOut myled(PB_1); //pin15
ADXL375_i2c adxl = ADXL375_i2c(PB_7, PB_6);
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

SDFileSystem sd = SDFileSystem(PB_5, PB_4, PB_3, PA_15, "sd"); //pin28,27,26,25

int16_t acc[3];
uint8_t whoami;

int main(){
    wait(1.0); //気持ち
    pc.printf("Start.\n\r");

    whoami = adxl.readByte(ADXL375_I2C_HIGH_READ, ADXL375_DEVID_REG);
    pc.printf("I AM 0x%x\n\r", whoami); //0xE5で正常

    if (whoami == 0xE5){ 
        pc.printf("ADXL375 is detected.\n\r");
        wait(1.0); //気持ち
        adxl.initADXL375();
        myled = 1;
        wait(1.0); //気持ち
   }
   else{
        pc.printf("Could not detect ADXL375.\n\r");
        while(1);
    }

    while(1) {  
        FILE *fp = fopen("/sd/test.csv", "a");

        if(fp != NULL){
            myled = 1;
            adxl.readAccelData(acc);
            //pc.printf("Acc: %d, %d, %d\n\r", acc[0], acc[1], acc[2]);
            pc.printf("Acc:%f,%f,%f\r\n",(float)acc[0]/20.5, (float)acc[1]/20.5, (float)acc[2]/20.5); 
            fprintf(fp, "%f,%f,%f\r\n",(float)acc[0]/20.5, (float)acc[1]/20.5, (float)acc[2]/20.5);
            fclose(fp);
            myled = 0;
        }else{
            pc.printf("Failed.\n\r");
        }
    
    wait(0.5);

    }

}