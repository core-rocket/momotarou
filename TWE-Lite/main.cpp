#include "mbed.h"
#include "TWE-Lite/TWE-Lite.hpp"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX

int main(){
    pc.printf("start TWE-Lite.\n\r");

	TWE_Lite twe(PA_9, PA_10, 115200);

	twe.init();

	char msgbuf[250];
	for(int i=0;i<250;i++){
		msgbuf[i] = 'T';
	}

	while(true){
		for(int len=1;len<16;len++){
			twe.send_buf_extend(0x00, 0x00, msgbuf, len);
			const auto ret = twe.check_send();
			pc.printf("ret: %d\n\r", ret);
			wait_ms(1000);
		}
	}
}
