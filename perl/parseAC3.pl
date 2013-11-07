#!/usr/bin/perl
use strict;	# Enforce some good programming rules

#####
#
# 	parseAC3.pl
# 	version:	0.9
# 	created:	2011-03-31
# 	modified:	2013-11-04
# 	author:		Theron Trowbridge
#
# 	description:
# 		extract header values from AC-3 file
# 
# 	syntax:
# 		parseAC3.pl <input.ac3>
# 
# 		accepts glob arguments (i.e., "parseAC3.pl *.ac3")
# 
# 	output:
# 		outputs to stdio
# 		can be piped to a text file or other tool
#
#####

### 	consider output to XML file, like the WAV file parser

# constants

my $AC3_SYNC_WORD = pack( 'H[4]', "0b77" );

# variables

my ( $input_file, $errors, $warnings, $extension, $result, $file_size );
my ( $byte, $bitfield );
my ( $syncinfo, $syncword, $crc1, $fscod, $frmsizecod );
my ( $bitrate_in_kbps, $sampling_rate_in_khz, $num_words_in_syncframe );
my ( $syncframe_size, $total_syncframes, $total_seconds );
my ( $byte_pointer, $bit_pointer );
my $bsi;
my ( $bsid, $bsmod, $acmod, $cmixlev, $surmixlev, $dsurmod, $lfeon );
my ( $bsid_bitfield, $bsmod_bitfield, $acmod_bitfield );
my ( $cmixlev_bitfield, $surmixlev_bitfield, $dsurmod_bitfield );
my ( $dialnorm_bitfield, $compre_bitfield, $compr_bitfield );
my ( $langcode_bitfield, $langcod_bitfield );
my ( $audprodie_bitfield, $mixlev_bitfield, $roomtyp_bitfield );
my ( $dialnorm, $compre, $compr, $langcode, $langcod, $audprodie, $mixlev, $roomtyp );
my ( $dialnorm2_bitfield, $compr2e_bitfield, $compr2_bitfield, $langcod2e_bitfield );
my ( $langcod2_bitfield, $audprodi2e_bitfield, $mixlev2_bitfield, $roomtyp2_bitfield );
my ( $dialnorm2, $compr2e, $compr2, $langcod2e, $langcod2, $audprodi2e, $mixlev2, $roomtyp2 );
my ( $compr_dB, $compr_dB_rounded, $compr2_dB, $compr2_dB_rounded );
my ( $copyrightb, $origbs, $timecod1e, $timecod1_bitfield, $timecod1 );
my ( $timecod2e, $timecod2_bitfield, $timecod2, $addbsie );
my ( $addbsil_bitfield, $addbsil, $addbsi );


# main

# if no arguments passed, return usage string
if ( !$ARGV[0] ) { print "Usage: parseAC3.pl <filename>\n"; }

