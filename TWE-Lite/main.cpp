#include "mbed.h"

Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

int main(){
    pc.printf("start TWE-Lite.\n\r");
}
