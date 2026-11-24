#include <iostream>
#include "../fsw/src/battery_logic.h"

using namespace std;

int main()
{
    cout << "Satellite Battery Monitor Demo\n";
    cout << "------------------------------\n";
    cout << "1. Normal Operation\n";
    cout << "2. Extended Eclipse\n";
    cout << "3. High Power Consumption\n";
    cout << "\nSelect a scenario (1-3): ";

    int choice;
    cin >> choice;

    if (!cin || choice < 1 || choice > 3)
    {
        cout << "Invalid scenario selection.\n";
        return 1;
    }

    BatteryScenario scenario = BatteryScenario::Normal;
    const char* scenarioName = "Normal Operation";

    if (choice == 2)
    {
        scenario = BatteryScenario::ExtendedEclipse;
        scenarioName = "Extended Eclipse";
    }
    else if (choice == 3)
    {
        scenario = BatteryScenario::HighLoad;
        scenarioName = "High Power Consumption";
    }

    ScenarioSettings settings = GetScenarioSettings(scenario);

    int batteryLevel = settings.startingLevel;
    int warningThreshold = 20;
    int criticalThreshold = 10;

    bool inSunlight = true;
    int phaseTime = 0;

    BatteryStatus previousStatus = GetBatteryStatus(
        batteryLevel, warningThreshold, criticalThreshold);

    cout << "\nRunning: " << scenarioName << "\n";
    cout << "Starting battery: " << batteryLevel << "%\n\n";

    // Run 40 simulation steps
    for (int step = 1; step <= 40; step++)
    {
        batteryLevel = UpdateBatteryForPhase(
            batteryLevel,
            inSunlight,
            settings.chargeRate,
            settings.dischargeRate);

        cout << "Step " << step << " | "
             << (inSunlight ? "Sunlight" : "Eclipse")
             << " | Battery: " << batteryLevel << "%\n";

        // Report changes in battery status
        BatteryStatus currentStatus = GetBatteryStatus(
            batteryLevel, warningThreshold, criticalThreshold);

        if (currentStatus != previousStatus)
        {
            if (currentStatus == BatteryStatus::Warning)
            {
                cout << "  WARNING: Low battery!\n";
            }
            else if (currentStatus == BatteryStatus::Critical)
            {
                cout << "  CRITICAL: Battery level dangerously low!\n";
            }
            else
            {
                cout << "  Battery recovered to normal.\n";
            }

            previousStatus = currentStatus;
        }

        phaseTime++;

        int phaseDuration = inSunlight
            ? settings.sunlightDuration
            : settings.eclipseDuration;

        // Switch between sunlight and eclipse
        if (phaseTime >= phaseDuration)
        {
            inSunlight = !inSunlight;
            phaseTime = 0;
        }
    }

    cout << "\nSimulation complete.\n";

    return 0;
}