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

# 	consider output to XML file, like the WAV file parser

# variables/constants

my ( $input_file, $errors, $warnings, $extension, $result, $file_size );
my ( $byte, $bitfield );
my ( $syncinfo, $syncword, $crc1, $fscod, $frmsizecod );
my ( $bitrate_in_kbps, $sampling_rate_in_khz, $num_words_in_syncframe );
my $syncframe_size;
my $total_syncframes;
my $total_seconds;


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
	my @samplingRate_table = (48,44.1,32,0);
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
	
	# read syncinfo()
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
	# fscod and frmsizecod are a packed bitfield
	
	$bitfield = unpack( "B[8]", substr( $syncinfo, 4, 1 ) );
	$fscod = binary_to_decimal( substr( $bitfield, 0, 2 ) );
	$frmsizecod = binary_to_decimal( substr( $bitfield, 2, 6 ) );
	
	$bitrate_in_kbps = getBitrate( $frmsizecod );
	$sampling_rate_in_khz = getSamplingRate( $fscod );
	
	$num_words_in_syncframe = evalFrmsizecodValue( $frmsizecod, $fscod );
	$syncframe_size = $num_words_in_syncframe * 2;
	$total_syncframes = $file_size / $syncframe_size;
	$total_seconds = $total_syncframes / 31.25;
	
	print "syncword: 0x$syncword\n";
	print "crc1: 0x$crc1\n";
#	print "bitfield: $bitfield\n";
	print "fscod: $fscod\n";
	print "frmsizecod: $frmsizecod\n";
	print "bitrate: $bitrate_in_kbps kbps\n";
	print "sampling rate: $sampling_rate_in_khz kHz\n";
	print "sync frame: $num_words_in_syncframe words\n";
	print "sync frame: $syncframe_size bytes\n";
	print "stream is $total_syncframes frames (31.25 fps) long\n";
	print "stream is $total_seconds seconds long\n";
	
	## start reading through the bitstream information
	## 	not a fixed size, so read 1 or 2 bytes at a time
	## parse through all that data
	
	
	
	
	# finish up
	print "$input_file: finished with $errors errors and $warnings warnings\n";
	close( INPUT_FILE );
}


exit;











