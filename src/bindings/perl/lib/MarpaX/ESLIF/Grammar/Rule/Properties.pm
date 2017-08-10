use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Grammar::Rule::Properties;

# ABSTRACT: ESLIF Grammar Rule Properties

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIF Grammar Rule Properties.

Calls to grammar's currentRuleProperties() or rulePropertiesByLevel() methods outputs an instance of this class.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new();
  my $data = do { local $/; <DATA> };
  my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $data);
  my $ruleProperties = $eslifGrammar->ruleProperties(0);
  my $rulePropertiesByLevel = $eslifGrammar->rulePropertiesByLevel(0, 0);

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
sub new {
    my ($pkg, %args) = @_;

    my $self = {
        'id'                       => $args{id},
        'description'              => $args{description},
        'show'                     => $args{show},
        'lhsId'                    => $args{lhsId},
        'separatorId'              => $args{separatorId},
        'rhsIds'                   => $args{rhsIds},
        'exceptionId'              => $args{exceptionId},
        'action'                   => $args{action},
        'discardEvent'             => $args{discardEvent},
        'discardEventInitialState' => $args{discardEventInitialState},
        'rank'                     => $args{rank},
        'nullRanksHigh'            => $args{nullRanksHigh},
        'sequence'                 => $args{sequence},
        'proper'                   => $args{proper},
        'minimum'                  => $args{minimum},
        'internal'                 => $args{internal},
        'propertyBitSet'           => $args{propertyBitSet},
        'hideseparator'            => $args{hideseparator}
    };

    return bless $self, $pkg
}

=head2 $self->getId

Returns Rule's id (always >= 0)

=cut

sub getId {
    my ($self) = @_;

    return $self->{id}
}

=head2 $self->getDescription

Returns Rule's description (auto-generated if there is not "name" keyword in the grammar)

=cut

sub getDescription {
    my ($self) = @_;

    return $self->{description}
}

=head2 $self->getShow

Returns Rule's show

=cut

sub getShow {
    my ($self) = @_;

    return $self->{show}
}

=head2 $self->getLhsId

Returns Rule's LHS symbol id (always >= 0)

=cut

sub getLhsId {
    my ($self) = @_;

    return $self->{lhsId}
}

=head2 $self->getSeparatorId

Returns Rule's separator symbol id (< 0 if the rule is not a sequence)

=cut

sub getSeparatorId {
    my ($self) = @_;

    return $self->{separatorId}
}

=head2 $self->getRhsIds

Returns Rule's RHS ids (none for a null rule)

=cut

sub getRhsIds {
    my ($self) = @_;

    return $self->{rhsIds}
}

=head2 $self->getExceptionId

Returns Rule's exception id (< 0 if there is no exception)

=cut

sub getExceptionId {
    my ($self) = @_;

    return $self->{exceptionId}
}

=head2 $self->getAction

Returns Rule's action (null if none)

=cut

sub getAction {
    my ($self) = @_;

    return $self->{action}
}

=head2 $self->getDiscardEvent

Returns Rule's discard event name (only when LHS is ":discard" and "event" keyword is present)

=cut

sub getDiscardEvent {
    my ($self) = @_;

    return $self->{discardEvent}
}

=head2 $self->isDiscardEventInitialState

Returns Rule's discard initial state is on ?

=cut

sub isDiscardEventInitialState {
    my ($self) = @_;

    return $self->{discardEventInitialState}
}

=head2 $self->getDiscardEventInitialState

Returns Rule's discard initial state is on ?

Alias to isDiscardEventInitialState()

=cut

sub getDiscardEventInitialState {
    goto &isDiscardEventInitialState
}

=head2 $self->getRank

Returns Rule's rank (defaults to 0)

=cut

sub getRank {
    my ($self) = @_;

    return $self->{rank}
}

=head2 $self->isNullRanksHigh

Returns Rule rank high when it is a nullable ?

=cut

sub isNullRanksHigh {
    my ($self) = @_;

    return $self->{nullRanksHigh}
}

=head2 $self->getNullRanksHigh

Returns Rule rank high when it is a nullable ?

Alias to isNullRanksHigh()

=cut

sub getNullRanksHigh {
    goto &isNullRanksHigh
}

=head2 $self->isSequence

Returns Rule is a sequence ?

=cut

sub isSequence {
    my ($self) = @_;

    return $self->{sequence}
}

=head2 $self->getSequence

Returns Rule is a sequence ?

Alias to isSequence()

=cut

sub getSequence {
    goto &isSequence
}

=head2 $self->isProper

Returns Rule's separation is proper ? (meaningful only when it is sequence)

=cut

sub isProper {
    my ($self) = @_;

    return $self->{proper}
}

=head2 $self->getProper

Returns Rule's separation is proper ? (meaningful only when it is sequence)

Alias to isProper()

=cut

sub getProper {
    goto &isProper
}

=head2 $self->getMinimum

Returns Rule's minimum number of RHS (meaningful only when rule is a sequence)

=cut

sub getMinimum {
    my ($self) = @_;

    return $self->{minimum}
}

=head2 $self->isInternal

Returns Rule is internal ? (possible only when there is the loosen operator "||")

=cut

sub isInternal {
    my ($self) = @_;

    return $self->{internal}
}

=head2 $self->getInternal

Returns Rule is internal ? (possible only when there is the loosen operator "||")

Alias to isInternal()

=cut

sub getInternal {
    goto &isInternal
}

=head2 $self->getPropertyBitSet

Returns Rule's low-level property bits (combination of MarpaX::ESLIF::Grammar::Rule::PropertyBitSet values)

=cut

sub getPropertyBitSet {
    my ($self) = @_;

    return $self->{propertyBitSet}
}

=head2 $self->isHideseparator

Returns Hide separator in action callback ? (meaningful only when rule is a sequence)

=cut

sub isHideseparator {
    my ($self) = @_;

    return $self->{hideseparator}
}

=head2 $self->getHideseparator

Returns Hide separator in action callback ? (meaningful only when rule is a sequence)

Alias to isHideseparator()

=cut

sub getHideseparator {
    goto &isHideseparator
}

1;
