#!/bin/bash
code=0
onFailure(){
	code=$?
}
trap onFailure ERR

#./build.sh 24LC1025
./build.sh ADXL375
./build.sh BME280
./build.sh GYSFDMAXB
./build.sh HX711
./build.sh MPU9250
./build.sh SDCard

exit $code
