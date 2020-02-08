#include "mbed.h"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX

int main(){
    pc.printf("start TWE-Lite.\n\r");

	Serial es920lr(PA_9, PA_10, 115200);

	//uint8_t length;

	//length = 1;
	//es920lr.putc(length);
	//es920lr.putc('A');
	//wait(1.8);
	wait(2.0);

	while(true){
		for(uint8_t len=1;len<=50;len++){
			for(uint8_t i=0;i<len;i++){
				es920lr.printf("A");
			}
			es920lr.printf("\r\n");
			pc.printf("send %d\r\n", len);
			wait_ms(250);
		}
	}
}
