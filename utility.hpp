#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include "mbed.h"
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
			mutex.lock();
			if(siz == BUFSIZE)
				return;
			buf[write] = val;
			siz++;
			write++;
			if(write == BUFSIZE)
				write = 0;
			mutex.unlock();
		}

		auto pop() -> T {
			const auto &ret = buf[read];

			mutex.lock();
			siz--;
			read++;
			if(read == BUFSIZE)
				read = 0;

			mutex.unlock();
			return ret;
		}
	protected:
		PlatformMutex mutex;
		size_t siz;
		size_t read, write;
		T buf[BUFSIZE];
	};
}

#endif
