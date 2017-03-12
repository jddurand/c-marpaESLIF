#!env perl
use strict;
use diagnostics;
use File::Basename;
use File::Copy qw/copy/;
use File::Copy::Recursive qw/dircopy/;
use File::Find qw/find/;
use File::Path qw/rmtree/;
use File::Spec;
use POSIX qw/EXIT_SUCCESS/;

my $updir = File::Spec->updir;
my $curdir = File::Spec->curdir;
#
# Move to upper dir of this script
#
my $okupdir = File::Spec->catdir(dirname($0), $updir);

chdir($okupdir) || die "Cannot move to $okupdir, $!";
#
# Copy everything needed
#
if (-d 'marpaESLIFCopy') {
    print "... Removing marpaESLIFCopy\n";
    rmtree('marpaESLIFCopy');
}

print "... Creating directory marpaESLIFCopy\n";
mkdir('marpaESLIFCopy') || die "Failed to create marpaESLIFCopy directory, $!";

print "... Creating directory marpaESLIFCopy/src\n";
my $src = File::Spec->catdir('marpaESLIFCopy', 'src');
mkdir($src) || die "Failed to create $src directory, $!";

print "... Copying directory ../../../3rdparty to marpaESLIFCopy/3rdparty\n";
dircopy(File::Spec->catdir(($updir) x 3, '3rdparty'), File::Spec->catdir('marpaESLIFCopy', '3rdparty'));

#
# 3rd party copy is likely to contain garbage. This can happen only in the ALL_IN_ONE case, and in such
# a case we know that every directory named CMakeFiles or output can be removed
# There are all eventual left over CTestTestfile.cmake files
#
my @toremove = ();
find({
      wanted => sub {
        if (-d $_) {
          my $b = basename($_);
          if (($b eq 'CMakeFiles') || ($b eq 'output')) {
            push(@toremove, File::Spec->canonpath($_));
          }
        } elsif (-f $_) {
          my $b = basename($_);
          if ($b eq 'CTestTestfile.cmake') {
            push(@toremove, File::Spec->canonpath($_));
          }
        }
      },
      no_chdir => 1
     }, 'marpaESLIFCopy');
foreach (@toremove) {
  if (-d $_) {
    print "... Removing $_\n";
    rmtree($_);
  } elsif (-f $_) {
    print "... Removing $_\n";
    unlink($_);
  }
}

print "... Copying directory ../../../include to marpaESLIFCopy/include\n";
dircopy(File::Spec->catdir(($updir) x 3, 'include'), File::Spec->catdir('marpaESLIFCopy', 'include'));

print "... Copying directory ../../../cmake to marpaESLIFCopy/cmake\n";
dircopy(File::Spec->catdir(($updir) x 3, 'cmake'), File::Spec->catdir('marpaESLIFCopy', 'cmake'));

print "... Copying file ../../../CMakeObjects.PL to marpaESLIFCopy/CMakeObjects.PL\n";
copy(File::Spec->catfile(($updir) x 3, 'CMakeObjects.PL'), File::Spec->catfile('marpaESLIFCopy', 'CMakeObjects.PL'));

print "... Copying file ../../../config_autoconf.ini to marpaESLIFCopy/config_autoconf.ini\n";
copy(File::Spec->catfile(($updir) x 3, 'config_autoconf.ini'), File::Spec->catfile('marpaESLIFCopy', 'config_autoconf.ini'));

foreach (qw/config.h.in bootstrap_actions.c marpaESLIF.c/) {
    print "... Copying file ../../../src/$_ to $src/$_\n";
    copy(File::Spec->catfile(($updir) x 3, 'src', $_), File::Spec->catfile($src, $_));
}

exit(EXIT_SUCCESS);
