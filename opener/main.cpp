#include "mbed.h"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX

int main(){
    pc.printf("start TWE-Lite.\n\r");
}
