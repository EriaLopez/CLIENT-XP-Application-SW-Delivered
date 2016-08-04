// BasePCGUIDlg.cpp : implementation file
//
#include "stdafx.h"
#include "BasePCGUI.h"
#include "BasePCGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Declarations for multi-class
#include "ThreadClass.h"
#include "GlobalsClass.h"
#include "ScannerClass.h"
#include "ScannerFunctions.h"

GlobalsClass g_theGlobals;

// multiclass declarations end

#define thisScannerPtr ((ScannerClass *)(g_theGlobals.ScannerPtr[atoi(m_currentScannerNumber)]))


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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

/////////////////////////////////////////////////////////////////////////////
// CBasePCGUIDlg dialog

CBasePCGUIDlg::CBasePCGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBasePCGUIDlg::IDD, pParent)
	, m_azimuthSpeed(_T("1000"))
	, m_azimuthPosition(_T("0"))
	, m_elevationSpeed(_T("1600000"))
	, m_azimuthStart(_T("-8000"))
	, m_elevationStart(_T("1500"))
	, m_pointsPerLine(_T("500"))
	, m_elevationResolution(_T("6"))
	, m_azimuthEnd(_T("8000"))
	, m_statusDisplay(_T("Click button to get status"))
    , m_ackStatusDisplay(_T(" "))
    , m_Dmin(_T("54"))
	, m_Dmax(_T("500"))
    , m_Xmin(_T("-500"))
	, m_Xmax(_T("500"))
    , m_Ymin(_T("-500"))
	, m_Ymax(_T("500"))
    , m_Zmin(_T("-100"))
	, m_Zmax(_T("100"))
{
	//{{AFX_DATA_INIT(CBasePCGUIDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBasePCGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasePCGUIDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EDITAzSpeed, m_azimuthSpeed);
	DDV_MaxChars(pDX, m_azimuthSpeed, 7);
	DDX_Text(pDX, IDC_EDITAzPos, m_azimuthPosition);
	DDX_Text(pDX, IDC_EDITElSpeed, m_elevationSpeed);
	DDX_Text(pDX, IDC_EDITAzStart, m_azimuthStart);
	DDX_Text(pDX, IDC_EDITElStart, m_elevationStart);
	DDX_Text(pDX, IDC_EDITpointsPerLine, m_pointsPerLine);
	DDX_Text(pDX, IDC_EDITelevationResolution, m_elevationResolution);
	DDX_Text(pDX, IDC_EDITazimuthEnd, m_azimuthEnd);
	DDV_MaxChars(pDX, m_azimuthEnd, 8);
	DDX_Text(pDX, IDC_EDITstatusDisplay, m_statusDisplay);
	DDV_MaxChars(pDX, m_azimuthStart, 8);
	DDX_Text(pDX, IDC_EDITCommandAck, m_ackStatusDisplay);
	DDX_Text(pDX, IDC_Dmin, m_Dmin);
	DDX_Text(pDX, IDC_Dmax, m_Dmax);
	DDX_Text(pDX, IDC_Xmin, m_Xmin);
	DDX_Text(pDX, IDC_Xmax, m_Xmax);
	DDX_Text(pDX, IDC_Ymin, m_Ymin);
	DDX_Text(pDX, IDC_Ymax, m_Ymax);
	DDX_Text(pDX, IDC_Zmin, m_Zmin);
	DDX_Text(pDX, IDC_Zmax, m_Zmax);
	DDX_Control(pDX, IDC_CHECK1, b_useDist);
	DDX_Control(pDX, IDC_CHECK2, b_useX);
	DDX_Control(pDX, IDC_CHECK5, b_useY);
	DDX_Control(pDX, IDC_CHECK6, b_useZ);
}

