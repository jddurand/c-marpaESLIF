use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Recognizer;
use parent qw/MarpaX::ESLIF::Base/;

#
# Base required class methods
#
sub _CLONABLE { return sub { 0 } }
sub _ALLOCATE { return \&MarpaX::ESLIF::Recognizer::allocate }
sub _DISPOSE  { return \&MarpaX::ESLIF::Recognizer::dispose }
sub _EQ       { return }

# ABSTRACT: MarpaX::ESLIF's recognizer

# AUTHORITY

# VERSION

=head1 DESCRIPTION

MarpaX::ESLIF::Recognizer is a possible step after a MarpaX::ESLIF::Grammar instance is created.

=head1 SYNOPSIS

  my $eslifRecognizer = MarpaX::ESLIF::Recognizer->new($eslifGrammar, $recognizerInterface);

The recognizer interface is used to read chunks of data, that the internal recognizer will keep in its internal buffers, until it is consumed. The recognizer internal buffer may not be an exact duplicate of the external data that was read: in case of a character stream, the external data is systematically converted to UTF-8 sequence of bytes. If the user is pushing alternatives, he will have to know how many bytes this represent: native number of bytes

=head1 METHODS

=head2 MarpaX::ESLIF::Recognizer->new($eslifGrammar, $recognizerInterface)

  my $eslifRecognizer = MarpaX::ESLIF::Recognizer->new($eslifGrammar, $recognizerInterface);

Returns a recognizer instance, noted C<$eslifRecognizer> later. Parameters are:

=over

=item C<$eslifGrammar>

MarpaX::ESLIF:Grammar object instance. Required.

=item C<$recognizerInterface>

An object implementing L<MarpaX::ESLIF::Recognizer::Interface> methods. Required.

=back

=head2 $eslifRecognizer->newFrom($eslifGrammar)

  my $eslifRecognizerNewFrom = $eslifRecognizer->newFrom($eslifGrammar);

Returns a recognizer instance that is sharing the stream of C<$eslifRecognizer>, but applied to the other grammar C<$eslifGrammar>.

=cut

sub _allocate_newFrom {
    my ($self, $eslifGrammar) = @_;  

    return $self->allocate_newFrom($eslifGrammar)
}

sub newFrom {
    my $self = shift;
    {
        no warnings 'redefine';
        local *_ALLOCATE = sub { return \&_allocate_newFrom };
        #
        # Take care, this is a method that returns a new instance
        #
        return $self->new(@_)
    }
}

=head2 $eslifRecognizer->peekFrom($eslifGrammar)

  my $eslifRecognizerPeekFrom = $eslifRecognizer->peekFrom($eslifGrammar);

Returns a recognizer instance that is peeking the stream of C<$eslifRecognizer>, but applied to the other grammar C<$eslifGrammar>.

=cut

sub _allocate_peekFrom {
    my ($self, $eslifGrammar) = @_;  

    return $self->allocate_peekFrom($eslifGrammar)
}

sub peekFrom {
    my $self = shift;
    {
        no warnings 'redefine';
        local *_ALLOCATE = sub { return \&_allocate_peekFrom };
        #
        # Take care, this is a method that returns a new instance
        #
        return $self->new(@_)
    }
}

=head2 $eslifRecognizer->set_exhausted_flag($flag)

  $eslifRecognizer->set_exhausted_flag($flag);

Changes the isWithExhaustion() flag associated with the C<$eslifRecognizer> recognizer instance.

=head2 $eslifRecognizer->share($eslifRecognizerShared)

  $eslifRecognizer->share($eslifRecognizerShared);

Shares the stream of C<$eslifRecognizerShared> recognizer instance with the C<$eslifRecognizer> instance.

=head2 $eslifRecognizer->unshare()

  $eslifRecognizer->unshare();

Unshares the stream of C<$eslifRecognizer> instance. This is equivalent to:

  $eslifRecognizer->share(undef);

=head2 $eslifRecognizer->isCanContinue()

Returns a true value if recognizing can continue.

=head2 $eslifRecognizer->isExhausted()

Returns a true value if parse is exhausted, always set even if there is no exhaustion event.

=head2 $eslifRecognizer->scan($initialEvents)

Start a recognizer scanning. This call is allowed once in recognizer lifetime. If specified, C<$initialEvents> must be a scalar. Default value is 0.

This method can generate events. Initial events are those that are happening at the very first step, and can be only prediction events. This may be annoying, and most applications do not want that - but some can use this to get the control before the first data read.

Returns a boolean indicating if the call was successful or not.

=head2 $eslifRecognizer->resume($deltaLength)

This method tell the recognizer to continue. Events can be generate after resume completion.

C<$deltaLength> is optional and is a number of B<bytes> to skip forward before resume goes on, must be positive or greater than 0. In case of a character stream, user will have to compute the number of bytes as if the input was in the UTF-8 encoding. Default value is C<0>.

Returns a boolean indicating if the call was successful or not.

=head2 $eslifRecognizer->events()

When control is given back to the end-user, he can always ask what are the current events.