# otherwise, parse through each argument passed and try to parse it
FILE: foreach $input_file (@ARGV) {
	# set error/warning flags
	$errors = $warnings = 0;
	
	# check file name (is extension .ac3?)
	$input_file =~ /\.([^\.]+)$/;
	$extension = $1;
	if ( $extension ne "ac3" ) {
		print "$input_file: warning: file name extension is not .ac3\n";
		$warnings++;
	}
	
	# open the file
	$result = open( INPUT_FILE, "<", $input_file );
	if ( $result eq undef ) {
		print "$input_file: error: coud not open file $!\n";
		$errors++;
		print "input_file: aborting with $errors errors and $warnings warnings\n";
		next FILE;
	}
	
	binmode( INPUT_FILE );			# binary file
	$file_size = -s INPUT_FILE;		# get size of input file
	
	# the earliest versions of the A/53 spec (circa 1995) had an Annex B (Informative)
	# which provides a mechanism for putting AC-3 bitstreams into S/PDIF streams
	# (or, presumably AES)
	# part of this Annex involved a preamble which could contain a timestamp
	# meaning the first two bytes of the file might not be the AC-3 syncword
	# 
	# Soft Encode (and other AC-3 encoders) from this time period could add this preamble
	# and it was still considered a valid AC-3 file,
	# though not all decoders will recognize it (then or now)
	# 
	# Annex B has been replaced by a bibliography
	
	###	rewind file
	###	read a byte into a buffer
	### read a byte at a time and add to the buffer
	### 	check to see if last two bytes are "0x0b77" (AC-3 syncword)
	### rewind to before the syncword
	### all bytes prior to syncword are a preamble
	
	# preamble is detailed in A/52-1995, Annex B section 4.4
	#	preamble starts with 64 bits
	# 	Preamble Word		Contents
	# 	Pa					16-bit sync word 1 = 0xF872
	# 	Pb					16-bit sync word 2 = 0x4E1F
	# 	Pc					16-bit burst_info value
	# 	Pd					16-bit length_code (unsigned integer)
	# 							equal to number of data bits in the following data burst
	#
	# but this isn't actually the format used
	#
	# time stamps are detailed in Annex B section 4.8
	# 
	# 4.8.2 Time stamp payload
	# 	The time stamp payload, shown in Table 8, has a minimum length of six 16-bit words
	# 	which have a defined meaning. Additional 16-bit words may be optionally added,
	# 	but the meaning of these words is not specified.
	#
	# 
	# 01 10 00 00 00 00 00 00 00 00 00 00 00 14 22 06
	# 01 10 00 00 00 01 00 08 00 1F 01 C0 00 14 22 06
	# 
	# Table 8 shows the bits and I don't get it
	# it doesn't seem to mesh with my C++ code
	# 
	
	# the first two bytes of the file should be 0x0b77
	# but there can be a short (16 byte) preamble in which time stamps can be written
	# Soft Encode allowed these as optional, and my C++ program dealt with them
	# but there is no mention in the current version of the A/52 spec
	# so no current AC-3 streams should have the preambles,
	# but it would be good to be able to detect them anyhow, as older streams may have them
	# 
	# so read the first bit and look for the syncword 0x0B77
	# if there are 16 bytes in front of it, decode the time stamp
	# and throw a warning
	
	my $preamble;
	$result = read( INPUT_FILE, $preamble, 16 );		# read first 16 bytes - this could be a preamble
	
	my $test_word = substr( $preamble, 0, 2 );
#	my $compare_word = pack( 'H[4]', "0b77" );			# AC-3 syncword is 0x0b77
	
	if ( $test_word eq $AC3_SYNC_WORD ) {
		seek( INPUT_FILE, 0, 0 );						# rewind the file
	} else {
		print "WARNING: A/53 (1995) Annex B IEC958:1989 (S/PDIF) Interface Preamble Found\n";
		$warnings++;
		
		# try to extract a timestamp
	}
	
#	$syncword = unpack( 'H[4]', substr( $syncinfo, 0, 2 ) );


	
#	$result = read( INPUT_FILE, $syncinfo, 5 );
#	if ( $result == undef ) {
#		print "$input_file: error: could not read syncinfo() $1\n";
#		$errors++;
#		print "$input_file: aborting with $errors errors and $warnings warnings\n";
#		close( INPUT_FILE );
#		next FILE;
#	}

	
	###
	###	there is sometimes a 16 byte preamble that contains a timecode
	### I am having trouble finding documentation on this
	### it might be a really old standard, but SoftEncode supports it
	### and I built in support in my old AC3Info tool
	### 
	### if first two bytes are not the expected 0x0B77,
	### we need to scan forward to find them
	### according to the C++ code
	### first three bytes are some sort of header
	### 01 10 00 - probably a ID and preamble size?
	### then 8 bits for hours???
	### 	4 bits for 10s place
	###		4 bits for 1s place
	### then 8 bits of nothing???
	### then 8 bits for minutes???
	###		4 bits for the 10s place
	### 	4 bits for the 1s place
	### then 8 bits of nothing???
	### then 8 bits for seconds???
	###		4 bits for the 10s place
	### 	4 bits for the 1s place
	### then 8 bits of nothing???
	### then 8 bits for frames???
	###		4 bits for the 10s place
	### 	4 bits for the 1s place
	### then 8 bits of nothing???
	### then 16 bits for sample number???
	###	
	### byte	value
	###	0		header
	### 1		header
	### 2		header
	### 3		hours
	### 4		0x00
	### 5		minutes
	### 6		0x00
	### 7		seconds
	### 8		0x00
	### 9		frames
	### 10		sample
	### 11		sample
	### 12		
	### 13		
	### 14		
	### 15		
	### 
	### that leaves us some left over bytes and I don't know what they are
	### in my sample file they're the same - 00 14 22 06
	###
	
	###
	### sample preambles:
	###
	###	01 10 00 00 00 00 00 00 00 00 00 00 00 14 22 06
	###	01 10 00 00 00 01 00 08 00 1F 01 C0 00 14 22 06
	###
	###
	###	header		hours		minutes		seconds		frames	samples		?? ?? ?? ??
	###	01 10 00	00	00		01	00		08	00		1F		01 C0		00 14 22 06
	###	01 10 00	00	00		00	00		00	00		00		00 00		00 14 22 06
	###
	### 1F =	0001 1111
	###			1*10 = 10	+	15 = 25
	### 01 C0	0000 0001 1100 0000 = 448
	###	does this match what AC3Info says?		YES!!!

	
	#	code from C++ program
	#
	# void CUTimecode::SetTC(CFile *ac3File)
	# {
	# 	//	Read the user time code from the current syncframe preamble
	# 	//	and set our member variables with it
	# 	//
	# 	//	Assumption: file pointer of ac3File sits just after a 16-byte preamble
	# 	//	Back up and read the user time code from that preamble
	# 	//	When done, return the file pointer to where it was
	# 
	# 	CString			bitString, digit1, digit2;
	# 	UINT			nBytesRead;
	# 	int				i, c = 0;
	# 	unsigned char	byte, buffer[20];
	# 
	# 	m_bE = true;
	# 
	# 	ac3File->Seek( -16, CFile::current );		//	Back up to preamble start
	# 	nBytesRead = ac3File->Read( buffer, 16 );	//	Read the preamble
	# 	//	Do some error checking here
	# 
	# 	c += 3;	//	Skip first three bytes
	# 
	# 	byte = buffer[c++];
	# 	bitString = ByteToBits( byte );			//	Convert to bitstring
	# 	digit1.Empty();
	# 	digit2.Empty();
	# 	for ( i = 0; i < 4; i++ )
	# 	{
	# 		digit1 += bitString.GetAt( i );		//	Extract 10's digit
	# 		digit2 += bitString.GetAt( i + 4 );	//	Extract 1's digit
	# 	}
	# 	m_nHH = 10 * BitsToDecimal( digit1 );	//	Set hours member
	# 	m_nHH += BitsToDecimal( digit2 );
	# 
	# 	c++;									//	Skip 1 byte
	# 
	# 	//	Set Minutes member
	# 	byte = buffer[c++];
	# 	bitString = ByteToBits( byte );
	# 	digit1.Empty();
	# 	digit2.Empty();
	# 	for ( i = 0; i < 4; i++ )
	# 	{
	# 		digit1 += bitString.GetAt( i );
	# 		digit2 += bitString.GetAt( i + 4 );
	# 	}
	# 	m_nMM = 10 * BitsToDecimal( digit1 );
	# 	m_nMM += BitsToDecimal( digit2 );
	# 
	# 	c++;
	# 
	# 	//	Set Seconds member
	# 	byte = buffer[c++];
	# 	bitString = ByteToBits( byte );
	# 	digit1.Empty();
	# 	digit2.Empty();
	# 	for ( i = 0; i < 4; i++ )
	# 	{
	# 		digit1 += bitString.GetAt( i );
	# 		digit2 += bitString.GetAt( i + 4 );
	# 	}
	# 	m_nSS = 10 * BitsToDecimal( digit1 );
	# 	m_nSS += BitsToDecimal( digit2 );
	# 
	# 	c++;
	# 
	# 	//	Set Frames member
	# 	byte = buffer[c++];
	# 	bitString = ByteToBits( byte );
	# 	digit1.Empty();
	# 	digit2.Empty();
	# 	for ( i = 0; i < 4; i++ )
	# 	{
	# 		digit1 += bitString.GetAt( i );
	# 		digit2 += bitString.GetAt( i + 4 );
	# 	}
	# 	m_nFF = 10 * BitsToDecimal( digit1 );
	# 	m_nFF += BitsToDecimal( digit2 );
	# 
	# 	//	Set Samples member
	# 	byte = buffer[c++];
	# 	bitString = ByteToBits( byte );
	# 	byte = buffer[c++];
	# 	bitString += ByteToBits( byte );
	# 	m_nWW = BitsToDecimal( bitString );
	# 
	# 	//	There is no drop frame flag in the TC data
	# 	//	Leave the DF flag alone (must be set separately)
	# 	//	If I try to do it here, we get an error
	# }


	
	
	# read Synchronization Information
	#
	# syncinfo()
	# {
	# 	syncword	// 16				// always 0x0b77
	# 	crc1		// 16				// CRC of first 5/8 of syncframe
	# 	fscod		// 2				// Sample Rate Code (sampling rate)
	# 	frmsizecod	// 6				// Frame Size Code
	# } /* end of syncinfo */
	#
	$result = read( INPUT_FILE, $syncinfo, 5 );
	if ( $result == undef ) {
		print "$input_file: error: could not read syncinfo() $1\n";
		$errors++;
		print "$input_file: aborting with $errors errors and $warnings warnings\n";
		close( INPUT_FILE );
		next FILE;
	}
	
	# parse syncinfo()
	$syncword = unpack( 'H[4]', substr( $syncinfo, 0, 2 ) );
	$crc1 = unpack( 'H[4]', substr( $syncinfo, 2, 2 ) );

	## note fscod and frmsizecod are a packed bitfield
	$bitfield = unpack( "B[8]", substr( $syncinfo, 4, 1 ) );
	$fscod = binary_to_decimal( substr( $bitfield, 0, 2 ) );
	$frmsizecod = binary_to_decimal( substr( $bitfield, 2, 6 ) );
	
	$bitrate_in_kbps = getBitrate( $frmsizecod );
	$sampling_rate_in_khz = getSamplingRate( $fscod );
	
	$num_words_in_syncframe = evalFrmsizecodValue( $frmsizecod, $fscod );
	$syncframe_size = $num_words_in_syncframe * 2;
	$total_syncframes = $file_size / $syncframe_size;
	$total_seconds = $total_syncframes / 31.25;
	
	print "File: $input_file\n\n";
	print "Synchronization Information:\n";
	print "syncword: 0x$syncword\n";
	print "crc1: 0x$crc1\n";
	print "bitfield: $bitfield\n";
	print "fscod: $fscod\n";
	print "frmsizecod: $frmsizecod\n";
	print "bitrate: $bitrate_in_kbps kbps\n";
	print "sampling rate: $sampling_rate_in_khz kHz\n";
	print "sync frame: $num_words_in_syncframe words\n";
	print "sync frame: $syncframe_size bytes\n";
	print "stream is $total_syncframes frames (31.25 fps) long\n";
	print "stream is $total_seconds seconds long\n\n";
	
	# read Bit Stream Information
	#
	# bsi()
	# {
	# 	bsid																	// 5
	# 	bsmod																	// 3
	# 	acmod																	// 3
	# 	if ((acmod & 0x1)&&(acmod != 0x1)) /* if 3 front channels */ {cmixlev}	// 2
	# 	if (acmod & 0x4) /* if a surround channel exists */ {surmixlev}			// 2
	# 	if (acmod == 0x2 ) /* if in 2/0 mode */ {dsurmod}						// 2
	# 	lfeon																	// 1
	# 	dialnorm																// 5
	# 	compre																	// 1	(running total: 24)
	# 	if (compre) {compr}														// 8
	# 	langcode																// 1
	# 	if (langcode) {langcod}													// 8
	# 	audprodie																// 1	(running total: 42)
	# 	if (audprodie)
	# 	{
	# 		mixlev																// 5
	#		roomtyp																// 2	(running total: 50)
	# 	}
	# 	if (acmod==0)	/* if in 1+1 mode (dual mono, so some items need a second value ) */
	# 	{
	# 		dialnorm2															// 5
	# 		compr2e																// 1
	# 		if (compr2e) {compr2}												// 8
	# 		langcod2e															// 1
	# 		if (langcod2e) {langcod2}											// 8
	# 		audprodi2e															// 1	(running total: 74)
	# 		if (auddprodi2e)
	# 		{
	# 			mixlev2															// 5
	# 			roomtyp2														// 2	(running total: 81)
	# 		}
	#	}
	# 	copyrightb																// 1
	# 	origbs																	// 1
	# 	timecod1e																// 1
	# 	if (timecod1e) {timecod1}												// 14
	# 	timecod2e																// 1
	# 	if (timecod2e) {timecod2}												// 14
	# 	addbsie																	// 1	(running total: 114)
	# 	if (addbsie)
	# 	{
	# 		addbsil																// 6	(running total: 120)
	# 		addbsi																// (addbisil+1)x8	(up to 64)
	# 	}
	# } /* end of bsi */
	#
	
	# bitstream information is not a fixed size
	# maximum possible size is 184 bytes, though chances are it will be much shorter
	# this program is not intended to decode any audio information
	# so reading into the first audio block is not a problem
	# though it would be nice to be able to pull out some of the audio block info such as dynrnge
	# so...
	# we know how big the syncframe is overall
	# and the si chunk was 5 bytes
	# so read syncframe-5 bytes more into $bsi
	$result = read( INPUT_FILE, $bsi, ( $syncframe_size - 5 ) );
	if ( $result == undef ) {
		print "$input_file: error: could not read bsi() $1\n";
		$errors++;
		print "$input_file: aborting with $errors errors and $warnings warnings\n";
		close( INPUT_FILE );
		next FILE;
	}

	
	
	
	# reset pointers
	$byte_pointer = 0;
	$bit_pointer = 0;
	
	# bsid and bsmod are always the first byte
	$bitfield = unpack( "B[8]", substr( $bsi, $byte_pointer, 1 ) );
	$byte_pointer += 1;
	
	$bsid_bitfield = substr( $bitfield, 0, 5 );
	$bsid = binary_to_decimal( $bsid_bitfield );
	$bsmod_bitfield = substr( $bitfield, 5, 3 );
	$bsmod = binary_to_decimal( $bsmod_bitfield );
		
	# bsmod meaning is affected by acmod, which is the next three bits
	# what comes after that can vary based on values of acmod
	# one byte should be enough to cover all the possibilities
	
	# read next byte
	$bitfield = unpack( "B[8]", substr( $bsi, $byte_pointer, 1 ) );
	$byte_pointer += 1;
	
	$acmod_bitfield = substr( $bitfield, $bit_pointer, 3 );
	$acmod = binary_to_decimal( $acmod_bitfield );
	$bit_pointer += 3;
	
	# if the third bit of acmod is '1' but the value of acmod is not 1 (basically if acmod is 3, 5, or 7),
	# we have three front channels, so cmixlev is next two bits
	$cmixlev = "";
	if ( ( $acmod eq 3 ) || ( $acmod eq 5 ) || ( $acmod eq 7 ) ) {
		$cmixlev_bitfield = substr( $bitfield, $bit_pointer, 2 );
		$cmixlev = binary_to_decimal( $cmixlev_bitfield );
		$bit_pointer += 2;
	}
	
	# if the first bit of acmod is '1' (basically if acmod is greater than or equal to 4
	# we have at least one surround channel, so surmixlev is next two bits
	$surmixlev = "";
	if ( $acmod ge 4 ) {
		$surmixlev_bitfield = substr( $bitfield, $bit_pointer, 2 );
		$surmixlev = binary_to_decimal( $surmixlev_bitfield );
		$bit_pointer += 2;
	}
	
	# if acmod equals 2 (2/0 mode), next two bits are dsurmod (Dolby Surround Mode)
	$dsurmod = "";
	if ( $acmod eq 2 ) {
		$dsurmod_bitfield = substr( $bitfield, $bit_pointer, 2 );
		$dsurmod = binary_to_decimal( $dsurmod_bitfield );
		$bit_pointer += 2;
	}
	
	# next bit is lefon (Low Frequency Effects Channel On)
	$lfeon = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer++;
	
	# next four bytes should cover the rest of the the first audio program's bsi
	# so add them to the bitfield
	$bitfield .= unpack( "B[32]", substr( $bsi, $byte_pointer, 4 ) );
	$byte_pointer += 4;
	
	# dialnorm
	$dialnorm_bitfield = substr( $bitfield, $bit_pointer, 5 );
	$dialnorm = binary_to_decimal( $dialnorm_bitfield );
	$bit_pointer += 5;
	
	# Compression Gain
	# next bit is Compression Gain Word Exists
	$compre = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	# if compre is 1, next 8 bits are the compression gain value
	$compr = "";
	if ( $compre ) {
		$compr_bitfield = substr( $bitfield, $bit_pointer, 8 );
		$compr = binary_to_decimal( $compr_bitfield );
		$bit_pointer += 8;
	}
	
	$compr_dB = compr_value( $compr_bitfield );
	# it would be nice to round this to two decimal places
	$compr_dB_rounded = sprintf( "%.2f", $compr_dB );
	
	# Language Code
	
	$langcode = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	$langcod = "";
	if ( $langcode ) {
		$langcod_bitfield = substr( $bitfield, $bit_pointer, 8 );
		$langcod = binary_to_decimal( $langcod_bitfield );
		$bit_pointer += 8;
	}
	
	# Audio Production Information Exists
	$audprodie = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	# Mixing Level / Room Type
	$mixlev = "";
	$roomtyp = "";
	if ( $audprodie ) {
		$mixlev_bitfield = substr( $bitfield, $bit_pointer, 5 );
		$mixlev = binary_to_decimal( $mixlev_bitfield );
		$bit_pointer += 5;
		
		$roomtyp_bitfield = substr( $bitfield, $bit_pointer, 2 );
		$roomtyp = binary_to_decimal( $roomtyp_bitfield );
		$bit_pointer += 2;
	}
	
	# in 1+1 dual mono mode, some items need a second value
	if ( $acmod eq 0 ) {
		
		# dialnorm2
		$dialnorm2_bitfield = substr( $bitfield, $bit_pointer, 5 );
		$dialnorm2 = binary_to_decimal( $dialnorm2_bitfield );
		$bit_pointer += 5;
		
		# compr2e
		$compr2e = substr( $bitfield, $bit_pointer, 1 );
		$bit_pointer += 1;
		
		# compr2
		$compr2 = "";
		if ( $compr2e ) {
			$compr2_bitfield = substr( $bitfield, $bit_pointer, 8 );
			$compr2 = binary_to_decimal( $compr2_bitfield );
			$bit_pointer += 8;
		}

		$compr2_dB = compr_value( $compr_bitfield );
		# it would be nice to round this to two decimal places
		$compr2_dB_rounded = sprintf( "%.2f", $compr_dB );
		
		# langcod2e
		$langcod2e = substr( $bitfield, $bit_pointer, 1 );
		$bit_pointer += 1;
		
		# langcod2
		$langcod2 = "";
		if ( $langcod2e ) {
			$langcod2_bitfield = substr( $bitfield, $bit_pointer, 8 );
			$langcod2 = binary_to_decimal( $langcod2_bitfield );
			$bit_pointer += 8;
		}
		
		# audprodi2e
		$audprodi2e = substr( $bitfield, $bit_pointer, 1 );
		$bit_pointer += 1;
		
		# mixlev2 / roomtyp2
		$mixlev2 = "";
		$roomtyp2 = "";
		if ( $audprodi2e ) {
			$mixlev2_bitfield = substr( $bitfield, $bit_pointer, 5 );
			$mixlev2 = binary_to_decimal( $mixlev2_bitfield );
			$bit_pointer += 5;
			
			$roomtyp2_bitfield = substr( $bitfield, $bit_pointer, 2 );
			$roomtyp2 = binary_to_decimal( $roomtyp2_bitfield );
			$bit_pointer += 2;
		}
	}
	
	#####	the rest of the bsi
	
	# Copyright Bit (copyrightb)
	$copyrightb = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	# Original Bit Stream (origbs)
	$origbs = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	# Timecode First Half Exists (timecod1e)
	$timecod1e = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	# Timecode First Half
	$timecod1_bitfield = substr( $bitfield, $bit_pointer, 14 );
	$bit_pointer += 14;
	
	# Timecode Second Half Exists (timecod2e)
	$timecod2e = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	# Timecode Second Half
	$timecod2_bitfield = substr( $bitfield, $bit_pointer, 14 );
	$bit_pointer += 14;
	
	##### interpret time code info
	# timecod1 5 bits = hour (0..24)
	# timecod1 6 bits = minutes (0..59)
	# timecod1 3 bits = 8 second increments (0, 8, 16, 24, 32, 40, 56)
	# timecod2 3 bits = additional seconds (0..7)
	# timecod2 5 bits = frames (0..29)
	# timecod2 6 bits = 1/64th frame fraction (0..63)
	#####
	
	# Additional Bit Stream Information Exists (addbsie)
	$addbsie = substr( $bitfield, $bit_pointer, 1 );
	$bit_pointer += 1;
	
	# addbsil / addbsi
	if ( $addbsie ) {
		$addbsil_bitfield = substr( $bitfield, $bit_pointer, 6 );
		$addbsil = binary_to_decimal( $addbsil_bitfield );
		$bit_pointer += 6;
		
		$addbsi = substr( $bitfield, $bit_pointer, ( ( $addbsil + 1 ) * 8 ) );
		$bit_pointer += ( ( $addbsil + 1 ) * 8 );
	}
	
	
	### output report	
	
	### Bit Stream Information
	
	print "Bit Stream Information:\n";
	print "Bit Stream Identification (bsid): $bsid_bitfield ($bsid) = " . interpret_bsid( $bsid ) . "\n";
	
	print "Bit Stream Mode (bsmod): $bsmod_bitfield ($bsmod) = " . interpret_bsmod( $bsmod, $acmod ) . "\n";
	print "Audio Coding Mode (acmod): $acmod_bitfield ($acmod) = " . interpret_acmod( $acmod ) . "\n";
	
	print "Center Mix Level (cmixlev): ";
	if ( $cmixlev ne undef ) {
		print " $cmixlev_bitfield ($cmixlev) = " . interpret_cmixlev( $cmixlev ) . "\n";
	} else { print "not stated\n"; }
	
	print "Surround Mix Level (surmixlev): ";
	if ( $surmixlev ne undef ) {
		print " $surmixlev_bitfield ($surmixlev) = " . interpret_surmixlev( $surmixlev ) . "\n";
	} else { print "not stated\n"; }
	
	print "Dolby Surround Mode (dsurmod): ";
	if ( $dsurmod ne undef ) {
		print " $dsurmod_bitfield ($dsurmod) = " . interpret_dsurmod( $dsurmod ) . "\n";
	} else { print "not stated\n"; }
	
	print "LFE Channel On (lefon): $lfeon = ";
	if ( $lfeon ) { print "on\n"; } else { print "off\n"; }
	
	print "\n";
	
	### primary audio stream bsi
	
	print "Primary Audio Stream Information:\n";
	
	print "Dialog Normalizaton (dilanorm): $dialnorm_bitfield ($dialnorm) = " . interpret_dialnorm( $dialnorm ) . "\n";
	
	print "Compression Gain Word Exists (compre): $compre = ";
	if ( $compre ) { print "true\n"; } else { print "false\n"; }
	
	print "Compression Gain Word (compr): ";
	if ( $compre ) { print "$compr_bitfield = $compr_dB_rounded dB\n"; }
	else { print "not stated\n"; }
	
	print "Language Code Exists (langcode): $langcode = ";
	if ( $langcode ) { print "true\n"; } else { print "false\n"; }
	
	print "Language Code (langcod): ";
	if ( $langcode ) {
		print "$langcod_bitfield ($langcod) = " . interpret_langcod( $langcod ) . "\n";
	} else { print "not stated\n"; }
	
	print "Audio Produciton Information Exists (audprodie): $audprodie = ";
	if ( $audprodie ) { print "true\n"; } else { print "false\n"; }
	
	print "Mixing Level (mixlev): ";
	if ( $audprodie ) {
		print "$mixlev_bitfield ($mixlev) = " . interpret_mixlev( $mixlev ) . "\n";
	} else { print "not stated\n"; }
	
	print "Room Type (roomtyp): ";
	if ( $audprodie ) {
		print "$roomtyp_bitfield ($roomtyp) = " . interpret_roomtyp( $roomtyp ) . "\n";
	} else { print "not stated\n"; }
	
	### secondary audio stream BSI for 1+1 streams
	
	print "\n";
	print "Secondary Audio Stream Informaion (1+1 Mode):\n";
	
	print "Dialog Normalizaton (dilanorm2): ";
	if ( $acmod eq 0 ) {
		print "$dialnorm2_bitfield ($dialnorm2) = " . interpret_dialnorm( $dialnorm2 ) . "\n";
	} else { print "not applicable\n"; }
	
	print "Compression Gain Word Exists (compr2e): ";
	if ( $acmod eq 0 ) {
		print "$compr2e = ";
		if ( $compr2e ) { print "true\n"; } else { print "false\n"; }
	} else { print "not applicable\n"; }
	
	print "Compression Gain Word (compr2): ";
	if ( $acmod eq 0 ) {
		if ( $compr2e ) {
			print "$compr2_bitfield = $compr2_dB_rounded dB\n";
		} else { print "not stated\n"; }
	} else { print "not applicable\n"; }
	
	print "Language Code Exists (langcod2e): ";
	if ( $acmod eq 0 ) {
		print "$langcod2e = ";
		if ( $langcod2e ) { print "true\n"; } else { print "false\n"; }
	} else { print "not applicable\n"; }
	
	print "Language Code (langcod2): ";
	if ( $acmod eq 0 ) {
		if ( $langcod2e ) {
			print "$langcod2_bitfield ($langcod2) = " . interpret_langcod( $langcod2 ) . "\n";
		} else { print "not stated\n"; }
	} else { print "not applicable\n"; }
	
	print "Audio Production Information Exists (audprodi2e): ";
	if ( $acmod eq 0 ) {
		print "$audprodi2e = ";
		if ( $audprodi2e ) { print "true\n"; } else { print "false\n"; }
	} else { print "not applicable\n"; }
	
	print "Mixing Level (mixlev2): ";
	if ( $acmod eq 0 ) {
		if ( $audprodi2e ) {
			print "mixlev2_bitfield ($mixlev2) = " . interpret_mixlev( $mixlev2 ) . "\n";
		} else { print "not stated\n"; }
	} else { print "not applicable\n"; }
	
	print "Room Type (roomtyp2): ";
	if ( $acmod eq 0 ) {
		if ( $audprodi2e ) {
			print "$roomtyp2_bitfield ($roomtyp2) = " . interpret_roomtyp( $roomtyp2 ) . "\n";
		} else { print "not stated\n"; }
	} else { print "not applicable\n"; }
	print "\n";

	### remainder of the bsi
	
	print "Copyright Bit (copyrightb): ";
	if ( $copyrightb ) { print "true\n"; } else { print "false\n"; }
	
	print "Original Bit Stream (origbs): ";
	if ( $origbs ) { print "true\n"; } else { print "false\n"; }
	
	print "Timecode First Half Exists (timecod1e): ";
	if ( $timecod1e ) { print "true\n"; } else { print "false\n"; }
	
	print "Timecode First Half (timecod1): ";
	if ( $timecod1e ) {
		print "$timecod1_bitfield\n";
	} else { print "not stated\n"; }
	
	print "Timecode Second Half Exists (timecod2e): ";
	if ( $timecod2e ) { print "true\n"; } else { print "false\n"; }
	
	print "Timecode Second Half (timecod2): ";
	if ( $timecod2e ) {
		print "$timecod2_bitfield\n";
	} else { print "not stated\n"; }
	
	print "Timecode: ";
	if ( $timecod1e || $timecod2e ) {
		print "\n";								##### print the timecode itself #####
	} else { print "not stated\n"; }
	
	print "Additional Bit Stream Information Exists (addbsie): ";
	if ( $addbsie ) { print "true\n"; } else { print "false\n"; }
	
	print "Additional Bit Stream Information Length (addbsil): ";
	if ( $addbsie ) { print "$addbsil_bitfield ($addbsil)\n"; }
	else { print "not applicable\n"; }
	
	print "Additional Bit Stream Information (addbsi): ";
	if ( $addbsie ) { print "$addbsi\n"; } else { print "not applicable\n"; }
	
	# finish up
	print "\n";
	print "Finished with $errors errors and $warnings warnings\n";
	close( INPUT_FILE );
}



