#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Declarations
void decreaseLights();
void increaseLights();


// Wi-Fi credentials
const char *ssid = "";
const char *password = "";

// NTP setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "b.ntp.br", 0, 60000); // Update time every 60 seconds

// GPIO for PWM
const int pwmPin = 2;
const int pwmFrequency = 1000;
int dutyCycle = 1023; // Initial duty cycle

void connectWiFi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup()
{
  Serial.begin(115200);

  // Set PWM frequency and attach pin
  pinMode(pwmPin, OUTPUT);
  analogWriteFreq(pwmFrequency);
  analogWrite(pwmPin, dutyCycle);

  // Connect to Wi-Fi
  connectWiFi();

  // Start the NTP client
  timeClient.begin();
}


void loop()
{
  
  // Reconnect if WiFi is disconnected
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }

  // Update NTP time
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  Serial.printf("Current time is: %02d:%02d:%02d\n", currentHour, currentMinute, currentSecond);


  // Handle PWM based on time
  if (currentHour >= 9 && currentHour < 19 && dutyCycle == 1023)
  {
    increaseLights();
  }
  else if ((currentHour >= 19 || currentHour < 9) && dutyCycle == 0)
  {
    decreaseLights();
  }

  delay(1000); // Main loop delay
}

void increaseLights() {
  // Gradually reduce duty cycle from current value to 0 over 1 minute
    int targetDutyCycle = 0;
    int duration = 60; // 60 seconds
    int stepDelay = duration * 1000 / 1024;
    for (int i = dutyCycle; i >= targetDutyCycle; i--)
    {
      analogWrite(pwmPin, i);
      Serial.printf("Duty cycle is: %d\n", i);
      delay(stepDelay);
    }
    dutyCycle = targetDutyCycle;
}


void decreaseLights() {
  // Gradually increase duty cycle from current value to 1023 over 1 minute
    int targetDutyCycle = 1023;
    int duration = 60; // 60 seconds
    int stepDelay = duration * 1000 / 1024;
    for (int i = dutyCycle; i <= targetDutyCycle; i++)
    {
      analogWrite(pwmPin, i);
      delay(stepDelay);
    }
    dutyCycle = targetDutyCycle;
}