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

// Current state of the battery simulation
struct SimulationState
{
    int batteryLevel;
    bool inSunlight;
    int phaseTime;
    BatteryStatus status;
};

// Update battery charge and keep it between 0% and 100%
int UpdateBatteryLevel(int currentLevel, int change);

// Update the battery during sunlight or eclipse
int UpdateBatteryForPhase(int currentLevel, bool inSunlight,
                          int chargeRate, int dischargeRate);

// Determine battery status
BatteryStatus GetBatteryStatus(int batteryLevel,
                               int warningThreshold,
                               int criticalThreshold);

// Get settings for the selected scenario
ScenarioSettings GetScenarioSettings(BatteryScenario scenario);

// Set up a new simulation
SimulationState StartSimulation(const ScenarioSettings& settings,
                                int warningThreshold,
                                int criticalThreshold);

// Advance the simulation by one step
// Returns true when the sunlight/eclipse phase changes
bool AdvanceSimulation(SimulationState& state,
                       const ScenarioSettings& settings,
                       int warningThreshold,
                       int criticalThreshold);

#endif