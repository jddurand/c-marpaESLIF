use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Event;
use Moo;
use MarpaX::ESLIF::Event::Type;
use Types::Standard qw/Str/;
use vars qw/$VERSION/;

# ABSTRACT: ESLIFEvent is describing an event coming out from the grammar.

# AUTHORITY

# VERSION

=head1 DESCRIPTION

ESLIFEvent is describing an event coming out from the grammar.

=cut

has 'type'   => (is => 'ro', isa => 'MarpaX::ESLIF::Event::Type');
has 'symbol' => (is => 'ro', isa => Str);
has 'event'  => (is => 'ro', isa => Str);

1;
