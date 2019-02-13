package MyRecognizerInterface;
use strict;
use diagnostics;

sub new                    { my ($pkg, $data, $characterStream) = @_; bless { data => $data, characterStream => $characterStream }, $pkg }
sub read                   { 1 }
sub isEof                  { 1 }
sub isCharacterStream      { $_[0]->{characterStream} // 1 }
sub encoding               { }
sub data                   { "".$_[0]->{data} }
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
sub input              { $_[0]->{data} = $_[1] }
sub output             { $_[0]->{data} }

package main;
use strict;
use warnings FATAL => 'all';
use Log::Log4perl qw/:easy/;
use Log::Any::Adapter;
use Log::Any qw/$log/;
use Math::BigInt;
use Math::BigFloat;

our @input;
BEGIN {
    use utf8;
    @input =
        (
         "XXX",
         ["\xa0\xa1", 0],
         ["\xf0\x28\x8c\x28", 0],
         "Ḽơᶉëᶆ ȋṕšᶙṁ ḍỡḽǭᵳ ʂǐť ӓṁệẗ, ĉṓɲṩḙċťᶒțûɾ ấɖḯƥĭṩčįɳġ ḝłįʈ, șếᶑ ᶁⱺ ẽḭŭŝḿꝋď ṫĕᶆᶈṓɍ ỉñḉīḑȋᵭṵńť ṷŧ ḹẩḇőꝛế éȶ đꝍꞎôꝛȇ ᵯáꞡᶇā ąⱡîɋṹẵ.",
         0,
         1,
         2.34,
         1.6e+308,
         Math::BigFloat->new("6.78E+9"),
         Math::BigInt->new("6.78E+9")
        );
}
use Test::More tests => 1 + scalar(@input);
use Test::More::UTF8;
use open qw( :utf8 :std );

BEGIN { require_ok('MarpaX::ESLIF') }

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
Output ::= LuaProxy action => output
LuaProxy ::= Input action => ::lua->lua_proxy
Input ::= /.+/   action => input

<luascript>
  io.stdout:setvbuf 'no'

  function lua_proxy(input)
    io.write(string.format('In lua_proxy\n'))
    return input
  end
</luascript>
};

my $eslif = MarpaX::ESLIF->new($log);
my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar);

foreach my $inputArray (@input) {
    my ($input, $characterStream) = (ref($inputArray) || '') eq 'ARRAY' ? @{$inputArray} : ($inputArray, 1);
    my $eslifRecognizerInterface = MyRecognizerInterface->new($input, $characterStream);
    my $eslifValueInterface = MyValueInterface->new();
    $eslifGrammar->parse($eslifRecognizerInterface, $eslifValueInterface);
    my $value = $eslifValueInterface->getResult;
    is($value, $input, "$value == $input (" . (ref($input) || 'scalar') . ")");
}

done_testing();

1;
