// ac3infoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ac3info.h"
#include "ac3infoDlg.h"
#include "utimecode.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CAc3infoDlg dialog

CAc3infoDlg::CAc3infoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAc3infoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAc3infoDlg)
	m_strAc3File = _T("");
	m_strFirstUTC = _T("");
	m_strLastUTC = _T("");
	m_strSamplingRate = _T("");
	m_strFrameSizeCode = _T("");
	m_strPreambleSize = _T("");
	m_strSyncFrameSize = _T("");
	m_strBsid = _T("");
	m_strBsmod = _T("");
	m_strAcmod = _T("");
	m_strCmixlev = _T("");
	m_strSurmixlev = _T("");
	m_strDsurmod = _T("");
	m_bLfeon = FALSE;
	m_bCopyrightb = FALSE;
	m_bOrigbs = FALSE;
	m_strDialnorm = _T("");
	m_strCompr = _T("");
	m_bCompre = FALSE;
	m_strLangcod = _T("");
	m_bLangcode = FALSE;
	m_bAudprodie = FALSE;
	m_strMixlevel = _T("");
	m_strRoomtyp = _T("");
	m_strDialnorm2 = _T("");
	m_strCompr2 = _T("");
	m_strLangcod2 = _T("");
	m_strMixlevel2 = _T("");
	m_strRoomtyp2 = _T("");
	m_bCompr2e = FALSE;
	m_bLangcod2e = FALSE;
	m_bAudprodi2e = FALSE;
	m_strFirstTimecod = _T("");
	m_strLastTimecod = _T("");
	m_bFirstTimecod1e = FALSE;
	m_bFirstTimecod2e = FALSE;
	m_bLastTimecod1e = FALSE;
	m_bLastTimecod2e = FALSE;
	m_strNumSyncframes = _T("");
	m_strSFDuration = _T("");
	m_strUDuration = _T("");
	m_strBitrate = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAc3infoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAc3infoDlg)
	DDX_Text(pDX, IDC_AC3_FILE, m_strAc3File);
	DDX_Text(pDX, IDC_FIRST_U_TC, m_strFirstUTC);
	DDX_Text(pDX, IDC_LAST_U_TC, m_strLastUTC);
	DDX_Text(pDX, IDC_FSCOD, m_strSamplingRate);
	DDX_Text(pDX, IDC_FRMSIZECOD, m_strFrameSizeCode);
	DDX_Text(pDX, IDC_PREAMBLE_SIZE, m_strPreambleSize);
	DDX_Text(pDX, IDC_SYNCFRAME_SIZE, m_strSyncFrameSize);
	DDX_Text(pDX, IDC_BSID, m_strBsid);
	DDX_Text(pDX, IDC_BSMOD, m_strBsmod);
	DDX_Text(pDX, IDC_ACMOD, m_strAcmod);
	DDX_Text(pDX, IDC_CMIXLEV, m_strCmixlev);
	DDX_Text(pDX, IDC_SURMIXLEV, m_strSurmixlev);
	DDX_Text(pDX, IDC_DSURMOD, m_strDsurmod);
	DDX_Check(pDX, IDC_LFEON, m_bLfeon);
	DDX_Check(pDX, IDC_COPYRIGHTB, m_bCopyrightb);
	DDX_Check(pDX, IDC_ORIGBS, m_bOrigbs);
	DDX_Text(pDX, IDC_DIALNORM, m_strDialnorm);
	DDX_Text(pDX, IDC_COMPR, m_strCompr);
	DDX_Check(pDX, IDC_COMPRE, m_bCompre);
	DDX_Text(pDX, IDC_LANGCOD, m_strLangcod);
	DDX_Check(pDX, IDC_LANGCODE, m_bLangcode);
	DDX_Check(pDX, IDC_AUDPRODIE, m_bAudprodie);
	DDX_Text(pDX, IDC_MIXLEVEL, m_strMixlevel);
	DDX_Text(pDX, IDC_ROOMTYP, m_strRoomtyp);
	DDX_Text(pDX, IDC_DIALNORM2, m_strDialnorm2);
	DDX_Text(pDX, IDC_COMPR2, m_strCompr2);
	DDX_Text(pDX, IDC_LANGCOD2, m_strLangcod2);
	DDX_Text(pDX, IDC_MIXLEVEL2, m_strMixlevel2);
	DDX_Text(pDX, IDC_ROOMTYP2, m_strRoomtyp2);
	DDX_Check(pDX, IDC_COMPR2E, m_bCompr2e);
	DDX_Check(pDX, IDC_LANGCOD2E, m_bLangcod2e);
	DDX_Check(pDX, IDC_AUDPRODI2E, m_bAudprodi2e);
	DDX_Text(pDX, IDC_FIRST_TIMECOD, m_strFirstTimecod);
	DDX_Text(pDX, IDC_LAST_TIMECOD, m_strLastTimecod);
	DDX_Check(pDX, IDC_FIRST_TIMECOD1E, m_bFirstTimecod1e);
	DDX_Check(pDX, IDC_FIRST_TIMECOD2E, m_bFirstTimecod2e);
	DDX_Check(pDX, IDC_LAST_TIMECOD1E, m_bLastTimecod1e);
	DDX_Check(pDX, IDC_LAST_TIMECOD2E, m_bLastTimecod2e);
	DDX_Text(pDX, IDC_NUM_SYNCFRAMES, m_strNumSyncframes);
	DDX_Text(pDX, IDC_EDIT2, m_strSFDuration);
	DDX_Text(pDX, IDC_U_DURATION, m_strUDuration);
	DDX_Text(pDX, IDC_BITRATE, m_strBitrate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAc3infoDlg, CDialog)
	//{{AFX_MSG_MAP(CAc3infoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_AC3_BROWSE, OnAc3Browse)
	ON_EN_CHANGE(IDC_AC3_FILE, OnChangeAc3File)
	ON_BN_CLICKED(IDC_AC3_INFO, OnAc3Info)
	ON_BN_CLICKED(IDC_SAVE_LOG, OnSaveLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAc3infoDlg message handlers

BOOL CAc3infoDlg::OnInitDialog()
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

	//	Get a poitner to the application object
	CWinApp*	myApp = AfxGetApp();
	//	Get the command line
	//	If any files were dropped on the icon, their names are listed
	//	in a space-delimited string m_lpCmdLine
	CString		cmdLine = myApp->m_lpCmdLine;
	//	Get the first file in the string
	//	Delete everything after (and including) the first space
	//	The order of files in the string seems to be arbitrary
	//	Can I sort these first?
	int	cLineLength = cmdLine.GetLength();
	if ( cLineLength > 0 )
	{
		int	cLineSpace = cmdLine.Find( ' ' );
		cmdLine.Delete( cLineSpace, ( cLineLength - cLineSpace ) );
		//	Put the filename in the AC-3 file field of the dialog box
		m_strAc3File = cmdLine;
		UpdateData( false );
		//	Go to work on the file
		OnAc3Info();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAc3infoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAc3infoDlg::OnPaint() 
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
HCURSOR CAc3infoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CAc3infoDlg::OnAc3Browse() 
{
	//	Set up filter string in the Microsoft-suggested way
	static char BASED_CODE szFilter[] =
		"AC-3 Streams (.ac3)|*.ac3|All files|*.*||";

	CFileDialog dlg(
		true,					//	bOpenFileDialog		Open file dialog
		NULL,					//	lpszDefExt			Default file extension
		NULL,					//	lpszFileName		Suggested filename
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,			//	dwFlags
		szFilter				//	lpszFilter			Filter String (see above)
		);

	if ( IDOK != dlg.DoModal() )	//	Display the dialog
		return;						//	The user hit cancel

	//	Get the full path of the file selected by the user
	//	and put it into the member variable for the appropriate edit box
	m_strAc3File = dlg.GetPathName();

	//	Update the dialog box to reflect the change
	UpdateData(FALSE);
}

void CAc3infoDlg::OnChangeAc3File() 
{
	UpdateData( TRUE );
}


void CAc3infoDlg::OnAc3Info() 
{
	CFile			ac3File;
	CFileException	e;
	CUTimecode		firstUTC, lastUTC;
	long			fileSize;
	BOOL			torf;

	ClearDialog();	//	Get rid of any pre-existing data

	//	Check filename field to make sure a file has been specified
	//	if none, give the user a chance to do it now
	if ( m_strAc3File.IsEmpty() )
	{
		//	Set up filter string in the Microsoft-suggested way
		static char BASED_CODE szFilter[] =
			"AC-3 Streams (.ac3)|*.ac3|All files|*.*||";

		CFileDialog dlg(
			true,					//	bOpenFileDialog		Open file dialog
			NULL,					//	lpszDefExt			Default file extension
			NULL,					//	lpszFileName		Suggested filename
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,			//	dwFlags
			szFilter				//	lpszFilter			Filter String (see above)
			);

		if ( IDOK != dlg.DoModal() )	//	Display the dialog
			return;						//	The user hit cancel

		//	Get the full path of the file selected by the user
		//	and put it into the member variable for the appropriate edit box
		m_strAc3File = dlg.GetPathName();

		//	Update the dialog box to reflect the change
		UpdateData(FALSE);
	}

	//	Open the AC-3 file
	ac3File.Open( m_strAc3File,			//	Open file object
		CFile::modeRead |				//	Read-only mode
		CFile::shareDenyNone |			//	Allow others to open
		CFile::typeBinary,				//	Set binary mode (no effect?)
		&e );							//	Exception to trap errors

	//	Check to see if any errors happened
	if ( e.m_cause != CFileException::none )
	{
		AfxMessageBox( "ERROR: File could not be opened, check name" );
		return;
	}

	ac3File.SeekToBegin();
	
	//	Read the first preamble and store it's length
	//	Assumption: preamble/syncframe sizes are consistent
	m_nPreambleSize = PreambleSize( &ac3File );
	m_strPreambleSize = IntToString( m_nPreambleSize );

	//	Read the syncinfo structure and get fscod and frmsizecod
	//	and calculate the syncframe size
	ReadSyncInfo( &ac3File );

	//	Calculate the bitrate from the syncframe size
	//	syncframe_size * 31.25 (ac-3 fps) * 8 (bits/byte) = bps
	m_lBitrate = m_nSyncframeSize * 250;
	m_strBitrate = LongToString( m_lBitrate ) + " bps";

	//	Read the bsi (bit stream information) structure
	ReadBitStreamInfo( &ac3File );

	//	Determine the size of the file
	fileSize = ac3File.GetLength();

	//	Calculate the number of syncframes
	m_lNumSyncframes = fileSize / ( m_nPreambleSize + m_nSyncframeSize );
	m_strNumSyncframes = LongToString( m_lNumSyncframes );

	//	Calculate the duration based on syncframes
	m_strSFDuration = SyncFrameDuration( m_lNumSyncframes );

	if ( m_nPreambleSize > 0 )
	{
		torf = firstUTC.IsDF( &ac3File );

		firstUTC.SetDF( torf );
		lastUTC.SetDF( torf );

		//	Seek to just after first preamble
		ac3File.Seek( m_nPreambleSize, CFile::begin );
		//	Get the first user time code
		firstUTC.SetTC( &ac3File );
		m_strFirstUTC = firstUTC.GetTC();

		//	Seek to just after last preamble
		ac3File.Seek( -(m_nSyncframeSize), CFile::end );
		//	Get the last user time code
		lastUTC.SetTC( &ac3File );
		m_strLastUTC = lastUTC.GetTC();

		//	Calculate a duration
		CUTimecode duration;
		duration = lastUTC - firstUTC;
		m_strUDuration = duration.GetShortTC();
	}

	//	Update the data displayed in the dialog box
	UpdateData( false );

	//	Close it
	ac3File.Close();
}


//
//	long PreambleSize(CFile *ac3File)
//
//	Reads past the preamble starting at current filepos
//	and returns its size in bytes
//	Return -1 if no syncword is found
//
long CAc3infoDlg::PreambleSize(CFile *ac3File)
{
	long	i = 0;				//	Bytes read
	UINT	nBytesRead;			//	Counter for bytes read during file reads
	BOOL	foundSync = false;	//	Flag to indicate we found the syncword
	unsigned char	byte;		//	Buffer for reading (1 byte at a time)

	while ( !foundSync )
	{
		nBytesRead = ac3File->Read( &byte, 1 );			//	Read 1 byte
		if ( nBytesRead == 0 )							//	Reached end of file
		{
			ac3File->Seek( -i, CFile::current );		//	Rewind all bytes read
			return( -1 );								//	Return error
		}

		if ( byte == '\x0b' )							//	Might be start of syncword
		{
			nBytesRead = ac3File->Read( &byte, 1 );		//	Read second byte
			if ( nBytesRead == 0 )						//	Reached end of file
			{
				ac3File->Seek( -(i + 1), CFile::current );	//	Rewind
				return( -1 );							//	Return error
			}

			if ( byte == '\x77' )						//	We found the syncword
			{
				ac3File->Seek( -2, CFile::current );	//	Rewind back over syncword
				foundSync = true;						//	Set our flag
			}
			else										//	Not the syncword
			{
				ac3File->Seek( -1, CFile::current );	//	Rewind 1 byte
				i++;									//	Increment byte counter
			}
		}
		else											//	Just another byte
			i++;										//	Increment byte counter
	}

	//	Syncword is found, file pointer sits just before it

	//	Return number of bytes read
	return( i );
}


//
//	void ReadSyncInfo(CFile *ac3File)
//
//	Reads the syncinfo block from a syncframe and
//	extracts fscod and frmsizecod information
//	Assumes file pointer is positioned at head of syncframe
//
void CAc3infoDlg::ReadSyncInfo(CFile *ac3File)
{
	UINT	nBytesRead;			//	Counter for bytes read during file reads
	unsigned char	byte;		//	Buffer for reading (1 byte at a time)
	CString	bitString, fscodBS, frmsizecodBS;

	//	Skip the syncword
	ac3File->Seek( 2, CFile::current );		//	Skip the syncword (2 bytes)
	ac3File->Seek( 2, CFile::current );		//	Skip the checksum (2 bytes)

	nBytesRead = ac3File->Read( &byte, 1 );
	if ( nBytesRead == 0 )
	{
		AfxMessageBox( "ERROR: unexpected EOF" );
		return;
	}

	//	Convert the byte into a bitstring
	bitString = ByteToBits( byte );

	fscodBS = "";
	frmsizecodBS = "";
	
	//	Copy the first two characters into fscod bitString
	fscodBS += bitString.GetAt( 0 );
	fscodBS += bitString.GetAt( 1 );

	//	Copy the last six characters into frmsizecod bitString
	frmsizecodBS += bitString.GetAt( 2 );
	frmsizecodBS += bitString.GetAt( 3 );
	frmsizecodBS += bitString.GetAt( 4 );
	frmsizecodBS += bitString.GetAt( 5 );
	frmsizecodBS += bitString.GetAt( 6 );
	frmsizecodBS += bitString.GetAt( 7 );

	//	Convert bitStrings back into numbers
	m_nFscod = BitsToDecimal( fscodBS );
	m_nFrmsizecod = BitsToDecimal( frmsizecodBS );

	switch( m_nFscod )
	{
		case 0: m_strSamplingRate = "48 kHz (" + fscodBS + ")"; break;
		case 1: m_strSamplingRate = "44.1 kHz (" + fscodBS + ")"; break;
		case 2: m_strSamplingRate = "32 kHz (" + fscodBS + ")"; break;
		case 3: m_strSamplingRate = "RES (" + fscodBS + ")"; break;
		default: m_strSamplingRate = "ERR (" + fscodBS + ")"; break;
	}

	m_strFrameSizeCode = "(" + frmsizecodBS + ")";
	m_nSyncframeSize = SyncFrameSize( m_nFscod, m_nFrmsizecod );
	m_strSyncFrameSize = IntToString( m_nSyncframeSize );
}


//
//	void ReadBitStreamInfo(CFile *ac3File)
//
//	Reads the bsi block from a syncframe and
//	extracts information
//	Assumes file pointer is positioned at head of bsi block
//
void CAc3infoDlg::ReadBitStreamInfo(CFile *ac3File)
{
	int				nBytesRead;		//	Counter for bytes read during file reads
	unsigned char	buffer[20];		//	Buffer for reading
	CString			bitString, readString, fieldString;
	int				i, c = 0;
	//	Stream Information Variables
	int				acmod;
	int				mixlevel, mixlevel2;
	CString			bsidStr, bsmodStr, acmodStr;
	CString			cmixlevStr, surmixlevStr, dsurmodStr;
	CString			lfeonStr, compreStr, compr2eStr, langcodeStr, langcod2eStr;
	CString			audprodieStr, audprodi2eStr, timecod1eStr, timecod2eStr;
	CString			timecod1Str, timecod2Str;
	CString			dialnormStr, dialnorm2Str, comprStr, compr2Str;
	CString			langcodStr, langcod2Str;
	CString			mixlevelStr, mixlevel2Str, roomtypStr, roomtyp2Str;
	CString			copyrightbStr, origbsStr;
	CString			bitBucket;
	//	For parsing the last syncblock
	CString			holderStr;
	int				holderVal;

	//	The Bit Stream Information (bsi) structure is variable in size
	//	Though it can be up to 627 bits (~79 bytes), much of it is
	//	Additional Bitsream Information, which is user info which we
	//	can safely ignore.  The information which can be interpreted
	//	has a maximum size of 108 bits (~14 bytes)

	nBytesRead = ac3File->Read( buffer, 14 );	//	Read 14 bytes
	if ( nBytesRead < 14 )						//	Hit end of file
		return;

	readString = buffer;						//	Copy it into a CString
	bitString = StringToBits( readString );		//	Make it a bitString

	//	Parse through it, one char at a time, setting variables as we go

	//	Bit Stream Iidentification (bsid) 5 bits
	bsidStr.Empty();
	for ( i = 0; i < 5; i++ )
		bsidStr += bitString.GetAt( c++ );
	m_strBsid = BitsToDecimalString( bsidStr ) + " (" + bsidStr + ")";

	//	Bit Stream Mode (bsmod) 3 bits
	bsmodStr.Empty();
	for ( i = 0; i < 3; i++ )
		bsmodStr += bitString.GetAt( c++ );

	//	Audio Coding Mode (acmod) 3 bits
	acmodStr.Empty();
	for ( i = 0; i < 3; i++ )
		acmodStr += bitString.GetAt( c++ );
	acmod = BitsToDecimal( acmodStr );

	m_strBsmod = LookupBsmod( bsmodStr, acmod ) + " (" + bsmodStr + ")";
	m_strAcmod = LookupAcmod( acmodStr ) + " (" + acmodStr + ")";

	//	Next can be two bits of various information
	if ( ( acmod & 0x1 ) && ( acmod != 0x1 ) )	//	if front 3 channels
	{
		//	Next two bits are center mix level (cmixlev) info
		cmixlevStr.Empty();
		for ( i = 0; i < 2; i++ )
			cmixlevStr += bitString.GetAt( c++ );
		m_strCmixlev = LookupCmixlev( cmixlevStr ) + " (" + cmixlevStr + ")";
	}

	if ( acmod & 0x4 )							//	if a surround channel exists
	{
		//	Next two bits are surrond mix level (surmixlev) info
		surmixlevStr.Empty();
		for ( i = 0; i < 2; i++ )
			surmixlevStr += bitString.GetAt( c++ );
		m_strSurmixlev = LookupSurmixlev( surmixlevStr ) + " (" + surmixlevStr + ")";
	}

	if ( acmod == 0x2 )						//	if in 2/0 mode
	{
		//	Next two bits are Dolby surround mode (dsmod) info
		dsurmodStr.Empty();
		for ( i = 0; i < 2; i++ )
			dsurmodStr += bitString.GetAt( c++ );
		m_strDsurmod = LookupDsurmod( dsurmodStr ) + " (" + dsurmodStr + ")";
	}

	//	The next bit indicates if the low frequency effects channel is on
	lfeonStr = bitString.GetAt( c++ );
	m_bLfeon = ( lfeonStr == "1" );

	//	The next 5 bits indicate dialog normalization level
	dialnormStr.Empty();
	for ( i = 0; i < 5; i++ )
		dialnormStr += bitString.GetAt( c++ );
	if ( BitsToDecimal( dialnormStr ) == 0 )
		m_strDialnorm = "reserved (" + dialnormStr + ")";
	else
	{
		m_strDialnorm = "-" + BitsToDecimalString( dialnormStr ) + " dB";
		m_strDialnorm += " (" + dialnormStr + ")";
	}

	//	Compression Gain Word Exists (compre) 1 bit
	compreStr = bitString.GetAt( c++ );
	m_bCompre = ( compreStr == "1" );

	if ( m_bCompre )
	{
		//	Compression Gain Word (compr) 8 bits
		comprStr.Empty();
		for ( i = 0; i < 8; i++ )
			comprStr += bitString.GetAt( c++ );
		m_strCompr = LookupCompr( comprStr ) + " dB (" + comprStr + ")";
	}

	//	Language Code Exists (langcode) 1 bit
	langcodeStr = bitString.GetAt( c++ );
	m_bLangcode = ( langcodeStr == "1" );

	if ( m_bLangcode )
	{
		//	Language Code (langcod) 8 bits
		langcodStr.Empty();
		for ( i = 0; i < 8; i++ )
			langcodStr += bitString.GetAt( c++ );
		m_strLangcod = LookupLangcod( langcodStr );
	}

	//	Audio Production Information Exists (audprodie) 1 bit
	audprodieStr = bitString.GetAt( c++ );
	m_bAudprodie = ( audprodieStr == "1" );

	if ( m_bAudprodie )
	{
		//	Mixing Level (mixlevel) 5 bits
		mixlevelStr.Empty();
		for ( i = 0; i < 5; i++ )
			mixlevelStr += bitString.GetAt( c++ );
		//	mixlevel is the peak mixing level of the channel minus 80 dB SPL
		mixlevel = BitsToDecimal( mixlevelStr ) + 80;
		m_strMixlevel = IntToString( mixlevel ) + " dB SPL (" + mixlevelStr + ")";

		//	Room Type (roomtyp) 2 bits
		roomtypStr.Empty();
		for ( i = 0; i < 2; i++ )
			roomtypStr += bitString.GetAt( c++ );
		m_strRoomtyp = LookupRoomtyp( roomtypStr ) + " (" + roomtypStr + ")";
	}

	if ( acmod == 0 )	//	if 1+1 mode (dual mono, some items need second value)
	{
		//	Dialog Normalization (dialnorm2) 5 bits
		dialnorm2Str.Empty();
		for ( i = 0; i < 5; i++ )
			dialnorm2Str += bitString.GetAt( c++ );
		if ( BitsToDecimal( dialnorm2Str ) == 0 )
			m_strDialnorm2 = "reserved (" + dialnorm2Str + ")";
		else
		{
			m_strDialnorm2 = "-" + BitsToDecimalString( dialnorm2Str ) + " dB";
			m_strDialnorm2 += " (" + dialnorm2Str + ")";
		}

		//	Compression Gain Word Exists (compr2e) 1 bit
		compr2eStr = bitString.GetAt( c++ );
		m_bCompr2e = ( compr2eStr == "1" );

		if ( m_bCompr2e )
		{
			//	Compression Gain Word (compr2) 8 bits
			compr2Str.Empty();
			for ( i = 0; i < 8; i++ )
				compr2Str += bitString.GetAt( c++ );
			m_strCompr2 = LookupCompr( compr2Str ) + " dB (" + compr2Str + ")";
		}

		//	Language Code Exists (langcod2e) 1 bit
		langcod2eStr = bitString.GetAt( c++ );
		m_bLangcod2e = ( langcod2eStr == "1" );

		if ( m_bLangcod2e )
		{
			//	Language Code (langcod2) 8 bits
			langcod2Str.Empty();
			for ( i = 0; i < 8; i++ )
				langcod2Str += bitString.GetAt( c++ );
			m_strLangcod2 = LookupLangcod( langcod2Str );
		}

		//	Audio Production Information Exists (audprodi2e) 1 bit
		audprodi2eStr = bitString.GetAt( c++ );
		m_bAudprodi2e = ( audprodi2eStr == "1" );

		if ( m_bAudprodi2e )
		{
			//	Mixing Level (mixlevel2) 5 bits
			mixlevel2Str.Empty();
			for ( i = 0; i < 5; i++ )
				mixlevel2Str += bitString.GetAt( c++ );
			//	mixlevel2 is the peak mixing level of the channel minus 80 dB SPL
			mixlevel2 = BitsToDecimal( mixlevel2Str ) + 80;
			m_strMixlevel2 = IntToString( mixlevel2 ) + " dB SPL (" + mixlevel2Str + ")";

			//	Room Type (roomtyp2) 2 bits
			roomtyp2Str.Empty();
			for ( i = 0; i < 2; i++ )
				roomtyp2Str += bitString.GetAt( c++ );
			m_strRoomtyp2 = LookupRoomtyp( roomtyp2Str ) + " (" + roomtyp2Str + ")";
		}
	}

	//	Copyright Bit (copyrightb) 1 bit
	copyrightbStr = bitString.GetAt( c++ );
	m_bCopyrightb = ( copyrightbStr == "1" );

	//	Original Bit Stream (origbs) 1 bit
	origbsStr = bitString.GetAt( c++ );
	m_bOrigbs = ( origbsStr == "1" );

	//	Timecode
	timecod1Str.Empty();
	timecod2Str.Empty();

	//	Time Code First Half Exists (timecod1e) 1 bit
	timecod1eStr = bitString.GetAt( c++ );
	m_bFirstTimecod1e = ( timecod1eStr == "1" );

	if ( m_bFirstTimecod1e )
	{
		//	Time Code First Half (timecod1) 14 bits
		for ( i = 0; i < 14; i++ )
			timecod1Str += bitString.GetAt( c++ );
	}

	//	Time Code Second Half Exists (timecod2e) 1 bit
	timecod2eStr = bitString.GetAt( c++ );
	m_bFirstTimecod2e = ( timecod2eStr == "1" );

	if ( m_bFirstTimecod2e )
	{
		//	Time Code Second Half (timecod2) 14 bits
		for ( i = 0; i < 14; i++ )
			timecod2Str += bitString.GetAt( c++ );
	}
	
	//	Interpret First Time Code
	m_strFirstTimecod = TimecodeString( timecod1Str, timecod2Str );

	//	Seek from the the end back one syncframe
	//	This assumes the filesize is an integral number of syncframes
	ac3File->Seek( -(m_nSyncframeSize ), CFile::end );
	ac3File->Seek( 5, CFile::current );			//	Skip si structure

	//	Read the syncframe (same as before)
	nBytesRead = ac3File->Read( buffer, 14 );	//	Read 14 bytes
	if ( nBytesRead < 14 )						//	Hit end of file
		return;

	readString = buffer;						//	Copy it into a CString
	bitString = StringToBits( readString );		//	Make it a bitString

	c = 0;										//	reset position pointer

	c += 8;										//	skip 8 bits (bsid and bsmod)
	holderStr.Empty();
	for ( i = 0; i <3; i++ )
		holderStr += bitString.GetAt( c++ );
	holderVal = BitsToDecimal( holderStr );
	
	if ( ( holderVal & 0x1 ) && ( holderVal != 0x1 ) )
		c += 2;									//	Skip 2 bits (cmixlev)
	if ( holderVal & 0x4 )
		c += 2;									//	Skip 2 bits (surmixlev)
	if ( holderVal == 0x2 )
		c += 2;									//	Skip 2 bits (dsurmod)

	c++;										//	Skip 1 bit (lfeon)

	c += 5;										//	Skip 5 bits (dialnorm)

	holderStr = bitString.GetAt( c++ );			//	comrpe
	if ( holderStr.GetAt( 0 ) == '1' )
		c += 8;									//	Skip 8 bits (compr)

	holderStr = bitString.GetAt( c++ );			//	langcode
	if ( holderStr.GetAt( 0 ) == '1' )
		c += 8;									//	Skip 8 bits (langcod)

	holderStr = bitString.GetAt( c++ );			//	audprodie
	if ( holderStr.GetAt( 0 ) == '1' )
		c += 7;									//	Skip 7 bits (mixlevel, roomtyp)

	//	holderVal still contains acmod
	if ( holderVal == 0 )
	{
		c += 5;									//	Skip 5 bits (dialnorm2)

		holderStr = bitString.GetAt( c++ );		//	compr2e
		if ( holderStr.GetAt( 0 ) == '1' )
			c += 8;								//	Skip 5 bits (compr2)

		holderStr = bitString.GetAt( c++ );		//	langcod2e
		if ( holderStr.GetAt( 0 ) == '1' )
			c += 8;								//	Skip 8 bits (langcod2)

		holderStr = bitString.GetAt( c++ );		//	audprodi2e
		if ( holderStr.GetAt( 0 ) == '1' )
			c += 7;								//	Skip 7 bits (mixlevel2, roomtyp2)
	}

	c++;										//	Skip 1 bit (copyrightb)

	c++;										//	Skip 1 bit (origbs)

	timecod1Str.Empty();
	timecod2Str.Empty();

	//	Time Code First Half Exists (timecod1e) 1 bit
	timecod1eStr = bitString.GetAt( c++ );
	m_bLastTimecod1e = ( timecod1eStr == "1" );

	if ( m_bLastTimecod1e )
	{
		//	Time Code First Half (timecod1) 14 bits
		for ( i = 0; i < 14; i++ )
			timecod1Str += bitString.GetAt( c++ );
	}

	//	Time Code Second Half Exists (timecod2e) 1 bit
	timecod2eStr = bitString.GetAt( c++ );
	m_bLastTimecod2e = ( timecod2eStr == "1" );

	if ( m_bLastTimecod2e )
	{
		//	Time Code Second Half (timecod2) 14 bits
		for ( i = 0; i < 14; i++ )
			timecod2Str += bitString.GetAt( c++ );
	}
	
	m_strLastTimecod = TimecodeString( timecod1Str, timecod2Str );
}


//
//	CString LookupBsmod(CString bitString, int acmod)
//
//	Returns string with bitstream mode
//	using table 5.2 (pg. 20) of ATSC document A/52
//
CString CAc3infoDlg::LookupBsmod(CString bitString, int acmod)
{
	int	bsmod = BitsToDecimal( bitString );

	switch( bsmod )
	{
		case 0: return "complete main (CM)"; break;
		case 1: return "music and effects (ME)"; break;
		case 2: return "visually impaired (VI)"; break;
		case 3: return "hearing impaired (HI)"; break;
		case 4: return "dialogue (D)"; break;
		case 5: return "commentary (C)"; break;
		case 6: return "emergency (E)"; break;
		case 7:
			if ( acmod == 1 )
				return "voice over (VO)";
			else
				return "karaoke";
			break;
		default: return "unrecognized"; break;
	}
}


//
//	CString LookupAcmod(CString bitString)
//
//	Returns string with audio coding mod
//	using table 5.3 (pg. 21) of ATSC document A/52
//
CString CAc3infoDlg::LookupAcmod(CString bitString)
{
	int acmod = BitsToDecimal( bitString );

	switch( acmod )
	{
		case 0: return "1+1 (Ch1,Ch2)"; break;
		case 1: return "1/0 (C)"; break;
		case 2: return "2/0 (L,R)"; break;
		case 3: return "3/0 (L,C,R)"; break;
		case 4: return "2/1 (L,R,S)"; break;
		case 5: return "3/1 (L,C,R,S)"; break;
		case 6: return "2/2 (L,R,SL,SR)"; break;
		case 7: return "3/2 (L,C,R,SL,SR)"; break;
		default: return "unrecognized"; break;
	}
}


//
//	CString LookupCmixlev(CString bitString)
//
//	Returns a string with center channel mix level info
//	using table 5.4 (pg. 21) of ATSC document A/52
//
CString CAc3infoDlg::LookupCmixlev(CString bitString)
{
	int cmixlev = BitsToDecimal( bitString );

	switch( cmixlev )
	{
		case 0: return "0.707 (-3dB)"; break;
		case 1: return "0.500 (-6dB)"; break;
		case 2: return "0"; break;
		case 3: return "reserved"; break;
		default: return "unrecognized"; break;
	}
}


//
//	CString LookupSurmixlev(CString bitString)
//
//	Returns a string with surround channel mix level info
//	using table 5.5 (pg. 22) of ATSC document A/52
//
CString CAc3infoDlg::LookupSurmixlev(CString bitString)
{
	return LookupCmixlev( bitString );	//	Values are same as cmixlev
}


//
//	CString LookupDsurmod(CString bitString)
//
//	Returns a string with Dolby surround mode info
//	using table 5.6 (pg. 22) of ATSC document A/52
//
CString CAc3infoDlg::LookupDsurmod(CString bitString)
{
	int dsurmod = BitsToDecimal( bitString );

	switch( dsurmod )
	{
		case 0: return "not indicated"; break;
		case 1: return "not encoded"; break;
		case 2: return "encoded"; break;
		case 3: return "reserved"; break;
		default: return "unrecognized"; break;
	}
}


//
//	double LookupCompr(CString bitBucket)
//
//	Returns the Compression Gain value
//	using table 7.30 (pg. 81) of ATSC document A/52
//
CString CAc3infoDlg::LookupCompr(CString bitString)
{
	int		i, value;
	double	gain;
	CString	bitBucket, returnStr;
	char	*buffer, readChar;
	int		dec, sign;

	//	bitBucket contains two 4 bit values
	//	Get the first four bits
	bitBucket.Empty();
	for ( i = 0; i < 4; i++ )
		bitBucket += bitString.GetAt( i );
	value = BitsToDecimal( bitBucket );
	switch( value )
	{
		case 0: gain = 6.02; break;
		case 1: gain = 12.04; break;
		case 2: gain = 18.06; break;
		case 3: gain = 24.08; break;
		case 4: gain = 30.10; break;
		case 5: gain = 36.12; break;
		case 6: gain = 42.14; break;
		case 7: gain = 48.16; break;
		case 8: gain = -42.14; break;
		case 9: gain = -36.12; break;
		case 10: gain = -30.10; break;
		case 11: gain = -24.08; break;
		case 12: gain = -18.06; break;
		case 13: gain = -12.04; break;
		case 14: gain = -6.02; break;
		case 15: gain = 0; break;
		default: gain = 0; break;
	}
	
	//	For their next trick, the geeks have decided that the next four bits
	//	will specify a fine-detail adjustment to the value of the gain above.
	//	But they express this as: "The value of Y is a linear representation
	//	of a gain change of up to -6 dB.  Y is considered to be an unsigned
	//	fractional integer, with a leading value of 1, or: .01 Y(4) Y(5) Y(6)
	//	Y(7) (base 2).  Y can represent values between 0.11111(2) (or 31/32)
	//	and 0.10000(2) or (1/2).  Thus, Y can represent gain changes from
	//	-0.28 dB to -6.02 dB." (ATSC document A/52, page 81)
	//
	//	Fractional integer?  Isn't that a contradiction in terms?
	//	If you take the whole number Y, add a leading 1 and divide by 2 ^ 5
	//	you get to the number.
	//	So, what they really mean is ((Y | 0x10) / 32)

	//	The Dolby encoder uses a value of 11111111 (0xFF) to indicate no gain
	//	This would suggest that if the first four bits indicate a coarse gain
	//	of 0 dB, the second four bits are ignored.  This is because with the
	//	leading 1 assumed for the fine gain, a value of 0 is not possible
	//	Thus, the smallest value possible is 0.5 dB (which would not be
	//	noticable to most listeners)
	//	If no compression is desired, the Compression Exists bit should be
	//	set to 0.

	//	Get the second four bits
	bitBucket = "1";	//	Assumed leading "1"
	for ( i = 4; i < 8; i++ )
		bitBucket += bitString.GetAt( i );
	gain -= ( (double)BitsToDecimal( bitBucket ) / (double)32 );

	//	Turn gain into a string with 2 digit precision
	buffer = _fcvt( gain, 2, &dec, &sign );

	returnStr.Empty();

	if ( sign != 0 )				//	Set the sign
		returnStr += "-";
	if ( dec == 0 )					//	Add leading zero
		returnStr += "0";
	for ( i = 0; i < dec; i++ )		//	Copy digits before decimal point
		returnStr += buffer[i];
	returnStr += ".";				//	Add decimal point
	readChar = ' ';					//	Copy digits after decimal point
	while ( readChar != '\0' )
	{
		readChar = buffer[i++];
		if ( readChar )
			returnStr += readChar;
	}
	
	return returnStr;
}


//
//	CString LookupLangcod(CString bitString)
//
//	Returns string with encoded language
//	using table 5.14 (pg. 39) of ATSC document A/52
//
CString CAc3infoDlg::LookupLangcod(CString bitString)
{
	int	langcod;

	langcod = BitsToDecimal(bitString);

	switch( langcod )
	{
		case 0x0: return "(0x00) unknown/n.a."; break;
		case 0x1: return "(0x01) Albanian"; break;
		case 0x2: return "(0x02) Breton"; break;
		case 0x3: return "(0x03) Catalan"; break;
		case 0x4: return "(0x04) Croatian"; break;
		case 0x5: return "(0x05) Welsh"; break;
		case 0x6: return "(0x06) Czech"; break;
		case 0x7: return "(0x07) Danish"; break;
		case 0x8: return "(0x08) German"; break;
		case 0x9: return "(0x09) English"; break;
		case 0xA: return "(0x0A) Spanish"; break;
		case 0xB: return "(0x0B) Esperanto"; break;
		case 0xC: return "(0x0C) Estonian"; break;
		case 0xD: return "(0x0D) Basque"; break;
		case 0xE: return "(0x0E) Faroese"; break;
		case 0xF: return "(0x0F) French"; break;
		case 0x10: return "(0x10) Frisian"; break;
		case 0x11: return "(0x11) Irish"; break;
		case 0x12: return "(0x12) Gaelic"; break;
		case 0x13: return "(0x13) Galician"; break;
		case 0x14: return "(0x14) Icelandic"; break;
		case 0x15: return "(0x15) Italian"; break;
		case 0x16: return "(0x16) Lappish"; break;
		case 0x17: return "(0x17) Latin"; break;
		case 0x18: return "(0x18) Latvian"; break;
		case 0x19: return "(0x19) Luxembourgian"; break;
		case 0x1A: return "(0x1A) Lithuanian"; break;
		case 0x1B: return "(0x1B) Hungarian"; break;
		case 0x1C: return "(0x1C) Maltese"; break;
		case 0x1D: return "(0x1D) Dutch"; break;
		case 0x1E: return "(0x1E) Norwegian"; break;
		case 0x1F: return "(0x1F) Occitan"; break;
		case 0x20: return "(0x20) Polish"; break;
		case 0x21: return "(0x21) Portuguese"; break;
		case 0x22: return "(0x22) Romanian"; break;
		case 0x23: return "(0x23) Romanish"; break;
		case 0x24: return "(0x24) Serbian"; break;
		case 0x25: return "(0x25) Slovak"; break;
		case 0x26: return "(0x26) Slovene"; break;
		case 0x27: return "(0x27) Finnish"; break;
		case 0x28: return "(0x28) Swedish"; break;
		case 0x29: return "(0x29) Turkish"; break;
		case 0x2A: return "(0x2A) Flemish"; break;
		case 0x2B: return "(0x2B) Waloon"; break;
		case 0x2C: return "(0x2C)"; break;
		case 0x2D: return "(0x2D)"; break;
		case 0x2E: return "(0x2E)"; break;
		case 0x2F: return "(0x2F)"; break;
		case 0x30: return "(0x30) reserved"; break;
		case 0x31: return "(0x31) reserved"; break;
		case 0x32: return "(0x32) reserved"; break;
		case 0x33: return "(0x33) reserved"; break;
		case 0x34: return "(0x34) reserved"; break;
		case 0x35: return "(0x35) reserved"; break;
		case 0x36: return "(0x36) reserved"; break;
		case 0x37: return "(0x37) reserved"; break;
		case 0x38: return "(0x38) reserved"; break;
		case 0x39: return "(0x39) reserved"; break;
		case 0x3A: return "(0x3A) reserved"; break;
		case 0x3B: return "(0x3B) reserved"; break;
		case 0x3C: return "(0x3C) reserved"; break;
		case 0x3D: return "(0x3D) reserved"; break;
		case 0x3E: return "(0x3E) reserved"; break;
		case 0x3F: return "(0x3F) reserved"; break;
		case 0x40: return "(0x40) bg sound"; break;
		case 0x41: return "(0x41)"; break;
		case 0x42: return "(0x42)"; break;
		case 0x43: return "(0x43)"; break;
		case 0x44: return "(0x44)"; break;
		case 0x45: return "(0x45) Zulu"; break;
		case 0x46: return "(0x46) Vietnamese"; break;
		case 0x47: return "(0x47) Uzbek"; break;
		case 0x48: return "(0x48) Urdu"; break;
		case 0x49: return "(0x49) Ukrainian"; break;
		case 0x4A: return "(0x4A) Thai"; break;
		case 0x4B: return "(0x4B) Telugu"; break;
		case 0x4C: return "(0x4C) Tatar"; break;
		case 0x4D: return "(0x4D) Tamil"; break;
		case 0x4E: return "(0x4E) Tadzhik"; break;
		case 0x4F: return "(0x4F) Swahili"; break;
		case 0x50: return "(0x50) Sranan Tongo"; break;
		case 0x51: return "(0x51) Somali"; break;
		case 0x52: return "(0x52) Sinhalese"; break;
		case 0x53: return "(0x53) Shona"; break;
		case 0x54: return "(0x54) Serbo-Croat"; break;
		case 0x55: return "(0x55) Ruthenian"; break;
		case 0x56: return "(0x56) Russian"; break;
		case 0x57: return "(0x57) Quechua"; break;
		case 0x58: return "(0x58) Pustu"; break;
		case 0x59: return "(0x59) Punjabi"; break;
		case 0x5A: return "(0x5A) Persian"; break;
		case 0x5B: return "(0x5B) Papamiento"; break;
		case 0x5C: return "(0x5C) Oriya"; break;
		case 0x5D: return "(0x5D) Nepali"; break;
		case 0x5E: return "(0x5E) Ndebele"; break;
		case 0x5F: return "(0x5F) Marathi"; break;
		case 0x60: return "(0x60) Moldavian"; break;
		case 0x61: return "(0x61) Malaysian"; break;
		case 0x62: return "(0x62) Malagasay"; break;
		case 0x63: return "(0x63) Macedonian"; break;
		case 0x64: return "(0x64) Laotian"; break;
		case 0x65: return "(0x65) Korean"; break;
		case 0x66: return "(0x66) Khmer"; break;
		case 0x67: return "(0x67) Kazakh"; break;
		case 0x68: return "(0x68) Kannada"; break;
		case 0x69: return "(0x69) Japanese"; break;
		case 0x6A: return "(0x6A) Indonesian"; break;
		case 0x6B: return "(0x6B) Hindi"; break;
		case 0x6C: return "(0x6C) Hebrew"; break;
		case 0x6D: return "(0x6D) Hausa"; break;
		case 0x6E: return "(0x6E) Gurani"; break;
		case 0x6F: return "(0x6F) Gujurati"; break;
		case 0x70: return "(0x70) Greek"; break;
		case 0x71: return "(0x71) Georgian"; break;
		case 0x72: return "(0x72) Fulani"; break;
		case 0x73: return "(0x73) Dari"; break;
		case 0x74: return "(0x74) Churash"; break;
		case 0x75: return "(0x75) Chinese"; break;
		case 0x76: return "(0x76) Burmese"; break;
		case 0x77: return "(0x77) Bulgarian"; break;
		case 0x78: return "(0x78) Bengali"; break;
		case 0x79: return "(0x79) Belorussian"; break;
		case 0x7A: return "(0x7A) Bambora"; break;
		case 0x7B: return "(0x7B) Azerbijani"; break;
		case 0x7C: return "(0x7C) Assamese"; break;
		case 0x7D: return "(0x7D) Armenian"; break;
		case 0x7E: return "(0x7E) Arabic"; break;
		case 0x7F: return "(0x7F) Amharic"; break;
		default: return "Not recognized"; break;
	}
}


//
//	CString LookupRoomtyp(CString bitString)
//
//	Returns string with room type
//	using table 5.7 (pg. 24) of ATSC document A/52
//
CString CAc3infoDlg::LookupRoomtyp(CString bitString)
{
	int	roomtyp = BitsToDecimal( bitString );

	switch( roomtyp )
	{
		case 0: return "not indicated"; break;
		case 1: return "lg. room, X curve monitor"; break;
		case 2: return "small room, flat monitor"; break;
		case 3: return "reserved"; break;
		default: return "not recognized"; break;
	}
}


//
//	int SyncFrameSize(int fscod, int frmsizecod)
//
//	Returns bytes in syncframe
//	using table 5.13 (pg. 38) of ATSC document A/52
//
int CAc3infoDlg::SyncFrameSize(int fscod, int frmsizecod)
{
	switch( fscod )
	{
		case 0:	//	48 kHz sampling rate
			switch( frmsizecod )
			{
				case 0: case 1: return ( 2 * 64 ); break;
				case 2: case 3: return ( 2 * 80 ); break;
				case 4: case 5: return ( 2 * 96 ); break;
				case 6: case 7: return ( 2 * 112 ); break;
				case 8: case 9: return ( 2 * 128 ); break;
				case 10: case 11: return ( 2 * 160 ); break;
				case 12: case 13: return ( 2 * 192 ); break;
				case 14: case 15: return ( 2 * 224 ); break;
				case 16: case 17: return ( 2 * 256 ); break;
				case 18: case 19: return ( 2 * 320 ); break;
				case 20: case 21: return ( 2 * 384 ); break;
				case 22: case 23: return ( 2 * 448 ); break;
				case 24: case 25: return ( 2 * 512 ); break;
				case 26: case 27: return ( 2 * 640 ); break;
				case 28: case 29: return ( 2 * 768 ); break;
				case 30: case 31: return ( 2 * 896 ); break;
				case 32: case 33: return ( 2 * 1024 ); break;
				case 34: case 35: return ( 2 * 1152 ); break;
				case 36: case 37: return ( 2 * 1280 ); break;
			}
			break;
		case 1:
			switch( frmsizecod )
			{
				case 0: return ( 2 * 69 ); break;
				case 1: return ( 2 * 70 ); break;
				case 2: return ( 2 * 87 ); break;
				case 3: return ( 2 * 88 ); break;
				case 4: return ( 2 * 104 ); break;
				case 5: return ( 2 * 105 ); break;
				case 6: return ( 2 * 121 ); break;
				case 7: return ( 2 * 122 ); break;
				case 8: return ( 2 * 139 ); break;
				case 9: return ( 2 * 140 ); break;
				case 10: return ( 2 * 174 ); break;
				case 11: return ( 2 * 175 ); break;
				case 12: return ( 2 * 208 ); break;
				case 13: return ( 2 * 209 ); break;
				case 14: return ( 2 * 243 ); break;
				case 15: return ( 2 * 244 ); break;
				case 16: return ( 2 * 278 ); break;
				case 17: return ( 2 * 279 ); break;
				case 18: return ( 2 * 348 ); break;
				case 19: return ( 2 * 349 ); break;
				case 20: return ( 2 * 417 ); break;
				case 21: return ( 2 * 418 ); break;
				case 22: return ( 2 * 487 ); break;
				case 23: return ( 2 * 488 ); break;
				case 24: return ( 2 * 557 ); break;
				case 25: return ( 2 * 558 ); break;
				case 26: return ( 2 * 696 ); break;
				case 27: return ( 2 * 697 ); break;
				case 28: return ( 2 * 835 ); break;
				case 29: return ( 2 * 836 ); break;
				case 30: return ( 2 * 975 ); break;
				case 31: return ( 2 * 976 ); break;
				case 32: return ( 2 * 1114 ); break;
				case 33: return ( 2 * 1115 ); break;
				case 34: return ( 2 * 1253 ); break;
				case 35: return ( 2 * 1254 ); break;
				case 36: return ( 2 * 1393 ); break;
				case 37: return ( 2 * 1394 ); break;
			}
			break;
		case 2:	//	32 kHz sampling rate
			switch( frmsizecod )
			{
				case 0: case 1: return ( 2 * 96 ); break;
				case 2: case 3: return ( 2 * 120 ); break;
				case 4: case 5: return ( 2 * 144 ); break;
				case 6: case 7: return ( 2 * 168 ); break;
				case 8: case 9: return ( 2 * 192 ); break;
				case 10: case 11: return ( 2 * 240 ); break;
				case 12: case 13: return ( 2 * 288 ); break;
				case 14: case 15: return ( 2 * 336 ); break;
				case 16: case 17: return ( 2 * 384 ); break;
				case 18: case 19: return ( 2 * 480 ); break;
				case 20: case 21: return ( 2 * 576 ); break;
				case 22: case 23: return ( 2 * 672 ); break;
				case 24: case 25: return ( 2 * 768 ); break;
				case 26: case 27: return ( 2 * 960 ); break;
				case 28: case 29: return ( 2 * 1152 ); break;
				case 30: case 31: return ( 2 * 1344 ); break;
				case 32: case 33: return ( 2 * 1536 ); break;
				case 34: case 35: return ( 2 * 1728 ); break;
				case 36: case 37: return ( 2 * 1920 ); break;
			}
			break;
		default:	//	Sampling rate code is either reserved or in error
			return 0;
			break;
	}
	return 0;	//	In case frmsizecod > 37
}

//
//	CString	SyncFrameDuration(long numSyncframes)
//
//	Returns duration of stream
//	based on given number of syncframes
//
CString CAc3infoDlg::SyncFrameDuration(long numSyncframes)
{
	double	duration;
	CString	durationStr;
	int		HH, MM, SS, FF;

	duration = numSyncframes / 31.25;

	//	Get hours
	HH = (int)floor( duration / ( 60 * 60 ) );
	duration -= ( HH * ( 60 * 60 ) );
	//	Get minutes
	MM = (int)floor( duration / ( 60 ) );
	duration -= ( MM * 60 );
	//	Get seconds
	SS = (int)floor( duration / ( 1 ) );
	duration -= SS;
	//	Remainder is frames as fraction of a second
	FF = (int)floor( duration * 31.25 );

	//	Build string from time elements
	durationStr = IntToString( HH ) + ":";
	if ( MM < 10 )
		durationStr += "0";
	durationStr += IntToString( MM ) + ":";
	if ( SS < 10 )
		durationStr += "0";
	durationStr += IntToString( SS ) + ":";
	durationStr += IntToString( FF );

	return durationStr;
}


CString CAc3infoDlg::TimecodeString(CString timecod1, CString timecod2)
{
	BOOL	timecod1e, timecod2e;
	CString	bitBucket, timecodeStr;
	int		i, ss;

	timecodeStr.Empty();

	timecod1e = ( !timecod1.IsEmpty() );
	timecod2e = ( !timecod2.IsEmpty() );

	if ( timecod1e )
	{
		//	Hours (timecod1[0..4]) 5 bits
		bitBucket.Empty();
		for ( i = 0; i < 5; i++ )
			bitBucket += timecod1.GetAt( i );
		timecodeStr += BitsToDecimalString( bitBucket ) + ":";

		//	Minutes (timecod1[5..10]) 6 bits
		bitBucket.Empty();
		for ( i = 5; i < 11; i++ )
			bitBucket += timecod1.GetAt( i );
		timecodeStr += BitsToDecimalString( bitBucket ) + ":";

		//	Seconds (coarse) (timecod1[11..13]) 3 bits
		bitBucket.Empty();
		for ( i = 11; i < 14; i++ )
			bitBucket += timecod1.GetAt( i );
		ss = 8 * BitsToDecimal( bitBucket );

		if ( timecod2e )
		{
			//	Seconds (fine) (timecod2[0..2]) 3 bits
			bitBucket.Empty();
			for ( i = 0; i < 3; i++ )
				bitBucket += timecod2.GetAt( i );
			ss += BitsToDecimal( bitBucket );

			timecodeStr += IntToString( ss ) + ":";	//	Is this DFTC?

			//	Frames (timecod2[3..7]) 5 bits
			bitBucket.Empty();
			for ( i = 3; i < 8; i++ )
				bitBucket += timecod2.GetAt( i );
			timecodeStr += BitsToDecimalString( bitBucket ) + " ";

			//	1/64 frames (timecod2[8..13]) 6 bits
			bitBucket.Empty();
			for ( i = 8; i < 14; i++ )
				bitBucket += timecod2.GetAt( i );
			timecodeStr += BitsToDecimalString( bitBucket ) + "/64";
		}
		else
			timecodeStr += IntToString( ss );
	}
	else if ( timecod2e )
	{
		//	Hours (timecod2[0..4]) 5 bits
		bitBucket.Empty();
		for ( i = 0; i < 5; i++ )
			bitBucket += timecod2.GetAt( i );
		timecodeStr += BitsToDecimalString( bitBucket ) + ":";

		//	Minutes (timecod2[5..10]) 6 bits
		bitBucket.Empty();
		for ( i = 5; i < 11; i++ )
			bitBucket += timecod2.GetAt( i );
		timecodeStr += BitsToDecimalString( bitBucket ) + ":";

		//	Seconds (coarse) (timecod2[11..13]) 3 bits
		bitBucket.Empty();
		for ( i = 11; i < 14; i++ )
			bitBucket += timecod2.GetAt( i );
		ss = 8 * BitsToDecimal( bitBucket );

		timecodeStr += IntToString( ss );
	}

	return timecodeStr;
}


//
//	CString ByteToBits(unsigned char byte)
//
//	converts one byte value to binary string
//
CString CAc3infoDlg::ByteToBits(unsigned char byte)
{
	CString bits = "";
	int i, digit;

	for ( i = 7; i >= 0; i-- )
	{
		byte -= ( digit = (int)( byte / (int)pow( 2, i ) ) ) * (int)pow( 2, i );
		bits += ( '0' + digit );
	}

	return bits;
}


//
//	CString StringToBits(CString inString)
//
//	Takes a string and turns it into a binary string
//	Converts each byte in the string into a 8-bit string
//	Allows for bitwise parsing of a stream
//
CString CAc3infoDlg::StringToBits(CString inString)
{
	CString	outString = "";
	int i;

	for ( i = 0; i < ( inString.GetLength() ); i++ )
		outString += ByteToBits( inString.GetAt( i ) );

	return outString;
}


//
//	int BitsToDecimal(CString bits)
//
//	converts a binary string to a decimal value
//
int CAc3infoDlg::BitsToDecimal(CString bits)
{
	int		i;
	double	x = 0;

	for ( i = 0; i < bits.GetLength(); i++ )
		x += ( ( bits.GetAt( bits.GetLength() - 1 - i ) == '1' ) * pow( 2, i ) );

	return( (int)x );
}


//
//	int	BitsToSignedDecimal(CString bits)
//
//	converts a binary string into a signed integer
//	first bit is used as sign
//
int CAc3infoDlg::BitsToSignedDecimal(CString bits)
{
	int		i, x;
	CString	noSign;
	BOOL	sign;

	sign = ( bits.GetAt( 0 ) == '1' );
	noSign.Empty();
	for ( i = 1; i < bits.GetLength(); i++ )
		noSign += bits.GetAt( i );
	x = BitsToDecimal( noSign );
	if ( sign )
		x *= -1;
	return( x );
}


//
//	CString BitsToDecimalString(CString bits)
//
//	Converts binary string to a decimal value string
//
CString CAc3infoDlg::BitsToDecimalString(CString bits)
{
	int x = BitsToDecimal( bits );
	return IntToString( x );
}

CString CAc3infoDlg::IntToString(int value)
{
	char buffer [20];				//	Should be a good general purpose size

	_itoa( value, buffer, 10 );
	return (CString)buffer;
}

CString CAc3infoDlg::LongToString(long value)
{
	char buffer[20];

	_ltoa( value, buffer, 10 );
	return (CString)buffer;
}

void CAc3infoDlg::ClearDialog()
{
	//	Empty every member variable except ac3 filename
	m_bAudprodi2e = false;
	m_bAudprodie = false;
	m_bCompr2e = false;
	m_bCompre = false;
	m_bCopyrightb = false;
	m_bFirstTimecod1e = false;
	m_bFirstTimecod2e = false;
	m_bLangcod2e = false;
	m_bLangcode = false;
	m_bLastTimecod1e = false;
	m_bLastTimecod2e = false;
	m_bLfeon = false;
	m_bOrigbs = false;
	
	m_lNumSyncframes = 0;

	m_nFrmsizecod = 0;
	m_nFscod = 0;
	m_nPreambleSize = 0;
	m_nSyncframeSize = 0;

	m_strAcmod.Empty();
	m_strBitrate.Empty();
	m_strBsid.Empty();
	m_strCmixlev.Empty();
	m_strCompr.Empty();
	m_strCompr2.Empty();
	m_strDialnorm.Empty();
	m_strDialnorm2.Empty();
	m_strDsurmod.Empty();
	m_strFirstTimecod.Empty();
	m_strFirstUTC.Empty();
	m_strFrameSizeCode.Empty();
	m_strLangcod.Empty();
	m_strLangcod2.Empty();
	m_strLastTimecod.Empty();
	m_strLastUTC.Empty();
	m_strMixlevel.Empty();
	m_strMixlevel2.Empty();
	m_strNumSyncframes.Empty();
	m_strPreambleSize.Empty();
	m_strRoomtyp.Empty();
	m_strRoomtyp2.Empty();
	m_strSamplingRate.Empty();
	m_strSFDuration.Empty();
	m_strSurmixlev.Empty();
	m_strSyncFrameSize.Empty();
	m_strUDuration.Empty();

	UpdateData( false );
}

void CAc3infoDlg::OnSaveLog() 
{
	CString		logFileName, suggFile;
	int			s, f, l, i;

	//*****	Make up a suggested log file name	*****//

	logFileName = m_strAc3File;

	l = logFileName.GetLength();
	logFileName.MakeReverse();
	s = logFileName.Find( '\\', 0 );
	f = logFileName.Find( '.', 0 );
	logFileName.MakeReverse();

	if ( s < 0 ) s = l;
	if ( f < 0 ) f = 0;
	else f++;

	suggFile.Empty();
	for ( i = ( l - s ); i <= ( l - f - 1 ); i++ )
		suggFile += logFileName.GetAt( i );

	suggFile += ".txt";

	//*****	Prompt the user for a log file name	*****//

	//	Set up filter string in the Microsoft-suggested way
	static char BASED_CODE szFilter[] =
		"Text files (.txt)|*.txt|All files|*.*||";

	CFileDialog dlg(
		false,					//	bOpenFileDialog		Save file dialog
		"txt",					//	lpszDefExt			Default file extension
		suggFile,				//	lpszFileName		Suggested filename
		OFN_OVERWRITEPROMPT |	//	dwFlags
		OFN_HIDEREADONLY,
		szFilter				//	lpszFilter			Filter String (see above)
		);

	if ( dlg.DoModal() != IDOK )	//	If user hit anything but OK
		return;

	logFileName = dlg.GetPathName();

	//*****	Prepare info to write	*****//

	CString	logString;

	logString = "AC-3 Info Report\n\n";
	logString += "File: " + m_strAc3File + "\n\n";
	logString += "* User Data Preamble *\n";
	logString += " Size: " + m_strPreambleSize + "\n";
	if ( m_nPreambleSize > 0 )
	{
		logString += " First Timecode: " + m_strFirstUTC + "\n";
		logString += " Last Timecode: " + m_strLastUTC + "\n";
		logString += " Calculated Duration: " + m_strUDuration + "\n";
	}
	logString += "\n";
	logString += "* Syncblock *\n";
	logString += " Sampling Rate: " + m_strSamplingRate + "\n";
	logString += " frmsizecod: " + m_strFrameSizeCode + "\n";
	logString += " Syncframe Size: " + m_strSyncFrameSize + "\n";
	logString += " Number of Syncframes: " + m_strNumSyncframes + "\n";
	logString += " Calculated Duration: " + m_strSFDuration + " (AC-3 frames)\n";
	logString += " Bitrate: " + m_strBitrate + "\n\n";

	logString += "* Bit Stream Information (BSI) *\n";
	logString += " ID: " + m_strBsid + "\n";
	logString += " Mode: " + m_strBsmod + "\n";
	logString += " Audio Coding Mode: " + m_strAcmod + "\n";
	logString += " Center Mix Level: " + m_strCmixlev + "\n";
	logString += " Surround Mix Level: " + m_strSurmixlev + "\n";
	logString += " Dolby Surround Mode: " + m_strDsurmod + "\n";
	logString += " Low Frequency Effects Channel: ";
	if ( m_bLfeon )
		logString += "on\n";
	else
		logString += "off\n";
	logString += " Copyright: ";
	if ( m_bCopyrightb )
		logString += "on\n";
	else
		logString += "off\n";
	logString += " Original: ";
	if ( m_bOrigbs )
		logString += "on\n";
	else
		logString += "off\n";
	logString += "\n";

	logString += "Main Audio Channel\n";
	logString += " Dialog Normalization: " + m_strDialnorm + "\n";
	logString += " Compression Gain: ";
	if ( m_bCompre )
		logString += m_strCompr + "\n";
	else
		logString += "not stated\n";
	logString += " Language Code: ";
	if ( m_bLangcode )
		logString += m_strLangcod + "\n";
	else
		logString += "not stated\n";
	logString += " Mix Level: ";
	if ( m_bAudprodie )
		logString += m_strMixlevel + "\n";
	else
		logString += "not stated\n";
	logString += " Room Type: ";
	if ( m_bAudprodie )
		logString += m_strRoomtyp + "\n";
	else
		logString += "not stated\n";
	logString += "\n";

	logString += "Second Audio Channel (Dual Mono Mode)\n";
	logString += " Dialog Normalization: " + m_strDialnorm2 + "\n";
	logString += " Compression Gain: ";
	if ( m_bCompr2e )
		logString += m_strCompr + "\n";
	else
		logString += "not stated\n";
	logString += " Language Code: ";
	if ( m_bLangcod2e )
		logString += m_strLangcod2 + "\n";
	else
		logString += "not stated\n";
	logString += " Mix Level: ";
	if ( m_bAudprodi2e )
		logString += m_strMixlevel2 + "\n";
	else
		logString += "not stated\n";
	logString += " Room Type: ";
	if ( m_bAudprodi2e )
		logString += m_strRoomtyp2 + "\n";
	else
		logString += "not stated\n";
	logString += "\n";

	logString += "AC-3 Timecode\n";
	logString += " First Timecode: " + m_strFirstTimecod + "\n";
	logString += " Last Timecode: " + m_strLastTimecod + "\n";

	//*****	Open/create the file	*****//

	CStdioFile	logFile;
	BOOL		fileError;

	fileError = logFile.Open( logFileName,
		CFile::typeText|CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive,
		NULL );
	if ( fileError == false )
	{
		AfxMessageBox( "Could not create the log file", MB_OK, 0 );
		return;
	}

	//*****	Write the information to the file	*****//
	logFile.WriteString( logString );

	//*****	Close the file	*****//
	logFile.Close();
}