# subroutines

#####
#
# getBitrate()
#
# takes value (in decimal form) of frmsizecod
# does lookup in ATSC A/52B Table 5.18
# returns bitrate (in kbps)
#
#####
sub getBitrate {
	my @bitrate_table = (32,32,40,40,48,48,56,56,64,64,80,80,96,96,112,112,128,128,160,160,192,192,224,224,256,256,320,320,384,384,448,448,512,512,576,576,640,640);
	return( $bitrate_table[$_[0]] );
}

#####
#
# getSamplingRate()
#
# takes value (in decimal form) of fscod
# does lookup in ATSC A/52B Table 5.6
# returns sampling rate (in kHz)
#
#####
sub getSamplingRate {
	my @samplingRate_table = (48,44.1,32,0);	#	'0' is really "reserved" in the spec
	return( $samplingRate_table[$_[0]] );
}

#####
#
# evalFrmsizecodValue()
#
# takes value (in decimal form) of frmsizecod and fscod
# does lookup in ATSC A/52B Table 5.18
# returns the number of 2-byte words per syncframe
#
#####
sub evalFrmsizecodValue {
	my $value = $_[0];
	my $samplerate = $_[1];
	my @thirtytwo_kHz_table = (96,96,120,120,144,144,168,168,192,192,240,240,288,288,336,336,384,384,480,480,576,576,672,672,768,768,960,960,1152,1152,1344,1344,1536,1536,1728,1728,1920,1920);
	my @fortyfour_kHz_table = (69,70,87,88,104,105,121,122,139,140,174,175,208,209,243,244,278,279,348,349,417,418,487,488,557,558,696,697,835,836,975,976,1114,1115,1253,1254,1393,1394);
	my @fortyeight_kHz_table = (64,64,80,80,96,96,112,112,128,128,160,160,192,192,224,224,256,256,320,320,384,348,448,448,512,512,640,640,768,768,896,896,1024,1024,1152,1152,1280,1280);
	
	if ( $samplerate eq 0 ) {	# 48 kHz
		return( $fortyeight_kHz_table[$value] );
	} elsif ( $samplerate = 1 ) {	# 44.1 kHz
		return( $fortyfour_kHz_table[$value] );
	} elsif ( $samplerate = 2 ) {	# 32 kHz
		return( $thirtytwo_kHz_table[$value] );	
	} else {	# reserved or bad value
		return 0;
	}
}

