#include "battery_logic.h"

#include <sstream>
#include <string>

extern "C"
{
    #include "cfe.h"
    #include "osapi.h"
}

// Read the scenario selected in the configuration file
BatteryScenario LoadScenario()
{
    osal_id_t file;

    if (OS_OpenCreate(&file, "/cf/scenario.cfg",
                      OS_FILE_FLAG_NONE, OS_READ_ONLY) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(7, CFE_EVS_EventType_ERROR,
                          "Scenario file not found. Using Normal Operation.");
        return BatteryScenario::Normal;
    }

    char buffer[256] = {};
    int32 bytesRead = OS_read(file, buffer, sizeof(buffer) - 1);

    OS_close(file);

    if (bytesRead > 0)
    {
        std::istringstream lines(std::string(buffer, bytesRead));
        std::string line;

        while (std::getline(lines, line))
        {
            // Handle Windows and Linux line endings
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            if (line == "scenario=1")
            {
                return BatteryScenario::Normal;
            }
            else if (line == "scenario=2")
            {
                return BatteryScenario::ExtendedEclipse;
            }
            else if (line == "scenario=3")
            {
                return BatteryScenario::HighLoad;
            }
        }
    }

    CFE_EVS_SendEvent(7, CFE_EVS_EventType_ERROR,
                      "Invalid scenario configuration. Using Normal Operation.");

    return BatteryScenario::Normal;
}

// Starts the battery monitor app
extern "C" void BATMON_Main(void)
{
    uint32 runStatus = CFE_ES_RunStatus_APP_RUN;

    if (CFE_EVS_Register(nullptr, 0, CFE_EVS_EventFilter_BINARY) != CFE_SUCCESS)
    {
        CFE_ES_ExitApp(CFE_ES_RunStatus_APP_ERROR);
        return;
    }

    // Load the selected scenario from the configuration file
    BatteryScenario selectedScenario = LoadScenario();
    ScenarioSettings settings = GetScenarioSettings(selectedScenario);

    const char* scenarioName = "Normal Operation";

    if (selectedScenario == BatteryScenario::ExtendedEclipse)
    {
        scenarioName = "Extended Eclipse";
    }
    else if (selectedScenario == BatteryScenario::HighLoad)
    {
        scenarioName = "High Power Consumption";
    }

    CFE_EVS_SendEvent(1, CFE_EVS_EventType_INFORMATION,
                      "Battery Monitor started: %s", scenarioName);

    const int warningThreshold = 20;
    const int criticalThreshold = 10;

    // Initialize the shared simulation
    SimulationState state = StartSimulation(
        settings, warningThreshold, criticalThreshold);

    // Run the battery simulation
    while (CFE_ES_RunLoop(&runStatus))
    {
        bool wasInSunlight = state.inSunlight;
        BatteryStatus previousStatus = state.status;

        bool phaseChanged = AdvanceSimulation(
            state, settings, warningThreshold, criticalThreshold);

        const char* phase = wasInSunlight ? "Sunlight" : "Eclipse";

        CFE_EVS_SendEvent(2, CFE_EVS_EventType_INFORMATION,
                          "Phase: %s | Battery: %d%%",
                          phase, state.batteryLevel);

        // Report events only when battery status changes
        if (state.status != previousStatus)
        {
            if (state.status == BatteryStatus::Warning)
            {
                CFE_EVS_SendEvent(3, CFE_EVS_EventType_ERROR,
                                  "Warning: Battery low at %d%%",
                                  state.batteryLevel);
            }
            else if (state.status == BatteryStatus::Critical)
            {
                CFE_EVS_SendEvent(5, CFE_EVS_EventType_ERROR,
                                  "Critical: Battery at %d%%",
                                  state.batteryLevel);
            }
            else
            {
                CFE_EVS_SendEvent(6, CFE_EVS_EventType_INFORMATION,
                                  "Battery recovered to normal: %d%%",
                                  state.batteryLevel);
            }
        }

        // Report sunlight and eclipse transitions
        if (phaseChanged)
        {
            CFE_EVS_SendEvent(4, CFE_EVS_EventType_INFORMATION,
                              "Entering %s",
                              state.inSunlight ? "Sunlight" : "Eclipse");
        }

        OS_TaskDelay(1000);
    }

    CFE_ES_ExitApp(runStatus);
}