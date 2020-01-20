/*
BME280から気圧・温度を取得してシリアルで表示する．
<BME280ピン設定>
CSB : HIGH
SDO : LOW
*/
#include "mbed.h"
#include "BME280.h"
BME280 bme = BME280(PB_7, PB_6); //pin30,29 SDA,SCL
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td

float pre, tem;
char senddata[5];

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void send(float value, char moji){
    senddata[4] = moji;
    Float2Byte sendFloat;
    sendFloat._float = value;
    for(int i=0;i<4;++i){
        senddata[i] = sendFloat._byte[i];
    }
    if(can.write(CANMessage(0x02, senddata, 5))){ //ID:0x02
        pc.printf("Send.\n\r");
    } 
}

int main(){
    wait(0.1);
    bme.initialize();
    pc.printf("Start.\n\r");
    while(1){
        pre = bme.getPressure();
        tem = bme.getTemperature();
        pc.printf("Pre: %f\n\r", pre);
        pc.printf("Tem: %f\n\r", tem);
        wait(0.2);
        send(pre, 'p');
        send(tem, 't');
    }
}