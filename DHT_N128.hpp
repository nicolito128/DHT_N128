#ifndef DHT_N128_H_
#define DHT_N128_H_
#define DEFAULT_PULL_TIME 40

#include <Arduino.h>

typedef int32_t raw_magnitude;

typedef float magnitude;

enum DHT_Type {
  DHT11,
  DHT22, // AM2302
};

class DHT {
public:  
  DHT(int pin, enum DHT_Type dht_typ);

  void begin();
  raw_magnitude rawRead();

private:
  enum DHT_Type _typ;

  int _pin;

  uint8_t _pullTime, _data[5];

  int32_t _raw;

  unsigned long _lastreadTime, _maxcycles;

  void _startSignal();

  bool _readBit();

  uint32_t _awaitPulse(bool);

  void _clear();
};

#endif
