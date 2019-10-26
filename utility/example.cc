#include <iostream>
#include "string.hpp"

int main(int argc, char **argv){
	using namespace utility;

	auto print_str = [](const string &s){
		std::cout
			<< "\"" << s.c_str() << "\""
			<< " (length = " << s.length() << ")"
			<< std::endl;
	};

	string s = "hell ";
	print_str(s);
	s[4] = 'o';
	print_str(s);

	s += ", friend";
	print_str(s);

	print_str(to_string(1.0f));
	print_str(to_string(-2.0f));
	print_str(to_string(1.234f));
	print_str(to_string(123456789.0f));
	print_str(to_string(12345678.9f));
	print_str(to_string(1234567.89f));
	print_str(to_string(123456.789f));
	print_str(to_string(12345.6789f));
	print_str(to_string(1234.56789f));
	print_str(to_string(123.456789f));
	print_str(to_string(12.3456789f));
	print_str(to_string(1.23456789f));
	print_str(to_string(0.1f));
	print_str(to_string(0.01f));
	print_str(to_string(0.001f));
	print_str(to_string(0.0001f));
	print_str(to_string(0.00001f));
	print_str(to_string(0.000001f));
	print_str(to_string(0.0000001f));
}
