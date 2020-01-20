#include "mbed.h"
AnalogIn analogin(PA_0); //pin6
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX
DigitalOut myled(PB_1); //pin15
float voltage=0, kpa=0, sensorvalue, velocity;
float air_density=1.1834, offset=0.2, coefficient=1.0;

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
    }
}