BEGIN_MESSAGE_MAP(CBasePCGUIDlg, CDialog)
	//{{AFX_MSG_MAP(CBasePCGUIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP


	ON_BN_CLICKED(IDC_BUTTONsingleScan, &CBasePCGUIDlg::OnBnClickedButtonsinglescan)
	ON_BN_CLICKED(IDC_BUTTONhaltScan, &CBasePCGUIDlg::OnBnClickedButtonhaltscan)
	ON_BN_CLICKED(IDC_BUTTONabortscan, &CBasePCGUIDlg::OnBnClickedButtonabortscan)
	ON_BN_CLICKED(IDC_BUTTONazimuthPosition, &CBasePCGUIDlg::OnBnClickedButtonazimuthposition)
	ON_BN_CLICKED(IDC_BUTTONelevationSpeed, &CBasePCGUIDlg::OnBnClickedButtonelevationspeed)
	ON_BN_CLICKED(IDC_BUTTONazimuthSpeed, &CBasePCGUIDlg::OnBnClickedButtonazimuthspeed)
	ON_BN_CLICKED(IDC_BUTTONstatusDisplay, &CBasePCGUIDlg::OnBnClickedButtonstatusdisplay)
	ON_BN_CLICKED(IDC_BUTTONAzimuthParameters, &CBasePCGUIDlg::OnBnClickedButtonazimuthparameters)
	ON_BN_CLICKED(IDC_BUTTONElevationParameters, &CBasePCGUIDlg::OnBnClickedButtonelevationparameters)
	ON_BN_CLICKED(IDC_BUTTONstatusDisplay2, &CBasePCGUIDlg::OnBnClickedButtonstatusdisplay2)
	ON_BN_CLICKED(IDC_BUTTONstatusDisplay3, &CBasePCGUIDlg::OnBnClickedSleep)
	ON_BN_CLICKED(IDC_BUTTONstatusDisplay4, &CBasePCGUIDlg::OnBnClickedConvert)
	ON_BN_CLICKED(IDC_BUTTONstatusDisplay5, &CBasePCGUIDlg::OnBnClickedButtonDisplay)
	ON_EN_CHANGE(IDC_Dmin, &CBasePCGUIDlg::OnEnChangeDmin)
	ON_EN_CHANGE(IDC_Dmax, &CBasePCGUIDlg::OnEnChangeDmax)
	ON_EN_CHANGE(IDC_Xmin, &CBasePCGUIDlg::OnEnChangeXmin)
	ON_EN_CHANGE(IDC_Xmax, &CBasePCGUIDlg::OnEnChangeXmax)
	ON_EN_CHANGE(IDC_Ymin, &CBasePCGUIDlg::OnEnChangeYmin)
	ON_EN_CHANGE(IDC_Ymax, &CBasePCGUIDlg::OnEnChangeYmax)
	ON_EN_CHANGE(IDC_Zmin, &CBasePCGUIDlg::OnEnChangeZmin)
	ON_EN_CHANGE(IDC_Zmax, &CBasePCGUIDlg::OnEnChangeZmax)
	ON_BN_CLICKED(IDC_CHECK1, &CBasePCGUIDlg::OnBnClickedUseDist)
	ON_BN_CLICKED(IDC_CHECK2, &CBasePCGUIDlg::OnBnClickedUseX)
	ON_BN_CLICKED(IDC_CHECK5, &CBasePCGUIDlg::OnBnClickedUseY)
	ON_BN_CLICKED(IDC_CHECK6, &CBasePCGUIDlg::OnBnClickedUseZ)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasePCGUIDlg message handlers

BOOL CBasePCGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int Tport = atoi(TCP_PORT); 
	int TDport = atoi(TCPDATA_PORT);

	char *ID = SCANNER_ID; //Name or IP of the scanner
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

//g_theGlobals.ScannerPtr[atoi(m_currentScannerNumber)] = ScannerClass::openScanner(atoi(m_currentScannerNumber), Tport, TDport, ID);
	//only implemented for 1 scanner at present
	g_theGlobals.ScannerPtr[0] =ScannerClass::openScanner(0, Tport, TDport, ID);
	
	// for OpenGL
	CRect rect;
	GetDlgItem(IDC_OPENGL)->GetWindowRect(rect); // Get size and pos of the template field in the dialog editor
	ScreenToClient(rect); 	// Convert screen coordinates to client coordinates
	m_oglWindow.oglCreate(rect, this); // Create OpenGL Control window
	m_oglWindow.m_unpTimer = m_oglWindow.SetTimer(1, 100, 0); // Setup the OpenGL Window's timer to render
	// openGL ends

	UpdateData(false);

	Sleep(1000); //Necessary to let other classes and threads spawn

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBasePCGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBasePCGUIDlg::OnPaint() 
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
HCURSOR CBasePCGUIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CBasePCGUIDlg::OnBnClickedButtonsinglescan()
{
	UpdateData();
	int result = startScan(thisScannerPtr);
}


void CBasePCGUIDlg::OnBnClickedButtonhaltscan()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int result = haltScan(thisScannerPtr);
}


void CBasePCGUIDlg::OnBnClickedButtonabortscan()
{
		UpdateData();
	int result = abortScan(thisScannerPtr);
}

void CBasePCGUIDlg::OnBnClickedButtonazimuthposition()
{
	// TODO: Add your control notification handler code here
	UpdateData();	
	int result = azimuthPosition(thisScannerPtr,atoi(m_azimuthPosition));
}


void CBasePCGUIDlg::OnBnClickedButtonelevationspeed()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int result = elevationSpeed(thisScannerPtr, atoi(m_elevationSpeed));
}

void CBasePCGUIDlg::OnBnClickedButtonazimuthspeed()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int result = azimuthSpeed(thisScannerPtr, atoi(m_azimuthSpeed));
}

