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

    // Test low battery warnings
    assert(IsBatteryLow(20, 20));
    assert(IsBatteryLow(10, 20));
    assert(!IsBatteryLow(30, 20));

    // Test battery charging during sunlight
    assert(UpdateBatteryForPhase(50, true, 10, 5) == 60);

    // Test battery discharging during an eclipse
    assert(UpdateBatteryForPhase(50, false, 10, 5) == 45);

    // Test maximum battery capacity during sunlight
    assert(UpdateBatteryForPhase(95, true, 10, 5) == 100);

    // Test minimum battery capacity during an eclipse
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

    // Test normal operation scenario
    ScenarioSettings normal = GetScenarioSettings(BatteryScenario::Normal);
    assert(normal.startingLevel == 70);
    assert(normal.chargeRate == 5);
    assert(normal.dischargeRate == 10);
    assert(normal.sunlightDuration == 5);
    assert(normal.eclipseDuration == 5);

    // Test extended eclipse scenario
    ScenarioSettings eclipse = GetScenarioSettings(BatteryScenario::ExtendedEclipse);
    assert(eclipse.startingLevel == 70);
    assert(eclipse.chargeRate == 5);
    assert(eclipse.dischargeRate == 10);
    assert(eclipse.sunlightDuration == 3);
    assert(eclipse.eclipseDuration == 10);

    // Test high power consumption scenario
    ScenarioSettings highLoad = GetScenarioSettings(BatteryScenario::HighLoad);
    assert(highLoad.startingLevel == 70);
    assert(highLoad.chargeRate == 2);
    assert(highLoad.dischargeRate == 15);
    assert(highLoad.sunlightDuration == 5);
    assert(highLoad.eclipseDuration == 5);

    cout << "All battery logic tests passed!" << endl;

    return 0;
}