#include "stdafx.h"
#include "TractorClient.h"
#include "TractorClientDlg.h"
#include "time.h"
#include "math.h"
#include "string.h"
#include "shlwapi.h"//为了使用StrToIntEx(),需要此头文件和lib
#include <sstream>

#pragma comment(lib,"shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SERVERIP "192.168.0.1" //定义宏，服务器IP
#define SERVERPORT 2111

#define pi 3.14159
#define CollectNum 181	//传感器采集的数量	 对应90°范围
#define D 2200

template<typename out_type, typename in_value>
out_type convert(const in_value & t) {
	stringstream stream;
	stream << std::hex << t;//向流中传值
	out_type result;//这里存储转换结果
	stream >> result;//向result中写入值
	return result;
}

bool IsMeasure2=false;//是否连续测量标识
clock_t start, finish;

HANDLE hCom;  //全局变量，串口句柄


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTractorClientDlg::CTractorClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTractorClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTractorClientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CTractorClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTractorClientDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTractorClientDlg, CDialog)
	//{{AFX_MSG_MAP(CTractorClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_GETINFO, OnBtnGetInfo)
	ON_BN_CLICKED(IDC_BTN_SYSSET, OnBtnSysSet)
	ON_BN_CLICKED(IDC_BTN_MEASURE1, OnBtnMeasure1)
	ON_BN_CLICKED(IDC_BTN_MEASURE2, OnBtnMeasure2)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStopMeasure)
	ON_BN_CLICKED(IDC_BTN_CONNECT, OnBtnConnect)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_TITLE, &CTractorClientDlg::OnStnClickedTitle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTractorClientDlg message handlers
CFont cfont;
BOOL CTractorClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	((CButton*)GetDlgItem(IDC_BTN_GETINFO))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BTN_SYSSET))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BTN_MEASURE1))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BTN_MEASURE2))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BTN_STOP))->EnableWindow(FALSE);

	cfont.CreatePointFont(250,"黑体");
	CStatic *static1;
	static1=(CStatic*)this->GetDlgItem(IDC_TITLE);
	static1->SetFont(&cfont,true);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTractorClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CTractorClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTractorClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

WORD wVersionRequested2;
WSADATA wsaData2;
int err2;
SOCKET sockClient2;
SOCKADDR_IN addrSrv2;
HANDLE hTCPTransThread; 
BOOL cnctstat=FALSE;


//连接设备
void CTractorClientDlg::OnBtnConnect() 
{
	wVersionRequested2 = MAKEWORD( 1, 1 );
	err2 = WSAStartup( wVersionRequested2, &wsaData2 );
	if ( err2 != 0 )
	{
		return ;
	}
	
	if ( LOBYTE( wsaData2.wVersion ) != 1 || HIBYTE( wsaData2.wVersion ) != 1 ) 
	{
		WSACleanup( );
		return ; 
	}
	
	sockClient2=socket(AF_INET,SOCK_STREAM,0);	
	addrSrv2.sin_addr.S_un.S_addr=inet_addr(SERVERIP);
	addrSrv2.sin_family=AF_INET;
	addrSrv2.sin_port=htons(SERVERPORT);
    int nNetTimeout=20;
	setsockopt(sockClient2,SOL_SOCKET,SO_RCVTIMEO,(char*)&nNetTimeout,sizeof(int));

	int err_cnect;
	err_cnect=connect(sockClient2,(SOCKADDR*)&addrSrv2,sizeof(SOCKADDR));
	if (err_cnect!=0)
	{
		AfxGetMainWnd()->MessageBox("连接设备失败！","冠层体积测量系统",MB_OK | MB_ICONERROR );
		GetDlgItem(IDC_WORKSTATUS)->SetWindowText("工作状态：未连接");
		((CButton*)GetDlgItem(IDC_BTN_GETINFO))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BTN_SYSSET))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BTN_MEASURE2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BTN_MEASURE1))->EnableWindow(FALSE);
				((CButton*)GetDlgItem(IDC_BTN_STOP))->EnableWindow(FALSE);

		closesocket(sockClient2);
		WSACleanup();	
		return ;		
	}
	
	if (!cnctstat)
	{
		cnctstat=TRUE;
		GetDlgItem(IDC_WORKSTATUS)->SetWindowText("工作状态：已连接");
		GetDlgItem(IDC_BTN_CONNECT)->SetWindowText("断开设备");
		((CButton*)GetDlgItem(IDC_BTN_GETINFO))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BTN_SYSSET))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BTN_MEASURE2))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BTN_MEASURE1))->EnableWindow(TRUE);
				((CButton*)GetDlgItem(IDC_BTN_STOP))->EnableWindow(TRUE);

	} 
	else
	{	
		closesocket(sockClient2);
		WSACleanup( );
		cnctstat=FALSE;
		GetDlgItem(IDC_WORKSTATUS)->SetWindowText("工作状态：未连接");
		GetDlgItem(IDC_BTN_CONNECT)->SetWindowText("连接设备");
		((CButton*)GetDlgItem(IDC_BTN_GETINFO))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BTN_SYSSET))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BTN_MEASURE2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BTN_MEASURE1))->EnableWindow(FALSE);
				((CButton*)GetDlgItem(IDC_BTN_STOP))->EnableWindow(FALSE);

	}

	
}


