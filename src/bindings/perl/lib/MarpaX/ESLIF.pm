use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

=head1 DESCRIPTION

ESLIF is an extension of perl's L<Marpa::R2> BNF, written as a stand-alone L<marpaESLIF|http://github.com/jddurand/c-marpaESLIF> library.

=head1 SYNOPSIS

  my $eslif = MarpaX::ESLIF->new();
  printf "ESLIF library version: %s\n", $eslif->version;

This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough, unless you want different logging interfaces.

=cut

use vars qw/$VERSION/;

BEGIN {
    # VERSION

    require XSLoader;
    XSLoader::load(__PACKAGE__, $VERSION);
}

1;
