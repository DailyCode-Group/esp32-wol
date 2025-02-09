#include <Arduino.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include "env.h"

// GND -> TIP
// INPUT -> SLEEVE
#define BUTTON_PIN 4

// counts up to 4,294,967,295 boots. Should suffice for a lifetime. If wanted use a BigNumer.
RTC_DATA_ATTR unsigned long bootCount = 0;

unsigned long startTime;
unsigned long endTime;

int status = WL_IDLE_STATUS; // the Wifi radio's status

// UDP broadcast address and port

IPAddress espIp(staticIpEsp);        // static assigned IP of ESP module
IPAddress gatewayAddress(gatewayIp); // IP of the gateway (router ip address)
IPAddress subnetMaskIp(subnetMask);  // IP subnet of network to connect to.

WiFiUDP udp;

void initWiFi()
{
  if (!WiFi.config(espIp, gatewayAddress, subnetMaskIp))
  {
    #ifdef DEV_MODE
      Serial.println("STA Failed to configure");
    #endif
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, wifiChannel, bssid);

  #ifdef DEV_MODE
    Serial.print("Connecting to WiFi ..");
  #endif

  unsigned int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    counter++;
    delay(5);
    if (counter == 50)
    {
      counter = 0;

      #ifdef DEV_MODE
        Serial.print('.');
      #endif
    }
  }

  #ifdef DEV_MODE
    Serial.println(WiFi.localIP());
  #endif
}

/**
 * Create and send the magic packet to the passed Mac Address
 */
void sendWOL(const byte *macAddress)
{

#ifdef DEV_MODE
  Serial.println("Sending WOL Packet... ");

#endif
  byte magicPacket[102];        // 6 bytes of 0xFF + 16 * 6 bytes of MAC
  memset(magicPacket, 0xFF, 6); // First 6 bytes are 0xFF
  for (int i = 6; i < 102; i++)
  {
    magicPacket[i] = macAddress[(i - 6) % 6];
  }

  // Send the magic packet over UDP
  udp.beginPacket(broadcastIP, wolPort);
  udp.write(magicPacket, sizeof(magicPacket));
  udp.endPacket();

  #ifdef DEV_MODE
    Serial.println("Sent WOL Packet... ");
  #endif
}

void sendWOLcommands()
{
  sendWOL(targetMac);
}

void setup()
{

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  bootCount++;

  // Initialize serial and wait for port to open. Only used in development mode.

  #ifdef DEV_MODE
    Serial.begin(9600);
    Serial.println("Boot count: " + String(bootCount));
  #endif

  if (bootCount == 1)
  {

  #ifdef DEV_MODE
    Serial.println("Since this is the plug in boot no wol command will be sent.");
  #endif
  }
  else
  {
    startTime = millis();
    initWiFi();
    endTime = millis();


    #ifdef DEV_MODE
      Serial.print("Connected after (ms): ");
      Serial.println(endTime - startTime);
    #endif

    sendWOLcommands();
  }

  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW); // Wake up when BUTTON_PIN is LOW

  #ifdef DEV_MODE
    Serial.println("Going to deep sleep. Press the button to wake up...");
    delay(500);
  #endif
  esp_deep_sleep_start();
}

void loop()
{
}
