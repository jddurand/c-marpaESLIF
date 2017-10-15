use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Grammar::Symbol::Properties;

# ABSTRACT: ESLIF Grammar Symbol Properties

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIF Grammar Symbol Properties.

Calls to grammar's currentSymbolProperties() or symbolPropertiesByLevel() methods outputs an instance of this class.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new();
  my $data = do { local $/; <DATA> };
  my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $data);
  my $symbolProperties = $eslifGrammar->symbolProperties(0);
  my $symbolPropertiesByLevel = $eslifGrammar->symbolPropertiesByLevel(0, 0);

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
        'type'                       => $args{type},
        'start'                      => $args{start},
        'discard'                    => $args{discard},
        'discardRhs'                 => $args{discardRhs},
        'lhs'                        => $args{lhs},
        'top'                        => $args{top},
        'id'                         => $args{id},
        'description'                => $args{description},
        'eventBefore'                => $args{eventBefore},
        'eventBeforeInitialState'    => $args{eventBeforeInitialState},
        'eventAfter'                 => $args{eventAfter},
        'eventAfterInitialState'     => $args{eventAfterInitialState},
        'eventPredicted'             => $args{eventPredicted},
        'eventPredictedInitialState' => $args{eventPredictedInitialState},
        'eventNulled'                => $args{eventNulled},
        'eventNulledInitialState'    => $args{eventNulledInitialState},
        'eventCompleted'             => $args{eventCompleted},
        'eventCompletedInitialState' => $args{eventCompletedInitialState},
        'discardEvent'               => $args{discardEvent},
        'discardEventInitialState'   => $args{discardEventInitialState},
        'lookupResolvedLeveli'       => $args{lookupResolvedLeveli},
        'priority'                   => $args{priority},
        'nullableAction'             => $args{nullableAction},
        'propertyBitSet'             => $args{propertyBitSet}
    };

    return bless $self, $pkg
}

=head2 $self->getType

Returns the type, c.f. L<MarpaX::ESLIF::Symbol::Type>

=cut

sub getType {
    my ($self) = @_;

    return $self->{type}
}

=head2 $self->isStart

Returns if this is the start symbol

=cut

sub isStart {
    my ($self) = @_;

    return $self->{start}
}

=head2 $self->getStart

Returns if this is the start symbol

Alias to isStart()

=cut

sub getStart {
    goto &isStart
}

=head2 $self->isDiscard

Returns if this is the discard symbol

=cut

sub isDiscard {
    my ($self) = @_;

    return $self->{discard}
}

=head2 $self->getDiscard

Returns if this is the discard symbol

Alias to isDiscard()

=cut

sub getDiscard {
    goto &isDiscard
}

=head2 $self->isDiscardRhs

Returns if this is a RHS of a discard rule

=cut

sub isDiscardRhs {
    my ($self) = @_;

    return $self->{discardRhs}
}

=head2 $self->isDiscardRhs

Returns if this is a RHS of a discard rule

Alias to isDiscardRhs()

=cut

sub getDiscardRhs {
    goto &isDiscardRhs
}

=head2 $self->isLhs

Returns if this is an LHS

=cut

sub isLhs {
    my ($self) = @_;

    return $self->{lhs}
}

=head2 $self->getLhs

Returns if this is an LHS

Alias to isLhs()

=cut

sub getLhs {
    goto &isLhs
}

=head2 $self->isTop

Returns if this is the first symbol of the grammar

=cut

sub isTop {
    my ($self) = @_;

    return $self->{top}
}

=head2 $self->getTop

Returns if this is the first symbol of the grammar

Alias to isTop()

=cut

sub getTop {
    goto &isTop
}

=head2 $self->getId

Returns the id

=cut

sub getId {
    my ($self) = @_;

    return $self->{id}
}

=head2 $self->getDescription

Returns the description

=cut

sub getDescription {
    my ($self) = @_;

    return $self->{description}
}

=head2 $self->getEventBefore

Returns the event before name, null if there is none

=cut

sub getEventBefore {
    my ($self) = @_;

    return $self->{eventBefore}
}

=head2 $self->isEventBeforeInitialState

Returns if the event before initial state is on, meaningless if there is no event before

=cut

sub isEventBeforeInitialState {
    my ($self) = @_;

    return $self->{eventBeforeInitialState}
}

=head2 $self->getEventBeforeInitialState

Returns if the event before initial state is on, meaningless if there is no event before

