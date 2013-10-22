// UTimecode.h: interface for the CUTimecode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTIMECODE_H__575DB6A1_B47C_11D2_A37E_00805FE32B78__INCLUDED_)
#define AFX_UTIMECODE_H__575DB6A1_B47C_11D2_A37E_00805FE32B78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUTimecode  
{
public:
	CUTimecode operator-(CUTimecode TC);
	BOOL IsDF(CFile *ac3File);
	long GetTotalFrames( void );
	void SetDF(BOOL torf) {m_bDF = torf;}
	BOOL GetDF() {return m_bDF;}
	CString GetTC();
	CString GetShortTC();
	void SetTC(CFile *ac3File);
	void SetTC(long totalFrames);
	void SetTC(long totalFrames, BOOL DF);
	void SetTC(int HH, int MM, int SS, int FF, int WW, BOOL DF);
	BOOL m_bDF;		//	Drop Frame flag
	int m_nWW;		//	Samples
	int m_nFF;		//	Frames
	int m_nSS;		//	Seconds
	int m_nMM;		//	Minutes
	int m_nHH;		//	Hours
	BOOL m_bE;		//	Exists flag
	CUTimecode(int HH, int MM, int SS, int FF, int WW, BOOL DF);
	CUTimecode(int HH, int MM, int SS, int FF, int WW);
	CUTimecode();
	virtual ~CUTimecode();

protected:
	CString IntToString(int value);
	int BitsToDecimal(CString bits);
	CString ByteToBits(unsigned char byte);

};

#endif // !defined(AFX_UTIMECODE_H__575DB6A1_B47C_11D2_A37E_00805FE32B78__INCLUDED_)
