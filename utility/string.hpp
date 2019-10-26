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

	string to_string(float val){
		string str;
		str.reserve(10);

		if(signbit(val)){
			val = -1.0 * val;
			str += "-";
		}

		return str;
	}

	void print_float(float val){
		if(signbit(val)){
			val = -1.0 * val;
			putchar('-');
		}

		long int_part = val;
		//printf("int part = %d\n", int_part);

		val -= int_part;	// 小数部
		if(val == 0.0) return;

		//printf("val = %f\n", val);

		putchar('.');
		val *= 10.0;
		for(int i=0;i<5;i++){
			char c = val;
			val -= c;
			val *= 10.0;

			putchar('0' + c);
			//printf("%c %f\n", '0' + c, val);
		}

		putchar('\n');
	}
}

#endif
