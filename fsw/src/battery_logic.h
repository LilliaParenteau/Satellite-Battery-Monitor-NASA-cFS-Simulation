#ifndef BATTERY_LOGIC_H
#define BATTERY_LOGIC_H

int UpdateBatteryLevel(int currentLevel, int change);

bool IsBatteryLow(int batteryLevel, int warningThreshold);

// Update battery based on sunlight or eclipse conditions
int UpdateBatteryForPhase(int currentLevel, bool inSunlight,
                          int chargeRate, int dischargeRate);

#endif