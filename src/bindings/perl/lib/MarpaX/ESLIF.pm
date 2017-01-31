use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Scalar::Util qw/blessed/;
use Params::Validate qw/validate_pos/;
use Class::Tiny;
use Role::Tiny;
use Carp qw/croak/;

# ABSTRACT: Marpa Extended Scanless Interface

# VERSION

=head1 DESCRIPTION

=cut

our %_validate = (
    'class'  => { callbacks => { 'is a class'  => \&_isClass } },
    'logger' => { callbacks => { 'is a logger' => \&_isLogger } },
    'undef'  => { callbacks => { 'is undef'    => \&_isUndef } },
    );

sub _isClass {
    return defined($_[0]) && ! blessed($_[0])
}

sub _isLogger {
    return defined($_[0]) && Role::Tiny::does_role($_[0], 'MarpaX::ESLIF::Role::Logger')
}

sub _isUndef {
    return ! defined($_[0])
}

sub new {
    #
    # Extend if necessary - necessary for validate_pos()
    #
    $_[1] //= undef;
    my ($class, $logger) = validate_pos( @_, $_validate{class}, { callbacks => { 'is undef or a logger' => sub { _isUndef($_[0]) || _isLogger($_[0]) } } } );

    return bless { logger => $logger }, $class;
}

1;
