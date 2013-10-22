// ac3infoDlg.h : header file
//

#if !defined(AFX_AC3INFODLG_H__E363EF09_AFCB_11D2_A379_00805FE32B78__INCLUDED_)
#define AFX_AC3INFODLG_H__E363EF09_AFCB_11D2_A379_00805FE32B78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAc3infoDlg dialog

class CAc3infoDlg : public CDialog
{
// Construction
public:
	int GetBlocksize() {return (m_nPreambleSize + m_nSyncframeSize);}
	void ClearDialog();
	CString SyncFrameDuration(long numSyncframes);
	CString LongToString(long value);
	long m_lNumSyncframes;
	CString IntToString( int value );
	int BitsToSignedDecimal(CString bits);
	CString BitsToDecimalString(CString bits);
	CString StringToBits(CString inString);
	int BitsToDecimal( CString bits );
	CString ByteToBits( unsigned char byte );
	CAc3infoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAc3infoDlg)
	enum { IDD = IDD_AC3INFO_DIALOG };
	CString	m_strAc3File;
	CString	m_strFirstUTC;
	CString	m_strLastUTC;
	CString	m_strSamplingRate;
	CString	m_strFrameSizeCode;
	CString	m_strPreambleSize;
	CString	m_strSyncFrameSize;
	CString	m_strBsid;
	CString	m_strBsmod;
	CString	m_strAcmod;
	CString	m_strCmixlev;
	CString	m_strSurmixlev;
	CString	m_strDsurmod;
	BOOL	m_bLfeon;
	BOOL	m_bCopyrightb;
	BOOL	m_bOrigbs;
	CString	m_strDialnorm;
	CString	m_strCompr;
	BOOL	m_bCompre;
	CString	m_strLangcod;
	BOOL	m_bLangcode;
	BOOL	m_bAudprodie;
	CString	m_strMixlevel;
	CString	m_strRoomtyp;
	CString	m_strDialnorm2;
	CString	m_strCompr2;
	CString	m_strLangcod2;
	CString	m_strMixlevel2;
	CString	m_strRoomtyp2;
	BOOL	m_bCompr2e;
	BOOL	m_bLangcod2e;
	BOOL	m_bAudprodi2e;
	CString	m_strFirstTimecod;
	CString	m_strLastTimecod;
	BOOL	m_bFirstTimecod1e;
	BOOL	m_bFirstTimecod2e;
	BOOL	m_bLastTimecod1e;
	BOOL	m_bLastTimecod2e;
	CString	m_strNumSyncframes;
	CString	m_strSFDuration;
	CString	m_strUDuration;
	CString	m_strBitrate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAc3infoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long m_lBitrate;
	int m_nSyncframeSize;
	int m_nFrmsizecod;
	int m_nFscod;
	int m_nPreambleSize;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAc3infoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAc3Browse();
	afx_msg void OnChangeAc3File();
	afx_msg void OnAc3Info();
	afx_msg void OnSaveLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString TimecodeString(CString timecod1, CString timecod2);
	CString LookupBsmod(CString bitString, int acmod);
	CString LookupAcmod(CString bitString);
	CString LookupCmixlev(CString bitString);
	CString LookupSurmixlev(CString bitString);
	CString LookupDsurmod(CString bitString);
	CString LookupRoomtyp(CString bitString);
	CString LookupLangcod(CString bitString);
	CString LookupCompr(CString bitString);
	void ReadBitStreamInfo(CFile *ac3File);
	int SyncFrameSize( int fscod, int frmsizecod );
	void ReadSyncInfo( CFile *ac3File );
	long PreambleSize( CFile *ac3File );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AC3INFODLG_H__E363EF09_AFCB_11D2_A379_00805FE32B78__INCLUDED_)
