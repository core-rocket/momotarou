#include "24LC1025.h"

/*
ref.http://akizukidenshi.com/download/I-02525_24lc1025.pdf
Created by sassy,CORE
*/

//******************************************************************************
LC1025::LC1025(PinName sda, PinName scl)
{
    i2c_ = new I2C(sda, scl);
    i2c_->frequency(400000);
}

//******************************************************************************
LC1025::LC1025(I2C *i2c):i2c_(i2c){}

//******************************************************************************
LC1025::~LC1025()
{    
   delete i2c_;
}

void LC1025::writeByte(uint8_t address, uint16_t subAddress, uint8_t data)
{
   char data_write[3];
   data_write[0] = (subAddress >> 8) & 0xff ;
   data_write[1] = (subAddress) & 0xff ;
   data_write[2] = data;
   i2c_->write(address, data_write, 3);
}

void LC1025::PageWrite(uint8_t address, uint16_t subAddress, char *data, int num)
{
   char data_write[num+2];
   data_write[0] = (subAddress >> 8) & 0xff ;
   data_write[1] = (subAddress) & 0xff ;
   for(int i = 0; i < num; i++){
        data_write[i+2] = data[i];
   }
   i2c_->write(address, data_write, num+2);
}

char LC1025::RandomRead(uint8_t address, uint16_t subAddress)
{
    char data[1]; // `data` will store the register data     
    char data_write[2];
    data_write[0] = (subAddress>>8) & 0xff;
    data_write[1] = subAddress & 0xff;
    i2c_->write(address, data_write, 2, true);
    i2c_->read(address+1, data, 1, true);
    i2c_->stop();
    return data[0]; 
}

char LC1025::SequentialRead(uint8_t address, uint16_t subAddress)
{
    char data[1]; // `data` will store the register data     
    char data_write[2];
    data_write[0] = (subAddress>>8) & 0xff;
    data_write[1] = subAddress & 0xff;
    i2c_->write(address, data_write, 2, true);
    i2c_->read(address+1, data, 0xffff, true);
    i2c_->stop();
    return data[0];
}

void LC1025::readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{     
    char data[14];
    char data_write[1];
    data_write[0] = subAddress;
    i2c_->write(address, data_write, 1, true); // no stop
    i2c_->read(address, data, count, 0); 
    for(int ii = 0; ii < count; ii++) {
     dest[ii] = data[ii];
    }
} 