/*
GYSFDMAXBから位置情報を取得してシリアルで表示する．
*/
#include "mbed.h"
Serial gps(PA_9, PA_10, 9600); //pin19,20 TX,RX
Serial pc(PA_2, PA_3, 9600); //pin8,9 TX,RX
CAN can(PA_11, PA_12); //pin21,22 rd,td

char gps_data[256], senddata[5];
char ns, ew;
float time_g, lat_g, lon_g, lat, lon;
int ht, mt, st;//計算結果 時刻
int qop, sat;
int i;

union Float2Byte{
    float _float;
    char _byte[4];
};
typedef union Float2Byte Float2Byte;

void GetGPS(){
    if((gps_data[i]=gps.getc())!='$'){
        i++;
        if(i==255){
        pc.printf("Data read error.\n\r");
        return;
        }
    }
    else{
        gps_data[i] = '\0';
        sscanf(gps_data, "GPGGA,%f,%f,%c,%f,%c,%d,%d",
        &time_g, &lat_g, &ns, &lon_g, &ew, &qop, &sat);
        i = 0;
    }
}

void send(float value, char moji){
    senddata[4] = moji;
    Float2Byte sendFloat;
    sendFloat._float = value;
    for(int i=0;i<4;++i){
        senddata[i] = sendFloat._byte[i];
    }
    if(can.write(CANMessage(0x04, senddata, 5))){ //ID:0x04
        pc.printf("Send.\n\r");
    } 
}

int main(){
    gps.attach(GetGPS, Serial::RxIrq);
    pc.printf("start.\n\r");
    while(1){
        lat = lat_g / 100.0f;
        lon = lon_g / 100.0f;
        lat = (int)lat + (lat-(int)lat)*100.0f/60.0f;
        lon = (int)lon + (lon-(int)lon)*100.0f/60.0f;
        
        ht=int(time_g/10000);
        mt=int((time_g-ht*10000)/100);
        st=int(time_g-ht*10000-mt*100);
        ht=ht+9;//UTC =>JST
        
        send(lat, 'a');
        send(lon, 'o');
        pc.printf("%f, %f, %d, %d, %d \n\r", lat, lon, ht, mt, st);
        wait(0.1); 
    }
}