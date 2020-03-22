#include "mbed.h"
#include "../common.h"
#include "../telemetry.hpp"
#include "../utility.hpp"

#define DEBUG

#define ACC_NORM_CHECK_VALUE	  3.0
#define ACC_NUM_CHECK			    5
#define DOWN_NUM_CHECK			   10
#define BURNING_TIMEOUT_MS		 5000
#define PARACHUTE_TIMEOUT_MS	14000

Serial pc(PA_9, PA_10, BRATE); //pin8,9 TX,RX
CAN can(PA_11, PA_12, CAN_SPEED);
DigitalIn flight_pin(PA_3);
DigitalOut hamada(PA_1);

namespace global {
	size_t loop_num;
	bool flag_manual_launch_clear;
	Phase phase;
	float accnorm, apress;
	uint8_t accnum;
	uint8_t downnum;
	size_t err_cansend;

	Timer boot_timer, flight_timer;
	CANMessage recv_msg;
}

union Float2Byte {
    float _float;
    char _byte[4];
};

size_t boot_time_ms(){ return global::boot_timer.read_ms(); }
size_t time_ms(){ return global::flight_timer.read_ms(); }

void init();
void can_recv();
void parse_cmd(const CANMessage &msg);

#ifdef DEBUG
	void debug_print();
#endif

int main(){
	init();

    pc.printf("start opener.\n\r");

	CANMessage msg;

	can.attach(can_recv, CAN::RxIrq);

	while(true){
		const auto &loop_num = global::loop_num;
		const auto &phase = global::phase;
		const auto &accnum = global::accnum;
		const auto &downnum = global::downnum;

		// フェーズ送信
		msg.id = static_cast<unsigned int>(MsgID::phase);
		msg.data[0] = static_cast<uint8_t>(phase);
		msg.len = 1;
		if(!can.write(msg)){
			global::err_cansend++;
		}

		#ifdef DEBUG
		if(loop_num % 1000 == 0){
			debug_print();
		}
		#endif

		switch(phase){
		case Phase::standby:
			// 待機フェーズ: 無線でフライトモードON
			hamada = 0;
			break;
		case Phase::flight:
			// 離床判定
			hamada = 0;
			{
				const auto &manual_launch = global::flag_manual_launch_clear;
				const bool is_pin = !flight_pin;
				const bool is_acc = accnum >= ACC_NUM_CHECK;
				if(is_pin || is_acc || manual_launch){
					global::phase = Phase::burning;
					global::flight_timer.reset();
					global::flight_timer.start();

					pc.printf("LIFT OFF: ");
					if(is_pin)
						pc.printf("pin ");
					if(is_acc)
						pc.printf("acc ");
					if(manual_launch)
						pc.printf("manual");
					pc.printf("\r\n");
				}
			}
			break;
		case Phase::burning:
			// 加速上昇中
			hamada = 0;
			if(time_ms() > BURNING_TIMEOUT_MS)
				global::phase = Phase::rising;
			break;
		case Phase::rising:
			// 慣性飛行: 開放判定
			hamada = 0;
			{
				const auto &manual_launch = global::flag_manual_launch_clear;
				const bool is_down = downnum >= DOWN_NUM_CHECK;
				const bool is_timeout = time_ms() > PARACHUTE_TIMEOUT_MS;

				// 手動で離床確認した場合，タイムアウト開傘を無効化
				const bool is_burnout = (manual_launch ? false : is_timeout);

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
			hamada = 1;
			break;
		}
		wait_us(100);	// 50us待たないとCAN送信でエラーが起こる．マージン取って100us．
		global::loop_num++;
	}
}

void init(){
	hamada = 0;
	global::loop_num = 0;
	global::flag_manual_launch_clear = false;
	global::phase = Phase::standby;
	global::accnorm= 0.0;
	global::apress = 0.0;
	global::accnum = 0;
	global::downnum = 0;
	global::err_cansend = 0;

	global::boot_timer.start();
}

void can_recv(){
	using namespace telemetry;
	using namespace utility::can;

	if(can.read(global::recv_msg) == 0)
		return;

	const auto &msg = global::recv_msg;
	switch(static_cast<MsgID>(msg.id)){
	case MsgID::command:
		parse_cmd(msg);
		break;
	case MsgID::error:
		break;
	case MsgID::acc:
		// push to FIFO
		{
			const Float2Byte *acc = (Float2Byte*)msg.data;
			if(acc->_float > ACC_NORM_CHECK_VALUE)
				global::accnum++;
			else
				global::accnum = 0;
			global::accnorm = acc->_float;
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

void parse_cmd(const CANMessage &msg){
	using namespace telemetry;

	const auto &phase = global::phase;

	if(msg.len != 1){
		pc.printf("error: unknown length command\r\n");
		return;
	}

	switch(static_cast<Cmd>(msg.data[0])){
	case Cmd::reset:	// reset
		pc.printf("reset\r\n");
		global::phase = Phase::standby;
		global::flight_timer.stop();
		global::flight_timer.reset();
		break;
	case Cmd::flight:	// flight mode ON
		if(phase == Phase::standby){
			pc.printf("FLIGHT MODE ON\r\n");
			global::phase = Phase::flight;
		}
		break;
	case Cmd::launch_clear:	// ランチクリア確認コマンド
		if(phase == Phase::standby){
			global::flag_manual_launch_clear = true;
		}
		break;
	case Cmd::no_flight_pin:	// no flight pin
		break;
	default:
		pc.printf("error: unknown command(%d)\r\n", (int)msg.data[0]);
		break;
	}
}

#ifdef DEBUG
void debug_print(){
	const auto &loop_num = global::loop_num;
	const auto &err_can = global::err_cansend;
	const auto &phase = global::phase;
	const auto &accnum= global::accnum;
	const auto &downnum= global::downnum;
	const auto &accnorm= global::accnorm;
	const auto &apress= global::apress;

	pc.printf("%10d:%f:%f ", loop_num, boot_time_ms() / 1000.0, time_ms() / 1000.0);
	pc.printf(" canerr=%f%% ", 100.0 * err_can / loop_num);
	pc.printf("phase: ");
	switch(phase){
	case Phase::standby:	pc.printf("standby,  "); break;
	case Phase::flight:		pc.printf("flight,   "); break;
	case Phase::burning:	pc.printf("burning,  "); break;
	case Phase::rising:		pc.printf("rising,   "); break;
	case Phase::parachute:	pc.printf("parachute,"); break;
	}
	pc.printf("accnorm: %f, apress: %f, ", accnorm, apress);
	pc.printf("accnum: %d, downnum: %d\r\n", (int)accnum, (int)downnum);
}
#endif
