package MyRecognizer;
use strict;
use diagnostics;

sub new {
    my ($pkg, $string, $log) = @_;
    open my $fh, "<", \$string;
    return bless { line => undef, fh => $fh, log => $log }, $pkg;
}


sub read {
    my $self = shift;
    $self->{log}->trace("read");
    $self->{line} = readline($self->{fh});
}

sub isEof {
    my $self = shift;
    $self->{log}->trace("isEof");
    return eof($self->{fh});
}

sub isCharacterStream {
    my ($self) = @_;
    $self->{log}->trace("isCharacterStream");
    return 1;
}

sub encoding {
    my ($self) = @_;
    $self->{log}->trace("encoding");
    return;
}

sub data {
    my $self = shift;
    $self->{log}->trace("data");
    return $self->{line};
}

sub isWithDisableThreshold {
    my ($self) = @_;
    $self->{log}->trace("isWithDisableThreshold");
    return 0;
}

sub isWithExhaustion {
    my ($self) = @_;
    $self->{log}->trace("isWithExhaustion");
    return 0;
}

sub isWithNewline {
    my ($self) = @_;
    $self->{log}->trace("isWithNewline");
    return 1;
}

package MyValue;
use strict;
use diagnostics;
use Carp qw/croak/;
#
# In our example we have NOT specified a symbol action, therefore
# lexemes that come directly from the grammar are exactly what is in the input
#*/

sub new {
    my ($pkg, $log) = @_;

    return bless { result => undef, log => $log }, $pkg;
}
	
sub do_int {
    my ($self, $number) = @_;

    $self->{log}->tracef("do_int(%s)", $number);
    return int($number);
}

sub do_op {
    my ($self, $left, $op, $right) = @_;

    $self->{log}->tracef("do_op(%s, %s, %s)", $left, $op, $right);

    my $result;
    if ($op eq '**') {
        $result = $left ** $right;
    }
    elsif ($op eq '*') {
        $result = $left * $right;
    }
    elsif ($op eq '/') {
        $result = $left / $right;
    }
    elsif ($op eq '+') {
        $result = $left + $right;
    }
    elsif ($op eq '-') {
        $result = $left - $right;
    }
    else {
        croak "Unsupported op $op";
    }

    return $result;
}

sub isWithHighRankOnly {
    my ($self) = @_;
    $self->{log}->trace("isWithHighRankOnly");
    return 1;
}

sub isWithOrderByRank {
    my ($self) = @_;
    $self->{log}->trace("isWithOrderByRank");
    return 1;
}

sub isWithAmbiguous {
    my ($self) = @_;
    $self->{log}->trace("isWithAmbiguous");
    return 0;
}

sub isWithNull {
    my ($self) = @_;
    $self->{log}->trace("isWithNull");
    return 0;
}

sub maxParses {
    my ($self) = @_;
    $self->{log}->trace("maxParses");
    return 0;
}

sub getResult {
    my ($self) = @_;
    $self->{log}->trace("result");
    return $self->{result};
}

sub setResult {
    my ($self, $result) = @_;
    $self->{log}->tracef("setResult(%s)", $result);
    $self->{result} = $result;
}

package main;
use strict;
use warnings FATAL => 'all';
use Test::More;
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

my $eslif = MarpaX::ESLIF->new($log);
isa_ok($eslif, 'MarpaX::ESLIF');
my $version = $eslif->version;
ok (defined($version), "Library version is defined (currently: $version)");

my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, do { local $/; <DATA> } );
isa_ok($eslifGrammar, 'MarpaX::ESLIF::Grammar');
my $ngrammar = $eslifGrammar->ngrammar;
ok($ngrammar > 0, "Number of grammars is > 0");
my $currentLevel = $eslifGrammar->currentLevel;
ok($currentLevel >= 0, "Current level is >= 0");

my $currentDescription = $eslifGrammar->currentDescription;
ok($currentDescription ne '', "Current description is not empty");
diag("Current Description: $currentDescription");
foreach my $level (0..$ngrammar-1) {
    my $descriptionByLevel = $eslifGrammar->descriptionByLevel($level);
    ok($descriptionByLevel ne '', "Description of level $level is not empty");
    diag("Description of level $level: $descriptionByLevel");
}

my $currentRuleIds = $eslifGrammar->currentRuleIds;
ok($#{$currentRuleIds} >= 0, "Number of current rules is > 0");
diag("Current Rule Ids: @{$currentRuleIds}");
foreach my $ruleId (0..$#{$currentRuleIds}) {
    my $ruleDisplay = $eslifGrammar->ruleDisplay($currentRuleIds->[$ruleId]);
    ok($ruleDisplay ne '', "Display of rule No " . $currentRuleIds->[$ruleId]);
    diag("Display of rule No $ruleId: $ruleDisplay");
    my $ruleShow = $eslifGrammar->ruleShow($currentRuleIds->[$ruleId]);
    ok($ruleShow ne '', "Show of rule No " . $currentRuleIds->[$ruleId]);
    diag("Show    of rule No $ruleId: $ruleShow");
}

foreach my $level (0..$ngrammar-1) {
    my $ruleIdsByLevel = $eslifGrammar->ruleIdsByLevel($level);
    ok($#{$ruleIdsByLevel} >= 0, "Number of rules at level $level is > 0");
    diag("Rule Ids at level $level: @{$ruleIdsByLevel}");

    foreach my $ruleId (0..$#{$ruleIdsByLevel}) {
        my $ruleDisplayByLevel = $eslifGrammar->ruleDisplayByLevel($level, $ruleIdsByLevel->[$ruleId]);
        ok($ruleDisplayByLevel ne '', "Display of rule No " . $ruleIdsByLevel->[$ruleId] . " of level $level");
        diag("Display of rule No " . $ruleIdsByLevel->[$ruleId] . " of level $level: $ruleDisplayByLevel");

        my $ruleShowByLevel = $eslifGrammar->ruleShowByLevel($level, $ruleIdsByLevel->[$ruleId]);
        ok($ruleShowByLevel ne '', "Show of rule No " . $ruleIdsByLevel->[$ruleId] . " of level $level");
        diag("Show    of rule No " . $ruleIdsByLevel->[$ruleId] . " of level $level: $ruleShowByLevel");
    }
}

my $show = $eslifGrammar->show;
ok($show ne '', "Show of current grammar");
diag("Show of current grammar:\n$show");
foreach my $level (0..$ngrammar-1) {
    my $showByLevel = $eslifGrammar->showByLevel($level);
    ok($show ne '', "Show of grammar at level $level");
    diag("Show of grammar at level $level:\n$showByLevel");
}

my @strings = (
    "(((3 * 4) + 2 * 7) / 2 - 1)/* This is a\n comment \n */** 3",
    "5 / (2 * 3)",
    "5 / 2 * 3",
    "(5 ** 2) ** 3",
    "5 * (2 * 3)",
    "5 ** (2 ** 3)",
    "5 ** (2 / 3)",
    "1 + ( 2 + ( 3 + ( 4 + 5) )",
    "1 + ( 2 + ( 3 + ( 4 + 50) ) )   /* comment after */",
    " 100"
    );

for (my $i = 0; $i <= $#strings; $i++) {
    my $string = $strings[$i];

    $log->infof("Testing parse() on %s", $string);
    my $recognizerInterface = MyRecognizer->new($string, $log);
    my $valueInterface = MyValue->new($log);
    if ($eslifGrammar->parse($recognizerInterface, $valueInterface)) {
        $log->infof("Result: %s", $valueInterface->getResult);
    }
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
    
