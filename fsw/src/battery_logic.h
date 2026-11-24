#ifndef BATTERY_LOGIC_H
#define BATTERY_LOGIC_H

// Battery status levels
enum class BatteryStatus
{
    Normal,
    Warning,
    Critical
};

// Available simulation scenarios
enum class BatteryScenario
{
    Normal,
    ExtendedEclipse,
    HighLoad
};

// Settings for each simulation scenario
struct ScenarioSettings
{
    int startingLevel;
    int chargeRate;
    int dischargeRate;
    int sunlightDuration;
    int eclipseDuration;
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

// Get settings for the selected simulation scenario
ScenarioSettings GetScenarioSettings(BatteryScenario scenario);

#endif