#include "mbed.h"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
Serial es920lr(PA_9, PA_10, 115200);

uint8_t buf[50];

bool recv(){
	const auto len = es920lr.getc();
	if(len < 0)
		return false;
	for(uint8_t i=0;i<len;i++){
		if(len > 50)
			break;
		buf[i] = es920lr.getc();
	}

	return true;
}



int main(){
    pc.printf("start TWE-Lite.\n\r");

	wait(5.0);

	es920lr.printf("start\r\n");

	while(true){
		if(recv()){
			uint8_t len = 50;
			es920lr.putc(len);
			for(uint8_t i=0;i<len;i++){
				es920lr.putc(buf[i]);
			}
			while(!recv());
			if(buf[0] == 'O' && buf[1] == 'K')
				wait_ms(250);
		}
	}
}