int getStatus(ScannerClass *scanner);

void CBasePCGUIDlg::OnBnClickedButtonstatusdisplay()  //current state of Scanner. Command status returned is displayed elsewhere
{
	// TODO: Add your control notification handler code here
	ScannerClass *scanner = thisScannerPtr;

	//sprintf(tmp,"%d",g_theGlobals.ScannerPtr->s_SRIdataPacket.packetNumber);

	int stat;
	char tmp[40];
	// send command to get status
	UpdateData();
	int result = getStatus(scanner);
	stat = scanner->s_status;

	switch(stat)
	{
	case OK_SLEEPING:
		this->m_statusDisplay="Sleeping";
		break;
	case OK_WAKING:
		this->m_statusDisplay="Waking";
		break;
	case OK_READY:
		this->m_statusDisplay="Ready";
		break;
	case OK_STARTING_SCAN:
		this->m_statusDisplay="Starting Scan";
		break;
	case OK_WAITING_ELEV_MOTOR_SPEED:
		this->m_statusDisplay="Waiting for Elevation Motor Speed";
		break;
	case OK_WAITING_AZ_MOTOR:
		this->m_statusDisplay="Waiting for Turret to Enter Scan";	
		break;
	case OK_IN_FIRST_SCAN:
		this->m_statusDisplay="In Scan, 1 More Scan Pending";
		break;
	case OK_IN_LAST_SCAN:
		this->m_statusDisplay="In Last Scan";
		break;
	case OK_SCAN_FINISHED:
		this->m_statusDisplay="Finishing Last Scan";
		break;
	case NO_RESPONSE:
		this->m_statusDisplay="No Reply from Scanner";
		break;
	case ACK_WAIT_FAILED:
		this->m_statusDisplay="Client Wait Failure";
		break;
	default:
		sprintf(tmp,"Error Number %d",stat);
		this->m_statusDisplay=tmp;
	}
	UpdateData(false);
}


void CBasePCGUIDlg::upDateAckStatus(short stat)  //Command status returned is displayed
{
	char buf[50]; 
	ScannerClass *scanner = thisScannerPtr;
	UpdateData();

	switch(stat)
	{
	case CMD_RCVD_OK:
		this->m_ackStatusDisplay="COMMAND_RECEIVED_AND_ACK'D";
		break;
	case INVALID_START_POINT:
		this->m_ackStatusDisplay="INVALID_START_POINT";
		break;
	case INVALID_FINAL_POINT:
		this->m_ackStatusDisplay="INVALID_FINAL_POINT";
		break;
	case INVALID_POINT_SPACING:
		this->m_ackStatusDisplay="INVALID_POINT_SPACING";
		break;
	case INVALID_CMD:
		this->m_ackStatusDisplay="INVALID_CMD";
		break;
	case INVALID_ELEV_START:
		this->m_ackStatusDisplay="INVALID_ELEV_START";
		break;
	case INVALID_ELEV_SPEED:
		this->m_ackStatusDisplay="INVALID_ELEV_SPEED";
		break;
	case INVALID_SAMPLE_RATE:
		this->m_ackStatusDisplay="INVALID_SAMPLE_RATE";
		break;
	case ELEV_SPEED_SET_FAILURE:
		this->m_ackStatusDisplay="ELEV_SPEED_SET_FAILURE";
		break;		
	case INVALID_AZIMUTH_START:
		this->m_ackStatusDisplay="INVALID_AZIMUTH_START";
		break;		
	case INVALID_AZIMUTH_END:
		this->m_ackStatusDisplay="INVALID_AZIMUTH_END";
		break;		
	case INVALID_AZIMUTH_PARAMS:
		this->m_ackStatusDisplay="INVALID_AZIMUTH_PARAMS";
		break;
	case INVALID_AZIMUTH_SPEED:
		this->m_ackStatusDisplay="INVALID_AZIMUTH_SPEED";
		break;		
	case AZIMUTH_SPEED_SET_FAILURE:
		this->m_ackStatusDisplay="AZIMUTH_SPEED_SET_FAILURE";
		break;
	case AZIMUTH_POSITION_SET_FAILURE:
		this->m_ackStatusDisplay="AZIMUTH_POSITION_SET_FAILURE";
		break;
	case INVALID_POINTS_PER_LINE:
		this->m_ackStatusDisplay="INVALID_POINTS_PER_LINE";
		break;
	case INVALID_ELEVATION_START:
		this->m_ackStatusDisplay="INVALID_ELEVATION_START";
		break;
	case SCANNER_NOT_READY_FOR_COMMAND:
		this->m_ackStatusDisplay="SCANNER_NOT_READY_FOR_COMMAND";
		break;
	case COMMAND_ACK_TIMEOUT:
		this->m_ackStatusDisplay="COMMAND_ACK_TIMEOUT";
		break;
	case CLIENT_WAIT_FAILED:
		this->m_ackStatusDisplay="CLIENT_WAIT_FAILED";
		break;	
	case CLIENT_SEND_FAILED:
		this->m_ackStatusDisplay="CLIENT_SEND_FAILED";
		break;	
	case BLANK_LINE:
		this->m_ackStatusDisplay=" ";
		break;
	case MISMATCHED_COMMAND:
		this->m_ackStatusDisplay="COMMAND_ID_MISMATCH_IN_ACK";
		break;
	case NO_LINE_PARAMS_RECD:
		this->m_ackStatusDisplay="NO_LINE_PARAMETERS_SET";
		break;
	case NO_AZIMUTH_PARAMS_RECD:
		this->m_ackStatusDisplay="NO_AZIMUTH_PARAMETERS_SET";
		break;
	default:
		sprintf(buf, "SCANNER ERROR #%d OCCURRED", stat); 
		this->m_ackStatusDisplay=buf;
	}
	UpdateData(false);
}

