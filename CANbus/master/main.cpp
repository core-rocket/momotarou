#include "mbed.h"

Serial pc(PA_9, PA_10, 115200); //pin19,20 TX,RX
CAN can(PA_11, PA_12, 1000000); //pin21,22 rd,td

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
        pc.printf("%d\n\r", msg.len);
    } 
}

int main(){
    wait(0.1);
    pc.printf("Start.\n\r");
    while(1){
        send(0x01, 1.0, 'a');
        wait(0.1);
    }
}