#####
#
# binary_to_decimal()
#
# takes bitfield strings (up to 32 bits long) and converts to decimal
#
# this is confusing so let me explain
# "0" x 32 = 00000000000000000000000000000000 ("0" times 32)
# shift grabs the first item of array @_, or the parameter passed to sub
# the . concatenates the 32 "0"s and the bit string passed to sub
# substr( foo, -32 ) returns the last 32 bytes of foo
# all this does is pad our bit string with leading zeros to make 32 bits
# pack "B32" converts a 32 bit field into a value
# unpack "N" converts that value into a scalar
#
#####
sub binary_to_decimal {
	return unpack( "N", pack( "B32", substr( "0" x 32 . shift, -32 ) ) );
}

#####
#
# interpret_bsid()
#
# bsid traditionally was only '01000' (8)
# but things have changed
#
# a standard AC-3 decoder should be able to decode anything with a bsid from 0..8
# a bsid of 6 is the Alternative Bitstream syntax (Annex D)
# a compliant standard AC-3 decoder shall mute anything with a bsid greater than 8
# a compliant Enhanced AC-3 (Annex E) decoder should be able to decode anything with a bsid of 0..8 and 11..16
# a compliant E-AC-3 decoder shall mute anything with a bsid of 9 or 10
# a compliant E-AC-3 decoder shall mute anything with a bsid greater than 16
#
#####
sub interpret_bsid {
	my $value = shift;
	if ( $value eq 8 ) { return "Standard AC-3 stream"; }
	if ( $value eq 6 ) { return "AC-3 with Alternative Bit Stream Syntax (Annex D)"; }
	if ( ( $value ge 0 ) && ( $value le 8 ) ) { return "Should be decodable by compliant AC-3/E-AC-3 decoder"; }
	if ( ( $value eq 9 ) || ( $value eq 10 ) ) {
		return "Non-standard stream - shall mute on compliant AC-3 and Enhanced AC-3 (Annex E) decoders";
		$warnings++;
	}
	if ( $value eq 16 ) { return "Standard Enhanced AC-3 (Annex E) stream"; }
	if ( ( $value ge 11 ) && ( $value le 15 ) ) {
		return "Backwards-compatible Enhanced AC-3 (Annex E) stream - should be decodable by E-AC-3 decoder, shall mute on a compliant AC-3 decoder";
	}
	if ( $value gt 16 ) {
		return "Shall mute on compliant AC-3 and Enhanced AC-3 (Annex E) decoders";
		$warnings++;
	}
	
	return( $value );
}

