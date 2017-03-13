use strict;
use warnings FATAL => 'all';

package MyLogger;
use Moo;
use Log::Log4perl qw/:easy/;
use Log::Any::Adapter;
use Log::Any qw/$log/;
#
# Init log
#
our $defaultLog4perlConf = '
log4perl.rootLogger              = WARN, Screen
log4perl.appender.Screen         = Log::Log4perl::Appender::Screen
log4perl.appender.Screen.stderr  = 0
log4perl.appender.Screen.layout  = PatternLayout
log4perl.appender.Screen.layout.ConversionPattern = %d %-5p %6P %m{chomp}%n
';
Log::Log4perl::init(\$defaultLog4perlConf);
Log::Any::Adapter->set('Log4perl');

sub trace     { my ($self, @args) = @_; $log->trace    (@args) }
sub debug     { my ($self, @args) = @_; $log->debug    (@args) }
sub info      { my ($self, @args) = @_; $log->info     (@args) }
sub warning   { my ($self, @args) = @_; $log->warning  (@args) }
sub error     { my ($self, @args) = @_; $log->error    (@args) }
sub critical  { my ($self, @args) = @_; $log->critical (@args) }
sub alert     { my ($self, @args) = @_; $log->alert    (@args) }
sub emergency { my ($self, @args) = @_; $log->emergency(@args) }

with 'MarpaX::ESLIF::Logger::Interface';

package main;
use Test::More;
BEGIN { require_ok('MarpaX::ESLIF') };
BEGIN { require_ok('MarpaX::ESLIF::Grammar') };

my $eslif = MarpaX::ESLIF->new(loggerInterface => MyLogger->new());
isa_ok($eslif, 'MarpaX::ESLIF');
my $version = $eslif->version;
ok (defined($version), "Library version is defined (currently: $version)");

my $eslifGrammar = MarpaX::ESLIF::Grammar->new(eslif => $eslif, grammar => do { local $/; <DATA> } );
isa_ok($eslifGrammar, 'MarpaX::ESLIF::Grammar');
my $ngrammar = $eslifGrammar->ngrammar;
ok($ngrammar > 0, "Number of grammars is > 0 (currently: $ngrammar)");
my $currentLevel = $eslifGrammar->currentLevel;
ok($currentLevel >= 0, "Current level is >= 0 (currently: $currentLevel)");
my $currentDescription = $eslifGrammar->currentDescription;
ok($currentDescription ne '', "Current description is not empty");
diag($currentDescription);
foreach (0..$ngrammar) {
    my $descriptionByLevel = $eslifGrammar->descriptionByLevel($_);
    ok($descriptionByLevel ne '', "Description of level $_ is not empty");
    diag($descriptionByLevel);
}

done_testing();

1;

__DATA__
:start   ::= Expression
:default ::=             action => do_op
:discard ::= whitespaces event  => discard_whitespaces$
:discard ::= comment     event  => discard_comment$

event ^Number = predicted Number
event Number$ = completed Number
Number   ::= NUMBER   action => ::shift

event Expression$ = completed Expression
event ^Expression = predicted Expression
Expression ::=
    Number                                           action => do_int
    | '(' Expression ')'              assoc => group action => ::copy[1]
   ||     Expression '**' Expression  assoc => right
   ||     Expression  '*' Expression
    |     Expression  '/' Expression
   ||     Expression  '+' Expression
    |     Expression  '-' Expression

:lexeme ::= NUMBER pause => before event => ^NUMBER
:lexeme ::= NUMBER pause => after  event => NUMBER$
NUMBER     ~ /[\d]+/
whitespaces ::= WHITESPACES
WHITESPACES ~ [\s]+
comment ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/u
    
