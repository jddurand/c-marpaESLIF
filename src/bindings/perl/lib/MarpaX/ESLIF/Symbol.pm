use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Symbol;
use Carp qw/croak/;
use Safe::Isa;

# ABSTRACT: MarpaX::ESLIF's symbol

# AUTHORITY

# VERSION

=head1 DESCRIPTION

MarpaX::ESLIF::Symbol allows to create external symbols on demand

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new();

  my $stringPattern = MarpaX::ESLIF::String->new('"String Pattern"', 'UTF-8');
  my $stringSymbol = MarpaX::ESLIF::Symbol->new($eslif, 'string', $stringPattern);

  my $regexPattern = MarpaX::ESLIF::String->new('Regex.*Pattern', 'UTF-8');
  my $regexSymbol = MarpaX::ESLIF::Symbol->new($eslif, 'regex', $regexPattern);

External symbols can be of type C<string> or C<regex>. They can be used agains a L<MarpaX::ESLIF::Recognizer> or any external input.

A string pattern I<must> follow ESLIF)s BNF, i.e. start and ends with:

=over

=item C<'> (single quote) character

=item C<"> (double quote) character

=item C<“> (LEFT DOUBLE QUOTATION MARK) and C<”> characters (RIGHT DOUBLE QUOTATION MARK), respectively

=back

=head1 METHODS

=head2 MarpaX::ESLIF::Symbol->new($eslif, $type, $pattern, $modifiers)

Returns a symbol instance, noted C<$symbol> later.

=over

=item C<$type>

Must be "string" or "regex".

=item C<$pattern>

Must be a L<MarpaX::ESLIF::String> instance.

=item C<$modifiers>

Must follow the specification of the I<Terminals> section of L<MarpaX::ESLIF::BNF>:

  ----------------------------------------------------------------
  Modifiers   Explanation
  ----------------------------------------------------------------
  e           Unset back-references in the pattern will match to empty strings
  i           Case-insensitive
  j           \u, \U and \x and unset back-references will act as JavaScript standard
  m           Multi-line regex
  n           Enable Unicode properties and extend meaning of meta-characters
  s           A dot meta-character in the pattern matches all characters, including newlines
  x           Enable comments. This has some limitation due marpaESLIF semantics
  D           A dollar meta-character matches only at the end of the subject string
  J           Allow duplicate names for sub-patterns
  U           Inverts the "greediness" of the quantifiers
  a           Meta-characters will be limited to their ASCII equivalent
  u           Forces support of large codepoints
  b           Could mean "forced binary" mode
  c           Could mean "forced unicode character" mode
  A           Remove the systematic anchoring
  ----------------------------------------------------------------

Note that a string pattern accepts only the C<i> and C<c> modifiers.

=back

=cut

#
# Tiny wrapper on MarpaX::ESLIF::Symbol->new, that is using the instance as void *.
# Could have been writen in the XS itself, but I feel it is more comprehensible like
# this.
#
sub new {
    my ($class, $eslif, $type, $pattern, $modifiers)  = @_;

    croak "Pattern must be a MarpaX::ESLIF::String instance" unless $pattern->$_isa('MarpaX::ESLIF::String');

    my $self;
    $type //= '';
    $modifiers //= '';
    #
    # Make type a true string
    #
    $type = "$type";
    if ($type eq 'string') {
        $self = $class->string_new($eslif->_getInstance, $pattern->value, bytes::length($pattern->value), $pattern->encoding, $modifiers)
    } elsif ($type eq 'regex') {
        $self = $class->regex_new($eslif->_getInstance, $pattern->value, bytes::length($pattern->value), $pattern->encoding, $modifiers)
    } else {
        croak "Type must be 'string' or 'regex'"
    }

    return $self
}

=head1 SEE ALSO

L<MarpaX::ESLIF>, L<MarpaX::ESLIF::Recognizer>

=cut

1;
