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
int acc32_t[3];

MPU9250 mpu = MPU9250(PB_7, PB_6);

int main(){
    wait(1.0); //気持ち
    whoami_MPU9250 = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    wait(0.1);
    whoami_AK8963 = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    pc.printf("MPU9250 IS 0x%x\n\r", whoami_MPU9250); //0x71で正常
    pc.printf("AK8963 IS 0x%x\n\r", whoami_AK8963); //0x48で正常
    

    if (whoami_MPU9250 == 0x71 || whoami_AK8963 == 0x48){  
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
        
        mpu.readGyroData(gyr);
        
        gx = gyr[0] *  0.03048;
        gy = gyr[1] *  0.03048;
        gz = gyr[2] *  0.03048;

        mpu.readAccelData(acc);
        ax = (int)acc[0] / 2049.81;
        ay = (int)acc[1] / 2049.81;
        az = (int)acc[2] / 2049.81;
        
        mpu.readMagData(mag);
        mx = mag[0] * 0.15;
        my = mag[1] * 0.15;
        mz = mag[2] * 0.15;

        Temp = mpu.readTempData();
        
        //pc.printf("Gyr: %f, %f, %f\n\r", gx, gy, gz); 
        //pc.printf("Acc: %f, %f, %f\n\r", ax, ay, az); 
        pc.printf("%f,%f,%f\n\r", mx, my, mz);
        
        /*
        mpu.readGyroData(gyr);
        mpu.readAccelData(acc);
        mpu.readMagData(mag);
        Temp = mpu.readTempData();
        acc32_t[0] = (int)acc[0];
        acc32_t[1] = (int)acc[1];
        acc32_t[2] = (int)acc[2];
        ax = (float)acc32_t[0] / 1000.0f;
        ay = (float)acc32_t[1];
        az = (float)acc32_t[2];
        pc.printf("%d,%d,%d\r\n",sizeof(acc[0]),sizeof(acc32_t[0]),sizeof(ax));
        pc.printf("Gyr: %d, %d, %d\n\r", gyr[0], gyr[1], gyr[2]); 
        pc.printf("Acc: %d, %d, %d\n\r", acc[0], acc[1], acc[2]);
        //pc.printf("Acc: %f, %f, %f\n\r", (float)acc32_t[0], (float)acc32_t[1], (float)acc32_t[2]); 
        //pc.printf("Acc: %f, %f, %f\n\r", ax, ay, az); 
        pc.printf("Mag: %d, %d, %d\n\r", mag[0], mag[1], mag[2]);
        pc.printf("Temp: %d\n\r",Temp); 
        */
        //wait(0.5);
    }
}
