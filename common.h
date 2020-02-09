#ifndef COMMON_HPP_
#define COMMON_HPP_

enum class Phase {
	standby,
	flight,
	burning,
	rising,
	parachute,
};

enum class MsgID : uint8_t {
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
