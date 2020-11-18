#include <thread>
#include "CustomAPI.h"

using RunFileInternal_t = int(*)(uint64_t, const char*, std::function<int(char*)>);
using LoadSystemFileInternal_t = int(*)(uint64_t, const char*);
char LSFIShell[] = { 0x55, 0x56, 0x57, 0x53, 0x48, 0x83, 0xEC, 0x38, 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69 };


uint64_t csLuaBase;
uint64_t grabbedInstance;
uint64_t allocLSFI;

int _fastcall LoadSystemFileInternal(uint64_t luaRuntime, const char* scriptFile) {
    if (!allocLSFI) {
        allocLSFI = reinterpret_cast<uint64_t>(VirtualAlloc(NULL, sizeof(LSFIShell), MEM_COMMIT, 0x40));
        memcpy((void*)allocLSFI, (void*)LSFIShell, sizeof(LSFIShell));
        *(uint64_t*)(allocLSFI + 14) = csLuaBase + 0x289A8;
    }

    return ((LoadSystemFileInternal_t)(allocLSFI))(luaRuntime, scriptFile);
}

int LoadSystemFile(uint64_t luaRuntime, const char* scriptFile) {
    *(BYTE*)(CustomAPI::GetModuleA("adhesive") + 0x49288C) = 1;
    auto result = ((RunFileInternal_t)(csLuaBase + 0x28A90))(luaRuntime, scriptFile, std::bind(&LoadSystemFileInternal, luaRuntime, std::placeholders::_1));
    return result;
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  callReason, LPVOID lpReserved ){
    if (callReason == DLL_PROCESS_ATTACH) {
        std::thread([&] {
            while (!csLuaBase)
                csLuaBase = CustomAPI::GetModuleA("citizen-scripting-lua");
            
            for (;;) {
                uint64_t* c1 = (uint64_t*)(csLuaBase + 0x63CFA0);
                if (*c1 != 0)
                    grabbedInstance = *c1;

                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        }).detach();

        std::thread([&] {

            for (;;) {
                if (!grabbedInstance)
                    std::this_thread::sleep_for(std::chrono::seconds(5));

                if (GetAsyncKeyState(VK_NUMPAD0)) {
                    LoadSystemFile(grabbedInstance, "C:/exec/dopamine.lua");
                    MessageBox(NULL, L"Open key is : TAB.", 776Exec | Dopamine", MB_OK);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                if (GetAsyncKeyState(VK_NUMPAD1)) {
                    LoadSystemFile(grabbedInstance, "C:/exec/absolute.lua");
                    MessageBox(NULL, L"Open key is : 9.", 776Exec | Absulute", MB_OK);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                if (GetAsyncKeyState(VK_NUMPAD2)) {
                    LoadSystemFile(grabbedInstance, "C:/exec/Infinity.lua");
                    MessageBox(NULL, L"Open key is : ALT.", 776Exec | Infinity", MB_OK);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
                if (GetAsyncKeyState(VK_NUMPAD3)) {
                    LoadSystemFile(grabbedInstance, "C:/exec/custom.lua");
                    MessageBox(NULL, L"There is your custom enjoy", 776Exec | Custom", MB_OK);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

        }).detach();
    }

    return TRUE;
}

