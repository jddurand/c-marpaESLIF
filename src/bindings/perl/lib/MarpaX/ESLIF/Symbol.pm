use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Symbol;
use Carp qw/croak/;
use MarpaX::ESLIF::Registry;

my $CLONABLE = 1;

# ABSTRACT: MarpaX::ESLIF's symbol

# AUTHORITY

# VERSION

=head1 DESCRIPTION

MarpaX::ESLIF::Symbol allows to create external symbols on demand

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new();

  my $stringSymbol = MarpaX::ESLIF::Symbol->new($eslif, type => 'string', pattern => '"String Pattern"');
  #
  # In ESLIF, a regex is anchored by default
  #
  my $regexSymbol = MarpaX::ESLIF::Symbol->new($eslif, type => 'regex', pattern => 'Regex.*Pattern', modifiers => 'A');

  if (defined(my $match = $stringSymbol->try('String Pattern here'))) {
      print "==> String match: $match\n";
  }

  if (defined(my $match = $regexSymbol->try('Should match Regex etc Pattern in there'))) {
      print "==> Regex match: $match\n";
  }

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

sub _eq {
    my ($args_ref, $eslif, $type, $pattern, $encoding, $modifiers) = @_;

    my $definedEncoding = defined($encoding); # It is legal to create a symbol with no encoding
    my $definedModifiers = defined($modifiers); # It is legal to create a symbol with no modifier

    my $_definedEncoding = defined($args_ref->[4]);
    my $_definedModifiers = defined($args_ref->[5]);
    return $_ if
        $eslif == $args_ref->[1]
        &&
        $type eq $args_ref->[2]
        &&
        $pattern eq $args_ref->[3]
        &&
        ((! $definedEncoding && ! $_definedEncoding) || ($definedEncoding && $_definedEncoding && ($encoding eq $args_ref->[4])))
        &&
        ((! $definedModifiers && ! $_definedModifiers) || ($definedModifiers && $_definedModifiers && ($modifiers eq $args_ref->[4])))
}

sub _allocate {
    my ($class, $eslif, $type, $pattern, $encoding, $modifiers) = @_;

    return ($type eq 'string')
        ?
        MarpaX::ESLIF::Symbol->string_allocate($eslif, $pattern, bytes::length($pattern), $encoding, $modifiers)
        :
        (($type eq 'regex')
         ?
         MarpaX::ESLIF::Symbol->regex_allocate($eslif, $pattern, bytes::length($pattern), $encoding, $modifiers)
         :
         croak "Type must be 'string' or 'regex'"
        )
}

sub _dispose {
    my ($class) = shift;

    return MarpaX::ESLIF::Symbol::Engine->dispose(@_)
}

sub new {
    my $class = shift;
    
    return MarpaX::ESLIF::Registry::new($class, $CLONABLE, \&_eq, \&_allocate, \&_dispose, @_)
}

=head2 $symbol->try($eslif, $scalar)

Try to match the external symbol C<$symbol> on C<$scalar>, that can be anything. Return C<undef> if failure, the matched string if success.

=cut

sub try {
    my $self = shift;
    return MarpaX::ESLIF::Symbol::Engine::try($self->[0], @_)
}

=head1 SEE ALSO

L<MarpaX::ESLIF>, L<MarpaX::ESLIF::Recognizer>

=cut

sub DESTROY {
    goto &MarpaX::ESLIF::Registry::ESLIFSymbol_DESTROY
}

1;
