#ifndef DHT_N128_H_
#define DHT_N128_H_

#include <Arduino.h>

enum class DHTSensorType {
  DHT11,
  DHT22, // AM2302
};

enum class TempScale {
  Celsius,
  Fahrenheit,
  Kelvin,
};

enum class ErrorCode {
  None,
  // Must wait MIN_INTERVAL_MILLIS to take another reading.
  MinIntervalWait,
  // Generic CYCLES_TIMEOUT limit error.
  Timeout,
  // Sensor pulls low fails. Sensor response signal stage.
  SensorPullLow,
  // Sensor pulls up fails. Sensor response signal stage.
  SensorPullUp,
  // LOW signal of sensor data stream of bits fails.
  StreamLOW,
  // HIGH signal of sensor data stream of bits fails.
  StreamHIGH,
  // Checksum do not match.
  BadChecksum,
};

String errorToString(ErrorCode);

class DHT {
public:
  DHT(int, DHTSensorType);

  void begin();

  // Set the MCU pull time. Default: 50 (microseconds)
  void setPullTime(unsigned long us);

  // Read for temperature. Default: TempScale::Celsius.
  float readTemperature(TempScale = TempScale::Celsius);

  // Read for relative humidity.
  float readHumidity();

  // Raw
  ErrorCode rawRead(uint32_t* = NULL);

  // Returns the last catched error. Default: ErrorCode::None
  ErrorCode error();

private:
  // Sensor type to adjust the values of the protocol.
  DHTSensorType _typ;

  // Pin used by the sensor.
  int _pin;

  // If the first rawRead was already executed.
  bool _started;
  // If the current temperature is negative .
  bool _neg_temp;
  // Stream of bits in each reading cycle.
  // STREAM = 16 bits of Relative Humidity + 16 bits of Temperature + 8 bits of Checksum.
  // Checksum = 16b RH + 16b T.
  bool _bits[40];

  // Array of bytes for the stream of data.
  // 8 bits RH + 8 bits RH + 8 bits T + 8 bits T + 8 bits Checksum.
  uint8_t _data[5];

  // Last raw stream of data expressed as an uint32.
  uint32_t _lastraw;

  // Max number of processor cycles.
  unsigned long _maxcycles;
  // Last timestamp of the execution of rawRead.
  unsigned long _lastreadTime;
  // Pull time of the MCU response.
  // Default: 50 us (microseconds).
  unsigned long _pullTime;

  float _temperature;
  float _humidity;

  ErrorCode _lastError;

  // Start communication signal.
  void _startSignal();
  // Clear the current states.
  void _clear();

  ErrorCode _sensorResponseSignals();
  ErrorCode _readBitStream();
  ErrorCode _readBit(int);
  ErrorCode _parseRawStream();

  uint32_t _awaitPulse(int);
};

#endif
