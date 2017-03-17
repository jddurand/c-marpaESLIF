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
if (-d 'src') {
    print "... Removing src\n";
    rmtree('src');
}

my ($src, $dst);

print "... Creating directory src\n";
mkdir('src') || die "Failed to create src directory, $!";

print "... Creating directory src/src\n";
$src = File::Spec->catdir('src', 'src');
mkdir($src) || die "Failed to create $src directory, $!";

$src = File::Spec->catdir(($updir) x 3, '3rdparty');
$dst = File::Spec->catdir('src', '3rdparty');
print "... Copying directory $src to $dst\n",
_dircopy($src, $dst);

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
     }, 'src');
foreach (@toremove) {
  if (-d $_) {
    print "... Removing $_\n";
    rmtree($_);
  } elsif (-f $_) {
    print "... Removing $_\n";
    unlink($_);
  }
}

$src = File::Spec->catdir(($updir) x 3, 'include');
$dst = File::Spec->catdir('src', 'include');
print "... Copying directory $src to $dst\n",
_dircopy($src, $dst);

$src = File::Spec->catdir(($updir) x 3, 'cmake');
$dst = File::Spec->catdir('src', 'cmake');
print "... Copying directory $src to $dst\n",
_dircopy($src, $dst);

$src = File::Spec->catfile(($updir) x 3, 'CMakeObjects.PL');
$dst = File::Spec->catfile('src', 'CMakeObjects.PL');
print "... Copying file $src to $dst\n";
copy($src, $dst)|| die "$!";

$src = File::Spec->catfile(($updir) x 3, 'config_autoconf.ini');
$dst = File::Spec->catfile('src', 'config_autoconf.ini');
print "... Copying file $src to $dst\n";
copy($src, $dst)|| die "$!";

$src = File::Spec->catfile(($updir) x 3, 'CMakeLists.txt');
$dst = File::Spec->catfile('src', 'CMakeLists.txt');
print "... Copying file $src to $dst\n";
copy($src, $dst)|| die "$!";

foreach (qw/bootstrap_actions.c marpaESLIF.c/) {
  $src = File::Spec->catfile(($updir) x 3, 'src', $_);
  $dst = File::Spec->catfile('src', 'src', $_);
  print "... Copying file $src to $dst\n";
  copy($src, $dst)|| die "$!";
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
        next if $_ eq 'cmake-utils';
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
    binmode($in) || die "Failed to set binary mode on $srcfile, $!";

    chdir($workdir) || die "Failed to chdir to $workdir, $!";
    _chdir($dstdir);

    open(my $out, '>', $entry) || die "Cannot open $dstfile, $!";
    binmode($out) || die "Failed to set binary mode on $dstfile, $!";
    chdir($workdir) || die "Failed to chdir to $workdir, $!";

    #
    #
    my $data = do { local $/; <$in> };
    print $out $data;
    close($in) || warn "Failed to close $srcfile, $!";
    close($out) || warn "Failed to close $dstfile, $!";
}
