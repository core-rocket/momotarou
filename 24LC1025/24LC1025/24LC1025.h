#include "mbed.h"
#include "math.h"
// 24LC64 の書き込み、読み込みテスト

#define ADDRESS_24LC1025_BLOCK0  0xA0 //B0=0,A0=0,A1=0 : 10100000
#define ADDRESS_24LC1025_BLOCK1  0xA8 //B0=1,A0=0,A1=0 : 10101000

class LC1025{

  public:
    LC1025(PinName sda, PinName scl);
    LC1025(I2C *i2c);
    ~LC1025();

    void writeByte(uint8_t address, uint16_t subAddress, uint8_t data);
    void PageWrite(uint8_t address, uint16_t subAddress, char *data, int num);
    char RandomRead(uint8_t address, uint16_t subAddress);
    char SequentialRead(uint8_t address, uint16_t subAddress);
    void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest);

  private:
         I2C *i2c_; 
};