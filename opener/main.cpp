#include "mbed.h"
#include "../phase.h"

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

void can_recv(){
	if(can.read(global::can_msg) == 0)
		return;

	const &msg = global::can_msg;
	// 簡単なparse
}
