#!/usr/bin/env perl
#
# Common configuration variables and sanity checks.
# Run this to check and fix your environment; when no more error messages are printed, 
# run run-speed-test.pl (or compute-md5sum.pl/verify-md5sum.pl)

# OS check
$isWindows = ("$^O" eq "MSWin32");
$isLinux   = ("$^O" eq "linux");

if ($isWindows) {
	# set your base directory where the dataset is stored:
	$BASEDIR = "Z:\\CDVA";
	#
	# set the following variables to the correct values;
	# set CDVA to a release cdva executable;
	# set CDVADEBUG to a debug cdva executable; 
	#
	$CDVA="..\\build\\win\\cdva\\x64\\Release\\cdva.exe";
	$CDVADEBUG="..\\build\\win\\cdva\\x64\\Debug\\cdva.exe";
	$WORKSPACE="..\\run\\workspace";
	$EXTRACTING="$BASEDIR\\TimingExtract.txt";
	$MATCHING="$BASEDIR\\TimingMatchingPairs.txt";
	$NONMATCHING="$BASEDIR\\TimingNonMatchingPairs.txt";
	$RETRIEVAL="$BASEDIR\\TimingRetrieval.txt";
	$DATABASE="$BASEDIR\\Database.txt";
	$TIMER="C:\\bin\\ptime.exe";
	$TIMEROPTIONS="";
	$MD5DEEP="C:\\bin\\md5deep-4.4\\md5deep64.exe";	
}

if ($isLinux) {
	# set your base directory where the dataset is stored:
	$BASEDIR = "/mnt/nfs/Datasets/CDVA";
	#
	# set the following variables to the correct values;
	# set CDVA to a release cdva executable;
	# set CDVADEBUG to a debug cdva executable; 
	#
	$CDVA=$ENV{"HOME"} . "/bin/cdva";
	$CDVADEBUG="../src/cdva";
	$WORKSPACE="../run/workspace";
	$EXTRACTING="$BASEDIR/TimingExtract.txt";
	$MATCHING="$BASEDIR/TimingMatchingPairs.txt";
	$NONMATCHING="$BASEDIR/TimingNonMatchingPairs.txt";
	$RETRIEVAL="$BASEDIR/TimingRetrieval.txt";
	$DATABASE="$BASEDIR/Database.txt";
	$TIMER="/usr/bin/perf";
	$TIMEROPTIONS="stat -B";
	$MD5DEEP="/usr/bin/md5sum";
	$MEMORYCHECK="/usr/bin/time";
	$VALGRIND="/usr/bin/valgrind";
	$VALOPTIONS="--tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20  --track-origins=yes ";

	#for more detailed output add --leak-check=full to VALOPTIONS
	
	-x $VALGRIND  	or die "$VALGRIND not found.\n";
	-x $MEMORYCHECK  or die "$MEMORYCHECK not found.\n";
}

# check that required files exist

	-r $BASEDIR     or die "$BASEDIR not found.\n";
	-r $WORKSPACE  	or die "$WORKSPACE not found.\n";
	-r $EXTRACTING  or die "$EXTRACTING not found.\n";
	-r $MATCHING  	or die "$MATCHING not found.\n";
	-r $NONMATCHING or die "$NONMATCHING not found.\n";
	-r $RETRIEVAL  	or die "$RETRIEVAL not found.\n";	
	-r $DATABASE  	or die "$DATABASE not found.\n";
	-x $TIMER  	or die "$TIMER not found; please install it.\n";
	-x $MD5DEEP  	or die "$MD5DEEP not found; plase install it.\n";
	-x $CDVA        or die "$CDVA not found; please build cdva in release mode.\n";
	-x $CDVADEBUG   or die "$CDVADEBUG not found; please build cdva in debug mode.\n";

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
