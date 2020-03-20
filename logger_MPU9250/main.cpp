/*
SDカードに整数を書き込む．
<pin接続>
NSS: pin10 --- pin2 CD/DAT3
SCK: pin11 --- pin5 CLK
MISO:pin12 --- pin7 DAT0
MOSI:pin13 --- pin3 CMD
*/
#include "mbed.h"
#include "SDFileSystem.h"
#include "MPU9250.h"

DigitalOut myled(PB_1); //pin15
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

MPU9250 mpu = MPU9250(PB_7, PB_6); // pin30,29 SDA,SCL
SDFileSystem sd = SDFileSystem(PA_7, PA_6, PA_5, PA_4, "sd"); //MOSI,MISO,SCK,CS

void mpu_init();

int main(){
    int16_t acc[3], gyr[3], mag[3];
    wait(1.0); //気持ち
    pc.printf("Start.Init....\n\r");
    mpu_init();
    wait(2.0); // 静止させるまで待つ
    pc.printf("Init OK\n\r");

    while(1){
    FILE *fp = fopen("/sd/test.csv", "a");
    //FILE *fp = fopen("/sd/test.txt", "w");
    mpu.readGyroData(gyr);
    mpu.readAccelData(acc);
    mpu.readMagData(mag);
    if(fp != NULL){
        myled = 1;
        pc.printf("Writing to SDcard......\n\r");
        for(int i=1;i<1000;i++){
            mpu.readGyroData(gyr);
            mpu.readAccelData(acc);
            mpu.readMagData(mag);
            //fprintf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f\n",acc[0]/2049.81,acc[1]/2049.81,acc[2]/2049.81, gyr[0]*0.03048,gyr[1]*0.03048,gyr[2]*0.03048, mag[0]*0.15,mag[1]*0.15,mag[2]*0.15);
            fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",acc[0],acc[1],acc[2], gyr[0],gyr[1],gyr[2], mag[0],mag[1],mag[2]);
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

void mpu_init(){
    uint8_t whoami_MPU9250, whoami_AK8963;
    float mag_init[3];
    
    whoami_MPU9250 = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    whoami_AK8963 = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    pc.printf("MPU9250 IS 0x%x\n\r", whoami_MPU9250); // 0x71で正常
    pc.printf("AK8963 IS 0x%x\n\r", whoami_AK8963); // 0x48で正常
    
    if (whoami_MPU9250 == 0x71 && whoami_AK8963 == 0x48){  
        pc.printf("MPU9250 is detected.\n\r");
        wait(0.1);
        mpu.resetMPU9250();
        mpu.initMPU9250(); 
        wait(0.1);
        mpu.initAK8963(mag_init);
        mpu.getGres();
        mpu.getAres();
        mpu.getMres();
        wait(0.1);
    }
   else{
        pc.printf("Could not detect MPU9250.\n\r");
        pc.printf("whoami_MPU9250 = 0x%x\n\rwhoami_AK8963 = 0x%x\r\n", whoami_MPU9250, whoami_AK8963);
        while(1);
    }
}
