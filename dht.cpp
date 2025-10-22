#include "dht.hpp"
#include <Arduino.h>

unsigned long start, elapsed;

DHT::DHT(int pin, enum DHT_Type dht_typ) {
  _pin = pin;
  _typ = dht_typ;
}

void DHT::begin() {
  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH);
}

byte DHT::read() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(1);

  digitalWrite(_pin, HIGH);
  delayMicroseconds(40);

  pinMode(_pin, INPUT);

  start = micros();
  while (digitalRead(_pin) == HIGH) {
    if (micros() - start > 1000) {
      return 0;
    }

    if (digitalRead(_pin) == LOW) {
      delayMicroseconds(80);
      if (digitalRead(_pin) == HIGH) {
        delayMicroseconds(80);
        for (int i = 0; i < 5; ++i) {
          _data[i] = readByte();
        }
      }
    }
  }

  return _data[2];
}

byte DHT::readByte() {
  byte value = 0;

  for (int i = 0; i < 8; ++i) {
    while (digitalRead(_pin) == LOW)
      ;
    delayMicroseconds(50);

    if (digitalRead(_pin) == HIGH) {
      value = value | (1 << (7 - i));
    }

    while (digitalRead(_pin) == HIGH)
      ;
  }

  return value;
}
