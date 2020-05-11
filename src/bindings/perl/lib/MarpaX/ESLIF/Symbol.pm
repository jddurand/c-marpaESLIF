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

  my $stringSymbol = MarpaX::ESLIF::Symbol->new($eslif, type => 'string', pattern => '"String Pattern"');
  my $regexSymbol = MarpaX::ESLIF::Symbol->new($eslif, type => 'regex', pattern => 'Regex.*Pattern');

External symbols can be of type C<string> or C<regex>. They can be used agains a L<MarpaX::ESLIF::Recognizer> or any external input.

A string pattern I<must> follow ESLIF)s BNF, i.e. start and ends with:

=over

=item C<'> (single quote) character

=item C<"> (double quote) character

=item C<“> (LEFT DOUBLE QUOTATION MARK) and C<”> characters (RIGHT DOUBLE QUOTATION MARK), respectively

=back

=head1 METHODS

=head2 MarpaX::ESLIF::Symbol->new($eslif, %options)

Returns a symbol instance, noted C<$symbol> later. C<%options> is a hash that contains:

=over

=item C<type>

Must be "string" or "regex". Required.

=item C<pattern>

The pattern content. Required.

=item C<encoding>

The pattern encoding. Optional.

=item C<modifiers>

A string containing modifiers. Optional.

It must follow the specification of the I<Terminals> section of L<MarpaX::ESLIF::BNF>:

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
    my ($class, $eslif, %options)  = @_;

    my $type = $options{type} // croak 'Type must be defined';
    $type = "$type"; # Make type a true string

    my $pattern = $options{pattern} // croak 'Pattern must be defined';
    $pattern = "$pattern"; # Make pattern a true string

    return
        ($type eq 'string')
        ?
        $class->string_new($eslif->_getInstance, $pattern, bytes::length($pattern), $options{encoding}, $options{modifiers})
        :
        (($type eq 'regex')
         ?
         $class->regex_new($eslif->_getInstance, $pattern, bytes::length($pattern), $options{encoding}, $options{modifiers})
         :
         croak "Type must be 'string' or 'regex'"
        )
}

=head1 SEE ALSO

L<MarpaX::ESLIF>, L<MarpaX::ESLIF::Recognizer>

=cut

1;
