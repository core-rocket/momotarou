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
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
MPU9250 mpu = MPU9250(PB_7, PB_6); //pin30,29 SDA,SCL
CAN can(PA_11, PA_12); //pin21,22 rd,td

uint8_t whoami_MPU9250,whoami_AK8963;
int16_t gyr[3], acc[3], mag[3], Temp;
float gx, gy, gz, ax, ay, az, mx, my, mz;
float mag_init[3];
int acc32_t[3];
char senddata[13];

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void send(float value_1,float value_2, float value_3, char moji){
    pc.printf("send_pre.");
    senddata[12] = moji;
    Float2Byte sendFloat;
    sendFloat._float = value_1;
    for(int i=0;i<4;++i){
        senddata[i] = sendFloat._byte[i];
    }
    sendFloat._float = value_2;
    for(int i=4;i<8;++i){
        senddata[i] = sendFloat._byte[i];
    }
    sendFloat._float = value_3;
    for(int i=8;i<12;++i){
        senddata[i] = sendFloat._byte[i];
    }
    if(can.write(CANMessage(0x05, senddata, 13))){ //ID:0x05
        pc.printf("Send.\n\r");
    } 
}

int main(){
    wait(0.1); //気持ち
    whoami_MPU9250 = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    whoami_AK8963 = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    pc.printf("MPU9250 IS 0x%x\n\r", whoami_MPU9250); //0x71で正常
    pc.printf("AK8963 IS 0x%x\n\r", whoami_AK8963); //0x48で正常
    
    if (whoami_MPU9250 == 0x71 || whoami_AK8963 == 0x48){  
        pc.printf("MPU9250 is detected.\n\r");
        mpu.resetMPU9250();
        //MPU9250とAK8963のCalibrationをここら辺でする
        mpu.initMPU9250(); 
        mpu.initAK8963(mag_init);
        pc.printf("Start.\n\r");
        mpu.getGres();
        mpu.getAres();
        mpu.getMres();
    }
    else{
        pc.printf("Could not detect MPU9250.\n\r");
        pc.printf("whoami_MPU9250 = 0x%x\n\rwhoami_AK8963 = 0x%x\r\n",whoami_MPU9250,whoami_AK8963);
        while(1){};
    }
    
    while(1){
        pc.printf("Read.");
        mpu.readGyroData(gyr);
        gx = gyr[0] * 0.03048;
        gy = gyr[1] * 0.03048;
        gz = gyr[2] * 0.03048;

        mpu.readAccelData(acc);
        ax = (int)acc[0] / 2049.81;
        ay = (int)acc[1] / 2049.81;
        az = (int)acc[2] / 2049.81;
        
        mpu.readMagData(mag);
        mx = mag[0] * 0.15;
        my = mag[1] * 0.15;
        mz = mag[2] * 0.15;

        Temp = mpu.readTempData();
        
        pc.printf("Gyr: %f, %f, %f\n\r", gx, gy, gz); 
        pc.printf("Acc: %f, %f, %f\n\r", ax, ay, az); 
        pc.printf("Mag: %f, %f, %f\n\r", mx, my, mz);
        
        send(gx, gy, gz, 'g');
        send(ax, ay, az, 'c');
        send(mx, my, mz, 'm');
        wait(0.1);
    }
}