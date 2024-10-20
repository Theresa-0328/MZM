#pragma once

#include <chrono>
#include <array>

#include "myimgui.h"

class MainWindow
{
public:
    MainWindow()
    {
    }
    void ShowImguiMenu()
    {
        static std::chrono::time_point LastTimePoint = std::chrono::steady_clock::now();
        auto CurTimePoint = std::chrono::steady_clock::now();
        if (GetAsyncKeyState(VK_INSERT) &&
            (CurTimePoint - LastTimePoint >= std::chrono::milliseconds(200)))
        {
            showMenu = !showMenu;
            LastTimePoint = CurTimePoint;
        }

        if (!showMenu)
        {
            return;
        }

        ImGui::Begin("MZM");

        if (ImGui::Button("自瞄", { 80, 20 }))
        {
            ButtonState = 1;
        }
        ImGui::SameLine(NULL, 15.f);
        if (ImGui::Button("视觉", { 80, 20 }))
        {
            ButtonState = 2;
        }
        ImGui::SameLine(NULL, 15.f);
        if (ImGui::Button("杂项", { 80, 20 }))
        {
            ButtonState = 3;
        }

        switch (ButtonState)
        {
        case 1:
        {
            ImGui::TextColored(ImColor(128, 128, 128), "自瞄机器人(?)");
            ImGui::Checkbox("自瞄机器人", &Aimbot);
            ImGui::SliderFloat("FOV", &Fov, 0.f, 180.f, "%.0f");
            ImGui::Checkbox("绘制FOV范围", &DrawFov);
            ImGui::RadioButton("头", &Aimplace, 6);
            ImGui::SameLine(NULL, 15.f);
            ImGui::RadioButton("胸", &Aimplace, 4);
            ImGui::SameLine(NULL, 15.f);
            ImGui::RadioButton("肚", &Aimplace, 3);

            ImGui::RadioButton("鼠标侧键2", &Aimkey, 5);
            ImGui::SameLine(NULL, 15.f);
            ImGui::RadioButton("左键", &Aimkey, 1);
            ImGui::SameLine(NULL, 15.f);
            ImGui::RadioButton("Shift", &Aimkey, VK_SHIFT);
            break;
        }
        case 2:
        {
            ImGui::TextColored(ImColor(128, 128, 128), "视觉(?)");
            ImGui::Checkbox("显示己方队伍", &Team);
            ImGui::Checkbox("显示方框", &box);
            ImGui::Checkbox("显示动态方框", &dynamicRect);
            ImGui::Checkbox("显示血量", &Health);
            ImGui::Checkbox("显示骨骼", &Skeleton);
            break;
        }
        case 3:
        {
            ImGui::TextColored(ImColor(128, 128, 128), "杂项(?)");
            break;
        }
        default:
            break;
        }

        ImGui::End();
    }

    //private:
    bool showMenu{ true };
    int ButtonState{};
    bool Aimbot{}; //自瞄
    float Fov{}; //自瞄范围
    bool DrawFov{}; //绘制自瞄范围
    int Aimplace; //自瞄部位
    int Aimkey; //自瞄热键

    bool Team{}; //是否绘制队友
    bool box{}; //方框
    bool dynamicRect{};
    bool Health{}; //血量
    bool Skeleton{}; //骨骼
};