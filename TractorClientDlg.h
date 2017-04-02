// TractorClientDlg.h : header file
//

#if !defined(AFX_TRACTORCLIENTDLG_H__5AE5DEF9_8E07_402E_8C34_6EC014199D19__INCLUDED_)
#define AFX_TRACTORCLIENTDLG_H__5AE5DEF9_8E07_402E_8C34_6EC014199D19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTractorClientDlg dialog
#include <vector>
using namespace std;
class CTractorClientDlg : public CDialog
{
// Construction
public:
	CTractorClientDlg(CWnd* pParent = NULL);	// standard constructor
	static void TCPThread(void *param);
	static void GetInfo();
	static void SetInfo();
	static void Login();
	static void Logout();
	static void Saveup();
	static void StartMeasuring(int vlume[]);
	static void Trans(int *data,char *send);
	static BOOL COMinit();
	static BOOL COMClose();
	static BOOL ReadCOM(char *str);
	static BOOL WriteCOM(char *str, DWORD dwBytesWrite);
	static int CalVolume(int *distance, float *CrossArea);
	static CString FindSpc(int SpcIndex1,int SpcIndex2,CString str);//取两个空格之间的内容，索引从0开始
// Dialog Data
	//{{AFX_DATA(CTractorClientDlg)
	enum { IDD = IDD_TRACTORCLIENT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTractorClientDlg)
	public:
	//static int vlume[10];
	static int lastvol;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTractorClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnGetInfo();
	afx_msg void OnBtnSysSet();
	afx_msg void OnBtnMeasure1();
	afx_msg void OnBtnMeasure2();
	afx_msg void OnBtnStopMeasure();
	afx_msg void OnBtnConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedTitle();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACTORCLIENTDLG_H__5AE5DEF9_8E07_402E_8C34_6EC014199D19__INCLUDED_)
