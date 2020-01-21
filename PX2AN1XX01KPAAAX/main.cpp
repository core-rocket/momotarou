#include "mbed.h"
/*
Vout 0.5~5.0[V]
analogIn 0.25~2.5[V]
coefficient:6.89/(2.5-0.25)=3.062
*/
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td

float sensorvalue, offset=0.0, chamberpressure, coefficient=2.585;
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
    if(can.write(CANMessage(0x03, senddata, 5))){ //ID:0x03
        pc.printf("Send.\n\r");
    } 
}

int main(){
    pc.printf("Start.\n\r");
    while(1){
        wait(0.1);
        sensorvalue = analogin.read()*3.3f - offset;
        if(sensorvalue > 0.25){
            chamberpressure = sensorvalue * coefficient;
        }
        else{
            chamberpressure = 0.0;
        }
        pc.printf("Voltage: %f\n\r", sensorvalue);
        pc.printf("Pressure: %f\n\r", chamberpressure);
        send(chamberpressure, 'c');
    }
}