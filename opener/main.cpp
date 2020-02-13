#include "mbed.h"
#include "../common.h"
#include "../utility.hpp"

Serial pc(PA_9, PA_10, 115200); //pin8,9 TX,RX
CAN can(PA_11, PA_12, 1000000);
DigitalIn flight_pin(PA_3);

namespace global {
	Phase phase;
	CANMessage recv_msg;
	float apress;
	uint8_t accnum;
	uint8_t downnum;
}

union Float2Byte {
    float _float;
    char _byte[4];
};

void can_recv();

int main(){
	global::phase = Phase::standby;
    pc.printf("start opener.\n\r");

	CANMessage msg;

	can.attach(can_recv, CAN::RxIrq);

	while(true){
		const auto &phase = global::phase;

		// フェーズ送信
		msg.id = static_cast<unsigned int>(MsgID::phase);
		msg.data[0] = static_cast<uint8_t>(phase);
		msg.len = 1;
		if(!can.write(msg))
			pc.printf("error: cannot send phase\r\n");

		pc.printf("phase: %d, accnum: %d, apress: %f, downnum: %d\r\n", (int)static_cast<uint8_t>(phase), (int)global::accnum, global::apress, (int)global::downnum);

		switch(phase){
		case Phase::standby:
			// 待機フェーズ: 無線でフライトモードON
			break;
		case Phase::flight:
			// 離床判定
			if(!flight_pin && global::accnum >= 5)
				global::phase = Phase::burning;
			break;
		case Phase::burning:
			// 加速上昇中
			break;
		case Phase::rising:
			// 慣性飛行: 開放判定
			if(global::downnum >= 10)
				global::phase = Phase::parachute;
			break;
		case Phase::parachute:
			break;
		}
		wait_us(100);
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
		{
			const Float2Byte *acc = (Float2Byte*)msg.data;
			if(acc->_float > 1.1)
				global::accnum++;
			else
				global::accnum = 0;
		}
		break;
	case MsgID::air_press:
		// push to FIFO
		{
			//const auto *apress = get_data<float>(msg);
			//global::apress = *apress;
			const Float2Byte *apress = (Float2Byte*)msg.data;
			if(apress->_float > global::apress)
				global::downnum++;
			else
				global::downnum = 0;
			global::apress = apress->_float;
		}
		break;
	default:
		break;
	}
}
