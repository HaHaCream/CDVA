#!/usr/bin/env perl
#
# install cdva before running this script:
# - get cdva from subversion repository;
# - run autogen.sh
# - ./configure
# - make
# - sudo make install
#
require "setenv.pl";

$TESTOUTPUT="speed-test.log";
$ERROUTPUT="error.log";

print "Running speed test:\n";

#on Windows, only errors are printed into stderr;
if ($isWindows) {
	system("$TIMER $TIMEROPTIONS $CDVA extract $EXTRACTING 256 -w $WORKSPACE 1> $TESTOUTPUT 2> $ERROUTPUT");
	system("$TIMER $TIMEROPTIONS $CDVA match $MATCHING  256  256 -w $WORKSPACE 1>> $TESTOUTPUT 2>> $ERROUTPUT");
	system("$TIMER $TIMEROPTIONS $CDVA match $NONMATCHING 256 256 -w $WORKSPACE 1>> $TESTOUTPUT 2>> $ERROUTPUT");
	system("$TIMER $TIMEROPTIONS $CDVA retrieve $RETRIEVAL 256 $DATABASE 0 -w $WORKSPACE 1>> $TESTOUTPUT 2>> $ERROUTPUT");
}

#on Linux, the time measures are printed into stderr;
if ($isLinux) {
	system("$TIMER $TIMEROPTIONS $CDVA extract $EXTRACTING 256 -t -w $WORKSPACE 2> $TESTOUTPUT");
	system("$TIMER $TIMEROPTIONS $CDVA match $MATCHING  256  256 -t -w $WORKSPACE 2>> $TESTOUTPUT");
	system("$TIMER $TIMEROPTIONS $CDVA match $NONMATCHING 256 256 -t -w $WORKSPACE 2>> $TESTOUTPUT");
	system("$TIMER $TIMEROPTIONS $CDVA retrieve $RETRIEVAL 256 $DATABASE 0 -w $WORKSPACE 2>> $TESTOUTPUT");
}

print "Done.\n";
