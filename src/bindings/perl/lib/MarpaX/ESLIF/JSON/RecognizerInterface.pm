use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::JSON::RecognizerInterface;

# ABSTRACT: MarpaX::ESLIF::JSON Recognizer Interface

# VERSION

# AUTHORITY

=head1 DESCRIPTION

MarpaX::ESLIF::JSON's Recognizer Interface

=head1 SYNOPSIS

    use MarpaX::ESLIF::JSON::RecognizerInterface;

    my $recognizerInterface = MarpaX::ESLIF::JSON::RecognizerInterface->new();

=cut

=head1 SUBROUTINES/METHODS

=cut

# ============================================================================
# new
# ============================================================================

=head2 new($class, %options)

Instantiate a new recognizer interface object. C<%options> should contain at least:

=over

=item input

The input to parse

=back

C<%options> may contain:

=over

=item input

The input

=item encoding

The encoding of the data

=item definitions

A reference to a hash containing known definitions, where values must be a MarpaX::ESLIF boolean.

=back

=cut

sub new {
    my ($pkg, %options) = @_;

    my $input = delete($options{input}) // '';

    return bless
        (
         {
             input => $input,
             %options
         },
         $pkg)
}

=head2 Required methods

=cut

# ============================================================================
# read
# ============================================================================

=head3 read($self)

Returns a true or a false value, indicating if last read was successful.

=cut

sub read {
    return 1 # First read callback will be ok
}

# ============================================================================
# isEof
# ============================================================================

=head3 isEof($self)

Returns a true or a false value, indicating if end-of-data is reached.

=cut

sub isEof {
    return 1 # ../. and we will say this is EOF
}

# ============================================================================
# isCharacterStream
# ============================================================================

=head3 isCharacterStream($self)

Returns a true or a false value, indicating if last read is a stream of characters.

=cut

sub isCharacterStream {
    return 1 # MarpaX::ESLIF will validate the input
}

# ============================================================================
# encoding
# ============================================================================

=head3 encoding($self)

Returns encoding information.

=cut

sub encoding {
    return $_[0]->{encoding} # Let MarpaX::ESLIF guess eventually - undef is ok
}

# ============================================================================
# data
# ============================================================================

=head3 data($self)

Returns last bunch of data. Default is the string passed in the constructor.

=cut

sub data {
    return $_[0]->{input} // croak 'Undefined input' # Data itself
}

# ============================================================================
# isWithDisableThreshold
# ============================================================================

=head3 isWithDisableThreshold($self)

Returns a true or a false value, indicating if threshold warning is on or off, respectively.

=cut

sub isWithDisableThreshold {
    return 0
}

# ============================================================================
# isWithExhaustion
# ============================================================================

=head3 isWithExhaustion($self)

Returns a true or a false value, indicating if exhaustion event is on or off, respectively.

=cut

sub isWithExhaustion {
    return $_[0]->{exhaustion} // 0
}

# ============================================================================
# isWithNewline
# ============================================================================

=head3 isWithNewline($self)

Returns a true or a false value, indicating if newline count is on or off, respectively.

=cut

sub isWithNewline {
    return 1
}

# ============================================================================
# isWithTrack
# ============================================================================

=head3 isWithTrack($self)

Returns a true or a false value, indicating if absolute position tracking is on or off, respectively.

=cut

sub isWithTrack {
    return 0
}

1;
