use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Grammar;
#
# Make sure loading ESLIF::Grammar is nothing else but a load of MarpaX::ESLIF
# (everything is in the XS loaded by the later)
#
use MarpaX::ESLIF;

# ABSTRACT: ESLIF's Grammar

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIFGrammar is the second step after getting an ESLIF instance. As many grammars as wanted can be created using the same ESLIF parent.

=head1 SYNOPSIS

  my $eslif = MarpaX::ESLIF->new();
  my $data = do { local $/; <DATA> };
  my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $data);

  __DATA__
  #
  # This is an example of a calculator grammar
  #
  :start   ::= Expression
  :default ::=             action        => do_op
                           symbol-action => do_symbol
                           free-action   => do_free     # Supported but useless
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

  :lexeme ::= NUMBER pause => before event => ^NUMBER
  :lexeme ::= NUMBER pause => after  event => NUMBER$
  NUMBER     ~ /[\d]+/
  whitespaces ::= WHITESPACES
  WHITESPACES ~ [\s]+
  comment ::= /(?:(?:(?:\/\/)(?:[^\n]*)(?:\n|\z))|(?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)))/u

=head1 METHODS

=head2 C<MarpaX::ESLIF::Grammar->new($eslif, $grammar, $encoding);>

Returns a grammar instance, noted C<$self> later. Parameters are:

=over

=item C<$eslif>

MarpaX::ESLIF object instance. Required.

=item C<$grammar>

A scalar containing the grammar. Required.

=item C<$encoding>

A scalar containing the grammar encoding. Optional.

Encoding will always be guessed if not given.

=back

=head2 C<$self->ngrammar>

Return the number of sub-grammars, represented as the latest grammar indice plus 1.

A grammar can have multiple I<sub-grammars>, identified by a level. Internally this is a I<sparse> array of grammars, and it is legal that a level is not defined.

=head2 C<$self->currentLevel>

Return the current level, which is always the first defined level of the grammar.

=head2 C<$self->currentDescription>

Return the description of the current level, with the same encoding as found in the grammar.

=cut

1;
