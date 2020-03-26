#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include "CAN.h"

namespace utility {
	namespace can {
		template<typename T>
		auto get_data(const CANMessage &msg, const size_t start=0) -> const T* {
			if(start + sizeof(T) != msg.len)
				return nullptr;
			const auto ptr = static_cast<const uint8_t*>(msg.data + start);
			return reinterpret_cast<const T*>(ptr);
		}
	}
}

#endif
