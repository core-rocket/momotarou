/*
CAN通信master側
2種類のIDで送信する．
rd - RXD
td - TXD
MCP2551まわりは以下参照(終端Rは410ohmを使った，そこにあったので)
https://raw.githubusercontent.com/rummanwaqar/teensy_can/master/schematic.png
*/
#include "mbed.h"
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td

char senddata_1[5];
char senddata_3[13];

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void send_1(float value, char moji){
    senddata_1[4] = moji;
    Float2Byte sendFloat;
    sendFloat._float = value;
    for(int i=0;i<4;++i){
        senddata_1[i] = sendFloat._byte[i];
    }
    if(can.write(CANMessage(0x02, senddata_1, 5))){ //ID:0x02
        pc.printf("Send.\n\r");
    } 
}

void send_3(float value_1,float value_2, float value_3, char moji){
    pc.printf("send_pre.");
    senddata_3[12] = moji;
    Float2Byte sendFloat;
    sendFloat._float = value_1;
    for(int i=0;i<4;++i){
        senddata_3[i] = sendFloat._byte[i];
    }
    sendFloat._float = value_2;
    for(int i=4;i<8;++i){
        senddata_3[i] = sendFloat._byte[i];
    }
    sendFloat._float = value_3;
    for(int i=8;i<12;++i){
        senddata_3[i] = sendFloat._byte[i];
    }
    if(can.write(CANMessage(0x05, senddata_3, 13))){ //ID:0x05
        pc.printf("Send.\n\r");
    } 
}

int main(){
    pc.printf("start.\n\r");
    while(1){
        wait(0.1);
        send_1(1.0, 'a');
        wait(0.1);
        send_3(2.0, 3.0, 4.0, 'b');
    }
}