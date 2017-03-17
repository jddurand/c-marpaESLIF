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
log4perl.rootLogger              = WARN, Screen
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
    
