use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

use vars qw/$VERSION/;

BEGIN {
    # VERSION

    require XSLoader;
    XSLoader::load(__PACKAGE__, $VERSION);
}

=head1 DESCRIPTION

ESLIF is derived from perl's L<Marpa::R2>, and has its own BNF, documented in L<MarpaX::ESLIF::BNF>.

=head1 SYNOPSIS

  use MarpaX::ESLIF;

  my $eslif = MarpaX::ESLIF->new();
  printf "ESLIF library version: %s\n", $eslif->version;

With a logger, using Log::Any::Adapter::Stderr as an example:

  use MarpaX::ESLIF;
  use Log::Any qw/$log/;
  use Log::Any::Adapter ('Stderr', log_level => 'trace' );

  my $eslif = MarpaX::ESLIF->new($log);
  printf "ESLIF library version: %s\n", $eslif->version;

This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough, unless you want different logging interfaces. Once a MarpaX::ESLIF instance is created, the user should create a L<MarpaX::ESLIF::Grammar> instance to have a working grammar.

=head1 METHODS

=head2 MarpaX::ESLIF->new($loggerInterface)

  my $loggerInterface = My::Logger::Interface->new();
  my $eslif = MarpaX::ESLIF->new();

Returns an instance of MarpaX::ESLIF, noted C<$eslif> below.

C<$loggerInterface> is an optional parameter that, when set, must be an object instance that can do the methods documented in L<MarpaX::ESLIF::Logger::Interface>.

An example of logging implementation can be a L<Log::Any> adapter.

=head2 $eslif->version()

  printf "ESLIF library version: %s\n", $eslif->version;

Returns a string containing the current underlying ESLIF library version.

=head1 SEE ALSO

L<MarpaX::ESLIF::Logger::Interface>, L<MarpaX::ESLIF::BNF>, L<MarpaX::ESLIF::Grammar>

=cut

1;
