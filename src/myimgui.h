#pragma once
#include <iostream>

#include <imconfig.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <imstb_rectpack.h>
#include <imstb_textedit.h>
#include <imstb_truetype.h>

#include <Windows.h>
#include <tlhelp32.h>
#include <d3d11.h>
#include <dwmapi.h>

//WINUSERAPI SHORT WINAPI GetAsyncKeyState(int vKey);

namespace myimgui
{
typedef void (*myFun)(void); //函数指针
struct windowinfo //窗口信息结构体
{
    HWND hwnd = nullptr; //窗口句柄
    DWORD pid = (DWORD) nullptr; //进程标识符
    HANDLE handle = nullptr; //进程句柄

    ImVec2 pos = { 0, 0 }; //窗口位置
    ImVec2 size = { 0, 0 }; //窗口尺寸

    LPCWSTR ClassName = nullptr; //窗口类名
    LPCWSTR TitleName = nullptr; //窗口标题
};
inline windowinfo mywindow; //透明窗口的窗口信息
inline windowinfo gamewindow; //游戏窗口的窗口信息

inline static ID3D11Device* g_pd3dDevice = nullptr;
inline static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
inline static IDXGISwapChain* g_pSwapChain = nullptr;
inline static bool g_SwapChainOccluded = false;
inline static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
inline static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

//参数1:类名 参数2:标题名 参数3:回调 参数4:字体路径 参数5:字体大小 参数6:垂直同步 参数7:菜单样式 0为黑色,1为白色
int CreateWindow_Violet(LPCWSTR class_name, LPCWSTR title_name, myimgui::myFun Fun, const char* Fontsname, float Fonts_size = 18.0f, bool Synclnterval = true, int Menustyle = 0);
void UpdateWindow(); //更新窗口
bool CreateDeviceD3D(HWND hWnd); //创建D3D设备
void CleanupDeviceD3D(); //清理D3D设备
void CreateRenderTarget(); //创建目标渲染视图
void CleanupRenderTarget(); //清理目标渲染视图
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //Win32窗口消息处理函数
}