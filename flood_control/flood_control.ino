#include <SoftwareSerial.h>

#define RX 14 //  RX of ESP to pin A0
#define TX 15 //  TX of ESP to pin A1

#define trigPin 3
#define echoPin 4
#define S0 8
#define S1 9
#define S2 10

#define flowInterrupt 0	//	Interrupt 0 is attached to digital pin 2
#define flowPin 2
#define calibrationFactor 4.5	//	Hall effect flow sensor outputs approximately 4.5 pulses per second per litre/minute of flow

#define motorPin 5

SoftwareSerial esp(RX, TX);
String ssid = "Abhinav";
String pass = "abcdefgh";
String host = "api.thingspeal.com";
String port = "80";
String api = "";

volatile byte pulseCount;
float flowRate;
unsigned int flowML;
unsigned long totalML;
unsigned long oldTime;

bool motorState;

void SndCmd(String cmd, char* reply, int maxTime)
{
	int currTime = 0;
	boolean found = false;
	Serial.println(cmd);
	while(currTime < maxTime)
	{
		esp.println(cmd);
		if(esp.find(reply))
		{
			found = true;
			break;
		}
		++currTime;
	}

	if(found)	Serial.println("OYI");
	else	Serial.println("FAIL");
}

void WifiConnect()
{
	SndCmd("AT", "OK", 5);
	SndCmd("AT+CWMODE=1", "OK", 5);
	SndCmd("AT+CWJAP=\""+ssid+"\",\""+pass+"\"", "OK", 10);
}

void SendData(int level, float flow, bool motor)
{
	//	Field 1 -> Level, 	Field 2 -> Flow,	Field 3 -> Motor
	String data = "GET https://api.thingspeak.com/update?api_key=" + api + "&field1=" + String(level) + "&field2=" + String(flow) + "&field3=" + String(motor);

	SndCmd("AT+CIPMUX=0", "OK", 5);
	SndCmd("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "OK", 15);
	SndCmd("AT+CIPSEND=" + String(data.length()), ">", 5);
	esp.println(data);
	SndCmd("AT+CIPCLOSE", "OK", 5);
}

int GetDistance()
{
	digitalWrite(trigPin, LOW);	//	Reset the pin before reading value
	delayMicroseconds(2);

	//	Sending ultrasonic wave for 10 microseconds
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	//	Reading the echo of wave
	int duration = pulseIn(echoPin, HIGH);

	return duration * 0.034 / 2;
}

int FromSensor(int num)
{
	digitalWrite(S0, LOW);
	digitalWrite(S1, LOW);
	digitalWrite(S2, LOW);
	switch(num)
	{
	case 1:
		digitalWrite(S0, HIGH);
		return GetDistance();
	case 2:
		digitalWrite(S1, HIGH);
		return GetDistance();
	case 3:
		digitalWrite(S0, HIGH);
		digitalWrite(S1, HIGH);
		return GetDistance();
	case 4:
		digitalWrite(S2, HIGH);
		return GetDistance();
	case 5:
		digitalWrite(S2, HIGH);
		digitalWrite(S0, HIGH);
		return GetDistance();
	default:
	    return -1;
	}
}

void PulseCounter()
{
	++pulseCount;
}

void GetFlow()
{
	if((millis() - oldTime) > 1000)	//	Process counters once per second
	{
		detachInterrupt(flowPin);

		flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

		oldTime = millis();

		//	Converting from litre/minute to millilitres
		flowML = (flowRate / 60) * 1000;
		totalML += flowML;

		//	Display flow for this second in litre/minute
		Serial.println("Flow Rate: " + String(int(flowRate)) + " L/min");
		//	Display cumulative total of litres  flowed since starting
		Serial.println("Output Liquid Quantity: " + String(totalML) + " mL\t" + String(totalML/1000) + " L");

		pulseCount = 0;	//	Reset pulse counter
		attachInterrupt(flowInterrupt, PulseCounter, FALLING);
	}
}

void setup()
{
	Serial.begin(9600);
	
	esp.begin(9600);
	WifiConnect();

	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	pinMode(flowPin, INPUT);
	digitalWrite(flowPin, HIGH);
	pulseCount = 0;
	flowRate = 0.0;
	flowML = 0;
	totalML = 0;
	oldTime = 0;
	attachInterrupt(flowInterrupt, PulseCounter, FALLING);

	pinMode(motorPin, OUTPUT);
	digitalWrite(motorPin, LOW);
	motorState = false;
}

void loop()
{
	int level = GetDistance();
	GetFlow();
	SendData(level, flowRate, motorState);
	delay(1000 * 60 * 5);
}