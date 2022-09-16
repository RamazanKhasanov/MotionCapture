#include <MPU6500.h>
#include <Wire.h>
#define TCAADDR 0x70
int tca=0;

void tcaselect(uint8_t i) { //функция, которая переключается между портами на плате TCA от 1-го до 7-го соответсвенно 
  if (i > 7) return; 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

MPU6500 mpu6500_3(Wire);//на порте 3
MPU6500 mpu6500_5(Wire); 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  tcaselect(3);
  Serial.print("MPU-6500 (3)");
  mpu6500_3.begin();
  mpu6500_3.calcGyroOffsets(true);
  tcaselect(5);
  Serial.println("");
  Serial.print("MPU-6500 (5)");
  mpu6500_5.begin();
  mpu6500_5.calcGyroOffsets(true);
  Serial.println("");
}

void loop() {
  if(tca==7) tca=0;
  tcaselect(tca);
  if(tca==3)
  {
    mpu6500_3.update();
    Serial.print(tca); Serial.print(" ");
    Serial.print(mpu6500_3.getAngleX());
    Serial.print(" ");
    Serial.print(mpu6500_3.getAngleY());
    Serial.print(" ");
    Serial.println(mpu6500_3.getAngleZ());
  }
  if(tca==5)
  {
    mpu6500_5.update();
    Serial.print(tca); Serial.print(" ");
    Serial.print(mpu6500_5.getAngleX());
    Serial.print(" ");
    Serial.print(mpu6500_5.getAngleY());
    Serial.print(" ");
    Serial.println(mpu6500_5.getAngleZ());
  } 
  tca++;
  delay(1);
}
