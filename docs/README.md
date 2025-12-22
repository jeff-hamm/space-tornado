# Space Tornado - Web Bluetooth Control

This folder contains a Progressive Web App (PWA) that allows you to control Space Tornado via Bluetooth Low Energy (BLE) from your phone's web browser.

## 🌐 Live Site

**https://space-tornado.infinitebutts.com** (redirects from jeff-hamm.github.io/space-tornado)

## 📱 How to Use

### From Android (Recommended)

1. Open Chrome on your Android device
2. Navigate to the control page URL
3. Tap **"Connect to Space Tornado"**
4. Select "SpaceTornado" from the Bluetooth device list
5. Once connected, you can:
   - Adjust speed with the slider or +/- buttons
   - Toggle direction between Forward and Reverse
   - Hold the **🔥 Fire Thrusters** button to activate exhaust
   - Tap **🛑 Emergency Stop** to immediately halt

### From Desktop

Chrome and Edge on Windows/Mac/Linux support Web Bluetooth:
1. Enable Bluetooth on your computer
2. Visit the control page
3. Click Connect and pair with SpaceTornado

### iOS Limitations

⚠️ **Safari on iOS does not support Web Bluetooth.** 

Options for iOS users:
- Use a Bluetooth Serial Terminal app (connects via Bluetooth Classic to "SpaceTornado-SPP")
- Use the built-in WiFi web interface when connected to Space Tornado's network

## 🔧 Technical Details

### BLE Service

| Property | Value |
|----------|-------|
| Device Name | `SpaceTornado` |
| Service UUID | `4fafc201-1fb5-459e-8fcc-c5c9c331914b` |
| Command Characteristic | `beb5483e-36e1-4688-b7f5-ea07361b26a8` (Write) |
| Status Characteristic | `beb5483f-36e1-4688-b7f5-ea07361b26a9` (Read/Notify) |

### Commands

| Command | Description |
|---------|-------------|
| `+` | Increase speed by 10% |
| `-` | Decrease speed by 10% |
| `S##` | Set speed to ## percent (e.g., `S50`) |
| `D` | Set direction to Forward |
| `R` | Set direction to Reverse |
| `F` | Start firing thrusters |
| `f` | Stop firing thrusters |
| `X` | Emergency stop |
| `C` | Clear emergency stop |
| `?` | Request status update |

### Status Format

The ESP32 sends status updates as:
```
S:50.0,T:60.0,D:1,E:1,F:0
```

| Field | Description |
|-------|-------------|
| `S` | Current speed (%) |
| `T` | Target speed (%) |
| `D` | Direction (1=Forward, 0=Reverse) |
| `E` | Enabled (1=Yes, 0=No) |
| `F` | Firing thrusters (1=Yes, 0=No) |

## 📁 Files

- `index.html` - Main PWA control interface
- `manifest.json` - PWA manifest for "Add to Home Screen"
- `icon-192.svg` - App icon (192x192)
- `icon-512.svg` - App icon (512x512)

## 🚀 Deploying

This folder is automatically deployed to GitHub Pages when pushed to the `main` branch.

GitHub Pages settings should be configured to:
- Source: Deploy from a branch
- Branch: `main`
- Folder: `/docs`

## 📄 License

Part of the Space Tornado project.
