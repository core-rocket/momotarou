#include "mbed.h"
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td

float voltage=0, kpa=0, sensorvalue, velocity;
float air_density=1.1834, offset=0.2, coefficient=1.0;
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
    if(can.write(CANMessage(0x01, senddata, 5))){ //ID:0x01
        pc.printf("Send.\n\r");
    } 
}

int main(){
    pc.printf("Start.\n\r");
    while(1){
        wait(0.1);
        sensorvalue = analogin.read()*3.3f - offset;
        kpa = sensorvalue*10.0f; //10=50(kPa)/5(V)
        velocity = coefficient*sqrt(2.0f*kpa*1000/air_density);
        pc.printf("sensorvalue: %f\n\r", sensorvalue);
        pc.printf("Pressure: %f\n\r", kpa);
        pc.printf("IAS: %f\n\r", velocity);
        send(kpa, 'k');
        send(velocity, 'v');
    }
}