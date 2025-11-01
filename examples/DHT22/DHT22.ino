// Example sketch of DHT11/DHT22 humidity+temperature sensor using DHT_N128 library
// Reference Lib: https://github.com/nicolito128/DHT_N128
// Public domain.
// Author: nicolito128 (Nicolas A. Serna) <serna.nicolas.abel@gmail.com>
#include <DHT_N128.h>

// Digital PIN connected to the sensor
#define PIN 7

// Create a new instance of the DHT class.
// Use PIN (7) for communication and use DHT22 specification.
DHT dht(PIN, DHTSensorType::DHT22); // Take a look of SensorType for available options.

void setup() {
  Serial.begin(9600);
  Serial.println("Humidity/Temperature Sensor Example");

  // Prepare the sensor and set defaults.
  dht.begin();
}

void loop() {
  float rh = dht.readHumidity(); // Relative Humidity
  float tempC = dht.readTemperature(); // Default: TempScale::Celsius
  float tempF = dht.readTemperature(TempScale::Fahrenheit); // Temperature as Fahrenheit
  float tempK = dht.readTemperature(TempScale::Kelvin); // Temperature as Kelvin

  if (!isnan(rh)) {
    Serial.print(rh); Serial.println(" %RH");
  } else {
    Serial.println("Humidity read error: NAN (Not-A-Number)");
  }

  if (!isnan(tempC) && !isnan(tempF) && !isnan(tempK)) {
    Serial.print(tempC); Serial.println(" ˚C");
    Serial.print(tempF); Serial.println(" ˚F");
    Serial.print(tempK); Serial.println(" ˚K");
  } else {
    Serial.println("Temperature read error: NAN (Not-A-Number)");
  }

  Serial.println("------------------------------------");

  ErrorCode err = dht.error();
  if (err != ErrorCode::None) {
    Serial.println(errorToString(err));
    Serial.println("------------------------------------");
  }

  // Each sensor reading must occur in an interval of 2s.
  // Internally, if not enough time has passed, the last reading taken by the sensor is returned.
  delay(2000);
}
