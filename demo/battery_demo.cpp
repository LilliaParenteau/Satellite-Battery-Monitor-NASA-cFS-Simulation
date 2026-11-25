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

    const int warningThreshold = 20;
    const int criticalThreshold = 10;

    // Initialize the shared simulation
    SimulationState state = StartSimulation(
        settings, warningThreshold, criticalThreshold);

    cout << "\nRunning: " << scenarioName << "\n";
    cout << "Starting battery: " << state.batteryLevel << "%\n\n";

    // Run 40 simulation steps
    for (int step = 1; step <= 40; step++)
    {
        // Record the phase and status before advancing
        bool wasInSunlight = state.inSunlight;
        BatteryStatus previousStatus = state.status;

        bool phaseChanged = AdvanceSimulation(
            state, settings, warningThreshold, criticalThreshold);

        cout << "Step " << step << " | "
             << (wasInSunlight ? "Sunlight" : "Eclipse")
             << " | Battery: " << state.batteryLevel << "%\n";

        // Report changes in battery status
        if (state.status != previousStatus)
        {
            if (state.status == BatteryStatus::Warning)
            {
                cout << "  WARNING: Low battery!\n";
            }
            else if (state.status == BatteryStatus::Critical)
            {
                cout << "  CRITICAL: Battery level dangerously low!\n";
            }
            else
            {
                cout << "  Battery recovered to normal.\n";
            }
        }

        // Report changes between sunlight and eclipse
        if (phaseChanged)
        {
            cout << "  Entering "
                 << (state.inSunlight ? "Sunlight" : "Eclipse")
                 << "\n";
        }
    }

    cout << "\nSimulation complete.\n";

    return 0;
}