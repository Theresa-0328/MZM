#pragma once

#include <array>
#include <string>

#include <offsets.hpp>
#include <client_dll.hpp>
#include <DirectXMath.h>

#include "Mem.h"
#include "myimgui.h"
#include "MainWindow.hpp"

struct ActorInfo
{
    uint8_t* address[2];
    uint8_t* skeletonAddress[2];
    int Health;
    int camp;
    DirectX::XMFLOAT3 Axis{};
    DirectX::XMFLOAT2 BoxPosition; // 左上角坐标 (x, y)
    DirectX::XMFLOAT2 BoxSize; // 宽度和高度 (width, height)
};

class Cheat
{
public:
    Cheat()
    {
    }

    void fun()
    {
        m.ShowImguiMenu();
        GetClientRect(g_hwnd, &rect);
        ClientToScreen(g_hwnd, &point);
        g_size = std::move(DirectX::XMFLOAT2(rect.right, rect.bottom));

        Traverse();
    }

    void Traverse()
    {
        if (!mem::Read(g_hanle, clientAddress + cs2_dumper::offsets::client_dll::dwViewMatrix, &ViewMatrix, sizeof(ViewMatrix)))
        {
            return;
        }
        for (size_t i = 0; i < 32; i++)
        {
            if (!ReadLocalPawn())
            {
                continue;
            }
            if (!mem::Read(g_hanle, clientAddress + 0x1A23138, &ActorPlayer.address[0], sizeof(uint8_t*)))
            {
                continue;
            }
            if (!mem::Read(g_hanle, ActorPlayer.address[0] + 0x0 + i * 0x18, &ActorPlayer.address[1], sizeof(uint8_t*)))
            {
                continue;
            }
            if (ActorPlayer.address[1] == nullptr || ActorPlayer.address[1] == LocalPlayer.address[0])
            {
                continue;
            }
            if (!ReadActorPawn())
            {
                continue;
            }

            //if (!PaintTest1())
            //{
            //	continue;
            //}
            The2DboxSize();
            if (m.box)
            {
                DrawBox();
            }
            if (m.Health)
            {
                DrawHealth();
            }
        }
        if (m.Fov)
        {
            ImGui::GetForegroundDrawList()->AddCircle({ static_cast<float>(rect.right / 2), static_cast<float>(rect.bottom / 2) }, m.Fov * 8, ImColor(255, 255, 255));
        }
    }

    bool ReadLocalPawn()
    {
        if (!mem::Read(g_hanle, clientAddress + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn, &LocalPlayer.address[0], sizeof(uint8_t*)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, LocalPlayer.address[0] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth, &LocalPlayer.Health, sizeof(int)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, LocalPlayer.address[0] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum, &LocalPlayer.camp, sizeof(int)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, LocalPlayer.address[0] + 0x38, &LocalPlayer.skeletonAddress[0], sizeof(uint8_t*)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, LocalPlayer.skeletonAddress[0] + 0x240 + 0x0, &LocalPlayer.skeletonAddress[1], sizeof(uint8_t*)))
        {
            return false;
        }
        return true;
    }

    bool ReadActorPawn()
    {
        if (!mem::Read(g_hanle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth, &ActorPlayer.Health, sizeof(int)))
        {
            return false;
        }
        if (ActorPlayer.Health <= 0)
        {
            return false;
        }

        if (!mem::Read(g_hanle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum, &ActorPlayer.camp, sizeof(int)))
        {
            return false;
        }
        if (ActorPlayer.camp > 3 || ActorPlayer.camp < 2)
        {
            return false;
        }
        if (m.Team == false && ActorPlayer.camp == LocalPlayer.camp)
        {
            return false;
        }

        if (!mem::Read(g_hanle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_Beam::m_fHaloScale, &ActorPlayer.Axis.x, sizeof(float)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_Beam::m_fAmplitude, &ActorPlayer.Axis.y, sizeof(float)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_Beam::m_fStartFrame, &ActorPlayer.Axis.z, sizeof(float)))
        {
            return false;
        }
        if (ActorPlayer.Axis.x == 0 && ActorPlayer.Axis.y == 0 && ActorPlayer.Axis.z == 0)
        {
            return false;
        }

        if (!mem::Read(g_hanle, ActorPlayer.address[1] + 0x38, &ActorPlayer.skeletonAddress[0], sizeof(void*)))
        {
            return false;
        }
        if (!mem::Read(g_hanle, ActorPlayer.skeletonAddress[0] + 0x240 + 0x0, &ActorPlayer.skeletonAddress[1], sizeof(void*)))
        {
            return false;
        }
        if (ActorPlayer.skeletonAddress[1] == nullptr || ActorPlayer.skeletonAddress[1] == LocalPlayer.skeletonAddress[1])
        {
            return false;
        }
        return true;
    }