Returns a reference to an array of hash references, eventually empty if there is none. Each array element is a reference to a hash containing these keys:

=over

=item type

The type of event, that is one one the value listed in L<MarpaX::ESLIF::Event::Type>.

=item symbol

The name of the symbol that triggered the event. Can be C<undef> in case of exhaustion event.

=item event

The name of the event that triggered the event. Can be C<undef> in case of exhaustion event.

=back

=head2 $eslifRecognizer->eventOnOff($symbol, $eventTypes, $onOff)

Events can be switched on or off. For performance reasons, if you know that you do not need an event, it can be a good idea to switch if off. Required parameters are:

=over

=item C<$symbol>

The symbol name to which the event is associated.

=item C<$symbol>

The symbol name to which the event is associated.

=item C<$eventTypes>

A reference to an array of event types, as per L<MarpaX::ESLIF::Event::Type>.

=item C<$onOff>

A flag that set the event on or off.

=back

Note that trying to change the state of an event that was not pre-declared in the grammar is a no-op.

Returns a reference to an array of hash references, eventually empty if there is none. Each array element is a reference to a hash containing these keys:

=head2 $eslifRecognizer->lexemeAlternative($name, $anything, $grammarLength)

A lexeme is a terminal in the legacy parsing terminology. The lexeme word mean that in the grammar it is associated to a sub-grammar. Pushing an alternative mean that the end-user is intructing the recognizer that, at this precise moment of lexing, there is a given lexeme associated with the C<$name> parameter, with a given opaque value <$anything>. Grammar length parameter C<$grammarLength> is optional, and defaults to C<1>, i.e. one lexeme (which is a symbol in the grammar) correspond to one token. Nevertheless it is possible to say that an alternative span over more than one symbol.

Returns a boolean indicating if the call was successful or not.

=head2 $eslifRecognizer->lexemeComplete($length)

