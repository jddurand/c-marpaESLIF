#!env perl
use strict;
use diagnostics;
use Module::Build;
use Alien::CMake;
use POSIX qw/EXIT_SUCCESS/;
use File::Spec;
use File::chdir;
use File::Temp qw/tempdir/;
use IO::Handle;
use ExtUtils::CBuilder;
use File::Copy::Recursive qw/dircopy/;
use File::Basename qw/dirname/;
use Cwd;
use IPC::Run qw/run/;
use Config;

autoflush STDOUT 1;

#
# Cache updir just for convenience
#
my $updir = File::Spec->updir;
my $curdir = File::Spec->curdir;

#
# Mapping of perl's setting to environment for cmake
#
my %perl2cmake = (
    ccname                   => 'CC',
    cccdlflags               => 'CFLAGS',
    ccdlflags                => 'CFLAGS',
    ccflags                  => 'CFLAGS',
    optimize                 => 'CFLAGS',
    lddlflags                => 'LDFLAGS',
    ldflags                  => 'LDFLAGS',
    lkflags                  => 'LDFLAGS',
    );
my %perl2cmakedone;

foreach (keys %perl2cmake) {
    next unless exists($Config{$_});
    next unless defined($Config{$_});
    next unless $Config{$_};

    my $envname = $perl2cmake{$_};
    if (defined($ENV{$envname})) {
	# print "... Appending \$Config{$_}=$Config{$_} to \$ENV{$envname}\n";
	$ENV{$envname} .= " $Config{$_}";
    } else {
	# print "... Setting \$Config{$_}=$Config{$_} in \$ENV{$envname}\n";
	$ENV{$envname} = $Config{$_};
    }
    $perl2cmakedone{$envname}++;
}
foreach (sort keys %perl2cmakedone) {
    print "... Setted \$ENV{$_} to $ENV{$_}\n";
}

#
# Make sure current working directory is the location of this script
#
BEGIN {
    chdir(dirname($0)) || die $!;
    print STDERR "... Current directory is " . getcwd . "\n";
}

my $cmake_bin_dir = Alien::CMake->config('bin_dir');
my $cmake_exe;
if (! $cmake_bin_dir) {
    #
    # When empty, it means that cmake is already in the path
    #
    $cmake_exe = 'cmake';
} else {
    $cmake_exe = File::Spec->catfile($cmake_bin_dir, 'cmake');
}

foreach (qw/genericLogger-remote/) {
    do_external($cmake_exe, $_);
}

exit(EXIT_SUCCESS);

sub do_external {
    my ($cmake, $what) = @_;
    my $dir = tempdir(CLEANUP => 1);

    my $orig = File::Spec->catdir(($updir) x 4, '3rdparty', 'github', $what);
    print "... Copying $orig in $dir\n";
    dircopy($orig, $dir) or die $!;
    print "... Building $what in $dir\n";

    local $CWD = $dir;
    run(['cmake', $curdir]) or die "$?";
    run(['make', 'VERBOSE=1']) or die "$?";
}
