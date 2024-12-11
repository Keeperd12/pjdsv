#include <Wire.h>

#define I2C_SDL    D1
#define I2C_SDA    D2

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay (1000); 
}

  unsigned int old_anin0=0;
  unsigned int old_input=0;
  unsigned int old_i=0;
  unsigned int i=0;
  unsigned int outputs =0;
  
void loop() {
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03));          
  Wire.write(byte(0x01));         
  Wire.endTransmission();

  //read button
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x00));      
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1);   
  unsigned int input = Wire.read(); 
  //Serial.println(input);
  if(input != old_input){  
  if(input & 0x0F){
    Serial.println("pressed");
  }
  else{
    Serial.println("not pressed");
  }
  }
  old_input = input;

  //delay(1000);
  if (Serial.available() > 0) { // Check if data is available to read
    i = Serial.parseInt();
  }
  if (old_i != i){
if (i == 1) {
    outputs = 32;
}
if (i == 2) {
    outputs = 16;
}
if (i == 3) {
    outputs = 48;
}
if (i == 0) {
    outputs = 0;
}
  }
  old_i = i;


    //buzzer on
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x01));            
  Wire.write(byte(outputs));            
  Wire.endTransmission(); 
  //Serial.println(outputs);

  Wire.beginTransmission(0x36);
  Wire.write(byte(0xA2));          
  Wire.write(byte(0x03));  
  Wire.endTransmission(); 

  //Read analog 10bit inputs 0&1
  Wire.requestFrom(0x36, 4);   
  unsigned int anin0 = Wire.read()&0x03;  
  anin0=anin0<<8;
  anin0 = anin0|Wire.read(); 
  //Serial.println(anin0);      
  if (anin0 >= 523 && old_anin0 < 523){
    Serial.println("brand");
  }
  old_anin0 = anin0;
  delay(10); 

}
