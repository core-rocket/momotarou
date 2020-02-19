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
	error,
	air_press,
	acc,
	gps,
	phase,
	angular_vel,
	burn_press,
	dynamic_press,
	temperature,
	magnetic,
};

#endif
