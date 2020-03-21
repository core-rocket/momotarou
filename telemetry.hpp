#ifndef TELEMETRY_HPP_
#define TELEMETRY_HPP_

namespace telemetry {
	enum class Cmd : uint8_t {
		reset			= 0x00,
		flight			= 0x01,
		launch_clear	= 0x02,
		no_flight_pin	= 0xf0,
		unknown			= 0xff,
	};
}

#endif
