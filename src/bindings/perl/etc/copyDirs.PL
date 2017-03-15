#!env perl
use strict;
use diagnostics;
use Cwd;
use Fcntl ':mode';
use File::Basename;
use File::Copy qw/copy/;
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
_dircopy(File::Spec->catdir(($updir) x 3, '3rdparty'), File::Spec->catdir('marpaESLIFCopy', '3rdparty'));

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
          if (grep { $b eq $_ } qw/CTestTestfile.cmake .gitignore .gitattributes/) {
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
_dircopy(File::Spec->catdir(($updir) x 3, 'include'), File::Spec->catdir('marpaESLIFCopy', 'include'));

print "... Copying directory ../../../cmake to marpaESLIFCopy/cmake\n";
_dircopy(File::Spec->catdir(($updir) x 3, 'cmake'), File::Spec->catdir('marpaESLIFCopy', 'cmake'));

print "... Copying file ../../../CMakeObjects.PL to marpaESLIFCopy/CMakeObjects.PL\n";
copy(File::Spec->catfile(($updir) x 3, 'CMakeObjects.PL'), File::Spec->catfile('marpaESLIFCopy', 'CMakeObjects.PL')) || die "$!";

print "... Copying file ../../../config_autoconf.ini to marpaESLIFCopy/config_autoconf.ini\n";
copy(File::Spec->catfile(($updir) x 3, 'config_autoconf.ini'), File::Spec->catfile('marpaESLIFCopy', 'config_autoconf.ini')) || die "$!";

print "... Copying file ../../../CMakeLists.txt to marpaESLIFCopy/CMakeLists.txt\n";
copy(File::Spec->catfile(($updir) x 3, 'CMakeLists.txt'), File::Spec->catfile('marpaESLIFCopy', 'CMakeLists.txt')) || die "$!";

foreach (qw/bootstrap_actions.c marpaESLIF.c/) {
    print "... Copying file ../../../src/$_ to $src/$_\n";
    copy(File::Spec->catfile(($updir) x 3, 'src', $_), File::Spec->catfile($src, $_)) || die "$!";
}

exit(EXIT_SUCCESS);

#
# Well, I have a problem on Windows because path is greater than MAX_PATH...!
# This private implementation of _dircopy is doing the job...
# Even if I would use Win32::LongPath, dzil would fail afterwards -;
#
sub _dircopy {
    my ($srcdir, $dstdir) = @_;

    if (! -d $srcdir) {
        die "$srcdir is not a directory";
    }
    if (! -d $dstdir) {
        print "... Creating directory $dstdir\n",
        die "Failed to create $dstdir directory, $!" unless mkdir $dstdir;
    }

    my $workdir = cwd;

    my $basedir = basename($srcdir);
    my $reducedLength = 0;
  
    my $stats = _readdir($srcdir);
    foreach (sort keys %{$stats}) {
        next if $_ eq $updir;
        next if $_ eq $curdir;
        #
        # Do not copy known garbage
        #
        next if $_ eq 'cmake_install.cmake';
        next if $_ eq 'Makefile';
        next if $_ eq 'CMakeFiles';
        next if $_ eq 'output';
        next if $_ eq 'CTestTestfile.cmake';
        next if $_ eq '.gitignore';
        next if $_ eq '.gitattributes';
        #
        # Do not copy known stuff we can skip
        #
        next if $_ eq 'cmake';
        next if $_ eq 'cmake-utils-remote';
        next if $_ eq 'test';
        next if $_ eq 'blog';
        next if $_ eq 't';
        next if $_ eq 'html';
        next if $_ eq 'inc';
        next if $_ eq 'lib';
        next if $_ eq 'm4';
        next if $_ eq 'pod';
        next if $_ eq 'etc';
        next if $_ eq 'xs';
        next if $_ eq 'author.t';
        next if $_ eq '.travis.yml';
        next if $_ eq 'visual-studio';
        next if $_ =~ /\..?sh$/;

        my $mode = $stats->{$_}->[2];

        if (S_ISDIR($mode)) {
            # if ($basedir eq 'github') {
            #     _dircopy(File::Spec->catdir($srcdir, $_), File::Spec->catdir($dstdir, $reducedLength++));
            # } else {
                _dircopy(File::Spec->catdir($srcdir, $_), File::Spec->catdir($dstdir, $_));
            # }
        } else {
            my $file = File::Spec->catfile($srcdir, $_);
            print "... Copying file $file to $dstdir\n";
            _copy($srcdir, $dstdir, $_);
        }
    }
}

sub _chdir {
    my ($dir) = @_;

    my ($volume, $directories, $file) = File::Spec->splitpath($dir, 1);
    my @dirs = File::Spec->splitdir($directories);
    my @donedirs = ();
    my $donedirs = '';
    foreach (@dirs) {
        push(@donedirs, $_);
        $donedirs = File::Spec->catdir(@donedirs);
        chdir($_) || die "Cannot chdir to $donedirs, $!";
    }
}

sub _readdir {
    my ($dir) = @_;

    my $workdir = cwd;

    _chdir($dir);

    my $dh;
    opendir($dh, $curdir) || die "Failed to opendir $dir, $!";
    my %stats = ();
    while (readdir($dh)) {
        my @stat = stat($_);
        if (! @stat) {
            warn "Failed to stat entry in $dir: $_";
        } else {
            $stats{$_} = \@stat;
        }
    }
    closedir($dh) || warn "Failed to closedir $dir, $!";

    chdir($workdir) || die "Failed to chdir to $workdir, $!";
    return \%stats;
}

sub _copy {
    my ($srcdir, $dstdir, $entry) = @_;

    my $srcfile = File::Spec->catfile($srcdir, $entry);
    my $dstfile = File::Spec->catfile($dstdir, $entry);

    #
    # All files I copy are short -;
    #
    my $workdir = cwd;
    _chdir($srcdir);
    open(my $in, '<', $entry) || die "Cannot open $srcfile, $!";   

    chdir($workdir) || die "Failed to chdir to $workdir, $!";
    _chdir($dstdir);

    open(my $out, '>', $entry) || die "Cannot open $dstfile, $!";
    chdir($workdir) || die "Failed to chdir to $workdir, $!";

    #
    #
    my $data = do { local $/; <$in> };
    print $out $data;
    close($in) || warn "Failed to close $srcfile, $!";
    close($out) || warn "Failed to close $dstfile, $!";
}
