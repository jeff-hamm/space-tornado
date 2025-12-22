#include "web_interface.h"
#include "config.h"
#include "rocket_state.h"
#include "logging.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

extern bool isConfigMode;
AsyncWebServer server(WEB_SERVER_PORT);

// WiFi config portal handlers (only used in config mode)
void handleWiFiConfigRoot(AsyncWebServerRequest *request) {
    const char* html = R"(
<!DOCTYPE html><html><head><title>WiFi Setup</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>body{font-family:Arial;margin:20px;background:#f0f0f0}
.c{max-width:300px;margin:auto;background:white;padding:20px;border-radius:5px}
input{width:100%;padding:8px;margin:5px 0;border:1px solid #ddd;box-sizing:border-box}
button{width:100%;background:#007cba;color:white;padding:10px;border:none;cursor:pointer;margin:5px 0}
.logs-btn{background:#28a745;text-decoration:none;display:block;text-align:center;padding:10px;color:white;border-radius:5px}
</style></head><body><div class="c"><h2>📡 WiFi Config</h2>
<form action="/wifi-save" method="POST">
<input type="text" name="ssid" placeholder="WiFi SSID" required>
<input type="password" name="password" placeholder="Password">
<button type="submit">Connect to WiFi</button></form>
<a href="/logs" class="logs-btn">📄 View System Logs</a>
</div></body></html>
)";
    request->send(200, "text/html", html);
}

void handleWiFiConfigSave(AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true)) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->hasParam("password", true) ? request->getParam("password", true)->value() : "";
        
        // Save credentials (defined in wifi_manager.cpp)
        extern void saveWiFiCredentials(const String& ssid, const String& password);
        saveWiFiCredentials(ssid, password);
        
        request->send(200, "text/plain", "Connecting to " + ssid + "...\nDevice will restart.");
        delay(1000);
        ESP.restart();
    } else {
        request->send(400, "text/plain", "SSID required");
    }
}

void initConfigPortalWebInterface() {
    server.on("/", HTTP_GET, handleWiFiConfigRoot);
    server.on("/wifi-save", HTTP_POST, handleWiFiConfigSave);
    server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", Logger.getLogsAsHtml());
    });
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    server.begin();
    Logger.println("✅ WiFi config portal web interface initialized");
}

