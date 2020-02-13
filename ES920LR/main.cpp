#include "mbed.h"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
Serial es920lr(PA_9, PA_10, 115200);
CAN can(PA_11, PA_12); //pin21,22 rd,td

uint8_t recv(char *buf){
	const auto len = es920lr.getc();
	if(len < 0)
		return 0;
	for(uint8_t i=0;i<len;i++){
		if(len > 50)
			break;
		buf[i] = es920lr.getc();
	}
	return len;
}

int check_response(const uint8_t &len, const char *buf){
	if(len != 6)
		return -1;
	if(buf[0] == 'N' && buf[1] == 'G')
		return -1;
	if(buf[0] == 'O' && buf[1] == 'K')
		return 1;
}

int main(){
	char buf[100];
    pc.printf("start ES920LR.\n\r");

	wait(5.0);

	es920lr.putc(0x01);
	es920lr.putc('A');
	while(!recv(buf));
	wait_ms(250);

	while(true){
		const auto len = recv(buf);
		if(len == 0) continue;

		if(len == 2 && buf[0] == 'F' && buf[1] == 'L'){
			can.write(CANMessage(0x01, buf, 1));
			es920lr.putc(0x11);
			es920lr.printf("flight mode on...");
			while(!recv(buf));
			wait_ms(250);
		}
	}
}
