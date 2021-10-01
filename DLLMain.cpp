#include "DLLMain.h"
#include <string.h>

typedef struct
{
    int type;
} BIO_METHOD;

typedef struct bio_st
{
    BIO_METHOD* method;
    void* callback;
    char* cb_arg;
    int init;
    int shutdown;
    int flags;
    int retry_reason;
    int num;
    void* ptr;
    struct bio_st* next_bio;
    struct bio_st* prev_bio;
    int refs;
    unsigned long num_read;
    unsigned long num_write;
} BIO;


typedef struct
{
    uint16_t version;
    uint16_t max_send_fragment;
    void* method;		//SSL3

    BIO* rbio;
    BIO* wbio;
    BIO* bbio;

    int rwstate;
    int in_handshake;
    void* handshake_func;

    int server;	// server/client
    int new_session;
    int quiet_shutdown;
    int shutdown;
    int state;
    int rstate;

    void* init_buf;
    void* init_msg;
    int   init_num;
    int   init_off;

    unsigned char* packet;
    unsigned int   packet_length;

} SSL;


typedef int64_t(__fastcall* SSL_write_hook)(SSL* ssl, const void* buf, int num);
SSL_write_hook of_sslWrite = NULL;


WCHAR logDir[MAX_PATH];
static uint32_t write_call_count = 0;


//{"endpointId":"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx","isActive":
unsigned char json_to_find[64] = {
    0x7B, 0x22, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69,
    0x6E, 0x74, 0x49, 0x64, 0x22, 0x3A, 0x22, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x22, 0x2C, 0x22, 0x69, 0x73,
    0x41, 0x63, 0x74, 0x69, 0x76, 0x65, 0x22, 0x3A
};

int64_t __fastcall Detour_SSL_write(SSL* ssl, const void* buf, int num)
{
    //Speed up performance
    if (num < 69 || num > 120)
    {
        return of_sslWrite(ssl, buf, num);
    }
    
    bool bFound = false;
    uintptr_t found_address_ptr = 0;

    //Search the body of the SSL_Write for our json pattern
    const BYTE* begin = static_cast<const BYTE*>(buf);
    for (size_t i = 0; i < num; i++)
    {
        if (Utils::CompareData((BYTE*)(begin + i), reinterpret_cast<const BYTE*>(json_to_find),
                            "xxxxxxxxxxxxxxx????????????????????????????????????xxxxxxxxxxxxx"))
        {
            bFound = true;
            uintptr_t current_position = (uintptr_t)(begin + i);
            found_address_ptr = current_position;
            break;
        }
    }

    if (bFound)
    {
        Log::LogW(L"[THREAD]Found SSL_Write with the matching json body pattern");

        size_t lenDif = found_address_ptr - (uintptr_t)buf;
        size_t copyLen = num - lenDif;

        char* sample_buff = (char*)malloc(copyLen);
        if (sample_buff == NULL)
        {
            Log::LogW(Log::LLevel::Error, L"[THREAD]Unable to allocate mem for sample_buff");

            //call original function
            return of_sslWrite(ssl, buf, num);
        }

        memset(sample_buff, 0, copyLen);
        memcpy(sample_buff, (PVOID)found_address_ptr, copyLen);

        char* result_ptr = strstr(sample_buff, "false"); //search for the false flag in json
        if (result_ptr == NULL)
        {
            Log::LogW(Log::LLevel::Warning, L"[THREAD]SSL_Write was triggered with presence report of active. [NO ACTION REQUIRED]");
            //doesn't exist all good move along
            free(sample_buff);

            //call original function
            return of_sslWrite(ssl, buf, num);
        }

        const char* true_json_str = "true";
        memcpy((PVOID)result_ptr, true_json_str, strlen(true_json_str));

        memcpy((PVOID)found_address_ptr, sample_buff, copyLen);
        free(sample_buff);

        Log::LogW(L"[THREAD]SSL_Write was triggered with presence report. [!!!BLOCKED!!!]");
    }

    //call original function
    return of_sslWrite(ssl, buf, num);
}


