#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "printf.h"
#include <math.h> 
#include "package.h"

RF24 radio(9, 10);
const uint64_t pipes[2] = { 0xA5A5A5A5D2LL, 0xA5A5A5A5E1LL };

//pin's
int joy1x = A0;
int joy1y = A1;

int ledRed = 8;
int ledBlue = 3;
int ledGreen = 2;

//var's
int error[4];
uint8_t but1;

typedef enum { role_send = 1, role_receive } role_e;
role_e role = role_send;

struct package
{
  int x;
  int y;
} Package;

int radioStart() {
  //radio start funktion
  radio.begin();

  //radio configuration
  radio.setChannel(10);
  radio.setPALevel(RF24_PA_HIGH); //MIN?
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15, 5);
  radio.setPayloadSize(32); // skal måske rettes

  //radio role config
  if ( role == role_send )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);
  }

  //radio set receive
  radio.startListening();

}

int systemFailCheckSerial(bool x) {
  if (x == true) {
    Serial.begin(57600);

    radio.stopListening();
    if (!radio.isChipConnected() /*Is chip connected to MCU, SPI bus?*/) {
      Serial.print("NRF24LO1 Spi: ");
      Serial.println(radio.isChipConnected());
      Serial.print('\n');
    }
    radio.startListening();

    Serial.print("yPin: ");
    Serial.println(analogRead(joy1y));
    Serial.print("xPin: ");
    Serial.println(analogRead(joy1x));
    Serial.print('\n');
    Serial.end();
  }
}

int systemFailCheck() {
  //blueled = radio fail
  //redled = something else failed
  //greenled = good to go
  Serial.begin(57600);

  int led[] = {ledRed, ledBlue, ledGreen};
  error[0, 1, 2, 3] = 0;

  radio.stopListening();
  if (!radio.isChipConnected() /*Is chip connected to MCU, SPI bus?*/) {
    error[0] = 1;
  }
  radio.printDetails();
  radio.startListening();

  if (error[0] == 1) {
    digitalWrite(led[1], HIGH);
  }
  else if (error[0, 1, 2, 3] != 1) {
    digitalWrite(led[2], HIGH);
  }
  Serial.end();
}

void setup() {
  pinMode(joy1x, INPUT);
  pinMode(joy1y, INPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  printf_begin();
  radioStart();
  systemFailCheck();

  if ( role == role_send ) {
    radio.stopListening();
  }
  else {
    radio.startListening();
  }

  but1 = 0;
  Package.x = 0;
  Package.y = 0;
}


void loop() {
  systemFailCheckSerial(false);

  constrain(Package.y & Package.x, 0, 1024);
  Package.x = analogRead(joy1x);
  Package.y = analogRead(joy1y);

  radio.write(&Package, sizeof(Package), 0);
  delay(200);
}
