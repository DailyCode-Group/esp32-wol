#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "env.h"

#define BUTTON_PIN 32

RTC_DATA_ATTR unsigned long bootCount = 0;

unsigned long startTime;
unsigned long endTime;

const uint8_t bssid[6] = {0x40, 0x3F, 0x8C, 0xA6, 0xA4, 0x50};
const uint8_t channel = 0;

int status = WL_IDLE_STATUS; // the Wifi radio's status

IPAddress local_IP(192, 168, 1, 201);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

// UDP broadcast address and port
const char *broadcastIP = "255.255.255.255";
const int wolPort = 9; // Common WOL port

WiFiUDP udp;

void initWiFi()
{
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, channel, bssid);
  Serial.print("Connecting to WiFi ..");
  unsigned int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    counter++;
    delay(5);
    if (counter == 50)
    {
      counter = 0;
      Serial.print('.');
    }
  }
  Serial.println(WiFi.localIP());
}


void sendWOL(const byte *macAdress)
{
  Serial.println("Sending WOL Packet... ");
  // Create the magic packet
  byte magicPacket[102];        // 6 bytes of 0xFF + 16 * 6 bytes of MAC
  memset(magicPacket, 0xFF, 6); // First 6 bytes are 0xFF
  for (int i = 6; i < 102; i++)
  {
    magicPacket[i] = macAdress[(i - 6) % 6];
  }

  // Send the magic packet over UDP
  udp.beginPacket(broadcastIP, wolPort); // Set up a broadcast packet
  udp.write(magicPacket, sizeof(magicPacket));
  udp.endPacket();

  Serial.println("Sent WOL Packet... ");
}


void sendWOLcommands() {
  sendWOL(targetMac);
  // sendWOL(targetMacReciever);
}


void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);

  bootCount++;
  
  Serial.print("Boot count: ");
  Serial.println(bootCount);
  if (bootCount == 1) {
    Serial.println("Since this is the plug in boot no wol command will be sent.");
  } else {
    startTime = millis();
    initWiFi();
    endTime = millis();

    Serial.print("Connected after (ms): ");
    Serial.println(endTime - startTime);

    sendWOLcommands();
  }

  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, HIGH); // Wake up when BUTTON_PIN is LOW
  Serial.println("Going to deep sleep. Press the button to wake up...");
  delay(500);
  esp_deep_sleep_start();
}

void loop()
{
  // empty
}

