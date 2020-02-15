#define flowInterrupt 0	//	Interrupt 0 is attached to digital pin 2
#define flowPin 2

//	Hall effect flow sensor outputs approximately 4.5 pulses per second per litre/minute of flow
#define calibrationFactor 4.5

volatile byte pulseCount;
float flowRate;
unsigned int flowML;
unsigned long totalML;
unsigned long oldTime;

//	Interrupt Service Routine
void PulseCounter()
{
	++pulseCount;
}

//	Use YF-S201 Water Flow Sensor
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
	pinMode(flowPin, INPUT);
	digitalWrite(flowPin, HIGH);

	pulseCount = 0;
	flowRate = 0.0;
	flowML = 0;
	totalML = 0;
	oldTime = 0;

	//	Configured to trigger on a FALLING state change (transition from HIGH to LOW)
	attachInterrupt(flowInterrupt, PulseCounter, FALLING);
}

void loop()
{
	GetFlow();
	delay(2000);
}