void initWebInterface() {
    // Only initialize main interface if not in config mode
    if (isConfigMode) {
        initConfigPortalWebInterface();
        return;
    }
    
    // Root page - main control interface
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Space Tornado Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background: #1a1a1a;
            color: #fff;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            margin-bottom: 30px;
        }
        .status-card {
            background: #2a2a2a;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
        }
        .status-item {
            display: flex;
            justify-content: space-between;
            margin: 10px 0;
            padding: 10px;
            background: #1a1a1a;
            border-radius: 5px;
        }
        .control-section {
            background: #2a2a2a;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
        }
        .slider-container {
            margin: 20px 0;
        }
        .slider {
            width: 100%;
            height: 30px;
            -webkit-appearance: none;
            appearance: none;
            background: #444;
            outline: none;
            border-radius: 15px;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 30px;
            height: 30px;
            background: #4CAF50;
            cursor: pointer;
            border-radius: 50%;
        }
        .slider::-moz-range-thumb {
            width: 30px;
            height: 30px;
            background: #4CAF50;
            cursor: pointer;
            border-radius: 50%;
        }
        .fire-button {
            width: 100%;
            height: 80px;
            font-size: 32px;
            background: #d32f2f;
            color: white;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            font-weight: bold;
        }
        .fire-button:active {
            background: #b71c1c;
        }
        .fire-button.fireing {
            background: #ff6f00;
            animation: pulse 0.5s infinite;
        }
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.7; }
        }
        .value-display {
            font-size: 24px;
            font-weight: bold;
            color: #4CAF50;
        }
        .timestamp {
            color: #888;
            font-size: 12px;
        }
        a {
            color: #4CAF50;
            text-decoration: none;
            margin: 0 10px;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 Space Tornado Control</h1>
        </div>
        
        <div class="status-card">
            <h2>Status</h2>
            <div class="status-item">
                <span>Current Speed:</span>
                <span class="value-display" id="currentSpeed">0%</span>
            </div>
            <div class="status-item">
                <span>Target Speed:</span>
                <span class="value-display" id="targetSpeed">0%</span>
            </div>
            <div class="status-item">
                <span>Direction:</span>
                <span class="value-display" id="direction">FORWARD</span>
            </div>
            <div class="status-item">
                <span>Approximate Velocity:</span>
                <span class="value-display" id="velocity">0</span>
            </div>
            <div class="status-item">
                <span>Enabled:</span>
                <span class="value-display" id="enabled">NO</span>
            </div>
            <div class="status-item">
                <span>Last Update:</span>
                <span class="timestamp" id="timestamp">--</span>
            </div>
        </div>
        
        <div class="control-section">
            <h2>Speed Control</h2>
            <div class="slider-container">
                <input type="range" min="0" max="100" value="0" class="slider" id="speedSlider">
                <div style="text-align: center; margin-top: 10px;">
                    <span class="value-display" id="speedValue">0%</span>
                </div>
            </div>
        </div>
        
        <div class="control-section">
            <h2>Direction</h2>
            <button onclick="toggleDirection()" style="width: 100%; padding: 15px; font-size: 20px; background: #2196F3; color: white; border: none; border-radius: 5px; cursor: pointer;">
                <span id="directionButton">TOGGLE DIRECTION</span>
            </button>
        </div>
        
        <div class="control-section">
            <h2>Thrusters</h2>
            <button class="fire-button" id="fireButton" onmousedown="startFire()" onmouseup="stopFire()" ontouchstart="startFire()" ontouchend="stopFire()">
                🔥 FIRE THRUSTERS
            </button>
        </div>
        
        <div style="text-align: center; margin-top: 30px;">
            <a href="/logs">View Logs</a>
            <a href="/api/state">API State (JSON)</a>
        </div>
    </div>
    
    <script>
        let currentDirection = true;
        let isFiring = false;
        
        const speedSlider = document.getElementById('speedSlider');
        const speedValue = document.getElementById('speedValue');
        
        speedSlider.addEventListener('input', function() {
            const value = this.value;
            speedValue.textContent = value + '%';
            setSpeed(value);
        });
        
        function setSpeed(speed) {
            fetch('/api/speed?value=' + speed, { method: 'POST' })
                .then(response => response.text())
                .then(data => console.log('Speed set:', data));
        }
        
        function toggleDirection() {
            // Toggle direction (send opposite of current)
            const newDirection = !currentDirection;
            fetch('/api/direction?value=' + (newDirection ? 'forward' : 'reverse'), { method: 'POST' })
                .then(response => response.text())
                .then(data => {
                    console.log('Direction set:', data);
                    // Status will update on next poll
                });
        }
        
        function startFire() {
            if (!isFiring) {
                isFiring = true;
                const button = document.getElementById('fireButton');
                button.classList.add('fireing');
                fetch('/api/fire?state=1', { method: 'POST' });
            }
        }
        
        function stopFire() {
            if (isFiring) {
                isFiring = false;
                const button = document.getElementById('fireButton');
                button.classList.remove('fireing');
                fetch('/api/fire?state=0', { method: 'POST' });
            }
        }
        
        function updateStatus() {
            fetch('/api/state')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('currentSpeed').textContent = data.currentSpeed.toFixed(1) + '%';
                    document.getElementById('targetSpeed').textContent = data.targetSpeed.toFixed(1) + '%';
                    document.getElementById('direction').textContent = data.direction ? 'FORWARD' : 'REVERSE';
                    document.getElementById('velocity').textContent = data.velocity.toFixed(2);
                    document.getElementById('enabled').textContent = data.enabled ? 'YES' : 'NO';
                    document.getElementById('timestamp').textContent = new Date(data.timestamp).toLocaleTimeString();
                    
                    // Update slider to match target speed
                    speedSlider.value = data.targetSpeed;
                    speedValue.textContent = data.targetSpeed.toFixed(1) + '%';
                    
                    // Update direction button
                    currentDirection = data.direction;
                    document.getElementById('directionButton').textContent = currentDirection ? 'FORWARD' : 'REVERSE';
                })
                .catch(error => console.error('Error:', error));
        }
        
        setInterval(updateStatus, 500);
        updateStatus();
    </script>
</body>
</html>
        )";
        request->send(200, "text/html", html);
    });
    
    // API endpoint for state
    server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(1024);
        doc["currentSpeed"] = getCurrentSpeedPercent();
        doc["targetSpeed"] = getTargetSpeedPercent();
        doc["direction"] = getCurrentDirection();
        doc["targetDirection"] = getTargetDirection();
        doc["velocity"] = getApproximateVelocity();
        doc["enabled"] = isEnabled();
        doc["firingThrusters"] = isFiringThrusters();
        doc["timestamp"] = millis();
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    // API endpoint for speed
    server.on("/api/speed", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            float speed = request->getParam("value")->value().toFloat();
            updateTargetSpeed(speed);
            request->send(200, "text/plain", "Speed set to " + String(speed) + "%");
        } else {
            request->send(400, "text/plain", "Missing value parameter");
        }
    });
    
    // API endpoint for direction
    server.on("/api/direction", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            String value = request->getParam("value")->value();
            bool forward = (value == "forward");
            updateTargetDirection(forward);
            request->send(200, "text/plain", "Direction set to " + value);
        } else {
            request->send(400, "text/plain", "Missing value parameter");
        }
    });
    
    // API endpoint for fire
    server.on("/api/fire", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("state")) {
            bool firing = (request->getParam("state")->value().toInt() == 1);
            setFiringThrusters(firing);
            request->send(200, "text/plain", firing ? "Thrusters firing" : "Thrusters stopped");
        } else {
            request->send(400, "text/plain", "Missing state parameter");
        }
    });
    
    // Logs page
    server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", Logger.getLogsAsHtml());
    });
    
    server.begin();
    Logger.println("✅ Web interface initialized");
}

void handleWebInterface() {
    // AsyncWebServer handles requests automatically, no need to call anything here
    // This function exists for consistency with the interface
}
