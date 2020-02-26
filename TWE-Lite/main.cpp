#include "mbed.h"
#include "../common.h"
#include "../telemetry.hpp"
#include "../utility.hpp"
#include "TWE-Lite/TWE-Lite.hpp"

Serial pc(PA_2, PA_3, BRATE); //pin8,9 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED);

namespace global {
	CANMessage can_msg;
}

void can_recv();

int main(){
	TWE_Lite twe(PA_9, PA_10, 115200);

	pc.printf("start TWE-Lite.\n\r");
	twe.init();
	can.attach(can_recv, CAN::RxIrq);

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

void can_recv(){
	using namespace telemetry;
	using namespace utility::can;

	if(can.read(global::can_msg) == 0)
		return;

	const auto &msg = global::can_msg;

	switch(static_cast<MsgID>(msg.id)){
	case MsgID::air_press:
		break;
	default:
		break;
	}
}
