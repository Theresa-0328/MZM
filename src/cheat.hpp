#pragma once

#include <array>
#include <cmath>
#include <string>
#include <numbers>

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
        w = g_size.x / 2;
        h = g_size.y / 2;
        Traverse();
    }

    void Traverse()
    {
        if (!mem::Read(g_handle, clientAddress + cs2_dumper::offsets::client_dll::dwViewMatrix, &ViewMatrix, sizeof(ViewMatrix)))
        {
            return;
        }
        for (size_t i = 0; i < 32; i++)
        {
            if (!ReadLocalPawn())
            {
                continue;
            }
            if (!mem::Read(g_handle, clientAddress + 0x1A25288, &ActorPlayer.address[0], sizeof(uint8_t*)))
            {
                continue;
            }
            if (!mem::Read(g_handle, ActorPlayer.address[0] + 0x0 + i * 0x18, &ActorPlayer.address[1], sizeof(uint8_t*)))
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
            if (m.Skeleton)
            {
                DrawSkeleton();
            }
            EnterAmiQueue();
        }
        if (m.Aimbot)
        {
            Aimbot();
        }
        if (m.Fov)
        {
            ImGui::GetForegroundDrawList()->AddCircle({ w, h }, m.Fov * 8, ImColor(255, 255, 255));
        }
    }

    bool ReadLocalPawn()
    {
        if (!mem::Read(g_handle, clientAddress + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn, &LocalPlayer.address[0], sizeof(uint8_t*)))
        {
            return false;
        }
        if (!mem::Read(g_handle, LocalPlayer.address[0] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth, &LocalPlayer.Health, sizeof(int)))
        {
            return false;
        }
        if (!mem::Read(g_handle, LocalPlayer.address[0] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum, &LocalPlayer.camp, sizeof(int)))
        {
            return false;
        }
        if (!mem::Read(g_handle, LocalPlayer.address[0] + 0x38, &LocalPlayer.skeletonAddress[0], sizeof(uint8_t*)))
        {
            return false;
        }
        if (!mem::Read(g_handle, LocalPlayer.skeletonAddress[0] + 0x240 + 0x0, &LocalPlayer.skeletonAddress[1], sizeof(uint8_t*)))
        {
            return false;
        }

        uint8_t* skeletonAddress{ LocalPlayer.skeletonAddress[1] + 6 * 32 };
        mem::Read(g_handle, skeletonAddress, &LocalPlayer.Axis, sizeof(LocalPlayer.Axis));
        return true;
    }

    bool ReadActorPawn()
    {
        if (!mem::Read(g_handle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth, &ActorPlayer.Health, sizeof(int)))
        {
            return false;
        }
        if (ActorPlayer.Health <= 0)
        {
            return false;
        }

        if (!mem::Read(g_handle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum, &ActorPlayer.camp, sizeof(int)))
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

        if (!mem::Read(g_handle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_Beam::m_fHaloScale, &ActorPlayer.Axis.x, sizeof(float)))
        {
            return false;
        }
        if (!mem::Read(g_handle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_Beam::m_fAmplitude, &ActorPlayer.Axis.y, sizeof(float)))
        {
            return false;
        }
        if (!mem::Read(g_handle, ActorPlayer.address[1] + cs2_dumper::schemas::client_dll::C_Beam::m_fStartFrame, &ActorPlayer.Axis.z, sizeof(float)))
        {
            return false;
        }
        if (ActorPlayer.Axis.x == 0 && ActorPlayer.Axis.y == 0 && ActorPlayer.Axis.z == 0)
        {
            return false;
        }

        if (!mem::Read(g_handle, ActorPlayer.address[1] + 0x38, &ActorPlayer.skeletonAddress[0], sizeof(void*)))
        {
            return false;
        }
        if (!mem::Read(g_handle, ActorPlayer.skeletonAddress[0] + 0x240 + 0x0, &ActorPlayer.skeletonAddress[1], sizeof(void*)))
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

    void Bonewires(uint8_t* Address, int start, int End)
    {
        uint8_t* lin_Address;
        DirectX::XMFLOAT3 SkeletalAxis;
        float Z;
        DirectX::XMFLOAT3 lin_start;
        DirectX::XMFLOAT3 lin_End;

        //lin_Address = Address + (start - 1) * 32;
        lin_Address = Address + start * 32;
        mem::Read(g_handle, lin_Address, &SkeletalAxis.x, 4);
        mem::Read(g_handle, lin_Address + 4, &SkeletalAxis.y, 4);
        mem::Read(g_handle, lin_Address + 8, &SkeletalAxis.z, 4);
        Z = SkeletalAxis.x * ViewMatrix.m[3][0] + SkeletalAxis.y * ViewMatrix.m[3][1] + SkeletalAxis.z * ViewMatrix.m[3][2] + ViewMatrix.m[3][3];
        if (Z < 0.01f)
            return;

        lin_start.x = w + (SkeletalAxis.x * ViewMatrix.m[0][0] + SkeletalAxis.y * ViewMatrix.m[0][1] + SkeletalAxis.z * ViewMatrix.m[0][2] + ViewMatrix.m[0][3]) / Z * w;
        lin_start.y = h - (SkeletalAxis.x * ViewMatrix.m[1][0] + SkeletalAxis.y * ViewMatrix.m[1][1] + SkeletalAxis.z * ViewMatrix.m[1][2] + ViewMatrix.m[1][3]) / Z * h;

        //lin_Address = Address + (End - 1) * 32;
        lin_Address = Address + End * 32;
        mem::Read(g_handle, lin_Address, &SkeletalAxis.x, 4);
        mem::Read(g_handle, lin_Address + 4, &SkeletalAxis.y, 4);
        mem::Read(g_handle, lin_Address + 8, &SkeletalAxis.z, 4);
        Z = SkeletalAxis.x * ViewMatrix.m[3][0] + SkeletalAxis.y * ViewMatrix.m[3][1] + SkeletalAxis.z * ViewMatrix.m[3][2] + ViewMatrix.m[3][3];
        if (Z < 0.01f)
            return;

        lin_End.x = w + (SkeletalAxis.x * ViewMatrix.m[0][0] + SkeletalAxis.y * ViewMatrix.m[0][1] + SkeletalAxis.z * ViewMatrix.m[0][2] + ViewMatrix.m[0][3]) / Z * w;
        lin_End.y = h - (SkeletalAxis.x * ViewMatrix.m[1][0] + SkeletalAxis.y * ViewMatrix.m[1][1] + SkeletalAxis.z * ViewMatrix.m[1][2] + ViewMatrix.m[1][3]) / Z * h;

        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(lin_start.x, lin_start.y), ImVec2(lin_End.x, lin_End.y), ImColor(255, 255, 0), 1);
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

    void DrawSkeleton()
    {
#if 0
        //绘制骨骼索引
        uint8_t* addres{};
        float screen[2]{};
        DirectX::XMFLOAT3 Skeleton{};
        for (size_t i = 0; i < 100; i++)
        {
            addres = ActorPlayer.skeletonAddress[1] + i * 32;
            mem::Read(g_handle, addres, &Skeleton, sizeof(Skeleton));
            float world[3]{ Skeleton.x, Skeleton.y, Skeleton.z };
            if (Worldscreen2D(world, screen))
            {
                ImGui::GetForegroundDrawList()->AddText({ screen[0], screen[1] }, ImColor{ 15, 255, 19 }, std::string(std::to_string(i)).data());
            }
        }
#else
        Bonewires(ActorPlayer.skeletonAddress[1], 6, 36);
        Bonewires(ActorPlayer.skeletonAddress[1], 36, 1);
        Bonewires(ActorPlayer.skeletonAddress[1], 36, 87);
        Bonewires(ActorPlayer.skeletonAddress[1], 36, 62);
        Bonewires(ActorPlayer.skeletonAddress[1], 87, 14);
        Bonewires(ActorPlayer.skeletonAddress[1], 14, 15);
        Bonewires(ActorPlayer.skeletonAddress[1], 62, 9);
        Bonewires(ActorPlayer.skeletonAddress[1], 9, 10);
        Bonewires(ActorPlayer.skeletonAddress[1], 1, 25);
        Bonewires(ActorPlayer.skeletonAddress[1], 1, 22);
        Bonewires(ActorPlayer.skeletonAddress[1], 25, 26);
        Bonewires(ActorPlayer.skeletonAddress[1], 26, 27);
        Bonewires(ActorPlayer.skeletonAddress[1], 22, 23);
        Bonewires(ActorPlayer.skeletonAddress[1], 23, 24);
#endif
    }

    float CrosshairDistance(const DirectX::XMFLOAT2& Crosshair, const DirectX::XMFLOAT2& target)
    {
        DirectX::XMFLOAT2 offset{ Crosshair.x - target.x, Crosshair.y - target.y };
        return std::sqrtf(offset.x * offset.x + offset.y * offset.y);
    }

    void EnterAmiQueue()
    {
        FOV = m.Fov * 8;
        float worldLocation1[3]{ ActorPlayer.Axis.x, ActorPlayer.Axis.y, ActorPlayer.Axis.z + 70.f - ActorPlayer.Axis.z };
        float screen[2];
        if (Worldscreen2D(worldLocation1, screen))
        {
            float ActorDistance = CrosshairDistance({ w, h }, { screen[0], screen[1] });
            if (FOV > ActorDistance)
            {
                AimEnterAddress[0] = ActorPlayer.address[1];
                AimEnterAddress[1] = ActorPlayer.skeletonAddress[1];
            }
        }
    }

    void Aimbot()
    {
        AimAddress[0] = AimEnterAddress[0];
        AimAddress[1] = AimEnterAddress[1];
        DirectX::XMFLOAT2 AimMouse{ Aiming(LocalPlayer.Axis, AimAddress[1]) };
        int hp;
        mem::Read(g_handle, AimAddress[0] + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth, &hp, 4);
        if (hp <= 0)
        {
            return;
        }
        if (GetAsyncKeyState(5))
        {
            mem::Write(g_handle, clientAddress + cs2_dumper::offsets::client_dll::dwViewAngles, &AimMouse, sizeof(AimMouse));
        }
    }

    //自瞄算法
    DirectX::XMFLOAT2 Aiming(DirectX::XMFLOAT3 LocalAxis, uint8_t* AimAddress)
    {
        uint8_t* Aimindex;
        DirectX::XMFLOAT3 ActorAxis;
        DirectX::XMFLOAT3 AimAxis;
        DirectX::XMFLOAT2 Aimmouse;

        Aimindex = AimAddress + 6 * 32;

        mem::Read(g_handle, Aimindex, &ActorAxis.x, 4);
        mem::Read(g_handle, Aimindex + 4, &ActorAxis.y, 4);
        mem::Read(g_handle, Aimindex + 8, &ActorAxis.z, 4);

        AimAxis.x = LocalAxis.x - ActorAxis.x;
        AimAxis.y = LocalAxis.y - ActorAxis.y;
        AimAxis.z = LocalAxis.z - ActorAxis.z;

        if (AimAxis.x >= 0 && AimAxis.y >= 0)
            Aimmouse.x = atan(AimAxis.y / AimAxis.x) / std::numbers::pi * 180 + 180;

        if (AimAxis.x >= 0 && AimAxis.y >= 0)
            Aimmouse.x = atan(AimAxis.y / AimAxis.x) / std::numbers::pi * 180 + 180;

        if (AimAxis.x <= 0 && AimAxis.y >= 0)
            Aimmouse.x = atan(AimAxis.y / AimAxis.x) / std::numbers::pi * 180;

        if (AimAxis.x <= 0 && AimAxis.y <= 0)
            Aimmouse.x = atan(AimAxis.y / AimAxis.x) / std::numbers::pi * 180;

        if (AimAxis.x >= 0 && AimAxis.y <= 0)
            Aimmouse.x = atan(AimAxis.y / AimAxis.x) / std::numbers::pi * 180 - 180;

        Aimmouse.y = atan(AimAxis.z / sqrt(AimAxis.x * AimAxis.x + AimAxis.y * AimAxis.y)) / std::numbers::pi * 180;
        return DirectX::XMFLOAT2(Aimmouse.y, Aimmouse.x);
    }
    //private:

    char* clientAddress{};
    DWORD g_pid{};
    HANDLE g_handle{};
    HWND g_hwnd{};
    POINT point{};
    RECT rect{};
    float w{};
    float h{};
    DirectX::XMFLOAT2 g_size{};
    DirectX::XMFLOAT4X4 ViewMatrix{};

    ActorInfo LocalPlayer{};
    ActorInfo ActorPlayer{};

    MainWindow m{};

    uint8_t* AimAddress[2]{};
    uint8_t* AimEnterAddress[2]{};
    float FOV{};
};