/*
GYSFDMAXBから位置情報・時刻を取得してCAN送信・シリアル送信．
*/
#include "mbed.h"
Serial gps(PA_9, PA_10, 9600); // pin19,20 TX,RX
Serial pc(PA_2, PA_3, 115200); // pin8,9 TX,RX
CAN can(PA_11, PA_12, 1000000); // pin21,22 rd,td

char gps_data[256], senddata_2[9], senddata[5];
char gps_buf[15];
char ns, ew;
float time_g, lat_g, lon_g, lat, lon;
float ht, mt, st;
int qop, sat;

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

// コマンドを送信する
void send_cmd(const char *cmd){
    uint8_t checksum = 'P' ^ 'M' ^ 'T' ^ 'K';
    for(size_t i=0;;i++){
        if(cmd[i] == '\0') break;
        checksum = checksum ^ cmd[i];
    }
    char buf[100];
    sprintf(buf, "$PMTK%s*%X\r\n", cmd, checksum);
    gps.printf(buf);
}

// ボードレート変更コマンドを送信する
void change_brate(const size_t &brate){
    char cmd[20];
    sprintf(cmd, "251,%u", brate);
    send_cmd(cmd);
}

bool parse8(const char c){
    static uint8_t header_count = 0;
    static uint8_t count = 0;
    static uint8_t read_num = 0;

    switch(c){
    case '$': // start
        header_count = 0;
        count = 0;
        read_num = 0;
        break;
    case 'G':
        if(header_count == 0)
            header_count++;
        else{
            gps_buf[count] = c;
            count++;
        }
        break;
    case 'P':
        if(header_count == 1)
            header_count++;
        else{
            gps_buf[count] = c;
            count++;
        }
        break;
    case ',':
    case '*':
        gps_buf[count] = '\0';
        count = 0;
        //parse_data(buf);
        pc.printf("GPS: %s\r\n", gps_buf);
        if(gps_buf[0] == 'G' && gps_buf[1] == 'G' && gps_buf[2] == 'A'){
            sscanf(gps_data, "GGA,%f,%f,%c,%f,%c,%d,%d",
              &time_g, &lat_g, &ns, &lon_g, &ew, &qop, &sat);
        }
        read_num++;
        break;
    case '\n':
        return true;
    default:
        gps_buf[count] = c;
        count++;
        break;
    }
    return false;
}

void GetGPS(){
    static int j=0;
    if((gps_data[j]=gps.getc()) != '$'){
        j++;
        if(j == 255){
            pc.printf("Data read error.\n\r");
        return;
        }
    }
    else{
        gps_data[j] = '\0';
        sscanf(gps_data, "GPGGA,%f,%f,%c,%f,%c,%d,%d",
        &time_g, &lat_g, &ns, &lon_g, &ew, &qop, &sat);
        j = 0;
    }
}

void send(int id, float value, char moji){
    senddata[0] = moji;
    f2b._float = value;
    for(int i=1;i<5;++i){
        senddata[i] = f2b._byte[i];
    }
    CANMessage msg(id, senddata, 5);
    if(can.write(msg)){
        pc.printf("%d,%c\n\r", id, moji);
    }
}

int main(){
    wait(0.1);
    change_brate(115200);
    wait(1.0);
    Serial gps(PA_9, PA_10, 115200);
    gps.attach(GetGPS, Serial::RxIrq);
    
    pc.printf("start.\n\r");
    while(1){
        lat = lat_g / 100.0f;
        lon = lon_g / 100.0f;
        lat = (int)lat + (lat-(int)lat)*100.0f/60.0f;
        lon = (int)lon + (lon-(int)lon)*100.0f/60.0f;
        
        //ht = time_g/10000.0f;
        //mt = (time_g-ht*10000.0f)/100.0f;
        //st = time_g-ht*10000.0f - mt*100.0f;
        //ht = ht + 9; // UTC->JST
        
        pc.printf("%f, %f, %f\n\r", lat, lon, time_g);
        send(0x06, lat, 'a');
        send(0x06, lon, 'o');
        send(0x06, time_g, 't');
        //wait(0.1);
    }
}