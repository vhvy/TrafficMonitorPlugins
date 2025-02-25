﻿#include "stdafx.h"
#include "Common.h"
#include "HardwareMonitor.h"
#include "../utilities/IniHelper.h"

namespace HardwareMonitor
{
    std::wstring Common::StringToStdWstring(System::String^ str)
    {
        if (str == nullptr)
        {
            return std::wstring();
        }
        else
        {
            const wchar_t* chars = (const wchar_t*)(Runtime::InteropServices::Marshal::StringToHGlobalUni(str)).ToPointer();
            std::wstring os = chars;
            Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chars));
            return os;
        }
    }

    String^ Common::GetTranslatedString(String^ str)
    {
        String^ translated = MonitorGlobal::Instance()->GetString(str);
        if (translated == nullptr || translated->Length == 0)
            return str;
        else
            return translated;
    }

    void Common::SetButtonIcon(Button^ button, String^ resName)
    {
        Icon^ icon = MonitorGlobal::Instance()->GetIcon(resName);
        if (icon != nullptr && button != nullptr)
        {
            button->Image = icon->ToBitmap(); // 将 Icon 转换为 Bitmap
            button->ImageAlign = ContentAlignment::MiddleCenter; // 设置图标对齐方式
            button->TextImageRelation = TextImageRelation::ImageBeforeText; // 设置图标和文本的关系
        }
    }

    void Common::SwapListBoxItems(ListBox^ listBox, int index1, int index2)
    {
        // 检查索引是否有效
        if (index1 < 0 || index1 >= listBox->Items->Count ||
            index2 < 0 || index2 >= listBox->Items->Count)
        {
            return;
        }

        // 获取两个项的值
        Object^ item1 = listBox->Items[index1];
        Object^ item2 = listBox->Items[index2];

        // 交换两个项的值
        listBox->Items[index1] = item2;
        listBox->Items[index2] = item1;
    }

    void Common::SaveFormSize(Form^ form, const std::wstring& name)
    {
        if (form != nullptr)
        {
            utilities::CIniHelper ini(CHardwareMonitor::GetInstance()->GetConfigPath());
            ini.WriteInt(L"form_size", (name + L"_width").c_str(), form->Size.Width);
            ini.WriteInt(L"form_size", (name + L"_height").c_str(), form->Size.Height);
            ini.Save();
        }
    }

    void Common::LoadFormSize(Form^ form, const std::wstring& name)
    {
        if (form != nullptr)
        {
            utilities::CIniHelper ini(CHardwareMonitor::GetInstance()->GetConfigPath());
            int width = ini.GetInt(L"form_size", (name + L"_width").c_str());
            int height = ini.GetInt(L"form_size", (name + L"_height").c_str());
            if (width > 0 && height > 0)
            {
                form->Size = Size(width, height);
            }
        }
    }
}