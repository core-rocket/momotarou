#include "mbed.h"
#include "../common.h"
#include "../telemetry.hpp"
#include "../utility.hpp"
#include "TWE-Lite/TWE-Lite.hpp"

Serial pc(PA_2, PA_3, BRATE); //pin8,9 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED);

namespace global {
	CANMessage can_msg;

	utility::queue<float, 10> apress;
}

union Float2Byte {
	float _float;
	char _byte[4];
};

void can_recv();

int main(){
	TWE_Lite twe(PA_9, PA_10, BRATE);
	Timer boot_timer;

	boot_timer.start();

	pc.printf("start TWE-Lite.\n\r");
	twe.init();
	can.attach(can_recv, CAN::RxIrq);

	size_t loop_num = 0;
	size_t send_cnt = 0;
	size_t send_err = 0;
	while(true){
		const auto &apress = global::apress;

		float send_buf[10];

		if(loop_num % 10000 == 0){
			pc.printf("queue=%d err=%d ", apress.size(), send_err);
			const auto send_kb = 5.0 * send_cnt * sizeof(float) / 1024;
			const auto time_sec = boot_timer.read_ms() / 1000.0;
			pc.printf("%f kbps\r\n", send_kb*8/time_sec);
		}

		if(apress.size() >= 5){
			for(size_t i=0;i<5;i++)
				send_buf[i] = global::apress.pop();
			twe.send_buf_simple(0x01, 0x00, send_buf, sizeof(float)*5);

			if(twe.check_send() == 1)
				send_cnt++;
			else
				send_err++;
		}
		loop_num++;
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
		{
			const auto *apress = (Float2Byte*)msg.data;
			global::apress.push(apress->_float);
		}
		break;
	default:
		break;
	}
}
