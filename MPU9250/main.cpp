/*
MPU9250から9軸データを取得してCAN送信・シリアル送信．
ノルムは5回移動平均から算出．
生データも送る．
姿勢角を出す．
<MPU9250ピン設定>
CS~ : HIGH
ADO : LOW
SDA/SCL 10kΩでpullup
*/
#include "mbed.h"
#include "MPU9250.h"

Serial pc(PA_9, PA_10, 115200); // pin19,20 TX,RX
MPU9250 mpu = MPU9250(PB_7, PB_6); // pin30,29 SDA,SCL
CAN can(PA_11, PA_12, 1000000); // pin21,22 rd,td

#define PI 3.14159265358979323846f
#define N 5 // 5回移動平均
#define sampleFreq 100.0f
#define beta 0.33f // gain（大きいと加速度による補正が早い）

char senddata[5];

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
        //pc.printf("%d,%c\n\r", id, moji);
    } 
}

void mpu_init(){
    uint8_t whoami_MPU9250, whoami_AK8963;
    float mag_init[3];
    
    whoami_MPU9250 = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    whoami_AK8963 = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    pc.printf("MPU9250 IS 0x%x\n\r", whoami_MPU9250); // 0x71で正常
    pc.printf("AK8963 IS 0x%x\n\r", whoami_AK8963); // 0x48で正常
    
    if (whoami_MPU9250 == 0x71 || whoami_AK8963 == 0x48){  
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
        pc.printf("whoami_MPU9250 = 0x%x\n\rwhoami_AK8963 = 0x%x\r\n", 
                  whoami_MPU9250, whoami_AK8963);
        while(1);
    }
}

