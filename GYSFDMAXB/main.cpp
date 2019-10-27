/*
GYSFDMAXBから位置情報を取得してシリアルで表示する．
*/
#include "mbed.h"

DigitalOut myled(PB_1); //pin15
Serial gps(PA_2, PA_3, 9600); //pin8,9 TX,RX
Serial pc(PA_9, PA_10, 9600); //pin19,20 TX,RX

char gps_data[256];
float time_g, lat_g, lon_g;
char ns, ew;
int qop, sat;
float lat, lon;
int i;

void GetGPS(){
    if((gps_data[i]=gps.getc())!='$'){
        myled = 0;
        i++;
        if(i==255){
        pc.printf("Data read error.\n\r");
        return;
        }
    }
    else{
        myled = 1;
        gps_data[i] = '\0';
        sscanf(gps_data, "GPGGA,%f,%f,%c,%f,%c,%d,%d",
        &time_g, &lat_g, &ns, &lon_g, &ew, &qop, &sat);
        i = 0;
    }
}

int main(){
    gps.attach(GetGPS, Serial::RxIrq);
    while(1){
        lat = lat_g / 100.0f;
        lon = lon_g / 100.0f;
        lat = (int)(lat) + (lat-(int)lat)*100.0f/60.0f;
        lon = (int)(lon) + (lon-(int)lon)*100.0f/60.0f;
        pc.printf("%f, %f\n\r", lat, lon);
        wait(2.0);
    }
}