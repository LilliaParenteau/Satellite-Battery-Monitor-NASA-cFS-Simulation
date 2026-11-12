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

    // Keeps the app running
    while (CFE_ES_RunLoop(&runStatus))
    {
        OS_TaskDelay(1000);
    }

    CFE_ES_ExitApp(runStatus);
}   