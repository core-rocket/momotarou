#include "mbed.h"
/*
Vout 0.2~4.7[V]
analogIn 0.133~3.133[V]
coefficient:50/(3.133-0.133)=16.666
*/
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, 115200); //pin19,20 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td

float kpa = 0.0;
float sensorvalue = 0.0;
float offset = 0.0;
float coefficient = 16.666;
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
        pc.printf("%d,%c\n\r", id, moji);
    }
}

int main(){
    pc.printf("Start.\n\r");
    wait(0.1);
    while(1){
        sensorvalue = analogin.read()*3.3f - offset;
        if(sensorvalue > 0.13){
            kpa = sensorvalue * coefficient;
        }
        else{
            kpa = 0.0;
        }
        
        pc.printf("sensorvalue: %f\n\r", sensorvalue);
        pc.printf("Pressure: %f\n\r", kpa);
        send(0x09, kpa, 'k');
    }
}