void CTractorClientDlg::OnBtnGetInfo() 
{
	GetInfo();
	if (TRUE == COMinit()) {
		char massage[30] = "hello world";
		DWORD dwBytesWrite = strlen(massage);
		WriteCOM(massage,dwBytesWrite);
		Sleep(20);
		ReadCOM(massage);
	}
	
}


void CTractorClientDlg::OnBtnSysSet() 
{
	
	Login();	
	Sleep(1000);	
	SetInfo();
	Sleep(1000);
	Saveup();
	Sleep(1000);
	Logout();

}

//计算体积
int CTractorClientDlg::CalVolume(int *distance,float *CrossArea)
{
	int i;
	float sita;
	//int D = 2200;//mm
	int d[CollectNum],h[CollectNum];	
	int S[CollectNum] = { 0 };
	d[0] = distance[1];
	h[0] = 0;
	for (i = 1; i < CollectNum; i++)
	{
		if (distance[i] > 3500)
			distance[i] = (D + 500);
		
			sita = (float)(i*pi/ 360);
			d[i] = distance[i] * cosf(sita);
			h[i] = distance[i]*sinf(sita);	// 单位mm
			if(distance[i]<D+500)
				CrossArea[i-1] = 0.5*(2 * D - d[i] - d[i - 1])*(fabsf(h[i] - h[i - 1]));//单位 mm^2
			else CrossArea[i - 1] = 0;

		if (CrossArea[i - 1] < 0)CrossArea[i-1] = 0;
	}

	return 0;
}

BOOL IsFrmOne=false;//标记多线程进入

char CmdMeasure1[17] = { (char)0x02,(char)0x73,(char)0x52,(char)0x4E,\
		(char)0x20,(char)0x4C,(char)0x4D,(char)0x44,(char)0x73,(char)0x63,\
		(char)0x61,(char)0x6E,(char)0x64,(char)0x61,(char)0x74,(char)0x61,\
		(char)0x03 };//Measure once 
