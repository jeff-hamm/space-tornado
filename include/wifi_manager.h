#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ArduinoOTA.h>

typedef void (*WiFiConnectedCallback)();

#ifndef WIFI_AP_NAME
#define WIFI_AP_NAME "Space-Tornado-Setup"
#endif

#ifndef WIFI_AP_PASSWORD
#define WIFI_AP_PASSWORD "tornado123"
#endif

#ifndef WIFI_PORTAL_TIMEOUT
#define WIFI_PORTAL_TIMEOUT 180
#endif

#ifndef OTA_HOSTNAME
#define OTA_HOSTNAME "space-tornado"
#endif

#ifndef OTA_PASSWORD
#define OTA_PASSWORD "tornado-ota-2024"
#endif

#ifndef OTA_PORT
#define OTA_PORT 3232
#endif

void initWiFi(WiFiConnectedCallback onConnected = nullptr);
void initOTA();
void startOTA();
void stopOTA();
bool connectToWiFi();
void saveWiFiCredentials(const String& ssid, const String& password);
bool startConfigPortalSafe();
void handleWiFiLoop();

extern bool isConfigMode;

#endif // WIFI_MANAGER_H

