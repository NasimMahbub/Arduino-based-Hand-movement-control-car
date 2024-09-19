#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define CSN 8
#define CE 7

#define R1 5
#define R2 4
#define L1 3
#define L2 2


RF24 radio(CE, CSN);

const byte Address[6] = "00001";
unsigned char Received_Command = 0, Speed_index = 0, stopmode = 0;
unsigned char Rx_Array[2];
unsigned int stop_count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  radio.begin();
  radio.openReadingPipe(0, Address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("START");
}

void loop()
{
  if (radio.available())
  {
    delay(1);

    radio.read(&Rx_Array, 2);
    Received_Command = Rx_Array[0];
    Speed_index = Rx_Array[1];
    Serial.print(Received_Command);
    Serial.print(" , ");
    Serial.println(Speed_index);
  }

  if (stopmode == 0)
  {
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
    digitalWrite(L1, LOW);
    digitalWrite(L2, LOW);
    stop_count++;
    if (stop_count >= ((5 - Speed_index) * 100))
    {
      stop_count = 0;
      stopmode = 1;
    }
  }
  else if (stopmode == 1)
  {
    stop_count++;
    if (stop_count >= ((Speed_index) * 100))
    {
      stop_count = 0;
      stopmode = 0;
    }

    if (Received_Command == 1)
    {
      digitalWrite(L1, HIGH);
      digitalWrite(R1, HIGH);
    }
    else if (Received_Command == 2)
    {
      digitalWrite(L2, HIGH);
      digitalWrite(R2, HIGH);
    }
    else if (Received_Command == 3)
    {
      digitalWrite(L1, HIGH);
      digitalWrite(R2, HIGH);
    }
    else if (Received_Command == 4)
    {
      digitalWrite(R1, HIGH);
      digitalWrite(L2, HIGH);
    }
    else if (Received_Command == 0)
    {
      digitalWrite(R1, LOW);
      digitalWrite(R2, LOW);
      digitalWrite(L1, LOW);
      digitalWrite(L2, LOW);
    }
    else
    {
      digitalWrite(R1, LOW);
      digitalWrite(R2, LOW);
      digitalWrite(L1, LOW);
      digitalWrite(L2, LOW);
    }
  }

}
