#include "mbed.h"

Serial pc(PA_9, PA_10, 115200); //pin19,20 TX,RX
//Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
CAN can(PA_11, PA_12, 1000000); //pin21,22 rd,td

CANMessage msg;

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

int main(){
    wait(0.1);
    pc.printf("Start.\n\r");
    while(1){
        if(can.read(msg)){
            //if(msg.id == 10){
                for(int i=1;i<5;++i){
                    f2b._byte[i] = msg.data[i];
                }
                pc.printf("ID:%d,%c,%f\n\r", msg.id, msg.data[0], f2b._float);
            //}
        }
        //wait(0.1);
    }
}