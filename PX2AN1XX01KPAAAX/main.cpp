#include "mbed.h"
#include "../common.h"
/*
Vout[V]    :0.5 -> 4.5
analogIn[V]:0.333 -> 3.0
range[Mpa] :0 -> 7
*/
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, BRATE); //pin19,20 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED); //pin21,22 rd,td

float sensorvalue = 0.0;
float offset = 0.8849;
float chamberpressure = 0.0;
float coefficient = 2.6249;
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
        //pc.printf("Voltage: %f\n\r", sensorvalue);
        //pc.printf("Pressure[MPa]: %f\n\r", chamberpressure);
        send(MsgID::burn_press, chamberpressure);
        wait(0.001);
    }
}