    // 3D 坐标转投影后的2D 坐标
    bool Worldscreen2D(float world[3], float screen[2]) const
    {
        float x = ViewMatrix.m[0][0] * world[0] + ViewMatrix.m[0][1] * world[1] + ViewMatrix.m[0][2] * world[2] + ViewMatrix.m[0][3] * 1;
        float y = ViewMatrix.m[1][0] * world[0] + ViewMatrix.m[1][1] * world[1] + ViewMatrix.m[1][2] * world[2] + ViewMatrix.m[1][3] * 1;
        float w = ViewMatrix.m[3][0] * world[0] + ViewMatrix.m[3][1] * world[1] + ViewMatrix.m[3][2] * world[2] + ViewMatrix.m[3][3] * 1;
        if (w < 0.01f)
        {
            return false;
        }
        x = x / w;
        y = y / w;
        float halfwide = g_size.x * 0.5f;
        float halfhigh = g_size.y * 0.5f;
        screen[0] = (halfwide * x) + (halfwide + x);
        screen[1] = -(halfhigh * y) + (halfhigh + y);
        return true;
    }

    bool PaintTest1()
    {
        float a[3]{ ActorPlayer.Axis.x, ActorPlayer.Axis.y, ActorPlayer.Axis.z };
        float b[2]{};
        if (!Worldscreen2D(a, b))
        {
            return false;
        }
        ImGui::GetForegroundDrawList()->AddText({ b[0], b[1] }, ImColor(255, 0, 0), std::string(std::to_string(ActorPlayer.Health)).c_str());
        return true;
    }

    bool The2DboxSize()
    {
        float worldLocation1[3]{ ActorPlayer.Axis.x, ActorPlayer.Axis.y, ActorPlayer.Axis.z };
        float b1[2]{};
        if (!Worldscreen2D(worldLocation1, b1))
        {
            return false;
        }

        float worldLocation2[3]{ ActorPlayer.Axis.x, ActorPlayer.Axis.y, ActorPlayer.Axis.z + 70 }; //角色高度
        float b2[2]{};
        if (!Worldscreen2D(worldLocation2, b2))
        {
            return false;
        }
        ActorPlayer.BoxSize.x = b2[0] - (b1[1] - b2[1]) / 4.0f;
        ActorPlayer.BoxSize.y = b2[1];
        ActorPlayer.BoxPosition.x = b1[0] + (b1[1] - b2[1]) / 4.0f;
        ActorPlayer.BoxPosition.y = b1[1];
        return true;
    }

    void DrawBox()
    {
        ImGui::GetForegroundDrawList()->AddRect({ ActorPlayer.BoxSize.x, ActorPlayer.BoxSize.y },
                                                { ActorPlayer.BoxPosition.x, ActorPlayer.BoxPosition.y },
                                                ImColor(255, 0, 0), 2);
    }

    void DrawHealth()
    {
        float health{ (ActorPlayer.Health / 100.f) * (ActorPlayer.BoxSize.y - ActorPlayer.BoxPosition.y) };
        ImGui::GetForegroundDrawList()->AddRect(
            { ActorPlayer.BoxSize.x - 7, ActorPlayer.BoxSize.y },
            { ActorPlayer.BoxSize.x - 2, ActorPlayer.BoxPosition.y },
            ImColor(0, 0, 0));
        ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(
            { ActorPlayer.BoxSize.x - 3, ActorPlayer.BoxPosition.y + health },
            { ActorPlayer.BoxSize.x - 6, ActorPlayer.BoxPosition.y }, ImColor(15, 255, 19), ImColor(95, 170, 13), ImColor(175, 85, 6), ImColor(255, 0, 0));
    }

    //private:

    char* clientAddress{};
    DWORD g_pid{};
    HANDLE g_hanle{};
    HWND g_hwnd{};
    POINT point{};
    RECT rect;
    DirectX::XMFLOAT2 g_size{};
    DirectX::XMFLOAT4X4 ViewMatrix{};
    ActorInfo LocalPlayer{};
    ActorInfo ActorPlayer{};
    MainWindow m;
};