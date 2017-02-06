#!env perl
use strict;
use diagnostics;
use Config::AutoConf 0.311;
use Capture::Tiny qw/capture/;
use POSIX qw/EXIT_SUCCESS/;

#
# Generation of a makefile for perl
#
my $config = File::Spec->catfile('output', 'include', 'config.h');
my $log = File::Spec->catfile('config.log');
my $ac = Config::AutoConf->new(logfile => $log);

#
# Get current version from CMakeLists.txt
#
my ($GENERICLOGGER_VERSION_MAJOR, $GENERICLOGGER_VERSION_MINOR, $GENERICLOGGER_VERSION_PATCH) = get_version();
print "Generating $config for genericLogger version $GENERICLOGGER_VERSION_MAJOR.$GENERICLOGGER_VERSION_MINOR.$GENERICLOGGER_VERSION_PATCH\n";
$ac->check_cc;
$ac->define_var('GENERICLOGGER_VERSION_MAJOR', "($GENERICLOGGER_VERSION_MAJOR)");
$ac->define_var('GENERICLOGGER_VERSION_MINOR', "($GENERICLOGGER_VERSION_MINOR)");
$ac->define_var('GENERICLOGGER_VERSION_PATCH', "($GENERICLOGGER_VERSION_PATCH)");
my $have_stdio_h = $ac->check_header('stdio.h');
$ac->check_header('stddef.h');
my $have_stdlib_h = $ac->check_header('stdlib.h');
my $have_stdarg_h = $ac->check_header('stdarg.h') || die "<stdarg.h> must be present";
$ac->check_header('stdint.h');
my $have_time_h = $ac->check_header('sys/time.h');
$ac->check_header('sys/types.h');
$ac->check_header('errno.h');
$ac->check_header('string.h');
$ac->check_header('unistd.h');
$ac->check_header('io.h');
$ac->check_header('time.h');
find_inline($ac);
find_va_copy($ac, $have_stdlib_h, $have_stdarg_h);
find_fileno($ac, $have_stdlib_h);
find_vsnprintf($ac, $have_stdio_h, $have_stdarg_h);
find_localtime_r($ac, $have_time_h);
$ac->write_config_h($config);

sub get_version {
  open(my $fh, '<', 'CMakeLists.txt') || die "Cannot open CMakeLists.txt, $!";
  my $content = do { local $/; <$fh>; };
  close($fh) || warn "Failed to close CMakeLists.txt, $!";

  my @rc;
  foreach (qw/GENERICLOGGER_VERSION_MAJOR GENERICLOGGER_VERSION_MINOR GENERICLOGGER_VERSION_PATCH/) {
    if ($content =~ /^SET\s*\(\s*$_\s*(\d+)\s*\)/sm) {
      push(@rc, $1);
    } else {
      die "Failed to find GENERICLOGGER_VERSION_MAJOR",
    }
  }

  return @rc;
}

sub find_inline {
  my ($ac, $have_stdlib_h, $have_stdarg_h) = @_;

  my $file = File::Spec->catfile('cmake', 'inline.c');
  open(my $fh, '<', $file) || die "Cannot open $file, $!";
  my $source = do { local $/; <$fh>; };
  close($fh) || warn "Cannot close $file, $!";

  foreach (qw/inline __inline__ inline__ __inline/) {
    my $prologue = "#define C_INLINE $_";
    $ac->msg_checking("inline keyword as \"$_\"");
    if ($ac->compile_if_else("$prologue\n$source")) {
      $ac->msg_result('yes');
      $ac->define_var('C_INLINE', $_);
      last;
    } else {
      $ac->msg_result('no');
    }
  }
}

sub find_va_copy {
  my ($ac, $have_stdlib_h, $have_stdarg_h) = @_;

  my $file = File::Spec->catfile('cmake', 'va_copy.c');
  open(my $fh, '<', $file) || die "Cannot open $file, $!";
  my $source = do { local $/; <$fh>; };
  close($fh) || warn "Cannot close $file, $!";
  my @prologue;
  push(@prologue, "#include <stdlib.h>") if ($have_stdlib_h);
  push(@prologue, "#include <stdarg.h>") if ($have_stdarg_h);
  foreach (qw/va_copy _va_copy __va_copy/) {
    push(@prologue, "#define C_VA_COPY $_");
    $ac->msg_checking("va_copy keyword as \"$_\"");
    my $prologue = join("\n", @prologue);
    if ($ac->compile_if_else("$prologue\n$source")) {
      $ac->msg_result('yes');
      $ac->define_var('C_VA_COPY', $_);
      last;
    } else {
      $ac->msg_result('no');
    }
  }
}

sub find_fileno {
  my ($ac, $have_stdio_h) = @_;

  my $file = File::Spec->catfile('cmake', 'fileno.c');
  open(my $fh, '<', $file) || die "Cannot open $file, $!";
  my $source = do { local $/; <$fh>; };
  close($fh) || warn "Cannot close $file, $!";
  my @prologue;
  push(@prologue, "#include <stdio.h>") if ($have_stdio_h);
  foreach (qw/fileno _fileno __fileno/) {
    push(@prologue, "#define C_FILENO $_");
    $ac->msg_checking("fileno keyword as \"$_\"");
    my $prologue = join("\n", @prologue);
    if ($ac->compile_if_else("$prologue\n$source")) {
      $ac->msg_result('yes');
      $ac->define_var('C_FILENO', $_);
      last;
    } else {
      $ac->msg_result('no');
    }
  }
}

sub find_vsnprintf {
  my ($ac, $have_stdio_h, $have_stdarg_h) = @_;

  my $file = File::Spec->catfile('cmake', 'vsnprintf.c');
  open(my $fh, '<', $file) || die "Cannot open $file, $!";
  my $source = do { local $/; <$fh>; };
  close($fh) || warn "Cannot close $file, $!";
  my @prologue;
  push(@prologue, "#include <stdio.h>") if ($have_stdio_h);
  push(@prologue, "#include <stdarg.h>") if ($have_stdarg_h);
  foreach (qw/vsnprintf _vsnprintf __vsnprintf/) {
    push(@prologue, "#define C_VSNPRINTF $_");
    $ac->msg_checking("vsnprintf keyword as \"$_\"");
    my $prologue = join("\n", @prologue);
    if ($ac->compile_if_else("$prologue\n$source")) {
      $ac->msg_result('yes');
      $ac->define_var('C_VSNPRINTF', $_);
      last;
    } else {
      $ac->msg_result('no');
    }
  }
}

sub find_localtime_r {
  my ($ac, $have_time_h) = @_;

  my $file = File::Spec->catfile('cmake', 'localtime_r.c');
  open(my $fh, '<', $file) || die "Cannot open $file, $!";
  my $source = do { local $/; <$fh>; };
  close($fh) || warn "Cannot close $file, $!";
  my @prologue;
  push(@prologue, "#include <time.h>") if ($have_time_h);
  foreach (qw/localtime_r _localtime_r __localtime_r/) {
    push(@prologue, "#define C_LOCALTIME_R $_");
    $ac->msg_checking("localtime_r keyword as \"$_\"");
    my $prologue = join("\n", @prologue);
    if ($ac->compile_if_else("$prologue\n$source")) {
      $ac->msg_result('yes');
      $ac->define_var('C_LOCALTIME_R', $_);
      last;
    } else {
      $ac->msg_result('no');
    }
  }
}

