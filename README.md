# DHT_N128

A simple library for using DHT-type sensors (DHT11, DHT22, etc.).

## Getting started

Download and import the library in your IDE.

## Usage

```cpp
#include <DHT_N128.hpp>

#define PIN 7

DHT dht(PIN, SensorType::DHT22);

void setup() {
  Serial.begin(9600);
  Serial.println("Humidity/Temperature Sensor Example");
  dht.begin();
}

void loop() {
  float rh = dht.readHumidity(); // Relative Humidity
  float temp = dht.readTemperature(); // Default: TempScale::Celsius

  Serial.print(rh); Serial.println(" %RH");
  Serial.print(temp); Serial.println(" ˚C");

  delay(2000);
}
```

You can find more usages examples in [examples/](./examples/).

## References

* [DHT22-AM2302-Datasheet | ASONG - Temp, Humidity & Dew point measurement experts](https://www.makerguides.com/wp-content/uploads/2019/02/DHT22-AM2302-Datasheet.pdf)
