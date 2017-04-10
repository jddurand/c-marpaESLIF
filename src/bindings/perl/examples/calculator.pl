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
my $eslifRecognizer = MyRecognizer->new($input);
my $eslifValue = MyValue->new();

my $result = $eslifGrammar->parse($eslifRecognizer, $eslifValue) ? $eslifValue->getResult : '??';
printf "Default parse tree value of $input: %s\n", $result;
exit;

my $grammar_v2 = $grammar_v1 . q{
      :discard ::= /[\s]+/
      :discard ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/
      :discard ::= /#[^\n]*(?:\n|\z)/
      };
$eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $grammar_v2);
$input = q{( /* C comment */1+2)
# perl comment
*3};
$eslifRecognizer = MyRecognizer->new($input);
$eslifValue = MyValue->new();
$result = $eslifGrammar->parse($eslifRecognizer, $eslifValue) ? $eslifValue->getResult : '??';
printf "Default parse tree value of $input: %s\n", $result;

package MyRecognizer;
no warnings 'redefine';
sub read {
  my ($self) = @_;   # read data
  CORE::read($self->{fh}, $self->{data}, 2) ? 1 : 0x
}

package MyRecognizer;
no warnings 'redefine';
sub encoding               { 'ASCII' } # Encoding ?


