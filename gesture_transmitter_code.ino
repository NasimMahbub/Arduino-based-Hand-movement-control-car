#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Wire.h"
#include "MPU6050.h"

#define CSN  8
#define CE 7


RF24 radio(CE, CSN);                         
MPU6050 accelgyro;

int16_t ax, ay, az; 
int16_t gx, gy, gz; 
#define TX_ENABLE_KEY     2
#define TX_LED            3

const byte Address[6] = "00001";
int Pot_Val_Y = 0,Pot_Val_X = 0, Up_key = 0, Dn_key = 0, Left_key = 0, Right_key = 0;
unsigned char Tx_command = 0,Speed_index = 0,Tx_Enable_Flag = 0,TX_Key_Pressed = 0;
unsigned char Tx_Array[2];



void setup() {
  Serial.begin(115200);
  pinMode(TX_ENABLE_KEY, INPUT_PULLUP);
  pinMode(TX_LED, OUTPUT);
  
  radio.begin();
  radio.openWritingPipe(Address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  radio.write(&Tx_command, sizeof(Tx_command));

  Wire.begin();
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  Tx_Array[0] = 0;
  Tx_Array[1] = 0;
  
}

void loop()
{
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); 

  if(!(PIND & 0x04)&&(TX_Key_Pressed==0))
  {
      TX_Key_Pressed = 1;
      if(Tx_Enable_Flag==0)
      {
        Tx_Enable_Flag = 1;
        PORTD |= 0x08;  
      }
      else
      {
        Tx_Enable_Flag = 0;
        PORTD &= 0xF7;  
      }
      Serial.println("Pressed");
  }
  else if((PIND & 0x04)&&(TX_Key_Pressed==1))
  {
    TX_Key_Pressed = 0;
    Serial.println("Released");
  }

  if((ay<=-4000)||((ay>=4000)))
  {
    if((ax>=-4000)||((ax<=4000)))
    {
      Serial.print("B , ");
      if((ay<=-4000))
      {
         
          Tx_command = 1;                     
          Speed_index = (ay + 4000)/-2000 + 1;
          if(Speed_index>5)
          {
            Speed_index = 5;  
          }
      }

      if((ay>=4000))
      {
         
          Tx_command = 2;                    
          Speed_index = (ay - 4000)/2000 + 1; 
          if(Speed_index>5)
          {
            Speed_index = 5;  
          }
      }
    }
    else
    {
       
        Tx_command = 0;
        Speed_index = 0;
    } 
  }
  else if((ax<=-4000)||((ax>=4000)))
  {
    if((ay>=-4000)||((ay<=4000)))
    {

      if((ax<=-4000))
      {

          Tx_command = 4;                     
          Speed_index = (ax + 4000)/-2000 + 1;
          if(Speed_index>5)
          {
            Speed_index = 5;  
          }
      }

      if((ax>=4000))
      {

          Tx_command = 3;                    
          Speed_index = (ax - 4000)/2000 + 1;
          if(Speed_index>5)
          {
            Speed_index = 5;  
          }
      }
    }
    else
    {
        
        Tx_command = 0;
        Speed_index = 0;
    } 
  }
  else
  {

      Tx_command = 0;
      Speed_index = 0;
  }
  Serial.print(Tx_command);
  Serial.print(" , ");
  Serial.println(Speed_index);
  if(Tx_Enable_Flag)
  {
    Tx_Array[0] = Tx_command;
    Tx_Array[1] = Speed_index;
    radio.write(&Tx_Array, 2); 
  }
  delay(100);
}
