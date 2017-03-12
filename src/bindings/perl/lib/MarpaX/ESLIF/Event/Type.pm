use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Event::Type;
use Moo;
use Carp qw/croak/;
use vars qw/$VERSION/;
use Scalar::Util qw/looks_like_number/;

# ABSTRACT: ESLIFEventType is an enumeration of all ESLIF possible events.

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIFEventType is an enumeration of all ESLIF possible events.

=cut

=head1 CONSTANTS

=head2 NONE

Should never happen -;

=cut

our $NONE = 0x00;

=head2 COMPLETED

Symbol completion event

=cut

our $COMPLETED = 0x01;

=head2 NULLED

Symbol nulling event

=cut

our $NULLED = 0x02;

=head2 PREDICTED

Symbol prediction event

=cut

our $PREDICTED = 0x04;

=head2 BEFORE

Symbol prediction event

=cut

our $BEFORE = 0x08;

=head2 AFTER

Symbol prediction event

=cut

our $AFTER = 0x10;

=head2 EXHAUSTED

Symbol prediction event

=cut

our $EXHAUSTED = 0x20;

=head2 DISCARD

Symbol Discard event

=cut

our $DISCARD = 0x40;

has code => (is => 'ro');

around BUILDARGS => sub {
    my ($orig, $class, @args) = @_;

    return { code => $args[0] } if @args == 1 && !ref $args[0];

    return $class->$orig(@args);
};

1;
