#include "pch.h"
#include "PingItem.h"
#include "DataManager.h"

const wchar_t* CPingItem::GetItemName() const
{
    return g_data.StringRes(IDS_PLUGIN_ITEM_PING_IP_CONF_NAME);
}

const wchar_t* CPingItem::GetItemId() const
{
	//TODO: 在此返回插件的唯一ID，建议只包含字母和数字
    return L"cGluZ2lwY29uZg";
}

const wchar_t* CPingItem::GetItemLableText() const
{
    return L"Ping";
}

const wchar_t* CPingItem::GetItemValueText() const
{
    return CDataManager::Instance().m_rtt.c_str();
}

const wchar_t* CPingItem::GetItemValueSampleText() const
{
    return L"99ms";
}

bool CPingItem::IsCustomDraw() const
{
	//TODO: 根据是否由插件自绘返回对应的值
    return false;
}