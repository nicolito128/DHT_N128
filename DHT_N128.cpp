#include "DHT_N128.hpp"

#define MIN_INTERVAL_MILLIS 2000
#define TRANSMISSION_TIMEOUT_MICROS 50
#define CYCLES_TIMEOUT UINT32_MAX

// "at least 18ms"
#define DHT11_FIRST_DELAY_MICROS 19000
// "at least 1ms"
#define DHT22_FIRST_DELAY_MICROS 1010

DHT::DHT(int pin, enum DHT_Type dht_typ) {
  _pin = pin;
  _typ = dht_typ;

  _pullTime = DEFAULT_PULL_TIME;
  _maxcycles = microsecondsToClockCycles(1000);
}

void DHT::begin() {
  pinMode(_pin, INPUT_PULLUP);
  _lastreadTime = millis() - MIN_INTERVAL_MILLIS;
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
    delay(DHT11_FIRST_DELAY_MICROS);
  }
}

raw_magnitude DHT::rawRead() {
  _clear();

  _startSignal();

  // Termina la señal de inicio y damos lugar a la respuesta del sensor
  pinMode(_pin, INPUT_PULLUP);
  delayMicroseconds(_pullTime);

  bool cycles[40];
  for (int i = 0; i < 40; ++i) {
    cycles[i] = _readBit();
  }

  for (int i = 0; i < 40; ++i) {
    if (cycles[i]) {
      _data[i / 8] |= (1 << (i % 8));
    }
  }

  uint8_t checksum = _data[0] + _data[1] + _data[2] + _data[3];

  if (checksum != _data[4]) {
    return -128;
  }

  _raw |= (_data[0] << 24);
  _raw |= (_data[1] << 16);
  _raw |= (_data[2] << 8);
  _raw |= (_data[3]);

  return raw;
}

bool DHT::_readBit() {
  unsigned long startTime, elapsed;
  delayMicroseconds(TRANSMISSION_TIMEOUT_MICROS);

  startTime = millis();
  while (_awaitPulse(HIGH));
  elapsed = millis() - startTime;

  if (elapsed >= 26 && elapsed <= 28) {
    return 0;
  }

  if (elapsed >= 70 && elapsed <= 71) {
    return 1;
  }

  return 0;
}

uint32_t DHT::_awaitPulse(bool level) {
  uint32_t acc;
  while (digitalRead(_pin) == level) {
    if (acc++ >= _maxcycles) {
      return CYCLES_TIMEOUT;
    }
  }
  return acc;
}

void DHT::_clear() {
  unsigned long currentTime = millis();
  if ((currentTime - _lastreadTime) > MIN_INTERVAL_MILLIS) {
    _lastreadTime = currentTime;
  }
  
 _data[0] = _data[1] = _data[2] = _data[3] = 0;

  _raw = 0;
}
