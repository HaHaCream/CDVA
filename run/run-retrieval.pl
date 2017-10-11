#!/usr/bin/env perl
#
# either execute run-pairwise-matching.pl before running this script,
# or run cdva extract $BASEDIR/Queries for all bitrates (16,64,256). 
#
require "setenv.pl";
require "concurrency.pl";
sub syncrun;
sub asyncrun;
sub waitAll;

#
# N.B.: Database.txt contains References.txt + Distractors.txt
#
print "Extract running on Database.txt;\n";
syncrun "cdva extract $DATABASE 0 $OUTFLAG";

print "Retrieval running;\n";
syncrun "cdva retrieve $RETRIEVAL  16 $DATABASE 0 $OUTFLAG";
syncrun "cdva retrieve $RETRIEVAL  64 $DATABASE 0 $OUTFLAG";
syncrun "cdva retrieve $RETRIEVAL 256 $DATABASE 0 $OUTFLAG";

print "Done.\n";
