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

    cout << "All battery logic tests passed!" << endl;

    return 0;
}