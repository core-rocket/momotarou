#include "Hx711.h"

HX711::HX711(PinName pin_dout, PinName pin_slk): _pin_dout(pin_dout), _pin_slk(pin_slk)
{
    _pin_slk = 1;
    wait_ms(100);
    _pin_slk = 0;
    
    set_gain(128);
}

HX711::~HX711()
{

}


void HX711::set_gain(char gain)
{
    switch (gain) {
        case 128:       // channel A, gain factor 128
            GAIN = 1;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 3;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 2;
            break;
    }
    _pin_slk = 0;
    read();
}

long HX711::read()
{
    long value = 0;

    while (_pin_dout)
        ;

    for (int i=0;i<24;i++)
    { 
        _pin_slk = 1;
        value=value<<1; 
        _pin_slk = 0;
        if(_pin_dout) value++; 
    } 

    _pin_slk = 1;
    _pin_slk = 0;

    value=value^0x800000;

    return value;
}

long HX711::read_average(char times)
{
    long sum = 0;
    for (char i = 0; i < times; i++) {
        sum += read();
    }
    return sum / times;
}

double HX711::get_value(char times)
{
    return read_average(times) - OFFSET;
}

float HX711::get_units(char times)
{
    return get_value(times) / SCALE;
}

void HX711::tare(char times)
{
    double sum = read_average(times);
    set_offset(sum);
}

void HX711::set_scale(float scale)
{
    SCALE = scale;
}

void HX711::set_offset(long offset)
{
    OFFSET = offset;
}

void HX711::power_down()
{
  _pin_slk = 0;
  _pin_slk = 1;
}

void HX711::power_up()
{
  _pin_slk = 0;
}