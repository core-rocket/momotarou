/*
MPU9250からジャイロ・加速度・地磁気を取得してCAN送信・シリアル送信．
<MPU9250ピン設定>
CS~ : HIGH
ADO : LOW
SDA/SCL 10kΩでpullup
*/
#include "mbed.h"
#include "MPU9250.h"
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
MPU9250 mpu = MPU9250(PB_7, PB_6); //pin30,29 SDA,SCL
CAN can(PA_11, PA_12, 100000); //pin21,22 rd,td

uint8_t whoami_MPU9250,whoami_AK8963;
int16_t gyr[3], acc[3], mag[3], Temp;
float g_x, g_y, g_z, a_x, a_y, a_z, m_x, m_y, m_z;
float mag_init[3];
int acc32_t[3];
char senddata[5];

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

void send(int id, float value, char moji){
    senddata[0] = moji;
    f2b._float = value;
    for(int i=1;i<5;++i){
        senddata[i] = f2b._byte[i];
    }
    CANMessage msg(id, senddata, 5);
    if(can.write(msg)){
        pc.printf("%d,%c\n\r", id, moji);
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
        mpu.readAccelData(acc);
        a_x = (int)acc[0] / 2049.81;
        a_y = (int)acc[1] / 2049.81;
        a_z = (int)acc[2] / 2049.81;
        
        mpu.readGyroData(gyr);
        g_x = gyr[0] * 0.03048;
        g_y = gyr[1] * 0.03048;
        g_z = gyr[2] * 0.03048;
        
        mpu.readMagData(mag);
        m_x = mag[0] * 0.15;
        m_y = mag[1] * 0.15;
        m_z = mag[2] * 0.15;

        //Temp = mpu.readTempData();
        
        pc.printf("Acc: %f, %f, %f\n\r", a_x, a_y, a_z);
        pc.printf("Gyr: %f, %f, %f\n\r", g_x, g_y, g_z); 
        pc.printf("Mag: %f, %f, %f\n\r", m_x, m_y, m_z);
        
        send(0x04, a_x, 'x');
        send(0x04, a_y, 'y');
        send(0x04, a_z, 'z');

        send(0x07, g_x, 'x');
        send(0x07, g_y, 'y');
        send(0x07, g_z, 'z');
        
        send(0xB, m_x, 'x');
        send(0xB, m_y, 'y');
        send(0xB, m_z, 'z');

        wait(0.1);
    }
}