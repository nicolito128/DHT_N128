#ifndef DHT_N128_
#define DHT_N128_

enum DHT_Type {
  DHT22,
};

class DHT {
public:
  DHT(int pin, enum DHT_Type dht_typ);

  void begin();
  byte read();

private:
  enum DHT_Type _typ;
  int _pin;
  byte _data[5];

  byte readByte();
};

#endif
