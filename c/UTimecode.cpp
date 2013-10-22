// UTimecode.cpp: implementation of the CUTimecode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ac3info.h"
#include "UTimecode.h"
#include "Ac3infoDlg.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUTimecode::CUTimecode(int HH, int MM, int SS, int FF, int WW, BOOL DF)
{
	m_bE = true;
	m_nHH = HH;
	m_nMM = MM;
	m_nSS = SS;
	m_nFF = FF;
	m_nWW = WW;
	m_bDF = DF;
}

CUTimecode::CUTimecode(int HH, int MM, int SS, int FF, int WW)
{
	m_bE = true;
	m_nHH = HH;
	m_nMM = MM;
	m_nSS = SS;
	m_nFF = FF;
	m_nWW = WW;
}

CUTimecode::CUTimecode()
{
	m_bE = false;
	m_nHH = 0;
	m_nMM = 0;
	m_nSS = 0;
	m_nFF = 0;
	m_nWW = 0;
	m_bDF = false;
}

CUTimecode::~CUTimecode()
{

}

void CUTimecode::SetTC(int HH, int MM, int SS, int FF, int WW, BOOL DF)
{
	m_bE = true;
	m_nHH = HH;
	m_nMM = MM;
	m_nSS = SS;
	m_nFF = FF;
	m_nWW = WW;
	m_bDF = DF;
}

void CUTimecode::SetTC(long totalFrames, BOOL DF)
{
	m_bDF = DF;
	SetTC( totalFrames );
}

void CUTimecode::SetTC(long totalFrames)
{
	if ( m_bDF )
	{
		totalFrames += ( (long)floor( totalFrames / ( 60 * 30 ) ) -
			(long)floor( floor( totalFrames / ( 60 * 30 ) ) / 10 ) ) * 2;
	}

	m_nHH = (int)floor( totalFrames / ( 60 * 60 * 30 ) );
	totalFrames -= m_nHH * ( 60 * 60 * 30 );
	
	m_nMM = (int)floor( totalFrames / ( 60 * 30 ) );
	totalFrames -= m_nMM * ( 60 * 30 );

	m_nSS = (int) floor( totalFrames / ( 30 ) );
	totalFrames -= m_nSS * ( 30 );

	m_nFF = totalFrames;

	m_nWW = 0;
}

void CUTimecode::SetTC(CFile *ac3File)
{
	//	Read the user time code from the current syncframe preamble
	//	and set our member variables with it
	//
	//	Assumption: file pointer of ac3File sits just after a 16-byte preamble
	//	Back up and read the user time code from that preamble
	//	When done, return the file pointer to where it was

	CString			bitString, digit1, digit2;
	UINT			nBytesRead;
	int				i, c = 0;
	unsigned char	byte, buffer[20];

	m_bE = true;

	ac3File->Seek( -16, CFile::current );		//	Back up to preamble start
	nBytesRead = ac3File->Read( buffer, 16 );	//	Read the preamble
	//	Do some error checking here

	c += 3;	//	Skip first three bytes

	byte = buffer[c++];
	bitString = ByteToBits( byte );			//	Convert to bitstring
	digit1.Empty();
	digit2.Empty();
	for ( i = 0; i < 4; i++ )
	{
		digit1 += bitString.GetAt( i );		//	Extract 10's digit
		digit2 += bitString.GetAt( i + 4 );	//	Extract 1's digit
	}
	m_nHH = 10 * BitsToDecimal( digit1 );	//	Set hours member
	m_nHH += BitsToDecimal( digit2 );

	c++;									//	Skip 1 byte

	//	Set Minutes member
	byte = buffer[c++];
	bitString = ByteToBits( byte );
	digit1.Empty();
	digit2.Empty();
	for ( i = 0; i < 4; i++ )
	{
		digit1 += bitString.GetAt( i );
		digit2 += bitString.GetAt( i + 4 );
	}
	m_nMM = 10 * BitsToDecimal( digit1 );
	m_nMM += BitsToDecimal( digit2 );

	c++;

	//	Set Seconds member
	byte = buffer[c++];
	bitString = ByteToBits( byte );
	digit1.Empty();
	digit2.Empty();
	for ( i = 0; i < 4; i++ )
	{
		digit1 += bitString.GetAt( i );
		digit2 += bitString.GetAt( i + 4 );
	}
	m_nSS = 10 * BitsToDecimal( digit1 );
	m_nSS += BitsToDecimal( digit2 );

	c++;

	//	Set Frames member
	byte = buffer[c++];
	bitString = ByteToBits( byte );
	digit1.Empty();
	digit2.Empty();
	for ( i = 0; i < 4; i++ )
	{
		digit1 += bitString.GetAt( i );
		digit2 += bitString.GetAt( i + 4 );
	}
	m_nFF = 10 * BitsToDecimal( digit1 );
	m_nFF += BitsToDecimal( digit2 );

	//	Set Samples member
	byte = buffer[c++];
	bitString = ByteToBits( byte );
	byte = buffer[c++];
	bitString += ByteToBits( byte );
	m_nWW = BitsToDecimal( bitString );

	//	There is no drop frame flag in the TC data
	//	Leave the DF flag alone (must be set separately)
	//	If I try to do it here, we get an error
}

CString CUTimecode::GetTC()
{
	CString	returnStr;

	returnStr = GetShortTC();

	returnStr += "+";

	if ( m_nWW < 1000 )
		returnStr += "0";
	if ( m_nWW < 100 )
		returnStr += "0";
	if ( m_nWW < 10 )
		returnStr += "0";
	returnStr += IntToString( m_nWW );

	return( returnStr );
}

