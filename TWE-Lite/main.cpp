#include "mbed.h"
#include "../common.h"
#include "../telemetry.hpp"
#include "../utility.hpp"
#include "TWE-Lite/TWE-Lite.hpp"
#include <cstddef>

#define QUEUE_SIZE	10

Serial pc(PA_2, PA_3, BRATE); //pin8,9 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED);

namespace global {
	TWE_Lite twe(PA_9, PA_10, BRATE);
	CANMessage can_msg;

	utility::queue<float, QUEUE_SIZE> apress;
	utility::queue<float, QUEUE_SIZE> acc;

	size_t send_count;
	size_t send_error_count;
}

union Float2Byte {
	float _float;
	char _byte[4];
};

void can_recv();

template<typename T>
auto send_telemetry(const size_t id, utility::queue<T, QUEUE_SIZE> &data) -> void {
	static T send_buf[QUEUE_SIZE];

	if(data.size() < 5) return;

	for(size_t i=0;i<5;i++)
		send_buf[i] = data.pop();

	global::twe.send_buf_extend(id, id, send_buf, sizeof(T)*5);
	if(global::twe.check_send() == 1)
		global::send_count++;
	else
		global::send_error_count++;
}

int main(){
	Timer boot_timer;

	boot_timer.start();

	pc.printf("start TWE-Lite.\n\r");
	global::twe.init();
	can.attach(can_recv, CAN::RxIrq);

	size_t loop_num = 0;
	const auto& send_cnt = global::send_count;
	const auto& send_err = global::send_error_count;
	while(true){
		const auto &apress = global::apress;

		float send_buf[10];

		if(loop_num % 10000 == 0){
			pc.printf("queue=%d err=%d ", apress.size(), send_err);
			const auto send_kb = 5.0 * send_cnt * sizeof(float) / 1024;
			const auto time_sec = boot_timer.read_ms() / 1000.0;
			pc.printf("%f kbps\r\n", send_kb*8/time_sec);
		}

		send_telemetry(0x01, global::apress);
		send_telemetry(0x02, global::acc);

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
	case MsgID::acc:
		{
			const auto *acc = (Float2Byte*)msg.data;
			global::acc.push(acc->_float);
		}
		break;
	default:
		break;
	}
}
