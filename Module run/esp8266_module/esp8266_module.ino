#include <SoftwareSerial.h>

#define RX 14 //  RX of ESP to pin A0
#define TX 15 //  TX of ESP to pin A1

//	Use ESP8266 WiFi module
SoftwareSerial esp(RX, TX);

//	Use ESP8266 WiFi module
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

//	Use ESP8266 WiFi module
void WifiConnect()
{
	String ssid = "Abhinav";
	String pass = "abcdefgh";
	SndCmd("AT", "OK", 5);
	SndCmd("AT+CWMODE=1", "OK", 5);
	SndCmd("AT+CWJAP=\""+ssid+"\",\""+pass+"\"", "OK", 10);
}

void setup()
{
	Serial.begin(9600);
	esp.begin(9600);
	WifiConnect();
}

void loop()
{
}
