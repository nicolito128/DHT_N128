#include "DHT_N128.hpp"

// Interval between each communication with the sensor.
#define MIN_INTERVAL_MILLIS 2000
// Pull up time in microseconds for the MCU.
#define DEFAULT_PULL_MICROS 50
// MCU cycles limit to avoid infinity loops.
#define CYCLES_TIMEOUT UINT32_MAX
// "at least 18ms"
#define DHT11_START_MICROS 18500
// "1~10ms"
#define DHT22_START_MICROS 1100

// Macro helper for early return on errors
#define IFERR(err) \
  if (err != ErrorCode::None) {\
    _lastError = err;\
    return err;\
  }\

String errorToString(ErrorCode err) {
  switch(err) {
  case ErrorCode::MinIntervalWait:
    return "Must wait MIN_INTERVAL_MILLIS to take another reading";

  case ErrorCode::Timeout:
    return "Generic CYCLES_TIMEOUT limit error";
      
  case ErrorCode::SensorPullLow:
    return "Sensor pulls low fails. Sensor response signal stage";

  case ErrorCode::SensorPullUp:
    return "Sensor pulls up fails. Sensor response signal stage";

  case ErrorCode::StreamLOW:
    return "LOW signal of sensor data stream of bits fails";

  case ErrorCode::StreamHIGH:
    return "HIGH signal of sensor data stream of bits fails";

  case ErrorCode::BadChecksum:
    return "Checksum do not match";

  case ErrorCode::None:
  default:
    return "No Error";
  }
}

DHT::DHT(int pin, DHTSensorType sen_typ) {
  _pin = pin;
  _typ = sen_typ;

  _pullTime = DEFAULT_PULL_MICROS;
}

void DHT::begin() {
  pinMode(_pin, INPUT_PULLUP);
  _started = false;
  _maxcycles = microsecondsToClockCycles(1000);
  _lastreadTime = millis() - MIN_INTERVAL_MILLIS;
  _lastError = ErrorCode::None;
}

void DHT::setPullTime(unsigned long us) {
  _pullTime = us;
}

float DHT::readHumidity() {
  ErrorCode err = rawRead();
  // If something is not working return an impossible value.
  if (
    (err != ErrorCode::None && err != ErrorCode::MinIntervalWait) || 
    (_lastError != ErrorCode::None && _lastError != ErrorCode::MinIntervalWait)
  ) {
    return NAN;
  }

  return _humidity;
}

float DHT::readTemperature(TempScale scale) {
  ErrorCode err = rawRead();
  // If something is not working return an impossible value.
  if (
    (err != ErrorCode::None && err != ErrorCode::MinIntervalWait) || 
    (_lastError != ErrorCode::None && _lastError != ErrorCode::MinIntervalWait)
  ) {
    return NAN;
  }
  
  switch(scale) {
    case TempScale::Fahrenheit:
      return _temperature * 1.8 + 32;

    case TempScale::Kelvin:
      return _temperature + 273.15;
    
    case TempScale::Celsius:
    default:
      return _temperature;
  }
}

void DHT::_startSignal() {
  pinMode(_pin, INPUT_PULLUP);

  // Dependiendo del sensor esperamos un tiempo determinado en LOW
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  switch (_typ) {
  case DHTSensorType::DHT22:
    delayMicroseconds(DHT22_START_MICROS);
    break;
  
  case DHTSensorType::DHT11:
  default:
    delayMicroseconds(DHT11_START_MICROS);
  }

  // Termina la señal de inicio y damos lugar a la respuesta del sensor
  pinMode(_pin, INPUT_PULLUP);
  delayMicroseconds(_pullTime);   
}

ErrorCode DHT::_sensorResponseSignals() {
  if (_awaitPulse(LOW) == CYCLES_TIMEOUT) {
    return ErrorCode::SensorPullLow;
  }

  if (_awaitPulse(HIGH) == CYCLES_TIMEOUT) {
    return ErrorCode::SensorPullUp;
  }

  return ErrorCode::None;
}

ErrorCode DHT::_readBitStream() {
  ErrorCode err = ErrorCode::None;
  for (int i = 0; i < 40; ++i) {
    IFERR(_readBit(i))
  }
  return err;
}

ErrorCode DHT::_readBit(int pos) {
  uint32_t nextBitLowTime, highBitTime;

  if ((nextBitLowTime = _awaitPulse(LOW)) == CYCLES_TIMEOUT) {
    return ErrorCode::StreamLOW;
  }

  if ((highBitTime = _awaitPulse(HIGH)) == CYCLES_TIMEOUT) {
    return ErrorCode::StreamHIGH;
  }

  if (highBitTime > nextBitLowTime) {
    _bits[pos] = 1;
  } else {
    _bits[pos] = 0;
  }

  return ErrorCode::None;
}

ErrorCode DHT::_parseRawStream() {
  for (int i = 0; i < 5; ++i) {
      _data[i] = 0;
  }

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 8; ++j) {
        _data[i] |= (static_cast<uint8_t>(_bits[i * 8 + j]) << (7 - j));
      }
  }

  uint8_t checksum = _data[4];
  uint8_t sum = (_data[0] + _data[1] + _data[2] + _data[3]);
  if (checksum != sum) {
    return ErrorCode::BadChecksum;
  }

  _lastraw = (_data[0] << 24) | (_data[1] << 16) | (_data[2] << 8) | (_data[3]);

  // Extracting the bit from temperature to determinate if it is negative
  bool b = _data[2] >> 7;
  if (b) {
    _neg_temp = true;
    // Remove the signed bit
    _data[2] &= 0b01111111;
  } else {
    _neg_temp = false;
  }

  _humidity = (float)((_data[0] << 8) | _data[1]) / 10;
  _temperature = (float)((_data[2] << 8) | _data[3]) / 10;

  if (_neg_temp) {
    _temperature *= (-1);
  }
  
  return ErrorCode::None;
}

ErrorCode DHT::rawRead(uint32_t *dst) {
  unsigned long currentTime = millis();
  if (currentTime - _lastreadTime < MIN_INTERVAL_MILLIS) {
    return ErrorCode::MinIntervalWait;
  }
  _lastreadTime = currentTime;

  _clear();

  _startSignal();

  IFERR(_sensorResponseSignals());

  IFERR(_readBitStream());

  _parseRawStream();

  if (dst != NULL) {
    (*dst) = _lastraw;
  }

  if (!_started) {
    _started = true;
  }

  return ErrorCode::None;
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
  _lastraw = 0;
  _lastError = ErrorCode::None;

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);

  pinMode(_pin, INPUT_PULLUP);
}

ErrorCode DHT::error() {
  return _lastError;
}
