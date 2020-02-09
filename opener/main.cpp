#include "mbed.h"
#include "../common.h"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
CAN can(PA_11, PA_12);

namespace global {
	Phase phase;
	CANMessage can_msg;
}

void can_recv();

int main(){
	global::phase = Phase::standby;
    pc.printf("start opener.\n\r");

	can.attach(can_recv, CAN::RxIrq);

	while(true){
		// フェーズ送信

		switch(global::phase){
		case Phase::standby:
			// 待機フェーズ: 無線でフライトモードON
			break;
		case Phase::flight:
			// 離床判定
			break;
		case Phase::burning:
			// 加速上昇中
			break;
		case Phase::rising:
			// 慣性飛行: 離床判定
			break;
		case Phase::parachute:
			break;
		}
	}
}

void can_recv(){
	if(can.read(global::can_msg) == 0)
		return;

	const auto &msg = global::can_msg;
	switch(msg.id){
	case MsgID::command:
		// flight mode on?
		break;
	case MsgId::error:
		break;
	case MsgID::acc:
		// push to FIFO
		break;
	case MsgID::air_press:
		// push to FIFO
		break;
	default:
		break;
	}
}
