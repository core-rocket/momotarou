#include <cstddef>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
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

	size_t loop_count = 0;
	size_t recv_count = 0;
	size_t recv_bytes = 0;
	uint8_t LQI;

	std::thread t([&]{
		using namespace std::chrono;
		size_t old_recv_bytes = 0;
		while(true){
			const auto start = system_clock::now();
			const auto rb = recv_bytes;
			const auto recv = rb - old_recv_bytes;
			old_recv_bytes = rb;

			const auto pdbm = (7*LQI-1970)/20;
			std::cout << std::fixed << std::setprecision(2)
				<< pdbm << "dBm("
				<< (LQI >= 150 ? "sgd" : (LQI >= 100 ? "god" : (LQI >= 50 ? "soo" : "bad")))
				<< ")  "
				<< (float)recv*8/1000.0 << "kbps  "
				<< (float)rb / 1024.0 << "KiB" << std::endl;

			const auto end = system_clock::now();
			const auto elapsed_ms = duration_cast<milliseconds>(end - start).count();
			std::this_thread::sleep_for(milliseconds(1000 - elapsed_ms));
		}
	});

	while(true){
		loop_count++;
		const auto n = twe->recv();
		if(n == 0) continue;
		if(twe->is_simple())
			std::cout << "S:" << std::endl;
		else if(twe->is_response())
			std::cout << "R:" << std::endl;
		else{
//			std::cout << "E:"
//				<< "from=" << (int)twe->from_id()
//				<< ",cmd=" << (int)twe->cmd_type()
//				<< ",res=" << (int)twe->response_id()
//				<< ",LQI=" << (int)twe->LQI() << std::endl;

			LQI = twe->LQI();

			recv_bytes += twe->get_length();
			recv_count++;
		}
	}

	delete twe;
	return 0;
}
