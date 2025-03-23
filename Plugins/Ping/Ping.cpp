#include "pch.h"
#include "Ping.h"
#include "DataManager.h"
#include "OptionsDlg.h"
#include <icmpapi.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <string>
#include <iphlpapi.h>
#include <stdexcept>
#include <locale>
#include <codecvt>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma warning(disable:4996)

CPing CPing::m_instance;

CPing::CPing()
{
}

CPing& CPing::Instance()
{
	return m_instance;
}

IPluginItem* CPing::GetItem(int index)
{
	switch (index)
	{
	case 0:
		return &m_item;
	default:
		break;
	}
	return nullptr;
}

const wchar_t* CPing::GetTooltipInfo()
{
	return m_tooltip_info.c_str();
}

static const std::string wstringToString(const std::wstring& wideStr) {
	std::string result;

	int len = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideStr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideStr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

const DWORD ping(const std::string& ipAddress) {
	// 初始化 Winsock（用于地址解析）
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return -1;
	}

	// 判断地址类型（IPv4 或 IPv6）
	bool isIPv6 = false;
	in_addr ipv4Addr = { 0 };
	in6_addr ipv6Addr = { 0 };

	// 尝试解析为 IPv4
	if (inet_pton(AF_INET, ipAddress.c_str(), &ipv4Addr) == 1) {
		isIPv6 = false;
	}
	// 尝试解析为 IPv6
	else if (inet_pton(AF_INET6, ipAddress.c_str(), &ipv6Addr) == 1) {
		isIPv6 = true;
	}
	// 无效地址
	else {
		WSACleanup();
		return -1;
	}

	// 创建 ICMP 句柄
	HANDLE hIcmp = isIPv6 ?
		Icmp6CreateFile() :
		IcmpCreateFile();
	if (hIcmp == INVALID_HANDLE_VALUE) {
		WSACleanup();
		return -1;
	}

	// 发送 Ping 请求
	constexpr DWORD payloadSize = 32;
	char sendData[payloadSize] = "PingTestData";
	DWORD replySize = isIPv6 ?
		sizeof(ICMPV6_ECHO_REPLY) + payloadSize :
		sizeof(ICMP_ECHO_REPLY) + payloadSize;
	std::vector<BYTE> replyBuffer(replySize);

	DWORD result = 0;

	if (isIPv6) {
		// IPv6 需要构造 sockaddr_in6
		sockaddr_in6 targetAddr = { 0 };
		targetAddr.sin6_family = AF_INET6;
		targetAddr.sin6_addr = ipv6Addr;
		targetAddr.sin6_scope_id = 0; // 根据实际网络环境设置（如链路本地地址）

		result = Icmp6SendEcho2(
			hIcmp,
			nullptr,    // 无异步事件
			nullptr,    // 无回调函数
			nullptr,    // 回调上下文
			&targetAddr, // 目标地址
			nullptr,    // 本地地址（可选）
			sendData,
			payloadSize,
			nullptr,    // 请求选项（如超时）
			replyBuffer.data(),
			replySize,
			2000        // 超时时间（毫秒）
		);
	}
	else {
		// IPv4 直接使用 IPAddr 类型
		IPAddr targetAddr = ipv4Addr.s_addr;

		result = IcmpSendEcho2(
			hIcmp,
			nullptr,    // 无异步事件
			nullptr,    // 无回调函数
			nullptr,    // 回调上下文
			targetAddr, // IPv4 地址
			sendData,
			payloadSize,
			nullptr,    // 请求选项（如超时）
			replyBuffer.data(),
			replySize,
			2000        // 超时时间（毫秒）
		);
	}

	// 获取 RTT
	int rtt = -1;
	if (result > 0) {
		if (isIPv6) {
			ICMPV6_ECHO_REPLY* pReply = (ICMPV6_ECHO_REPLY*)replyBuffer.data();
			rtt = pReply->RoundTripTime;
		}
		else {
			ICMP_ECHO_REPLY* pReply = (ICMP_ECHO_REPLY*)replyBuffer.data();
			rtt = pReply->RoundTripTime;
		}
	}

	// 清理资源
	IcmpCloseHandle(hIcmp);
	WSACleanup();

	return rtt;
}

void CPing::DataRequired()
{
	//TODO: 在此添加获取监控数据的代码

	std::wstring ip = CDataManager::Instance().m_setting_data.target_ip;
	bool gap_space = CDataManager::Instance().m_setting_data.gap_space;

	std::string target_ip = wstringToString(ip);

	int rtt = ping(target_ip);

	m_tooltip_info = ip;
	CDataManager::Instance().m_rtt = std::to_wstring(rtt) + (gap_space ? L" ms" : L"ms");
}

ITMPlugin::OptionReturn CPing::ShowOptionsDialog(void* hParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWnd* pParent = CWnd::FromHandle((HWND)hParent);
	COptionsDlg dlg(pParent);
	dlg.m_data = g_data.m_setting_data;
	if (dlg.DoModal() == IDOK)
	{
		g_data.m_setting_data = dlg.m_data;
		g_data.SaveConfig();
		return ITMPlugin::OR_OPTION_CHANGED;
	}
	return ITMPlugin::OR_OPTION_UNCHANGED;
}

const wchar_t* CPing::GetInfo(PluginInfoIndex index)
{
	static CString str;
	switch (index)
	{
	case TMI_NAME:
		return g_data.StringRes(IDS_PLUGIN_NAME).GetString();
	case TMI_DESCRIPTION:
		return g_data.StringRes(IDS_PLUGIN_DESCRIPTION).GetString();
	case TMI_AUTHOR:
		return L"vhvy";
	case TMI_COPYRIGHT:
		//TODO: 在此返回版权信息
		return L"Copyright (C) by vhvy 2025";
	case ITMPlugin::TMI_URL:
		//TODO: 在此返回URL
		return L"https://github.com/vhvy/TrafficMonitorPlugins";
		break;
	case TMI_VERSION:
		//TODO: 在此修改插件的版本
		return L"1.00";
	default:
		break;
	}
	return L"";
}

void CPing::OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data)
{
	switch (index)
	{
	case ITMPlugin::EI_CONFIG_DIR:
		//从配置文件读取配置
		g_data.LoadConfig(std::wstring(data));
		break;
	default:
		break;
	}
}

ITMPlugin* TMPluginGetInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return &CPing::Instance();
}