#####
#
# interpret_bsmod()
#
# bsmod		acmod		Type Of Service
# '000'		any			main audio service: complete main (CM)
# '001'		any			main audio service: music and effects (ME)
# '010'		any			associated service: visually impaired (VI)
# '011'		any			associated service: hearing impaired (HI)
# '100'		any			associated service: dialogue (D)
# '101'		any			associated service: commentary (C)
# '110'		any			associated service: emergency (E)
# '111'		'001'		associated service: voice over (VO)
# '111'		'010'-'111'	main audio service: karaoke
#
#####
sub interpret_bsmod {
	my $bsmod_value = shift;
	my $acmod_value = shift;
	if ( $bsmod_value eq 0 ) { return "main audio service: complete main (CM)"; }
	if ( $bsmod_value eq 1 ) { return "main audio service: music and effects (ME)"; }
	if ( $bsmod_value eq 2 ) { return "associated service: visually impaired (VI)"; }
	if ( $bsmod_value eq 3 ) { return "associated service: hearing impaired (HI)"; }
	if ( $bsmod_value eq 4 ) { return "associated service: dialogue (D)"; }
	if ( $bsmod_value eq 5 ) { return "associated service: commentary (C)"; }
	if ( $bsmod_value eq 6 ) { return "associated service: emergency (E)"; }
	if ( $bsmod_value eq 7 ) {
		if ( $acmod_value eq 1 ) { return "associated service: voice over (VO)"; }
		if ( ( $acmod_value ge 2 ) && ( $acmod_value le 7 ) ) {
			return "main audio service: karaoke";
		}
	}
	return "invalid";
}

