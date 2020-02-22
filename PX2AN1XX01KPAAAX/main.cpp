#include "mbed.h"
/*
Vout[V]    :0.5 -> 4.5
analogIn[V]:0.333 -> 3.0
range[Mpa] :0 -> 7
*/
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, 115200); //pin19,20 TX,RX
CAN can(PA_11, PA_12, 1000000); //pin21,22 rd,td

float sensorvalue = 0.0;
float offset = 0.8849;
float chamberpressure = 0.0;
float coefficient = 2.6249;
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
    wait(0.1);
    pc.printf("Start.\n\r");
    while(1){
        sensorvalue = analogin.read()*3.3f;
        if(sensorvalue > 0.333){
            chamberpressure = sensorvalue * coefficient - offset;
        }
        else{
            chamberpressure = 0.0;
        }
        pc.printf("Voltage: %f\n\r", sensorvalue);
        pc.printf("Pressure[MPa]: %f\n\r", chamberpressure);
        send(0x0A, chamberpressure, 'c');
    }
}