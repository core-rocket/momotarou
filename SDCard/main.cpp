/*
mbedCLIでcompileすると容量大きくて書き込めないのでオンラインコンパイラでcompileしてる．
(floatでSDに書き込みたい時)

SDカードに整数を書き込む．
<pin接続>
NSS: pin25 --- pin2 CD/DAT3
SCK: pin26 --- pin5 CLK
MISO:pin27 --- pin7 DAT0
MOSI:pin28 --- pin3 CMD
*/
#include "mbed.h"
#include "SDFileSystem.h"

Serial pc(PA_9, PA_10, 115200); //pin19,20 TX,RX
SDFileSystem sd = SDFileSystem(PA_7, PA_6, PA_5, PA_4, "sd");
CAN can(PA_11, PA_12, 1000000); //pin21,22 rd,td

CANMessage msg;

int buff = -1;

union Float2Byte{
    float _float;
    char _byte[4];
}f2b;

int main(){
    wait(0.1); //気持ち
    pc.printf("Start.\n\r");
    FILE *fp = fopen("/sd/log.txt", "a");
    if(fp != NULL){
        fprintf(fp, "Start.\n");
        fclose(fp);
    }
    else{
        pc.printf("Failed.\n\r");
    }
    
    while(1){
        FILE *fp = fopen("/sd/log.txt", "a");
        int cnt = 0;
        if(fp != NULL){
            while(1){
                if(can.read(msg)){
                    //moji nashi
                    if(msg.id == 3 || msg.id == 4 || msg.id == 10 || msg.id == 11 || msg.id == 12){
                        //printf(fp, "%d,%d,%d,%d,%d\n", msg.id, msg.data[1], msg.data[2], msg.data[3], msg.data[4]);
                        
                        for(int i=0;i<4;++i){
                            f2b._byte[i] = msg.data[i];
                        }
                        fprintf(fp, "%d,%f\n", msg.id, f2b._float);             
                        cnt++;
                    }
                    //moji ari
                    else if(msg.id == 5 || msg.id == 7 || msg.id == 8 || msg.id == 9 || msg.id == 13){
                        //fprintf(fp, "%d,%d,%d,%d,%d,%c\n", msg.id, msg.data[1], msg.data[2], msg.data[3], msg.data[4], msg.data[0]);
                        
                        for(int i=1;i<5;++i){
                            f2b._byte[i] = msg.data[i];
                        }
                        fprintf(fp, "%d,%f,%c\n", msg.id, f2b._float, msg.data[0]);
                        cnt++;
                    }
                    //1byte
                    else if(msg.id == 1 || msg.id == 2){
                        fprintf(fp, "%d,%d\n", msg.id, msg.data[0]);
                        cnt++;
                    }
                    //phase
                    else if(msg.id == 6){
                        if(buff != msg.data[0]){
                            fprintf(fp, "%d,%d\n", msg.id, msg.data[0]);
                            buff = msg.data[0];
                        }
                    }
                }
                if(cnt == 2000){
                    fclose(fp);
                    //pc.printf("Save.\n\r");
                    break;
                }
            }
        }
        else{
            pc.printf("Failed.\n\r");
        }
    }
}