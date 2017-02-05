#!env perl
use strict;
use diagnostics;
use Module::Build;
use Alien::CMake;
use POSIX qw/EXIT_SUCCESS/;
use File::Spec;

my $cmake_bin_dir = Alien::CMake->config('bin_dir');
my $cmake_exe;
if (! $cmake_bin_dir) {
    #
    # When empty, it means that cmake is already in the path
    #
    $cmake_exe = 'cmake';
} else {
    $cmake_exe = File::Spec->catfile($cmake_bin_dir 'cmake');
}

exit(EXIT_SUCCESS);
