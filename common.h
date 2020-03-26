#ifndef COMMON_HPP_
#define COMMON_HPP_

#define BRATE		115200
#define CAN_SPEED	1000000

enum class Phase : uint8_t {
	standby,
	flight,
	burning,
	rising,
	parachute,
};

enum class MsgID : unsigned int {	// 29bit identifier
	command		= 0x01,
	error = 0x02,
	air_press = 0x03,
	acc_norm = 0x04,
	gps = 0x05,
	phase = 0x06,
	quaternion = 0x07,
	acc = 0x08,
	gyro = 0x09,
	burn_press = 0x0A,
	dynamic_press = 0x0B,
	temperature = 0x0C,
	magnetic = 0x0D,
};

#endif
