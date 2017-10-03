package MyRecognizerInterface;
use strict;
use diagnostics;

sub new                    { my ($pkg, $string) = @_; bless { string => $string }, $pkg }
sub read                   { 1 }
sub isEof                  { 1 }
sub isCharacterStream      { 1 }
sub encoding               { }
sub data                   { $_[0]->{string} }
sub isWithDisableThreshold { 0 }
sub isWithExhaustion       { 0 }
sub isWithNewline          { 1 }
sub isWithTrack            { 1 }

package MyValueInterface;
use strict;
use diagnostics;

sub new                { my ($pkg) = @_; bless { result => undef }, $pkg }
sub isWithHighRankOnly { 1 }
sub isWithOrderByRank  { 1 }
sub isWithAmbiguous    { 0 }
sub isWithNull         { 0 }
sub maxParses          { 0 }
sub getResult          { $_[0]->{result} }
sub setResult          { $_[0]->{result} = $_[1] }

package main;
use strict;
use warnings FATAL => 'all';

use Log::Log4perl qw/:easy/;
use Log::Any::Adapter;
use Log::Any qw/$log/;

BEGIN {
  use Config;
  if (! $Config{usethreads}) {
    print("1..0 # Skip: No threads\n");
    exit(0);
  } else {
    eval 'require threads; threads->import(); require threads::shared; threads::shared->import(); 1' || die "$@"
  }
}
#
# As per the doc, Test::More is thread-aware only if threads is loaded before - so we synchronize it
#
BEGIN {
    eval 'use Test::More; 1' || die "$@";
    require_ok('MarpaX::ESLIF');
}

#
# Init log
#
our $defaultLog4perlConf = '
log4perl.rootLogger              = TRACE, Screen
log4perl.appender.Screen         = Log::Log4perl::Appender::Screen
log4perl.appender.Screen.stderr  = 0
log4perl.appender.Screen.layout  = PatternLayout
log4perl.appender.Screen.layout.ConversionPattern = %d %-5p %6P %m{chomp}%n
';
Log::Log4perl::init(\$defaultLog4perlConf);
Log::Any::Adapter->set('Log4perl');

my $grammar = q{
Expression ::=
    /[\d]+/
    | '(' Expression ')'              assoc => group
   ||     Expression '**' Expression  assoc => right
   ||     Expression  '*' Expression
    |     Expression  '/' Expression
   ||     Expression  '+' Expression
    |     Expression  '-' Expression
};

my $count = 0;
share($count);

sub thr_sub {
  my ($input, $expected, $logger) = @_;

  my $tid = threads->tid();
  # printf STDERR "Thread No %d is starting\n", $tid;
  {
    lock($count);
    # diag(sprintf("Thread No %d is starting", $tid));
    ++$count;
  }
  #
  # Do a random sleep - https://alvinalexander.com/perl/perl-random-number-integer-rand-function
  #
  # my $lower_limit = 1;
  # my $upper_limit = 3;
  # my $random_number = int(rand($upper_limit-$lower_limit)) + $lower_limit;
  # sleep($random_number) if $random_number;

  my $eslif = MarpaX::ESLIF->multiton($logger);
  my $eslif2 = MarpaX::ESLIF->multiton($logger);
  ok($eslif == $eslif2, "Thread No $tid - multiton $eslif == $eslif2");
  my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar);
  my $eslifRecognizerInterface = MyRecognizerInterface->new($input);
  my $eslifValueInterface = MyValueInterface->new();

  $eslifGrammar->parse($eslifRecognizerInterface, $eslifValueInterface);
  my $value = $eslifValueInterface->getResult;
  is($value, $expected, "Thread No $tid - value $value, expected value $expected");
  {
    lock($count);
    # diag(sprintf("Thread No %d is ending", $tid));
    --$count;
  }
  # printf STDERR "Thread No %d is ending\n", $tid;
}

my $NTHREAD = 15;

my $input = '(1+2)*3';
my $expected = '(1+2)*3';
my @t = map {
  my $logger = $_ ? $log : undef;
  threads->create(\&thr_sub, $input, $expected, $logger)->detach
} (1..$NTHREAD);
#
# Time to at least one thread to start
#
while (1) {
  sleep(1);
  lock($count);
  last unless $count
}

done_testing();

1;
