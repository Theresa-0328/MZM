#pragma once

#include <iostream>

#include <Windows.h>
#include <tlhelp32.h>

//WINUSERAPI SHORT WINAPI GetAsyncKeyState(int vKey);

namespace mem
{
inline char* GetModule(DWORD PID, const wchar_t* ModuleName)
{
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }

    MODULEENTRY32W module_info;
    ZeroMemory(&module_info, sizeof(module_info));
    module_info.dwSize = sizeof(module_info);

    bool ret = Module32FirstW(handle, &module_info);
    if (!ret)
    {
        return nullptr;
    }

    do
    {
        //判断模块名是否相等
        if (wcscmp(ModuleName, module_info.szModule) == 0)
        {
            //如果找到了就打印模块的地址 并返回该模块的地址
            //printf("%ws %llx\n", module_info.szModule, module_info.modBaseAddr);
            CloseHandle(handle);
            return (char*)(module_info.modBaseAddr);
        }
    } while (Module32NextW(handle, &module_info));

    CloseHandle(handle);
    return nullptr;
}

inline bool Read(HANDLE processHandle, void* memoryAddress, void* buffer, int bytesToRead)
{
    if (ReadProcessMemory(processHandle, memoryAddress, buffer, bytesToRead, nullptr))
        return true;
    return false;
}

inline bool Write(HANDLE processHandle, void* memoryAddress, void* dataToWrite, int bytesToWrite)
{
    if (WriteProcessMemory(processHandle, memoryAddress, dataToWrite, bytesToWrite, nullptr))
        return true;
    return false;
}

}