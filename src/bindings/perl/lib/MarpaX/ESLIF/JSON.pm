use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::JSON;
use parent qw/MarpaX::ESLIF:Grammar/;
use MarpaX::ESLIF::JSON::ValueInterface;

# ABSTRACT: ESLIF's JSON interface

# AUTHORITY

use vars qw/$VERSION/;

=head1 DESCRIPTION

This is JSON's strict and relax encoder/decoder writen directly in L<MarpaX::ESLIF> library.

There are two JSON modes:

=over

=item Strict

Encoder and decoder are strict, as per L<ECMA-404 The JSON Data Interchange Standard|https://www.json.org>.

=item Relax

This is strict grammar extended with:

=over

=item Unlimited commas

=item Trailing separator

=item Perl style comment

=item C++ style comment

=item Infinity

=item NaN

=item Unicode's control characters (range C<[\x00-\x1F]>).

=back

=back

=cut

=head1 METHODS

=head2 MarpaX::ESLIF::JSON->new($eslif[, $strict])

   my $eslifJSON = MarpaX::ESLIF::JSON->new($eslif);

Returns a JSON grammar instance, noted C<$eslifJSON> later. Parameters are:

=over

=item C<$eslif>

MarpaX::ESLIF object instance. Required.

=item C<$strict>

A true value means strict JSON, else relax JSON. Default is a false value.

=back

=cut

#
# Tiny wrapper on MarpaX::ESLIF::JSON->new, that is using the instance as void *.
# Could have been writen in the XS itself, but I feel it is more comprehensible like
# this.
#
sub new {
    my $class = shift;
    my $eslif = shift;
    my $strict = shift // 0;

    my $self = $class->_new($eslif->_getInstance, $strict);
    return $self
}

=head2 $eslifJSON->encode($value)

   my $string = $eslifJSON->encode($value);

Returns a string containing encoded JSON data. In relax mode, special floating point values can appear:

=over

=item C<+Infinity>

Positive infinity.

=item C<-Infinity>

Negative infinity.

=item C<NaN>

Not-a-Number.

=back

Otherwise the output remains conform to the strict JSON grammar: an UTF-8 string that is using the UTF-16 surrogate's C<\u> notation for characters outside of the BMP.

=cut

sub encode {
    my ($self, $value) = @_;

    my $valueInterface = MarpaX::ESLIF::JSON::ValueInterface->new();

    $self->_encode($value, $valueInterface);

    return $valueInterface->getResult()
}

=head1 NOTES

Formally, the JSON implementation is only a grammar coded directly in the ESLIF library, therefore this module inherits from L<MarpaX::ESLIF::Grammar>.

=cut

1;
