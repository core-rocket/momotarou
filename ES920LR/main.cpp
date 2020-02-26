#include "mbed.h"
#include "../common.h"
#include "../telemetry.hpp"

Serial pc(PA_2, PA_3, BRATE); //pin8,9 TX,RX
Serial es920lr(PA_9, PA_10, BRATE);
CAN can(PA_11, PA_12, CAN_SPEED); //pin21,22 rd,td

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

telemetry::Cmd parse_cmd(const char *buf){
	using telemetry::Cmd;

	if(buf[0] == 'R' && buf[1] == 'S'){
		return Cmd::reset;
	}else if(buf[0] == 'F' && buf[1] == 'L'){
		return Cmd::flight;
	}else if(buf[0] == 'L' && buf[1] == 'A'){
		return Cmd::launch_clear;
	}else if(buf[0] == 'N' && buf[1] == 'F'){
		return Cmd::no_flight_pin;
	}else{
		return Cmd::unknown;
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
			const auto cmd = parse_cmd(buf);
			if(cmd == telemetry::Cmd::unknown) continue;

			char cmd_buf[1];
			cmd_buf[0] = static_cast<uint8_t>(cmd);
			if(can.write(CANMessage(0x01, cmd_buf, 1))){
				es920lr.putc(0x10);
				es920lr.printf("send command: %02X", cmd_buf[0]);
			}else{
				es920lr.putc(0x06);
				es920lr.printf("failed");
			}
			while(!recv(buf));
			wait_ms(250);
		}
	}
}