void CBasePCGUIDlg::OnBnClickedButtonazimuthparameters()
{
	UpdateData();
	int result = azimuthParameters(thisScannerPtr,atoi(m_azimuthSpeed),atoi(m_azimuthStart),atoi(m_azimuthEnd));

}

void CBasePCGUIDlg::OnBnClickedButtonelevationparameters()
{
	UpdateData();
	int result = elevationParameters(thisScannerPtr,atoi(m_elevationSpeed),atoi(m_elevationStart), atoi(m_pointsPerLine),atoi(m_elevationResolution));
}

void CBasePCGUIDlg::OnBnClickedButtonstatusdisplay2()
{
	// gets and prints the current scanner state and recent activity
		int result = getLog(thisScannerPtr);

}

void CBasePCGUIDlg::OnBnClickedSleep()
{
	// send sleep command
	int result = scannerSleep(thisScannerPtr);

}

void CBasePCGUIDlg::OnBnClickedConvert()
{
	// Convert Last binary file to ASCII, calibrated
	int result = calibrateScan(thisScannerPtr);
	printf("Conversion complete.\n");
}

void CBasePCGUIDlg::OnBnClickedButtonDisplay()
{
	OnBnClickedUseDist();
	OnBnClickedUseX();
	OnBnClickedUseY();
	OnBnClickedUseZ();
	OnEnChangeDmin();
	OnEnChangeDmax();
	OnEnChangeXmin();
	OnEnChangeXmax();
	OnEnChangeYmin();
	OnEnChangeYmax();
	OnEnChangeZmin();
	OnEnChangeZmax();
	ScannerClass *scanner = thisScannerPtr;
	scanner->ScanToXYZ();
	m_oglWindow.oglDrawScene();
}

void CBasePCGUIDlg::OnBnClickedUseDist()
{
	UpdateData();
	thisScannerPtr->useDist = b_useDist.GetCheck();
}

void CBasePCGUIDlg::OnBnClickedUseX()
{
	UpdateData();
	thisScannerPtr->useX = b_useX.GetCheck();
}

void CBasePCGUIDlg::OnBnClickedUseY()
{
	UpdateData();
	thisScannerPtr->useY = b_useY.GetCheck();
}

void CBasePCGUIDlg::OnBnClickedUseZ()
{
	UpdateData();
	thisScannerPtr->useZ = b_useZ.GetCheck();
}

void CBasePCGUIDlg::OnEnChangeDmin()
{
		UpdateData();
thisScannerPtr->Dmin = atof(m_Dmin);
}

void CBasePCGUIDlg::OnEnChangeDmax()
{
		UpdateData();
	thisScannerPtr->Dmax = atof(m_Dmax);
}

void CBasePCGUIDlg::OnEnChangeXmin()
{
		UpdateData();
thisScannerPtr->Xmin = atof(m_Xmin);
}

void CBasePCGUIDlg::OnEnChangeXmax()
{
		UpdateData();
thisScannerPtr->Xmax = atof(m_Xmax);
}

void CBasePCGUIDlg::OnEnChangeYmin()
{
		UpdateData();
thisScannerPtr->Ymin = atof(m_Ymin);
}

void CBasePCGUIDlg::OnEnChangeYmax()
{
		UpdateData();
thisScannerPtr->Ymax = atof(m_Ymax);
}

void CBasePCGUIDlg::OnEnChangeZmin()
{
		UpdateData();
thisScannerPtr->Zmin = atof(m_Zmin);
}

void CBasePCGUIDlg::OnEnChangeZmax()
{
		UpdateData();
thisScannerPtr->Zmax = atof(m_Zmax);
}
