use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::String;
    
#
# Based on BSON::String v1.10.2
#

# ABSTRACT: ESLIF String is any string value with encoding attribute

# AUTHORITY

=head1 DESCRIPTION

MarpaX::ESLIF::String is a string type wrapper that associates an encoding information to the string value. Without this wrapper, only valid utf8 perl string will be able to transport encoding information to MarpaX::ESLIF, with a fixed encoding value to C<UTF-8> or C<Latin-1>.

=cut

use namespace::autoclean;
use Encode qw//;
use Moo;

# VERSION

=head1 METHODS

=head2 MarpaX::ESLIF::String->new($string, $encoding)

  my $string = 'value';
  my $encoding = 'UTF-8';
  my $s = MarpaX::ESLIF::String->new($string, $encoding)

C<$string> parameter defaults to the empty string, and C<$encoding> parameters defaults to C<UTF-8> if string value is a well-formed C<UTF-8> sequence of bytes, C<Latin-1> otherwise.

C<$s> overloads by default as if it was C<$string>.

=head2 $s->value

Returns the string value.

=cut

has 'value' => ( is => 'ro' );

=head2 $s->encoding

Returns the associated encoding.

=cut

has 'encoding' => ( is => 'ro' );

sub BUILDARGS {
    my ($class, $value, $encoding) = @_;

    $value //= '';
    $value = "$value"; # Make sure it is a PV

    if (! $encoding) { # neither 0, neither undef, etc..
	$encoding = Encode::is_utf8($value, 1) ? 'UTF-8' : 'Latin-1'
    } else {
	$encoding = "$encoding"; # Make sure it is a PV
    }

    return {value => $value, encoding => $encoding}
}

use overload (
    # Unary
    q{bool} => sub { !! $_[0]->{value} },
    q{""} => sub { $_[0]->{value} },
    q{0+} => sub { 0+ $_[0]->{value} },
    q{~}  => sub { ~( $_[0]->{value} ) },
    # Binary
    ( map { $_ => eval "sub { return \$_[0]->{value} $_ \$_[1] }" } qw( + * ) ), ## no critic
    (
        map {
            $_ => eval ## no critic
              "sub { return \$_[2] ? \$_[1] $_ \$_[0]->{value} : \$_[0]->{value} $_ \$_[1] }"
        } qw( - / % ** << >> x <=> cmp & | ^ )
    ),
    (
        map { $_ => eval "sub { return $_(\$_[0]->{value}) }" } ## no critic
          qw( cos sin exp log sqrt int )
    ),
    q{atan2} => sub {
        return $_[2] ? atan2( $_[1], $_[0]->{value} ) : atan2( $_[0]->{value}, $_[1] );
    },

    # Special
    fallback => 1
);

=head1 SEE ALSO

L<MarpaX::ESLIF>, L<Encode>

=cut

1;
