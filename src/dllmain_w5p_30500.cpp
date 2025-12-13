#include "banapass.h"
#include "MinHook.h"

#define PATCH_OFFSET 0x90A7D8

// W5P 3.05.00 Function Address Offset
#define ADDR_BngRwAttach       0xD00760
#define ADDR_BngRwInit         0xCFFE20
#define ADDR_BngRwFin          0xCFFEB0
#define ADDR_BngRwGetVersion   0xD00750
#define ADDR_BngRwIsCmdExec    0xCFFF80
#define ADDR_BngRwReqLed       0xD00480
#define ADDR_BngRwReqAction    0xD00670
#define ADDR_BngRwReqBeep      0xD00570
#define ADDR_BngRwReqCancel    0xCFFFF0
#define ADDR_BngRwReqSendUrlTo 0xD001D0
#define ADDR_BngRwReqWaitTouch 0xD000C0

static uintptr_t imageBase;

static void installHooks() {
    imageBase = (uintptr_t)GetModuleHandleA(NULL);
    
    MH_Initialize();
    
    MH_CreateHook((void*)(imageBase + ADDR_BngRwAttach), (void*)BngRwAttach, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwInit), (void*)BngRwInit, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwFin), (void*)BngRwFin, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwGetVersion), (void*)BngRwGetVersion, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwIsCmdExec), (void*)BngRwIsCmdExec, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwReqLed), (void*)BngRwReqLed, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwReqAction), (void*)BngRwReqAction, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwReqBeep), (void*)BngRwReqBeep, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwReqCancel), (void*)BngRwReqCancel, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwReqSendUrlTo), (void*)BngRwReqSendUrlTo, NULL);
    MH_CreateHook((void*)(imageBase + ADDR_BngRwReqWaitTouch), (void*)BngRwReqWaitTouch, NULL);
    
    MH_EnableHook(MH_ALL_HOOKS);
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        initBanapass();
        installHooks();
        patchCardDataCheck(PATCH_OFFSET);
        CreateThread(NULL, 0, cardPollThread, NULL, 0, NULL);
    }
    else if (reason == DLL_PROCESS_DETACH) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
    return TRUE;
}
