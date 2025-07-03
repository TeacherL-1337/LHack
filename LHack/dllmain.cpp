#include <Windows.h>
#include "src/Hook/hookManger.h"
#include "src/Gui/gui.h"

DWORD WINAPI LhackThread(HMODULE hModule)
{
    Gui* gui = new Gui();

    while (true)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            gui->Show();
        }

        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            break;
        }

        Sleep(1);
    }

    hookManger->DisableAll();
    gui->DisableAll();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

bool APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)LhackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}