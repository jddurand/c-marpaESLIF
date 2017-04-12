#!env perl

package main;
use strict;
use diagnostics;
use Log::Any qw/$log/;
use Log::Any::Adapter qw/Stderr/;
use MarpaX::ESLIF;

my $eslif = MarpaX::ESLIF->new($log);
my $grammar_v1 = q{
      Expression ::=
          /[\d]+/
          | '(' Expression ')'              assoc => group
         ||     Expression '**' Expression  assoc => right
         ||     Expression  '*' Expression
          |     Expression  '/' Expression
         ||     Expression  '+' Expression
          |     Expression  '-' Expression
      };
my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar_v1);

package MyRecognizer;
use strict;
use diagnostics;
#
# Constructor
#
sub new {
  my ($pkg, $string) = @_;
  open my $fh, "<", \$string;
  bless { data => undef, fh => $fh }, $pkg
}

#
# Required methods
#
sub read                   {
  my ($self) = @_;   # read data
  defined($self->{data} = readline($self->{fh}))
}
sub isEof                  {  eof shift->{fh} } # End of data ?
sub isCharacterStream      {                1 } # Character stream ?
sub encoding               {                  } # Encoding ?
sub data                   {    shift->{data} } # data
sub isWithDisableThreshold {                0 } # Disable threshold warning ?
sub isWithExhaustion       {                0 } # Exhaustion event ?
sub isWithNewline          {                1 } # Newline count ?

package MyValue;
use strict;
use diagnostics;
#
# Constructor
#
sub new { bless { result => undef}, shift }

#
# Required methods
#
sub isWithHighRankOnly { 1 }  # When there is the rank adverb: highest ranks only ?
sub isWithOrderByRank  { 1 }  # When there is the rank adverb: order by rank ?
sub isWithAmbiguous    { 0 }  # Allow ambiguous parse ?
sub isWithNull         { 0 }  # Allow null parse ?
sub maxParses          { 0 }  # Maximum number of parse tree values
#
# ... result getter and setter
#
sub getResult          { my ($self) = @_; $self->{result} }
sub setResult          { my ($self, $result) = @_; $self->{result} = $result }

package main;

my $input = '(1+2)*3';
my $eslifRecognizerInterface = MyRecognizer->new($input);
my $eslifValueInterface = MyValue->new();

my $result = $eslifGrammar->parse($eslifRecognizerInterface, $eslifValueInterface) ? $eslifValueInterface->getResult : '??';
printf "Default parse tree value of $input: %s\n", $result;

my $grammar_v2 = $grammar_v1 . q{
      :discard ::= /[\s]+/
      :discard ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/
      :discard ::= /#[^\n]*(?:\n|\z)/
      };
$eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar_v2);
$input = q{( /* C comment */1+2)
# perl comment
*3};
$eslifRecognizerInterface = MyRecognizer->new($input);
$eslifValueInterface = MyValue->new();
$result = $eslifGrammar->parse($eslifRecognizerInterface, $eslifValueInterface) ? $eslifValueInterface->getResult : '??';
printf "Default parse tree value of $input: %s\n", $result;

package MyRecognizer;
no warnings 'redefine';
sub read {
  my ($self) = @_;   # read data
  CORE::read($self->{fh}, $self->{data}, 1) ? 1 : 0x
}

#package MyRecognizer;
#no warnings 'redefine';
#sub encoding               { 'ASCII' } # Encoding ?

package MyValue;
sub do_pow   { my ($self, $left, $op, $right) = @_; $left**$right }
sub do_mul   { my ($self, $left, $op, $right) = @_; $left*$right }
sub do_div   { my ($self, $left, $op, $right) = @_; $left/$right }
sub do_plus  { my ($self, $left, $op, $right) = @_; $left+$right }
sub do_minus { my ($self, $left, $op, $right) = @_; $left-$right }

package main;
my $grammar_v3 = q{
  Expression ::=
      /[\d]+/
      | '(' Expression ')'              assoc => group action => ::copy[1]
     ||     Expression '**' Expression  assoc => right action => do_pow
     ||     Expression  '*' Expression                 action => do_mul
      |     Expression  '/' Expression                 action => do_div
     ||     Expression  '+' Expression                 action => do_plus
      |     Expression  '-' Expression                 action => do_minus
  :discard ::= /[\s]+/
  };
$eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar_v3);
$input = q{(1 + 2) * 3};
$eslifRecognizerInterface = MyRecognizer->new($input);
$eslifValueInterface = MyValue->new();
$result = $eslifGrammar->parse($eslifRecognizerInterface, $eslifValueInterface) ? $eslifValueInterface->getResult : '??';
printf "Default parse tree value of $input: %s\n", $result;

$input = q{(1 + 2) * 3 + ( abcdef};
# Remember that we are using the 'read-one-character-per-character' implementation
$eslifRecognizerInterface = MyRecognizer->new($input);
$eslifValueInterface = MyValue->new();
$eslifGrammar->parse($eslifRecognizerInterface, $eslifValueInterface);

$input = q{(1 + 2) * 3};
# Our usual ESLIF Recognizer interface
$eslifRecognizerInterface = MyRecognizer->new($input);
# ESLIF Recognizer engine
my $eslifRecognizer = MarpaX::ESLIF::Recognizer->new($eslifGrammar, $eslifRecognizerInterface);
#
# Start scanning the input, we want initial events here
#
$eslifRecognizer->scan(1);
my $eventsRef = $eslifRecognizer->events();
use Data::Dumper;
print "Events after scan():\n" . Dumper($eventsRef);

my $grammar_v4 = $grammar_v3 . q{
  event ^Expression = predicted Expression
  };
$eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar_v4);
$eslifRecognizer = MarpaX::ESLIF::Recognizer->new($eslifGrammar, $eslifRecognizerInterface);
$eslifRecognizer->scan(1);
$eventsRef = $eslifRecognizer->events();
print "Events after scan():\n" . Dumper($eventsRef);
use MarpaX::ESLIF::Event::Type;
printf "MARPAESLIF_EVENTTYPE_PREDICTED is: %d\n", MarpaX::ESLIF::Event::Type->MARPAESLIF_EVENTTYPE_PREDICTED; # 4
