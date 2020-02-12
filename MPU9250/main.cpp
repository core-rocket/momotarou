/*
MPU9250から9軸データを取得してCAN送信・シリアル送信．
9軸データ->5回移動平均．
姿勢角を出す．
<MPU9250ピン設定>
CS~ : HIGH
ADO : LOW
SDA/SCL 10kΩでpullup
20191023
*/
#include "mbed.h"
#include "MPU9250.h"

Serial pc(PA_9, PA_10, 115200); // pin19,20 TX,RX
MPU9250 mpu = MPU9250(PB_7, PB_6); // pin30,29 SDA,SCL
CAN can(PA_11, PA_12, 1000000); // pin21,22 rd,td

uint8_t whoami_MPU9250,whoami_AK8963;
int16_t acc[3], gyr[3], mag[3], Temp;
float mag_init[3];
int acc32_t[3];
char senddata[5];

#define N 5
float buff_ax[N], buff_ay[N], buff_az[N];
float buff_gx[N], buff_gy[N], buff_gz[N];
float buff_mx[N], buff_my[N], buff_mz[N];
float sum_ax = 0.0, sum_ay = 0.0, sum_az = 0.0;
float sum_gx = 0.0, sum_gy = 0.0, sum_gz = 0.0;
float sum_mx = 0.0, sum_my = 0.0, sum_mz = 0.0;
float ax_new = 0.0, ay_new = 0.0, az_new = 0.0;
float gx_new = 0.0, gy_new = 0.0, gz_new = 0.0;
float mx_new = 0.0, my_new = 0.0, mz_new = 0.0;
float ax = 0.0, ay = 0.0, az = 0.0;
float gx = 0.0, gy = 0.0, gz = 0.0;
float mx = 0.0, my = 0.0, mz = 0.0;
int cnt = 0;

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

// CAN送信
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

void mpu_init(){
    whoami_MPU9250 = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    wait(0.1);
    whoami_AK8963 = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    pc.printf("MPU9250 IS 0x%x\n\r", whoami_MPU9250); // 0x71で正常
    pc.printf("AK8963 IS 0x%x\n\r", whoami_AK8963); // 0x48で正常
    
    if (whoami_MPU9250 == 0x71 || whoami_AK8963 == 0x48){  
        pc.printf("MPU9250 is detected.\n\r");
        wait(0.1);
        mpu.resetMPU9250();
        /*
        MPU9250とAK8963のCalibrationをここら辺でする
        */
        mpu.initMPU9250(); 
        wait(0.1);
        mpu.initAK8963(mag_init);
        pc.printf("Start.\n\r");
        mpu.getGres();
        mpu.getAres();
        mpu.getMres();
        wait(0.1);
    }
   else{
        pc.printf("Could not detect MPU9250.\n\r");
        pc.printf("whoami_MPU9250 = 0x%x\n\rwhoami_AK8963 = 0x%x\r\n", 
                  whoami_MPU9250, whoami_AK8963);
        while(1);
    }
}

int main(){
    wait(0.1); // 気持ち
    mpu_init();
    
    for(int i=0;i<N;i++){
        mpu.readGyroData(gyr);
        mpu.readAccelData(acc);
        mpu.readMagData(mag);
        buff_ax[i] = acc[0] / 2049.81;
        buff_ay[i] = acc[1] / 2049.81;
        buff_az[i] = acc[2] / 2049.81;
        buff_gx[i] = gyr[0] * 0.03048;
        buff_gy[i] = gyr[1] * 0.03048;
        buff_gz[i] = gyr[2] * 0.03048;
        buff_mx[i] = mag[0] * 0.15;
        buff_my[i] = mag[1] * 0.15;
        buff_mz[i] = mag[2] * 0.15;
        sum_ax += buff_ax[i];
        sum_ay += buff_ay[i];
        sum_az += buff_az[i];
        sum_gx += buff_gx[i];
        sum_gy += buff_gy[i];
        sum_gz += buff_gz[i];
        sum_mx += buff_mx[i];
        sum_my += buff_my[i];
        sum_mz += buff_mz[i];
    }
    
    wait(0.1);
    pc.printf("Start.\n\r");
    while(1){
        mpu.readGyroData(gyr);
        mpu.readAccelData(acc);
        mpu.readMagData(mag);
        sum_ax = sum_ax - buff_ax[cnt];
        sum_ay = sum_ay - buff_ay[cnt];
        sum_az = sum_az - buff_az[cnt];
        sum_gx = sum_gx - buff_gx[cnt];
        sum_gy = sum_gy - buff_gy[cnt];
        sum_gz = sum_gz - buff_gz[cnt];
        sum_mx = sum_mx - buff_mx[cnt];
        sum_my = sum_my - buff_my[cnt];
        sum_mz = sum_mz - buff_mz[cnt];
        ax_new = acc[0] / 2049.81;
        ay_new = acc[1] / 2049.81;
        az_new = acc[2] / 2049.81;
        gx_new = gyr[0] * 0.03048;
        gy_new = gyr[1] * 0.03048;
        gz_new = gyr[2] * 0.03048;
        mx_new = mag[0] * 0.15;
        my_new = mag[1] * 0.15;
        mz_new = mag[2] * 0.15;
        buff_ax[cnt] = acc[0] / 2049.81;
        buff_ay[cnt] = acc[1] / 2049.81;
        buff_az[cnt] = acc[2] / 2049.81;
        buff_gx[cnt] = gyr[0] * 0.03048;
        buff_gy[cnt] = gyr[1] * 0.03048;
        buff_gz[cnt] = gyr[2] * 0.03048;
        buff_mx[cnt] = mag[0] * 0.15;
        buff_my[cnt] = mag[1] * 0.15;
        buff_mz[cnt] = mag[2] * 0.15;
        sum_ax = sum_ax + buff_ax[cnt];
        sum_ay = sum_ay + buff_ay[cnt];
        sum_az = sum_az + buff_az[cnt];
        sum_gx = sum_gx + buff_gx[cnt];
        sum_gy = sum_gy + buff_gy[cnt];
        sum_gz = sum_gz + buff_gz[cnt];
        sum_mx = sum_mx + buff_mx[cnt];
        sum_my = sum_my + buff_my[cnt];
        sum_mz = sum_mz + buff_mz[cnt];
        cnt++;
        if(cnt == N) cnt = 0;
        ax = sum_ax/N;
        ay = sum_ay/N;
        az = sum_az/N;
        gx = sum_gx/N;
        gy = sum_gy/N;
        gz = sum_gz/N;
        mx = sum_mx/N;
        my = sum_my/N;
        mz = sum_mz/N;        
        /*
        姿勢角計算をする．
        */        
        pc.printf("acc:%f,%f,%f\n\r", ax, ay, az);
        pc.printf("gyr:%f,%f,%f\n\r", gx, gy, gz);
        pc.printf("mag:%f,%f,%f\n\r", mx, my, mz);
        send(0x04, ax, 'x');
        send(0x04, ay, 'y');
        send(0x04, az, 'z');
        send(0x07, gx, 'x');
        send(0x07, gy, 'y');
        send(0x07, gz, 'z');
        send(0x0B, mx, 'x');
        send(0x0B, my, 'y');
        send(0x0B, mz, 'z');
        wait(0.001);
    }
}
