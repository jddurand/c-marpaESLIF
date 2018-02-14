#
# This file is adapted from MarpaX::ESLIF::ECMA404
#
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
use Test::More;
use Test::More::UTF8;
use Log::Log4perl qw/:easy/;
use Log::Any::Adapter;
use Log::Any qw/$log/;

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

BEGIN { require_ok('MarpaX::ESLIF') };

my $base_dsl = q{
:default ::= action => ::shift
:start       ::= XXXXXX # Replaced on-the-fly by json or members
json         ::= object
               | array
object       ::= LCURLY members RCURLY
               | OBJECT_FROM_INNER_GRAMMAR
members      ::= pair*                 action => do_array separator => ','
pair         ::= string ':' value      action => do_array
value        ::= string
               | object
               | number
               | array
               | 'true'                action => do_true
               | 'false'               action => do_true
               | 'null'                action => ::undef
array        ::= '[' ']'               action => do_empty_array
               | '[' elements ']'
elements     ::= value+                action => do_array separator => ','
number         ~ int
               | int frac
               | int exp
               | int frac exp
int            ~ digits
               | '-' digits
digits         ~ [\d]+
frac           ~ '.' digits
exp            ~ e digits
e              ~ 'e'
               | 'e+'
               | 'e-'
               | 'E'
               | 'E+'
               | 'E-'
string       ::= lstring
:lexeme ::= lstring pause => after event => lstring$
lstring        ~ quote in_string quote
quote          ~ [\"]
in_string      ~ in_string_char*
in_string_char  ~ [^"] | '\"'
:discard       ::= whitespace
whitespace     ~ [\s]+
:lexeme ::= LCURLY pause => before event => ^LCURLY
LCURLY         ~ '{'
:lexeme ::= RCURLY pause => before event => ^RCURLY
RCURLY         ~ '}'
OBJECT_FROM_INNER_GRAMMAR ~ [^\s\S]
};

my $eslif = MarpaX::ESLIF->new($log);
isa_ok($eslif, 'MarpaX::ESLIF');

done_testing();


