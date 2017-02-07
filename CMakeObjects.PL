#!env perl
use strict;
use diagnostics;
use Config::AutoConf 0.311;
use Capture::Tiny qw/capture/;
use ExtUtils::CBuilder;
use File::Basename;
use POSIX qw/EXIT_SUCCESS/;
use File::Find qw/find/;
use File::Path qw/make_path/;

#
# Generation of the library using perl compiler
#
my $config = File::Spec->catfile('output', 'include', 'config.h');
my $log = File::Spec->catfile('config.log');
my $ac = Config::AutoConf->new(logfile => $log);

my %ac;
get_version($ac, \%ac);
$ac{GENERICLOGGER_VERSION} = "$ac{GENERICLOGGER_VERSION_MAJOR}.$ac{GENERICLOGGER_VERSION_MINOR}.$ac{GENERICLOGGER_VERSION_PATCH}";
print "Generating $config for genericLogger version $ac{GENERICLOGGER_VERSION}\n";
$ac->check_cc;
$ac{HAVE_STDIO_H}     = $ac->check_header('stdio.h');
$ac{HAVE_STDDEF_H}    = $ac->check_header('stddef.h');
$ac{HAVE_STDLIB_H}    = $ac->check_header('stdlib.h');
$ac{HAVE_STDARG_H}    = $ac->check_header('stdarg.h') || die "<stdarg.h> must be present";
$ac{HAVE_STDINT_H}    = $ac->check_header('stdint.h');
$ac{HAVE_SYS_TIME_H}  = $ac->check_header('sys/time.h');
$ac{HAVE_SYS_TYPES_H} = $ac->check_header('sys/types.h');
$ac{HAVE_ERRNO_H}     = $ac->check_header('errno.h');
$ac{HAVE_STRING_H}    = $ac->check_header('string.h');
$ac{HAVE_UNISTD_H}    = $ac->check_header('unistd.h');
$ac{HAVE_IO_H}        = $ac->check_header('io.h');
$ac{HAVE_TIME_H}      = $ac->check_header('time.h');
find_this($ac, \%ac, 'C_INLINE',      File::Spec->catfile('cmake', 'inline.c'),      undef,                             [qw/inline __inline__ inline__ __inline/]);
find_this($ac, \%ac, 'C_VA_COPY',     File::Spec->catfile('cmake', 'va_copy.c'),     [qw/HAVE_STDLIB_H HAVE_STDARG_H/], [qw/va_copy _va_copy __va_copy/]);
find_this($ac, \%ac, 'C_FILENO',      File::Spec->catfile('cmake', 'fileno.c'),      [qw/HAVE_STDIO_H/],                [qw/fileno _fileno __fileno/]);
find_this($ac, \%ac, 'C_WRITE',       File::Spec->catfile('cmake', 'write.c'),       [qw/HAVE_STDIO_H/],                [qw/write _write __write/]);
find_this($ac, \%ac, 'C_VSNPRINTF',   File::Spec->catfile('cmake', 'vsnprintf.c'),   [qw/HAVE_STDIO_H HAVE_STDARG_H/],  [qw/vsnprintf _vsnprintf __vsnprintf/]);
find_this($ac, \%ac, 'C_LOCALTIME_R', File::Spec->catfile('cmake', 'localtime_r.c'), [qw/HAVE_TIME_H/],                 [qw/localtime_r _localtime_r __localtime_r/]);

my $config_h_in = File::Spec->catfile('include', 'genericLogger_config.h.in');
my $config_h    = File::Spec->catfile('output', 'include', 'genericLogger', 'config.h');
write_config($ac, \%ac, $config_h_in, $config_h);

#
# Get source files
#
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

