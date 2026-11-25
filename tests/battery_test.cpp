#include <iostream>
#include <cassert>
#include "../fsw/src/battery_logic.h"

using namespace std;

int main()
{
    // Test normal battery discharge
    assert(UpdateBatteryLevel(100, -10) == 90);

    // Test battery charging
    assert(UpdateBatteryLevel(50, 10) == 60);

    // Battery cannot exceed 100%
    assert(UpdateBatteryLevel(95, 10) == 100);

    // Battery cannot drop below 0%
    assert(UpdateBatteryLevel(5, -10) == 0);

    // Test charging during sunlight
    assert(UpdateBatteryForPhase(50, true, 10, 5) == 60);

    // Test discharging during an eclipse
    assert(UpdateBatteryForPhase(50, false, 10, 5) == 45);

    // Test maximum battery capacity
    assert(UpdateBatteryForPhase(95, true, 10, 5) == 100);

    // Test minimum battery capacity
    assert(UpdateBatteryForPhase(3, false, 10, 5) == 0);

    // Test normal battery status
    assert(GetBatteryStatus(100, 20, 10) == BatteryStatus::Normal);
    assert(GetBatteryStatus(21, 20, 10) == BatteryStatus::Normal);

    // Test warning battery status
    assert(GetBatteryStatus(20, 20, 10) == BatteryStatus::Warning);
    assert(GetBatteryStatus(11, 20, 10) == BatteryStatus::Warning);

    // Test critical battery status
    assert(GetBatteryStatus(10, 20, 10) == BatteryStatus::Critical);
    assert(GetBatteryStatus(0, 20, 10) == BatteryStatus::Critical);

    // Test recovery to normal status
    assert(GetBatteryStatus(25, 20, 10) == BatteryStatus::Normal);

    // Test normal operation settings
    ScenarioSettings normal = GetScenarioSettings(BatteryScenario::Normal);

    assert(normal.startingLevel == 70);
    assert(normal.chargeRate == 5);
    assert(normal.dischargeRate == 10);
    assert(normal.sunlightDuration == 5);
    assert(normal.eclipseDuration == 5);

    // Test extended eclipse settings
    ScenarioSettings eclipse = GetScenarioSettings(
        BatteryScenario::ExtendedEclipse);

    assert(eclipse.startingLevel == 70);
    assert(eclipse.chargeRate == 5);
    assert(eclipse.dischargeRate == 10);
    assert(eclipse.sunlightDuration == 3);
    assert(eclipse.eclipseDuration == 10);

    // Test high power consumption settings
    ScenarioSettings highLoad = GetScenarioSettings(
        BatteryScenario::HighLoad);

    assert(highLoad.startingLevel == 70);
    assert(highLoad.chargeRate == 2);
    assert(highLoad.dischargeRate == 15);
    assert(highLoad.sunlightDuration == 5);
    assert(highLoad.eclipseDuration == 5);

    // Test simulation initialization
    SimulationState state = StartSimulation(normal, 20, 10);

    assert(state.batteryLevel == 70);
    assert(state.inSunlight);
    assert(state.phaseTime == 0);
    assert(state.status == BatteryStatus::Normal);

    // Test normal sunlight duration
    bool phaseChanged = false;

    for (int step = 0; step < 4; step++)
    {
        phaseChanged = AdvanceSimulation(state, normal, 20, 10);
        assert(!phaseChanged);
    }

    assert(state.batteryLevel == 90);
    assert(state.inSunlight);
    assert(state.phaseTime == 4);

    // Fifth step should switch to eclipse
    phaseChanged = AdvanceSimulation(state, normal, 20, 10);

    assert(phaseChanged);
    assert(state.batteryLevel == 95);
    assert(!state.inSunlight);
    assert(state.phaseTime == 0);

    // Test normal eclipse duration
    for (int step = 0; step < 4; step++)
    {
        phaseChanged = AdvanceSimulation(state, normal, 20, 10);
        assert(!phaseChanged);
    }

    assert(state.batteryLevel == 55);
    assert(!state.inSunlight);

    // Fifth eclipse step should return to sunlight
    phaseChanged = AdvanceSimulation(state, normal, 20, 10);

    assert(phaseChanged);
    assert(state.batteryLevel == 45);
    assert(state.inSunlight);
    assert(state.phaseTime == 0);

    // Test extended eclipse simulation
    SimulationState eclipseState = StartSimulation(eclipse, 20, 10);

    for (int step = 0; step < 2; step++)
    {
        phaseChanged = AdvanceSimulation(eclipseState, eclipse, 20, 10);
        assert(!phaseChanged);
    }

    phaseChanged = AdvanceSimulation(eclipseState, eclipse, 20, 10);

    assert(phaseChanged);
    assert(eclipseState.batteryLevel == 85);
    assert(!eclipseState.inSunlight);

    // Eclipse must last ten steps
    for (int step = 0; step < 9; step++)
    {
        phaseChanged = AdvanceSimulation(eclipseState, eclipse, 20, 10);
        assert(!phaseChanged);
    }

    assert(eclipseState.batteryLevel == 0);
    assert(eclipseState.status == BatteryStatus::Critical);
    assert(!eclipseState.inSunlight);

    phaseChanged = AdvanceSimulation(eclipseState, eclipse, 20, 10);

    assert(phaseChanged);
    assert(eclipseState.inSunlight);
    assert(eclipseState.phaseTime == 0);

    // Test high power consumption simulation
    SimulationState highLoadState = StartSimulation(highLoad, 20, 10);

    for (int step = 0; step < 5; step++)
    {
        phaseChanged = AdvanceSimulation(highLoadState, highLoad, 20, 10);
    }

    assert(phaseChanged);
    assert(highLoadState.batteryLevel == 80);
    assert(!highLoadState.inSunlight);

    // Discharge until the warning threshold
    for (int step = 0; step < 4; step++)
    {
        phaseChanged = AdvanceSimulation(highLoadState, highLoad, 20, 10);
    }

    assert(highLoadState.batteryLevel == 20);
    assert(highLoadState.status == BatteryStatus::Warning);

    // Next step should reach critical status
    phaseChanged = AdvanceSimulation(highLoadState, highLoad, 20, 10);

    assert(phaseChanged);
    assert(highLoadState.batteryLevel == 5);
    assert(highLoadState.status == BatteryStatus::Critical);
    assert(highLoadState.inSunlight);

    // Test recovery from critical to warning
    for (int step = 0; step < 3; step++)
    {
        phaseChanged = AdvanceSimulation(highLoadState, highLoad, 20, 10);
    }

    assert(highLoadState.batteryLevel == 11);
    assert(highLoadState.status == BatteryStatus::Warning);

    // Test recovery from warning to normal
    SimulationState recoveryState = StartSimulation(normal, 20, 10);

    recoveryState.batteryLevel = 20;
    recoveryState.status = BatteryStatus::Warning;

    phaseChanged = AdvanceSimulation(recoveryState, normal, 20, 10);

    assert(!phaseChanged);
    assert(recoveryState.batteryLevel == 25);
    assert(recoveryState.status == BatteryStatus::Normal);

    cout << "All battery logic tests passed!" << endl;

    return 0;
}