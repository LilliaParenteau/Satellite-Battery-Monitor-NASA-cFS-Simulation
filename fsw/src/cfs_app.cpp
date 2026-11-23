#include "battery_logic.h"

extern "C"
{
    #include "cfe.h"
    #include "osapi.h"
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

    CFE_EVS_SendEvent(1, CFE_EVS_EventType_INFORMATION,
                      "Battery Monitor started");

    int batteryLevel = 70;
    int warningThreshold = 20;
    int criticalThreshold = 10;
    int chargeRate = 5;
    int dischargeRate = 10;

    bool inSunlight = true;
    int phaseTime = 0;
    int phaseDuration = 5;

    BatteryStatus previousStatus = GetBatteryStatus(
        batteryLevel, warningThreshold, criticalThreshold);

    // Simulate battery charging and discharging
    while (CFE_ES_RunLoop(&runStatus))
    {
        batteryLevel = UpdateBatteryForPhase(
            batteryLevel, inSunlight, chargeRate, dischargeRate);

        const char* phase = inSunlight ? "Sunlight" : "Eclipse";

        CFE_EVS_SendEvent(2, CFE_EVS_EventType_INFORMATION,
                          "Phase: %s | Battery: %d%%",
                          phase, batteryLevel);

        // Check the current battery status
        BatteryStatus currentStatus = GetBatteryStatus(
            batteryLevel, warningThreshold, criticalThreshold);

        // Report warnings only when the status changes
        if (currentStatus != previousStatus)
        {
            if (currentStatus == BatteryStatus::Warning)
            {
                CFE_EVS_SendEvent(3, CFE_EVS_EventType_ERROR,
                                  "Warning: Battery low at %d%%",
                                  batteryLevel);
            }
            else if (currentStatus == BatteryStatus::Critical)
            {
                CFE_EVS_SendEvent(5, CFE_EVS_EventType_ERROR,
                                  "Critical: Battery at %d%%",
                                  batteryLevel);
            }
            else
            {
                CFE_EVS_SendEvent(6, CFE_EVS_EventType_INFORMATION,
                                  "Battery recovered to normal: %d%%",
                                  batteryLevel);
            }

            previousStatus = currentStatus;
        }

        phaseTime++;

        // Switch between sunlight and eclipse
        if (phaseTime >= phaseDuration)
        {
            inSunlight = !inSunlight;
            phaseTime = 0;

            CFE_EVS_SendEvent(4, CFE_EVS_EventType_INFORMATION,
                              "Entering %s",
                              inSunlight ? "Sunlight" : "Eclipse");
        }

        OS_TaskDelay(1000);
    }

    CFE_ES_ExitApp(runStatus);
}