#
# Create a fake export.h
#
my $export_h = File::Spec->catfile('output', 'include', 'genericLogger', 'export.h');
make_path(dirname($export_h));
open(my $fh, '>', $export_h) || die "Cannot open $export_h, $!";
print $fh "#define genericLogger_EXPORT\n";
# I do not want to play with extra_compiler_flags, I use the fake export.h instead
print $fh "#define GENERICLOGGER_VERSION \"$ac{GENERICLOGGER_VERSION}\"\n";
close($fh) || warn "Cannot close $export_h, $!";
#
# Generate object individually (not efficient but I do not see how CBuilder mixes C and C++ source files)
#
my $cbuilder = ExtUtils::CBuilder->new();
my $have_cplusplus = 0;
my @objects;
my $obj_dir = File::Spec->catfile('output', 'obj');

make_path($obj_dir);
foreach my $source (@sources) {
    my $is_cplusplus = ($source =~ /\.cpp$/i || $source =~ /\.c\+\+$/i);
    if ($is_cplusplus) {
	$have_cplusplus = 1 unless $have_cplusplus;
    }
    my $obj = basename($cbuilder->object_file($source));
    $obj = File::Spec->catfile($obj_dir, basename($cbuilder->object_file($source)));
    push(@objects, $cbuilder->object_file($source));
    $cbuilder->compile(
	source               => $source,
	object_file          => $obj,
	include_dirs         => [ 'include', File::Spec->catdir('output', 'include') ],
	have_cplusplus       => $is_cplusplus
	);
}

#
# Done
#
exit(EXIT_SUCCESS);

sub get_version {
    my ($ac, $acp) = @_;

    open(my $fh, '<', 'CMakeLists.txt') || die "Cannot open CMakeLists.txt, $!";
    my $content = do { local $/; <$fh>; };
    close($fh) || warn "Failed to close CMakeLists.txt, $!";

    my @rc;
    foreach (qw/GENERICLOGGER_VERSION_MAJOR GENERICLOGGER_VERSION_MINOR GENERICLOGGER_VERSION_PATCH/) {
	if ($content =~ /^SET\s*\(\s*$_\s*(\d+)\s*\)/sm) {
	    $acp->{$_} = $ac->define_var($_, $1);
	} else {
	    die "Failed to find $_",
	}
    }
}

sub find_this {
  my ($ac, $acp, $what, $file, $ac_for_prologue, $alternatives) = @_;

  open(my $fh, '<', $file) || die "Cannot open $file, $!";
  my $source = do { local $/; <$fh>; };
  close($fh) || warn "Cannot close $file, $!";
  my @prologue;
  $ac_for_prologue //= [];
  foreach my $key (@{$ac_for_prologue}) {
      next unless $acp->{$key};
      if ($key =~ /^HAVE_(.+)$/) {
	  my $INC = $1;
	  $INC =~ s/_([^_]*)$/.$1/;
	  $INC =~ s/_/\//;
	  my $inc = lc($INC);
	  push(@prologue, "#include <$inc>");
      }
  }
  $alternatives //= [];
  foreach (@{$alternatives}) {
      push(@prologue, "#define $what $_");
      $ac->msg_checking("$what macro as \"$_\"");
      my $prologue = join("\n", @prologue);
      my $c = "$prologue\n$source";
      if ($ac->link_if_else($c)) {
	  $ac->msg_result('yes');
	  $acp->{$what} = $ac->define_var($what, $_);
	  last;
      } else {
	  $ac->msg_result('no');
      }
  }
}

sub write_config {
    my ($ac, $acp, $input, $output) = @_;

    make_path(dirname($output));
    open(my $fh, '<', $input) || die "Cannot open $input, $!";
    my $source = do { local $/; <$fh>; };
    close($fh) || warn "Cannot close $input, $!";

    $source =~ s/^[ \t]*#[ \t]*cmakedefine[ \t]+(\w+)+[ \t]+\@([^ \t@]*)\@/get_define($ac, $acp, $1, $2)/smge;

    open($fh, '>', $output) || die "Cannot open $output, $!";
    print $fh $source;
    close($fh) || warn "Cannot close $output, $!";
}

sub get_define {
    my ($ac, $acp, $define, $key) = @_;

    my $value = $acp->{$key};
    return $value ? "#define $define $value" : "/* #undef $define */";
}

