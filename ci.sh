#!/bin/bash
code=0
onFailure(){
	code=$?
}
trap onFailure ERR

#parallel ./build.sh {} ::: ADXL375 BME280 GYSFDMAXB HX711 MPU9250 SDCard

./build.sh ADXL375
./build.sh BME280
./build.sh GYSFDMAXB
./build.sh HX711
./build.sh MPU9250
./build.sh SDCard

#./build.sh 24LC1025

exit $code
