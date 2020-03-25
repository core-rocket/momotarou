#include "mbed.h"
#include "../common.h"
/*
Vout 0.2~4.7[V]
analogIn 0.133~3.133[V]
coefficient:50/(3.133-0.133)=16.666
*/
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, BRATE); //pin19,20 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED); //pin21,22 rd,td

float kpa = 0.0;
float sensorvalue = 0.0;
float offset = 0.0;
float coefficient = 16.666;
char senddata[4];

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

void send(const MsgID &id, const float &value){
    f2b._float = value;
    for(int i=0;i<4;++i){
        senddata[i] = f2b._byte[i];
    }
    CANMessage msg(static_cast<uint8_t>(id), senddata, sizeof(float));
    if(can.write(msg)){
        pc.printf("%d\n\r", static_cast<uint8_t>(id));
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
        
        //pc.printf("sensorvalue: %f\n\r", sensorvalue);
        //pc.printf("Pressure: %f\n\r", kpa);
        send(MsgID::dynamic_press, kpa);
        wait(0.001);
    }
}