int main(){
    int16_t acc[3], gyr[3], mag[3];
    
    float buff_ax[N], buff_ay[N], buff_az[N];
    float buff_gx[N], buff_gy[N], buff_gz[N];
    float buff_mx[N], buff_my[N], buff_mz[N];
    float sum_ax = 0.0f, sum_ay = 0.0f, sum_az = 0.0f;
    float sum_gx = 0.0f, sum_gy = 0.0f, sum_gz = 0.0f;
    float sum_mx = 0.0f, sum_my = 0.0f, sum_mz = 0.0f;
    float ax_new = 0.0f, ay_new = 0.0f, az_new = 0.0f;
    float gx_new = 0.0f, gy_new = 0.0f, gz_new = 0.0f;
    float mx_new = 0.0f, my_new = 0.0f, mz_new = 0.0f;
    float gx_i = 0.0f, gy_i = 0.0f, gz_i = 0.0f;
    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    float gx = 0.0f, gy = 0.0f, gz = 0.0f;
    float mx = 0.0f, my = 0.0f, mz = 0.0f;
    int cnt = 0;
    float a_norm = 0.0f;

    float psi = 0.0f, cta = 0.0f, eta =0.0f;
    float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    float _gx, _gy, _gz;
    float norm;
    float s0, s1, s2, s3;
    float qdot0, qdot1, qdot2, qdot3;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, _q0q0, _q1q1, _q2q2, _q3q3;
    
    wait(0.1);
    mpu_init();
    wait(2.0); // 静止させるまで待つ

    // 初期値
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
    
    // 静止時角速度
    gx_i = sum_gx / N;
    gy_i = sum_gy / N;
    gz_i = sum_gz / N;
    pc.printf("gyr_ini:%f,%f,%f\n\r", gx_i, gy_i, gz_i);

    wait(0.1);
    pc.printf("Start.\n\r");
    while(1){
        //timea.start();
        mpu.readGyroData(gyr);
        mpu.readAccelData(acc);
        mpu.readMagData(mag);

        /* 5回移動平均 */
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
        buff_ax[cnt] = ax_new;
        buff_ay[cnt] = ay_new;
        buff_az[cnt] = az_new;
        buff_gx[cnt] = gx_new;
        buff_gy[cnt] = gy_new;
        buff_gz[cnt] = gz_new;
        buff_mx[cnt] = mx_new;
        buff_my[cnt] = my_new;
        buff_mz[cnt] = mz_new;
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
        
        ax = sum_ax / N;
        ay = sum_ay / N;
        az = sum_az / N;
        gx = sum_gx / N - gx_i;
        gy = sum_gy / N - gy_i;
        gz = sum_gz / N - gz_i;
        mx = sum_mx / N;
        my = sum_my / N;
        mz = sum_mz / N;

        /* 加速度ノルム */
        a_norm = sqrt(ax*ax + ay*ay + az*az);

        /* 姿勢角(Quaternion->Eulerangle) */ 
        _gx = gx * PI / 180.f;
        _gy = gy * PI / 180.f;
        _gz = gz * PI / 180.f;
        // qdot=0.5*q*w
        qdot0 = 0.5f * (       - q1*_gx - q2*_gy - q3*_gz);
        qdot1 = 0.5f * (q0*_gx          + q2*_gz - q3*_gy);
        qdot2 = 0.5f * (q0*_gy - q1*_gz          + q3*_gx);
        qdot3 = 0.5f * (q0*_gz + q1*_gy - q2*_gx         );
        // 加速度で補正する（動加速度下ではどうなん？動だけに）
        if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))){
            norm = sqrt(ax*ax + ay*ay + az*az);
            ax /= norm;
            ay /= norm;
            az /= norm;   

            _2q0 = 2.0f * q0;
            _2q1 = 2.0f * q1;
            _2q2 = 2.0f * q2;
            _2q3 = 2.0f * q3;
            _4q0 = 4.0f * q0;
            _4q1 = 4.0f * q1;
            _4q2 = 4.0f * q2;
            _8q1 = 8.0f * q1;
            _8q2 = 8.0f * q2;
            _q0q0 = q0 * q0;
            _q1q1 = q1 * q1;
            _q2q2 = q2 * q2;
            _q3q3 = q3 * q3;

            s0 =    _4q0*_q2q2 + _2q2*ax +    _4q0*_q1q1 - _2q1*ay;
            s1 =    _4q1*_q3q3 - _2q3*ax + 4.0f*_q0q0*q1 - _2q0*ay - _4q1 + _8q1*_q1q1 + _8q1*_q2q2 + _4q1*az;
            s2 = 4.0f*_q0q0*q2 + _2q0*ax +    _4q2*_q3q3 - _2q3*ay - _4q2 + _8q2*_q1q1 + _8q2*_q2q2 + _4q2*az;
            s3 = 4.0f*_q1q1*q3 - _2q1*ax + 4.0f*_q2q2*q3 - _2q2*ay;
            
            norm = sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); 
            s0 /= norm;
            s1 /= norm;
            s2 /= norm;
            s3 /= norm;

            qdot0 -= beta * s0;
            qdot1 -= beta * s1;
            qdot2 -= beta * s2;
            qdot3 -= beta * s3;
        }
        // q+=qdot*dt
        q0 += qdot0 * (1.0f / sampleFreq);
        q1 += qdot1 * (1.0f / sampleFreq);
        q2 += qdot2 * (1.0f / sampleFreq);
        q3 += qdot3 * (1.0f / sampleFreq);
        
        norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q0 /= norm;
        q1 /= norm;
        q2 /= norm;
        q3 /= norm;
        
        //psi = atan2(2.0f*q0*q1 + 2.0f*q2*q3, 2.0f*q0*q0 + 2.0f*q3*q3 - 1.0f) * 180.0f / PI;
        //cta =  asin(2.0f*q0*q2 - 2.0f*q1*q3)                                 * 180.0f / PI;
        //eta = atan2(2.0f*q0*q3 - 2.0f*q1*q2, 2.0f*q0*q0 + 2.0f*q1*q1 - 1.0f) * 180.0f / PI;
        
        //pc.printf("a_norm:%f\n\r", a_norm);       
        //pc.printf("acc_ave:%f,%f,%f\n\r", ax, ay, az);
        //pc.printf("gyr_ave:%f,%f,%f\n\r", gx, gy, gz);
        //pc.printf("mag_ave:%f,%f,%f\n\r", mx, my, mz);
        //pc.printf("%f,%f,%f,%f\n\r", q0, q1, q2, q3);
        //pc.printf("angle: %f, %f, %f\n\r", psi, cta, eta);
        //pc.printf("%f\n\r", timea.read());
        send(0x04, a_norm, 'a');

        send(0x07, q0, 'a');
        send(0x07, q1, 'b');
        send(0x07, q2, 'c');
        send(0x07, q3, 'd');

        send(0x08, ax, 'x');
        send(0x08, ay, 'y');
        send(0x08, az, 'z');
        
        send(0x09, gx, 'x');
        send(0x09, gy, 'y');
        send(0x09, gz, 'z');
        
        send(0x0D, mx, 'x');
        send(0x0D, my, 'y');
        send(0x0D, mz, 'z');
        //wait(0.001);
    }
}