Say the recognizer that alternatives are complete at this precise moment of parsing, and that the recognizer must move forward by C<$length> B<bytes>, which can be zero (end-user's responsibility).
This method can generate events.

Returns a boolean indicating if the call was successful or not.

=head2 $eslifRecognizer->lexemeRead($name, $anything, $length, $grammarLength)

A short-hand version of lexemeAlternative() followed by lexemeComplete(), with the same meaning for all parameters.
This method can generate events.

Returns a boolean indicating if the call was successful or not.

=head2 $eslifRecognizer->lexemeTry($name)

The end-user can ask the recognizer if a lexeme C<$name> may match.

Returns a boolean indicating if the lexeme is recognized.

=head2 $eslifRecognizer->discardTry()

The end-user can ask the recognizer if C<:discard> rule may match.

Returns a boolean indicating if :discard is recognized.

=head2 $eslifRecognizer->lexemeExpected()

Ask the recognizer a list of expected lexemes.

Returns a reference to an array of names, eventually empty.

=head2 $eslifRecognizer->lexemeLastPause($name)

Ask the recognizer the end-user data associated to last lexeme I<pause after> event. A I<pause after> event is the when the recognizer was responsible of lexeme recognition, after a call to scan() or resume() methods. This data will be an exact copy of the last bytes that matched for a given lexeme, where data is the internal representation of end-user data, meaning that it may be UTF-8 sequence of bytes in case of character stream.

Returns the associated bytes, or C<undef>.

=head2 $eslifRecognizer->lexemeLastTry($name)

Ask the recognizer the end-user data associated to last successful lexeme try. This data will be an exact copy of the last bytes that matched for a given lexeme, where data is the internal representation of end-user data, meaning that it may be UTF-8 sequence of bytes in case of character stream.

Returns the associated bytes, or C<undef>.

=head2 $eslifRecognizer->discardLastTry()

Ask the recognizer the end-user data associated to last successful discard try. This data will be an exact copy of the last bytes that matched for a given lexeme, where data is the internal representation of end-user data, meaning that it may be UTF-8 sequence of bytes in case of character stream.

Returns the associated bytes, or C<undef>.

=head2 $eslifRecognizer->discardLast()

Ask the recognizer the end-user data associated to last successful discard. This data will be an exact copy of the last bytes that matched for the latest :discard rule, meaning that it may be UTF-8 sequence of bytes in case of character stream.

For performance reasons, last discard data is available only if the recognizer interface returned a true value for C<isWithTrack()> method I<and> if there is a discard event for the C<:discard> rule that matched.

Returns the associated bytes, or C<undef>.

=head2 $eslifRecognizer->isEof()

This method is similar to the isEof()'s recognizer interface. Except that this is asking the question directly to the recognizer's internal state, that maintains a copy of this flag.

Returns a boolean indicating of end-of-user-data is reached.

=head2 $eslifRecognizer->read()

Forces the recognizer to read more data. Usually, the recognizer interface is called automatically whenever needed.

Returns a boolean value indicating success or not.

=head2 $eslifRecognizer->input()

Get a copy of the current internal recognizer buffer, starting at the exact byte where resume() would start. An undefined output does not mean there is an error, but that internal buffers are completely consumed. ESLIF will automatically require more data unless the EOF flag is set. Internal buffer is always UTF-8 encoded to every chunk of data that was declared to be a character stream.

Returns the associated input bytes, or C<undef>.

=head2 $eslifRecognizer->progressLog($start, $end, $loggerLevel)

Asks to get a logging representation of the current parse progress. The format is fixed by the underlying libraries. The C<$start> and C<$end> parameters follow the perl convention of indices, i.e. when they are negative, start that far from the end. For example, -1 mean the last indice, -2 mean one before the last indice, etc... C<$loggerLevel> is a level as per L<MarpaX::ESLIF::Logger::Level>.

Nothing is returned.

=head2 $eslifRecognizer->progress($start, $end)

Asks to get the internal progress in terms of Earley parsing. The C<$start> and C<$end> parameters follow the perl convention of indices, i.e. when they are negative, start that far from the end. For example, -1 mean the last Earley Set Id, -2 mean one before the last Earley Set Id, etc...

Returns a reference to an array of hash references, eventually empty if there is none. Each array element is a reference to a hash containing these keys:

=over

=item earleySetId

The Earley Set Id.

=item earleySetOrigId

The origin Earley Set Id.

=item rule

The rule number.

=item position

The position in the rule, where a negative number or a number bigger than the length of the rule means the rule is completed, C<0> means the rule is predicted, else the rule is being run.

=item earleme

The Earleme Id corresponding to the Earley Set Id.

=item earlemeOrig

The origin Earleme Id corresponding to the origin Earley Set Id.

=back

=head2 $eslifRecognizer->eventOnOff($symbol, $eventTypes, $onOff)

Events can be switched on or off. For performance reasons, if you know that you do not need an event, it can be a good idea to switch if off. Required parameters are:

=over

=item C<$symbol>

The symbol name to which the event is associated.

=item C<$symbol>

The symbol name to which the event is associated.

=item C<$eventTypes>

A reference to an array of event types, as per L<MarpaX::ESLIF::Event::Type>.

=item C<$onOff>

A flag that set the event on or off.

=back

Note that trying to change the state of an event that was not pre-declared in the grammar is a no-op.

Returns a reference to an array of hash references, eventually empty if there is none. Each array element is a reference to a hash containing these keys:

=head2 $eslifRecognizer->latestEarleySetId()

Returns the latest Earley Set Id.

=head2 $eslifRecognizer->earleme($earleySetId)

Returns the earleme corresponding C<$earleySetId>.

=head2 $eslifRecognizer->lastCompletedOffset($name)

The recognizer is tentatively keeping an absolute offset every time a lexeme is complete. We say tentatively in the sense that no overflow checking is done, thus this number is not reliable in case the user data spanned over a very large number of bytes. In addition, the unit is in bytes. C<$name> can be any symbol in the grammar.

Returns the absolute offset in bytes.

=head2 $eslifRecognizer->lastCompletedLength($name)

The recognizer is tentatively computing the length of every symbol completion. Since this value depend internally on the absolute previous offset, it is not guaranteed to be exact, in the sense that no overflow check is done. C<$name> can be any symbol in the grammar.

Returns the absolute length in bytes.

=head2 $eslifRecognizer->lastCompletedLocation($name)

Returns an array containing at indices 0 and 1 the values of C<$eslifRecognizer->lastCompletedOffset($name)> and C<$eslifRecognizer->lastCompletedLength($name)>, respectively.

=head2 $eslifRecognizer->line()

If, at creation, the recognizer interface returned a true value for the C<$recognizerInterface->isWithNewline()> method, then the recognizer will track the number of lines for ever character-oriented chunk of data.

Returns the line number, or 0.

=head2 $eslifRecognizer->column()

If, at creation, the recognizer interface returned a true value for the C<$recognizerInterface->isWithNewline()> method, then the recognizer will track the number of columns for ever character-oriented chunk of data.

Returns the column number, or 0.

=head2 $eslifRecognizer->location()

Returns an array containing at indices 0 and 1 the values of C<$eslifRecognizer->line()> and C<$eslifRecognizer->column()>, respectively.

=head2 $eslifRecognizer->hookDiscard($discardOnOff)

Hook the recognizer to enable or disable the use of C<:discard> if it exists. Default mode is on. This is a I<permanent> setting.

=head2 $eslifRecognizer->hookDiscardSwitch()

Hook the recognizer to switch the use of C<:discard> if it exists. This is a I<permanent> setting.

=head2 $eslifRecognizer->symbolTry($symbol)

Tries to match the external symbol C<$symbol>, that is an instance of L<MarpaX::ESLIF::Symbol>. Return the match or C<undef>.

=head1 SEE ALSO

L<MarpaX::ESLIF::Recognizer::Interface>, L<MarpaX::ESLIF::Event::Type>, L<MarpaX::ESLIF::Logger::Level>, L<MarpaX::ESLIF::Symbol>

=head1 NOTES

L<MarpaX::ESLIF::Recognizer> cannot be reused across threads.

=cut

sub CLONE_SKIP {
    return 1
}

1;
