#include "mbed.h"
DigitalOut myled(PB_1);
Serial gps(PA_2, PA_3);
Serial pc(PA_9, PA_10);
char gps_data[256];
float gpstime, gpsido, gpskeido;
char ns, ew;
int gps_i;
int AmountSatelite;
int dummy;
float lat, lon;

void GPS(){
    if((gps_data[gps_i]=gps.getc())!='$'){
        printf("%c", gps_data[gps_i]);
        gps_i++;
        if(gps_i==255){
        printf("%c", gps_data[gps_i]);
        return;
        }
    }
    else{
        gps_data[gps_i] = '\0';
        if(sscanf(gps_data, "GPGGA,%f,%f,%c,%f,%c,%d,%d",
          &gpstime, &gpsido, &ns, &gpskeido, &ew,&dummy, &AmountSatelite)>=1){
            if(AmountSatelite>=1){
                myled = 1;
            }
        }
        gps_i = 0;
        sprintf(gps_data, "");
    }
}

int main(){
    gps.baud(9600);
    gps.attach(GPS, Serial::RxIrq);
    while(1){
        lat = gpsido / 100.0f;
        lon = gpskeido / 100.0f;
        lat = (int)(lat) + (lat-(int)lat)*100.0f/60.0f;
        lon = (int)(lon) + (lon-(int)lon)*100.0f/60.0f;
        pc.printf("%f, %f\n\r", lat, lon);
        wait(0.5);
    }
}