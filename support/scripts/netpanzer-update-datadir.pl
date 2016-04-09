#! /usr/bin/env perl

if ( $#ARGV < 0 )
{
	print "Error: needs file parameter\n";
	exit(1);
}

open NPFILE, "+<", $ARGV[0] or die $!; 
binmode NPFILE;
seek NPFILE, -256, 2;

my $datadir;
read NPFILE, $datadir, 256;

if ($datadir =~ /^datadir=/)
{
	$datadir =~ s/\s+$//;
	print "Found datadir: $datadir\n";
	seek NPFILE, -256, 2;
}
else
{
	print "datadir NOT FOUND\n";
}

if ( $#ARGV > 0 )
{
	print "Setting datadir to $ARGV[1]\n";
	printf NPFILE "datadir=%-248s", $ARGV[1];
}
else
{
	print "no change done\n";
}

close NPFILE;

