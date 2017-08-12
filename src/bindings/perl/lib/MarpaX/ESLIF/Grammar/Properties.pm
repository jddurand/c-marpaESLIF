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

=head1 METHODS

=head2 MarpaX::ESLIF::Grammar::Properties->new(%args)

Creation of an ESLIFGrammarProperties instance, noted C<$self> afterwards. C<%args> is a hash that should contain:

=over

=item level

Grammar level

=item maxLevel

Maximum grammar level

=item description

Grammar description

=item latm

Grammar is in LATM (Longest Accepted Token Mode) ?

=item defaultSymbolAction

Grammar default symbol action

=item defaultRuleAction

Grammar default rule action

=item defaultFreeAction

Grammar default free action

=item startId

Start symbol Id

=item discardId

Discard symbol Id

=item symbolIds

Symbol Ids (array reference)

=item ruleIds

Rule Ids (array reference)

=back

=cut

sub new {
    my ($pkg, %args) = @_;
    #
    # This should be called ONLY by the XS, and we know what we do
    #
    return bless { level               => $args{level},
                   maxLevel            => $args{maxLevel},
                   description         => $args{description},
                   latm                => $args{latm},
                   defaultSymbolAction => $args{defaultSymbolAction},
                   defaultRuleAction   => $args{defaultRuleAction},
                   defaultFreeAction   => $args{defaultFreeAction},
                   startId             => $args{startId},
                   discardId           => $args{discardId},
                   symbolIds           => $args{symbolIds},
                   ruleIds             => $args{ruleIds}
                   }, $pkg
}

#
# Explicit getters - XS and this file are in sync, fallbacks to undef value if not
#

=head2 $self->getLevel

Returns grammar's level

=cut

sub getLevel {
  my ($self) = @_;

  return $self->{level}
}

=head2 $self->getMaxLevel

Returns maximum grammar's level

=cut

sub getMaxLevel {
  my ($self) = @_;

  return $self->{maxLevel}
}

=head2 $self->getDescription

Returns grammar's description

=cut

sub getDescription {
  my ($self) = @_;

  return $self->{description}
}

=head2 $self->isLatm

Returns a boolean that indicates if this grammar is in the LATM (Longest Acceptable Token Mode) or not

=cut

sub isLatm {
  my ($self) = @_;

  return $self->{latm}
}

=head2 $self->getLatm

Alias to isLatm()

=cut

sub getLatm {
  goto &isLatm
}

=head2 $self->getDefaultSymbolAction

Returns grammar's default symbol action, never null

=cut

sub getDefaultSymbolAction {
  my ($self) = @_;

  return $self->{defaultSymbolAction}
}

=head2 $self->getDefaultRuleAction

Returns grammar's default rule action, can be null

=cut

sub getDefaultRuleAction {
  my ($self) = @_;

  return $self->{defaultRuleAction}
}

=head2 $self->getDefaultFreeAction

Returns grammar's default free action, never null and always ":defaultFreeActions" (hardcoded in the JNI interface)

=cut

sub getDefaultFreeAction {
  my ($self) = @_;

  return $self->{defaultFreeAction}
}

=head2 $self->getStartId

Returns grammar's start symbol id, always >= 0

=cut

sub getStartId {
  my ($self) = @_;

  return $self->{startId}
}

=head2 $self->getDiscardId

Returns grammar's discard symbol id, < 0 if none.

=cut

sub getDiscardId {
  my ($self) = @_;

  return $self->{discardId}
}

=head2 $self->getSymbolIds

Returns a reference to a list of symbol identifiers

=cut

sub getSymbolIds {
  my ($self) = @_;

  return $self->{symbolIds}
}

=head2 $self->getRuleIds

Returns a reference to a list of rule identifiers

=cut

sub getRuleIds {
  my ($self) = @_;

  return $self->{ruleIds}
}

1;
