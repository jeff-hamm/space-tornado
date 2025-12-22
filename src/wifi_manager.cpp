#include "wifi_manager.h"
#include "logging.h"
#include "web_interface.h"
#include <nvs_flash.h>
#include <ESPmDNS.h>

// WiFi Setup Variables
DNSServer dnsServer;
Preferences wifiPrefs;
bool isConfigMode = false;
unsigned long portalStartTime = 0;

// WiFi connection callback
static WiFiConnectedCallback wifiConnectedCallback = nullptr;

void saveWiFiCredentials(const String& ssid, const String& password) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        Logger.println("⚠️ NVS partition issue, erasing and reinitializing...");
        nvs_flash_erase();
        nvs_flash_init();
    }
    
    if (!wifiPrefs.begin("wifi", false)) {
        Logger.println("❌ Failed to open WiFi preferences for writing");
        return;
    }
    
    wifiPrefs.putString("ssid", ssid);
    wifiPrefs.putString("password", password);
    wifiPrefs.end();
    
    Logger.printf("✅ WiFi credentials saved for SSID: %s\n", ssid.c_str());
}

bool connectToWiFi() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        Logger.println("⚠️ NVS partition issue, erasing and reinitializing...");
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        Logger.printf("❌ NVS init failed: %d\n", err);
    }
    
    if (!wifiPrefs.begin("wifi", true)) {
        Logger.println("ℹ️ No WiFi preferences found (first boot?)");
        return false;
    }
    
    String ssid = wifiPrefs.getString("ssid", "");
    String password = wifiPrefs.getString("password", "");
    wifiPrefs.end();
    
    if (ssid.length() == 0) {
        Logger.println("📡 No saved WiFi credentials found");
        return false;
    }
    
    Logger.printf("📡 Starting WiFi connection to: %s\n", ssid.c_str());
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    Logger.println("📡 WiFi connection initiated in background");
    return true;
}

bool startConfigPortalSafe() {
    Logger.println("🔧 Starting WiFi configuration portal (safe mode)...");
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(2000);
    
    Logger.println("🔧 Setting WiFi mode to AP...");
    for (int retry = 0; retry < 3; retry++) {
        if (WiFi.mode(WIFI_AP)) {
            Logger.println("✅ WiFi mode set to AP");
            break;
        }
        Logger.printf("⚠️ WiFi mode retry %d/3\n", retry + 1);
        delay(1000);
        if (retry == 2) {
            Logger.println("❌ Failed to set WiFi mode after retries");
            return false;
        }
    }
    
    delay(1000);
    
    Logger.println("🔧 Starting SoftAP...");
    for (int retry = 0; retry < 3; retry++) {
        if (WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASSWORD)) {
            Logger.println("✅ SoftAP started successfully");
            isConfigMode = true;
            break;
        }
        Logger.printf("⚠️ SoftAP retry %d/3\n", retry + 1);
        delay(1000);
        if (retry == 2) {
            Logger.println("❌ Failed to start SoftAP after retries");
            return false;
        }
    }
    
    delay(1000);
    
    IPAddress apIP = WiFi.softAPIP();
    Logger.printf("📡 WiFi configuration portal started\n");
    Logger.printf("AP Name: %s\n", WIFI_AP_NAME);
    Logger.printf("AP Password: %s\n", WIFI_AP_PASSWORD);
    Logger.printf("AP IP: %s\n", apIP.toString().c_str());
    
    dnsServer.start(53, "*", apIP);
    
    initConfigPortalWebInterface();
    
    return true;
}

void initWiFi(WiFiConnectedCallback onConnected) {
    Logger.printf("🔧 Starting WiFi initialization (non-blocking)...\n");
    
    wifiConnectedCallback = onConnected;
    
    Logger.println("🔧 Checking for saved credentials...");
    bool hasCredentials = connectToWiFi();
    
    if (!hasCredentials) {
        Logger.println("📱 No saved WiFi credentials - starting configuration portal...");
        if (startConfigPortalSafe()) {
            portalStartTime = millis();
        }
    } else {
        isConfigMode = false;
    }
    
    Logger.println("📡 WiFi initialization complete - connection status will be monitored in background");
}

void initOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.setPort(OTA_PORT);
    
    ArduinoOTA.onStart([]() { Logger.println("OTA Start"); });
    ArduinoOTA.onEnd([]() { Logger.println("OTA End"); });
    ArduinoOTA.onError([](ota_error_t error) { Logger.printf("OTA Error: %u\n", error); });
    
    Logger.println("🔄 OTA configuration complete - will start when WiFi is ready");
}

void startOTA() {
    ArduinoOTA.begin();
    Logger.printf("✅ OTA Ready: %s:%d\n", WiFi.localIP().toString().c_str(), OTA_PORT);
}

void stopOTA() {
    ArduinoOTA.end();
    Logger.println("🔄 OTA stopped due to WiFi change");
}

void handleWiFiLoop() {
    static bool connectionLogged = false;
    static bool otaStarted = false;
    static unsigned long connectionStartTime = 0;
    
    if (isConfigMode) {
        dnsServer.processNextRequest();
        // Web interface is handled asynchronously by AsyncWebServer
        
        if (!otaStarted) {
            startOTA();
            otaStarted = true;
        }
    } else if (WiFi.getMode() == WIFI_STA) {
        if (WiFi.status() == WL_CONNECTED && !connectionLogged) {
            Logger.printf("✅ WiFi connected successfully!\n");
            Logger.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
            Logger.printf("Signal Strength: %d dBm\n", WiFi.RSSI());
            
            IPAddress dns1(8, 8, 8, 8);
            IPAddress dns2(1, 1, 1, 1);
            WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), dns1, dns2);
            Logger.println("🌐 DNS configured: 8.8.8.8, 1.1.1.1");
            
            if (wifiConnectedCallback != nullptr) {
                Logger.println("📞 Calling WiFi connected callback...");
                wifiConnectedCallback();
            }
            
            // Start mDNS so device is discoverable at spacetornado.local
            if (MDNS.begin("spacetornado")) {
                MDNS.addService("http", "tcp", 80);
                Logger.println("📡 mDNS started: spacetornado.local");
            } else {
                Logger.println("⚠️ mDNS failed to start");
            }
            
            if (!otaStarted) {
                startOTA();
                otaStarted = true;
            }
            connectionLogged = true;
            // Initialize main web interface when connected
            extern void initWebInterface();
            initWebInterface();
        } else if (WiFi.status() != WL_CONNECTED && connectionStartTime == 0) {
            connectionStartTime = millis();
        } else if (WiFi.status() != WL_CONNECTED && connectionStartTime > 0 && 
                 (millis() - connectionStartTime) > 30000) {
            Logger.println("❌ WiFi connection timeout - starting configuration portal");
            
            if (otaStarted) {
                stopOTA();
                otaStarted = false;
            }
            
            connectionStartTime = 0;
            connectionLogged = false;
            
            if (startConfigPortalSafe()) {
                portalStartTime = millis();
            }
        }
    }
    
    if (otaStarted && (WiFi.status() == WL_CONNECTED || isConfigMode)) {
        ArduinoOTA.handle();
    }
    
    // Web interface is handled asynchronously by AsyncWebServer
}