CString CUTimecode::GetShortTC()
{
	CString	returnStr;

	returnStr.Empty();

	if ( m_nHH < 10 )
		returnStr += "0";
	returnStr += IntToString( m_nHH ) + ":";

	if ( m_nMM < 10 )
		returnStr += "0";
	returnStr += IntToString( m_nMM ) + ":";

	if ( m_nSS < 10 )
		returnStr += "0";
	returnStr += IntToString( m_nSS );

	if ( m_bDF ) returnStr += ".";
	else returnStr += ":";

	if ( m_nFF < 10 )
		returnStr += "0";
	returnStr += IntToString( m_nFF );

	return( returnStr );
}


//
//	long GetTotalFrames( void )
//
//	Returns timecode expressed as number of SMPTE frames
//	from 0:00:00:0, adjusting for DFTC if necessary
//
long CUTimecode::GetTotalFrames()
{
	long	frames;

	//	For base and non drop:
	//	hours = 10800 frames (60 * 60 * 30)
	//	minutes = 1800 frames (60 * 30)
	//	seconds = 30 frames
	//	frames = 1 frame
	frames = ( m_nHH * 60 * 60 * 30 ) + ( m_nMM * 60 * 30 ) +
		( m_nSS * 30 ) + m_nFF;

	if ( m_bDF )
	{
		//	For every hour, drop 108 frames ( ( 60 - 6 ) * 2 )
		//	For every minute, drop 2 frames
		//	For every 10th minute, add back 2 frames
		frames -= ( m_nHH * ( 60 - 6 ) * 2 ) +
			( m_nMM * 2 ) - ( (long)( floor( m_nMM / 10 ) * 2 ) );
	}

	return frames;
}


//
//	BOOL IsDF(CFile *ac3File)
//
//	Tells if timecode in the specified file is drop frame time code
//
BOOL CUTimecode::IsDF(CFile *ac3File)
{
	unsigned long	fileSize;
	long			filePos;
	int				blocksize, preambleSize = 16;
	CUTimecode		TC1, TC2;
	BOOL			done = false, returnValue = false;
	int				numFrames;

	//	Get the blocksize (preamble + syncframe) from our main object
	CAc3infoDlg *dlg = (CAc3infoDlg*)AfxGetApp()->m_pMainWnd;
	blocksize = dlg->GetBlocksize();

	fileSize = ac3File->GetLength();		//	Store the file size
	filePos = ac3File->GetPosition();		//	Store current position
	ac3File->SeekToBegin();					//	Rewind file

	ac3File->Seek( preambleSize, CFile::current );	//	Skip to end of preamble
	TC1.SetTC( ac3File );					//	Grab a timecode

	//	Check to see if the minute ends in a zero (in which case
	//	no frames will be dropped and we cannot use it to test
	//	for drop frame time code)
	//	If it is, skip ahead one minute to get to a usable minute
	if ( ( ( TC1.m_nMM + 1 ) % 10 ) == 0 )
	{
		ac3File->Seek( ( blocksize * 60 * 30 ), CFile::current );
		TC1.SetTC( ac3File );
	}

	//	Figure out how many frames until the end of the minute
	//	(roughly -- timecodes repeat every now and then due to
	//	the NTSC/AC-3 framerate mismatch, but if we get within
	//	a few seconds, it's pretty quick to scan through that)
	numFrames = ( 29 - TC1.m_nFF ) + ( ( 59 - TC1.m_nSS ) * 30 );
	ac3File->Seek( ( numFrames * blocksize ), CFile::current );

	TC2.SetTC( ac3File );

	while ( !done )
	{
		if ( ac3File->GetPosition() > ( fileSize - 16 ) )
			done = true;					//	We've run out of file
		else
		{
			TC1 = TC2;
			ac3File->Seek( blocksize, CFile::current );
			TC2.SetTC( ac3File );
			
			if ( ( TC1.m_nSS == 59 ) && ( TC1.m_nFF == 29 ) )
			{
				switch ( TC2.m_nFF )
				{
					case 0:					//	No frames were dropped
						if ( ( TC2.m_nMM % 10 ) != 0 )
						{
							returnValue = false;
							done = true;
						}
						break;
					case 2:					//	Frames were dropped
						returnValue = true;
						done = true;
						break;
				}							//	Ignore anything else
			}
		}
	}

	ac3File->Seek( filePos, CFile::begin );	//	Restore file position
	return returnValue;
}


//
//	CString ByteToBits(unsigned char byte)
//
//	converts one byte value to binary string
//
CString CUTimecode::ByteToBits(unsigned char byte)
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
//	int BitsToDecimal(CString bits)
//
//	converts a binary string to a decimal value
//
int CUTimecode::BitsToDecimal(CString bits)
{
	int		i;
	double	x = 0;

	for ( i = 0; i < bits.GetLength(); i++ )
		x += ( ( bits.GetAt( bits.GetLength() - 1 - i ) == '1' ) * pow( 2, i ) );

	return( (int)x );
}



CString CUTimecode::IntToString(int value)
{
	char buffer [20];				//	Should be a good general purpose size

	_itoa( value, buffer, 10 );
	return (CString)buffer;
}

CUTimecode CUTimecode::operator-(CUTimecode TC)
{
	CUTimecode	outTC;

	outTC.SetTC( ( this->GetTotalFrames() - TC.GetTotalFrames() ), TC.m_bDF );

	return outTC;
}
