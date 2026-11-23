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

    cout << "All battery logic tests passed!" << endl;

    return 0;
}