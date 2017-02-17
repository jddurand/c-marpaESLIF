#!env perl
use strict;
use diagnostics;
use Config::AutoConf::INI;
use ExtUtils::CBuilder;
use File::Basename;
use File::Find qw/find/;
use File::Path qw/make_path/;
use File::Copy::Recursive qw/dircopy/;
use POSIX qw/EXIT_SUCCESS/;

# ------------------------------------------------------------
# Generation of objects using perl setup, for use in perl's XS
# ------------------------------------------------------------
my $version = get_version();
print "Generating config for marpaWrapper version $version\n";

# ------------------------
# Write configuration file
# ------------------------
my $config_h_in = File::Spec->catfile('include', 'config.h.in');
my $config_h    = File::Spec->catfile('output', 'include', 'config.h');
make_path(dirname($config_h));

my $ac = Config::AutoConf::INI->new(logfile => 'config.log')->check;
$ac->define_var('MARPA_LIB_MAJOR_VERSION', 'MARPA_MAJOR_VERSION');
$ac->define_var('MARPA_LIB_MICRO_VERSION', 'MARPA_MICRO_VERSION');
$ac->define_var('MARPA_LIB_MINOR_VERSION', 'MARPA_MINOR_VERSION');
$ac->check;
write_config($version, $config_h_in, $config_h);

# -------------
# Fake export.h
# -------------
my $export_h = File::Spec->catfile('output', 'include', 'marpaWrapper', 'export.h');
make_path(dirname($export_h));
open(my $fh, '>', $export_h) || die "Cannot open $export_h, $!";
print $fh "#define marpaWrapper_EXPORT\n";
close($fh) || warn "Cannot close $export_h, $!";

# ------------------------------
# We depend on marpa third party
# ------------------------------
if (! dircopy(File::Spec->catdir('3rdparty', 'github', 'Marpa--R2-remote'),
              File::Spec->catdir('3rdparty', 'output', 'Marpa--R2-remote'))) {
  die 'Failed to copy Marpa--R2--remote';
}

# ---------------------------------
# We depend on these generic* stuff
# ---------------------------------
foreach (qw/genericLogger genericHash genericSparseArray genericStack/) {
  $ac->push_includes(File::Spec->catdir('output', '3rdparty', $_, 'include'));
}

# ----------------
# Get source files
# ----------------
my @sources;
find(
    {
	no_chdir => 1,
	wanted => sub {
	    my $file = File::Spec->canonpath($_);
	    if (-f $file && ($file =~ /\.c$/ || $file =~ /\.cpp$/)) {
		push(@sources, $file)
	    }
	},
    },
    'src');

# ----------------------------------------------------------------------------------------
# Generate objects
# (individually- not efficient but I do not see how CBuilder mixes C and C++ source files)
# ----------------------------------------------------------------------------------------
my $cbuilder = ExtUtils::CBuilder->new();
my @objects;
my $obj_dir = File::Spec->catfile('output', 'obj');

make_path($obj_dir);
foreach my $source (@sources) {
    my $is_cplusplus = ($source =~ /\.cpp$/i || $source =~ /\.c\+\+$/i);
    my $obj = basename($cbuilder->object_file($source));
    $obj = File::Spec->catfile($obj_dir, basename($cbuilder->object_file($source)));
    push(@objects, $cbuilder->object_file($source));
    $cbuilder->compile(
	source        => $source,
	object_file   => $obj,
	include_dirs  => [ 'include', File::Spec->catdir('output', 'include'), File::Spec->catdir('3rdparty', 'github', 'Marpa--R2-remote', 'cpan', 'engine', 'read_only') ],
	'C++'         => $is_cplusplus
	);
}

# ----
# Done
# ----
exit(EXIT_SUCCESS);

sub get_version {
    open(my $fh, '<', 'CMakeLists.txt') || die "Cannot open CMakeLists.txt, $!";
    my $content = do { local $/; <$fh>; };
    close($fh) || warn "Failed to close CMakeLists.txt, $!";

    my @rc;
    foreach (qw/MARPAWRAPPER_VERSION_MAJOR MARPAWRAPPER_VERSION_MINOR MARPAWRAPPER_VERSION_PATCH/) {
	if ($content =~ /^SET\s*\(\s*$_\s*(\d+)\s*\)/sm) {
            push(@rc, $1);
	} else {
	    die "Failed to find $_",
	}
    }

    return join('.', @rc)
}

sub write_config {
    my ($version, $input, $output) = @_;

    make_path(dirname($output));
    open(my $fh, '<', $input) || die "Cannot open $input, $!";
    my $source = do { local $/; <$fh>; };
    close($fh) || warn "Cannot close $input, $!";

    $source =~ s/^[ \t]*#[ \t]*cmakedefine[ \t]+(\w+)+[ \t]+\@([^ \t@]*)\@//smg;

    open($fh, '>', $output) || die "Cannot open $output, $!";
    print $fh <<CONFIG;
#ifndef MARPAWRAPPER_CONFIG_WRAPPER_H
#define MARPAWRAPPER_CONFIG_WRAPPER_H

#include <marpaWrapper/config_autoconf.h>

#define C_INLINE
#define MARPAWRAPPER_VERSION "$version"
   
$source

#endif /* MARPAWRAPPER_CONFIG_WRAPPER_H */
CONFIG
    close($fh) || warn "Cannot close $output, $!";
}