Alias to isEventBeforeInitialState()

=cut

sub getEventBeforeInitialState {
    goto &isEventBeforeInitialState
}

=head2 $self->getEventAfter

Returns the event after name, null if there is none

=cut

sub getEventAfter {
    my ($self) = @_;

    return $self->{eventAfter}
}

=head2 $self->isEventAfterInitialState

Returns if the event after initial state is on, meaningless if there is no event after

=cut

sub isEventAfterInitialState {
    my ($self) = @_;

    return $self->{eventAfterInitialState}
}

=head2 $self->getEventAfterInitialState

Returns if the event after initial state is on, meaningless if there is no event after

Alias to isEventAfterInitialState()

=cut

sub getEventAfterInitialState {
    goto &isEventAfterInitialState
}

=head2 $self->getEventPredicted

Returns the event predicted name, null if there is none

=cut

sub getEventPredicted {
    my ($self) = @_;

    return $self->{eventPredicted}
}

=head2 $self->isEventPredictedInitialState

Returns if the event predicted initial state is on, meaningless if there is no prediction event

=cut

sub isEventPredictedInitialState {
    my ($self) = @_;

    return $self->{eventPredictedInitialState}
}

=head2 $self->getEventPredictedInitialState

Returns if the event predicted initial state is on, meaningless if there is no prediction event

Alias to isEventPredictedInitialState()

=cut

sub getEventPredictedInitialState {
    goto &isEventPredictedInitialState
}

=head2 $self->getEventNulled

Returns the null event name, null if there is none - used internally for ":discard[on]" and ":discard[off]" in particular

=cut

sub getEventNulled {
    my ($self) = @_;

    return $self->{eventNulled}
}

=head2 $self->isEventNulledInitialState

Returns the nulled event initial state, meaningless if there is nulled event

=cut

sub isEventNulledInitialState {
    my ($self) = @_;

    return $self->{eventNulledInitialState}
}

=head2 $self->getEventNulledInitialState

Returns the nulled event initial state, meaningless if there is nulled event

Alias isEventNulledInitialState()

=cut

sub getEventNulledInitialState {
    goto &isEventNulledInitialState
}

=head2 $self->getEventCompleted

Returns the completion event name, null if there is none

=cut

sub getEventCompleted {
    my ($self) = @_;

    return $self->{eventCompleted}
}

=head2 $self->isEventCompletedInitialState

Returns the completion event initial state, meaningless if there is no completion event

=cut

sub isEventCompletedInitialState {
    my ($self) = @_;

    return $self->{eventCompletedInitialState}
}

=head2 $self->getEventCompletedInitialState

Returns the completion event initial state, meaningless if there is no completion event

Alias to isEventCompletedInitialState()

=cut

sub getEventCompletedInitialState {
    goto &isEventCompletedInitialState
}

=head2 $self->getDiscardEvent

Returns the discard event, null if there is none

=cut

sub getDiscardEvent {
    my ($self) = @_;

    return $self->{discardEvent}
}

=head2 $self->isDiscardEventInitialState

Returns the discard event initial state, meaningless if there is no discard event

=cut

sub isDiscardEventInitialState {
    my ($self) = @_;

    return $self->{discardEventInitialState}
}

=head2 $self->isDiscardEventInitialState

Returns the discard event initial state, meaningless if there is no discard event

Alias to isDiscardEventInitialState()

=cut

sub getDiscardEventInitialState {
    goto &isDiscardEventInitialState
}

=head2 $self->getLookupResolvedLeveli

Returns the grammar level to which it is resolved, can be different to the grammar used when this is a lexeme

=cut

sub getLookupResolvedLeveli {
    my ($self) = @_;

    return $self->{lookupResolvedLeveli}
}

=head2 $self->getPriority

Returns the symbol priority

=cut

sub getPriority {
    my ($self) = @_;

    return $self->{priority}
}

=head2 $self->getNullableAction

Returns the nullable action, null if there is none

=cut

sub getNullableAction {
    my ($self) = @_;

    return $self->{nullableAction}
}

=head2 $self->getPropertyBitSet

Returns the low-level properties (combination of MarpaX::ESLIF::Symbol::PropertyBitSet values)

=cut

sub getPropertyBitSet {
    my ($self) = @_;

    return $self->{propertyBitSet}
}

=head1 SEE ALSO

L<MarpaX::ESLIF::Symbol::Type>

1;