#####
#
# interpret_acmod()
# 
# interpret Audio Coding Mode (acomd)
#
# acmod		Audio Coding Mode		nfchans		Channel Array Ordering
# '000'		1+1						2			Ch1, Ch2
# '001'		1/0						1			C
# '010'		2/0						2			L, R
# '011'		3/0						3			L, C, R
# '100'		2/1						3			L, R, S
# '101'		3/1						4			L, C, R, S
# '110'		2/2						4			L, R, SL, SR
# '111		3/2						5			L, C, R, SL, SR
#
#####
sub interpret_acmod {
	my $acmod_value = shift;
	if ( $acmod_value eq 0 ) { return "1+1: Ch1, Ch2"; }
	if ( $acmod_value eq 1 ) { return "1/0: C"; }
	if ( $acmod_value eq 2 ) { return "2/0: L, R"; }
	if ( $acmod_value eq 3 ) { return "3/0: L, C, R"; }
	if ( $acmod_value eq 4 ) { return "2/1: L, R, S"; }
	if ( $acmod_value eq 5 ) { return "3/1: L, C, R, S"; }
	if ( $acmod_value eq 6 ) { return "2/2: L, R, SL, SR"; }
	if ( $acmod_value eq 7 ) { return "3/2: L, C, R, SL, SR"; }
	return "invalid";
}

#####
#
# interpret_cmixlev()
#
# interpret Center Mix Level (cmixlev)
#
# cmixlev
# '00' (0) = 0.707 (-3.0 dB)
# '01' (1) = 0.595 (-4.5 dB)
# '10' (2) = 0.500 (-6.0 dB)
# '11' (3) = reserved
#
#####
sub interpret_cmixlev {
	my $value = shift;
	if ( $value eq 0 ) { return "0.707 (-3.0 dB)"; }
	if ( $value eq 1 ) { return "0.595 (-4.5 dB)"; }
	if ( $value eq 2 ) { return "0.500 (-6.0 dB)"; }
	if ( $value eq 3 ) { return "reserved"; }
}

#####
#
# interpret_surmixlev()
#
# interpret Surround Mix Level (surmixlev)
#
# surmixlev		slev
# '00'			0.707 (-3.0 dB)
# '01'			0.500 (-6.0 dB)
# '10'			0
# '11'			rserved
#
#####
sub interpret_surmixlev {
	my $surmixlev_value = shift;
	if ( $surmixlev_value eq 0 ) { return "0.707 (-3.0 dB)"; }
	if ( $surmixlev_value eq 1 ) { return "0.500 (-6.0 dB)"; }
	if ( $surmixlev_value eq 2 ) { return "0"; }
	if ( $surmixlev_value eq 3 ) { return "reserved"; }	
}

#####
#
# interpret_dsurmod()
#
# interpret Dolby Surround Mode (dsmod)
# 
# dsmod		Indication
# '00'		not indicated
# '01'		Not Dolby Surround encoded
# '10'		Dolby Surround encoded
# '11'		reserved
#
#####
sub interpret_dsurmod {
	my $dsurmod_value = shift;
	if ( $dsurmod_value eq 0 ) { return "not indicated"; }
	if ( $dsurmod_value eq 1 ) { return "Not Dolby Surround encoded"; }
	if ( $dsurmod_value eq 2 ) { return "Dolby Surround encoded"; }
	if ( $dsurmod_value eq 3 ) { return "reserved"; }
}

#####
#
# interpret dialnorm()
#
# interpret Dialog Normalization (dialnorm)
#
# dialnorm 1..31 is -1 to -31 dB
# dialnorm of 0 is reserved, but shall be -31 dB
#
#####
sub interpret_dialnorm {
	my $dialnorm_value = shift;
	if ( $dialnorm_value eq 0 ) { return "reserved (-31 dB)"; }
	else { return "-" . $dialnorm_value . " dB"; }
}

