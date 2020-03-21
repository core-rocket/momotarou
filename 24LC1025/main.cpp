#include "mbed.h"
#include "24LC1025.h"

Serial pc(PA_9, PA_10, 9600);

LC1025 eeprom = LC1025(PB_7, PB_6);

int main()
{
  pc.printf("Hello OSHIMA\r\n");
  char x;
  char a[1000];
  // 書き込む場合
 
        for(uint16_t i=0x0000; i<0x0008; i++)
        {
            a[0x0000] = 'T';
            a[0x0001] = 'A';
            a[0x0002] = 'K';
            a[0x0003] = 'E';
            a[0x0004] = 'Y';
            a[0x0005] = 'U';
            a[0x0006] = 'K';
            a[0x0007] = 'I';
            eeprom.writeByte(ADDRESS_24LC1025_BLOCK0, i ,a[i]);
            pc.printf("%c:0x%x\r\n",a[i],a[i]);
            wait(0.1);
            
        }
 
 // 読み込む場合
        for(uint16_t i=0x0000; i<0x0008; i++)
        {
            x = eeprom.RandomRead(ADDRESS_24LC1025_BLOCK0, i);
            pc.printf("%04x,%c ",i,x);
            wait(0.1);
        }
        pc.printf("\r\n");
}