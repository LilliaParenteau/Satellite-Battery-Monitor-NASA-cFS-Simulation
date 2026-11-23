#ifndef BATTERY_LOGIC_H
#define BATTERY_LOGIC_H

// Battery status levels
enum class BatteryStatus
{
    Normal,
    Warning,
    Critical
};

int UpdateBatteryLevel(int currentLevel, int change);

bool IsBatteryLow(int batteryLevel, int warningThreshold);

// Update battery based on sunlight or eclipse conditions
int UpdateBatteryForPhase(int currentLevel, bool inSunlight,
                          int chargeRate, int dischargeRate);

// Determine battery status using warning and critical thresholds
BatteryStatus GetBatteryStatus(int batteryLevel,
                               int warningThreshold,
                               int criticalThreshold);

#endif