#####
#
# compr_value()
#
# take compression gain word bitfield
#
# compression gain word is 8 bits
# XXXXYYYY
# XXXX is a signed 4 bit integer (call this x)
# (x+1)*6.02 dB is a starting value that can be -42.14 dB to +48.16
#
# YYYY is an unsigned integer (call this y)
# 0.1YYYY (base 2), which can represent values of -0.28 dB to -6.02 dB
# this is added to base x value, so the gain change can be from -48.16 dB to +47.89 dB
#
# I don't fully understand the 0.1YYYY bit yet
# 
# 
#
#####
sub compr_value {
	my @signed_nibble_table = (0,1,2,3,4,5,6,7,-8,-7,-6,-5,-4,-3,-2,-1);
	
	my $compr_word_bitfield = shift;
	my $X_bitfield = substr( $compr_word_bitfield, 0, 4 );
	my $Y_bitfield = substr( $compr_word_bitfield, 4, 4 );
	
	my $X_byte = "0000" . $X_bitfield;					# pad 4 msb with 0
	my $X_value = binary_to_decimal( $X_bitfield );		# convert to an unsigned int value
	
	my $Y_value = 1 * ( 2 ** -1 );
	$Y_value += substr( $Y_bitfield, 0, 1 ) * ( 2 ** -2 );
	$Y_value += substr( $Y_bitfield, 1, 1 ) * ( 2 ** -3 );
	$Y_value += substr( $Y_bitfield, 2, 1 ) * ( 2 ** -4 );
	$Y_value += substr( $Y_bitfield, 3, 1 ) * ( 2 ** -5 );
	
	my $Y_gain_value = 20 * log( $Y_value ) / log( 10 );
	
	my $gain_value = ( ( @signed_nibble_table[$X_value] + 1 ) * 6.02 ) + ( 20 * log( $Y_value ) / log( 10 ) );
	
	return( $gain_value );
}

#####
# 
# interpret_langcod()
#
# interpret Language Code (langocd)
#
# OK, so this field was depreciated at some point before A/52:2012
# in favor of ISO-639 language code in the wrapper
# the current A/52 spec says this should be 0xFF
# but in most cases, the value will be 0x9, English
#
# oanguage list take from my C++ program, which referenced an older version of the spec
#
#####
sub interpret_langcod {
	my $langcod_value = shift;
	if ( $langcod_value eq 255 ) { return "reserved (default value)"; }
	if ( $langcod_value eq 0 ) { return "unknown/NA"; }
	if ( $langcod_value eq 1 ) { return "Albanian"; }
	if ( $langcod_value eq 2 ) { return "Breton"; }
	if ( $langcod_value eq 3 ) { return "Catalan"; }
	if ( $langcod_value eq 4 ) { return "Croatian"; }
	if ( $langcod_value eq 5 ) { return "Welsh"; }
	if ( $langcod_value eq 6 ) { return "Czech"; }
	if ( $langcod_value eq 7 ) { return "Danish"; }
	if ( $langcod_value eq 8 ) { return "German"; }
	if ( $langcod_value eq 9 ) { return "English"; }
	if ( $langcod_value eq 10 ) { return "Spanish"; }
	if ( $langcod_value eq 11 ) { return "Esperanto"; }
	if ( $langcod_value eq 12 ) { return "Estonian"; }
	if ( $langcod_value eq 13 ) { return "Basque"; }
	if ( $langcod_value eq 14 ) { return "Faroese"; }
	if ( $langcod_value eq 15 ) { return "French"; }
	if ( $langcod_value eq 16 ) { return "Frisian"; }
	if ( $langcod_value eq 17 ) { return "Irish"; }
	if ( $langcod_value eq 18 ) { return "Gaelic"; }
	if ( $langcod_value eq 19 ) { return "Galician"; }
	if ( $langcod_value eq 20 ) { return "Icelandic"; }
	if ( $langcod_value eq 21 ) { return "Italian"; }
	if ( $langcod_value eq 22 ) { return "Lappish"; }
	if ( $langcod_value eq 23 ) { return "Latin"; }
	if ( $langcod_value eq 24 ) { return "Latvian"; }
	if ( $langcod_value eq 25 ) { return "Luxembourgian"; }
	if ( $langcod_value eq 26 ) { return "Lithuanian"; }
	if ( $langcod_value eq 27 ) { return "Hungarian"; }
	if ( $langcod_value eq 28 ) { return "Maltese"; }
	if ( $langcod_value eq 29 ) { return "Dutch"; }
	if ( $langcod_value eq 30 ) { return "Norwegian"; }
	if ( $langcod_value eq 31 ) { return "Occitan"; }
	if ( $langcod_value eq 32 ) { return "Polish"; }
	if ( $langcod_value eq 33 ) { return "Portuguese"; }
	if ( $langcod_value eq 34 ) { return "Romanian"; }
	if ( $langcod_value eq 35 ) { return "Romanish"; }
	if ( $langcod_value eq 36 ) { return "Serbian"; }
	if ( $langcod_value eq 37 ) { return "Slovak"; }
	if ( $langcod_value eq 38 ) { return "Slovene"; }
	if ( $langcod_value eq 39 ) { return "Finnish"; }
	if ( $langcod_value eq 40 ) { return "Swedish"; }
	if ( $langcod_value eq 41 ) { return "Turkish"; }
	if ( $langcod_value eq 42 ) { return "Flemish"; }
	if ( $langcod_value eq 43 ) { return "Waloon"; }
	if ( $langcod_value eq 44 ) { return "undefined"; }
	if ( $langcod_value eq 45 ) { return "undefined"; }
	if ( $langcod_value eq 46 ) { return "undefined"; }
	if ( $langcod_value eq 47 ) { return "undefined"; }
	if ( $langcod_value eq 48 ) { return "reserved"; }
	if ( $langcod_value eq 49 ) { return "reserved"; }
	if ( $langcod_value eq 50 ) { return "reserved"; }
	if ( $langcod_value eq 51 ) { return "reserved"; }
	if ( $langcod_value eq 52 ) { return "reserved"; }
	if ( $langcod_value eq 53 ) { return "reserved"; }
	if ( $langcod_value eq 54 ) { return "reserved"; }
	if ( $langcod_value eq 55 ) { return "reserved"; }
	if ( $langcod_value eq 56 ) { return "reserved"; }
	if ( $langcod_value eq 57 ) { return "reserved"; }
	if ( $langcod_value eq 58 ) { return "reserved"; }
	if ( $langcod_value eq 59 ) { return "reserved"; }
	if ( $langcod_value eq 60 ) { return "reserved"; }
	if ( $langcod_value eq 61 ) { return "reserved"; }
	if ( $langcod_value eq 62 ) { return "reserved"; }
	if ( $langcod_value eq 63 ) { return "reserved"; }
	if ( $langcod_value eq 64 ) { return "bg sound"; }
	if ( $langcod_value eq 65 ) { return "unknown"; }
	if ( $langcod_value eq 66 ) { return "unknown"; }
	if ( $langcod_value eq 67 ) { return "unknown"; }
	if ( $langcod_value eq 68 ) { return "unknown"; }
	if ( $langcod_value eq 69 ) { return "Zulu"; }
	if ( $langcod_value eq 70 ) { return "Vietnamese"; }
	if ( $langcod_value eq 71 ) { return "Uzbek"; }
	if ( $langcod_value eq 72 ) { return "Urdu"; }
	if ( $langcod_value eq 73 ) { return "Ukrainian"; }
	if ( $langcod_value eq 74 ) { return "Thai"; }
	if ( $langcod_value eq 75 ) { return "Telugu"; }
	if ( $langcod_value eq 76 ) { return "Tatar"; }
	if ( $langcod_value eq 77 ) { return "Tamil"; }
	if ( $langcod_value eq 78 ) { return "Tadzhik"; }
	if ( $langcod_value eq 79 ) { return "Swahili"; }
	if ( $langcod_value eq 80 ) { return "Sranan Tongo"; }
	if ( $langcod_value eq 81 ) { return "Somali"; }
	if ( $langcod_value eq 82 ) { return "Sinhalese"; }
	if ( $langcod_value eq 83 ) { return "Shona"; }
	if ( $langcod_value eq 84 ) { return "Serbo-Croat"; }
	if ( $langcod_value eq 85 ) { return "Ruthenian"; }
	if ( $langcod_value eq 86 ) { return "Russian"; }
	if ( $langcod_value eq 87 ) { return "Quechua"; }
	if ( $langcod_value eq 88 ) { return "Pustu"; }
	if ( $langcod_value eq 89 ) { return "Punjabi"; }
	if ( $langcod_value eq 90 ) { return "Persian"; }
	if ( $langcod_value eq 91 ) { return "Papamiento"; }
	if ( $langcod_value eq 92 ) { return "Oriya"; }
	if ( $langcod_value eq 93 ) { return "Nepali"; }
	if ( $langcod_value eq 94 ) { return "Ndebele"; }
	if ( $langcod_value eq 95 ) { return "Marathi"; }
	if ( $langcod_value eq 96 ) { return "Moldavian"; }	
	if ( $langcod_value eq 97 ) { return "Malaysian"; }
	if ( $langcod_value eq 98 ) { return "Malagasay"; }
	if ( $langcod_value eq 99 ) { return "Macedonian"; }
	if ( $langcod_value eq 100 ) { return "Laotian"; }
	if ( $langcod_value eq 101 ) { return "Korean"; }
	if ( $langcod_value eq 102 ) { return "Khmer"; }
	if ( $langcod_value eq 103 ) { return "Kazakh"; }
	if ( $langcod_value eq 104 ) { return "Kannada"; }
	if ( $langcod_value eq 105 ) { return "Japanese"; }
	if ( $langcod_value eq 106 ) { return "Indonesian"; }
	if ( $langcod_value eq 107 ) { return "Hindi"; }
	if ( $langcod_value eq 108 ) { return "Hebrew"; }
	if ( $langcod_value eq 109 ) { return "Hausa"; }
	if ( $langcod_value eq 110 ) { return "Gurani"; }
	if ( $langcod_value eq 111 ) { return "Gujurati"; }
	if ( $langcod_value eq 112 ) { return "Greek"; }
	if ( $langcod_value eq 113 ) { return "Georgian"; }
	if ( $langcod_value eq 114 ) { return "Fulani"; }
	if ( $langcod_value eq 115 ) { return "Dari"; }
	if ( $langcod_value eq 116 ) { return "Churash"; }
	if ( $langcod_value eq 117 ) { return "Chinese"; }
	if ( $langcod_value eq 118 ) { return "Burmese"; }
	if ( $langcod_value eq 119 ) { return "Bulgarian"; }
	if ( $langcod_value eq 120 ) { return "Bengali"; }
	if ( $langcod_value eq 121 ) { return "Belorussian"; }
	if ( $langcod_value eq 122 ) { return "Bambora"; }
	if ( $langcod_value eq 123 ) { return "Azerbijani"; }
	if ( $langcod_value eq 124 ) { return "Assamese"; }
	if ( $langcod_value eq 125 ) { return "Armenian"; }
	if ( $langcod_value eq 126 ) { return "Arabic"; }
	if ( $langcod_value eq 127 ) { return "Amharic"; }
	return( "not recognized" );
}

