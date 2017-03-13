use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF::Exception;
use Moo;

with 'Throwable';

around BUILDARGS => sub {
    my ($orig, $class, @args) = @_;

    return { message => $args[0] } if $#args == 0 && ! ref($args[0]);

    return $class->$orig(@args);
};

has message => (is => 'ro');

1;
