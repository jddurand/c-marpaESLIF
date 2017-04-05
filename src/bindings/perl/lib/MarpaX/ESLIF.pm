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

ESLIF is an extension of perl's L<Marpa::R2> BNF, written as a stand-alone L<marpaESLIF|http://github.com/jddurand/c-marpaESLIF> library.

=head1 SYNOPSIS

  my $eslif = MarpaX::ESLIF->new();
  printf "ESLIF library version: %s\n", $eslif->version;

This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough, unless you want different logging interfaces.

=head1 METHODS

=head2 C<MarpaX::ESLIF->new()>

  my $eslif = MarpaX::ESLIF->new();

Returns an instance of MarpaX::ESLIF, noted C<$self> below.

=head2 C<$self->version()>

  printf "ESLIF library version: %s\n", $eslif->version;

Returns a string containing the current underlying ESLIF library version.

=head1 SEE ALSO

L<c-marpaESLIF|https://github.com/jddurand/c-marpaESLIF>

=cut

1;
