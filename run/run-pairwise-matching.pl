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
require "concurrency.pl";
sub syncrun;
sub asyncrun;
sub waitAll;

print "Running extraction;\n";

syncrun "cdva extract $QUERIES  16 $OUTFLAG";
syncrun "cdva extract $QUERIES  64 $OUTFLAG";
syncrun "cdva extract $QUERIES 256 $OUTFLAG";

syncrun "cdva extract $REFERENCES  16 $OUTFLAG";
syncrun "cdva extract $REFERENCES  64 $OUTFLAG";
syncrun "cdva extract $REFERENCES 256 $OUTFLAG";

#waitAll;

#
# run matching pairs
#
print "Running matching pairs;\n";

asyncrun "cdva match $MATCHING  16  16 $OUTFLAG";
asyncrun "cdva match $MATCHING  64  64 $OUTFLAG";
asyncrun "cdva match $MATCHING 256 256 $OUTFLAG";
asyncrun "cdva match $MATCHING  16 256 $OUTFLAG";
#
# run non-matching pairs
#
print "Running non matching pairs;\n";

asyncrun "cdva match $NONMATCHING  16  16 $OUTFLAG";
asyncrun "cdva match $NONMATCHING  64  64 $OUTFLAG";
asyncrun "cdva match $NONMATCHING 256 256 $OUTFLAG";
asyncrun "cdva match $NONMATCHING  16 256 $OUTFLAG";

waitAll;

print "Done.\n";

