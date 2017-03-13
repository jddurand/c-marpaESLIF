use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Moo;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

=head1 DESCRIPTION

ESLIF is an extension of perl's L<Marpa::R2> BNF, written as a stand-alone L<marpaESLIF|http://github.com/jddurand/c-marpaESLIF> library.

=head1 SYNOPSIS

  use Try::Tiny;

  my $eslif;
  try {
    $eslif = MarpaX::ESLIF->new();
    # Your work
  } catch {
    # Oups
  }

This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough, unless you want different logging interfaces.

=cut

require XSLoader;
use Carp qw/croak/;
use MarpaX::ESLIF::Exception;
use MarpaX::ESLIF::Logger::Interface;
use Scalar::Util qw/blessed/;
use Types::Standard qw/ConsumerOf/;
use vars qw/$VERSION/;

BEGIN {

    # VERSION

    $VERSION //= '1.0.43';
    XSLoader::load __PACKAGE__, $VERSION;
}

has 'loggerInterface' => (
    reader => '_getLoggerInterface',
    is => 'ro',
    isa => ConsumerOf['MarpaX::ESLIF::Logger::Interface'],
    required => 1
    );

has 'marpaESLIFp' => (
    is => 'rw',
    reader => '_getMarpaESLIFp',
    writer => '_setMarpaESLIFp',
    default => sub {
        0
    }
    );

has 'genericLoggerContextp' => (
    is => 'rw',
    reader => '_getGenericLoggerContextp',
    writer => '_setGenericLoggerContextp',
    default => sub {
        0
    }
    );

has 'genericLoggerp' => (
    is => 'rw',
    reader => '_getGenericLoggerp',
    writer => '_setGenericLoggerp',
    default => sub {
        0
    }
    );

=head1 METHODS

=head2 $package->new(loggerInterface => $loggerInterface)

C<loggerInterface> is a consumer of L<MarpaX::ESLIF::Logger::Interface> role, may be C<undef>.
Throws a L<MarpaX::ESLIF::Exception> exception in case of error.

Returns an instance of MarpaX::ESLIF.

=cut

sub BUILD {
    my ($self, $args) = @_;
    MarpaX::ESLIF::Thin::new($self,
                             $args->{loggerInterface});
}
	
sub DEMOLISH {
    my ($self, $in_global_destruction) = @_;
    MarpaX::ESLIF::Thin::free($self,
                              $self->_getMarpaESLIFp,
                              $self->_getGenericLoggerp,
                              $self->_getGenericLoggerContextp);
}


=head2 version()

Returns the version of the underlying C library.

=cut

sub version {
    return MarpaX::ESLIF::Thin::version();
}

1;
