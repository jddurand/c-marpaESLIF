use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::JSON::Encoder;
use parent qw/MarpaX::ESLIF::Grammar/;

# ABSTRACT: ESLIF's JSON encoder interface

# AUTHORITY

# VERSION

=head1 DESCRIPTION

This is JSON's strict and relax encoder writen directly in L<MarpaX::ESLIF> library.

There are two JSON modes:

=over

=item Strict

Encoder is strict, as per L<ECMA-404 The JSON Data Interchange Standard|https://www.json.org>.

=item Relax

Encoder is relax, i.e.:

=over

=item Infinity

C<+Infinity> and C<-Infinity> can appear in the output.

=item NaN

C<NaN> can appear in the output.

=back

=back

=cut

=head1 METHODS

=head2 MarpaX::ESLIF::JSON::Encoder->new($eslif[, $strict])

   my $eslifJSONEncoder = MarpaX::ESLIF::JSON::Encoder->new($eslif);

Returns a JSON grammar instance, noted C<$eslifJSONEncoder> later. Parameters are:

=over

=item C<$eslif>

MarpaX::ESLIF object instance. Required.

=item C<$strict>

A true value means strict JSON, else relax JSON. Default is a true value.

=back

=cut

#
# Tiny wrapper on MarpaX::ESLIF::JSON::Encoder->new, that is using the instance as void *.
# Could have been writen in the XS itself, but I feel it is more comprehensible like
# this.
#
sub new {
    my $class = shift;
    my $eslif = shift;
    my $strict = shift // 1;

    my $self = $class->_new($eslif->_getInstance, $strict);
    return $self
}

=head2 $eslifJSONEncoder->encode($value)

   my $string = $eslifJSONEncoder->encode($value);

Returns a string containing encoded JSON data.

=cut

sub encode {
    my ($self, $value) = @_;

    return MarpaX::ESLIF::JSON::Encoder::_encode($self, $value)
}

=head1 NOTES

Formally, the JSON implementation is only a grammar coded directly in the ESLIF library, therefore this module inherits from L<MarpaX::ESLIF::Grammar>.

=cut

1;
