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
find_inline($ac);
$ac->define_var('GENERICLOGGER_VERSION_MAJOR', $GENERICLOGGER_VERSION_MAJOR);
$ac->define_var('GENERICLOGGER_VERSION_MINOR', $GENERICLOGGER_VERSION_MINOR);
$ac->define_var('GENERICLOGGER_VERSION_PATCH', $GENERICLOGGER_VERSION_PATCH);
$ac->check_header('stdio.h');
$ac->check_header('stddef.h');
$ac->check_header('stdlib.h');
$ac->check_header('stdarg.h') || die "<stdarg.h> must be present";
$ac->check_header('stdint.h');
$ac->check_header('sys/time.h');
$ac->check_header('sys/types.h');
$ac->check_header('errno.h');
$ac->check_header('string.h');
$ac->check_header('unistd.h');
$ac->check_header('io.h');
$ac->check_header('time.h');

sub get_version {
  open(my $fh, '<', 'CMakeLists.txt') || die "Cannot open CMakeLists.txt, $!";
  my $content = do { local $/; <$fh>; };
  close($fh) || print STDERR "Failed to close CMakeLists.txt, $!\n";

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
  my ($ac) = @_;

  foreach (qw/inline __inline__ inline__ __inline/) {
    $ac->msg_checking("inline keyword as $_");
    my $source = "
typedef int foo_t;
static $_ foo_t static_foo() {
  return 0;
}
foo_t foo() {
  return 0;
}
int main(int argc, char *argv[]){
  return 0;
}
";
    if ($ac->compile_if_else($source)) {
      $ac->msg_result('yes');
      $ac->define_var('C_INLINE', $_);
      last;
    } else {
      $ac->msg_result('no');
    }
  }
}
