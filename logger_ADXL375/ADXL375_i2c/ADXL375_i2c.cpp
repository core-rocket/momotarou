/*This file edit by sassy.
2019/10/25
degmyb.5534g@gmail.com
*/
#include "ADXL375_i2c.h"

ADXL375_i2c::ADXL375_i2c(PinName sda, PinName scl)
{
    i2c_ = new I2C(sda, scl);
    i2c_->frequency(400000);
}

//******************************************************************************
ADXL375_i2c::ADXL375_i2c(I2C *i2c):i2c_(i2c){}

//******************************************************************************
ADXL375_i2c::~ADXL375_i2c()
{    
   delete i2c_;
}

void ADXL375_i2c::writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
   char data_write[2];
   data_write[0] = subAddress;
   data_write[1] = data;
   i2c_->write(address, data_write, 2);
}

char ADXL375_i2c::readByte(uint8_t address, uint8_t subAddress)
{
    char data[1]; // `data` will store the register data     
    char data_write[1];
    data_write[0] = subAddress;
    i2c_->write(address, data_write, 1,1); // no stop
    i2c_->read(address, data, 1, 0); 
    return data[0]; 
}

void ADXL375_i2c::readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t* dest)
{     
    char data[14];
    char data_write[1];
    data_write[0] = subAddress;
    i2c_->write(ADXL375_I2C_LOW_WRITE, data_write, 1,1); // no stop
    i2c_->read(ADXL375_I2C_LOW_READ, data, count); 
    for(int ii = 0; ii < count; ii++) {
     dest[ii] = data[ii];
    }
}

void ADXL375_i2c::readAccelData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z accel register data stored here
  readBytes(ADXL375_I2C_LOW_READ, ADXL375_DATAX0_REG, 6, &rawData[0]);  // Read the six raw data registers into data array
  destination[0] = (int16_t)(((int16_t)rawData[1] << 8) | rawData[0]) ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)(((int16_t)rawData[3] << 8) | rawData[2]) ;  
  destination[2] = (int16_t)(((int16_t)rawData[5] << 8) | rawData[4]) ; 
}

void ADXL375_i2c::initADXL375()
{  
 // Initialize MPU9250 device
 //Measure Bit
 writeByte(ADXL375_I2C_LOW_WRITE, ADXL375_POWER_CTL_REG, 0x08);
 wait(0.1);

 //init DATA_FORMAT
 writeByte(ADXL375_I2C_LOW_WRITE, ADXL375_DATA_FORMAT_REG, 0x0B);
 wait(0.1);

  //init DATA_FORMAT
 writeByte(ADXL375_I2C_LOW_WRITE, ADXL375_BW_RATE_REG, 0x0D); //800Hz
 wait(0.1);
}