//测量函数	
void CTractorClientDlg::StartMeasuring(int vlume[])
{
	start = clock();

	send(sockClient2, CmdMeasure1, 17, 0);
	char recvBuf[2000] = { '\0' };
	int Msglen;
	Msglen = recv(sockClient2, recvBuf, 2000, 0);

	recvBuf[0] = 0x24;//起始符换为$
	recvBuf[Msglen - 1] = 0x23;//终止符换为#

	CString strMsg;
	strMsg.Format("%s", recvBuf);

	int k1, k2;

	k1 = strMsg.Find("1388");
	strMsg = strMsg.Right(strMsg.GetLength() - k1 - 4);

	k1 = strMsg.Find("1388");
	strMsg = strMsg.Right(strMsg.GetLength() - k1 - 5);

	k2 = strMsg.Find("#");
	strMsg = strMsg.Left(k2);//提取有效信息

	int data[CollectNum ] = { 0 };/*这是最后得到的距离数据 单位是mm */
	int vol[10] = { 0 };
	int Ssum[10] = { 0 };
	float CrossArea[CollectNum - 1] = { 0 };
	CString tmpdata2, strData = "";
	
	stringstream ss;
	string s;
	ss << strMsg;
	int i = 0;
	ss >> s;	//第一个是数据个数 不需要
	for(i=0;i<CollectNum;i++)
	{
		ss >> s;
		data[i] = convert<int>(s);
		if (data[i] > D || data[i]<100 ) data[i] = D;
	}
	CalVolume(data, CrossArea);		//计算每个微段截面积

	for (int i = 0; i < 120 ; i++)		//取120个数据  即0-60°
	{
		Ssum[i / 24] += 0.5*CrossArea[i];	//每部分截面积之和 mm^2
		if (Ssum[i / 24] < 0)Ssum[i/24] = 0;
	}

	for (int i = 0; i < 10; i++)
	{
		vol[i] = (float)(0.00232*Ssum[i]);//将截面积等效成体积 单位cm^3
		//*0.155->cm^3/s -> /10^6  m^3/s =
		//0.025L/s 每个喷头出水量/1.55*10^-7 *Percent   =     需要0.12L/m^3
		//Percent=0.12/0.025*1.55*10^-7=7.44*10^-7*300=2.232*10^-4
		if ( vol[i]<100)vol[i] = 0;
		else if (vol[i] > 900)vol[i] = 900;
		
		vlume[i] = vol[i]*0.1;
		tmpdata2.Format("%d ", vlume[i]);
		strData = strData + tmpdata2;
	}

	

	if (IsFrmOne == false) {
		finish = clock();
		CListBox *box = (CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
		box->InsertString(box->GetCount(), strData);
		box->SetCurSel(box->GetCount() - 1);
		strMsg.Empty();

		CString RunTime;
		int rt;
		rt = (finish - start);
		RunTime.Format("运行时间：%d ms", rt);
		AfxGetMainWnd()->GetDlgItem(IDC_MEA_TYPE)->SetWindowText(RunTime);
	}
}

 //单次测量 
void CTractorClientDlg::OnBtnMeasure1() 
{
	DWORD dwBytesWrite = 40;
	IsFrmOne = false;
	int vlume[10] = { 0 };
	StartMeasuring(vlume);
	char src[40] = "";
	Trans((int *)vlume, src);
	WriteCOM(src, dwBytesWrite);
	Sleep(20);
	
}



//开启新线程
void CTractorClientDlg::OnBtnMeasure2() 
{

	DWORD TCPThreadID;
	IsMeasure2 = true;
	hTCPTransThread= CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同 
		(LPTHREAD_START_ROUTINE)TCPThread, //线程的全局函数 
		AfxGetMainWnd()->m_hWnd, //此处传入了主框架的句柄 
		0, 
		&TCPThreadID);		
	ASSERT(hTCPTransThread!=NULL); 
	CloseHandle(hTCPTransThread);
	
}

