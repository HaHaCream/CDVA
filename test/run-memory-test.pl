#!/usr/bin/env perl
#
# *** you must install timer before running this script ***
# on Ubuntu 15.10:
#	sudo apt-get install time
#
require "setenv.pl";

$TESTOUTPUT="memory-test.log";

print "Running memory test, please wait...\n";

#on Windows, we have no "scriptable" memory test so far...
if ($isWindows) {
	print("No script for memory test available on Windows.\n");
}

#on Linux, use time:
if ($isLinux) {
	system("/usr/bin/time -v -o $TESTOUTPUT -p $CDVA extract $EXTRACTING 256 -n 1 -t -w $WORKSPACE ");
	system("/usr/bin/time -v -a -o $TESTOUTPUT -p $CDVA match $MATCHING 256 256 -n 1 -t -w $WORKSPACE ");
	system("/usr/bin/time -v -a -o $TESTOUTPUT -p $CDVA match $NONMATCHING 256 256 -n 1 -t -w $WORKSPACE ");
	system("/usr/bin/time -v -a -o $TESTOUTPUT -p $CDVA retrieve $RETRIEVAL 256 $DATABASE 0 -n 1 -t -w $WORKSPACE ");
}

print "Done.\n";
