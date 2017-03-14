use strict;
use warnings FATAL => 'all';

package MarpaX::ESLIF;
#
# Make sure Encode is loaded
#
use Encode qw//;

# ABSTRACT: ESLIF is Extended ScanLess InterFace

# AUTHORITY

=head1 DESCRIPTION

ESLIF is an extension of perl's L<Marpa::R2> BNF, written as a stand-alone L<marpaESLIF|http://github.com/jddurand/c-marpaESLIF> library.

=head1 SYNOPSIS

  my $eslif = MarpaX::ESLIF->new();

This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough, unless you want different logging interfaces.

=cut

require XSLoader;
use vars qw/$VERSION/;

BEGIN {
    # VERSION

    XSLoader::load __PACKAGE__, $VERSION;
}

1;
