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

=head2 MarpaX::ESLIF::Grammar->new($eslif, $grammar, $encoding)

   my $eslifGrammar = MarpaX::ESLIF::Grammar->new($eslif, $data);

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

=head2 $self->ngrammar()

  printf "Maximum grammar indice: %d\n", $self->ngrammar + 1;

Return the number of sub-grammars, represented as the latest grammar indice plus 1.

A grammar can have multiple I<sub-grammars>, identified by a level. Internally this is a I<sparse> array of grammars, and it is legal that a level is not defined.

=head2 $self->currentLevel()

  printf "Current indice: %d\n", $self->currentLevel;

Return the current level, which is always the first defined level of the grammar.

=head2 $self->currentDescription()

  printf "Current description: %s\n", $self->currentDescription;

Return the description of the current level, with the same encoding as found in the grammar.

=head2 $self->descriptionByLevel($level)

  printf "Level 0 description: %s\n", $self->descriptionByLevel(0);

Return the description of the grammar at indice C<$level>, with the same encoding as found in the grammar.

=head2 $self->currentRuleIds

  printf "Current Rule Ids: %s\n", join(' ', @{$self->currentRuleIds});

Return the list of rule identifiers of the current grammar, as a reference to an array of integers.

I<Rule identifiers are integers that uniquely identify a rule>.

=head2 $self->ruleIdsByLevel($level)

  printf "Level 0 Rule Ids: %s\n", join(' ', @{$self->ruleIdsByLevel(0)});

Return the list of rule identifiers at indice C<$level>, as a reference to an array of integers.

=head2 $self->ruleDisplay($ruleId)

  printf "Name of first rule: %s\n", $self->ruleDisplay($self->currentRuleIds->[0]);

Return the name of a rule identified by its rule ID C<$ruleId>.

=head2 $self->ruleShow($ruleId)

  printf "Description of first rule: %s\n", $self->ruleShow($self->currentRuleIds->[0]);

Return the description of a rule identified by its rule ID C<$ruleId>.

=head2 $self->ruleDisplayByLevel($level, $ruleId)

  printf "Name of first rule of level 0: %s\n", $self->ruleDisplayByLevel(0, $self->ruleIdsByLevel(0)->[0]);

Return the name of a rule at a specificed indice C<$level> identified by its rule ID C<$ruleId>.

=head2 $self->ruleShowByLevel($level, $ruleId)

  printf "Description of first rule of level 0: %s\n", $self->ruleShowByLevel(0, $self->ruleIdsByLevel(0)->[0]);

Return the description of a rule at a specified indice C<$level>, identified by its rule ID C<$ruleId>.

=head2 $self->show()

  printf "Description of current grammar: %s\n", $self->show();

Return the description of current grammar.

=head2 $self->showByLevel($level)

  printf "Description of first grammar: %s\n", $self->showByLevel(0);

Return the description of the grammar at indice C<$level>.

=head2 $self->parse($recognizerInterface, $valueInterface)

  my $recognizerInterface = My::Recognizer::Interface->new();
  my $valueInterface      = My::Value::Interface->new();
  printf "Parse result: %s\n", $self->parse($recognizerInterface, $valueInterface);

Short version of input validation and valuation, that will never give back control to the user until the end or a failure. No event is possible when using this method. If this method returns true, then it is guaranteed that the result is in C<$valueInterface->getResult()>.
C<$recognizerInterface> and C<$valueInterface> must be object instances that provide the following methods:

=over

=item Recognizer Interface

=over

=item $recognizerInterface->read()

Performs read of user data and returns a true value on success, a false value otherwise. C<$recognizerInterface> is responsible to maintain the status in terms of: data content, data type (binary or character), eof flag, eventual encoding information, that are queried using the following methods:

=item $recognizerInterface->isEof()

Returns a boolean value indicating the end of the stream.

=item $recognizerInterface->isCharacterStream()

Returns a boolean value indicating if current chunk is a character stream or not.

=item $recognizerInterface->encoding()

Encoding of latest chunk of data, when the later is a character chunk. It is legal to return C<undef>.

If current chunk of data is a character stream, and this method returns C<undef>, then marpaESLIF will either:

=over

=item guess the encoding if there previous chunk of data was not a character stream,

=item continue with previous encoding if previous chunk of data was a character stream

=back

=item $recognizerInterface->data()

Returns data content of current chunk, may be of zero size.

=item $recognizerInterface->isWithDisableThreshold()

Returns a boolean indicating if exhaustion should trigger an exhaustion event.

When the parse is exhausted, the normal behavior is to exit with an error if the eof flag is not set. This method is saying that an exhaustion event should be raised instead, and is used at recognizer creation step only.

=item $recognizerInterface->isWithNewline()

Returns a boolean indicating if line/number accounting is on.

Error reporting can be accurate up to line and column numbers when this is happening on a character stream enabled chunk of data. This is handy, but has an extra cost on parsing performance. This method is used at recognizer creation step only.

=back

=item C<$valueInterface>

=over

=item $valueInterface->isWithHighRankOnly()

Returns the rank order preference.

When the interface returns true, only the choices with the highest rank are kept. This method is used at valuation instance creation step only.

=item $valueInterface->isWithOrderByRank()

Returns the rank order preference.

Orders the parse tree values by their rank value. This method is used at valuation instance creation step only.

=item $valueInterface->isWithAmbiguous()

Returns the ambiguity acceptance.

Accept ambiguous parse tree. This method is used at valuation instance creation step only.

=item $valueInterface->isWithNull()

Returns the null acceptance.

Accept a null parse tree value (e.g. when the start rule is not complete). This method is used at valuation instance creation step only.

=item $valueInterface->maxParses()

Returns the maximum number of wanted parses, 0 for all parse trees.

A very ambiguous parsing can provide a lot of parse tree values, it is possible to reduce such number, at the cost of not having all the valuation possibilities. This method is used at valuation instance creation step only.

=item $valueInterface->setResult($result)

Store the parsing result.

When valuation is successful, marpaESLIF is using this callback.

=item $valueInterface->getResult()

Returns the parsing result.

marpaESLIF is not using this method per-se, but it is nevertheless required to ensure the interface coherency v.s. the C<getResult()> required method.

=back

=back

=cut

1;
