#include <iostream>

#include "cheat.hpp"

Cheat cheat;

void callback()
{
    cheat.fun();
}

void initGameCheat()
{
    //cheat.g_hwnd = FindWindowW(L"SDL_app", L"反恐精英：全球攻势");
    cheat.g_hwnd = FindWindowW(L"SDL_app", L"Counter-Strike 2");
    GetWindowThreadProcessId(cheat.g_hwnd, &cheat.g_pid);
    cheat.g_hanle = OpenProcess(PROCESS_ALL_ACCESS, true, cheat.g_pid);
    cheat.clientAddress = mem::GetModule(cheat.g_pid, L"client.dll");
}

int main()
{
    initGameCheat();
    //myimgui::CreateWindow_Violet(L"SDL_app\000", L"反恐精英：全球攻势", &callback, "C:\\Windows\\Fonts\\msyh.ttc", 18.0f, false, 0);
    myimgui::CreateWindow_Violet(L"SDL_app\000", L"Counter-Strike 2", &callback, "C:\\Windows\\Fonts\\msyh.ttc", 18.0f, false, 0);
    return 0;
}
