#!/usr/bin/env perl
#
# verify MD5sum of all files in the dataset;
# you must have md5deep installed on you system)
# (on Ubuntu: "apt-get install md5deep")
# (on Windows: see http://md5deep.sourceforge.net/)
#
use Cwd;
require "setenv.pl";

my $MYDIR = getcwd();
chdir $BASEDIR or die "Can't cd to $BASEDIR\n";
system("$MD5DEEP -c $MYDIR/md5sum.txt ")== 0
	or die "$MD5DEEP failed to run: $?";

print "Done.\n";
