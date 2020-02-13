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

void debug_print();

int main(){
	global::phase = Phase::standby;
    pc.printf("start opener.\n\r");

	CANMessage msg;
	Timer flight_timer;

	can.attach(can_recv, CAN::RxIrq);

	while(true){
		const auto &phase = global::phase;
		const auto &accnum = global::accnum;
		const auto &downnum = global::downnum;

		// フェーズ送信
		msg.id = static_cast<unsigned int>(MsgID::phase);
		msg.data[0] = static_cast<uint8_t>(phase);
		msg.len = 1;
		if(!can.write(msg))
			pc.printf("error: cannot send phase\r\n");

		debug_print();

		switch(phase){
		case Phase::standby:
			// 待機フェーズ: 無線でフライトモードON
			break;
		case Phase::flight:
			// 離床判定
			{
				const bool is_pin = !flight_pin;
				const bool is_acc = accnum >= 5;
				if(is_pin || is_acc){
					global::phase = Phase::burning;
					flight_timer.reset();
					flight_timer.start();

					pc.printf("LIFT OFF: ");
					if(is_pin)
						pc.printf("pin ");
					if(is_acc)
						pc.printf("acc");
					pc.printf("\r\n");
				}
			}
			break;
		case Phase::burning:
			// 加速上昇中
			if(flight_timer.read_ms() > 5000)
				global::phase = Phase::rising;
			break;
		case Phase::rising:
			// 慣性飛行: 開放判定
			{
				const bool is_down = downnum >= 10;
				const bool is_burnout = flight_timer.read_ms() > 14000;
				if(is_down || is_burnout){
					global::phase = Phase::parachute;

					pc.printf("PARACHUTE OPEN: ");
					if(is_down)
						pc.printf("down ");
					if(is_burnout)
						pc.printf("timer");
					pc.printf("\r\n");
				}
			}
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
		case 0x00:	// reset
			pc.printf("reset\r\n");
			global::phase = Phase::standby;
			break;
		case 0x01:	// flight mode ON
			pc.printf("FLIGHT MODE ON\r\n");
			global::phase = Phase::flight;
			break;
		case 0x02:	// open parachute
			if(global::phase == Phase::rising){
				global::phase = Phase::parachute;
			}
			break;
		case 0xf0:	// no flight pin
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
			if(acc->_float > 3.0)
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

void debug_print(){
	const auto &phase = global::phase;
	const auto &accnum= global::accnum;
	const auto &downnum= global::downnum;
	const auto &apress= global::apress;

	pc.printf("phase: ");
	switch(phase){
	case Phase::standby:	pc.printf("standby,  "); break;
	case Phase::flight:		pc.printf("flight,   "); break;
	case Phase::burning:	pc.printf("burning,  "); break;
	case Phase::rising:		pc.printf("rising,   "); break;
	case Phase::parachute:	pc.printf("parachute,"); break;
	}
	pc.printf("accnum: %d, apress: %f, downnum: %d\r\n", (int)accnum, apress, (int)downnum);
}
