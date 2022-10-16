/*
Prototype that casts spells using compinations of movments with a 3-axis gyroscope (MPU6050)
displaying on a oled (SSD1306)using esp32

===Contact & Support===
Tutiorals and libaries for gyro sourced from 
Website: http://eeenthusiast.com/
Youtube: https://www.youtube.com/EEEnthusiast
Tutiorals on oled sourced from https://www.youtube.com/watch?v=nGjDSgCR-ck
*/


#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);


long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

int sCorrect[4] = {0, 0, 0, 0};


void setup() {
  
  pinMode(15, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  Serial.begin(115200);
  Wire.begin();
//  testDisplay();
  display.display(); 
  setupMPU();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

}

void loop() {

 int sensorVal = digitalRead(15);
  //print out the value of the pushbutton
  Serial.println(sensorVal);


for(;;){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("START BATTLE \n press button \n to continue");
     if (sensorVal == HIGH) break;
}

 
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("cast spell \n");
  spellCorrect();
  delay(3000);
}

void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}

void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}

void printData() {
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print(" Z=");
  Serial.print(rotZ);
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);
}

void testDisplay(){
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    ; // Don't proceed, loop forever
  }
}

void dirDisplay(){
    recordAccelRegisters();
    recordGyroRegisters();
    printData();
    
    int i = 0;
    
    if (rotX < 250 && rotY > 250 && rotZ > 250) {
      display.println("left");
      sCorrect[i] = 1;
    } else if (rotX > 250 && rotY < 250 && rotZ > 250) {
      display.println("right");
      sCorrect[i] = 2;
    } else if (rotX > 250 && rotY > 250 && rotZ < 250) {
      display.println("foward");
      sCorrect[i] = 3;
    } else {
        display.println("up");
        sCorrect[i] = 4;
    }
}

void spellCorrect() {
    for (int i = 1; i <= 4; ++i) {
      dirDisplay();
      delay(1000); 
      display.display(); 
    } 

    delay(3000);
    
    if (sCorrect[0] == 1
    &&  sCorrect[1] == 2
    &&  sCorrect[2] == 3
    &&  sCorrect[3] == 4){
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("\n \n   SPELL SUCESSFUL");
        display.display();
        delay(2000);
    }
      else{
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("\n \n");
          display.println("  SPELL FIZZlED");
          display.display();
          delay(2000);
      }
    }
      
# magicSword01
