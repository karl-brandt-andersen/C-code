#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "printf.h"
#include "math.h"
#include "package.h"

RF24 radio(9, 10);
const uint64_t pipes[2] = { 0xA5A5A5A5D2LL, 0xA5A5A5A5E1LL };

//pin's
int ledRed = 8;
int ledBlue = 3;
int ledGreen = 2;
int m1RPwm = 6;
int m1LPwm = 5;
int m2RPwm = 7;
int m2LPwm = 3;
int xAxis = A0;
int yAxis = A1;

//var's
int error[4];
uint8_t but1;
int timeOne;
int timeTwo;


typedef enum { role_send = 1, role_receive } role_e;
role_e role = role_receive;

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

    Serial.print("Package.y: ");
    Serial.println(Package.y);
    Serial.print("Package.x: ");
    Serial.println(Package.x);
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

int engineFunc(int input) {
  if (input > 256) {
    input = map(input, 0, 1020, 0 , 255);
  }
  int output = input;

  if (output > (255 / 2) + 2) {
    analogWrite(m1RPwm, output);
    analogWrite(m1LPwm, 0);
    analogWrite(m2RPwm, output);
    analogWrite(m2LPwm, 0);
    //digitalWrite(ledGreen, HIGH);
  }
  else if (output < (255 / 2) - 2 && output > 0) {
    analogWrite(m2RPwm, 0);
    analogWrite(m1LPwm, output);
    analogWrite(m2RPwm, 0);
    analogWrite(m2LPwm, output);
    //digitalWrite(ledGreen, HIGH);
  }
  else if (output == -1) {
    analogWrite(m1RPwm, 0);
    analogWrite(m1LPwm, 0);
    analogWrite(m2RPwm, 0);
    analogWrite(m2LPwm, 0);
    //digitalWrite(ledGreen, LOW);
  }
  return (output);
}

int advEngine_funk(int input) {
  if (input > 256) {
    input = map(input, 0, 1020, 0 , 255);
  }

  int inputFunc = 0.55 * 10 ^ (-6) * input ^ 3 - 0.00045347 * input ^ 2 + 0.15439421 * input + 5;

  if (input > (255 / 2) + 2) {
    analogWrite(m1RPwm, inputFunc);
    analogWrite(m1LPwm, 0);
    analogWrite(m2RPwm, inputFunc);
    analogWrite(m2LPwm, 0);
    //digitalWrite(ledGreen, HIGH);
  }
  else if (input < (255 / 2) - 2 && input > 0) {
    analogWrite(m2RPwm, 0);
    analogWrite(m1LPwm, inputFunc);
    analogWrite(m2RPwm, 0);
    analogWrite(m2LPwm, inputFunc);
    //digitalWrite(ledGreen, HIGH);
  }
  else if (input == -1) {
    analogWrite(m1RPwm, 0);
    analogWrite(m1LPwm, 0);
    analogWrite(m2RPwm, 0);
    analogWrite(m2LPwm, 0);
    //digitalWrite(ledGreen, LOW);
  }
  return (inputFunc);
}

void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(m1RPwm, OUTPUT);
  pinMode(m1LPwm, OUTPUT);
  pinMode(m2RPwm, OUTPUT);
  pinMode(m2LPwm, OUTPUT);

  pinMode(xAxis, INPUT);
  pinMode(yAxis, INPUT);

  Serial.begin(9600);

  printf_begin();
  /*
    radioStart();
    systemFailCheck();

    if ( role == role_send ) {
    radio.stopListening();
    }
    else {
    radio.startListening();
    }
  */
  but1 = 0;
}


void loop() {
  systemFailCheckSerial(false);
  int i;
  i = analogRead(yAxis);

  if (i < 520 && i > 500) {
    Serial.print("Raw y input = ");
    Serial.print(i);
    Serial.print('\n');
    i = -1;
  }
  Serial.print("y input = ");
  Serial.println(i);
  Serial.print("engine out = ");
  Serial.println(engineFunc(i));
  Serial.print('\n');

  delay(400);
}
/*
  if (radio.available()) {
  while (radio.available()) {
    radio.read(&Package, sizeof(Package));
  }*/
