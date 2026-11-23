#include "battery_logic.h"

// Update battery charge and keep it between 0% and 100%
int UpdateBatteryLevel(int currentLevel, int change)
{
    int newLevel = currentLevel + change;

    if (newLevel > 100)
    {
        newLevel = 100;
    }
    else if (newLevel < 0)
    {
        newLevel = 0;
    }

    return newLevel;
}

// Check whether the battery has reached the warning threshold
bool IsBatteryLow(int batteryLevel, int warningThreshold)
{
    return batteryLevel <= warningThreshold;
}

// Charge during sunlight and discharge during an eclipse
int UpdateBatteryForPhase(int currentLevel, bool inSunlight,
                          int chargeRate, int dischargeRate)
{
    if (inSunlight)
    {
        return UpdateBatteryLevel(currentLevel, chargeRate);
    }
    else
    {
        return UpdateBatteryLevel(currentLevel, -dischargeRate);
    }
}