#include "mbed.h"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
Serial es920lr(PA_9, PA_10, 115200);
CAN can(PA_11, PA_12, 1000000); //pin21,22 rd,td

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

uint8_t parse_cmd(const char *buf){
	if(buf[0] == 'R' && buf[1] == 'S'){
		return 0x00;
	}else if(buf[0] == 'F' && buf[1] == 'L'){
		return 0x01;
	}else if(buf[0] == 'L' && buf[1] == 'A'){
		return 0x02;
	}else if(buf[0] == 'N' && buf[1] == 'F'){
		return 0xf0;
	}else{
		return 0xff;
	}
}

int main(){
	char buf[50];
    pc.printf("start ES920LR.\n\r");

	wait(5.0);

	es920lr.putc(0x01);
	es920lr.putc('A');
	while(!recv(buf));
	wait_ms(250);

	while(true){
		const auto len = recv(buf);
		if(len == 0) continue;

		if(len == 2){
			char cmd[1];
			cmd[0] = parse_cmd(buf);
			if(cmd[0] == 0xff) continue;
			if(can.write(CANMessage(0x01, cmd, 1))){
				es920lr.putc(0x10);
				es920lr.printf("send command: %02X", cmd[0]);
			}else{
				es920lr.putc(0x06);
				es920lr.printf("failed");
			}
			while(!recv(buf));
			wait_ms(250);
		}
	}
}
