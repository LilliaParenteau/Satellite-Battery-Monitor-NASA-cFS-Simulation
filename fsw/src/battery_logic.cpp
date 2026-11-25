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

// Charge during sunlight and discharge during an eclipse
int UpdateBatteryForPhase(int currentLevel, bool inSunlight,
                          int chargeRate, int dischargeRate)
{
    if (inSunlight)
    {
        return UpdateBatteryLevel(currentLevel, chargeRate);
    }

    return UpdateBatteryLevel(currentLevel, -dischargeRate);
}

// Determine the current battery status
BatteryStatus GetBatteryStatus(int batteryLevel,
                               int warningThreshold,
                               int criticalThreshold)
{
    if (batteryLevel <= criticalThreshold)
    {
        return BatteryStatus::Critical;
    }
    else if (batteryLevel <= warningThreshold)
    {
        return BatteryStatus::Warning;
    }

    return BatteryStatus::Normal;
}

// Return the settings for the selected scenario
ScenarioSettings GetScenarioSettings(BatteryScenario scenario)
{
    switch (scenario)
    {
        case BatteryScenario::Normal:
            return {70, 5, 10, 5, 5};

        case BatteryScenario::ExtendedEclipse:
            return {70, 5, 10, 3, 10};

        case BatteryScenario::HighLoad:
            return {70, 2, 15, 5, 5};
    }

    return {70, 5, 10, 5, 5};
}

// Initialize the battery simulation
SimulationState StartSimulation(const ScenarioSettings& settings,
                                int warningThreshold,
                                int criticalThreshold)
{
    SimulationState state;

    state.batteryLevel = settings.startingLevel;
    state.inSunlight = true;
    state.phaseTime = 0;
    state.status = GetBatteryStatus(
        state.batteryLevel, warningThreshold, criticalThreshold);

    return state;
}

// Advance the simulation by one step
bool AdvanceSimulation(SimulationState& state,
                       const ScenarioSettings& settings,
                       int warningThreshold,
                       int criticalThreshold)
{
    state.batteryLevel = UpdateBatteryForPhase(
        state.batteryLevel,
        state.inSunlight,
        settings.chargeRate,
        settings.dischargeRate);

    state.status = GetBatteryStatus(
        state.batteryLevel, warningThreshold, criticalThreshold);

    state.phaseTime++;

    int phaseDuration = state.inSunlight
        ? settings.sunlightDuration
        : settings.eclipseDuration;

    // Switch phases when the current duration is reached
    if (state.phaseTime >= phaseDuration)
    {
        state.inSunlight = !state.inSunlight;
        state.phaseTime = 0;

        return true;
    }

    return false;
}