//停止测量
void CTractorClientDlg::OnBtnStopMeasure() 
{
	char massage[] = "00000000000000000000000000000";
	DWORD dwBytesWrite = strlen(massage);
	WriteCOM(massage, dwBytesWrite);
	Sleep(20);
	COMClose();//关闭串口
	IsMeasure2=false;
	char CmdMeasure1[19]={(char)0x02,(char)0x73,(char)0x45,(char)0x4E,(char)0x20,\
		(char)0x4C,(char)0x4D,(char)0x44,(char)0x73,(char)0x63,(char)0x61,(char)0x6E,\
		(char)0x64,(char)0x61,(char)0x74,(char)0x61,(char)0x20,(char)0x30,(char)0x03};//Measure continues 
	send(sockClient2,CmdMeasure1,19,0);
	char recvBuf[100]={'\0'};
	int Msglen;
	Msglen=recv(sockClient2,recvBuf,100,0);


	CString strMsg;
	strMsg.Format("%s",recvBuf);
	strMsg=strMsg.Left(Msglen-1);
	strMsg=strMsg+" ";
	

	/*CString flname;
	flname="Data.txt";
	FILE *pFile=fopen(flname,"a");
	char lineend=10;
	fwrite(strMsg,1,strlen(strMsg),pFile);
	fwrite(&lineend,1,1,pFile);
	fclose(pFile);	*/
	
	CListBox *box=(CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
	box->InsertString(box->GetCount(),strMsg);
	strMsg.Empty();
	box->SetCurSel(box->GetCount()-1);
	AfxGetMainWnd()->GetDlgItem(IDC_MEA_TYPE)->SetWindowText("测量类型：停止测量");
}

//连续测量
char src2[4][40];
void CTractorClientDlg::TCPThread(void *param)
{
	CTractorClientDlg *dlg=(CTractorClientDlg*)param;
//	AfxGetMainWnd()->GetDlgItem(IDC_MEA_TYPE)->SetWindowText("测量类型：连续测量");	
	DWORD dwBytesWrite = 40;
	int vol[10] = { 0 };
	unsigned int volSum[10] = { 0 };
	int twice = 0;
	while (IsMeasure2)
	{
		twice++;
		IsFrmOne = true;
		StartMeasuring(vol);
		for (int i = 0; i < 10; i++)
		{
			if (vol[i] > 0)vol[i] += 50;
			volSum[i] += vol[i];
		}
			
		Sleep(10);
		if (twice >= 10)//10次平均滤波
		{//大约160ms发送一次数据
			for (int i = 0; i<10; i++)
				volSum[i] *= 0.1;
			//转换成字符串准备发送
			char src[40] = "";
			Trans((int *)volSum, src);
			for (int j = 3; j >0; j--)
			{
				strcpy(src2[j], src2[j-1]);
			}
			strcpy(src2[0] ,src);
			
 			WriteCOM(src2[3], dwBytesWrite);
			twice = 0;
		}

	}
	
}


CString CTractorClientDlg::FindSpc(int SpcIndex1, int SpcIndex2, CString str)
{
	CString strtmp=str;
		vector <int> SpcIndex;
		BYTE tmpindex=0;
		for (int i=0;i<SpcIndex2+1;i++)
		{
			tmpindex=str.Find(" ");
			SpcIndex.push_back(tmpindex);
			str=str.Mid(tmpindex+1);//从tmpindex+1索引值处开始取字符，直到取完
		}
		int index1=0,index2=0;
		for (int j=0;j<SpcIndex1+1;j++)
		{
			index1+=SpcIndex.at(j);
		}
		for (int k=0;k<SpcIndex2+1;k++)
		{
			index2+=SpcIndex.at(k);
		}
		int a,b;
		a=index1+SpcIndex1;
		b=index2+SpcIndex2;
		str=strtmp.Mid(a+1,b-a-1);//从索引值为a+1开始取数据，取得数据个数是b-a-1个

	return str;
}

void CTractorClientDlg::GetInfo()
{
	
	
	AfxGetMainWnd()->GetDlgItem(IDC_IPADDR)->SetWindowText("IP地址：192.168.0.1");
	AfxGetMainWnd()->GetDlgItem(IDC_PORT)->SetWindowText("端口号：2111");
	AfxGetMainWnd()->GetDlgItem(IDC_DATASTREAM)->SetWindowText("连接类型：TCP连接");
	
	
	char CmdFreqReso[16]={(char)0x02,(char)0x73,(char)0x52,(char)0x4E,(char)0x20, \
		(char)0x4C, (char)0x4D,(char)0x50,(char)0x73,(char)0x63,(char)0x61, \
		(char)0x6E,(char)0x63,(char)0x66,(char)0x67,(char)0x03};//Get frequency and angular resolution 
	send(sockClient2,CmdFreqReso,16,0);
	char recvBuf[100]={'\0'};
	int Msglen;
	Msglen=recv(sockClient2,recvBuf,100,0);
	CString strMsg;
	strMsg.Format("%s",recvBuf);
	strMsg=strMsg.Left(Msglen-1);
	strMsg=strMsg+" ";
	
	CString strFreq,strReso,strStarAng,strStopAng,tmpstr;
	strFreq="0x"+FindSpc(1,2,strMsg);
	int freq;
	StrToIntEx(strFreq,STIF_SUPPORT_HEX,&freq);
	tmpstr.Format("扫描频率：%dHz",freq/100);
	AfxGetMainWnd()->GetDlgItem(IDC_FREQ)->SetWindowText(tmpstr);
	
	strReso="0x"+FindSpc(3,4,strMsg);
	int reso;
	StrToIntEx(strReso,STIF_SUPPORT_HEX,&reso);
	tmpstr.Format("角分辨率：%0.2f°",(float)reso/10000);
	AfxGetMainWnd()->GetDlgItem(IDC_RESO)->SetWindowText(tmpstr);
	
	strStarAng="0x"+FindSpc(4,5,strMsg);
	int startang;
	StrToIntEx(strStarAng,STIF_SUPPORT_HEX,&startang);
	tmpstr.Format("起始角：%0.2f°",(float)startang/10000);
	AfxGetMainWnd()->GetDlgItem(IDC_STARTANG)->SetWindowText(tmpstr);
	
	strStopAng="0x"+FindSpc(5,6,strMsg);
	int stopang;
	StrToIntEx(strStopAng,STIF_SUPPORT_HEX,&stopang);
	tmpstr.Format("终止角：%0.2f°",(float)stopang/10000);
	AfxGetMainWnd()->GetDlgItem(IDC_STOPANG)->SetWindowText(tmpstr);

	
	
	/*文件保存*/
	CString flname;
	flname="Data.txt";
	FILE *pFile=fopen(flname,"a");
	char lineend=10;
	fwrite(strMsg,1,strlen(strMsg),pFile);
	fwrite(&lineend,1,1,pFile);
	fclose(pFile);
	
		CListBox *box=(CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
		box->InsertString(box->GetCount(),strMsg);
		strMsg.Empty();
		box->SetCurSel(box->GetCount()-1);
	
}



void CTractorClientDlg::SetInfo()
{
	char CmdSetAngle180[36]={(char)0x02,(char)0x73,(char)0x57,(char)0x4E,(char)0x20,\
		(char)0x4C,(char)0x4D,(char)0x50,(char)0x6F,(char)0x75,(char)0x74,(char)0x70,\
		(char)0x75,(char)0x74,(char)0x52,(char)0x61,(char)0x6E,(char)0x67,(char)0x65,\
		(char)0x20,(char)0x31,(char)0x20,(char)0x31,(char)0x33,(char)0x38,(char)0x38,\
		(char)0x20,(char)0x30,(char)0x20,(char)0x31,(char)0x42,(char)0x37,(char)0x37,\
		(char)0x34,(char)0x30,(char)0x03};  //Set Angle From 0(30) to 180(31 42 37 37 34 30)

	char CmdSetAngle90[35] = { (char)0x02,(char)0x73,(char)0x57,(char)0x4E,(char)0x20,\
		(char)0x4C,(char)0x4D,(char)0x50,(char)0x6F,(char)0x75,(char)0x74,(char)0x70,\
		(char)0x75,(char)0x74,(char)0x52,(char)0x61,(char)0x6E,(char)0x67,(char)0x65,\
		(char)0x20,(char)0x31,(char)0x20,(char)0x31,(char)0x33,(char)0x38,(char)0x38,\
		(char)0x20,(char)0x30,(char)0x20,(char)0x44,(char)0x42,(char)0x42,(char)0x41,\
		(char)0x30,(char)0x03 };	//Set Angle From 0(30) to 90(44 42 42 41 30)

	send(sockClient2,CmdSetAngle90,35,0);
	char recvBuf[100]={'\0'};
	int Msglen;
	Msglen=recv(sockClient2,recvBuf,100,0);
	CString strMsg;
	strMsg.Format("%s",recvBuf);
	strMsg=strMsg.Left(Msglen-1);
	strMsg=strMsg+" ";

	CListBox *box=(CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
	box->InsertString(box->GetCount(),strMsg);
	strMsg.Empty();
	box->SetCurSel(box->GetCount()-1);

	AfxGetMainWnd()->GetDlgItem(IDC_STARTANG)->SetWindowText("起始角：0°");
	AfxGetMainWnd()->GetDlgItem(IDC_STOPANG)->SetWindowText("终止角：90°");

}


void CTractorClientDlg::Login()
{
	char CmdLogin[31]={(char)0x02,(char)0x73,(char)0x4D,(char)0x4E,\
		(char)0x20,(char)0x53,(char)0x65,(char)0x74,(char)0x41,(char)0x63,\
		(char)0x63,(char)0x65,(char)0x73,(char)0x73,(char)0x4D,(char)0x6F,\
		(char)0x64,(char)0x65,(char)0x20,(char)0x30,(char)0x33,(char)0x20,\
		(char)0x46,(char)0x34,(char)0x37,(char)0x32,(char)0x34,(char)0x37,\
		(char)0x34,(char)0x34,(char)0x03};//login system with Authorised client 

	send(sockClient2,CmdLogin,31,0);
	char recvBuf[100]={'\0'};
	int Msglen;
	Msglen=recv(sockClient2,recvBuf,100,0);
	CString strMsg;
	strMsg.Format("%s",recvBuf);
	strMsg=strMsg.Left(Msglen-1);
	strMsg=strMsg+" ";

	CListBox *box=(CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
	box->InsertString(box->GetCount(),strMsg);
	strMsg.Empty();
	box->SetCurSel(box->GetCount()-1);
}

void CTractorClientDlg::Logout()
{
	char CmdLogout[9]={(char)0x02,(char)0x73,(char)0x4D,(char)0x4E,(char)0x20,(char)0x52,(char)0x75,(char)0x6E,(char)0x03};//logout 
	send(sockClient2,CmdLogout,9,0);
	char recvBuf[100]={'\0'};
	int Msglen=0;
	
	Msglen=recv(sockClient2,recvBuf,100,0);

	CString strMsg;
	strMsg.Format("%s",recvBuf);
	strMsg=strMsg.Left(Msglen-1);
	strMsg=strMsg+" ";

	CListBox *box=(CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
	box->InsertString(box->GetCount(),strMsg);
	strMsg.Empty();
	box->SetCurSel(box->GetCount()-1);
}

//保存设置
void CTractorClientDlg::Saveup()
{
	char CmdSave[17]={(char)0x02,(char)0x73,(char)0x4D,(char)0x4E,(char)0x20,\
		(char)0x6D,(char)0x45,(char)0x45,(char)0x77,(char)0x72,(char)0x69,\
		(char)0x74,(char)0x65,(char)0x61,(char)0x6C,(char)0x6C,(char)0x03};//SaveUP 
	send(sockClient2,CmdSave,17,0);
	char recvBuf[100]={'\0'};
	int Msglen;

	BOOL IsBack=FALSE;
	while (IsBack==FALSE)
	{
		Msglen=recv(sockClient2,recvBuf,100,0);
		if (Msglen>1)
		{
			IsBack=TRUE;
		}
	}

	CString strMsg;
	strMsg.Format("%s",recvBuf);
	strMsg=strMsg.Left(Msglen-1);
	strMsg=strMsg+" ";

	CListBox *box=(CListBox *)AfxGetMainWnd()->GetDlgItem(IDC_LIST_RECV);
	box->InsertString(box->GetCount(),strMsg);
	strMsg.Empty();
	box->SetCurSel(box->GetCount()-1);
}


void CTractorClientDlg::OnStnClickedTitle()
{
	// TODO: 在此添加控件通知处理程序代码
}

//串口的打开和配置
BOOL CTractorClientDlg::COMinit()
{
	
	hCom = CreateFile("COM3",//COM3口
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	if (hCom == (HANDLE)-1)
	{
		AfxMessageBox("打开COM失败!");
		return FALSE;
	}
	SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024

	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = 1000;
	TimeOuts.ReadTotalTimeoutMultiplier = 500;
	TimeOuts.ReadTotalTimeoutConstant = 5000;
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom, &TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600; //波特率为9600
	dcb.ByteSize = 8; //每个字节有8位
	dcb.Parity = NOPARITY; //无奇偶校验位
	dcb.StopBits = TWOSTOPBITS; //两个停止位
	SetCommState(hCom, &dcb);

	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return TRUE;
}

//关闭串口
BOOL CTractorClientDlg::COMClose()
{
	return CloseHandle(hCom);
}

BOOL CTractorClientDlg::ReadCOM(char *str)
{
	//同步读串口
	//char str[100];
	DWORD wCount;//读取的字节数
	BOOL bReadStat;
	bReadStat = ReadFile(hCom, str, 60, &wCount, NULL);
	if (!bReadStat)
	{
		AfxMessageBox("读串口失败!");
		return FALSE;
	}
	else
	{
		AfxMessageBox(str);
	}
	return TRUE;
}
BOOL CTractorClientDlg::WriteCOM(char *str,DWORD dwBytesWrite)
{
	//char lpOutBuffer[100];
	//发送字节数
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	bWriteStat = WriteFile(hCom, str, dwBytesWrite, &dwBytesWrite, NULL);
	if (!bWriteStat)
	{
		AfxMessageBox("写串口失败!");

	}
	PurgeComm(hCom, PURGE_TXABORT |
		PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return TRUE;
}

void CTractorClientDlg::Trans(int *data, char *send)
{
	char dest[] = "000";
	int i;
	for (i = 0; i<10; i++)
	{
		if (data[i] > 999)data[i] = 999;
		else if (data[i] < 0)data[i] = 0;
		sprintf(dest, "%3d", data[i]);
		strcat(send, dest);
	}
}