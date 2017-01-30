use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
use Function::Parameters qw/:strict/;
use Class::Tiny;
use Role::Tiny;
use Carp qw/croak/;

# ABSTRACT: Marpa Extended Scanless Interface

# VERSION

=head1 DESCRIPTION

=cut

method BUILDARGS($class: %init) {
  my %rc;
  #
  # Check we have loggerInterface, eventually
  #
  my $loggerInterface = $init{loggerInterface};
  if (defined($loggerInterface)) {
    croak "Argument loggerInterface does not implement role ESLIFLoggerInterface" unless Role::Tiny::does_role($loggerInterface, 'MarpaX::ESLIFLoggerInterface')) {
  }
}

method new($class: %init) {
  return bless { %init }, $class;
}

1;
