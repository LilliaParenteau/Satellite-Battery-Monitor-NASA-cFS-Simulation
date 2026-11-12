extern "C"
{
    #include "cfe.h"
    #include "osapi.h"
}

// Starts the app
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

    int batteryLevel = 100;

    // Check the battery level
    while (CFE_ES_RunLoop(&runStatus))
    {
        if (batteryLevel > 0)
        {
            CFE_EVS_SendEvent(2, CFE_EVS_EventType_INFORMATION,
                                "Battery: %d%%", batteryLevel);

            if (batteryLevel <= 20)
            {
                CFE_EVS_SendEvent(3, CFE_EVS_EventType_ERROR,
                                    "Warning: Low Battery!");
            }
            
            batteryLevel -= 10;
        }

        OS_TaskDelay(1000);
    }

    CFE_ES_ExitApp(runStatus);
}   