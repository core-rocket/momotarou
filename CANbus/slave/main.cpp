/*
CAN通信slave側
2種類のIDから受信する．
rd - RXD
td - TXD
MCP2551まわりは以下参照(終端Rは410ohmを使った，そこにあったので)
https://raw.githubusercontent.com/rummanwaqar/teensy_can/master/schematic.png
*/
#include "mbed.h"

Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td
DigitalOut myled(PB_1); //pin15

CANMessage msg;

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void receive(){
    Float2Byte getFloat;
    
    if(can.read(msg)){
        /*ID: 0x01*/
        if(msg.id == 0x01){
            pc.printf("ID: 0x01\n\r");
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
                pc.printf("get_char: %d\n\r", getFloat._byte[i]);
            }
            pc.printf("getFloat: %f\n\r", getFloat._float);
            myled = !myled;
        }
        /*ID: 0x02*/
        if(msg.id == 0x02){
            pc.printf("ID: 0x02\n\r");
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
                pc.printf("get_char: %d\n\r", getFloat._byte[i]);
            }
            pc.printf("getFloat: %f\n\r", getFloat._float);
            myled = !myled;
        }
    }
}

int main(){
    pc.printf("Slave_Start.\n\r");
    can.attach(receive, CAN::RxIrq);
    while(1){
        pc.printf("Slave loop()\n\r");
        wait(0.2);
    }
}