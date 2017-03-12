use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Moo;
use MooX::Role::Logger;
require XSLoader;
use vars qw/$VERSION/;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

BEGIN {

    # VERSION

    $VERSION //= '1.0.43';
    XSLoader::load __PACKAGE__, $VERSION;
}

=head1 DESCRIPTION

ESLIF is an extension of perl's Marpa::R2 BNF, written as a stand-alone marpaESLIF library.

Please note that the call to the free() method is required to dispose the resources allocated by constructors.

Example:

 my $eslif;
 try {
   eslif = ESLIF->new();
   # Your work
 } catch {
   # Exception in $_
  // ...
 }
 
This class and its derivatives is thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough, unless you want different logging interfaces.

=cut

sub BUILD {
    my ($self, $args) = @_;

    $self->_set_marpaESLIFp(MarpaX::ESLIF::Thin::new($self));
}

sub DEMOLISH {
    my ($self, $in_global_destruction) = @_;

    MarpaX::ESLIF::Thin::free($self);
}

#
# Internal methods and attributes, the XS know they exist
#

has '_genericLoggerp' => (is => 'rwp', reader => '_get_genericLoggerp', writer => '_set_genericLoggerp', default => sub { return undef });
has '_marpaESLIFp'    => (is => 'rwp', reader => '_get_marpaESLIFp',    writer => '_set_marpaESLIFp',    default => sub { return undef });

#
# Internal tracing is always in the form $self->xxx($message)
# so no need to unstack, we cal use $_[0] and $_[1] directly
#
sub _trace     { $_[0]->_logger->trace    ($_[1]) }
sub _debug     { $_[0]->_logger->debug    ($_[1]) }
sub _info      { $_[0]->_logger->info     ($_[1]) }
sub _notice    { $_[0]->_logger->notice   ($_[1]) }
sub _error     { $_[0]->_logger->error    ($_[1]) }
sub _critical  { $_[0]->_logger->critical ($_[1]) }
sub _alert     { $_[0]->_logger->alert    ($_[1]) }
sub _emergency { $_[0]->_logger->emergency($_[1]) }

with 'MooX::Role::Logger';

1;
