#!env perl
use strict;
use diagnostics;
use Module::Build;
use Alien::CMake;
use POSIX qw/EXIT_SUCCESS/;
use File::Spec;
use File::chdir;
use File::which;
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
# Make sure current working directory is the location of this script
#
BEGIN {
    chdir(dirname($0)) || die $!;
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

our %_more_config = (ar => 'CMAKE_AR');
sub _get_config {
    #
    # We use ExtUtils::CBuilder for C and CXX setup
    #
    my $b = ExtUtils::CBuilder->new();
    my %bconfig = $b->get_config();
    my %config = (
	C => $bconfig{c}
	);
    #
    # And add others
    #
    foreach (keys %_more_config) {
	next unless exists $Config{$_};
	next unless $Config{$_};
	$config{$_} = which($Config{$_}) || $_;
    }
}

sub do_external {
    my ($cmake, $what) = @_;
    my $dir = tempdir(CLEANUP => 1);

    my $orig = File::Spec->catdir(($updir) x 4, '3rdparty', 'github', $what);
    dircopy($orig, $dir) or die $!;

    local $CWD = $dir;
    # Just in case
    unlink 'CMakeLists.txt';
    # Push our setting
    my $tmp = File::Temp->new(UNLINK => 0, SUFFIX => '.cmake');
    print $tmp "SET (Some data\n";
      print "Filename is $tmp\n";
      $tmp->seek( 0, SEEK_END );

    run(['cmake', $curdir]) or die "$?";
    run(['make', 'VERBOSE=1']) or die "$?";
}
