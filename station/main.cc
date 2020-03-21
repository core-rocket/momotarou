#include <iostream>
#include "../TWE-Lite/TWE-Lite/TWE-Lite.hpp"

TWE_Lite *twe;

int main(int argc, char **argv){
	const std::string devfile = argv[1];
	twe = new TWE_Lite(devfile, 115200);

	std::cerr << "init" << std::endl;
	if(!twe->init()){
		std::cerr << "TWE-Lite initialize failed" << std::endl;
	}

	std::cerr << "start main loop" << std::endl;
	while(true){
		//std::cout << twe->sread8() << std::endl;
		const auto n = twe->recv();
		std::cout << n << ":";
		if(twe->is_simple())
			std::cout << "S:" << std::endl;
		else if(twe->is_response())
			std::cout << "R:" << std::endl;
		else{
			std::cout << "E:"
				<< "from=" << (int)twe->from_id()
				<< ",cmd=" << (int)twe->cmd_type()
				<< ",res=" << (int)twe->response_id()
				<< ",LQI=" << (int)twe->LQI() << std::endl;
		}
	}

	delete twe;
	return 0;
}
