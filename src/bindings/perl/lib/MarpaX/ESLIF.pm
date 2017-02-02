use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Scalar::Util qw/blessed/;
use Params::Validate qw/validate_pos/;
use Class::Tiny;
use Role::Tiny;
use Carp qw/croak/;

# ABSTRACT: Marpa Extended Scanless Interface

# AUTHORITY

# VERSION

=head1 DESCRIPTION

=cut

our %_validate = (
    'class'   => { callbacks => { 'is a class'  => \&_isClass } },
    'logger'  => { callbacks => { 'is a logger' => \&_isLogger } },
    'undef'   => { callbacks => { 'is undef'    => \&_isUndef } },
    'defined' => { callbacks => { 'is defined'  => \&_isDefined } },
    );

sub _isClass {
    return _isDefined($_[0]) && ! blessed($_[0])
}

sub _isLogger {
    return _isDefined($_[0]) && Role::Tiny::does_role($_[0], 'MarpaX::ESLIF::Role::Logger')
}

sub _isUndef {
    return ! _isDefined($_[0])
}

sub _isDefined {
    return defined($_[0])
}

sub new {
    #
    # Extend if necessary - needed for validate_pos()
    #
    $#_ = 1;
    my ($class, $logger) = validate_pos(@_,
					$_validate{class},
					{ callbacks => { 'is undef or a logger' => sub { _isUndef($_[0]) || _isLogger($_[0]) }}} );

    return bless { logger => $logger }, $class;
}

1;
