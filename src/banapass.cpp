#include "banapass.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dns_auth.h"  

#define EXPORT __declspec(dllexport)

// Global state
bool waitingForTouch = false;
bool touchFinished   = false;
u64 touchData;
callbackTouch touchCallback;

char accessCode[21] = "00000000000000000000";
char chipId[33]     = "00000000000000000000000000000000";
u8 cardData[168];

int cardInsertKey = 'C';

static const u8 cardDataTemplate[168] = {
    0x01, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x2E, 0x58, 0x32,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x5C, 0x97, 0x44,
    0xF0, 0x88, 0x04, 0x00, 0x43, 0x26, 0x2C, 0x33, 0x00, 0x04, 0x06, 0x10, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4E, 0x42, 0x47, 0x49, 0x43, 0x36, 0x00, 0x00,
    0xFA, 0xE9, 0x69, 0x00, 0xF6, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static BOOL fileExists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

static void createCard() {
    const char hexCharacterTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char buf[64] = {0};
    srand((unsigned int)time(0));

    for (int i = 0; i < 20; i++) buf[i] = hexCharacterTable[rand() % 10];
    buf[20] = 0;
    WritePrivateProfileStringA("card", "accessCode", buf, ".\\card.ini");
    
    for (int i = 0; i < 32; i++) buf[i] = hexCharacterTable[rand() % 16];
    buf[32] = 0;
    WritePrivateProfileStringA("card", "chipId", buf, ".\\card.ini");
}

void reloadCard() {
    if (!fileExists(".\\card.ini")) createCard();
    GetPrivateProfileStringA("card", "accessCode", accessCode, accessCode, 21, ".\\card.ini");
    GetPrivateProfileStringA("card", "chipId", chipId, chipId, 33, ".\\card.ini");
}

void initBanapass() {
    memcpy(cardData, cardDataTemplate, 168);
    reloadCard();
}

// Patch the game's card data clearing function call with NOPs.
// Without this patch, the game clears card data when using virtual card emulation,
// resulting in empty card data being sent to the server.
void patchCardDataCheck(u64 patchOffset) {
    HMODULE gameModule = GetModuleHandle(NULL);
    if (!gameModule) return;
    
    u8* patchAddr = (u8*)gameModule + patchOffset;
    DWORD oldProtect;
    
    if (VirtualProtect(patchAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *(DWORD*)patchAddr = 0x90909090;
        patchAddr[4] = 0x90;
        VirtualProtect(patchAddr, 5, oldProtect, &oldProtect);
    }
}

DWORD WINAPI cardPollThread(LPVOID lpParam) {
    while (true) {
        if ((GetAsyncKeyState(cardInsertKey) & 0x8000) != 0) {
            if (waitingForTouch && touchCallback) {
                reloadCard();
                memcpy(cardData + 0x2C, chipId, 33);
                memcpy(cardData + 0x50, accessCode, 21);
                touchCallback(0, 0, cardData, touchData);
                waitingForTouch = false;
                touchFinished   = true;
            }
        }
        Sleep(16);
    }
    return 0;
}

extern "C" {

EXPORT u64 BngRwDevReset() { return 0; }
EXPORT u64 BngRwReadMifare() { return 0xFFFFFF9C; }
EXPORT void BngRwFin() { return; }
EXPORT u64 BngRwGetFwVersion() { return 0; }
EXPORT u64 BngRwGetStationID() { return 0; }
EXPORT u64 BngRwGetRetryCount() { return 0; }
EXPORT i32 BngRwIsCmdExec() { return -1; }
EXPORT u64 BngRwReqAction() { return 1; }
EXPORT u64 BngRwReqAiccAuth() { return 1; }
EXPORT u64 BngRwReqBeep() { return 1; }
EXPORT u64 BngRwReqFwCleanup() { return 1; }
EXPORT u64 BngRwReqFwVersionUp() { return 1; }
EXPORT i32 BngRwReqLatchID() { return 1; }
EXPORT u64 BngRwReqLed() { return 1; }
EXPORT i32 BngRwReqSendMail() { return 1; }
EXPORT i32 BngRwReqSendUrl() { return 1; }
EXPORT i32 BngRwReqSendUrlTo() { return 1; }
EXPORT u64 BngRwReqSetLedPower() { return 0; }
EXPORT i32 BngRwReqCancel() { return 1; }
EXPORT const char* BngRwGetVersion() { return "Ver 1.6.0"; }
EXPORT u64 BngRwAttach(i32 a1, char* a2, i32 a3, i32 a4, void* callback, u64 a6) { return 1; }
EXPORT i32 BngRwInit() { return 0; }

EXPORT i32 BngRwReqWaitTouch(u32 a1, i32 a2, u32 a3, callbackTouch callback, u64 a5) {
    if (touchFinished) {
        touchFinished = false;
        return 1;
    }
    touchCallback   = callback;
    touchData       = a5;
    waitingForTouch = true;
    return -1;
}

// Global DNS authenticator instance  
DnsAuthenticator dnsAuth;  

// Function to validate DNS  
bool validateDNS(const std::string& domain) {  
    return dnsAuth.validate(domain);  
}  

// Modified server-related export functions to validate DNS before proceeding  
int BngRwReqSendUrl(const std::string& url) {  
    if (!validateDNS(url)) {  
        return 404;  
    }  
    // Original implementation  
}  

int BngRwReqSendUrlTo(const std::string& url, const std::string& recipient) {  
    if (!validateDNS(url)) {  
        return 404;  
    }  
    // Original implementation  
}  

int BngRwReqSendMail(const std::string& subject, const std::string& body, const std::string& recipient) {  
    if (!validateDNS(recipient)) {  
        return 404;  
    }  
    // Original implementation  
} 
