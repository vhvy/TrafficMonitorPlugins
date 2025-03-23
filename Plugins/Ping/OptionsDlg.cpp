// OptionsDlg.cpp: 实现文件
//

#include "pch.h"
#include "Ping.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"
#include "DataManager.h"

// COptionsDlg 对话框

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_OPTIONS_DIALOG, pParent)
{
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &COptionsDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_data.gap_space = g_data.m_setting_data.gap_space;
	m_data.target_ip = g_data.m_setting_data.target_ip;

    // TODO: 在此将 m_data 中的数据设置到对话框中
	SetDlgItemText(IDC_EDIT1, m_data.target_ip.c_str());
	CheckDlgButton(IDC_CHECK2, m_data.gap_space);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void COptionsDlg::OnBnClickedOk()
{
	CString str;
	GetDlgItemText(IDC_EDIT1, str);
	m_data.target_ip = str;
	m_data.gap_space = IsDlgButtonChecked(IDC_CHECK2);
    CDialog::OnOK();
}


void COptionsDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialog::OnCancel();
}
