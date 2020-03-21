/*
BME280から気圧・温度を取得してCAN送信・シリアル送信．
気圧->5回移動平均．
<BME280ピン設定>
CSB : HIGH
SDO : LOW
*/
#include "mbed.h"
#include "BME280.h"
BME280 bme = BME280(PB_7, PB_6); // pin30,29 SDA,SCL
Serial pc(PA_9, PA_10, 115200); // pin19,20 TX,RX
CAN can(PA_11, PA_12, 1000000); // pin21,22 rd,td

#define N 5
float pressure_ave = 0.0f; // 気圧[hPa]
float pressure_new = 0.0f;
float buff_p[N];
float sum = 0.0f;
int cnt = 0;
float tem = 0.0f; // 温度[C]
char senddata[5];

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

void send(int id, float value){
    f2b._float = value;
    for(int i=0;i<4;++i){
        senddata[i] = f2b._byte[i];
    }
    CANMessage msg(id, senddata, sizeof(float));
    if(can.write(msg)){
        //pc.printf("%d,%c\n\r", id, moji);
    } 
}

int main(){
    wait(0.1);
    bme.initialize();
    
    for(int i;i<N;i++){
        buff_p[i] = bme.getPressure();
        sum += buff_p[i];
    }
    
    pc.printf("Start.\n\r");
    while(1){
        sum = sum - buff_p[cnt];
        pressure_new = bme.getPressure();
        buff_p[cnt] = pressure_new;
        sum = sum + pressure_new;
        cnt++;
        if(cnt == N) cnt = 0;
        pressure_ave = sum/N;
        
        tem = bme.getTemperature();
        
        //pc.printf("Pre[hPa]: %f\n\r", pressure_ave);
        //pc.printf("Tem[C] : %f\n\r", tem);
        send(0x03, pressure_ave);
        send(0x0C, tem);
        wait(0.0005);
    }
}
