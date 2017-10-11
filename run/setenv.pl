#!/usr/bin/env perl
#
# Common configuration variables and sanity checks.
# Run this to check and fix your environment; when no more error messages are printed, 
# run the run-pairwise-matching.pl and run-retrieval scripts (in this order).
#


# OS check
$isWindows = ("$^O" eq "MSWin32");

# set your base directory where the dataset is stored:
$BASEDIR = $isWindows ? "Z:\\CDVA" : "/home/massimo/CDVA";

# set your workspace directory (where output files are stored)
$WORKSPACE = "workspace";

# output options: -h (html), -c (csv), -t (text). Can be combined, e.g. "-c -t"
$OUTFLAG = "-c -w $WORKSPACE -t";

# set and check all annotation files
$QUERIES	= $BASEDIR."/Queries.txt";     
$REFERENCES	= $BASEDIR."/References.txt";
$MATCHING	= $BASEDIR."/MatchingPairs.txt";
$NONMATCHING    = $BASEDIR."/NonMatchingPairs.txt";
$DATABASE 	= $BASEDIR."/Database.txt";
$RETRIEVAL 	= $BASEDIR."/Retrieval.txt";

-r $QUERIES  	or die "$QUERIES not found.\n";
-r $REFERENCES  or die "$REFERENCES not found.\n";
-r $MATCHING  	or die "$MATCHING not found.\n";
-r $NONMATCHING or die "$NONMATCHING not found.\n";
-r $DATABASE  	or die "$DATABASE not found.\n";
-r $RETRIEVAL  	or die "$RETRIEVAL not found.\n";

if (-d "$WORKSPACE") {
    print("workspace exists; using $WORKSPACE \n");
} else {
	if ($isWindows) {
			system("robocopy $BASEDIR $WORKSPACE /e /xf *");
	   }
	   else {
			system("rsync -a -f'+ */' -f'- *' $BASEDIR/ $WORKSPACE/");
	}

}

# module has to return a value:
1;
