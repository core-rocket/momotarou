#ifndef UTILITY_STRING_HPP_
#define UTILITY_STRING_HPP_

#include <iostream>
#include <cstdint>

namespace utility {
	auto memcpy(void *dest, const void *src, size_t n) -> void* {
		uint8_t *d = static_cast<uint8_t*>(dest);
		const uint8_t *s = (const uint8_t*)src;
		for(size_t i=0;i<n;i++){
			*d = *s;
			s++;
			d++;
		}
		return dest;
	}

	auto strlen(const char *str) -> size_t {
		for(size_t len=0;;len++){
			if(str[len] == '\0') return len;
		}
	}

	template<typename T>
	auto swap(T &v1, T &v2){
		T tmp = v1;
		v1 = v2;
		v2 = tmp;
	}

	class string {
	public:
		string() : ptr(nullptr), capacity_(0), length_(0) {
			reserve(5);
			this->ptr[0] = '\0';
		}
		string(const char *str) : ptr(nullptr), capacity_(0), length_(0) {
			const auto len= strlen(str);
			reserve(len);

			memcpy(this->ptr, str, len+1);
			this->length_ = len;
		}
		~string(){
			if(ptr != nullptr)
				delete[] ptr;
		}

		auto operator[](const size_t pos) -> char& {
			if(pos >= length()) return ptr[length()];
			return ptr[pos];
		}

		auto operator+=(const char c) -> string& {
			const auto len = length() + 1;
			reserve(len);

			this->ptr[length()] = c;
			this->length_ = len;
			return *this;
		}

		auto operator+=(const string &str) -> string& {
			const auto len = this->length() + str.length();
			reserve(len);
			memcpy(this->ptr+this->length(), str.ptr, str.length()+1);

			this->length_ = len;
			return *this;
		}

		auto reserve(const size_t sz) -> void {
			if(capacity() >= sz) return;

			auto p = new char[sz+1];
			if(this->ptr != nullptr){
				memcpy(p, this->ptr, length()+1);
				delete[] this->ptr;
			}

			this->ptr = p;
			this->capacity_ = sz;
		}

		auto reverse() -> void {
			if(length() == 0) return;
			size_t first = 0;
			size_t last  = length();

			for(;first!=last && first!=--last;++first){
				swap(this->ptr[first], this->ptr[last]);
			}
		}

		auto length() const -> const size_t {
			return this->length_;
		}
		auto capacity() const -> const size_t {
			return this->capacity_;
		}
		auto c_str() const -> const char* {
			return static_cast<const char*>(ptr);
		}
	protected:
		char *ptr;
		size_t capacity_;
		size_t length_;
	};

	struct float_bin_t {
		uint32_t fraction: 23;
		uint8_t  exponent: 8;
		bool     sign    : 1;
	} __attribute__ ((__packed__));

	inline auto signbit(const float &val) -> bool {
		auto *bin = reinterpret_cast<const float_bin_t*>(&val);
		return bin->sign;
	}

	template<typename Float>
	void float_to_string(Float val, string &str){
		size_t precision = 5;
		Float rounder = 0.5;
		for(size_t i=0;i<precision;i++){
			rounder /= 10.0;
		}

		str.reserve(10);

		if(signbit(val)){
			val = -1.0 * val;
			str += "-";
		}

		val += rounder;

		long int_part = val;
		val -= int_part;

		if(int_part == 0) str += '0';
		else{
			string str_int;
			while(int_part){
				str_int += '0' + int_part % 10;
				int_part /= 10;
			}
			str_int.reverse();
			str += str_int;
		}

		if(val == 0.0) return;
		str += ".";

		val *= 10.0;
		while(precision--){
			char c = val;
			val -= c;
			val *= 10.0;

			str += '0' + c;
		}

		return;
	}

	auto to_string(float val) -> string {
		string str;
		float_to_string(val, str);
		return str;
	}

	auto to_string(double val) -> string {
		string str;
		float_to_string(val, str);
		return str;
	}
}

#endif
