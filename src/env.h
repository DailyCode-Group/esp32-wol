#ifndef ENV_H
#define ENV_H

// Uncomment for development mode
#define DEV_MODE


// Values are set in a file env.cpp in the same directory as the env.h file
extern const char ssid[];                       // your network SSID (name)
extern const char password[];                   // your network key
extern const unsigned char targetMac[6];        // mac to wake
extern const unsigned char bssid[6];            // mac of internet access point

extern const unsigned char staticIpEsp[4];      // IpAddress of ESP module
extern const unsigned char gatewayIp[4];        // IpAddress of the gateway
extern const unsigned char subnetMask[4];       // IpAddress of your networks subnet mask


const unsigned char wifiChannel = 0;            // Wifi Channel for my specific network. Should work by default
const char broadcastIP[16] = "255.255.255.255"; // UDP broadcast address
const int wolPort = 9;                          // Common WOL port



#endif
