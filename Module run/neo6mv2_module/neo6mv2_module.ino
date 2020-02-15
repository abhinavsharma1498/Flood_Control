#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define GRX 6
#define GTX 7

SoftwareSerial gpsSerial(GRX, GTX);
TinyGPSPlus gps;

void setup()
{
	Serial.begin(9600);
	gpsSerial.begin(9600);
}

void loop()
{
	while(gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }
    Serial.println("Satellite count: " + String(gps.satellites.value()));
    Serial.println("Latitude: " + String(gps.location.lat()));
    Serial.println("Longitude: " + String(gps.location.lng()));
    delay(5000);
}
