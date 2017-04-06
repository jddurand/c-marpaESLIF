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

ESLIF is an extension of perl's L<Marpa::R2> BNF, implemented as a stand-alone L<marpaESLIF|http://github.com/jddurand/c-marpaESLIF> C library.

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

C<$loggerInterface> is an optional parameter that, when set, must be an object instance that can do the following methods, inspired from Log4perl terminology:

=over

=item $loggerInterface->trace($message)

Logging of a string C<$message> at the I<TRACE> level.

=item $loggerInterface->debug($message)

Logging of a string C<$message> at the I<DEBUG> level.

=item $loggerInterface->info($message)

Logging of a string C<$message> at the I<INFO> level.

=item $loggerInterface->notice($message)

Logging of a string C<$message> at the I<NOTICE> level.

=item $loggerInterface->warning($message)

Logging of a string C<$message> at the I<WARNING> level.

=item $loggerInterface->error($message)

Logging of a string C<$message> at the I<ERROR> level.

=item $loggerInterface->critical($message)

Logging of a string C<$message> at the I<CRITICAL> level.

=item $loggerInterface->alert($message)

Logging of a string C<$message> at the I<ALERT> level.

=item $loggerInterface->emergency($message)

Logging of a string C<$message> at the I<EMERGENCY> level.

=back

An example of implementation can be a L<Log::Any> adapter.

=head2 $eslif->version()

  printf "ESLIF library version: %s\n", $eslif->version;

Returns a string containing the current underlying ESLIF library version.

=head1 SEE ALSO

L<MarpaX::ESLIF::Grammar>

=cut

1;
