#include "mbed.h"
#include "TWE-Lite/TWE-Lite.hpp"

Serial pc(PA_2, PA_3, 115200); //pin8,9 TX,RX

int main(){
    pc.printf("start TWE-Lite.\n\r");

	TWE_Lite twe(PA_9, PA_10, 115200);

	twe.init();
	twe.send_simple(0x00, 0x00, "hoge");
	const auto ret = twe.check_send();
	pc.printf("ret: %d\n\r", ret);
}
