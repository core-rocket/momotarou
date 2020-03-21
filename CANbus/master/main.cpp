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
DigitalOut myled(PB_1); //pin15
Ticker ticker;

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void send(){
    pc.printf("Master send()\n\r");
    
    /*ID: 0x01*/
    Float2Byte sendFloat;
    sendFloat._float = 89.123; //ここに送りたい値を入れる．
    
    char serialData[4];
    for(int i=0;i<4;++i){
        serialData[i] = sendFloat._byte[i];
        pc.printf("send_char: %d\n\r", serialData[i]);
    }
    pc.printf("sendFloat: %f\n\r", sendFloat._float);
    if(can.write(CANMessage(0x01, serialData, 4))){
        pc.printf("Send.\n\r");
    } 
    
    /*ID: 0x02*/
    Float2Byte sendFloat2;
    sendFloat2._float = 30.203; //ここに送りたい値を入れる．
    char serialData2[4];
    for(int i=0;i<4;++i){
        serialData2[i] = sendFloat2._byte[i];
        pc.printf("send_char2: %d\n\r", serialData2[i]);
    }
    pc.printf("sendFloat2: %f\n\r", sendFloat2._float);
    if(can.write(CANMessage(0x02, serialData2, 4))){
        pc.printf("Send2.\n\r");
    } 
    
    myled = !myled;
}

int main(){
    pc.printf("Master_start.\n\r");
    ticker.attach(&send, 1);
    while(1){
        pc.printf("Master loop()\n\r");
        wait(1.0);
    }
}