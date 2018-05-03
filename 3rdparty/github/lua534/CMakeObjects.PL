#!env perl
use strict;
use diagnostics;
use Config;
use Config::AutoConf::INI;
use ExtUtils::CBuilder;
use File::chdir;
use File::Basename qw/basename dirname/;
use File::Find qw/find/;
use File::Path qw/make_path/;
use File::Copy::Recursive qw/dircopy/;
use IPC::Run qw/run/;
use POSIX qw/EXIT_SUCCESS/;
use Perl::OSType qw/is_os_type/;

# ------------------------------------------------------------
# Generation of objects using perl setup, for use in perl's XS
# ------------------------------------------------------------
my $version = get_version();
print "Generating config for lua534 version $version\n";

# ------------------------
# Write configuration file
# ------------------------
my $config_h_in = File::Spec->catfile('src', 'luaconf.h.in');
my $config_h    = File::Spec->catfile('output', 'include', 'luaconf.h');
make_path(dirname($config_h));

my $ac = Config::AutoConf::INI->new(logfile => 'config.log');
$ac->define_var('LUA_32BITS', 1);
$ac->define_var('LUA_USE_C89', 1);
if (is_os_type('Windows')) {
    $ac->define_var('LUA_DL_DLL', 1);
} else {
    $ac->define_var('LUA_DL_DLOPEN', 1);
    $ac->define_var('LUA_USE_POSIX', 1);
}

$ac->check;
write_config($version, $config_h_in, $config_h);

# -------------
# Fake export.h
# -------------
my $export_h = File::Spec->catfile('output', 'include', 'lua', 'export.h');
make_path(dirname($export_h));
open(my $fh, '>', $export_h) || die "Cannot open $export_h, $!";
print $fh "#define lua_EXPORT\n";
print $fh "#define LUA_NO_EXPORT\n";
close($fh) || warn "Cannot close $export_h, $!";

my @additional_includes = ();
my @additional_srcdir = ();
# -----------------------------------------------------
# Well, no third party
# -----------------------------------------------------

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
    'src', @additional_srcdir);

# ----------------------------------------------------------------------------------------
# Generate objects
# (individually- not efficient but I do not see how CBuilder mixes C and C++ source files)
# ----------------------------------------------------------------------------------------
my $cbuilder = ExtUtils::CBuilder->new();
my @objects;
my $obj_dir = File::Spec->catfile('output', 'obj4perl');

make_path($obj_dir);
foreach my $source (@sources) {
    my $is_cplusplus = ($source =~ /\.cpp$/i || $source =~ /\.c\+\+$/i);
    my $obj = basename($cbuilder->object_file($source));
    $obj = File::Spec->catfile($obj_dir, basename($cbuilder->object_file($source)));
    push(@objects, $cbuilder->object_file($source));
    $cbuilder->compile(
	source        => $source,
	object_file   => $obj,
	include_dirs  => [ 'include', File::Spec->catdir('output', 'include'), @additional_includes ],
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
    if ($content =~ /^MYPACKAGESTART\s*\(\s*lua\s+(\d+)\s+(\d+)\s+(\d+)\s*\)/sm) {
        @rc = ($1, $2, $3);
    } else {
        foreach (qw/LUA_VERSION_MAJOR LUA_VERSION_MINOR LUA_VERSION_PATCH/) {
            if ($content =~ /^SET\s*\(\s*$_\s*(\d+)\s*\)/sm) {
                push(@rc, $1);
            } else {
                die "Failed to find $_",
            }
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
#ifndef LUA_CONFIG_WRAPPER_H
#define LUA_CONFIG_WRAPPER_H

#include <lua/config_autoconf.h>

#define LUA_VERSION "$version"
   
$source

#endif /* LUA_CONFIG_WRAPPER_H */
CONFIG
    close($fh) || warn "Cannot close $output, $!";
}
