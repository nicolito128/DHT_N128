#ifndef DHT_N128_H_
#define DHT_N128_H_
#define DEFAULT_PULL_TIME 50

#include <Arduino.h>

typedef uint32_t raw_magnitude;

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

  bool _bits[40];

  int _pin;

  uint8_t _pullTime, _data[5];

  raw_magnitude _raw;

  unsigned long _maxcycles;

  void _startSignal();

  bool _readBit();

  uint32_t _awaitPulse(int);

  void _clear();
};

#endif