VOID HookSSLProc(PVOID lpParam)
{

    // create the log directory
    GetModuleFileName((HMODULE)lpParam, logDir, MAX_PATH);
    WCHAR* sep = wcsrchr(logDir, '\\');
    if (sep) *sep = '\0';
    wcscat_s(logDir, MAX_PATH, L"\\log");
    CreateDirectory(logDir, 0);

    // Open Log/create
    std::wstring logFilePath = std::wstring(logDir) + L"\\AnonPresence.log";
    if (!Log::OpenW(logFilePath))
    {
        printf("Log Initialize failed! \n");
    }
    Log::LogW(Log::LLevel::Info, L"Thread HookSSLProc Spawned!");
    Log::LogW(Log::LLevel::Info, L"Anon Presence PID(%x)", GetCurrentProcessId());



    // Initialize MinHook.
    if (MH_Initialize() != MH_OK)
    {
        Log::LogW(Log::LLevel::Error, L"MinHook Initialize failed!");
        Log::CloseW();
        return;
    }
    Log::LogW(Log::LLevel::Info, L"MinHook Initialized");


    //Locate the module base address
    uintptr_t teams_module_handle = (uintptr_t)GetModuleHandleA("Teams.exe");
    if (teams_module_handle == NULL)
    {
        Log::LogW(Log::LLevel::Error, L"Unable to find module handle for Teams.");
        Log::LogW(Log::LLevel::Error, L"Terminating Thread HookSSLProc..");
        Log::CloseW();
        return;
    }
    Log::LogW(Log::LLevel::Info, L"Teams Module: 0x%llX", teams_module_handle);


    /*
        .text:0000000143DE1DE4 E8 A7 B1 F0 FC                                      call    v8::HeapObjectStatistics::operator=(v8::HeapObjectStatistics const &)
        .text:0000000143DE1DE9 E8 62 02 97 FE                                      call    sub_142752050
        .text:0000000143DE1DEE 44 8B 46 48                                         mov     r8d, [rsi+48h]
        .text:0000000143DE1DF2 48 8B 46 40                                         mov     rax, [rsi+40h]
        .text:0000000143DE1DF6 48 8B 50 10                                         mov     rdx, [rax+10h]
        .text:0000000143DE1DFA 48 8B 8E 28 01 00 00                                mov     rcx, [rsi+128h]
        .text:0000000143DE1E01 E8 FA 5A 8B FE                                      call    SSL_Write_142697900

        E8 ? ? ? ? E8 ? ? ? ? 44 8B 46 48 48 8B 46 40 48 8B 50 10 48 8B 8E ? ? ? ? E8 ? ? ? ?
        \xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x46\x48\x48\x8B\x46\x40\x48\x8B\x50\x10\x48\x8B\x8E\x00\x00\x00\x00\xE8\x00\x00\x00\x00, x????x????xxxxxxxxxxxxxxx????x????
    */

    //Locate the SSL_Write function of BoringSSL inside the Teams.exe module
    //There is probably a more efficent way of doing this like searching only the .text section of the PE Image but
    //this should do.
    auto ssl_write_func_target = Utils::FindPatternSafe(teams_module_handle, 
                                                        "\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x46\x48\x48\x8B\x46\x40\x48\x8B\x50\x10\x48\x8B\x8E\x00\x00\x00\x00\xE8\x00\x00\x00\x00", 
                                                                    "x????x????xxxxxxxxxxxxxxx????x????");
    if (ssl_write_func_target == 0)
    {
        Log::LogW(Log::LLevel::Error, L"Unable to find required signature for SSL_WRITE");
        Log::LogW(Log::LLevel::Warning, L"Terminating Thread HookSSLProc..");
        Log::CloseW();
        return;
    }

    //Resolve the function address from the assembly (call    SSL_Write_142697900)
    auto ssl_write_func = Utils::ResolveX64_REL(ssl_write_func_target + 29, 1, 5); 
    if (ssl_write_func == 0)
    {
        Log::LogW(Log::LLevel::Error, L"Unable to locate SSL_WRITE");
        Log::LogW(Log::LLevel::Warning, L"Terminating Thread HookSSLProc..");
        Log::CloseW();
        return;
    }
    Log::LogW(Log::LLevel::Info, L"Found SSL_WRITE: 0x%llX", ssl_write_func);

    if (MH_CreateHookEx((PVOID)ssl_write_func, &Detour_SSL_write, &of_sslWrite) != MH_OK)
    {
        Log::LogW(Log::LLevel::Error, L"MH_CreateHookEx failed! Detour_SSL_write");
        Log::LogW(Log::LLevel::Warning, L"Terminating Thread HookSSLProc..");
        Log::CloseW();
        return;
    }
    Log::LogW(Log::LLevel::Info, L"MinHook CreateHook [Detour_SSL_write] Created.");

    //Enable the hook
    MH_STATUS hresult = MH_EnableHook((PVOID)ssl_write_func);
    if (hresult != MH_OK)
    {
        Log::LogW(Log::LLevel::Error, L"MH_EnableHook failed!");
        Log::CloseW();
        return;
    }
    Log::LogW(Log::LLevel::Info, L"MinHook CreateHook [Detour_SSL_write] Enabled!");
    Log::LogW(Log::LLevel::Warning, L"Terminating Thread HookSSLProc.. (Work Complete)");
    Log::FlushLogBuffers();
    return;
}


BOOL WINAPI DllMain(HMODULE hModule,
                    DWORD  ul_reason_for_call,
                    LPVOID lpReserved
                   )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            DisableThreadLibraryCalls(hModule);
            CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(HookSSLProc), (PVOID)hModule, NULL, NULL);
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    }

    return TRUE;
}