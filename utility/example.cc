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

	print_str(to_string(1.234f));
}
