#include "mbed.h"
#include "../common.h"
#include "../telemetry.hpp"
#include "../utility.hpp"
#include "TWE-Lite/TWE-Lite.hpp"
#include <cstddef>

#define QUEUE_SIZE	15

Serial pc(PA_2, PA_3, BRATE); //pin8,9 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED);

namespace global {
	TWE_Lite twe(PA_9, PA_10, BRATE);
	CANMessage can_msg;

	utility::queue<float, QUEUE_SIZE> apress;
	utility::queue<float, QUEUE_SIZE> acc_norm;
	utility::queue<float, QUEUE_SIZE> temperature;

	size_t send_count;
	size_t send_error_count;
}

union Float2Byte {
	float _float;
	char _byte[4];
};

void can_recv();

template<typename T>
auto send_telemetry(const size_t &id, const size_t &res_id, utility::queue<T, QUEUE_SIZE> &data, const size_t &size) -> void {
	static T send_buf[QUEUE_SIZE];

	if(data.size() < size) return;

	for(size_t i=0;i<size;i++)
		send_buf[i] = data.pop();

	global::twe.send_buf_extend(id, res_id, send_buf, sizeof(T)*size);
//	if(global::twe.check_send() == 1)
		global::send_count++;
//	else
//		global::send_error_count++;
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
		const auto &acc_norm = global::acc_norm;
		const auto &temperature = global::temperature;

		float send_buf[10];

		if(loop_num % 10000 == 0){
			pc.printf("queue=%02d,%02d,%02d err=%d ", apress.size(), acc_norm.size(), temperature.size(), send_err);
			const auto send_kb = 5.0 * send_cnt * sizeof(float) / 1024;
			const auto time_sec = boot_timer.read_ms() / 1000.0;
			pc.printf("%f kbps\r\n", send_kb*8/time_sec);
		}

		send_telemetry(0x01, 0x01, global::apress, 15);
		send_telemetry(0x01, 0x02, global::acc_norm, 15);
		send_telemetry(0x01, 0x03, global::temperature, 15);

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
	case MsgID::command:
		break;
	case MsgID::error:
		break;
	case MsgID::air_press:
		{
			const auto *apress = (Float2Byte*)msg.data;
			global::apress.push(apress->_float);
		}
		break;
	case MsgID::acc_norm:
		{
			const auto *acc_norm = (Float2Byte*)msg.data;
			global::acc_norm.push(acc_norm->_float);
		}
		break;
	case MsgID::phase:
		break;
	case MsgID::temperature:
		{
			const auto *temperature = (Float2Byte*)msg.data;
			global::temperature.push(temperature->_float);
		}
		break;
	default:
		//pc.printf("%x\r\n", msg.id);
		break;
	}
}
