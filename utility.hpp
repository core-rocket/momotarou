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

	template<typename T, size_t BUFSIZE>
	class queue {
	public:
		queue() : siz(0), read(0), write(0) {}

		auto empty() const -> bool { return (size() == 0); }
		auto size() const -> size_t { return this->siz; }

		auto front() const -> const T& { return buf[read]; }

		auto push(const T &val) -> void {
			buf[write] = val;
			siz++;
			write++;
			if(write == BUFSIZE)
				write = 0;
		}

		auto pop() -> T {
			const auto &ret = buf[read];
			siz--;
			read++;
			if(read == BUFSIZE)
				read = 0;
			return ret;
		}
	protected:
		size_t siz;
		size_t read, write;
		T buf[BUFSIZE];
	};
}

#endif
