#include <stdio.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>

//------------------------- SETTINGS

//sensor settings
const unsigned long dt = 50;                    //time resolution (ms)

//------------------------- SETTINGS

//comms variables
int tx_a_counter = 0;
int tx_b_counter = 0;
int rx_a_counter = 0;
int rx_b_counter = 0;
const int transmitLim = 3000 / dt;
const int receiveLim = 1000 / dt;

//PINS:
const int rx_received_pin       = 12;
const int rx_a_pin              = 2;
const int rx_b_pin              = 4;
const int tx_a_pin              = 3;
const int tx_b_pin              = 5;
const int pump_pin              = 13;


//sensor data
bool rx_received = false;
bool rx_a = false;
bool rx_b = false;
bool rx_c = false;
bool rx_d = false;

//states
bool pump_command = false;
bool pump_running = false;


void setup() {
  //------------------------- DEBUG
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  //------------------------- DEBUG

  //Pin setup
  pin_setup();
}

void loop() {
  //time div
  unsigned long t0 = millis();

  //------------------------- DEBUG
  //Serial.println((int)millis());
  //------------------------- DEBUG

  //Poll Sensors:
  sensorPoll();

  //pump logic
  pumpController();

  //comms
  transmit();
  receive();

  //wait until next time division
  while (millis() < (t0 + dt)){}
}

void pin_setup() {
  pinMode(pump_pin, OUTPUT);
  pinMode(rx_received_pin, INPUT);
  pinMode(rx_a_pin, INPUT);
  pinMode(rx_b_pin, INPUT);
  pinMode(tx_a_pin, INPUT);
  pinMode(tx_b_pin, INPUT);
}

void sensorPoll() {
  //radio RX
  rx_received = digitalRead(rx_received_pin);
  rx_a = digitalRead(rx_a_pin);
  rx_b = digitalRead(rx_b_pin);
}

void transmit() {
  if (rx_a) digitalWrite(tx_a_pin, HIGH);
  else      digitalWrite(tx_a_pin, LOW);

  if (rx_b) digitalWrite(tx_b_pin, HIGH);
  else      digitalWrite(tx_b_pin, LOW);
}

void receive(){
  if (rx_a){
    rx_a_counter++;  
    if (rx_a_counter > receiveLim){
      pump_command = true;  
    }
  }
  else {
    rx_a_counter = 0;
  }
  if (rx_b){
    rx_b_counter++;  
    if (rx_b_counter > receiveLim){
      pump_command = false;  
    }
  }
  else {
    rx_b_counter = 0;
  }
}

void pumpController(){
  if(pump_command){
    pump_running = true;
    digitalWrite(pump_pin, HIGH);
    //Serial.println("pump on");
  }
  else {
    pump_running = false;
    digitalWrite(pump_pin, LOW);
    //Serial.println("pump off");
  }
}

/*
  Serial.println("Current Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.println("Unix Time: ");
  Serial.print("elapsed ");
  Serial.print(now.unixtime());
  Serial.print(" seconds/");
  Serial.print(now.unixtime() / 86400L);
  Serial.println(" days since 1/1/1970");


*/
