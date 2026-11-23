#ifndef BATTERY_LOGIC_H
#define BATTERY_LOGIC_H

int UpdateBatteryLevel(int currentLevel, int change);
bool IsBatteryLow(int batteryLevel, int warningThreshold);

#endif