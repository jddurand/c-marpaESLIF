#!env perl
use strict;
use diagnostics;
use File::Basename;
use File::Copy qw/copy/;
use File::Copy::Recursive qw/dircopy/;
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
print "... Removing marpaESLIFCopy if any\n";
rmtree('marpaESLIFCopy');

print "... Creating directory marpaESLIFCopy\n";
mkdir('marpaESLIFCopy') || die "Failed to create marpaESLIFCopy directory, $!";

print "... Creating directory marpaESLIFCopy/src\n";
my $src = File::Spec->catdir('marpaESLIFCopy', 'src');
mkdir($src) || die "Failed to create $src directory, $!";

print "... Copying directory ../../../3rdparty to marpaESLIFCopy\n";
dircopy(File::Spec->catdir(($updir) x 3, '3rdparty'), File::Spec->catdir('marpaESLIFCopy', '3rdparty'));

print "... Copying directory ../../../include to marpaESLIFCopy\n";
dircopy(File::Spec->catdir(($updir) x 3, 'include'), File::Spec->catdir('marpaESLIFCopy', 'include'));

print "... Copying directory ../../../cmake to marpaESLIFCopy\n";
dircopy(File::Spec->catdir(($updir) x 3, 'cmake'), File::Spec->catdir('marpaESLIFCopy', 'cmake'));

print "... Copying file ../../../CMakeObjects.PL to .\n";
copy(File::Spec->catfile(($updir) x 3, 'CMakeObjects.PL'), File::Spec->catfile('marpaESLIFCopy', 'CMakeObjects.PL'));

print "... Copying file ../../../config_autoconf.ini to .\n";
copy(File::Spec->catfile(($updir) x 3, 'config_autoconf.ini'), File::Spec->catfile('marpaESLIFCopy', 'config_autoconf.ini'));

foreach (qw/config.h.in bootstrap_actions.c marpaESLIF.c/) {
    print "... Copying file ../../../src/$_ to $src\n";
    copy(File::Spec->catfile(($updir) x 3, 'src', $_), File::Spec->catfile($src, $_));
}

exit(EXIT_SUCCESS);
