/*
MPU9250からジャイロ・加速度・地磁気を取得してシリアルで表示する．
<MPU9250ピン設定>
CS~ : HIGH
ADO : LOW
SDA/SCL 10kΩでpullup
20191023
*/
#include "mbed.h"
#include "MPU9250.h"

DigitalOut myled(PB_1);
Serial pc(PA_9, PA_10, 9600);

uint8_t whoami_MPU9250,whoami_AK8963;
int16_t gyr[3], acc[3], mag[3],Temp;
float gx, gy, gz, ax, ay, az, mx, my, mz;
float mag_init[3];

MPU9250 mpu = MPU9250(PB_7, PB_6);

int main(){
    wait(1.0); //気持ち
    whoami_MPU9250 = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    whoami_AK8963 = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    pc.printf("MPU9250 IS 0x%x\n\r", whoami_MPU9250); //0x71で正常
    pc.printf("AK8963 IS 0x%x\n\r", whoami_AK8963); //0x48で正常
    

    if (whoami_MPU9250 == 0x71 && whoami_AK8963 == 0x48){  
        pc.printf("MPU9250 is detected.\n\r");
        wait(1.0); //気持ち
        mpu.resetMPU9250();
        //MPU9250とAK8963のCalibrationをここら辺でする
        mpu.initMPU9250(); 
        wait(1.0); //気持ち
        mpu.initAK8963(mag_init);
        wait(1.0);
        myled = 1;
        pc.printf("Start.\n\r");
        mpu.getGres();
        mpu.getAres();
        mpu.getMres();
        wait(1.0); //気持ち
   }
   else{
        pc.printf("Could not detect MPU9250.\n\r");
        pc.printf("whoami_MPU9250 = 0x%x\n\rwhoami_AK8963 = 0x%x\r\n",whoami_MPU9250,whoami_AK8963);
        while(1);
    }
    
    while(1) {        
        /*
        mpu.readGyroData(gyr);
        
        gx = gyr[0] * 250.0 / 32768.0;
        gy = gyr[1] * 250.0 / 32768.0;
        gz = gyr[2] * 250.0 / 32768.0;

        mpu.readAccelData(acc);
        ax = acc[0] * 2.0 / 32768.0;
        ay = acc[1] * 2.0 / 32768.0;
        az = acc[2] * 2.0 / 32768.0;
        
        mpu.readMagData(mag);
        mx = mag[0] * 10.0 * 4219.0 / 8190.0;
        my = mag[1] * 10.0 * 4219.0 / 8190.0;
        mz = mag[2] * 10.0 * 4219.0 / 8190.0;

        Temp = mpu.readTempData();
        
        pc.printf("Gyr: %d, %d, %d\n\r", gx, gy, gz); 
        pc.printf("Acc: %d, %d, %d\n\r", ax, ay, az); 
        pc.printf("Mag: %d, %d, %d\n\r", mx, my, mz);
        */

        mpu.readGyroData(gyr);
        mpu.readAccelData(acc);
        mpu.readMagData(mag);
        Temp = mpu.readTempData();
        pc.printf("Gyr: %d, %d, %d\n\r", gyr[0], gyr[1], gyr[2]); 
        pc.printf("Acc: %d, %d, %d\n\r", acc[0], acc[1], acc[2]); 
        pc.printf("Mag: %d, %d, %d\n\r", mag[0], mag[1], mag[2]);
        pc.printf("Temp: %d\n\r",Temp); 
        wait(1);
    }
}
