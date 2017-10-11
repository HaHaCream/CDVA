Running the tests on Linux or Windows requires some preconditions:
- the CDVA Dataset and annotations;
- the CDVA executables (both debug and release versions);
- "workspace" must contain a Database of CDVA descriptors (produced by the CDVA executables);
- the following external applications must be installed in your system:

On Linux:
1) Perf
	Ubuntu: sudo apt-get install linux-tools-generic
	Fedora / Red Hat Enterprise Linux: yum install perf"
	openSUSE / SUSE Linux Enterprise: zypper install perf"

2) md5deep
	Ubuntu: sudo apt-get install md5deep
	Fedora / Red Hat Enterprise Linux: yum install md5deep"
	openSUSE / SUSE Linux Enterprise: zypper install md5deep"

3) Valgrind
	Ubuntu: sudo apt-get install valgrind"
	Fedora / Red Hat Enterprise Linux: yum install valgrind"
	openSUSE / SUSE Linux Enterprise: zypper install valgrind"

On Windows:
1) C:\bin\ptime.exe 
	Download from http://www.pc-tools.net/win32/ptime/ and install it in C:\bin

2) C:\bin\md5deep-4.4\md5deep64.exe (http://md5deep.sourceforge.net/)
	Download from https://github.com/jessek/hashdeep/releases the zip archive and install it on C:\bin


Run the "setenv.pl" script to check and fix your environment; when no more error messages are printed, all other script can be run:
- verify-md5sum.pl  : to verify the MD5 checksum of all data and annotation files in the CDVA Dataset;
- run-speed-test.pl : to get the execution time of cdva extract, match and retrieve;
- run-memory-test.pl : (only on Linux) to check the memory allocation of cdva extract, match and retrieve.
