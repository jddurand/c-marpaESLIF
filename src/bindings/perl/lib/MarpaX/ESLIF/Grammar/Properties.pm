use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Grammar::Properties;

# ABSTRACT: ESLIF Grammar Properties

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIF Grammar Properties.

Calls to grammar's currentProperties() or propertiesByLevel() methods outputs an instance of this class.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new();
  my $data = do { local $/; <DATA> };
  my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $data);
  my $grammarProperties = $eslifGrammar->currentProperties;
  my $grammarPropertiesByLevel = $eslifGrammar->propertiesByLevel(0);

  __DATA__
  #
  # This is an example of a calculator grammar
  #
  :start   ::= Expression
  :default ::=             action        => do_op
                           symbol-action => do_symbol
                           free-action   => do_free     # Supported but useless
  :desc    ::= 'Calculator'
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

  whitespaces ::= WHITESPACES
  comment ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/u

  :lexeme ::= NUMBER pause => before event => ^NUMBER
  :lexeme ::= NUMBER pause => after  event => NUMBER$

  :desc      ~ 'Calculator Tokens'
  NUMBER     ~ /[\d]+/   name => 'NUMBER Lexeme'
  WHITESPACES ~ [\s]+    name => 'WHITESPACES Lexeme'

=cut

#
# old-style perl - getters only in java style
#
my @MEMBERS = qw/level
        description
        latm
        defaultSymbolAction
        defaultRuleAction
        defaultFreeAction
        startId
        discardId
        symbolIds
        ruleIds/;

sub new {
    my ($pkg, %args) = @_;

    return bless \%args, $pkg
}

#
# Explicit getters - XS and this file are in sync, fallbacks to undef value if not
#
foreach my $member (@MEMBERS) {
    my $getter = 'get' . ucfirst($member);
    eval "sub $getter { return \$_[0]->{$member} }" ## no critic
}

1;
