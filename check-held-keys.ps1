Add-Type @"
using System;
using System.Runtime.InteropServices;
public class KeyboardChecker {
    [DllImport("user32.dll")]
    public static extern short GetAsyncKeyState(int vKey);
    
    [DllImport("user32.dll")]
    public static extern int GetKeyState(int nVirtKey);
}
"@

Write-Host "Checking for held-down keys..." -ForegroundColor Cyan
Write-Host "Press Ctrl+C to stop monitoring`n" -ForegroundColor Yellow

# Common virtual key codes
$keys = @{
    0x08 = "Backspace"
    0x09 = "Tab"
    0x0D = "Enter"
    0x10 = "Shift"
    0x11 = "Ctrl"
    0x12 = "Alt"
    0x13 = "Pause"
    0x14 = "Caps Lock"
    0x1B = "Esc"
    0x20 = "Space"
    0x21 = "Page Up"
    0x22 = "Page Down"
    0x23 = "End"
    0x24 = "Home"
    0x25 = "Left Arrow"
    0x26 = "Up Arrow"
    0x27 = "Right Arrow"
    0x28 = "Down Arrow"
    0x2C = "Print Screen"
    0x2D = "Insert"
    0x2E = "Delete"
    0x5B = "Left Win"
    0x5C = "Right Win"
    0x5D = "Menu"
    0x90 = "Num Lock"
    0x91 = "Scroll Lock"
    0xA0 = "Left Shift"
    0xA1 = "Right Shift"
    0xA2 = "Left Ctrl"
    0xA3 = "Right Ctrl"
    0xA4 = "Left Alt"
    0xA5 = "Right Alt"
}

# Add A-Z (0x41-0x5A)
for ($i = 0x41; $i -le 0x5A; $i++) {
    $keys[$i] = [char]$i
}

# Add 0-9 (0x30-0x39)
for ($i = 0x30; $i -le 0x39; $i++) {
    $keys[$i] = [char]$i
}

# Add F1-F12 (0x70-0x7B)
for ($i = 0x70; $i -le 0x7B; $i++) {
    $keys[$i] = "F$($i - 0x6F)"
}

# Monitor loop
$iteration = 0
while ($true) {
    $heldKeys = @()
    
    foreach ($keyCode in $keys.Keys) {
        $state = [KeyboardChecker]::GetAsyncKeyState($keyCode)
        # High bit set means key is down
        if ($state -band 0x8000) {
            $heldKeys += "$($keys[$keyCode]) (0x$($keyCode.ToString('X')))"
        }
    }
    
    if ($heldKeys.Count -gt 0) {
        $timestamp = Get-Date -Format "HH:mm:ss"
        Write-Host "[$timestamp] HELD KEYS: " -NoNewline -ForegroundColor Red
        Write-Host ($heldKeys -join ", ") -ForegroundColor Yellow
    } elseif ($iteration % 10 -eq 0) {
        # Print a dot every 10 iterations to show script is running
        Write-Host "." -NoNewline -ForegroundColor Green
    }
    
    Start-Sleep -Milliseconds 100
    $iteration++
}
