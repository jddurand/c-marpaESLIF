use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Scalar::Util qw/blessed/;
use Params::Validate qw/validate_pos/;
use Class::Tiny;
use Role::Tiny;
use Carp qw/croak/;
use vars qw(@ISA $VERSION $XS_VERSION);

# ABSTRACT: Marpa Extended Scanless Interface

# AUTHORITY

# VERSION

#
# In case library is not in the default path
# (this happen at least when doing make check in the package source tree)
#
BEGIN {
    $XS_VERSION = '0.001';
    eval {
	require XSLoader;
	XSLoader::load(__PACKAGE__, $XS_VERSION);
	1;
    } or do {
	require DynaLoader;
	push(@ISA, 'DynaLoader');
	if ($ENV{MARPAESLIF_LIBRARY_ABSOLUTEPATH}) {
	    #
	    push @DynaLoader::dl_library_path, $ENV{MARPAESLIF_LIBRARY_ABSOLUTEPATH};
	    my @dl_resolve_using = DynaLoader::dl_findfile(qw/-lmarpaESLIF/);
	    die "Cannot resolve marpaESLIF library after adding $ENV{MARPAESLIF_LIBRARY_ABSOLUTEPATH}" unless @dl_resolve_using;
	    my $dl_resolve_using = shift @dl_resolve_using;
	    DynaLoader::dl_load_file($dl_resolve_using, 0x01);
	}
	DynaLoader::bootstrap(__PACKAGE__, $XS_VERSION);
    }
}

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

sub version {
    #
    # This is working without a namespace
    #
    return MarpaX::ESLIF::Thin::version();
}

1;
