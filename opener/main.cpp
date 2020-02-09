#include "mbed.h"
#include "../common.h"
#include "../utility.hpp"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX
CAN can(PA_11, PA_12);

namespace global {
	Phase phase;
	CANMessage recv_msg;
}

void can_recv();

int main(){
	global::phase = Phase::standby;
    pc.printf("start opener.\n\r");

	CANMessage msg;

	can.attach(can_recv, CAN::RxIrq);

	while(true){
		// フェーズ送信
		msg.id = static_cast<unsigned int>(MsgID::phase);
		msg.data[0] = static_cast<uint8_t>(global::phase);
		msg.len = 1;
		if(!can.write(msg))
			pc.printf("error: cannot send phase\r\n");

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
	using namespace utility::can;

	if(can.read(global::recv_msg) == 0)
		return;

	const auto &msg = global::recv_msg;
	switch(static_cast<MsgID>(msg.id)){
	case MsgID::command:
		if(msg.len != 1){
			pc.printf("error: unknown length command\r\n");
			return;
		}
		// とりあえず0x01をフライトモードONとする
		// 後で必ずちゃんと決めること！！！！！！！
		switch(msg.data[0]){
		case 0x01: // flight mode ON
			pc.printf("FLIGHT MODE ON!!!!!!!\r\n");
			global::phase = Phase::flight;
			break;
		default:
			pc.printf("error: unknown command(%d)\r\n", (int)msg.data[0]);
			break;
		}
		break;
	case MsgID::error:
		break;
	case MsgID::acc:
		// push to FIFO
		break;
	case MsgID::air_press:
		// push to FIFO
		{
			const auto apress = get_data<float>(msg);
		}
		break;
	default:
		break;
	}
}