#####
#
# interpret_mixlev()
#
# interpret Mixing Level (mixlev)
#
# This 5-bit code indicates the absolute acoustic sound pressure level of an individual
# channel during the final audio mixing session. The 5-bit code represents a value in the
# range 0 to 31. The peak mixing level is 80 plus the value of mixlevel dB SPL, or
# 80 to 111 dB SPL. The peak mixing level is the acoustic level of a sine wave in a single
# channel whose peaks reach 100 percent in the PCM representation. The absolute SPL value
# is typically measured by means of pink noise with an RMS value of -20 or -30 dB with
# respect to the peak RMS sine wave level. The value of mixlevel is not typically used
# within the AC-3 decoder, but may be used by other parts of the audio reproduction equipment.
#
#####
sub interpret_mixlev {
	my $mixlev_value = shift;
	return( $mixlev_value + 80 );
}

#####
#
# interpret_roomtyp()
#
# interpret Room Type (roomtyp)
#
# roomtyp		Type of Mixing Room
# '00'			not indicated
# '01'			large room, X curve monitor
# '10'			small room, flat monitor
# '11'			reserved
#
#####
sub interpret_roomtyp {
	my $roomtyp_value = shift;
	if ( $roomtyp_value eq 0 ) { return "not indicated"; }
	if ( $roomtyp_value eq 1 ) { return "large room, X curve monitor"; }
	if ( $roomtyp_value eq 2 ) { return "small room, flat monitor"; }
	if ( $roomtyp_value eq 3 ) { return "reserved"; }
}

#####
#
# decode_timecode()
#
# take timecod1 and timecod2 and turn them into a formatted timecode value
# HH:MM:SS:FF+ff
# (+ff = 1/64th frame fraction)
#
# 	timecod1
# 	00 01 02 03 04 05 06 07 08 09 10 11 12 13
# 	HH HH HH HH HH MM MM MM MM MM MM SS SS SS
#
# 	timecod2
# 	00 01 02 03 04 05 06 07 08 09 10 11 12 13
# 	SS SS SS FF FF FF FF FF ff ff ff ff ff ff
#
# 	timecod1 5 bits = hour (0..24)
# 	timecod1 6 bits = minutes (0..59)
# 	timecod1 3 bits = 8 second increments (0, 8, 16, 24, 32, 40, 56)
# 	timecod2 3 bits = additional seconds (0..7)
# 	timecod2 5 bits = frames (0..29)
# 	timecod2 6 bits = 1/64th frame fraction (0..63)
#
# both bitfields are NOT necessary to decode a timecode that is usable
# how do handle this?
#
# I can't 
#
#####
sub decode_timecode {
	my $timecod1_value = shift;
	my $timecod2_value = shift;
	
	my ( $output, $hours, $minutes, $seconds, $frames, $fraction );
	my ( $hour_bits, $minute_bits, $big_second_bits );
	my ( $little_second_bits, $frame_bits, $fraction_bits );
	
	$hour_bits = substr( $timecod1_value, 0, 5 );
	$minute_bits = substr( $timecod1_value, 5, 6 );
	$big_second_bits = substr( $timecod1_value, 11, 3 );
	$little_second_bits = substr( $timecod2_value, 0, 3 );
	$frame_bits = substr( $timecod2_value, 3, 5 );
	$fraction_bits = substr( $timecod2_value, 8, 6 );
	
	$hours = sprintf( "%02d", binary_to_decimal( $hour_bits ) );
	$minutes = sprintf( "%02d", binary_to_decimal( $minute_bits ) );
	$seconds = sprintf( "%02d", ( binary_to_decimal( $big_second_bits) * 8 ) + binary_to_decimal( $little_second_bits ) );
	$frames = sprintf( "%02d", binary_to_decimal( $frame_bits ) );
	$fraction = sprintf( "%02d", binary_to_decimal( $fraction_bits ) );
	
	$output = "$hours:$minutes:$seconds:$frames+$fraction";
	
	return $output;
}
