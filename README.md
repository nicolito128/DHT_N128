# DHT_N128

A simple library for using DHT-type sensors (DHT11, DHT22, etc.).

## Getting started

Download and import the library in your IDE.

### Arduino IDE

Go to `Sketch |> Include Library |> Add .ZIP Library...`

## Usage

```cpp
#include <DHT_N128.hpp>

#define PIN 7

DHT dht(PIN, DHTSensorType::DHT22);

void setup() {
  Serial.begin(9600);
  Serial.println("Humidity/Temperature Sensor Example");
  dht.begin();
}

void loop() {
  float rh = dht.readHumidity(); // Relative Humidity
  float temp = dht.readTemperature(); // Default: TempScale::Celsius

  if (isnan(rh) || isnan(temp)) {
    Serial.println("Error (Not-A-Number) when trying to read humidity and temperature.");
  } else {
    Serial.print(rh); Serial.println(" %RH");
    Serial.print(temp); Serial.println(" ˚C");
  }

  delay(2000);
}
```

You can find more usages examples in [examples/](./examples/).

## Acknowledgments

* Eduardo Gomez (Professor. Head of "Robotics and Automation" subject).
* Luciano Yurquina (Testing and Research).

## References

* [DHT22-AM2302-Datasheet | ASONG - Temp, Humidity & Dew point measurement experts](https://www.makerguides.com/wp-content/uploads/2019/02/DHT22-AM2302-Datasheet.pdf)
