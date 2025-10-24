#include "DHT_N128.hpp"

#define MIN_INTERVAL_MILLIS 2000

#define TRANSMISSION_TIMEOUT_MICROS 50

#define CYCLES_TIMEOUT UINT32_MAX
// "at least 18ms"
#define DHT11_FIRST_DELAY_MICROS 18500
// "1~10ms"
#define DHT22_FIRST_DELAY_MICROS 1100

DHT::DHT(int pin, enum DHT_Type dht_typ) {
  _pin = pin;
  _typ = dht_typ;

  _pullTime = DEFAULT_PULL_TIME;
  _maxcycles = microsecondsToClockCycles(1000);
}

void DHT::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

void DHT::_startSignal() {
  pinMode(_pin, INPUT_PULLUP);
  delay(1);

  // Dependiendo del sensor esperamos un tiempo determinado en LOW
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  switch (_typ) {
  case DHT22:
    delayMicroseconds(DHT22_FIRST_DELAY_MICROS);
    break;
  
  case DHT11:
  default:
    delayMicroseconds(DHT11_FIRST_DELAY_MICROS);
  }

  // Termina la señal de inicio y damos lugar a la respuesta del sensor
  digitalWrite(_pin, LOW);
  pinMode(_pin, INPUT_PULLUP);
  delayMicroseconds(_pullTime);   
}

raw_magnitude DHT::rawRead() {
  _clear();

  _startSignal();

  if (_awaitPulse(LOW) == CYCLES_TIMEOUT) {
    return CYCLES_TIMEOUT;
  }
  if (_awaitPulse(HIGH) == CYCLES_TIMEOUT) {
    return CYCLES_TIMEOUT;
  }

  for (int i = 0; i < 40; ++i) {
    _bits[i] = _readBit();
  }

  Serial.println("Printing bits:");
  for (int i = 0; i < 40; ++i) {
    Serial.print(_bits[i]);
    if ((i + 1) % 4 == 0 && i != 39) {
      Serial.print(" ");
    }
  }
  Serial.println();

  for (int i = 0; i < 5; ++i) {
      _data[i] = 0;
  }

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 8; ++j) {
        _data[i] |= (static_cast<uint8_t>(_bits[i * 8 + j]) << (7 - j));
      }
  }

  Serial.println((_data[0] << 8) | _data[1]);
  Serial.println((_data[2] << 8) | _data[3]);
  Serial.println(_data[4] == (_data[0] + _data[1] + _data[2] + _data[3]));

  return _raw;
}

bool DHT::_readBit() {
  unsigned long lowtick, hightick;

  lowtick = _awaitPulse(LOW);
  hightick = _awaitPulse(HIGH);

  if (hightick > lowtick) {
    return 1;
  } else {
    return 0;
  }

  return 0;
}

uint32_t DHT::_awaitPulse(int state) {
  pinMode(_pin, INPUT_PULLUP);
  uint32_t acc = 0;

  while (digitalRead(_pin) == state) {
    if (acc++ >= _maxcycles) {
      return CYCLES_TIMEOUT;
    }
  }

  return acc;
}

void DHT::_clear() {
  _data[0] = _data[1] = _data[2] = _data[3], _data[4] = 0;
  _raw = 0;

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);

  pinMode(_pin, INPUT_PULLUP);
}
