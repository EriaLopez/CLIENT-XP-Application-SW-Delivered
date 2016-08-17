// BasePCGUIDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_BASEPCGUIDLG_H__8C646E82_1978_4F74_8B28_9C01E48EE1EB__INCLUDED_)
#define AFX_BASEPCGUIDLG_H__8C646E82_1978_4F74_8B28_9C01E48EE1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBasePCGUIDlg dialog


// for openGL
#include "OpenGLControl.h"


class CBasePCGUIDlg : public CDialog
{
// Construction
public:
	CBasePCGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBasePCGUIDlg)
	enum { IDD = IDD_BASEPCGUI_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasePCGUIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBasePCGUIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_currentScannerNumber;
	CString m_currentScannerPtr;
	CString m_azimuthSpeed;
	CString m_azimuthPosition;
	CString m_elevationSpeed;
	CString m_azimuthStart;
	CString m_elevationStart;
	CString m_pointsPerLine;

	CString m_elevationResolution;
	CString m_azimuthEnd;
	CString m_Dmin;
	CString m_Dmax;
	CString m_Xmin;
	CString m_Xmax;
	CString m_Ymin;
	CString m_Ymax;
	CString m_Zmin;
	CString m_Zmax;
	afx_msg void OnEnChangeEditazstart();
	afx_msg void OnBnClickedButtonsinglescan();
	afx_msg void OnBnClickedButtonhaltscan();
	CString m_statusDisplay;
	afx_msg void OnBnClickedButtonazimuthposition();
	afx_msg void OnBnClickedButtonazimuthspeed();
	afx_msg void OnBnClickedButtonelevationspeed();
	afx_msg void OnBnClickedButtonabortscan();

	afx_msg void OnBnClickedButtonstatusdisplay();
	afx_msg void OnStnClickedStaticazstart();
	afx_msg void OnEnChangeEditazspeed();

	// for openGL
	private:
		COpenGLControl m_oglWindow;
public:
	afx_msg void OnStnClickedStaticpointsperline();
	afx_msg void OnEnChangeEditelspeed();
	afx_msg void OnBnClickedButtonazimuthparameters();
	afx_msg void OnBnClickedButtonelevationparameters();
	afx_msg void OnEnChangeEditelstart();
	CString m_ackStatusDisplay;
	void upDateAckStatus(short stat);
	afx_msg void OnEnChangeEditcommandack();
	afx_msg void OnBnClickedButtonstatusdisplay2();
	afx_msg void OnBnClickedButtonstatusdisplay3();
	afx_msg void OnBnClickedSleep();
	afx_msg void OnBnClickedConvert();
	afx_msg void OnBnClickedButtonstatusdisplay4();
	afx_msg void OnBnClickedButtonDisplay();
	afx_msg void OnBnClickedUseDist();
	afx_msg void OnBnClickedUseX();
	afx_msg void OnBnClickedUseY();
	afx_msg void OnBnClickedUseZ();
	afx_msg void Xmin();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeDmin();
	afx_msg void OnEnChangeDmax();
	afx_msg void OnEnChangeXmin();
	afx_msg void OnEnChangeXmax();
	afx_msg void OnEnChangeYmin();
	afx_msg void OnEnChangeYmax();
	afx_msg void OnEnChangeZmin();
	afx_msg void OnEnChangeZmax();
	CButton b_useDist;
	CButton b_useX;
	CButton b_useY;
	CButton b_useZ;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnEnChangeMfceditbrowse1();
    // //eclopezv file name to open and display
    CWnd fileName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_BASEPCGUIDLG_H__8C646E82_1978_4F74_8B28_9C01E48EE1EB__INCLUDED_)
