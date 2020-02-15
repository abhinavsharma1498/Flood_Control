#define trigPin 2
#define echoPin 3
#define S0 8
#define S1 9
#define S2 10

//	Use HC-SR04 Ultrasonic Sensor
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

//	Use SN74HC151N multiplexer for HC-SR04 ultrasonic sensor
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

void setup()
{
	Serial.begin(9600);
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
}

void loop()
{
}