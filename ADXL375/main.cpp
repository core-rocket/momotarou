#include "mbed.h"
#include "ADXL375_i2c.h"

DigitalOut myled(PB_1);
Serial pc(PA_9, PA_10, 9600);

ADXL375_i2c adxl = ADXL375_i2c(PB_7, PB_6);

int16_t acc[3];
uint8_t whoami;

int main() {
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
        adxl.readAccelData(acc);
        pc.printf("Acc: %d, %d, %d\n\r", acc[0], acc[1], acc[2]);
        pc.printf("Acc:%f,%f,%f\r\n",(float)acc[0]/20.5, (float)acc[1]/20.5, (float)acc[2]/20.5); 
        wait(1);
    }
}
