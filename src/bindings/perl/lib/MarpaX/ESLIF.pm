use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Scalar::Util;
use Params::Validate;
use Class::Tiny;
use Role::Tiny;
use Carp qw/croak/;

# ABSTRACT: Marpa Extended Scanless Interface

# VERSION

=head1 DESCRIPTION

=cut

our %_validate = (
    #
    # Every argument is passed by value in $_[0]
    #
    class  => { callbacks => {    defined($_[0])  && (! blessed($_[0])                                            ) } },
    logger => { callbacks => { (! defined($_[0])) ||    Role::Tiny::does_role($_[0], 'MarpaX::ESLIF::Role::Logger') } }
    );

sub new {
    my ($class, $logger) = validate_pos( @_, $_validate{class}, $_validate{logger});

    return bless { logger => $logger }, $class;
}

1;
