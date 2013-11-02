#!/usr/bin/perl
use strict;	# Enforce some good programming rules

#####
#
# 	parseAC3.pl
# 	version:	0.9
# 	created:	2011-03-31
# 	modified:	2011-03-31
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

# variables/constants

my ( $input_file, $errors, $warnings, $extension, $result, $file_size );
my ( $byte, $bitfield );
my ( $syncinfo, $syncword, $crc1, $fscod, $frmsizecod );
my ( $bitrate_in_kbps, $sampling_rate_in_khz, $num_words_in_syncframe );
my $syncframe_size;
my $total_syncframes;
my $total_seconds;
my $bsi;
my ( $byte_pointer, $bit_pointer );


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
	#		roomtyp																// 3	(running total: 50)
	# 	}
	# 	if (acmod==0)	/* if in 1+1 mode (dual mono, so some items need a second value ) */
	# 	{
	# 		dialnorm2															// 5
	# 		compr2e																// 1
	# 		if (compr2e) {compr2}												// 8
	# 		langcod2															// 1
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
	# 	addbside																// 1	(running total: 114)
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

	
	my ( $bsid, $bsmod, $acmod, $cmixlev, $surmixlev, $dsurmod, $lfeon );
	my ( $bsid_bitfield, $bsmod_bitfield, $acmod_bitfield );
	my ( $cmixlev_bitfield, $surmixlev_bitfield, $dsurmod_bitfield );
	
	
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
	
	
	
	### OK, we're now in a bit of a pickle
	### if we're surround mode, we're used up all the bits of the byte
	### if we're not, we have bits to process yet in the existing byte
	###
	### now that I've got the bit_pointer in place, we can just read more of the file
	### append it on to the end of bitfield and keep parsing through
	###
	###
	
	
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
	
	print "lefon: $lfeon = ";
	if ( $lfeon ) { print "on\n"; } else { print "off\n"; }
	
	
	#####
	
	
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
# interpret Dolby Surround mode (dsmod)
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







