#include "MPU6500.h"
#include "Arduino.h"

MPU6500::MPU6500(TwoWire &w){
	wire = &w;
	accCoef = 0.02f;
	gyroCoef = 0.98f;
}

MPU6500::MPU6500(TwoWire &w, float aC, float gC){
	wire = &w;
	accCoef = aC;
	gyroCoef = gC;
}

void MPU6500::begin(){
	writeMPU6500(MPU6500_SMPLRT_DIV, 0x00);
	writeMPU6500(MPU6500_CONFIG, 0x00);
	writeMPU6500(MPU6500_GYRO_CONFIG, 0x08);
	writeMPU6500(MPU6500_ACCEL_CONFIG, 0x00);
	writeMPU6500(MPU6500_PWR_MGMT_1, 0x01);
	this->update();
	angleGyroX = 0;
	angleGyroY = 0;
  angleX = this->getAccAngleX();
  angleY = this->getAccAngleY();
  preInterval = millis();
}

void MPU6500::writeMPU6500(byte reg, byte data){
	wire->beginTransmission(MPU6500_ADDR);
	wire->write(reg);
	wire->write(data);
	wire->endTransmission();
}

byte MPU6500::readMPU6500(byte reg) {
	wire->beginTransmission(MPU6500_ADDR);
	wire->write(reg);
	wire->endTransmission(true);
	wire->requestFrom((uint8_t)MPU6500_ADDR, (size_t)2);
	byte data =  wire->read();
	wire->read();
	return data;
}

void MPU6500::setGyroOffsets(float x, float y, float z){
	gyroXoffset = x;
	gyroYoffset = y;
	gyroZoffset = z;
}

void MPU6500::calcGyroOffsets(bool console){
	float x = 0, y = 0, z = 0;
	int16_t rx, ry, rz;

  delay(1000);
	if(console){
    Serial.println();
    Serial.println("========================================");
    Serial.println("calculating");
	Serial.print("DO NOT MOVE DEVICE");
	}
	for(int i = 0; i < 3000; i++){
		if(console && i % 1000 == 0){
			Serial.print(".");
		}
		wire->beginTransmission(MPU6500_ADDR);
		wire->write(0x3B);
		wire->endTransmission(false);
		wire->requestFrom((int)MPU6500_ADDR, 14, (int)true);

		wire->read() << 8 | wire->read();
		wire->read() << 8 | wire->read();
		wire->read() << 8 | wire->read();
		wire->read() << 8 | wire->read();
		rx = wire->read() << 8 | wire->read();
		ry = wire->read() << 8 | wire->read();
		rz = wire->read() << 8 | wire->read();

		x += ((float)rx) / 65.5;
		y += ((float)ry) / 65.5;
		z += ((float)rz) / 65.5;
	}
	gyroXoffset = x / 3000;
	gyroYoffset = y / 3000;
	gyroZoffset = z / 3000;

	if(console){
		Serial.println();
		Serial.println("Done!!!");
		Serial.print("X : ");Serial.println(gyroXoffset);
		Serial.print("Y : ");Serial.println(gyroYoffset);
		Serial.print("Z : ");Serial.println(gyroZoffset);
		Serial.println("Program will start after 3 seconds");
		Serial.print("========================================");
		delay(3000);
	}
}

void MPU6500::update(){
	wire->beginTransmission(MPU6500_ADDR);
	wire->write(0x3B);
	wire->endTransmission(false);
	wire->requestFrom((int)MPU6500_ADDR, 14, (int)true);

	rawAccX = wire->read() << 8 | wire->read();
	rawAccY = wire->read() << 8 | wire->read();
	rawAccZ = wire->read() << 8 | wire->read();
	rawTemp = wire->read() << 8 | wire->read();
	rawGyroX = wire->read() << 8 | wire->read();
	rawGyroY = wire->read() << 8 | wire->read();
	rawGyroZ = wire->read() << 8 | wire->read();

	temp = (rawTemp + 12412.0) / 340.0;

	accX = ((float)rawAccX) / 16384.0;
	accY = ((float)rawAccY) / 16384.0;
	accZ = ((float)rawAccZ) / 16384.0;

	angleAccX = atan2(accY, accZ + abs(accX)) * 360 / 2.0 / PI;
	angleAccY = atan2(accX, accZ + abs(accY)) * 360 / -2.0 / PI;

	gyroX = ((float)rawGyroX) / 65.5;
	gyroY = ((float)rawGyroY) / 65.5;
	gyroZ = ((float)rawGyroZ) / 65.5;

	gyroX -= gyroXoffset;
	gyroY -= gyroYoffset;
	gyroZ -= gyroZoffset;

	interval = (millis() - preInterval) * 0.001;

	angleGyroX += gyroX * interval;
	angleGyroY += gyroY * interval;
	angleGyroZ += gyroZ * interval;

	angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
	angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
	angleZ = angleGyroZ;

	preInterval = millis();

}

