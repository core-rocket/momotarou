/*
CAN通信slave側
rd - RXD
td - TXD
*/
#include "mbed.h"

//Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
Serial pc(PA_2, PA_3, 9600); //pin8,9 TX,RX

CAN can(PA_11, PA_12); //pin21,22 rd,td

float _x, _y, _z;
CANMessage msg;

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void receive(){
    Float2Byte getFloat;
    if(can.read(msg)){
        if(msg.id == 0x01){
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
            }            
            pc.printf("ID:%d,%c,%f\n\r", msg.id, msg.data[4], getFloat._float);
        }
        if(msg.id == 0x02){
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
            }            
            pc.printf("ID:%d,%c,%f\n\r", msg.id, msg.data[4], getFloat._float);
        }
        if(msg.id == 0x03){
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
            }            
            pc.printf("ID:%d,%c,%f\n\r", msg.id, msg.data[4], getFloat._float);
        }
        if(msg.id == 0x04){
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
            }            
            pc.printf("ID:%d,%c,%f\n\r", msg.id, msg.data[4], getFloat._float);
        }
        if(msg.id == 0x05){
            for(int i=0;i<4;++i){
                getFloat._byte[i] = msg.data[i];
            }
            _x = getFloat._float;
            for(int i=4;i<8;++i){
                getFloat._byte[i] = msg.data[i];
            }
            _y = getFloat._float;
            for(int i=8;i<12;++i){
                getFloat._byte[i] = msg.data[i];
            }
            _z = getFloat._float;
            pc.printf("ID:%d,%c,%f,%f,%f\n\r", msg.id, msg.data[12], _x, _y, _z);
        }
    }
}

int main(){
    pc.printf("Start.\n\r");
    can.attach(receive, CAN::RxIrq);
    while(1){
        wait(0.2);
    }
}