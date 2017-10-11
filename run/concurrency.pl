#!/usr/bin/env perl
#
# concurrency utilities
# currently working only under Linux
# Don't launch directly: use the run-*.pl scripts
#

use warnings;
use strict;

use Time::HiRes qw(usleep);


# Check for Proc::Queue and Sys::CPU.
# Needed to limit the processes spawned
# to the number of available cores.
#
# To install them, use the following commands:
#	perl -MCPAN -e 'install Proc::Queue'
#	perl -MCPAN -e 'install Sys::CPU'

if(eval("use Proc::Queue;") and eval("use Sys::CPU")) {
	Proc::Queue::size(Sys::CPU::cpu_count());
}
my $isWindows = ("$^O" eq "MSWin32");


sub asyncrun {
	my $command = $_[0];
	print "$command \n";

	if ($isWindows) {
		system $command;
	} else {
		my $pid = fork();
		if ($pid == -1) {
			die "Process spawning error\n";
		} elsif ($pid == 0) {
		exec $command or die "Failed to launch: $command\n";
		}
	}
}

# launch single process
sub syncrun {
	my $command = $_[0];
	print "$command \n";
	system $command;
}

sub waitAll {
	if (!$isWindows) {
	while (wait() != -1) {
		usleep 1;
	}
    }
}

# module has to return a value:
1;
